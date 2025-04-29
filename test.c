#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <stdlib.h> // For exit()
#include <unistd.h> //for gethostname()

#define MAX_NUM 8 //matrix width

void fillOrigMatrix(double X[MAX_NUM][MAX_NUM]) {
    for (int i =0; i < MAX_NUM; i++) {
        for (int j=0; j < MAX_NUM; j++) {
            X[i][j] = -1.0;
        }
    }
    return;
}

void fillNewSubMatrix(double x[][MAX_NUM], int slab) {
    //fill array with -1 to start off with
    for (int i = 0; i < slab+2; i++) {
        for (int j = 0; j< MAX_NUM; j++) {
            x[i][j] = -1.0;
        } 
    }
    return;
}

void fillSubMatrixWithRank(double x[][MAX_NUM], int slab, int rank) {
    //fill my slab (not ghost points) with my rank
    for (int i = 1; i < slab+1; i++) {
        for (int j = 0; j < MAX_NUM; j++) {
            x[i][j] = rank;
        }
    }
    return;
}

int main(void) {
    double X[MAX_NUM][MAX_NUM]; //The matrix to divide up

    int comm_sz; //Number of processes
    int my_rank; //My process rank

    MPI_Init(NULL,NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    int slab = MAX_NUM / comm_sz;
    double x[slab+2][MAX_NUM]; //each process has own submatrix, +2 for ghost rows
    int numEls = slab * MAX_NUM;

    // According to assign instr, throw an error if:
    if (MAX_NUM % comm_sz != 0) {
        if (my_rank == 0) { printf("Error: Matrix (%d,%d) must be evenly divisible by workers\n", MAX_NUM, MAX_NUM); }
        MPI_Finalize();
        exit(0);
    }

    //Divide Matrix Up Bt Workers
    if (my_rank == 0) {
        fillOrigMatrix(X); //fill OG matrix with -1's
        //From P0 send all other processes their slab
        for (int processID = 1; processID < comm_sz; processID++) {
            int firstrow = my_rank * slab;     //ex firstrow = 0 means X[0][j]
            int numEls = slab * MAX_NUM;
            MPI_Send(&X[firstrow][0], numEls, MPI_DOUBLE, processID, 0, MPI_COMM_WORLD);
        }

        //Next, fill P0's submatrix x
        fillNewSubMatrix(x, slab);//initially, fill array with -1
        fillSubMatrixWithRank(x, slab, my_rank); //fill my slab (not ghost points) w/my rank
    }
    else {
        //workers should receive their pieces from process 0
        fillNewSubMatrix(x, slab);//initially, fill array with -1
        //start copying to 2nd row of allocated space
        MPI_Recv(x[1], numEls, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        fillSubMatrixWithRank(x, slab, my_rank); //fill my slab (not ghost points) w/my rank
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

    char hostbuffer[256];
    gethostname(hostbuffer, sizeof(hostbuffer));
    printf("\nProcess %d is on %s", my_rank, hostbuffer);

    //print my full x matrix to check 
    printf("\n");
    for (int i = 0; i < slab+2; i++) {
        for (int j= 0; j < MAX_NUM; j++) {
            printf("%d:[%.1f]", my_rank, x[i][j] );
        }
        printf("\n");
    }

    MPI_Finalize();
    return 0;
}
