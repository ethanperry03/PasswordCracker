/** global.h - Ethan Perry - Dec 6, 2024
 * This header file contains the definitions and declarations of global variables,
 * structures, and constants used throughout the password cracking program. It provides
 * the necessary includes, defines, and external declarations to ensure proper data 
 * sharing and synchronization between different parts of the program.
 *
 * The main components of this file include:
 * - Macro definitions for maximum word length, local buffer size, and global buffer size.
 * - The `GlobalBuffer` structure, which holds the global buffer for inter-thread 
 *   communication, along withsynchronization primitives like mutexes and condition variables.
 * - External declarations of global variables such as the buffer instance (BUFFER), target hash
 *   (TARGET), and flags to indicate the state of the program (isFound, numProdsDone, and isDone).
 */

#ifndef __GLOBAL__
#define __GLOBAL__
#include <pthread.h>

// global constants setting maximum value for respective items below
#define MAX_WORD_LENGTH 100
#define MAX_LOCAL_BUFFER_SIZE 100   
#define MAX_GLOBAL_BUFFER_SIZE 10000

/** GlobalBuffer
 * This structure contains the global buffer used for inter-thread communication,
 * as well as the necessary mutex and condition variables for synchronization.
 */
typedef struct {
    char** buffer;               // Pointer to the array of strings in the buffer
    int start;                   // Index of the start of the buffer (used for circular buffer)
    int end;                     // Index of the end of the buffer (used for circular buffer)
    int count;                   // Current count of items in the buffer
    pthread_mutex_t mutex;       // Mutex for synchronizing access to the buffer
    pthread_cond_t notEmpty;     // Condition variable to signal that the buffer is not empty
    pthread_cond_t notFull;      // Condition variable to signal that the buffer is not full
} GlobalBuffer;

/* Global variables */
extern GlobalBuffer BUFFER;      // Global buffer instance used for inter-thread communication
extern char TARGET[65];          // Target hash value to be matched
extern int isFound;              // Flag to indicate if the correct password has been found
extern int numProdsDone;         // Count of the number of producer threads that have finished
extern int isDone;               // Flag to indicate if all producer threads are done


#endif