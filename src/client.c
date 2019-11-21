
#ifndef _CLIENT
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
}*/

void lockfile_protect() {
    while(!access(LOCKFILE_PATH, F_OK)) {
        sched_yield(); //On ne test pas la valeur de retour car notre programme est conçu pour LINUX :) 
        #ifdef DEBUG
            printf("DEBUG : WAITING FOR 0.LOCK FREE ... SCHED YIELD\n");
        #endif
    }
    exit_if(open(LOCKFILE_PATH, O_CREAT)==-1,"Lockfile creation"); // Create lockfile 

}

void send_to_server(int fd, char* message, int n) {
    lockfile_protect();
    write(fd, message, n);
    remove(LOCKFILE_PATH);
}

int main(int argc, char **argv){
    
    if (access(MAIN_PIPE, F_OK)){ // Si le pipe 0 n'existe pas
        printf("Please launch server first !\n"); 
        client_exit();
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


    #ifdef DEBUG
        printf("DEBUG : fifo file opened\n");    
    #endif

    //Choose nickname
    printf("Please choose a nickname before chating with us :\n");
    char buffer_nickname[20];
    int jeanpierre = read(STDIN_FILENO, buffer_nickname, 20);
    buffer_nickname[jeanpierre-1] = NULL;

    #ifdef DEBUG
        printf("DEBUG : %s, size=%d\n", buffer_nickname, strlen(buffer_nickname));
    #endif

    char cmd[100];
    int n = snprintf(cmd, sizeof(cmd),"%d,%lu,%s", getpid(), strlen(cmd), "/who");

    send_to_server(fd, cmd, n);
    
    int fork_rtn;

    switch (fork_rtn = fork()){
        case -1: {
            exit_if(1,"fork");
        }
        break;

        case 0: { // C'est le fils /!!\ NOUVEAU PID - Reader

            redirect_ctrl_c();

            int my_fd = open(path_pid_pipe, O_RDONLY);
            exit_if(my_fd == -1, path_pid_pipe); // Open fifo file in read only
        
            int n;
            char buffer[100];
            while(1){
                while((n = read(my_fd, buffer, sizeof(buffer)-1)) > 0) {
                    
                    printf("n = %d\n",n);

                    buffer[n] = 0;

                    #ifdef DEBUG

                        printf("\nDEBUG : Received %d bytes : \"%s\"\n",n ,buffer);
                        
                        printf("Hdbg : \n\"");
                        for(int g=0;g<20;g-=-1) printf("%c-",buffer[g]);
                        printf("\"\n");

                    #endif

                    //// Pid calculation
                    int rmt_pid = get_pid(buffer); // Pid calculation

                    //// Len & Data calculation
                    char data_c[10000];
                    int data_len = get_data(buffer, data_c);
                    
                    printf("\n\n// Data received ! \\\\\n   From pid = %d\n   Data_len = %d\n   Data = \"%s\"\n\\\\ End of data //\n",rmt_pid, data_len,data_c);
                    printf("%s",prompt);

                    //Check if data received is command

                    fflush(stdout);
                }
            }
        }    
        break;
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
                    if(read_rtn==-1) {
                        printf("read error\n");
                        kill(fork_rtn,SIGINT);
                        exit(EXIT_FAILURE);
                    }
                    #ifdef DEBUG
                        printf("DEBUG : read : \"%c\"=%d read_rtn = %d\n",buffer_read[0],(int)buffer_read[0],read_rtn);
                    #endif
                    if(buffer_read[0]==10) buffer[i] = 0;
                    else buffer[i] = buffer_read[0];
                    i-=-1;
                }while(buffer_read[0] != 10);
                
                n = snprintf(message, sizeof(message),"%d,%lu,%s", getpid(), strlen(buffer), buffer);
                if(i-1){
                    
                    if(write(fd, message, n) ==-1) {
                        printf("write error\n");
                        kill(fork_rtn,SIGINT);
                        exit(EXIT_FAILURE);
                    }

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

#endif