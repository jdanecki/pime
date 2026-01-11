#!/bin/bash

find . -name *.cpp -o -name *.h  -exec wc -l {} \; | awk '{s+=$$1} END { print s} '
