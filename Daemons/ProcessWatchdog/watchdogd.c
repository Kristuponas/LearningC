#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <dirent.h>

#define EXT_SUCCESS 0
#define EXT_ERR_USAGE 1
#define EXT_ERR_POPEN 2
#define EXT_ERR_PCLOSE 3
#define EXT_ERR_FOPEN 4
#define EXT_ERR_FCLOSE 5

int main(int argc, char** argv) 
{
    if (argc != 2) 
    {
        fprintf(stderr, "Error - USAGE: %s <process_name>\n", argv[0]);
        exit(EXT_ERR_USAGE);
    }

    FILE *fp; 
    FILE *sp;   // Stats file pointer

    char line[128];
    char command[256];
    char path[128];

    char stats[128];
    char label[64];
    char value[64];

    snprintf(command, sizeof(command), "pgrep %s", argv[1]);

    fp = popen(command, "r");
    if (fp == NULL) 
    {
        fprintf(stderr, "Error - popen failed");
        exit(EXT_ERR_POPEN);
    }
    
    while (fgets(line, sizeof(line), fp) != NULL) 
    {
        //printf("%s", line);
        line[strcspn(line, "\n")] = '\0';
        snprintf(path, sizeof(path), "/proc/%s/status", line);

        printf("Trying to open: '%s'\n", path);

        sp = fopen(path, "r");
        if (sp == NULL)
        {
            fprintf(stderr, "Error - fopen error");
            exit(EXT_ERR_FOPEN);
        }

        while (fgets(stats, sizeof(stats), sp))
        {
            if (sscanf(stats, "%s %s", label, value) == 2)
            {
                if (strcmp(label, "Name:") == 0)
                    printf("Process name: %s\n", value);

                else if (strcmp(label, "State:") == 0)
                    printf("State: %s\n", value);

                else if (strcmp(label, "Pid:") == 0)
                {
                    int pid = atoi(value);
                    printf("PID: %d\n\n", pid);
                }
            }
        }

        fclose(sp);
        sp = NULL;
    }

    if (pclose(fp) == -1) 
    {
        fprintf(stderr, "Error - pclose failed");
        exit(EXT_ERR_PCLOSE);
    }

    return EXT_SUCCESS;
}
