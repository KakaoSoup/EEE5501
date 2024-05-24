#!/bin/bash

system="$(uname -s)"

make clean
make

if [[ $system == "Darwin" ]]; then
    leaks --atExit -- ./vector
elif [[ $system == "Linux" ]]; then
    valgrind ./vector
fi

make clean
