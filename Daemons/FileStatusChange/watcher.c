#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <sys/inotify.h>

#define EXT_SUCCESS 0
#define EXT_ERR_TOO_FEW_ARGS 1
#define EXT_ERR_BASE_PATH 2
#define EXT_ERR_INIT_INOTIFY 3
#define EXT_ERR_ADD_WATCH 4

typedef int8_t i8;
typedef int16_t i16;
typedef uint32_t u32;

i8 IeventQueue = -1;
i8 IeventStatus = -1;

int main(int argc, char** argv) {
    char *basePath = NULL;
    char *token = NULL; 

    char buffer[4096];
    i16 readLength;

    const struct inotify_event* watchEvent;

    const u32 watchMask = IN_CREATE | IN_DELETE | IN_ACCESS | IN_CLOSE_WRITE | IN MODIFY | IN_MOVE_SELF;

    if (argc < 2) {
        fprintf(stderr, "Error - too few arguments. USAGE: watcher PATH");
        exit(EXT_ERR_TOO_FEW_ARGS);
    }

    basePath = (char *)malloc(sizeof(char)*strlen(argv[1]) + 1);
    strcpy(basePath, argv[1]);

    token = strtok(basePath, "/");

    while (token != NULL) {
        basePath = token;
        token = strtok(NULL, "/");
    }

    if (basePath == NULL) {
        fprintf(stderr, "Error - base path not defined!\n");
        exit(EXT_ERR_BASE_BATH);
    }

    IeventQueue = inotify_init();
    if (IeventQueue == -1) {
        fprintf(stderr, "Error - initialising inotify instance!\n");
        exit(EXT_ERR_INIT_INOTIFY);
    }

    IeventStatus = inotify_add_watch(IeventQueue, argv[1], watchMask);
    if (IeventStatus == -1) {
        fprintf(stderr, "Error - adding file to watch instance!\n");
        exit(EXT_ERR_ADD_WATCH);
    }

    while (1) {
        printf("Waiting for ievent")
    
        readLength = read(IeventQueue, buffer, sizeof(buffer));

        for (char *bufferPointer = buffer; bufferPointer < buffer + readLength; bufferPointer += sizeof(struct inotify_event + watchEvent->len)) {
            
        }
    }
    

    exit(EXT_SUCCESS);
}

























