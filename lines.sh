#!/bin/bash

find . \( -name "*.cpp" -o -name "*.h" -o -name "*.inl" \) -print0 \
| xargs -0 awk 'END {print NR}'

