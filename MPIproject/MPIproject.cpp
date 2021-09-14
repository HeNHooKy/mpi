// MPIproject.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "mpi.h"

using namespace std;

#define SEND_PROCESS 0
#define RECV_PROCESS 1
#define TAG 0

int SendProcess()
{
    cout << "Start sending!" << endl;

    char* data = (char*) "Hello, I'm sending process!";
    int size = strlen(data) + 1;

    MPI_Send(data, size, MPI_CHAR, RECV_PROCESS, TAG, MPI_COMM_WORLD);

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

    //get message
    MPI_Recv(data, size, MPI_CHAR, SEND_PROCESS, TAG, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
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

