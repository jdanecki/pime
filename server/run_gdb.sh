#!/bin/bash
#
export LIBSERVER=$(find . -name libserver_cpp.so)
export LIBPIME=$(find . -name libpime.so)
echo "LIBSERVER=$LIBSERVER"
echo "LIBPIME=$LIBPIME"
SERVER_PATH=$(dirname $LIBSERVER)
PIME_PATH=$(dirname $LIBPIME)
echo "SERVER_PATH=$SERVER_PATH"
echo "PIME_PATH=$PIME_PATH"
export LD_LIBRARY_PATH=$SERVER_PATH:$PIME_PATH
echo "LIBRARY_PATH=$LD_LIBRARY_PATH"

cgdb target/debug/server


