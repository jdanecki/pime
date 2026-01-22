#!/bin/bash

sudo sysctl -w kernel.yama.ptrace_scope=0

cd build/server
xterm -e ./server_pime &
sleep 1
cgdb -p `pidof server_pime`

