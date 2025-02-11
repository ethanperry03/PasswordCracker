/** cracker_cmd - Ethan Perry - Dec 6, 2024
 * The primary function in this file is `parse_cmd`, which opens and validates the
 * dictionary and target files provided as command-line arguments, reads a 64-character
 * hash value from the target file, and returns a pointer to the opened dictionary file.
 * If any file operations fail, the function prints an error message and exits the program.
 * The function in this file is essential for ensuring that the input files are correctly
 * opened and read, and they handle error conditions gracefully by informing the user and
 * terminating the program if necessary.
 */

#ifndef __CRACKER_CMD__
#define __CRACKER_CMD__

/** parse_cmd()
 * This function opens the dictionary file for reading and validates that it
 * exists. It also opens the target file, validates it, and reads a 64-character
 * hash value from it, storing it in the global `TARGET` variable. If any file
 * operations fail, the function prints an error message and exits the program.
 *
 * @param argc: Array of command-line arguments. The first element is assumed to be 
 * the dictionary file path, and the second element is assumed to be the target file path.
 * The other arguments are validated elsewhere and do not get used here.
 * @return FILE*: Pointer to the opened dictionary file to be used by the program. 
 * If any error occurs, the program will exit, so the return value is only 
 * meaningful if the function completes successfully.
 */
FILE* parse_cmd(char** argc);

#endif