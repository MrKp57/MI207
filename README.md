Projet chat client serveur

# Client

SOLVED : 
  ~~- Le fork implique des soucis dans les getpid suivants et le ctrl c est capté par le fils et le pere~~

WIP :
    
TODO :
  - Ask client for nick at startup
  - Lockfile to prevent 2 client for write at same time

# Server

TODO :
  - Change client memory management : change from struct to linked list


# Both

TODO :
  - Each sys call can return -1 and need 2 be tested.
  - Add commands (i.e nickname, dest (pid or nick))
  - Dynamic messge size

if (on a le temps) : TOTO :
  - TCP/IP implement
