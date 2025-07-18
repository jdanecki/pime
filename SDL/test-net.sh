#!/bin/bash
#

for i in `seq 1 10`
do
    ./build/pime_SDL 192.168.0.67  & 
    pid=$!
    sleep 5
    kill $pid
done



