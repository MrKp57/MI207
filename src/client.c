
#define _CLIENT
#include "functions.h"

/*
int read_message(char *message){
    int n;

    do{
        n = read(STDIN_FILENO, message, 1);
        exit_if(n==-1,"read");
        message[n]
    }while(message[i] != 10);
}
*/

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

    int fd = open(MAIN_PIPE, O_WRONLY);
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

                    //// Pid calculation
                    int rmt_pid = get_pid(buffer); // Pid calculation

                    //// Len calculation
                    char data_c[100];
                    int data_len = get_data(buffer, data_c);
                    
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
                
                n = snprintf(message, sizeof(message),"%d,%lu,%s", getpid(), strlen(buffer), buffer);
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