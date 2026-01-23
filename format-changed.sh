#!/bin/bash

for f in `git diff --name-only -- '*.h' '*.cpp' '*.inl'` 
do
    echo $f
    clang-format -i $f
done    

for f in `git diff --cached --name-only -- '*.h' '*.cpp' '*.inl' `
do
    echo $f
    clang-format -i $f
done    

