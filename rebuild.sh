#!/bin/bash

rm -rf build
export CC="ccache clang"
export CXX="ccache clang++"
#export CC="ccache gcc"
#export CXX="ccache g++"

meson setup  build .
#meson setup -Dtests=false build .
meson compile -C build

