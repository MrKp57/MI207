#ifndef _FUNCTS
#define _FUNCTS

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sched.h>

#include <sys/stat.h>
#include <sys/wait.h>

#define PIPE_PATH "/tmp/chat"
#define MAIN_PIPE PIPE_PATH "/0"
#define LOCKFILE_PATH PIPE_PATH "/0.LOCK"

#define MAX_NICK_SIZE 20

#define EXIT_MESSAGE "/quit"
#define prompt "Msg : "

#define LOG_PATH "/tmp/chat.log"
#define SERV_DAEMON_LOGFILE_PATH LOG_PATH "/srv_dem.log"


static const char *signames[] = {
    "SIGHUP",  "SIGINT",    "SIGQUIT", "SIGILL",   "SIGTRAP", "SIGABRT", "SIGEMT",  "SIGFPE",
    "SIGKILL", "SIGBUS",    "SIGSEGV", "SIGSYS",   "SIGPIPE", "SIGALRM", "SIGTERM", "SIGURG",
    "SIGSTOP", "SIGTSTP",   "SIGCONT", "SIGCHLD",  "SIGTTIN", "SIGTTOU", "SIGIO",   "SIGXCPU",
    "SIGXFSZ", "SIGVTALRM", "SIGPROF", "SIGWINCH", "SIGINFO", "SIGUSR1", "SIGUSR2",
};

struct client_list{
    int nb_of_clients;
    struct client *first_client;
};

struct client{
    int pid;
    char *nick;
    int fd;
    struct client *next_client;
};

void send_disconnect();
int send_hello(int fd);
const char *signame(int signal);
void client_exit();
void client_exit_wait();
void server_exit();
void exit_if(int condition, const char *prefix);
void rm_client(struct client_list *c_list, int c_pid);
void print_c_list(struct client_list c_list);
int data_input(int in_fd, char **out_str);
int data_input_key(char **out_str);
void launch_serv_if_abs();
int choose_nick(char *buffer_nickname);
void add_client(struct client_list *c_list, int c_pid); 
int get_fd(struct client_list c_list, int pid);
void send_to_pid(struct client_list c_list, int pid, char *buffer);
void send_to_all_exept(struct client_list c_list, char *buffer, int pid);
void send_to_all(struct client_list c_list, char *buffer);
void redirect_ctrl_c();
void redirect_ctrl_c_wait();

int send_to_server(int fd, char* message, int n);
void lockfile_protect();
int pipe_input(int in_fd, char **out_str);

int set_nickname_to(struct client_list c_list, int rmt_pid, char *cmd_args);
void list_of_clients(struct client_list c_list, char **nicknames);

void create_folder(char *path);
int get_pid(char *buffer);
int is_hello(char *buffer);
int get_data(char *buffer, char **data);

int is_command(int len, char *data_c, char **command, char **cmd_args);

int is_disconnect(char *data_c);

#endif

