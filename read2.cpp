#include <iostream>
#include <fstream>
#include <sstream> 
#include <stdio.h>
#include <vector>
#include <mpi.h>
#include <omp.h>
#include <cmath>
using namespace std;

int SEND_N = 1;
int SEND_SOLN = 5;
int SEND_FLAG = 15;
int SEND_X = 20;

int read(double* &X, int* &Soln, double &n, double &error, const char* filename, int n_pr) {
    FILE *fp;
   
    fp = fopen(filename, "r");
 
    if(fp == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }
 
    fscanf(fp, "%lf", &n);
    fscanf(fp, "%lf", &error);
    X = (double *)malloc(sizeof(double)*(n+1));
    // get X
    for(int i =0; i < n ; i++) {
        fscanf(fp, "%lf", &X[i]);
    }

    int count = 0;
    // int n_pr = 100;
    int chunk = n / n_pr;
    cout<<"Size of chunk: "<< chunk<<endl;
    for(int k = 0; k < n_pr; k++) {

       Soln = (int*) malloc(sizeof(int)*chunk*(n+1));
       for(int i = 0; i < chunk; i++) {
           int addr2 = k*chunk + i;
            cout<<"Processed: "<<addr2<<endl;
            for(int j = 0; j < n+1; j++) {
               int addr = i*(n+1) + j;
               fscanf(fp, "%d", &Soln[addr]);
            }
        }

        MPI_Send(Soln, chunk*(n+1), MPI_INT, k+1, 0, MPI_COMM_WORLD);
        delete Soln;
    }

    fclose(fp);
    return 0;
}


int main(int argc, char *argv[]) {
    int comm_sz, my_rank;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    char* filename = argv[1];
    double n, error;
    double* X;
    int* Soln;
    int n_th = atoi(argv[2]);
    // readinFile(X, Soln, n, error, "100.txt");
    if(my_rank == 0) {
        read(X, Soln, n, error, filename, comm_sz-1);
    } else {
        int n = 100000;
        int chunk = 10000;
        int *Soln = (int*) malloc(sizeof(int)*chunk*(n+1));
        MPI_Recv(Soln, chunk*(n+1), MPI_INT, 0, 0, MPI_COMM_WORLD, NULL);
        cout<<"Received by "<<my_rank<<endl;
    }
}
