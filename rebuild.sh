#!/bin/bash

rm -rf build
export CC=clang
export CXX=clang++
#export CC=gcc 
#export CXX=g++

meson setup  build .
#meson setup -Dtests=false build .
meson compile -C build

