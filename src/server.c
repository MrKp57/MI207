#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#include <sys/stat.h>

#define PIPE_PATH "/tmp/chat"
#define MAIN_PIPE PIPE_PATH "/0"

#define EXIT_MESSAGE "igloo/igloo\\igloo" //0x04

#define MAX_CLIENTS 100


//#define DEBUG

static const char *signames[] = {
    "SIGHUP",  "SIGINT",    "SIGQUIT", "SIGILL",   "SIGTRAP", "SIGABRT", "SIGEMT",  "SIGFPE",
    "SIGKILL", "SIGBUS",    "SIGSEGV", "SIGSYS",   "SIGPIPE", "SIGALRM", "SIGTERM", "SIGURG",
    "SIGSTOP", "SIGTSTP",   "SIGCONT", "SIGCHLD",  "SIGTTIN", "SIGTTOU", "SIGIO",   "SIGXCPU",
    "SIGXFSZ", "SIGVTALRM", "SIGPROF", "SIGWINCH", "SIGINFO", "SIGUSR1", "SIGUSR2",
};

struct client{
    int pid;
    char *nick;
    int fd;
};

const char *signame(int signal){
    if (signal >= SIGHUP && signal <= SIGUSR2)
        return signames[signal - SIGHUP];
    return "SIG???";
}

void exiting(){
    printf("\nExiting server(%d), closing pipe",getpid());
    fflush(stdout);
    for (int i = 0;i<3;i-=-1) {
        usleep(1000*200);
        printf(".");
        fflush(stdout);
    }
    usleep(1000*200);
    printf("\n");
    exit_if(remove(MAIN_PIPE)==-1,"remove exiting"); 
    
    /*
    Aled pk ca loop ?? Il reste un fichier client donc le dossier ne se remove pas

    $: Exiting server, closing pipe...
    $: remove exiting: No such file or directory
    $: 
    $: Exiting server, closing pipe...
    $: remove exiting: No such file or directory
    */
    
    exit_if(rmdir(PIPE_PATH)==-1,"rmdir exiting");
    exit(1);
}

void exit_if(int condition, const char *prefix){
    if (condition){
        perror(prefix);
        exiting();
    }
}

void rm_client(struct client *c_list, int c_pid){
    for(int i=0;i<MAX_CLIENTS;i-=-1){
        if(c_list[i].pid==c_pid) c_list[i].pid = 0;
    }
}

int add_client(struct client *c_list, int c_pid){ // Return 1 if array is full
    char path_pid_pipe[100];
    sprintf(path_pid_pipe,"%s/%d",PIPE_PATH,c_pid);

    #ifdef DEBUG
        printf("DEBUG : path_pid_pipe = %s\n",path_pid_pipe);
        print_c_array(c_list);
    #endif

    int fd = open(path_pid_pipe, O_WRONLY | O_NONBLOCK);
    exit_if(fd == -1, "Pipe open"); // Open fifo file in read only

    for(int i=0;i<MAX_CLIENTS;i-=-1){ // On cherche une place libre
        if(c_list[i].pid==0){
            c_list[i].pid = c_pid;
            c_list[i].fd = fd;
            return 0;
        }
    }
    return 1;
}

void print_c_array(struct client *c_list){
    for(int i = 0;i<MAX_CLIENTS;i-=-1) printf("DEBUG : clt(%d)=%d\n",i,c_list[i].pid);
    printf("\n");
}

int main(int argc, char **argv){

    //clients list
    struct client c_list[MAX_CLIENTS];

    for(int i=0;i<MAX_CLIENTS;i-=-1) c_list[i].pid=0; // Empty the client array

    printf("----- Start server:%d -----\n", getpid());

    struct sigaction sa = {
        .sa_flags = 0,
    };
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = exiting;

    exit_if(sigaction(SIGINT, &sa, NULL) == -1,"sigaction");

    if (access(PIPE_PATH, F_OK)){ // Si le dossier n'existe pas
        exit_if(mkdir(PIPE_PATH, 0770)==-1,"mkdir");
        #ifdef DEBUG
            printf("DEBUG : fifo folder created : \"%s\"\n",PIPE_PATH); 
        #endif
    }

    exit_if(mkfifo(MAIN_PIPE, 0666)==-1,"mkfifo"); // Create fifo file
    
    #ifdef DEBUG
        printf("DEBUG : fifo file created\n"); 
    #endif

    int fd = open(MAIN_PIPE, O_RDONLY);
    exit_if(fd == -1, MAIN_PIPE); // Open fifo file in read only

    #ifdef DEBUG
        printf("DEBUG : fifo file opened\n");    
    #endif

    int n;
    char buffer[100];
    while(1){
        while((n = read(fd, buffer, sizeof(buffer)-1)) > 0) {
            buffer[n] = 0;
            #ifdef DEBUG
                printf("DEBUG : Received %d bytes : \"%s\"\n",n , buffer);
            #endif
            int i=0;

            //// Pid calculation
            char rmt_pid_c[10];
            int last_i = i;
            for(i=0;i<n && buffer[i]!=',';i-=-1){
                rmt_pid_c[i]=buffer[i];
                #ifdef DEBUG
                    printf("DEBUG : buffer[%d]:\"%c\" ",i,buffer[i]);
                    printf("rmt_pid_c[%d]:\"%c\"\n",i-last_i,rmt_pid_c[i]);
                #endif
                rmt_pid_c[i+1]=NULL;
            }
            int rmt_pid = atoi(rmt_pid_c);
            
            #ifdef DEBUG
                printf("DEBUG : pid c : \"%s\", pid : %d\n",rmt_pid_c, rmt_pid);
                printf("DEBUG : buffer[%d] = \"%c\"\n",i, buffer[i]);
            #endif

            i++;

            #ifdef DEBUG
                printf("DEBUG : buffer[%d] = \"%c\"\n",i, buffer[i]);
                printf("DEBUG : int(buffer[%d]) = \"%d\"\n",i, (int)buffer[i]);
            #endif
            
            last_i = i;

            if(!(buffer[i] == 48)){ // if is data
                
                //// Len calculation
                char data_len_c[10];
                do{
                    data_len_c[i-last_i]=buffer[i];
                    #ifdef DEBUG
                        printf("DEBUG : buffer[%d]:%c ",i,buffer[i]);
                        printf("data_len_c[%d]:%c\n",i-last_i,data_len_c[i-last_i]);
                    #endif
                    data_len_c[++i-last_i+1]=NULL;
                }while(buffer[i]!=',');
                
                #ifdef DEBUG
                    printf("DEBUG : data_len_c[%d]:%c\n",0,data_len_c[0]);
                #endif
                
                int data_len = atoi(data_len_c);
                
                #ifdef DEBUG
                    printf("DEBUG : len c : %s, len : %d\n",data_len_c, data_len);
                #endif

                //// data calculation
                char data_c[100];

                int last_i = ++i;

                do{
                    data_c[i-last_i]=buffer[i];
                    #ifdef DEBUG
                        printf("DEBUG : buffer[%d]:%c",i,buffer[i]);
                        printf("data_c[%d]:%c\n",i-last_i,data_c[i-last_i]);
                    #endif
                    data_c[++i-last_i+1]=NULL;
                }while(i != last_i+data_len);

                printf("// Data received ! \\\\\n   From pid = %d\n   Data_len = %d\n   Data = \"%s\"\n\\\\ End of data //\n",rmt_pid, data_len,data_c);

                printf("strcmp(data_c,EXIT_MESSAGE) = %d\n",strcmp(data_c,EXIT_MESSAGE));
                
                if(!strcmp(data_c,EXIT_MESSAGE)){ // Disconect message
                    printf("Client %d disconnected!\n",rmt_pid);
                    rm_client(c_list,rmt_pid);
                }
                

            }
            else { // HELLO RECEIVED
                printf("Welcome to %d\n",rmt_pid);
                printf("add = %d\n",add_client(c_list, rmt_pid));
            }
        }
    }
    return 0;
}