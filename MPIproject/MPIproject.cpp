// MPIproject.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include "mpi.h"

using namespace std;

#define SEND_PROCESS 0
#define RECV_PROCESS 1
#define TAG 0


int SendProcess(int data, int world_size, int process)
{
    MPI_Send(&data, 1, MPI_INT, (process + 1) % world_size, TAG, MPI_COMM_WORLD);

    return 0;
}

int RecvProcess(int world_size, int process)
{
    //get length
    int buf;
    int source = (process - 1);
    source = source >= 0 ? source : world_size - 1;
    MPI_Status status;

    MPI_Recv(&buf, 1, MPI_INT, source, TAG, MPI_COMM_WORLD, &status);

    return buf;
}


int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int world_size = -1;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int process = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &process);

    cout << "World size: " << world_size << ", process number: " << process << endl;

    if (world_size < 1) {
        return MPI_Finalize();
    }

    int count = 0;

    if (process == SEND_PROCESS) 
    {
        int msg = 0;
        SendProcess(msg, world_size, process);
    }


    while (count < 2) 
    {
        int msg = RecvProcess(world_size, process);
        cout << "process number - " << process << " says: " << msg << endl;
        msg++;
        if (count == 1 && process == SEND_PROCESS) 
        {
            break;
        }
        SendProcess(msg, world_size, process);

        
        //последнюю отправку никто не принимает
        count++;

        
    }


    return MPI_Finalize();
}
    /* Первая задача
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

*/