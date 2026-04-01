#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

#define EXT_ERR_FOPEN 1
#define EXT_ERR_FGETS 2
#define SLEEP_TIME 5

volatile int terminate = false;

static void handle_termination(int signum) 
{
	terminate = true;
}

static void init_service(void)
{
    signal(SIGINT, handle_termination);
    signal(SIGTERM, handle_termination);
}

static void run_main_loop(void)
{
	FILE *fd;
    char buffer[20];

    while (!terminate) 
    {
		fd = fopen("/proc/uptime", "r");
    	if (fd == NULL) {
        	fprintf(stderr, "Error - Unable to read /proc/uptime!\n");
        	exit(EXT_ERR_FOPEN);
    	}

    	if (fgets(buffer, sizeof(buffer), fd) == NULL) {
        	fprintf(stderr, "Error - Unable to read contents!\n");
        	fclose(fd);
        	exit(EXT_ERR_FGETS);
    	}

    	fclose(fd);

    	char *token = strtok(buffer, " ");
    	
        double uptime_seconds = strtod(token, NULL);
        int hours = (int)uptime_seconds / 3600;
        int minutes = ((int)uptime_seconds % 3600) / 60;
        int seconds = (int)uptime_seconds % 60;

        printf("Uptime: %dh %dm %ds\n", hours, minutes, seconds);

		sleep(SLEEP_TIME);
   	}
}

int main(void) 
{
	init_service();
    run_main_loop();
    return 0;
}
