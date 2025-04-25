#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <stdlib.h> // For exit()


const int MAX_STRING = 100;

int main(void) {
    printf("SUPPPPP\n");
    int MAX_NUM = 8; //matrix width
    double X[MAX_NUM][MAX_NUM]; //The matrix to divide up

    int comm_sz; //Number of processes
    int my_rank; //My process rank

    MPI_Init(NULL,NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    int slab = MAX_NUM / comm_sz;
    double x[slab+2][MAX_NUM]; //add two for ghost top&bottom
    int numEls = slab * MAX_NUM;

    // According to assign instr, throw an error if:
    if (MAX_NUM % comm_sz != 0) {
        printf("Error: Matrix (%d,%d) must be evenly divisible by workers\n", MAX_NUM, MAX_NUM);
        exit(-1);
    }

    if (my_rank == 0) {
        //Divide Matrix Up Bt Workers
        //fill array as test
        for (int i =0; i < MAX_NUM; i++) {
            for (int j=0; j < MAX_NUM; j++) {
                X[i][j] = -1.0;
            }
        }

        for (int i =0; i < MAX_NUM; i++) {
            for (int j=0; j < MAX_NUM; j++) {
                printf("[%d-%d]",i,j);
            }
        }

        for (int processID = 1; processID < comm_sz; processID++) {
            int firstrow = my_rank * slab;     //ex firstrow = 0 means X[0][j]
            int lastrow = firstrow + slab - 1;
            int numEls = slab * MAX_NUM;
            MPI_Send(&X[firstrow][0], numEls, MPI_DOUBLE, processID, 0, MPI_COMM_WORLD);
        }
        //fill array with -1 to start off with
        for (int i = 0; i < slab+2; i++) {
            for (int j = 0; j< MAX_NUM; j++) {
                x[i][j] = -1.0;
            } 
        }
        //fill my slab (not ghost points) with my rank
        for (int i = 1; i < slab+1; i++) {
            for (int j = 0; j < MAX_NUM; j++) {
                x[i][j] = my_rank;
            }
        }

    }
    else {
        //workers should receive their pieces from process 0
        //fill array with -1 to start off with
        for (int i = 0; i < slab+2; i++) {
            for (int j = 0; j< MAX_NUM; j++) {
                x[i][j] = -1.0;
            } 
        }
        //start copying to 2nd row of allocated space
        MPI_Recv(x[1], numEls, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Received slab from process 0\n");
        //fill my slab (not ghost points) with my rank
        for (int i = 1; i < slab+1; i++) {
            for (int j = 0; j < MAX_NUM; j++) {
                x[i][j] = my_rank;
            }
        }
        // //print my full x matrix
        // for (int i = 0; i < slab+2; i++) {
        //     for (int j= 0; j < MAX_NUM; j++) {
        //         printf("%d:[%e]", my_rank, x[i][j] );
        //     }
        // }
    }

    //Now send top row of your slab to preceding neighbor,
    //// except for process 0 ( as it has no preceding neighbor)
    if (my_rank != 0) {
        MPI_Send(x[1], MAX_NUM, MPI_DOUBLE, my_rank-1, 0, MPI_COMM_WORLD);
    }
    //Recieve your bottom ghost row,unless you are numWorkers-1 process
    if (my_rank != comm_sz - 1) {
        MPI_Recv(x[slab+1], MAX_NUM, MPI_DOUBLE, my_rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    //Now send your bottom row of your slab to the succeeding neighbor
    /// except for process numWorkers - 1 (as it has no succeeding neighbor)
    if (my_rank != comm_sz -1) {
        MPI_Send(x[slab], MAX_NUM, MPI_DOUBLE, my_rank+1, 1, MPI_COMM_WORLD);
    }
    if (my_rank != 0) {
        MPI_Recv(x[0], MAX_NUM, MPI_DOUBLE, my_rank-1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    printf("---");
    //print my full x matrix to check 
    for (int i = 0; i < slab+2; i++) {
        for (int j= 0; j < MAX_NUM; j++) {
            printf("%d:[%e]", my_rank, x[i][j] );
        }
    }

    char greeting[MAX_STRING];

    if(my_rank != 0) {
        sprintf(greeting, "Greetings from process %d of %d!", my_rank, comm_sz);
        MPI_Send(greeting, strlen(greeting)+1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }
    else {
        printf("Greetings from process %d of %d!\n", my_rank, comm_sz);
        for (int q=1; q < comm_sz; q++) {
            MPI_Recv(greeting, MAX_STRING, MPI_CHAR, q, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("%s\n", greeting);
        }
    }

    MPI_Finalize();
    return 0;
}
