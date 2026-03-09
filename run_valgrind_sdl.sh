#!/bin/bash
cd build/SDL
valgrind --track-origins=yes -- ./pime_SDL 127.0.0.1
