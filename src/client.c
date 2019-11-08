#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/stat.h>

#define PIPE_PATH "/tmp/chat/0"

#define DEBUG

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

void exiting(){
    printf("----- Stop client -----\n");
    exit(0);
}
int main(int argc, char **argv){   
    printf("----- Start client:%d -----\n",getpid());

    if (access(PIPE_PATH, F_OK)){ // Si le pipe 0 n'existe pas
        printf("Please launch server first !\n"); 
        printf("----- Stop client -----\n");
        exit(0);
    }

    int fd = open(PIPE_PATH, O_WRONLY);
    exit_if(fd == -1, "Pipe open"); // Open fifo file in read only

    #ifdef DEBUG
        printf("DEBUG : fifo file opened\n");    
    #endif

    send_hello(fd);

    //usleep(1000*1000);

    //exiting();

    #ifdef DEBUG
        printf("file %s is open for write\n", PIPE_PATH);
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