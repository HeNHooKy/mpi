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
    MPI_Comm_size(*decartCircle, &groupSize); //WHY decartCircle's NULL?
    cout << "count of procees in decart: " << groupSize << endl;

    //MPI_Comm_size(*masterSlave, &groupSize);
    cout << "count of process in masterSlave: " << groupSize << endl;
}

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int world_size = -1;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int process = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &process);

    cout << "World size: " << world_size << ", process number: " <<  process << endl;

    MPI_Comm decartCircle;
    MPI_Comm masterSlave;

    createComms(&decartCircle, &masterSlave);


    


    

    

    
    return MPI_Finalize();
}

