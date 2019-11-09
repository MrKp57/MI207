#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <sys/stat.h>

#define PIPE_PATH "/tmp/chat"
#define MAIN_PIPE PIPE_PATH "/0"

#define EXIT_MESSAGE "igloo/igloo\\igloo"

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

    char path_pid_pipe[100];
    sprintf(path_pid_pipe,"%s/%d",PIPE_PATH,getpid());
    #ifdef DEBUG
        printf("DEBUG :  FILE PATH =  %s\n", path_pid_pipe);
    #endif
    exit_if(mkfifo(path_pid_pipe, 0666)==-1,"mkfifo"); // Create pid chat file

    if (access(MAIN_PIPE, F_OK)){ // Si le pipe 0 n'existe pas
        printf("Please launch server first !\n"); 
        exiting(0);
    }

    fd = open(MAIN_PIPE, O_WRONLY);
    exit_if(fd == -1, "Pipe open"); // Open fifo file in read only

    #ifdef DEBUG
        printf("DEBUG : fifo file opened\n");
    #endif

    send_hello(fd);

    #ifdef DEBUG
        printf("file %s is open for write\n", MAIN_PIPE);
    #endif
    
    // Demande du message au client

    char message[100];
    char buffer[100];
    int n;
    int i = 0;

    char buffer_read[1];
    int read_rtn;
    while(1){
        printf("Msg : ");
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
        write(fd, message, n);

        #ifdef DEBUG
            printf("DEBUG : %d bytes sent : \"%s\"\n",n,buffer);
        #endif
    }
    

    
 
    return 0;

}