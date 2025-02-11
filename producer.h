/** producer.h - Ethan Perry - Dec 6, 2024
 * This file contains the declarations of functions and data structures used by
 * the producer threads in the password cracking program. The producer threads
 * are responsible for reading words from an input dictionary file and writing
 * them into the global buffer. This header file ensures that the producer-related
 * functionalities are properly defined and can be used by other parts of the program.
 *
 * The main components of this file include:
 * - The `producerData` structure, which holds the necessary data for each producer thread.
 * - The `producer` function, which is the entry point for each producer thread.
 * - The 'writeToBuffer' function that helps the producer thread operations.
 */

#ifndef __PRODUCER__
#define __PRODUCER__

/** producerData
 * This structure contains the information needed by each producer thread,
 * including the number of producer threads and the file pointer to the
 * dictionary file from which words are read.
 */
struct producerData{
    // number of producer threads currently active
    int numProducers;
    // file pointer to password dictionary
    FILE* dictPointer;
};

/** writeToBuffer()
 * This function locks the global buffer mutex and writes the given words into
 * the global buffer. If there is not enough room in the buffer to add all the words,
 * the function waits until space is available. If a consumer has found the password
 * (`isFound` flag is set), the function unlocks the mutex and exits the thread. 
 * Once the words are added, the function broadcasts a signal to wake up any waiting 
 * consumer threads and unlocks the mutex.The function ensures thread-safe access to 
 * the global buffer and handles the synchronization of producer and consumer 
 * threads using mutexes and condition variables.
 *
 * @param words Array of words to be written into the buffer.
 * @param offset Number of words to be written into the buffer.
 */
void writeToBuffer(char**, int);

/** producer()
 * This function is executed by producer threads. Each thread reads words from an input 
 * dictionary file and writes them into the global buffer. The function uses a local buffer
 * to temporarily store the words before writing them to the global buffer in batches. 
 * The function ensures thread-safe access to the global buffer and handles synchronization 
 * using mutexes and condition variables.
 *
 * @param arg: Pointer to a `producerData` structure containing the dictionary file pointer
 * and the number of producers.
 * @return void* Always returns NULL.
 *
 * The function follows these steps:
 * - Reads words from the dictionary file into a local buffer.
 * - Writes words from the local buffer to the global buffer when 
 *   the local buffer is full or at the end of the dictionary.
 * - Frees the local buffer memory.
 * - Increments the count of terminated producers and signals 
 *   consumers if all producers are done.
 */
void* producer(void* arg);

#endif