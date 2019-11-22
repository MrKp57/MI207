#!/bin/bash
pid_du_process=$(ps -eo pid,cmd,tty,args | grep debug/server | awk '{print $1}' | head -n 1)
echo "$pid_du_process"
kill -s SIGINT $pid_du_process
rm -rf /tmp/chat /tmp/chat.log