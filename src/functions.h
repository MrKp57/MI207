#ifndef _FUNCTS
#define _FUNCTS

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>

#include <sys/stat.h>

#define PIPE_PATH "/tmp/chat"
#define MAIN_PIPE PIPE_PATH "/0"

#define EXIT_MESSAGE "/quit"
#define prompt "Msg : "

#define MAX_CLIENTS 100

static const char *signames[] = {
    "SIGHUP",  "SIGINT",    "SIGQUIT", "SIGILL",   "SIGTRAP", "SIGABRT", "SIGEMT",  "SIGFPE",
    "SIGKILL", "SIGBUS",    "SIGSEGV", "SIGSYS",   "SIGPIPE", "SIGALRM", "SIGTERM", "SIGURG",
    "SIGSTOP", "SIGTSTP",   "SIGCONT", "SIGCHLD",  "SIGTTIN", "SIGTTOU", "SIGIO",   "SIGXCPU",
    "SIGXFSZ", "SIGVTALRM", "SIGPROF", "SIGWINCH", "SIGINFO", "SIGUSR1", "SIGUSR2",
};

struct client{
    int pid;
    char *nick;
    int fd;
};

struct client c_list[MAX_CLIENTS];

void send_disconnect();
void send_hello(int fd);
const char *signame(int signal);
void client_exit();
void server_exit();
void exit_if(int condition, const char *prefix);
void rm_client(int c_pid);
void print_c_array();
int add_client(int c_pid); 
int get_fd(int pid);
void send_to_pid(int pid, char *buffer);
void send_to_all_exept(char *buffer, int pid);
void send_to_all(char *buffer);
void redirect_ctrl_c();

void create_folder(char *path);
int get_pid(char *buffer);
int is_hello(char *buffer);
int get_data(char *buffer, char *data);

#endif