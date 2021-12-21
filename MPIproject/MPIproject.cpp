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
#define OP_LONG 1000

int notEmpty(int* a, int n) {
    int real = 0;
    for (int i = 0; i < n; i++) {
        if (a[i] != -1) {
            real++;
        }
    }

    return real;
}

void removeEmpty(int* a, int n, int real, int* new_a) {
    int k = 0;
    for (int i = 0; i < n; i++) {
        if (a[i] != -1) {
            new_a[k++] = a[i];
        }
    }
}

bool isSimple(int n) {
    bool answer = true;

    if (n < 2) {
        answer = false;
    }

    for (int i = 2; i < n; i++) {
        if (n % i == 0) {
            answer = false;
            break;
        }
    }

    return answer;
}

void print(int* a, int n) {
    for (int i = 0; i < n; i++) {
        cout << a[i] << " ";
    }
    cout << endl;
}

void createComms(MPI_Comm * decartCircle, MPI_Comm * masterSlave) {

    MPI_Group worldGroup;
    MPI_Group decartGroup;
    MPI_Group masterSlaveGroup;

    MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);

    int world_size = -1;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int* decartRanks = (int*)malloc(sizeof(int) * world_size);
    int* masterSlaveRanks = (int*)malloc(sizeof(int) * world_size);

    for (int i = 0; i < world_size; i++) {
        decartRanks[i] = -1;
        masterSlaveRanks[i] = -1;
    }

    for (int i = 0; i < world_size; i++) {
        if (isSimple(i)) {
            decartRanks[i] = i;
        }
        else {
            masterSlaveRanks[i] = i;
        }
    }

    int ms_n_real = notEmpty(masterSlaveRanks, world_size);
    int* masterSlaveRealRanks = (int*)malloc(sizeof(int) * ms_n_real);
    

    int d_n_real = notEmpty(decartRanks, world_size);
    int* decartRealRanks = (int*)malloc(sizeof(int) * d_n_real);

    removeEmpty(masterSlaveRanks, world_size, ms_n_real, masterSlaveRealRanks);
    removeEmpty(decartRanks, world_size, d_n_real, decartRealRanks);
    

    MPI_Group_incl(worldGroup, ms_n_real, masterSlaveRealRanks, &masterSlaveGroup);
    MPI_Group_incl(worldGroup, d_n_real, decartRealRanks, &decartGroup);

    


    if (masterSlaveGroup == MPI_GROUP_EMPTY || decartGroup == MPI_GROUP_EMPTY)
    {
        printf("A group created is empty.\n");
        MPI_Finalize();
    }

    MPI_Comm_create(MPI_COMM_WORLD, decartGroup, decartCircle);
    MPI_Comm_create(MPI_COMM_WORLD, masterSlaveGroup, masterSlave);

    int groupSize;

    int process = -1;
    

    if (*decartCircle != MPI_COMM_NULL) {
        MPI_Comm_rank(*decartCircle, &process);

        if (process == ROOT_PROCESS) {
            MPI_Comm_size(*decartCircle, &groupSize);
            cout << "count of procees in decart: " << groupSize << endl;
        }
    }

    if (*masterSlave != MPI_COMM_NULL) {
        MPI_Comm_rank(*masterSlave, &process);
        if (process == ROOT_PROCESS) {
            MPI_Comm_size(*masterSlave, &groupSize);
            cout << "count of process in masterSlave: " << groupSize << endl;
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

    if (process == ROOT_PROCESS) {
        cout << "World size: " << world_size << ", process number: " << process << endl;
    }
    

    MPI_Comm decartCircle;
    MPI_Comm masterSlave;

    createComms(&decartCircle, &masterSlave);

    if (decartCircle != MPI_COMM_NULL) {
        int size;
        MPI_Comm_size(decartCircle, &size);

        int dims[1] = { 0 };
        

        MPI_Dims_create(size, 1, dims);

        int periods[1] = { true };

        int reorder = true;

        MPI_Comm decartCircleTopology;
        MPI_Cart_create(decartCircle, 1, dims, periods, reorder, &decartCircleTopology);

        int source, dest;

        MPI_Cart_shift(decartCircleTopology, 0, 1, &source, &dest);


        int my_rank;
        MPI_Comm_rank(decartCircleTopology, &my_rank);
        int buffer_send = my_rank;
        int buffer_recv;

        printf("[Decart topology]:: MPI process %d sends value %d to MPI process %d.\n", my_rank, buffer_send, dest);
        MPI_Sendrecv(&buffer_send, 1, MPI_INT, dest, TAG,
            &buffer_recv, 1, MPI_INT, source, TAG, decartCircleTopology, MPI_STATUS_IGNORE);
        printf("[Decart topology]:: MPI process %d received value %d from MPI process %d.\n", my_rank, buffer_recv, source);
    }

    if (masterSlave != MPI_COMM_NULL) {
        int size;
        MPI_Comm_size(masterSlave, &size);

        int my_rank;
        MPI_Comm_rank(masterSlave, &my_rank);

        const int number_of_nodes = size;

        int edges_length = size * 2 - 2;
        int* indexes = (int*)malloc(sizeof(int) * size);
        int* edges = (int*)malloc(sizeof(int) * edges_length);

        indexes[0] = size - 1;
        for (int i = 1; i < size; i++)
        {
            indexes[i] = size - 1 + i;
        }

        for (int i = 0; i < edges_length; i++)
        {
            edges[i] = 0;
        }
        
        for (int i = 1; i < size; i++) {
            edges[i - 1] = i;
        }

        if (my_rank == ROOT_PROCESS) {
            cout << "indexes: ";
            print(indexes, size);
            cout << "edges: ";
            print(edges, edges_length);
        }
        


        MPI_Comm graphComm;

        MPI_Graph_create(masterSlave, size, indexes, edges, false, &graphComm);


        if (graphComm != MPI_COMM_NULL)
        {
            int my_number_of_neighbours;
            MPI_Graph_neighbors_count(graphComm, my_rank, &my_number_of_neighbours);
            printf("[Master slave]->[MPI process %d] I am part of the graph and have %d neighbours.\n", my_rank, my_number_of_neighbours);


            int* neighbours = (int*)malloc(sizeof(int) * my_number_of_neighbours);
            MPI_Graph_neighbors(graphComm, my_rank, my_number_of_neighbours, neighbours);

            for (int i = 0; i < my_number_of_neighbours; i++)
            {
                int my_rank;
                MPI_Comm_rank(graphComm, &my_rank);
                int buffer_send = my_rank + 1000;
                int buffer_recv;

                printf("[Master slave]:: MPI process %d sends value %d to MPI process %d.\n", my_rank, buffer_send, neighbours[i]);
                MPI_Sendrecv(&buffer_send, 1, MPI_INT, neighbours[i], TAG,
                    &buffer_recv, 1, MPI_INT, neighbours[i], TAG, graphComm, MPI_STATUS_IGNORE);
                printf("[Master slave]:: MPI process %d received value %d from MPI process %d.\n", my_rank, buffer_recv, neighbours[i]);
            }
        }
        else
        {
            printf("[MPI process %d] I am not part of the graph communicator.\n", my_rank);
        }



    }
    


    

    

    
    return MPI_Finalize();
}

