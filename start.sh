#!/bin/bash

cd server/
cargo run &
server_PID=$!
sleep 5
cd ../SDL
./build/pime_SDL 127.0.0.1
kill $server_PID
