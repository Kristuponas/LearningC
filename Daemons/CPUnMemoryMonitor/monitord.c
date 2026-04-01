#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#define EXT_ERR_FOPEN 1

int main(void)
{
	FILE *fd = fopen("/proc/meminfo", "r");

    long user, nice, system, idle, iowait, irq, softirq;

    if (fd == NULL) {
        fprintf(stderr, "Error - Unable to read /proc/meminfo!\n");
        exit(EXT_ERR_FOPEN);
    }

	fscanf(fd, "cpu %ld %ld %ld %ld %ld %ld %ld",
            &user, &nice, &system, &idle, &iowait, &irq, &softirq);
    fclose(fd);

    printf("cpu %ld %ld %ld %ld %ld %ld %ld\n",
            &user, &nice, &system, &idle, &iowait, &irq, &softirq);
    return 0;
}

