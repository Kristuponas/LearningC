#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>

#define EXT_SUCCESS 0
#define EXT_ERR_USAGE 1
#define EXT_ERR_POPEN 2
#define EXT_ERR_PCLOSE 3
#define EXT_ERR_FOPEN 4
#define EXT_ERR_FCLOSE 5
#define EXT_ERR_FORK 6
#define EXT_ERR_READLINK 7
#define EXT_ERR_EXECVP 8

typedef int8_t i8;
typedef uint8_t u8;
typedef uint32_t u32;

typedef struct {
    char name[128];
    char command[128];
    char state[8];
    u32 pid;
    bool is_running;
} WatchedProcess;

volatile int terminate = false;

static void handle_termination(int signum)
{
    terminate = true;
}

static void init_service()
{
    signal(SIGINT, handle_termination);
    signal(SIGTERM, handle_termination);
}

int main(int argc, char** argv) 
{
    if (argc < 2) 
    {
        fprintf(stderr, "Error - USAGE: %s <process_name(s)>\n", argv[0]);
        exit(EXT_ERR_USAGE);
    }

    init_service();

    FILE *fp; 
    FILE *sp;   // Stats file pointer

    char line[128];
    char command[256];
    char path[128];

    char stats[128];
    char label[64];
    char value[64];

    char executable_path[256];

    u8 watch_count = argc - 1;
    WatchedProcess watchlist[10];

    for (int i = 0; i < watch_count; i++)
    {
        strncpy(watchlist[i].command, argv[i + 1], sizeof(watchlist[i].command) - 1);
        watchlist[i].command[sizeof(watchlist[i].command) - 1] = '\0';
        watchlist[i].pid = 0;
        watchlist[i].is_running = false;
    }

    while(!terminate)
    {
        for (int i = 0; i < watch_count; i++) 
        {
            bool was_running = watchlist[i].is_running;
            printf("Was_running: %d, is_running: %d\n", was_running, watchlist[i].is_running);
            snprintf(command, sizeof(command), "pgrep -x %s 2>/dev/null", watchlist[i].command);

            fp = popen(command, "r");
            if (fp == NULL) 
            {
                fprintf(stderr, "Error - popen failed");
                exit(EXT_ERR_POPEN);
            }

            if (fgets(line, sizeof(line), fp) != NULL)
            {
                line[strcspn(line, "\n")] = '\0';
                watchlist[i].pid = atoi(line);
                watchlist[i].is_running = true;

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
                            strncpy(watchlist[i].name, value, sizeof(watchlist[i].name) - 1);
                         
                        else if (strcmp(label, "State:") == 0)
                            strncpy(watchlist[i].state, value, sizeof(watchlist[i].state) - 1);

                        else if (strcmp(label, "Pid:") == 0)
                            watchlist[i].pid = atoi(value);
                    }
                }

                fclose(sp);
                sp = NULL;
            }
            else 
            {
                watchlist[i].is_running = false;
                printf("Process %s not found\n", watchlist[i].name);
            }

            if (pclose(fp) == -1) 
            {
                fprintf(stderr, "Error - pclose failed");
                exit(EXT_ERR_PCLOSE);
            }

            if (was_running && !watchlist[i].is_running)
            {
                printf("Process: %s (PID: %u) died.\n", watchlist[i].name, watchlist[i].pid);
                printf("Resetarting process...\n");

                pid_t child_pid = fork();
                if (child_pid == -1)
                {
                    fprintf(stderr, "Error - fork failed\n");
                    exit(EXT_ERR_FORK);
                }
                else if (child_pid == 0)
                {
                    snprintf(executable_path, sizeof(executable_path), "/proc/%d/exe", watchlist[i].pid);
                    
                    ssize_t len = readlink(executable_path, executable_path, sizeof(executable_path) - 1);
                    if (len == -1) {
                        fprintf(stderr, "Error - readlink failed to get executable path\n");
                        exit(EXT_ERR_READLINK);
                    }

                    executable_path[len] = '\0';

                    char *args[] = { executable_path, NULL };
                    i8 status_code = execvp(executable_path, args);
                    if (status_code == -1)
                    {
                        fprintf(stderr, "Error - execvp did not terminate correctly\n");
                        exit(EXT_ERR_EXECVP);
                    }
                } 
                else
                {
                    // Parent process
                    printf("Restarted process: %s (PID: %d)\n", watchlist[i].name, child_pid);
                    watchlist[i].pid = child_pid;
                    waitpid(child_pid, NULL, WNOHANG);
                }
            }
            else if (!was_running && watchlist[i].is_running)
                printf("Process: %s (PID: %u) is back.\n", watchlist[i].name, watchlist[i].pid); 
        }
        sleep(5);
    }
    return EXT_SUCCESS;
}
