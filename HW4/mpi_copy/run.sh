#!/bin/bash

make clean; make; echo
for ((i = 1; i <= 16; i++)); do
    (set -x; mpiexec -np $i ./mpi)
    echo
done

(set -x; diff output result-2d)

