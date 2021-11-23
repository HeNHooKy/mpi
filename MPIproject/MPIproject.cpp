// MPIproject.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "mpi.h"
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <chrono>

using namespace std;


#define SEND_PROCESS 0
#define RECV_PROCESS 1
#define TAG 0
#define OP_LONG 100000


int SendProcess()
{
    cout << "Start sending!" << endl;

    char* data = (char*) "Hello, I'm sending process!";
    int size = strlen(data) + 1;

    double start = MPI_Wtime();

    MPI_Request* request = (MPI_Request*) malloc(sizeof(MPI_Request) * OP_LONG);
    for (int i = 0; i < OP_LONG; i++) {
        
        MPI_Isend(data, size, MPI_CHAR, RECV_PROCESS, TAG, MPI_COMM_WORLD, &request[i]);
    }

    MPI_Status* statuses = (MPI_Status*)malloc(sizeof(MPI_Status) * OP_LONG);

    MPI_Waitall(OP_LONG, request, statuses);
    
    double end = MPI_Wtime();
    double dif = end - start;

    double realDif = (double)dif * 1000 / (double)OP_LONG;

    cout << "Time spent on the send operation (ms): " << realDif << endl;

    return 0;
}

int RecvProcess() 
{
    cout << "Start receiving!" << endl;
    MPI_Status status;
    
    //get length
    MPI_Probe(SEND_PROCESS, TAG, MPI_COMM_WORLD, &status);
    int size = -1;
    MPI_Get_count(&status, MPI_CHAR, &size);
    char* data = (char*) malloc(size);
    //here we've already known that Send is started

    
    //get message
    for (int i = 0; i < OP_LONG; i++) {
        MPI_Request request;
        MPI_Irecv(data, size, MPI_CHAR, SEND_PROCESS, TAG, MPI_COMM_WORLD, &request);

        MPI_Status status;
        MPI_Wait(&request, &status);
    }

    
    cout << data << endl;

    free(data);

    return 0;
}

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int world_size = -1;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int process = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &process);

    cout << "World size: " << world_size << ", process number: " <<  process << endl;

    

    if (world_size > 1)
    {
        switch (process)
        {
        case SEND_PROCESS:
            SendProcess();
            break;
        case RECV_PROCESS:
            RecvProcess();
            break;
        default:
            cout << "Process number: #" << process << " doesn't have a task!" << endl;
            break;
        }
    }

    return MPI_Finalize();;
}

