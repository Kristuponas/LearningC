#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <stdbool.h>
#include <signal.h>

#define SLEEP_TIME 10

// Based on: https://www.thinkbroadband.com/download
// SMALL FILE = 10MB || 20 MB, for 20MB time doubles
#define SMALL_FILE_VERY_SLOW 40     //  40s @ 2 Mbps 
#define SMALL_FILE_SLOW 10          //  10s @ 8 Mbps
#define SMALL_FILE_NORMAL 3         //   3s @ 30 Mbps
#define SMALL_FILE_FAST 1.5         // 1.5s @ 60 Mbps
#define SMALL_FILE_VERY_FAST 0.7    // 0.7s @ 120 Mbps

// MEDIUM FILE = 50MB || 100 MB, for 100MB time doubles
#define MEDIUM_FILE_VERY_SLOW 240   // 240s @ 2 Mbps 
#define MEDIUM_FILE_SLOW 60         //  60s @ 8 Mbps
#define MEDIUM_FILE_NORMAL 14       //  14s @ 30 Mbps
#define MEDIUM_FILE_FAST 7          //   7s @ 60 Mbps
#define MEDIUM_FILE_VERY_FAST 4     //   4s @ 120 Mbps

// Exit codes
#define EXT_ERR_USAGE 1
#define EXT_ERR_CURL_EASY_PERFORM 2

volatile bool terminate = false;

static void handle_termination(int signum)
{
    (void)signum;
    terminate = true;
}

static void init_service()
{
    signal(SIGINT, handle_termination);
    signal(SIGTERM, handle_termination);
}

static size_t discard_data (void *ptr, size_t size, size_t nmemb, void *userdata)
{
    (void)ptr;
    (void)userdata;
    return size * nmemb;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s file size MB (10 | 20 | 50 | 100)\n", argv[0]);
        exit(EXT_ERR_USAGE);
    }

    init_service();

    int size = atoi(argv[1]);
    if (size != 10 && size != 20 && size != 50 && size != 100)
    {
        fprintf(stderr, "Error - invalid size, use one of these: 10, 20, 50, 100\n");
        exit(EXT_ERR_USAGE);
    }

    char url[128];
    snprintf(url, sizeof(url), "http://ipv4.download.thinkbroadband.com/%sMB.zip", argv[1]);

    printf("URL: %s\n", url);

    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, discard_data);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            fprintf(stderr, "Error - curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            exit(EXT_ERR_CURL_EASY_PERFORM);
        }

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return EXIT_SUCCESS;
}
