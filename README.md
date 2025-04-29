## Makefile Commands
make run
runs the program with NUM_WORKERS (default is 4)

make clean
removes the executable and the object file

make
compiles and links the executable

## File Meaning
test.c - the C file that contains the MPI program for the stencil data structure
Makefile - makefile containg commands for compiling test.c, running it with four workers, and cleaning the object and exec files
run_on_cluster.PNG - picture containing the output of running the test.c file on the cluster

## Notes about the Output
The output for the program has all the processes print out its rank followed by brackets containing
an element from its matrix (matrix contains the process's slab and the preceding processes
bottom row and the succeeding process's top row, as the current process's top and bottom rows
respectively.)

By this logic, however, the 0th process would not have a top row & the last process would not
have a bottom row, so in the output, those rows contain -1s to denote their "invalid" state.
The negative ones simply mean to ignore those rows.
  
Example Output:

make: Nothing to be done for 'all'.
mpiexec -n 4 ./test

Process 0 is on gpunode005
0:[-1.0]0:[-1.0]0:[-1.0]0:[-1.0]0:[-1.0]0:[-1.0]0:[-1.0]0:[-1.0]
0:[0.0]0:[0.0]0:[0.0]0:[0.0]0:[0.0]0:[0.0]0:[0.0]0:[0.0]
0:[0.0]0:[0.0]0:[0.0]0:[0.0]0:[0.0]0:[0.0]0:[0.0]0:[0.0]
0:[1.0]0:[1.0]0:[1.0]0:[1.0]0:[1.0]0:[1.0]0:[1.0]0:[1.0]

Process 1 is on gpunode007
1:[0.0]1:[0.0]1:[0.0]1:[0.0]1:[0.0]1:[0.0]1:[0.0]1:[0.0]
1:[1.0]1:[1.0]1:[1.0]1:[1.0]1:[1.0]1:[1.0]1:[1.0]1:[1.0]
1:[1.0]1:[1.0]1:[1.0]1:[1.0]1:[1.0]1:[1.0]1:[1.0]1:[1.0]
1:[2.0]1:[2.0]1:[2.0]1:[2.0]1:[2.0]1:[2.0]1:[2.0]1:[2.0]

Process 2 is on gpunode009
2:[1.0]2:[1.0]2:[1.0]2:[1.0]2:[1.0]2:[1.0]2:[1.0]2:[1.0]
2:[2.0]2:[2.0]2:[2.0]2:[2.0]2:[2.0]2:[2.0]2:[2.0]2:[2.0]

Process 3 is on gpunode012
3:[2.0]3:[2.0]3:[2.0]3:[2.0]3:[2.0]3:[2.0]3:[2.0]3:[2.0]
3:[3.0]3:[3.0]3:[3.0]3:[3.0]3:[3.0]3:[3.0]3:[3.0]3:[3.0]
2:[2.0]2:[2.0]2:[2.0]2:[2.0]2:[2.0]2:[2.0]2:[2.0]2:[2.0]
2:[3.0]2:[3.0]2:[3.0]2:[3.0]2:[3.0]2:[3.0]2:[3.0]2:[3.0]
3:[3.0]3:[3.0]3:[3.0]3:[3.0]3:[3.0]3:[3.0]3:[3.0]3:[3.0]
3:[-1.0]3:[-1.0]3:[-1.0]3:[-1.0]3:[-1.0]3:[-1.0]3:[-1.0]3:[-1.0]

