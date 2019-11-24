
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

int main(int argc, char **argv){
    
    launch_serv_if_abs();

    int srv_fd = open(MAIN_PIPE, O_WRONLY);
    exit_if(srv_fd == -1, "Main pipe open"); // Open fifo file in read only

    #ifdef DEBUG
        printf("DEBUG : fifo file %s is open for write\n", MAIN_PIPE);
    #endif

    send_hello(srv_fd);

    char path_pid_pipe[100];
    sprintf(path_pid_pipe,"%s/%d",PIPE_PATH,getpid());

    #ifdef DEBUG
        printf("DEBUG : FILE PATH =  %s\n", path_pid_pipe);
    #endif

    exit_if(mkfifo(path_pid_pipe, 0666)==-1,"mkfifo"); // Create pid chat file

    #ifdef DEBUG
        printf("DEBUG : fifo file opened\n");    
    #endif

    // char cmd[100] = {0};
    // int n = snprintf(cmd, sizeof(cmd),"%d,%lu,%s", getpid(), strlen("/who"), "/who");

    // send_to_server(srv_fd, cmd, n);
    
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

            int rmt_pid  = 0;
            int data_len = 0;

            char *rec_msg = NULL;
            char *data_c  = NULL;

            while(1){
                data_len = pipe_input(my_fd,&rec_msg);

                #ifdef DEBUG
                    printf("\nDEBUG : Received %d bytes : \"%s\"\n",data_len ,rec_msg);
                #endif

                //// Pid calculation
                rmt_pid = get_pid(rec_msg); // Pid calculation

                //// Len & Data calculation
                data_len = get_data(rec_msg, &data_c);
                
                printf("\n\n// Data received ! \\\\\n   From pid = %d\n   Data_len = %d\n   Data = \"%s\"\n\\\\ End of data //\n",rmt_pid, data_len,data_c);
                printf("%s",prompt);

                fflush(stdout);
            
            }
        }    
        break;


        default: { // C'est le papa - Sender

            char buffer_nickname[MAX_NICK_SIZE];
            int nick_size = choose_nick(buffer_nickname);

            char *message = NULL;
            char *buffer = NULL;
            int n;

            int len;

            while(1){
                redirect_ctrl_c_wait();

                printf("%s",prompt);
                fflush(stdout);

                len = data_input_key(&buffer); // Blocking read function
                
                if(len==-1) {
                    printf("Keyboard error\n");
                    kill(fork_rtn,SIGINT);
                    exit(EXIT_FAILURE);
                }

                int msg_size = strlen(buffer)+100; // Will be enough

                message = malloc(msg_size);
                if(message==NULL) {
                    printf("malloc failed\n");
                    kill(fork_rtn,SIGINT);
                    exit(EXIT_FAILURE);
                }

                n = snprintf(message, msg_size,"%d,%lu,%s", getpid(), strlen(buffer), buffer);
                                
                if(len-1){
                    
                    if(send_to_server(srv_fd, message, n) ==-1) {
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