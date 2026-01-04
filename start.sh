#!/bin/bash

pushd server/build
xterm -e bash -c ./server_pime &
popd
sleep 1
pushd SDL
./run_local.sh
