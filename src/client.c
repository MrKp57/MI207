#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <sys/stat.h>

#define PIPE_PATH "/tmp/chat"
#define MAIN_PIPE PIPE_PATH "/0"

#define EXIT_MESSAGE "igloo/igloo\\igloo"
#define prompt "Msg : "

int fd = -1;

static const char *signames[] = {
    "SIGHUP",  "SIGINT",    "SIGQUIT", "SIGILL",   "SIGTRAP", "SIGABRT", "SIGEMT",  "SIGFPE",
    "SIGKILL", "SIGBUS",    "SIGSEGV", "SIGSYS",   "SIGPIPE", "SIGALRM", "SIGTERM", "SIGURG",
    "SIGSTOP", "SIGTSTP",   "SIGCONT", "SIGCHLD",  "SIGTTIN", "SIGTTOU", "SIGIO",   "SIGXCPU",
    "SIGXFSZ", "SIGVTALRM", "SIGPROF", "SIGWINCH", "SIGINFO", "SIGUSR1", "SIGUSR2",
};

const char *signame(int signal){
    if (signal >= SIGHUP && signal <= SIGUSR2)
        return signames[signal - SIGHUP];
    return "SIG???";
}

void send_disconnect(){
    char buffer[100];
    int n = snprintf(buffer, sizeof(buffer),"%d,%d,%s", getpid(),sizeof(EXIT_MESSAGE),EXIT_MESSAGE);
    write(fd, buffer, n);
    #ifdef DEBUG
        printf("DEBUG : disconnect sent\n");    
    #endif
}

void exiting(){
    if(fd > 0) {
        send_disconnect(fd);
    }
    printf("----- Stop client -----\n");
    char path_pid_pipe[100];
    sprintf(path_pid_pipe,"%s/%d",PIPE_PATH,getpid());
    exit_if(remove(path_pid_pipe)==-1,"remove");
    exit(1);
}


void exit_if(int condition, const char *prefix){
    if (condition){
        perror(prefix);
        exit(1);
    }
}

void send_hello(int fd){
    char buffer[100];
    int n = snprintf(buffer, sizeof(buffer),"%d,0", getpid());
    write(fd, buffer, n);
    #ifdef DEBUG
        printf("DEBUG : hello sent\n");    
    #endif
}

void read_message(char *message){
    int i = 0;

    char buffer_read;
    int read_rtn;

    do{
        read(STDIN_FILENO, &message[i], 1);
        exit_if(read_rtn==-1,"read");
        #ifdef DEBUG
            printf("read : %s read_rtn = %d\n",buffer_read,read_rtn);
        #endif
    }while(buffer_read != 10);
}

int main(int argc, char **argv){
    printf("----- Start client:%d -----\n",getpid());

    struct sigaction sa = {
        .sa_flags = 0,
    };

    sigemptyset(&sa.sa_mask);
    sa.sa_handler = exiting;

    exit_if(sigaction(SIGINT, &sa, NULL) == -1,"sigaction");
    
    if (access(MAIN_PIPE, F_OK)){ // Si le pipe 0 n'existe pas
        printf("Please launch server first !\n"); 
        exiting(0);
    }

    fd = open(MAIN_PIPE, O_WRONLY);
    exit_if(fd == -1, "Pipe open"); // Open fifo file in read only

    #ifdef DEBUG
        printf("DEBUG : fifo file %s is open for write\n", MAIN_PIPE);
    #endif

    send_hello(fd);

    char path_pid_pipe[100];
    sprintf(path_pid_pipe,"%s/%d",PIPE_PATH,getpid());
    #ifdef DEBUG
        printf("DEBUG : FILE PATH =  %s\n", path_pid_pipe);
    #endif
    exit_if(mkfifo(path_pid_pipe, 0666)==-1,"mkfifo"); // Create pid chat file

    int my_fd = open(path_pid_pipe, O_RDONLY);
    exit_if(my_fd == -1, path_pid_pipe); // Open fifo file in read only

    #ifdef DEBUG
        printf("DEBUG : fifo file opened\n");    
    #endif
    
    int fork_rtn;

    switch (fork_rtn = fork()){
        case 0: { // C'est le fils - Reader
        
            int n;
            char buffer[100];
            while(1){
                while((n = read(my_fd, buffer, sizeof(buffer)-1)) > 0) {
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
                        rmt_pid_c[i+1]=NULL;
                    }
                    int rmt_pid = atoi(rmt_pid_c);

                    i++;
                    
                    last_i = i;

                    
                        
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

                    last_i = ++i;

                    do{
                        data_c[i-last_i]=buffer[i];
                        #ifdef DEBUG
                            printf("DEBUG : buffer[%d]:%c",i,buffer[i]);
                            printf("data_c[%d]:%c\n",i-last_i,data_c[i-last_i]);
                        #endif
                        data_c[++i-last_i+1]=NULL;
                    }while(i != last_i+data_len);

                    printf("\n// Data received ! \\\\\n   From pid = %d\n   Data_len = %d\n   Data = \"%s\"\n\\\\ End of data //\n",rmt_pid, data_len,data_c);
                    printf("%s",prompt);
                    fflush(stdout);
                }
            }
        }    
        break;
        case -1: {
            exit_if(1,"fork");
        }
        default: { // C'est le papa - Sender
            char message[100];
            char buffer[100];
            int n;
            int i = 0;

            char buffer_read[1];
            int read_rtn;
            while(1){
                printf("%s",prompt);
                fflush(stdout);
                i=0;
                do{
                    read_rtn = read(STDIN_FILENO, buffer_read, 1);
                    exit_if(read_rtn==-1,"read");
                    #ifdef DEBUG
                        printf("DEBUG : read : \"%c\"=%d read_rtn = %d\n",buffer_read[0],(int)buffer_read[0],read_rtn);
                    #endif
                    if(buffer_read[0]==10) buffer[i] = 0;
                    else buffer[i] = buffer_read[0];
                    i-=-1;
                }while(buffer_read[0] != 10);
                
                n = snprintf(message, sizeof(message),"%d,%d,%s", getpid(), strlen(buffer), buffer);
                if(i-1){
                    write(fd, message, n);

                    #ifdef DEBUG
                        printf("DEBUG : %d bytes sent : \"%s\"\n",n,buffer);
                    #endif
                }

            }
        }
        break;
    }

    return 0;

}