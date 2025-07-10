#!/bin/bash

cd server/
cargo run &
server_PID=$!
sleep 5
cd ../SDL
./build/pime_SDL
kill $server_PID
