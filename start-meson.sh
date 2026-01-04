#!/bin/bash
pushd build/server
xterm -e bash -c ./server_pime &
popd
sleep 1
pushd build/SDL
ln -s ../../SDL/textures
ln -s ../../SDL/nerdfont.otf
./pime_SDL
