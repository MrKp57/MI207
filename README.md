Projet chat client serveur

## Client

KNOWN BUGS :

SOLVED : 
  ~~- When ctrl-c, if father kill himself first, son cannot remove ppid pipe :/~~
  ~~- Le fork implique des soucis dans les getpid suivants et le ctrl c est capté par le fils et le pere~~
  ~~- Ask client for nick at startup~~
  ~~- Lockfile to prevent 2 client for write at same time~~
  ~~- Print all users after connexion~~

WIP :
    
TODO :


## Server

KNOWN BUGS :
  - Client P2P Message seg fault if size > block size PTNNNNNNN

SOLVED :  
  ~~- Change client memory management : change from struct to linked list~~
  ~~- Suicide if empty (last client disco)~~
  ~~- Make it a daemon~~

WIP :
  - handle commands (/who, /nick)

TODO :
  

## Both

SOLVED :
  ~~- Dynamic messge size~~

WIP :
  - Each sys call can return -1 and need 2 be tested.
  - Add commands (i.e nickname, dest (pid or nick))

TODO :
  - Add channels

if (on a le temps) : TOTO :
  - TCP/IP implement


# Command protocol
  - Identifier : '/'
  - Command list :
    - /who "List of connected users"
    - /nick "Change my nickname to another free"
