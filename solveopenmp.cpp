#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
using namespace std;

int main() {
    int n;
    double error;
    cin>>n;
    cin>>error;
    vector<double> X(n,0);
    vector<vector<double>> Soln(n,vector<double>(n+1));
    for(int i = 0; i <n ;i++) {
        cin>>X[i];
    }

    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n+1; j++) {
            cin>>Soln[i][j];
        }
    }


    vector<double> new_X(n,0);
    auto start = omp_get_wtime();
    while(true) {

        vector<int> flags(n,0);
        #pragma omp parallel for num_threads(1) shared(flags, X, Soln, new_X)
        for(int i = 0; i < n; i++) {
            auto c = Soln[i][n];
            double denom = 0;

            for(int j = 0; j < n; j++) {
                if(i != j) {
                    auto a = Soln[i][j];
                    denom += a * X[j];
                }
            }

            new_X[i] = (c - denom) / Soln[i][i];
            double new_err = abs(new_X[i] - X[i]) / new_X[i];
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

    auto end = omp_get_wtime();
    cout<<"Time taken: "<< end - start <<endl;

    for(int i = 0; i < n; i++) {
        cout<<X[i]<<endl;
        float ans = 0;
        for(int j = 0; j < n; j++) {
            ans += Soln[i][j] * X[j];
        }

        // cout<<"Calculated: "<<ans<<" Real: "<<Soln[i][n]<<endl;
    }


}

