#include <iostream>
#include <vector>
#include <mpi.h>
#include <stdlib.h>
#include<stdio.h>
#include <cmath>
using namespace std;
int SEND_N = 5;
int SEND_X = 10;
int SEND_SOLN = 15;
int SEND_FLAG = 20;
int SEND_QUIT = 25;
int main() {

    int comm_sz;
    int my_rank;
    int n;
    double error;
    cin>>n;
    cin>>error;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int blck_size = n / (comm_sz - 1);

    if(my_rank == 0) {
        
        double helper_vars[3];
        helper_vars[0] = n;
        helper_vars[1] = blck_size;
        helper_vars[2] = error;
        for(int i = 1; i < comm_sz; i++) {
            MPI_Send(&helper_vars, 3, MPI_DOUBLE, i, SEND_N, MPI_COMM_WORLD);
        }

        double* X = (double*)malloc(sizeof(double)*(n+1));
        for(int i = 0; i < n ; i++) {
            cin>>X[i];
        }
        X[n] = 0;

        double Soln[n][n+1];
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < n+1; j++) {
                cin>>Soln[i][j];
            }
        }

        double* addr = &Soln[0][0];
        for(int i = 1; i < comm_sz; i++) {
            int sz = (blck_size)*(n+1);
            MPI_Send(addr, sz, MPI_DOUBLE, i, SEND_SOLN, MPI_COMM_WORLD);
            addr += sz;
        }

        int final = 0;
        double* new_X = (double*) malloc(sizeof(double)*(n+1));
        while(true) {
            X[n] = final;

            for(int i = 1; i < comm_sz; i++) {
                MPI_Send(X, n+1, MPI_DOUBLE, i, SEND_X, MPI_COMM_WORLD);
            }

            if(final == 1) {
                break;
            }


            final = 1;
            for(int i = 1; i < comm_sz; i++) {
                int flag;
                double* new_X = (double*) malloc(sizeof(double)*(blck_size+1));
                MPI_Status *status;
                MPI_Recv(new_X, blck_size+1, MPI_DOUBLE, i, SEND_FLAG, MPI_COMM_WORLD, status);
                flag = new_X[blck_size];
                // update X
                for(int j = 0; j < blck_size; j++) {
                    X[(i-1)*blck_size+j] = new_X[j];
                }

                if(flag == 1) {
                    final = 0;
                }
            }

        }

        for(int i = 0; i < n; i++) {
            cout<<X[i]<<" ";
        }
        cout<<endl;
    } else {

        double helper_vars[3];
        MPI_Status *status;
        MPI_Recv(&helper_vars, 3, MPI_DOUBLE, 0, SEND_N, MPI_COMM_WORLD, status);
        int N = helper_vars[0];
        int blck_size = helper_vars[1];
        double error = helper_vars[2];
        int offset = (my_rank-1)*blck_size;        
        double Soln[blck_size][N+1];
        int total_size = blck_size*(N+1);
        double* addr = &Soln[0][0];
        MPI_Recv(addr, total_size, MPI_DOUBLE, 0, SEND_SOLN, MPI_COMM_WORLD, status);
        double* X = (double* ) malloc(sizeof(double)*(N+1));
        double* new_X = (double*) malloc(sizeof(double)*(blck_size+1));
        while(true) {
            MPI_Recv(X, N+1, MPI_DOUBLE, 0, SEND_X, MPI_COMM_WORLD, status);

            if(X[N] == 1) {
                break;
            }

            int flag = 0;
            for(int i = 0; i < blck_size; i++) {
                auto c = Soln[i][N];
                double denom = 0;

                for(int j = 0; j < N; j++) {
                    if(i + offset != j) {
                        auto a = Soln[i][j];
                        denom += a * X[j];
                    }
                }

                new_X[i] = (c - denom) / Soln[i][i + offset];
                double new_err = abs(new_X[i] - X[offset + i]) / new_X[i];
                if(new_err > error) {
                    flag = 1;
                }
            }

            new_X[blck_size] = flag;
            MPI_Send(new_X, blck_size + 1, MPI_DOUBLE, 0, SEND_FLAG, MPI_COMM_WORLD);
        }
        
    }

    return 0;
}