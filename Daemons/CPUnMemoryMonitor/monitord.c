#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>

#define EXT_ERR_FOPEN_MEM 1
#define EXT_ERR_FOPEN_CPU 2
#define SLEEP_TIME 10

volatile int terminate = false;

static void handle_termination(int signum) {
    terminate = true;
}

static void init_service(void) {
    signal(SIGINT, handle_termination);
    signal(SIGTERM, handle_termination);
}

int main(void)
{
    FILE *fp;
    
    char label[64];
    char line[128];
    long value;
    long mem_total = 0, mem_available = 0;
    long user, nice, system, idle, iowait, irq, softirq;

    init_service();

	while (!terminate) {
        // Memory stats

        fp = fopen("/proc/meminfo", "r");
        if (fp == NULL) {
            fprintf(stderr, "Error - Unable to read /proc/meminfo!\n");
            exit(EXT_ERR_FOPEN_MEM);
        }
    
        printf("Memory stats:\n\n");
        // sscanf returning 2 means the number of items it successfully parsed
        while (fgets(line, sizeof(line), fp)) {
            if (sscanf(line, "%s %ld", label, &value) == 2) {
                if (strcmp(label, "MemTotal:") == 0) mem_total = value;
                if (strcmp(label, "MemAvailable:") == 0) mem_available = value;
            }
        }
	    fclose(fp);
        fp = NULL;

        printf("Total: %ld kB\nAvailable: %ld kB\n", mem_total, mem_available);

        // CPU stats
    
        fp = fopen("/proc/stat", "r");

        if (fp == NULL) {
            fprintf(stderr, "Error - Unable to read /proc/stat!\n");
            exit(EXT_ERR_FOPEN_CPU);
        }

        fscanf(fp, "cpu %ld %ld %ld %ld %ld %ld %ld",
            &user, &nice, &system, &idle, &iowait, &irq, &softirq);
    
        fclose(fp);

        printf("\nCPU stats (values measured in jiffies (1/100th of a second)):\n\n");
        printf("User:    %ld - time running your programs.\n", user);
        printf("Nice:    %ld - time running low priority background processes.\n", nice);
        printf("System:  %ld - time the kernel itself was doing work.\n", system);
        printf("Idle:    %ld - time doing nothing (idling).\n", idle);
        printf("Iowait:  %ld - time waiting for disk/network operations.\n", iowait);
        printf("Irq:     %ld - time handling hardware interrupts (keyboard, mouse).\n", irq);
        printf("Softirq: %ld - time handling software interrupts.\n\n", softirq);
    
        sleep(SLEEP_TIME);
    }
    
    return 0;
}

