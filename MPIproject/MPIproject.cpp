// MPIproject.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "mpi.h"
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;


#define SEND_PROCESS 0
#define RECV_PROCESS 1
#define TAG 0
#define OP_LONG 10000000

milliseconds GetCurrentTime() {
    return duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
    );
}

int SendProcess()
{
    cout << "Start sending!" << endl;

    char* data = (char*) "Hello, I'm sending process!";
    int size = strlen(data) + 1;


    for (int i = 0; i < OP_LONG; i++) {
        MPI_Send(data, size, MPI_CHAR, RECV_PROCESS, TAG, MPI_COMM_WORLD);
    }
    

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

    milliseconds start = GetCurrentTime();
    //get message
    for (int i = 0; i < OP_LONG; i++) {
        MPI_Recv(data, size, MPI_CHAR, SEND_PROCESS, TAG, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
    }

    milliseconds end = GetCurrentTime();
    milliseconds dif = end - start;

    double realDif = (double)dif.count() / (double)OP_LONG;

    cout << "Time spent on the send operation (ms): " << realDif << endl;
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

