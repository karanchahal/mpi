#include <iostream>
#include <vector>
#include <mpi.h>
#include <stdlib.h>
#include<stdio.h>
using namespace std;


int SEND_X = 5;
int SEND_SOLN = 10;
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

    int blck_size = n / (comm_sz-1);

    if(my_rank == 0) {
        double* X;
        double* new_X;
        double** Soln;
        X = (double *)malloc(sizeof(double)*n);
        Soln = (double **)malloc(sizeof(double*)*n);
        for(int i = 0; i < n; i++) {
            Soln[i] = (double *)malloc(sizeof(double)*(n+1));
        }

        // Soln.resize(n,vector<float>(n+1));
        for(int i = 0; i <n ;i++) {
            cin>>X[i];
        }

        for(int i = 0; i < n; i++) {
            for(int j = 0; j < n+1; j++) {
                cin>>Soln[i][j];
            }
        }

        new_X = (double *)malloc(sizeof(double)*n);
        int offset = 0;
        
        for(int i = 1; i < comm_sz; i++) {
            // cout<<offset<<endl;
            double* arr_to_send = X + offset;
            cout<<*arr_to_send<<endl;
            // cout<<arr_to_send<<endl;
            MPI_Send(X, 1, MPI_DOUBLE, i, SEND_X, MPI_COMM_WORLD);
            offset += blck_size;
        } 
        
        // int* arr = (int*)malloc(sizeof(int));
        // arr[0] = 5;

        // for(int i = 1; i < comm_sz; i++) {
        //     MPI_Send(arr, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        // }



    } else {
        double* X = (double* ) malloc(sizeof(double)*n);
        MPI_Status *status;
        MPI_Recv(X, 1, MPI_DOUBLE, 0, SEND_X, MPI_COMM_WORLD, status);
        // for(int i = 0; i < blck_size; i++) {
        //     cout<<X[i]<<" ";
        // }
        // cout<<endl;
        // cout<<arr[0]<<" ";
        // cout<<endl;
    }

    return 0;
}