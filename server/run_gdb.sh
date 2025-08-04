#!/bin/bash
#
LIBSERVER=$(find . -name libserver_cpp.so)
LIBPIME=$(find . -name libpime.so)
echo $LIBSERVER
echo $LIBPIME
SERVER_PATH=$(dirname $LIBSERVER)
PIME_PATH=$(dirname $LIBPIME)
echo $SERVER_PATH
echo $PIME_PATH
export LD_LIBRARY_PATH=$SERVER_PATH:$PIME_PATH
cgdb target/debug/server


