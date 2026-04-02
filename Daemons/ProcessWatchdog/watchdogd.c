#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>

#define EXT_SUCCESS 0
#define EXT_ERR_TOO_FEW_ARGS 1
#define EXT_ERR_POPEN 2
#define EXT_ERR_PCLOSE 3

int main(int argc, char** argv[]) 
{
    if (argc < 2) {
        fprintf(stderr, "Error - too few arguments... USAGE: ./watchdogd PROCESS_NAME");
        exit(EXT_ERR_TOO_FEW_ARGS);
    }

    FILE *fp;
    char line[128];

    char command[256];
    snprintf(command, sizeof(command), "pgrep %s", argv[1]);

    fp = popen(command, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error - popen failed");
        exit(EXT_ERR_POPEN);
    }
    
    while (fgets(line, sizeof(line), fp) != NULL) {
        printf("%s", line);
    }

    if (pclose(fp) == -1) {
        fprintf(stderr, "Error - pclose failed");
        exit(EXT_ERR_PCLOSE);
    }

    return EXT_SUCCESS;
}
