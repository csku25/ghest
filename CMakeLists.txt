cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)
project(test_project C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()
add_executable(ghest
  ghest.c
)

target_include_directories(ghest PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Uncomment if you want to flash your board via picotool.
# pico_enable_stdio_usb(ghest 1)
pico_add_extra_outputs(ghest)

target_link_libraries(ghest
  pico_stdlib
  # Uncomment when using Pico W
  pico_cyw43_arch_none
  pico_btstack_classic
  pico_btstack_cyw43
)

target_link_libraries(pico_stdlib)
