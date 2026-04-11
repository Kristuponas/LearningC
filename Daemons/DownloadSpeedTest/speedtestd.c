#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <stdbool.h>
#include <signal.h>

#define SLEEP_TIME 10

// https://speed.cloudflare.com/ 
// Speed thresholds in MB/s
#define SPEED_VERY_FAST 50.0
#define SPEED_FAST      12.5
#define SPEED_MEDIUM     6.25
#define SPEED_SLOW       1.25

// Exit codes
#define EXT_ERR_USAGE 1
#define EXT_ERR_CURL_EASY_PERFORM 2

typedef struct {
    double threshold;
    const char *label;
} SpeedRating;

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

static void print_speed_rating(double total_time, int size_mb)
{
    SpeedRating ratings[] = {
        { (double)size_mb / SPEED_VERY_FAST, "Very fast"  },
        { (double)size_mb / SPEED_FAST,      "Fast"       },
        { (double)size_mb / SPEED_MEDIUM,    "Medium"     },
        { (double)size_mb / SPEED_SLOW,      "Slow"       },
        { __DBL_MAX__,                       "Very slow"  },
    };

    for (int i = 0; i < 5; i++)
    {
        if (total_time <= ratings[i].threshold)
        {
            printf("Download speed: %s\n", ratings[i].label);
            return;
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s file size MB\n", argv[0]);
        exit(EXT_ERR_USAGE);
    }

    init_service();

    int size = atoi(argv[1]);
    if (size <= 0)
    {
        fprintf(stderr, "Error - invalid size\n");
        exit(EXT_ERR_USAGE);
    }

    char url[128];
    snprintf(url, sizeof(url), "https://speed.cloudflare.com/__down?bytes=%lld", (long long)size * 1024 * 1024);

    printf("URL: %s\n", url);

    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, discard_data);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");

        res = curl_easy_perform(curl);

        printf("CURLcode: %d\n", res);
        if (res != CURLE_OK)
        {
            fprintf(stderr, "Error - curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            exit(EXT_ERR_CURL_EASY_PERFORM);
        }

        curl_off_t speed_download;
        curl_off_t bytes_downloaded;
        double total_time;
        long http_code = 0;

        curl_easy_getinfo(curl, CURLINFO_SPEED_DOWNLOAD_T, &speed_download);
        curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total_time);
        curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD_T, &bytes_downloaded);
        
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code != 200)
        {
            fprintf(stderr, "Error - HTTP %ld\n", http_code);
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            exit(EXT_ERR_CURL_EASY_PERFORM);
        }

        printf("Speed: %.2f MB/s\n", (double)speed_download / 1024 / 1024);
        printf("Time: %.2f seconds\n", total_time);
        printf("Downloaded: %.2f MB\n", (double)bytes_downloaded / 1024 / 1024);

        curl_easy_cleanup(curl);

        print_speed_rating(total_time, size);
    }

    curl_global_cleanup();

    return EXIT_SUCCESS;
}
