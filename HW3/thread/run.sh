#!/bin/bash

make clean; make; echo
for ((i = 1; i <= 64; i *= 2)); do
    (set -x; ./thread $i)
    echo
done
make clean; ls
