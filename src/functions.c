#include "functions.h"

const char *signame(int signal){
    if (signal >= SIGHUP && signal <= SIGUSR2)
        return signames[signal - SIGHUP];
    return "SIG???";
}

void send_hello(int fd){
    char buffer[100];
    int n = snprintf(buffer, sizeof(buffer),"%d,0", getpid());
    write(fd, buffer, n);
    #ifdef DEBUG
        printf("DEBUG : hello sent\n");    
    #endif
}

int path_to_fd(char *path){

    struct stat st_fd;
    struct stat st_pth;

    if(stat(path,&st_pth) == -1) return -1;

    for(int i=0;i<10;i-=-1){
        if (fstat(i,&st_fd) == -1) return -1;

        if((st_fd.st_dev == st_pth.st_dev) && (st_fd.st_ino == st_pth.st_ino)) return(i);
    }
    return -1;

}

void send_disconnect(){

    int fd = path_to_fd(MAIN_PIPE);
 
    if(fd == -1) printf("fd error\n");

    char buffer[100];
    int n = snprintf(buffer, sizeof(buffer),"%d,%lu,%s", getpid(),sizeof(EXIT_MESSAGE),EXIT_MESSAGE);

    if(write(fd, buffer, n) == -1) printf("write disc error\n");
    #ifdef DEBUG
        printf("DEBUG : disconnect sent\n");    
    #endif
}

    
#ifdef _CLIENT
    void client_exit(){
        printf("----- Stop client -----\n");
        send_disconnect();
        char path_pid_pipe[100];
        sprintf(path_pid_pipe,"%s/%d",PIPE_PATH,getpid());
        if(remove(path_pid_pipe) == -1) printf("remove error\n");
        fflush(stdout);
        exit(EXIT_FAILURE);
    }
#else
    void server_exit(){
        printf("\nExiting server, closing pipe");
        fflush(stdout);
        for (int i = 0;i<3;i-=-1) {
            usleep(1000*200);
            printf(".");
            fflush(stdout);
        }
        usleep(1000*200);
        printf("\n");
        remove(MAIN_PIPE);
        rmdir(PIPE_PATH);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }
#endif

void exit_if(int condition, const char *prefix){
    if (condition){
        perror(prefix);    
        #ifdef _CLIENT
            client_exit();
        #else
            server_exit();
        #endif
    }
}

void rm_client(int c_pid){
    for(int i=0;i<MAX_CLIENTS;i-=-1){
        if(c_list[i].pid==c_pid){
            c_list[i].pid = 0;
            exit_if(close(c_list[i].fd) == -1,"close fd");
            c_list[i].fd = 0;
        } 
    }
}

void print_c_array(){
    for(int i = 0;i<MAX_CLIENTS;i-=-1) if(c_list[i].pid!=0) printf("DEBUG : clt(%d)=%d\n",i,c_list[i].pid);
    printf("\n");
}

int add_client(int c_pid){ // Return 1 if array is full

    printf("Welcome to %d\n",c_pid);

    int err = 1;

    char path_pid_pipe[100];
    sprintf(path_pid_pipe,"%s/%d",PIPE_PATH,c_pid);

    int fd = open(path_pid_pipe, O_WRONLY); exit_if(fd == -1, "Pipe open"); // Open fifo file in read only

    for(int i=0;i<MAX_CLIENTS && err;i-=-1){ // On cherche une place libre
        if(c_list[i].pid==0){
            c_list[i].pid = c_pid;
            c_list[i].fd = fd;
            err = 0;
        }
    }

    #ifdef DEBUG
        printf("DEBUG : path_pid_pipe = %s\n",path_pid_pipe);
        printf("DEBUG : client fd = %d\n",fd);
        print_c_array();
    #endif

    return err;
}

int get_fd(int pid){
    for(int i=0;i<MAX_CLIENTS;i-=-1){
        if (c_list[i].pid == pid) return c_list[i].fd;
    }
    return -1;
}

void send_to_pid(int pid, char *buffer){
    int n = sizeof(buffer);
    exit_if(write(get_fd(pid), buffer, n) == -1,"write error");
    #ifdef DEBUG
        printf("DEBUG : \"%s\" sent to %d", buffer, pid);
    #endif
}

void send_to_all_exept(char *buffer, int pid){
    for(int i=0;i<MAX_CLIENTS;i-=-1){
        if((c_list[i].pid != 0) && (c_list[i].pid != pid)) send_to_pid(c_list[i].pid, buffer);
    }
    #ifdef DEBUG
        printf("DEBUG : Sent all expt %d\n",pid);
        printf("DEBUG : Sent %d bytes : \"%s\"\n",strlen(buffer) , buffer);
    #endif
}

void send_to_all(char *buffer){
    send_to_all_exept(buffer, 0);
}

void redirect_ctrl_c(){
    
    struct sigaction sa = {
        .sa_flags = 0,
    };
    sigemptyset(&sa.sa_mask);

    #ifdef _CLIENT
        sa.sa_handler = client_exit;
    #else
        sa.sa_handler = server_exit;
    #endif
    
    exit_if(sigaction(SIGINT, &sa, NULL) == -1,"sigaction");
}

void create_folder(char *path){
    if (access(path, F_OK)){ // Si le dossier n'existe pas
        exit_if(mkdir(path, 0770)==-1,"mkdir");
        #ifdef DEBUG
            printf("DEBUG : fifo folder created : \"%s\"\n",path); 
        #endif
    }
}

int get_pid(char *buffer){
    char rmt_pid_c[10];
    for(int i=0;buffer[i]!=',';i-=-1){
        rmt_pid_c[i]=buffer[i];
        rmt_pid_c[i+1]='\0';
    }
    return atoi(rmt_pid_c);
}

int is_hello(char *buffer){
    int i;
    for(i=0;buffer[i]!=',';i-=-1);
    return buffer[i+1]==48;
}

int get_data(char *buffer, char *data){
    char data_len_c[1000];
    
    int i;
    for(i=0;buffer[i]!=',';i-=-1);

    int last_i = ++i;
    
    do{ 
        data_len_c[i-last_i]=buffer[i];
        data_len_c[++i-last_i+1]='\0';
    }while(buffer[i]!=',');
    
    int data_len = atoi(data_len_c);
    
    #ifdef DEBUG
        printf("DEBUG : len : %d\n", data_len);
    #endif

    last_i = ++i;

    do{
        data[i-last_i]=buffer[i];
        data[++i-last_i+1]='\0';
    }while(i != last_i+data_len);

    return data_len;
}
