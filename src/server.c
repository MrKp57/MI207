
#ifndef _SERVER
#define _SERVER
#include "functions.h"

int main(int argc, char **argv){

    if(argc > 1){
        if(!strcmp(argv[1],"dem")){
            const char *log_fname = (argc == 3) ? argv[2] : SERV_DAEMON_LOGFILE_PATH;

            int i;
            for(i = strlen(log_fname);log_fname[i]!='/';i--);

            char *path = malloc(i);
            memcpy(path, log_fname, i);

            create_folder(path);
            
            int fork_session;

            int log_fd = open(log_fname, O_WRONLY | O_CREAT, 0744);

            exit_if(log_fd == -1, "open logfile");

            printf("%s = %d\n",log_fname, log_fd);
            exit_if(write(log_fd,"coucou",6) == -1,"write");

            exit_if(dup2(log_fd,STDOUT_FILENO) == -1,"Dup2 error stdout");
            exit_if(dup2(log_fd,STDERR_FILENO) == -1,"Dup2 error stderr");

            switch (fork_session = fork()){
                case -1 : {
                    exit_if(1,"fork error");
                }
                break;
                case 0 : { // Fils
                    exit_if(setsid() == -1,"Truc de la session la ou j'ai rien compris mais jcrois ca marche");
                }
                break;
                
                default : { // Pere
                    exit(0);
                }
                break;
            }

        }else printf("Error, syntax : server args\n  args are optionals [dem] \"Path to logfile\"\n");
    }
    else if (argc > 3) printf("Error, syntax : server args\n  args are optionals [dem] \"Path to logfile\"\n");


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