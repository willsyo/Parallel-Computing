#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define TRUE 1

typedef int buffer_item;
#define BUFFER_SIZE 5

int insert_item(buffer_item item);
int remove_item(buffer_item *item);

buffer_item buffer[BUFFER_SIZE];

pthread_mutex_t insertThread, removeThread;

sem_t empty;
sem_t occupied;

int insertPointer = 0, removePointer = 0;

void *producer(void *param);
void *consumer(void *param);

int insert_item(buffer_item item) {

    sem_wait(&empty);
    pthread_mutex_lock(&insertThread);
    buffer[insertPointer] = item;
    if (++insertPointer >= BUFFER_SIZE)
        insertPointer = 0;
    
    /*printf("Added item %d, Array contents are now: ", item);
    for (int i = 0; i < BUFFER_SIZE - 1; i++){
        printf("%d, ", buffer[i]);
    } printf("%d\n", buffer[BUFFER_SIZE - 1]);
    */
    pthread_mutex_unlock(&insertThread);
    sem_post(&occupied);
    
    return 0;

}

int remove_item(buffer_item *item) {

    sem_wait(&occupied);
    pthread_mutex_lock(&removeThread);
    *item = buffer[removePointer];
    //printf("Removed item %d, Array contents are now: ", buffer[removePointer]);
    buffer[removePointer] = 0;
    if (++removePointer >= BUFFER_SIZE)
        removePointer = 0;
    
    /*for (int i = 0; i < BUFFER_SIZE - 1; i++){
        printf("%d, ", buffer[i]);
    } printf("%d\n", buffer[BUFFER_SIZE - 1]);*/
    
    pthread_mutex_unlock(&removeThread);
    sem_post(&empty);

    return 0;
}

int main(int argc, char *argv[]) {
    int sleepTime, producerThreads, consumerThreads;
    int i, j;

    if (argc != 4) {
        fprintf(stderr, "Useage: <sleep time> <producer threads> <consumer threads>\n");
        return -1;
    }
    
    //printf("Creating threads...\n");
            
    sleepTime = atoi(argv[1]);
    producerThreads = atoi(argv[2]);
    consumerThreads = atoi(argv[3]);
    
    //printf("Sleep time is: %d\n# of producer threads: %d\n# of consumer threads: %d\n", sleepTime, producerThreads, consumerThreads);

    pthread_mutex_init(&insertThread, NULL);
    pthread_mutex_init(&removeThread, NULL);

    int BSIZE = BUFFER_SIZE;
    int ret1 = sem_init(&empty, 0, BSIZE);
    int ret2 = sem_init(&occupied, 0, 0);

    pthread_t prod[producerThreads], cons[consumerThreads];

    for (i = 0; i < producerThreads; i++) {
        pthread_create(&prod[i], NULL, producer, NULL);
        sleep(1);
    }

    for (j = 0; j < consumerThreads; j++) {
        pthread_create(&cons[j], NULL, consumer, NULL);
        sleep(1);
    }
    
    //printf("Done creating threads, going to sleep now!\n");

    /* Sleep for user specified time */
    sleep(sleepTime);
    return 0;
}

void *producer(void *param) {
    time_t t;
    srand((unsigned) time(&t));
    buffer_item item;
    while (TRUE) {
        /* sleep for a random period of time */
        sleep(rand() % 3 + 1);
        /* generate a random number */
        item = rand() % 100;
        if (insert_item(item)) {
            printf("report error condition");
        } else {
            printf("producer produced %d\n", item);
        }
    }
}

void *consumer(void *param) {
    time_t t;
    srand((unsigned) time(&t));
    buffer_item item;
    while (TRUE) {
        /* sleep for a random period of time */
        sleep(rand() % 3 + 1);
        if (remove_item(&item)) {
            printf("report error condition, Consumer");
        } else {
            printf("consumer consumed %d\n", item);
        }
    }
}
