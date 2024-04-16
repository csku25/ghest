/*
Filename: ghest.c
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "btstack.h"

int main() {
    stdio_init_all(); //needed for everything
    if (cyw43_arch_init()) { //specifically for Pico W
        printf("Error: Failed to intialize cyw43_arch!\n");
        return -1;
    }
}
