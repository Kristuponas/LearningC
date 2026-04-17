#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

#define FLAG_VALUE 111

void core1_entry(void)
{
    multicore_fifo_push_blocking(FLAG_VALUE);

    uint32_t g = multicore_fifo_pop_blocking();

    if (g != FLAG_VALUE)
        printf("Not core1...\n");
    else
        printf("Hello from core0...\n");
    
    while(1)
        tight_loop_contents();
}

int main(void)
{
    stdio_init_all();
    sleep_ms(2000);
    printf("Multicore test\n");

    multicore_launch_core1(core1_entry);

    uint32_t g = multicore_fifo_pop_blocking();

    if (g != FLAG_VALUE)
        printf("Not core1...\n");
    else {
        multicore_fifo_push_blocking(FLAG_VALUE);
        printf("Hello from core1!\n");
    }

    sleep_ms(100);
    stdio_flush();
    while(1)
        tight_loop_contents();
}
