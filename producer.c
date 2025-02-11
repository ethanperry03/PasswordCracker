#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "producer.h"
#include "global.h"

void writeToBuffer(char** words, int offset) {
    // lock global buffer mutex
    pthread_mutex_lock(&BUFFER.mutex);

    // while there is not room in the global buffer to add data items
    while(BUFFER.count + offset > MAX_GLOBAL_BUFFER_SIZE && !isFound) {
        pthread_cond_wait(&BUFFER.notFull, &BUFFER.mutex);
    }

    // after conditions are good, did a consumer find the password
    if (isFound) {
        pthread_mutex_unlock(&BUFFER.mutex);
        pthread_exit(NULL);
    }
    // else, write to buffer as normal
    else{
        // once space frees, add data values
        for(int i = 0; i < offset; i++) {
            // put the ith word into the buffer and account for circular buffer
            strcpy(BUFFER.buffer[BUFFER.end], words[i]);
            // update BUFFER counters accordingly
            BUFFER.end = (BUFFER.end + 1) % MAX_GLOBAL_BUFFER_SIZE;
            BUFFER.count++;
        }
    }
    // check if number of words < number of consumers

    // broadcast all consumers to wake up and start processing
    pthread_cond_broadcast(&BUFFER.notEmpty);

    // unlock global buffer mutex
    pthread_mutex_unlock(&BUFFER.mutex);
}

void* producer(void* arg) {
    struct producerData* prod_arg = (struct producerData*)arg;

    // declare variables
    FILE* dict = prod_arg->dictPointer;
    int nProds = prod_arg->numProducers;

    // define local buffer for producer thread
    char** localBuffer = malloc(MAX_LOCAL_BUFFER_SIZE * sizeof(char*));
    for(int i = 0; i < MAX_LOCAL_BUFFER_SIZE; i++) {
        localBuffer[i] = malloc(MAX_WORD_LENGTH * sizeof(char));
    }

    // read data from the input dictionary
    int index = 0;
    while (fscanf(dict, "%99s", localBuffer[index]) != EOF && !isDone) {
        index++;
        if (index == MAX_LOCAL_BUFFER_SIZE) {
            writeToBuffer(localBuffer, index);
            index = 0;
        }
    }

    // if the local buffer is not full but also not empty
    if (index > 0) {
        writeToBuffer(localBuffer, index);
    }

    // free local, dynamic array
    for(int i = 0; i < MAX_LOCAL_BUFFER_SIZE; i++) {
        free(localBuffer[i]);
    }
    free(localBuffer);

    // increment number of terminated producers
    pthread_mutex_lock(&BUFFER.mutex);
    numProdsDone = numProdsDone + 1;
    // if all the producers are done, signal and change flag
    if (numProdsDone == nProds) {
        isDone = 1;
        // broadcast all consumers
        pthread_cond_broadcast(&BUFFER.notEmpty);
    }
    pthread_mutex_unlock(&BUFFER.mutex);

    pthread_exit(NULL);

    return NULL;
}