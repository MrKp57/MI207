
#ifndef _SERVER
#define _SERVER
#include "functions.h"

int main(int argc, char **argv){

    struct client_list c_list;
    c_list.first_client  = NULL;
    c_list.nb_of_clients = 0;

    redirect_ctrl_c();

    printf("----- Start server:%d -----\n", getpid());

    create_folder(PIPE_PATH);

    exit_if(mkfifo(MAIN_PIPE, 0666)==-1,"mkfifo"); // Create fifo file
    
    #ifdef DEBUG
        printf("DEBUG : fifo file created\n"); 
    #endif

    int fd = open(MAIN_PIPE, O_RDONLY); exit_if(fd == -1, MAIN_PIPE); // Open fifo file in read only

    #ifdef DEBUG
        printf("DEBUG : fifo file opened\n");    
    #endif

    int n;
    char buffer[100];
    while(1){
        printf("Waiting for data ...\n");
        fflush(stdout);
        usleep(1000*200);
        while((n = read(fd, buffer, sizeof(buffer)-1)) > 0) {
            buffer[n] = 0;
            #ifdef DEBUG
                printf("DEBUG : Received %d bytes : \"%s\"\n",n , buffer);
            #endif

            int rmt_pid = get_pid(buffer); // Pid calculation
            
            if(is_hello(buffer)) add_client(&c_list,rmt_pid); // HELLO RECEIVED
            else { // if is data

                char data_c[100];
                int data_len = get_data(buffer, data_c);

                printf("// Data received ! \\\\\n   From pid = %d\n   Data_len = %d\n   Data = \"%s\"\n\\\\ End of data //\n",rmt_pid, data_len,data_c);

                if(!strcmp(data_c,EXIT_MESSAGE)){ // Disconect message
                    printf("Client %d disconnected!\n",rmt_pid);
                    rm_client(&c_list, rmt_pid);
                }
                else {
                    printf("sending to all expt %d\n",rmt_pid);
                    send_to_all_exept(c_list, buffer, rmt_pid);
                }
                

            }
        }
    }
    return 0;
}

#endif