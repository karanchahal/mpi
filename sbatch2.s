#!/bin/bash
#
#SBATCH --job-name=classification
#SBATCH --nodes=11
#SBATCH --cpus-per-task=4
#SBATCH --time=24:00:00
#SBATCH --mem=32GB

. ~/.bashrc
cd /scratch/ksc487/openmp/lab2
mpiexec -n 11 build/read2 100000.txt 1
#mpiexec -n 2 ./build/open 10000.txt 1
#mpiexec -n 3 ./build/open 10000.txt 1
#mpiexec -n 4 ./build/open 10000.txt 1
#mpiexec -n 5 ./build/open 10000.txt 1
#mpiexec -n 6 ./build/open 10000.txt 1
#mpiexec -n 7 ./build/open 10000.txt 1
#mpiexec -n 8 ./build/open 10000.txt 1
#mpiexec -n 9 ./build/open 10000.txt 1
#mpiexec -n 10 ./build/open 10000.txt 1
