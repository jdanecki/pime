#!/bin/bash
cd build/ogl
valgrind --track-origins=yes --log-file=ogl_valgrind.txt -- ./pime_ogl 127.0.0.1
