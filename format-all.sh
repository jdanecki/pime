#!/bin/bash

for f in `find -name '*.h'`
do
    echo $f
    clang-format -i $f
done

for f in `find \( -path "./the_game_net/target" -o -path "./server/target" -o -path "./SDL/build" \) -prune -o -name '*.cpp'`
do
    if [ -f $f ]
    then
      echo $f
      clang-format -i $f
    fi
done
