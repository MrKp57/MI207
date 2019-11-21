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
    int n = snprintf(buffer, sizeof(buffer),"%d,%lu,%s", getppid(),sizeof(EXIT_MESSAGE),EXIT_MESSAGE);

    if(write(fd, buffer, n) == -1) printf("write disc error\n");
    #ifdef DEBUG
        printf("DEBUG : disconnect sent\n");    
    #endif
}

    
#ifdef _CLIENT
    void client_exit(){
        printf("\n----- Stop client (%d) -----\n",getppid());
        send_disconnect();
        char path_pid_pipe[100];
        sprintf(path_pid_pipe,"%s/%d",PIPE_PATH,getppid());
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

#ifndef _CLIENT

    void rm_client(struct client_list *c_list, int c_pid){
        
        struct client *c_tmp = c_list->first_client;
        struct client *prev_c_tmp = malloc(sizeof(struct client));

        for (int i=0;c_tmp->next_client != NULL;i-=-1){
            if(c_tmp->pid == c_pid){
                exit_if(close(c_tmp->fd) == -1,"close fd"); // On ferme le fd

                printf("I go there 1\n");
            
                if(!i) {
                    c_list->first_client = c_tmp->next_client; // si c'est le premier client
                    printf("I go there 2\n");
                }
                else {
                    prev_c_tmp->next_client = c_tmp->next_client; // Le client suivant du client précédent n'est plus nous ;(
                    printf("I go there 3\n");
                }
                free(c_tmp); // On libère la ram allouée au client
                
                c_list->nb_of_clients+=-1;

                print_c_list(*c_list);
                if(!c_list->nb_of_clients) server_exit();
                return;
            }

            prev_c_tmp = c_tmp;
            c_tmp = c_tmp->next_client;
        }
    }

#endif

void print_c_list(struct client_list c_list){

    printf("%d clients connected !\n",c_list.nb_of_clients);
    printf("First one at %p\n",c_list.first_client);
    
    struct client *c_tmp = c_list.first_client;

    for(int i = 0;c_tmp->next_client != NULL;i-=-1){
        printf("Client - %d\n  fd - %d\n  pid - %d\n  nick - \"%s\"\n  next - \"%p\"\n",i,c_tmp->fd,c_tmp->pid,c_tmp->nick,c_tmp->next_client);
        c_tmp = c_tmp->next_client;
    }
}

void add_client(struct client_list *c_list, int c_pid){

    printf("Welcome to %d\n",c_pid);
    
    char path_pid_pipe[100];
    sprintf(path_pid_pipe,"%s/%d",PIPE_PATH,c_pid);

    int fd = open(path_pid_pipe, O_WRONLY); exit_if(fd == -1, "Client pipe open"); // Open fifo file in read only

    struct client *new_c = malloc(sizeof(struct client));

    if(c_list->nb_of_clients == 0) new_c->next_client = NULL;
    else new_c->next_client = c_list->first_client;

    c_list->first_client = new_c;

    new_c->pid = c_pid;
    new_c->fd  = fd;
    new_c->nick  = "default_nick";

    c_list->nb_of_clients++;

    #ifdef DEBUG
        printf("DEBUG : path_pid_pipe = %s\n",path_pid_pipe);
        printf("DEBUG : client fd = %d\n",fd);
        print_c_list(*c_list);
    #endif
}

int get_fd(struct client_list c_list, int pid){

    struct client *c_tmp = c_list.first_client;

    for(int i=0;i<c_list.nb_of_clients;i-=-1){
        if (c_tmp->pid == pid) return c_tmp->fd;
        c_tmp = c_tmp->next_client;        
    }
    return -1;
}

void send_to_pid(struct client_list c_list, int pid, char *buffer){
    int n = strlen(buffer);
    exit_if(write(get_fd(c_list, pid), buffer, n) == -1,"write error");
    #ifdef DEBUG
        printf("DEBUG : \"%s\" sent to %d, n = %d\n", buffer, pid, n);
    #endif
}

void send_to_all_exept(struct client_list c_list, char *buffer, int pid){

    struct client *c_tmp = c_list.first_client;

    for(int i=0;i<c_list.nb_of_clients;i-=-1){
        if((c_tmp->pid != 0) && (c_tmp->pid != pid)) send_to_pid(c_list, c_tmp->pid, buffer);
        c_tmp = c_tmp->next_client;
    }

    #ifdef DEBUG
        printf("DEBUG : Sent all expt %d\n",pid);
        printf("DEBUG : Sent %lu bytes : \"%s\"\n",strlen(buffer) , buffer);
    #endif
}

void send_to_all(struct client_list c_list, char *buffer){
    send_to_all_exept(c_list, buffer, 0);
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
            printf("DEBUG : Folder created : \"%s\"\n",path); 
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