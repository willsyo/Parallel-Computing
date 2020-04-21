#include "matrix.h"

int main(int argc, char** argv) {

    NUM_THREADS = 8;
    MATRIX_SIZE = 10;

    // Init threads
    pthread_t p_threads[NUM_THREADS];
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    // Start timer
    start = clock();

    ROWS_PER_THREAD = MATRIX_SIZE / (NUM_THREADS - 1);
    EXTRA_ROWS = ROWS_PER_THREAD + (MATRIX_SIZE % (NUM_THREADS - 1));

    allocMemory();
    generateRandomMatrix();

    for (int i = 0; i < NUM_THREADS; i++) {
        if (i > 0) {
            int k = 0;
            for (int j = (NUM_THREADS - 2)*(ROWS_PER_THREAD) * MATRIX_SIZE; j < ((NUM_THREADS - 2)*(ROWS_PER_THREAD + EXTRA_ROWS) * MATRIX_SIZE - (NUM_THREADS - 1) + ROWS_PER_THREAD * MATRIX_SIZE); j++) {
                sendBuf[k++] = A[j];
            }
            printMatrices();
        }
        pthread_create(&p_threads[i], &attr, testFunction, (void *) &sendBuf);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(p_threads[i], NULL);
        // Construct C matrix
    }

    end = clock();
}

void *testFunction(double *recvBuf) {
    printf("\n___________________MATRIX_BUFFER_____________________\n");

    for (int i = 0; i < MATRIX_SIZE * ROWS_PER_THREAD; i++) {
        printf(" %f ", recvBuf[i]);
        if (i % MATRIX_SIZE == (MATRIX_SIZE - 1)) {
            printf("\n");
        }
    }
}

void matrixCompute(int start_index, int end_index) {
    int B_row = 0;
    int B_col = 0;
    int A_row = 0;
    int A_index = 0;
    int k = 0;
    for (int l = start_index; l < end_index; l++) {
        int row = (l) * ROWS_PER_THREAD*MATRIX_SIZE;
        for (int i = row; i < (MATRIX_SIZE + row); i++) {
            for (int j = row; j < (MATRIX_SIZE + row); j++) {
                C_result += recvA[A_row * MATRIX_SIZE + A_index] * B[(MATRIX_SIZE * B_row) + B_col];
                A_index++;
                B_row++;
                if (A_index % MATRIX_SIZE == 0) {
                    A_index = 0;
                }
            }
            slavC[k++] = C_result;
            B_col++;
            B_row = 0;
            C_result = 0;
        }
        B_col = 0;
        B_row = 0;
        A_row++;
    }
}

void printMatrices() {

    // Print all the matrices if size is less than 16
    if (MATRIX_SIZE <= 16) {

        printf("___________________MATRIX_A_____________________\n");
        for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
            printf(" %f ", A[i]);
            if (i % MATRIX_SIZE == (MATRIX_SIZE - 1)) {
                printf("\n");
            }
        }
/*
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
*/
        printf("\n___________________MATRIX_BUFFER_____________________\n");

        for (int i = 0; i < MATRIX_SIZE * ROWS_PER_THREAD; i++) {
            printf(" %f ", recvBuf[i]);
            if (i % MATRIX_SIZE == (MATRIX_SIZE - 1)) {
                printf("\n");
            }
        }
    }

}

void allocSlaveMemory() {
    recvA = (double*) calloc(MATRIX_SIZE * MATRIX_SIZE*MATRIX_SIZE, sizeof (double));
    slavC = (double*) calloc(MATRIX_SIZE*MATRIX_SIZE, sizeof (double));
}

void allocMemory() {

    A = (double*) calloc(MATRIX_SIZE * MATRIX_SIZE * MATRIX_SIZE, sizeof (double));
    B = (double*) calloc(MATRIX_SIZE * MATRIX_SIZE * MATRIX_SIZE, sizeof (double));
    C = (double*) calloc(MATRIX_SIZE * MATRIX_SIZE * MATRIX_SIZE, sizeof (double));

}

void deallocSlaveMemory() {
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
        A[i] = (double) rand() / 10000000;
        B[i] = (double) rand() / 10000000;
    }
}
