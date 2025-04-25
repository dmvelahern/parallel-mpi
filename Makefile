CFILE = test.c
OBJ = test.o
EXECUTABLE = test

NUM_WORKERS = 4

# Update the executable on make
all: $(EXECUTABLE)

# Compile
$(OBJ): $(CFILE)
	mpicc -g -Wall -c $(CFILE) -o $(OBJ)

# Link
$(EXECUTABLE): $(OBJ)
	mpicc -o $(EXECUTABLE) $(OBJ)

# Run the exec with NUM_WORKERS
run: $(EXECUTABLE)
	mpiexec -n $(NUM_WORKERS) ./$(EXECUTABLE)

# Remove the exec and o file on clean
clean:
	rm -rf $(EXECUTABLE) $(OBJ)

.PHONY: all run clean