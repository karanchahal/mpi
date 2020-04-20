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
void get_numbers(double* addr, string line) {
    stringstream ss;
    int i = 0;
    double found;
    ss << line;
    string temp;
    while (!ss.eof()) {
        ss >> temp; 
        if (stringstream(temp) >> found)  {
            addr[i] = found;
            i++;
        }

        /* To save from space at the end of string */
        temp = "";
    }
}

void readinFile(double* X, double** Soln, double &n, double &error, string filename) {
    ifstream myfile (filename);
    string line;
    int i = 0;
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {   
            if(i == 0) {
                n = stod(line);
                X = (double*) malloc(sizeof(double)*n);
                Soln = (double**) malloc(sizeof(double*)*n); // 3 doubles extra space 
            } else if(i == 1) {
                error = stod(line);
            } else if(i == 2) {
                get_numbers(X, line);
            } else {
                // cout<<line<<endl;
                int offset = (n+3)*(i-3);
                double* addr = (double *)malloc(sizeof(double)*(n+1));
                get_numbers(addr, line);
                Soln[i-3] = addr;
            }
            cout<<i<<endl;
            i++;
        }
        myfile.close();
    } else {
        cout<<"Unable to open file !"<<endl;
    }
}



// int alternate(double* &X, double* &Soln, double &n, double &error, const char* filename) {
//     FILE *fp;
   
//     fp = fopen(filename, "r");
 
//     if(fp == NULL)
//     {
//         printf("Error opening file\n");
//         exit(1);
//     }
 
//     fscanf(fp, "%lf", &n);
//     fscanf(fp, "%lf", &error);
//     cout<<n<<endl;
//     cout<<error<<endl;
//     X = (double *)malloc(sizeof(double)*(n));
//     // get X
//     for(int i =0; i < n ; i++) {
//         fscanf(fp, "%lf", &X[i]);
//     }

//     Soln = (double*) malloc(sizeof(double)*n);
//     int count = 0;
//     for(int i = 0; i < n; i++) {
//         double* temp = (double*) malloc(sizeof(double)*(n+1));
//         Soln[i] = temp;
//         for(int j = 0; j < n+1; j++) {
//             fscanf(fp, "%lf", &temp[j]);
//         }
//     }

//     // for(int i = 0; i < n; i ++) {
//     //     for(int j = 0; j < n+1; j ++) {
//     //         cout<<Soln[i][j]<<" ";
//     //     }
//     //     cout<<endl;
//     // }

//     fclose(fp);
//     return 0;
// }


int alternate2(double* &X, double* &Soln, double &n, double &error, const char* filename) {
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

    Soln = (double*) malloc(sizeof(double)*n*(n+1));
    int count = 0;
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n+1; j++) {
            int addr = i*(n+1) + j;
            fscanf(fp, "%lf", &Soln[addr]);
        }
    }

    // for(int i = 0; i < n; i ++) {
    //     for(int j = 0; j < n+1; j ++) {
    //         cout<<Soln[i][j]<<" ";
    //     }
    //     cout<<endl;
    // }

    fclose(fp);
    return 0;
}


void pureOpenMP(double* X, double* Soln, int n , float error) {

    double* new_X = (double*) malloc(sizeof(double)*n);
    int flags[n];

    auto start = omp_get_wtime();
    int n_th = 1;
    while(true) {

        for(int i = 0; i < n; i++)  flags[i] = 0;

        #pragma omp parallel for num_threads(n_th) shared(flags, X, Soln, new_X)
        for(int i = 0; i < n; i++) {
            double c = Soln[i*(n+1) + n];
            double denom = 0;

            for(int j = 0; j < n; j++) {
                if(i != j) {
                    double a = Soln[i*(n+1) + j];
                    denom += a * X[j];
                }
            }

            new_X[i] = (c - denom) / Soln[i*(n+1) + i];
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

        for(int i = 0; i < n; i++) {
            X[i] = new_X[i];
        }

        if(stop == true) {
            break;
        }
    }
    // Print X
    for(int i = 0; i < n; i++) {
        cout<<X[i]<<endl;
    }
}

int main(int argc, char *argv[]) {
    int comm_sz, my_rank;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    char* filename = argv[1];

    if(my_rank == 0) {

        double n, error;
        double* X;
        double* Soln;
        // readinFile(X, Soln, n, error, "100.txt");
        alternate2(X, Soln, n, error, filename);

        if(comm_sz == 1) {
            // Pure OpenMP
            pureOpenMP(X, Soln, n, error);
            return 0;
        }

        // OpenMPI Master
        int blck_size = n / (comm_sz-1);
        int rem = ((int)n) % (comm_sz - 1);
        double helper_vars[2];
        helper_vars[0] = n;
        helper_vars[1] = error;

        // send info for other processes.
        for(int i = 1; i < comm_sz; i++) {
            MPI_Send(&helper_vars, 2, MPI_DOUBLE, i, SEND_N, MPI_COMM_WORLD);
        }

        // Send Matrix to other processes.
        for(int i = 1; i < comm_sz; i++) {
            int offset = (i-1)*blck_size*(n+1);
            double* addr = Soln + offset;
            int sz = blck_size;
            if(i == comm_sz - 1) {
                sz += rem;
            }
            sz = sz*(n+1);
            MPI_Send(addr, sz, MPI_DOUBLE, i, SEND_SOLN, MPI_COMM_WORLD);
        }

        int final = 0;
        while(true) {
            X[(int)n] = final;

            for(int i = 1; i < comm_sz; i++) {
                MPI_Send(X, n+1, MPI_DOUBLE, i, SEND_X, MPI_COMM_WORLD);
            }

            if(final == 1) {
                break;
            }

            final = 1;
            for(int i = 1; i < comm_sz; i++) {
                int flag;
                int sz = blck_size;
                if(i == comm_sz - 1) {
                    sz += rem;
                }
                double* new_X = (double*) malloc(sizeof(double)*(sz+1));
                MPI_Status *status;
                MPI_Recv(new_X, sz+1, MPI_DOUBLE, i, SEND_FLAG, MPI_COMM_WORLD, status);
                flag = new_X[sz];
                // update X
                for(int j = 0; j < sz; j++) {
                    X[(i-1)*blck_size+j] = new_X[j];
                }

                if(flag == 1) {
                    final = 0;
                }
            }
            
        }

        for(int i = 0; i < n; i++) {
            cout<<X[i]<<endl;
        }

    } else {
        // OpenMPI Slave
        double helper_vars[2];
        MPI_Recv(&helper_vars, 2, MPI_DOUBLE, 0, SEND_N, MPI_COMM_WORLD, NULL);
        double n = helper_vars[0];
        double error = helper_vars[1];

        int rem = ((int)n) % (comm_sz-1);
        int blck_size = n / (comm_sz-1);
        int orig_blck_sz = n / (comm_sz-1);

        if(my_rank == comm_sz - 1) {
            blck_size += rem;
        }
        int offset = (my_rank-1)*orig_blck_sz;

        // cout<<offset<< " "<<blck_size<<" "<<orig_blck_sz<<endl;

        double *Soln = (double *)malloc(sizeof(double)*blck_size*(n+1));
        MPI_Recv(Soln, blck_size*(n+1), MPI_DOUBLE, 0, SEND_SOLN, MPI_COMM_WORLD, NULL);

        double* X = (double* ) malloc(sizeof(double)*(n+1));
        double* new_X = (double*) malloc(sizeof(double)*(blck_size+1));
        
        // for(int i = 0; i < blck_size; i++) {
        //     for(int j = 0; j < n+1; j++) {
        //          cout<<Soln[i*((int)n+1) + j]<<" ";
        //     }
        //     cout<<endl;
        // }
        while(true) {
            MPI_Recv(X, n+1, MPI_DOUBLE, 0, SEND_X, MPI_COMM_WORLD, NULL);

            if(X[(int)n] == 1) {
                break;
            }

            int flags[blck_size];
            for(int i = 0; i < blck_size; i++) flags[i] = 0;
            int n_th = 1;
            #pragma omp parallel for num_threads(n_th)
            for(int i = 0; i < blck_size; i++) {
                auto c = Soln[i*((int)n+1) + (int)n];
                double denom = 0;

                for(int j = 0; j < n; j++) {
                    if(i + offset != j) {
                        auto a = Soln[i*((int)n+1) + j];
                        denom += a * X[j];
                    }
                }

                new_X[i] = (c - denom) / Soln[i*((int)n+1) + i + offset];
                double new_err = abs(new_X[i] - X[offset + i]) / new_X[i];
                if(new_err > error) {
                    flags[i] = 1;
                }
            }

            int flag = 0;
            for(int i = 0; i < blck_size; i++) {
                if(flags[i] == 1) {
                    flag = 1;
                    break;
                }
            }

            new_X[(int)blck_size] = flag;
            MPI_Send(new_X, blck_size + 1, MPI_DOUBLE, 0, SEND_FLAG, MPI_COMM_WORLD);
        }
    }
}