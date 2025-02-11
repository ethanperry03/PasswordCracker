/** consumer.h - Ethan Perry - Dec 6, 2024
 * This file contains the implementation of functions used by consumer threads in the 
 * password cracking program. The consumer threads retrieve words from the global buffer,
 * process them to generate variants and compute hashes, and compare the hashes to a 
 * target hash. If a match is found, the correct password is written to an output file.
 * The file also includes utility functions for managing the global buffer and processing words.
 *
 * The main components of this file include:
 * - get_word(): Retrieves a word from the global buffer in a thread-safe manner.
 * - get_variants(): Generates variants of a given word with character substitutions 
 *   and trailing digits.
 * - process_word(): Processes a word by generating its variants and checking each
 *   variant against a target hash.
 * - output_to_file(): Writes a word to an output file.
 * - consumer(): Consumer thread function that processes words from the global buffer
 *   and writes the found password to a file.
 *
 * The functions in this file ensure thread-safe access to the global buffer and handle 
 * synchronization of producer and consumer threads using mutexes and condition variables.
 */

#ifndef __CONSUMER__
#define __CONSUMER__
#include "global.h"

/** consumerData
 * This structure contains the information needed by each consumer thread,
 * specifically the name of the output file where the found password will be written.
 */
struct consumerData {
    char* outputFileName;
};

/** get_word()
 * This function locks the global buffer mutex and retrieves a word from the buffer.
 * If the buffer is empty, the function waits until a word is available. If a consumer
 * has found the password (`isFound` flag is set) or all producers are done and the buffer
 * is empty (`isDone` flag is set), the function returns `NULL`. Once a word is retrieved,
 * the function signals producers that space is available in the buffer and unlocks the mutex.
 *
 * @return char* Pointer to the retrieved word. Returns `NULL` if the password is found or 
 * all producers are done and the buffer is empty.
 */
char* get_word();

/** get_variants()
 * This function generates 88 variants of a given word by performing character substitutions
 * (e.g., 'i' to '!', 'l' to '1', 'o' to '0') and adding trailing digits (0-9) to the word.
 * The variants are stored in the provided 2D array `variants`.
 *
 * @param word The input word for which variants are to be generated.
 * @param variants A 2D array to store the generated variants. The array should have
 * dimensions [88][MAX_WORD_LENGTH].
 *
 * The function follows these steps:
 * - Initializes the first 8 variants with the base word.
 * - Performs character substitutions to generate 8 base permutations.
 * - Appends trailing digits (0-9) to each base permutation to generate the final 88 variants.
 */
void get_variants(char*, char[88][MAX_WORD_LENGTH]);

/** process_word()  
 * This function generates 88 variants of the input word using the `get_variants` function,
 * hashes each variant, and compares the hash to a global target hash (`TARGET`). If a variant's
 * hash matches the target hash, the function sets a flag (`isFound`), signals all waiting consumers,
 * and returns the correct password. If no variant matches, the function returns `NULL`.
 *
 * @param word The input word to be processed.
 * @return char* Pointer to the correct password if found, or `NULL` if no match is found.
 *
 * The function follows these steps:
 * - Generates variants of the input word.
 * - Hashes each variant and compares the hash to the target hash.
 * - If a match is found, sets the `isFound` flag, stores the correct password, signals consumers,
 *   and returns the password.
 * - If no match is found, returns `NULL`.
 */
char* process_word(char*);

/** output_to_file()
 * This function opens the specified output file, writes the given word to it, and
 * then closes the file. If the file cannot be opened, the function prints an error
 * message and exits the program.
 *
 * @param outfile The path to the output file.
 * @param word The word to be written to the output file.
 *
 * The function follows these steps:
 * - Opens the specified output file in write mode.
 * - Checks if the file was successfully opened; if not, prints an error message and exits.
 * - Writes the word to the file.
 * - Closes the file.
 */
void output_to_file(char*, char*);

/** consumer()
 * This function is executed by consumer threads. Each thread retrieves words from the 
 * global buffer using the `get_word` function, processes the words to find the correct 
 * password, and writes the found password to the specified output file. The function 
 * ensures thread-safe access to the global buffer and handles synchronization using 
 * mutexes and condition variables.
 *
 * @param arg Pointer to a `consumerData` structure containing the output file name.
 * @return void* Always returns NULL.
 *
 * The function follows these steps:
 * - Retrieves words from the global buffer.
 * - Thread exits if the buffer is empty or an ending condition is met (isFound` or isDone)
 * - Processes each word using the `process_word` function.
 * - If the password is found, writes the password to the output file and exits the thread.
 * - Continues processing until an ending condition is met.
 */
void* consumer(void*);

#endif