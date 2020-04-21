#!/bin/bash
#
#SBATCH --job-name=classification
#SBATCH --nodes=10
#SBATCH --cpus-per-task=4
#SBATCH --time=24:00:00
#SBATCH --mem=32GB

. ~/.bashrc
cd /scratch/ksc487/openmp/lab2
mpiexec -n 10 ./build/open 100000.txt 1
mpiexec -n 10 ./build/open 100000.txt 2
mpiexec -n 10 ./build/open 100000.txt 3
mpiexec -n 10 ./build/open 100000.txt 4
mpiexec -n 10 ./build/open 100000.txt 5
mpiexec -n 10 ./build/open 100000.txt 6
mpiexec -n 10 ./build/open 100000.txt 7
mpiexec -n 10 ./build/open 100000.txt 8
mpiexec -n 10 ./build/open 100000.txt 9
mpiexec -n 10 ./build/open 100000.txt 10
