#!/bin/bash

pushd server
cargo run &
server_PID=$!
sleep 5
pushd ../SDL
./run_local.sh
kill $server_PID
