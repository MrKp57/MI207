Projet chat client serveur

## Client

SOLVED : 
  ~~- Le fork implique des soucis dans les getpid suivants et le ctrl c est capté par le fils et le pere~~

WIP :
    
TODO :
  - Ask client for nick at startup
  - Lockfile to prevent 2 client for write at same time

## Server

WIP : 
  - Change client memory management : change from struct to linked list

TODO :
  - Make it a daemon
  - Suicide if empty (last client disco)
  

## Both

TODO :
  - Each sys call can return -1 and need 2 be tested.
  - Add commands (i.e nickname, dest (pid or nick))
  - Dynamic messge size
  - Add channels

if (on a le temps) : TOTO :
  - TCP/IP implement


# Command protocol
  - Identifier : '/'
  - Command list :
    - /who "List of connected users"
    - /nick "Change my nickname to another free"
