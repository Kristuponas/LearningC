#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <curl/curl.h>

#define SLEEP_TIME 10

#define EXT_ERR_USAGE 1
#define EXT_ERR_CURL_EASY_PERFORM 2

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s file size MB (10/20/50/100...)\n", argv[0]);
        exit(EXT_ERR_USAGE);
    }

    int size = atoi(argv[1]);
    if (size <= 0)
    {
        fprintf(stderr, "Error - invalid size, use a number like 10, 20, 50\n");
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
