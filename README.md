Projet chat client serveur

## Client

KNOWN BUGS :
  - When ctrl-c, if father kill himself first, son cannot remove ppid pipe :/

SOLVED : 
  ~~- Le fork implique des soucis dans les getpid suivants et le ctrl c est capté par le fils et le pere~~
  ~~- Ask client for nick at startup~~
  ~~- Lockfile to prevent 2 client for write at same time~~

WIP :
    
TODO :
  - Print all users after connexion


## Server

SOLVED :  
  ~~- Change client memory management : change from struct to linked list~~
  ~~- Suicide if empty (last client disco)~~

WIP :

TODO :
  - Make it a daemon
  

## Both

WIP :
  - Each sys call can return -1 and need 2 be tested.
  - Add commands (i.e nickname, dest (pid or nick))

TODO :
  - Dynamic messge size
  - Add channels

if (on a le temps) : TOTO :
  - TCP/IP implement


# Command protocol
  - Identifier : '/'
  - Command list :
    - /who "List of connected users"
    - /nick "Change my nickname to another free"
