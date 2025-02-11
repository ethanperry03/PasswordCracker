#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"
#include "cracker_cmd.h"

FILE* parse_cmd(char** argc) {    
    // open file to read
    FILE* pDict = fopen(argc[1], "r");
    // check if file pointer is null (indicating invalid file)
    if (pDict == NULL) {
        printf("error: '%s' is an invalid file\n", argc[1]);
        printf("ensure file exists and entered correctly\n\n");
        exit(1);
    }
    
    // open file to validate
    FILE* pTarget = fopen(argc[2], "r");
    // check if file pointer is null (indicating invalid file)
    if (pTarget == NULL) {
        printf("error: '%s' is an invalid file\n", argc[2]);
        printf("ensure file exists and entered correctly\n\n");
        fclose(pDict);
        fclose(pTarget); 
        exit(1);
    }
    // Read the 64-character hash value and error check in same step
    if (fscanf(pTarget, "%64s", TARGET) != 1) { 
        printf("Failed to read hash value\n"); 
        fclose(pDict);
        fclose(pTarget); 
        exit(1); 
    }

    // close file pointer
    fclose(pTarget); 

    // return file pointer to be used by program
    return pDict;
}