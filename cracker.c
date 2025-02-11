/** main.c - Ethan Perry - Dec 6, 2024
 * The main function initializes global structures, creates producer and consumer threads,
 * and coordinates their execution to perform the password cracking task. The program reads
 * command-line arguments to set up the number of producer and consumer threads, processes
 * input files to read the dictionary and target hash, and handles synchronization between
 * threads using mutexes and condition variables. The program ensures proper resource 
 * management by dynamically allocating and freeing memory for buffers, thread data 
 * structures, and thread IDs. It also manages the lifetime of the global buffer and 
 * ensures that all threads complete their tasks before exiting.
 *
 * Usage:
 * ./password_cracker <dictionary_file> <target_file> <out_file> <num_prods> <num_cons>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include "sha-256.h"
#include "cracker_cmd.h"
#include "producer.h"
#include "consumer.h"
#include "global.h"

// global variables 
// (see global.h for comments)
GlobalBuffer BUFFER;
char TARGET[65];
int isFound = 0;
int numProdsDone = 0;
int isDone = 0;

/** main(argv, argc)
 * This function initializes global variables, sets up synchronization primitives,
 * creates and starts producer and consumer threads, and waits for all threads to complete.
 * It parses command-line arguments to get the names of input and output files, and the number
 * of producer and consumer threads to create. It ensures proper initialization of the global
 * buffer and associated synchronization primitives, and handles resource cleanup upon completion.
 *
 * @param argv Number of command-line arguments.
 * @param argc Array of command-line arguments.
 *             argc[1]: Dictionary file path
 *             argc[2]: Target file path
 *             argc[3]: Output file path
 *             argc[4]: Number of producer threads
 *             argc[5]: Number of consumer threads
 * @return int Returns 0 on successful completion.
 *
 * @note The function exits with an error message if the number of command-line arguments 
 * is incorrect, or if there are invalid thread number inputs.
 */
int main (int argv, char** argc) {
    // for formatting
    printf("\n");
    // error check amount of input
    if (argv != 6) {
        printf("Error: incorrect number of input parameters\n");
        exit(1);
    }

    // store and check values
    int nProds = atoi(argc[4]);
    int nCons = atoi(argc[5]);
    // check if they are greater than 0
    if (nProds < 1 || nCons < 1) {
        printf("Invalid thread number input\n");
        exit(1);
    }

    // declare outfile name
    char* outputFile = argc[3];

    // parse command line and check files
    FILE* dict = parse_cmd(argc);

    // initialize all data related to the global BUFFER
    BUFFER.start = BUFFER.end = BUFFER.count = 0;
    
    pthread_mutex_init(&BUFFER.mutex, NULL);
    pthread_cond_init(&BUFFER.notEmpty, NULL);
    pthread_cond_init(&BUFFER.notFull, NULL);

    // dynamically allocate BUFFER's buffer
    BUFFER.buffer = malloc(MAX_GLOBAL_BUFFER_SIZE * sizeof(char*));
    for(int i = 0; i < MAX_GLOBAL_BUFFER_SIZE; i++) {
        BUFFER.buffer[i] = malloc(MAX_WORD_LENGTH * sizeof(char));
    }

    /********************* create thread data arrays ********************************/
    // generate producers and send them to the thread produce function
    struct producerData* prod_data = malloc(nProds * sizeof(struct producerData));
    for(int i = 0; i < nProds; i++) {
        prod_data[i].numProducers = nProds;
        prod_data[i].dictPointer = dict;
    }
    // generate consumers and send them to the thread consume function
    struct consumerData* con_data = malloc(nCons * sizeof(struct consumerData));
    for(int i = 0; i < nCons; i++) {
        con_data[i].outputFileName = outputFile;
    }

    /********************* create threads and store the ids *************************/
    // loop through all of the thread data structs and create threads for each
    // for producers
    pthread_t* producer_ids = malloc(nProds * sizeof(pthread_t));
    for (int i = 0; i < nProds; i++) {
        pthread_create(&producer_ids[i], NULL, producer, &prod_data[i]);
    }
    // for consumers
    pthread_t* consumer_ids = malloc(nCons * sizeof(pthread_t));
    for (int i = 0; i < nCons; i++) {
        pthread_create(&consumer_ids[i], NULL, consumer, &con_data[i]);
    }
    
    /************************* join all thread **************************************/
    // join all producer threads
    for (int i = 0; i < nProds; i++) {
        pthread_join(producer_ids[i], NULL);
    }    
    // join all consumer threads
    for (int i = 0; i < nCons; i++) {
        pthread_join(consumer_ids[i], NULL);
    }

    if(!isFound) {
        printf("No password match found\n");
    }
    else {
        printf("outfile:  %s\n", outputFile);
    }
    printf("\n");

    // destroy and deallocate data
    pthread_mutex_destroy(&BUFFER.mutex);
    pthread_cond_destroy(&BUFFER.notEmpty);
    pthread_cond_destroy(&BUFFER.notFull);
    fclose(dict);
    
    // deallocate BUFFER's buffer
    for(int i = 0; i < MAX_GLOBAL_BUFFER_SIZE; i++) {
        free(BUFFER.buffer[i]);
    }
    free(BUFFER.buffer);

    // free prod/con array for storing input structs
    free(prod_data);
    free(con_data);

    // free producer and consumer ID arrays
    free(producer_ids);
    free(consumer_ids);

    return 0;
}