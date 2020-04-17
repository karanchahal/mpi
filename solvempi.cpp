#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
#include <mpi.h>
using namespace std;


struct Buffer{
    vector<float> X;
    vector<vector<float>> Soln;
    int start, end;

    Buffer(vector<float> x, vector<vector<float>> sol, int n, int m)  {
        X = x;
        Soln = sol;
        n = start;
        m = end;
    }
}
int main() {

    int comm_sz ; // number of processes
    int my_rank; //my process rank
    vector<float> X;
    vector<vector<float>> Soln;
    vector<float> new_X;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int n;
    float error;
    if(my_rank == 0) {
        cin>>n;
        cin>>error;
        X.resize(n,0);
        Soln.resize(n,vector<float>(n+1));
        for(int i = 0; i <n ;i++) {
            cin>>X[i];
        }

        for(int i = 0; i < n; i++) {
            for(int j = 0; j < n+1; j++) {
                cin>>Soln[i][j];
            }
        }
        new_X.resize(n,0);

        // send information to other processes.

        while(true) {
            vector<int> flags(n,0);
            int num_process = 10;
            int block = X.size() / comm_sz;
            int start = 0;
            for(int i = 0; i < num_process; i++) {
                // send 
                start += ;
                MPI_Send(&X, , MPI_DOUBLE);
            }

            for(int i = 0; i < n; i++) {
                flag[i] = recieve(flag);
            }
        }
    } else {
        recieve(soln[i:j], X);
        // just do the same and return results
        
    }



    while(true) {

        vector<int> flags(n,0);
       
        for(int i = 0; i < n; i++) {
            auto c = Soln[i][n];
            float denom = 0;

            for(int j = 0; j < n; j++) {
                if(i != j) {
                    auto a = Soln[i][j];
                    denom += a * X[j];
                }
            }

            new_X[i] = (c - denom) / Soln[i][i];
            float new_err = abs(new_X[i] - X[i]) / new_X[i];
            if(new_err > error) {
                flags[i] = 1;
            }
        }

        bool stop = true;
        for(int i = 0; i < n; i++) {
            if(flags[i] == 1) {
                stop = false;
                break;
            }
        }

        X = new_X;

        if(stop == true) {
            break;
        }
    }


    for(int i = 0; i < n; i++) {
        float ans = 0;
        for(int j = 0; j < n; j++) {
            ans += Soln[i][j] * X[j];
        }

        //cout<<"Calculated: "<<ans<<" Real: "<<Soln[i][n]<<endl;
    }

    MPI_Finalize();


}

