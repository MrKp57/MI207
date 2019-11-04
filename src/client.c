#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/stat.h>

#define PIPE_PATH "/tmp/chat/0"

#define DEBUG 0

void exit_if(int condition, const char *prefix){
    if (condition){
        perror(prefix);
        exit(1);
    }
}

int main(int argc, char **argv){   
    printf("----- Start client:%d -----\n",getpid());
    
    #if DEBUG
        printf("DEBUG : fifo file created\n"); 
    #endif

    if (access(PIPE_PATH, F_OK)){
        printf("Please launch server first !\n"); 
        printf("----- Stop client -----\n");
        exit(0);
    }
    int fd = open(PIPE_PATH, O_WRONLY);
    exit_if(fd == -1, "Pipe open"); // Open fifo file in read only
    #if DEBUG
        printf("DEBUG : fifo file opened\n");    
    #endif

    printf("file %s is open for write\n", PIPE_PATH);
    char buffer[100];
    int n = snprintf(buffer, sizeof(buffer),"%d,6,coucou", getpid());
    write(fd, buffer, n);
    printf("%d bytes sent : \"%s\"\n",n,buffer);
    
    printf("----- Stop client -----\n");

    return 0;
}