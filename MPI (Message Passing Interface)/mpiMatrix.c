#include "lab3.h"

/* 
 * File:   lab3.c
 * Author: Tyler Willis
 
 */

int main(int argc, char** argv) {
	
	MATRIX_SIZE = atoi(argv[1]);
	
	int rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

    ROWS_PER_PROC = MATRIX_SIZE / (size - 1);
	EXTRA_ROWS = ROWS_PER_PROC + (MATRIX_SIZE % (size-1));
	
	allocMemory();
	generateRandomMatrix();
	
	if(MATRIX_SIZE < size){
		printf("\nERROR: Can't have number of processes %d larger than matrix size %d.\n", size, MATRIX_SIZE);
		MPI_Finalize();
		return(0);
	}
	
	// Broadcast B matrix to all slave processes
	MPI_Bcast(
	  /* data		  = */ B,
	  /* count        = */ (MATRIX_SIZE*MATRIX_SIZE), 
	  /* datatype     = */ MPI_FLOAT,
	  /* source       = */ 0,
	  /* communicator = */ MPI_COMM_WORLD);

    if (rank == 0) { // If current process is master process
		// Start timer
		start = (double)clock();
		
		// Send A matrix rows to slave processes
		sendMatrices();
		
		// Send A matrix rows to last slave process
		sendMatricesLast();
		
		// Receive results from slave processes
		receiveResults();
		
		// Receive results from last slave process
		receiveResultsLast();
		
		// Construct result matrix
		constructResultMatrix();
		
		// End timer
		end = (double)clock();  
		printf("Time elapsed: %fus\n", ((end - start)/(double)CLOCKS_PER_SEC)*100000);
		
		// Display matrix resutls smaller than or equal to 16
		printMatrices();

    } else if(rank < size-1){
		allocSlaveMemory();
		
		// Receive matrix information from master process
		MPI_Recv(
		  /* data         = */ recvA,
		  /* count        = */ (ROWS_PER_PROC*MATRIX_SIZE), 
		  /* datatype     = */ MPI_FLOAT, 
		  /* source       = */ 0,
		  /* tag          = */ 0,
		  /* communicator = */ MPI_COMM_WORLD, 
		  /* status       = */ MPI_STATUS_IGNORE);
		
		// Perform calculation
		slaveCompute(0, ROWS_PER_PROC);
		
		// Send result back to master process
		MPI_Send(
		 /* data         = */ slavC,
		 /* count        = */ ROWS_PER_PROC*MATRIX_SIZE,
		 /* datatype     = */ MPI_FLOAT, 
		 /* destination  = */ 0, 
		 /* tag          = */ 0,
		 /* comms        = */ MPI_COMM_WORLD);
		
		// Deallocate memory
		deallocSlaveMemory();
    } else {
		// Allocate memory
		allocSlaveMemory();
		
		// Receive matrix information from master process
		MPI_Recv(
		  /* data         = */ recvA,
		  /* count        = */ EXTRA_ROWS*MATRIX_SIZE, 
		  /* datatype     = */ MPI_FLOAT, 
		  /* source       = */ 0,
		  /* tag          = */ 0,
		  /* communicator = */ MPI_COMM_WORLD, 
		  /* status       = */ MPI_STATUS_IGNORE);

		// Perform calculation
		slaveCompute(rank-1,rank+EXTRA_ROWS);

		// Send results back to master process
		MPI_Send(
		 /* data         = */ slavC,
		 /* count        = */ (EXTRA_ROWS)*MATRIX_SIZE, 
		 /* datatype     = */ MPI_FLOAT, 
		 /* destination  = */ 0, 
		 /* tag          = */ 0,
		 /* comms        = */ MPI_COMM_WORLD);
	}

	deallocMemory();
	MPI_Finalize();
    return (0);
}

void receiveResultsLast(){
	MPI_Recv(
	  /* data         = */ exrecvBuf,
	  /* count        = */ (EXTRA_ROWS)*MATRIX_SIZE, 
	  /* datatype     = */ MPI_FLOAT, 
	  /* source       = */ size-1,
	  /* tag          = */ 0,
	  /* communicator = */ MPI_COMM_WORLD, 
	  /* status       = */ MPI_STATUS_IGNORE);
}

void slaveCompute(int start_index, int end_index){
	int B_row = 0;
    int B_col = 0;
	int A_row = 0;
	int A_index = 0;
	int k = 0;
	for(int l= start_index; l < end_index; l++) {
		int row = (l)*ROWS_PER_PROC*MATRIX_SIZE;
		for (int i = row; i < (MATRIX_SIZE+row); i++) {
			for (int j = row; j < (MATRIX_SIZE+row); j++) {
				C_result += recvA[A_row*MATRIX_SIZE+A_index]*B[(MATRIX_SIZE*B_row) + B_col];
				A_index++;B_row++;
				if(A_index%MATRIX_SIZE==0){
					A_index = 0;
				}
			}
			slavC[k++] = C_result;
			B_col++;B_row = 0;C_result = 0;
		}
		B_col=0;B_row=0;A_row++;
	}
}

void constructResultMatrix(){
	int c_index_start = (((size-2)*ROWS_PER_PROC*MATRIX_SIZE + ROWS_PER_PROC*MATRIX_SIZE)-MATRIX_SIZE*ROWS_PER_PROC);
	int c_index_end = ((((size-2)*ROWS_PER_PROC*MATRIX_SIZE + ROWS_PER_PROC*MATRIX_SIZE)-MATRIX_SIZE*ROWS_PER_PROC)+EXTRA_ROWS*MATRIX_SIZE);
	
	int buf_index = 0;
	for(int i = c_index_start; i < c_index_end; i++){
		C[i] = exrecvBuf[buf_index++];
	}
}

void receiveResults(){
	for (int i = 1; i < (size-1); i++){
		MPI_Recv(
		  /* data         = */ recvBuf,
		  /* count        = */ ROWS_PER_PROC*MATRIX_SIZE, 
		  /* datatype     = */ MPI_FLOAT, 
		  /* source       = */ i,
		  /* tag          = */ 0,
		  /* communicator = */ MPI_COMM_WORLD, 
		  /* status       = */ MPI_STATUS_IGNORE);
		  int k = 0;
		  for (int j = (i-1)*ROWS_PER_PROC*MATRIX_SIZE; j < ((i-1)*ROWS_PER_PROC*MATRIX_SIZE + ROWS_PER_PROC*MATRIX_SIZE); j++){
			  C[j] = recvBuf[k++];
		  }
	}
}

void sendMatricesLast(){
	int k = 0;
	for (int j = (size-2)*(ROWS_PER_PROC)*MATRIX_SIZE; j < ((size-2)*(ROWS_PER_PROC+EXTRA_ROWS)*MATRIX_SIZE-(size-1) + ROWS_PER_PROC*MATRIX_SIZE); j++){
		sendBuf[k++] = A[j];
	}
	MPI_Send(
		 /* data         = */ sendBuf,
		 /* count        = */ EXTRA_ROWS*MATRIX_SIZE, 
		 /* datatype     = */ MPI_FLOAT, 
		 /* destination  = */ size-1, 
		 /* tag          = */ 0,
		 /* comms        = */ MPI_COMM_WORLD);
}

void printMatrices(){
	
	// Print all the matrices if size is less than 16
	if (MATRIX_SIZE <= 16) {

		printf("___________________MATRIX_A_____________________\n");
		for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
			printf(" %f ", A[i]);
			if (i % MATRIX_SIZE == (MATRIX_SIZE - 1)) {
				printf("\n");
			}
		}
		printf("\n___________________MATRIX_B_____________________\n");
		for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
			printf(" %f ", B[i]);
			if (i % MATRIX_SIZE == (MATRIX_SIZE - 1)) {
				printf("\n");
			}
		}
		printf("\n___________________MATRIX_C_____________________\n");

		for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
			printf(" %f ", C[i]);
			if (i % MATRIX_SIZE == (MATRIX_SIZE - 1)) {
				printf("\n");
			}
		}
	}
	
}

void sendMatrices(){
	for(int i = 0; i < (size-2); i++){
		int k = 0;
		for (int j = i*(ROWS_PER_PROC)*MATRIX_SIZE; j < i*(ROWS_PER_PROC)*MATRIX_SIZE+ROWS_PER_PROC*MATRIX_SIZE; j++){
			sendBuf[k++] = A[j];
		}
		MPI_Send(
			 /* data         = */ sendBuf,
			 /* count        = */ ROWS_PER_PROC*MATRIX_SIZE, 
			 /* datatype     = */ MPI_FLOAT, 
			 /* destination  = */ i+1, 
			 /* tag          = */ 0,
			 /* comms        = */ MPI_COMM_WORLD);
	}
}

void allocSlaveMemory() {
    recvA = (float*) calloc(MATRIX_SIZE*MATRIX_SIZE*MATRIX_SIZE, sizeof (float));
    slavC = (float*) calloc(MATRIX_SIZE*MATRIX_SIZE, sizeof (float));
}

void allocMemory() {
	
    A = (float*) calloc(MATRIX_SIZE*MATRIX_SIZE*MATRIX_SIZE, sizeof (float));
    B = (float*) calloc(MATRIX_SIZE*MATRIX_SIZE*MATRIX_SIZE, sizeof (float));
    C = (float*) calloc(MATRIX_SIZE*MATRIX_SIZE*MATRIX_SIZE, sizeof (float));
	sendBuf = (float*) calloc(MATRIX_SIZE*MATRIX_SIZE*MATRIX_SIZE, sizeof (float));
	recvBuf = (float*) calloc(MATRIX_SIZE*MATRIX_SIZE*MATRIX_SIZE, sizeof (float));
	exrecvBuf = (float*) calloc(MATRIX_SIZE*MATRIX_SIZE*MATRIX_SIZE, sizeof (float));
}

void deallocSlaveMemory(){
	free(recvA);
	free(slavC);
}

void deallocMemory() {
    free(A);
    free(B);
    free(C);
	free(sendBuf);
	free(recvBuf);
}

void generateRandomMatrix() {
    // Generate a matrix of size MATRIX_SIZE
    for (int i = 0; i < (MATRIX_SIZE * MATRIX_SIZE); i++) {
        A[i] = (float) rand() / 10000000;
        B[i] = (float) rand() / 10000000;
    }
}