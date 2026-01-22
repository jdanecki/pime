#!/bin/bash

for f in `find -name '*.h'`
do
    echo $f
    clang-format -i $f
done

for f in `find -name '*.cpp'`
do
    if [ -f $f ]
    then
      echo $f
      clang-format -i $f
    fi
done
for f in `find -name '*.inl'`
do
    if [ -f $f ]
    then
      echo $f
      clang-format -i $f
    fi
done
