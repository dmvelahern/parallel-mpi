#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <stdlib.h> // For exit()


const int MAX_STRING = 100;

int main(void) {
    int MAX_NUM = 8; //matrix width
    double X[MAX_NUM][MAX_NUM]; //The matrix to divide up

    int comm_sz; //Number of processes
    int my_rank; //My process rank

    MPI_Init(NULL,NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // According to assign instr, throw an error if:
    if (MAX_NUM % comm_sz != 0) {
        printf("Error: Matrix (%d,%d) must be evenly divisible by workers\n", MAX_NUM, MAX_NUM);
        exit(-1);
    }
    // for (int i = 0; i < MAX_NUM; i++) {
    //     for (int j = 0; j < )
    // }

    if (my_rank == 0) {
        //Divide Matrix Up Bt Workers
        int slab = MAX_NUM / comm_sz;
        //fill array as test
        for (int i =0; i < MAX_NUM; i++) {
            for (int j=0; j < MAX_NUM; j++) {
                X[i][j] = 0.0;
            }
        }

        for (int i =0; i < MAX_NUM; i++) {
            for (int j=0; j < MAX_NUM; j++) {
                printf("[%d-%d]",i,j);
            }
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
