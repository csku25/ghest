/*
Filename: main.c
*/

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "btstack_run_loop.h"
#include "btstack.h"
#include "ghest.c"

int main() {
    stdio_init_all(); //needed for everything
    if (cyw43_arch_init()) { //specifically for Pico W
        printf("Error: Failed to intialize cyw43_arch!\n");
        return -1;
    }

    btstack_main(0, NULL);
    btstack_run_loop_execute();
}
