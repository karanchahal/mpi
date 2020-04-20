openmp:
	g++ -fopenmp -o build/open openmp.cpp
	./build/open
hw1:
	g++ --std=c++11 -fopenmp -o build/open tsm.cpp
	./build/open
hw1p:
	g++ --std=c++11 -fopenmp -o build/open tsmparallel.cpp
	./build/open
seq1:
	g++ --std=c++11 -fopenmp -o build/open seq.cpp
	./build/open
lab2:
	g++ --std=c++11 -fopenmp -o build/open solveopenmp.cpp
	./build/open < 1000.txt
read:
	mpicc --std=c++11 -fopenmp -o build/open readFile.cpp
	mpiexec -n 10 build/open 1000.txt
