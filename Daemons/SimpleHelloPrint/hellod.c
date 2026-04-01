#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

volatile int terminate = false;

static void handle_termination(int signum) {
    terminate = true;
}

static void init_service(void) {
    signal(SIGINT, handle_termination);
    signal(SIGTERM, handle_termination);
}

static void run_main_loop(void) {
    while(!terminate) {
        fprintf(stderr, "Hello!\n");
        sleep(1);
    }
}

int main(void) {
    init_service();
    run_main_loop();
    return 0;
}
