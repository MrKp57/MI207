#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <sys/stat.h>

#define PIPE_PATH "/tmp/chat"
#define MAIN_PIPE PIPE_PATH "/0"

#define DEBUG

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
    int n = snprintf(buffer, sizeof(buffer),"%d,18,igloo/igloo\\igloo", getpid());
    write(fd, buffer, n);
    #ifdef DEBUG
        printf("DEBUG : disconnect sent\n");    
    #endif
}

void exiting(){
    /*if(fd > 0) {
        send_disconnect(fd);
    }*/
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
    char buffer_read;
    int read_rtn;
    do{
        read_rtn = read(STDIN_FILENO, &buffer_read, 1);
        exit_if(read_rtn==-1,"read");
        #ifdef DEBUG
            printf("read : %c\n",buffer_read);
        #endif
    }while(buffer_read != 'a');

    char buffer[100];
    int n = snprintf(buffer, sizeof(buffer),"%d,6,coucou", getpid());
    write(fd, buffer, n);
    printf("%d bytes sent : \"%s\"\n",n,buffer);
 
    return 0;

}