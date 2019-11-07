#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#include <sys/stat.h>

#define PIPE_PATH "/tmp/chat/0"

//#define DEBUG

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

void exiting(){
    printf("\nExiting server, closing pipe.\n");
    exit_if(remove(PIPE_PATH)==-1,"remove");
    exit(1);
}

void exit_if(int condition, const char *prefix){
    if (condition){
        perror(prefix);
        exit(1);
    }
}

int main(int argc, char **argv){

    printf("----- Start server:%d -----\n", getpid());

    struct sigaction sa = {
        .sa_flags = 0,
    };
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = exiting;

    exit_if(sigaction(SIGINT, &sa, NULL) == -1,"sigaction");

    exit_if(mkfifo(PIPE_PATH, 0666)==-1,"mkfifo"); // Create fifo file
    
    #ifdef DEBUG
        printf("DEBUG : fifo file created\n"); 
    #endif

    int fd = open(PIPE_PATH, O_RDONLY);
    exit_if(fd == -1, PIPE_PATH); // Open fifo file in read only

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
            int i;

            //// Pid calculation
            char rmt_pid_c[10];
            for(i=0;i<n && buffer[i]!=',';i++){
                rmt_pid_c[i]=buffer[i];
                rmt_pid_c[i+1]=NULL;
            }
            int rmt_pid = atoi(rmt_pid_c);
            
            #ifdef DEBUG
                printf("DEBUG : pid c : %s, pid : %d\n",rmt_pid_c, rmt_pid);
            #endif

            //// Len calculation
            char data_len_c[10];

            int last_i = ++i;
            do{
                data_len_c[i-last_i]=buffer[i];
                #ifdef DEBUG
                    printf("DEBUG : buffer[%d]:%c\n",i,buffer[i]);
                    printf("DEBUG : data_len_c[%d]:%c\n",i-last_i,data_len_c[i-last_i]);
                #endif
                data_len_c[++i-last_i+1]=0;
            }while(buffer[i]!=',');
            
            #ifdef DEBUG
                printf("DEBUG : data_len_c[%d]:%c\n",0,data_len_c[0]);
            #endif
            
            int data_len = atoi(data_len_c);
            
            #ifdef DEBUG
                printf("DEBUG : len c : %s, len : %d\n",data_len_c, data_len);
            #endif
            
            //// data calculation
            if(data_len){
                char data_c[100];

                int last_i = ++i;

                do{
                    data_c[i-last_i]=buffer[i];
                    #ifdef DEBUG
                        printf("DEBUG : buffer[%d]:%c\n",i,buffer[i]);
                        printf("DEBUG : data_c[%d]:%c\n",i-last_i,data_c[i-last_i]);
                    #endif
                    data_c[++i-last_i+1]=0;
                }while(i != last_i+data_len);


                printf("// Data received ! \\\\\n  from pid = %d\n  data_len = %d\n  data = \"%s\"\n\\\\ End of data //\n",rmt_pid, data_len,data_c);
            }
            else printf("Welcome %d\n",rmt_pid);
        }
    }
    
    return 0;
}
