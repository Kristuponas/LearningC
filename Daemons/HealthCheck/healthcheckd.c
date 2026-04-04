#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>

#define EXT_SUCCESS 0
#define EXT_ERR_TOO_FEW_ARGS 1

typedef uint8_t u8;
typedef uint16_t u16;

typedef struct {
    char host[128];
    u16 port;
} HostInfo;

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

int main (int argc, char** argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "USAGE: ./healthcheckd <service(s):port(s)>\n");
        exit(EXT_ERR_TOO_FEW_ARGS);
    }

	init_service();

	HostInfo hostInfo[10];

	u8 hosts = argc - 1;
	for (int i = 0; i < hosts; i++)
    {
        char *dptr = strchr(argv[i + 1], ':');
        if (dptr != NULL)
        {
            size_t host_len = dptr - argv[i + 1];
            strncpy(hostInfo[i].host, argv[i + 1], host_len);
            hostInfo[i].host[host_len] = '\0';

			hostInfo[i].port = (u16)atoi(dptr + 1);

            printf("Host: %s, Port: %d\n", hostInfo[i].host, hostInfo[i].port);
        }
    }

	while (!terminate)
    {
	
    }
    exit(EXT_SUCCESS);
}
