// MPIproject.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "mpi.h"
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <chrono>

using namespace std;

#define TAG 0
#define ROOT_PROCESS 0
#define OP_LONG 10000

void My_Gather(int* sendarray, int sendcount, MPI_Datatype sendtype, int* recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    int process = -1;
    MPI_Comm_rank(comm, &process);

    if (process == -1) {
        throw "error: process doesn't exist";
    }

    if (process != root) {
        MPI_Send(sendarray, sendcount, sendtype, root, TAG, comm);
    }


    if (process == root) {

        int comm_size = -1;
        MPI_Comm_size(comm, &comm_size);

        if (process == -1) {
            throw "error: can't find world size";
        }

        

        for (int i = 0; i < comm_size; i++) {
            if (i != root) {
                MPI_Status status;
                int* loc_rbuf = (int*)malloc(recvcount * sizeof(int));
                MPI_Recv(loc_rbuf, recvcount, recvtype, i, TAG, comm, &status);

                for (int j = 0; j < recvcount; j++) {
                    int k = i * recvcount + j;
                    recvbuf[k] = loc_rbuf[j];
;               }

                //MPI_Probe(i, TAG, comm, &status);
            }
            else {
                for (int j = 0; j < recvcount; j++) {
                    int k = i * recvcount + j;
                    recvbuf[k] = sendarray[j];
                }
            }
        }
    }
}


int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int world_size = -1;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int process = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &process);

    cout << "World size: " << world_size << ", process number: " <<  process << endl;
    int sendarray[2]{ process, -1 };
    int root = ROOT_PROCESS, * rbuf;
    rbuf = (int*)malloc(world_size * 2 * sizeof(int));

    
    double start = MPI_Wtime();
    for (int i = 0; i < OP_LONG; i++) {
        MPI_Gather(sendarray, 2, MPI_INT, rbuf, 2, MPI_INT, root, MPI_COMM_WORLD);
    }
    double end = MPI_Wtime();
    double dif = end - start;

    if (process == root) {
        double realDif = (double)dif * 1000 / (double)OP_LONG;
        cout << "Time spent for MPI_Gather operation (ms): " << realDif << endl;
    }
    
    
    start = MPI_Wtime();
    for (int i = 0; i < OP_LONG; i++) {
        My_Gather(sendarray, 2, MPI_INT, rbuf, 2, MPI_INT, root, MPI_COMM_WORLD);
    }
    end = MPI_Wtime();
    dif = end - start;

    if (process == root) {
        double realDif = (double)dif * 1000 / (double)OP_LONG;
        cout << "Time spent for My_Gather operation (ms): " << realDif << endl;
    }

    
    if (process == ROOT_PROCESS) {
        for (int i = 0; i < 2 * world_size; i++) {
            cout << rbuf[i] << endl;
        }
    }
    
    

    return MPI_Finalize();;
}

