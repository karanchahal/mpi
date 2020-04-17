#include <iostream>
#include <vector>
#include <mpi.h>
using namespace std;

int main() {

    int comm_sz;
    int my_rank;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);


    if(my_rank == 0) {
        vector<int> arr = {1,2,3,4,5,6};
        for(int i = 1; i < comm_sz; i++) {
            MPI_Send((void *) &arr, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    } else {
        int* arr;
        MPI_Status *status;
        cout<<"Process"<<my_rank<<endl;
        MPI_Recv(arr, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, status);

        for(int i = 0; i < 2; i++) {
            cout<<arr[i]<<" ";
        }
        cout<<endl;
    }

    return 0;
}