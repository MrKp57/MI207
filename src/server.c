#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#include <sys/stat.h>

#define PIPE_PATH "/tmp/pipe0"

#define DEBUG 1

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

void exiting(int signal){
    printf("\nExiting server, closing pipe\n");
    
    exit(1);
}

void exit_if(int condition, const char *prefix){
    if (condition){
        perror(prefix);
        remove(PIPE_PATH);
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
    
    #if DEBUG
        printf("DEBUG : fifo file created\n"); 
    #endif

    int fd = open(PIPE_PATH, O_RDONLY);
    exit_if(fd == -1, PIPE_PATH); // Open fifo file in read only

    #if DEBUG
        printf("DEBUG : fifo file opened\n");    
    #endif

    int n;
    char buffer[100];
    while((n = read(fd, buffer, sizeof(buffer)-1)) > 0) {
        buffer[n] = 0;
        printf("reader: \"%s\"\n", buffer);
    }

    exit_if(remove(PIPE_PATH)==-1,"remove");
    
    return 0;
}
