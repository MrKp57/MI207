#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/stat.h>

#define PIPE_PATH "/tmp/pipe0"

#define DEBUG 1

void exit_if(int condition, const char *prefix){
    if (condition){
        perror(prefix);
        remove(PIPE_PATH);
        exit(1);
    }
}

int main(int argc, char **argv){

    printf("----- Start server:%d -----\n", getpid());

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