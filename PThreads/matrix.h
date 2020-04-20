/*
 * Author: Tyler Willis
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h> 
#include <sys/types.h>

#ifndef MATRIX_H
#define MATRIX_H



clock_t start, end;
int MATRIX_SIZE, ROWS_PER_THREAD, EXTRA_ROWS, NUM_THREADS;

double *A,*B,*C,*recvA,*recvB,*slavC,*sendBuf,*recvBuf,*exrecvBuf;
double C_result;

void slaveCompute(int start_index, int end_index);
void constructResultMatrix();
void receiveResultsLast();
void receiveResults();
void sendMatricesLast();
void printMatrices();
void sendMatrices();
void allocMemory();
void deallocMemory();
void generateRandomMatrix();
void allocSlaveMemory();
void deallocSlaveMemory();
void printMatrices();
void *testFunction(double *recvArray);
		

#endif /* MATRIX_H */

