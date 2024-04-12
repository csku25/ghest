/*
Filename: ghest.c
*/
#include <pico/stdlib.h>
#include <pico/cyw43_arch.h>
#include <btstack_run_loop.h>

int main() {
    stdio_init_all();
    if (cyw43_arch_init()) {
        printf("Wireless init failed");
        return -1;
    }
    btstack_main(0, NULL);
    btstack_run_loop_execute();
}