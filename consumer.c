#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include "sha-256.h"
#include "consumer.h"
#include "global.h"

char* get_word() {
    // Declare a static buffer for the word
    static char tempWord[MAX_WORD_LENGTH];

    // Acquire lock
    pthread_mutex_lock(&BUFFER.mutex);

    // While conditions are not met
    while (BUFFER.count == 0 && !isDone && !isFound) {
        // Wait for buffer to have data to process
        pthread_cond_wait(&BUFFER.notEmpty, &BUFFER.mutex);
    }
    if (isFound || (isDone && BUFFER.count == 0)) {
        pthread_mutex_unlock(&BUFFER.mutex);
        return NULL;
    }

    // Pop word from end of the buffer and update global variables
    BUFFER.end = (BUFFER.end - 1 + MAX_GLOBAL_BUFFER_SIZE) % MAX_GLOBAL_BUFFER_SIZE;
    strcpy(tempWord, BUFFER.buffer[BUFFER.end]);
    BUFFER.count--;

    // Signal a producer and unlock mutex
    pthread_cond_signal(&BUFFER.notFull);
    pthread_mutex_unlock(&BUFFER.mutex);

    // Return static buffer
    return tempWord;
}

void get_variants(char* word, char variants[88][MAX_WORD_LENGTH]) {
    // Start all 8 words at the base word
    for (int i = 0; i < 8; i++) {
        strcpy(variants[i], word);
    }

    // Loop through to get the 8 permutations
    for (int j = 0; j < strlen(word); j++) {
        if (word[j] == 'i') {
            variants[1][j] = '!';
            variants[3][j] = '!';
            variants[5][j] = '!';
            variants[7][j] = '!';
        }
        if (word[j] == 'l') {
            variants[2][j] = '1';
            variants[3][j] = '1';
            variants[6][j] = '1';
            variants[7][j] = '1';
        }
        if (word[j] == 'o') {
            variants[4][j] = '0';
            variants[5][j] = '0';
            variants[6][j] = '0';
            variants[7][j] = '0';
        }
    }

    // Add trailing digits to variants
    int index;
    for (int i = 0; i < 8; i++) { 
        for (int j = 0; j < 10; j++) { 
            index = 8 + i * 10 + j;
            sprintf(variants[index], "%s%d", variants[i], j); 
        }
    }
}

char* process_word(char* word) {
    // store all of the generated variants in this array
    char variants[88][MAX_WORD_LENGTH];
    // store the hashed value
    char hash[65];
    // store the target word
    static char correct[MAX_WORD_LENGTH];

    // get the variants
    get_variants(word, variants);

    // loop through all variants
    for (int i = 0; i < 88; i++) {
        // convert the input word to the hashed value
        sha_256_string(hash, variants[i], strlen(variants[i]));
        // acquire lock
        pthread_mutex_lock(&BUFFER.mutex);
        if (strcmp(hash, TARGET) == 0) {
            // output to user
            printf("Correct password has been found!\npassword: %s\n", variants[i]);
            // change flag
            isFound = 1;
            // store the correct password
            strcpy(correct, variants[i]);
            // signal all waiting consumers
            pthread_cond_broadcast(&BUFFER.notFull);
            pthread_cond_broadcast(&BUFFER.notEmpty);
            pthread_mutex_unlock(&BUFFER.mutex);
            // return the correct password
            return correct;
        }
        // release lock
        pthread_mutex_unlock(&BUFFER.mutex);
    }
    // return NULL if nothing was found
    return NULL;
}

void output_to_file(char* outfile, char* word){
    // open outfile
    FILE* file = fopen(outfile, "w");
    if (file == NULL) {
        printf("Failed to open file\n");
        exit(1);
    }
    fprintf(file, "%s\n", word);
    fclose(file);
}

void* consumer(void* arg) {
    struct consumerData* con_arg = (struct consumerData*)arg;
    char* filename = con_arg->outputFileName;

    while (1) {
        char* word = get_word();
        // this means either buffer is empty or ending condition was met
        if (word == NULL) {
            pthread_mutex_lock(&BUFFER.mutex);
            // one of the ending conditions, exit the program
            if (isFound || (isDone && BUFFER.count == 0)) {
                pthread_mutex_unlock(&BUFFER.mutex);
                pthread_exit(NULL);
            }
            pthread_mutex_unlock(&BUFFER.mutex);
        } 
        // else, consume the word
        else {
            char* found_word = process_word(word);
            if (found_word != NULL) {
                // output to file, free memory, exit thread
                output_to_file(filename, found_word);
                pthread_exit(NULL);
            }
        }
    }
}