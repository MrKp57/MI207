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
    
    
# How to use

- make release to compile sources in release mode, binary files on bin/release/client and bin/release/server
- make debug to compile sources in debug mode, binary files on bin/debug/client and bin/debug/server
- make force to force rebuild from sources for both debug and release.
- make clean to clear bin and obj files.

- ./kill_server.sh used to kill daemon server running and cleaning log files and pipe folders.

## server
### the output of server is for information purpose only

- server can be launched with "dem" argument : 'bin/release/server dem', log file at "/tmp/chat.log/srv_dem.log"
  - log file name can be provided as optional argument : log file on folder "/tmp/chat.log/"
  
## client
- no args 
### Command protocol
  - Command list :
    - /who : "List of connected users : nickname and pid"
    - /nick $nickname : "Change my nickname"
    - /msg $pid $message : "To send message to specific pid"
    
## both 
- if client is started with no server running, one server is started in background as daemon.
- server stop when the last client leave.
- dynamic message size, dynamic client list
