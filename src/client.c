#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/stat.h>

#define PIPE_PATH "/tmp/pipe0"

#define DEBUG 1

void exit_if(int condition, const char *prefix){
    if (condition){
        perror(prefix);
        exit(1);
    }
}

int main(int argc, char **argv){   
    printf("----- Start client -----\n"); 
    //exit_if(mkfifo(PIPE_PATH, 0666)==-1,"mkfifo"); // Create fifo file
    #if DEBUG
        printf("DEBUG : fifo file created\n"); 
    #endif

    int fd = open(PIPE_PATH, O_WRONLY);
    exit_if(fd == -1, PIPE_PATH); // Open fifo file in read only
    #if DEBUG
        printf("DEBUG : fifo file opened\n");    
    #endif

    printf("file %s is open for write\n", PIPE_PATH);
    char buffer[100];
    int n = snprintf(buffer, sizeof(buffer),"I'm %d", getpid());
    write(fd, buffer, n);
    printf("end of write\n");

    return 0;
}