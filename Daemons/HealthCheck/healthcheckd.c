#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define SLEEP_TIME 10

#define EXT_SUCCESS 0
#define EXT_ERR_TOO_FEW_ARGS 1
#define EXT_ERR_GETADDRINFO 2
#define EXT_ERR_SOCKET 3
#define EXT_ERR_FCNTL 4
#define EXT_ERR_CONNECT 5

typedef uint8_t u8;
typedef uint16_t u16;

typedef struct {
    char host[128];
    u16 port;
} HostInfo;

volatile int terminate = false;

static void handle_termination(int signum)
{
    (void)signum;
    terminate = true;
}

static void init_service(void)
{
    signal(SIGINT, handle_termination);
    signal(SIGTERM, handle_termination);
}

static bool check_host(HostInfo *hostInfo)
{
    struct addrinfo hint;
    struct addrinfo *result;

    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = 0;
    hint.ai_protocol = 0;
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_addr = NULL;

    char port_str[6];
    snprintf(port_str, sizeof(port_str), "%d", hostInfo->port);

    int status = getaddrinfo(hostInfo->host, port_str, &hint, &result);
    if (status != 0)
    {
        fprintf(stderr, "Error - getaddrinfo failure\n");
        exit(EXT_ERR_GETADDRINFO);
    }

    struct addrinfo *tmp = result;
    while (tmp != NULL)
    {
        printf("Entry:\n");
        printf("\tType: %i\n", tmp->ai_socktype);
        printf("\tFamily: %i\n", tmp->ai_family);
        
        char addr_string[INET6_ADDRSTRLEN];
        void *addr;

        if (tmp->ai_family == AF_INET)
            addr = &((struct sockaddr_in*)tmp->ai_addr)->sin_addr;
        else
            addr = &((struct sockaddr_in6*)tmp->ai_addr)->sin6_addr;
        
        inet_ntop(tmp->ai_family, addr, addr_string, sizeof(addr_string));

        printf("\tAddress: %s\n", addr_string);
        
        int sockfd = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
        if (sockfd == -1)
        {
            fprintf(stderr, "Error - socket failure\n");
            exit(EXT_ERR_SOCKET);
        }

        int flags = fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) | O_NONBLOCK); // Get flags
        if (flags == -1)
        {
            fprintf(stderr, "Error - fcntl failure\n");
            close(sockfd);
            exit(EXT_ERR_FCNTL);
        }

        int conn = connect(sockfd, tmp->ai_addr, tmp->ai_addrlen);
        if (conn == -1 && errno != EINPROGRESS) // Connection started, not done yet
        {
            fprintf(stderr, "Error - connect failure\n");
            close(sockfd);
            exit(EXT_ERR_CONNECT);
        }

        fd_set writefds;
        FD_ZERO(&writefds);
        FD_SET(sockfd, &writefds);

        struct timeval timeout;
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;

        int retval = select(sockfd + 1, NULL, &writefds, NULL, &timeout);
        if (retval == -1)
        {
            fprintf(stderr, "Error - select failure\n");
            close(sockfd);
            return false;
        }
        else if (retval == 0)
        {
            fprintf(stderr, "Error - timeout, host unreachable\n");
            close(sockfd);
            return false;
        }

        int error = 0;
        socklen_t errlen = sizeof(error);
        getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &errlen);

        if (error != 0)
        {
            fprintf(stderr, "Error - connection failed (getsockopt)\n");
            close(sockfd);
            return false;
        }
        
        close(sockfd);
        
        if (error == 0)
        {
            printf("\tConnection successful\n\n");
            freeaddrinfo(result);
            return true;
        }

        tmp = tmp->ai_next;
    }
    freeaddrinfo(result);
    return false;
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
        for (int i = 0; i < hosts; i++)
            check_host(&hostInfo[i]);
	    
        sleep(SLEEP_TIME);
    }
    exit(EXT_SUCCESS);
}
