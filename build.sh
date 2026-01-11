#!/bin/bash

rm -rf build
meson setup  build .
#meson setup -Dtests=false build .
meson compile -C build

