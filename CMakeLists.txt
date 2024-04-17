cmake_minimum_required(VERSION 3.17)
set(PICO_SDK_PATH ~/Documents/pico/pico-sdk)
set(PICO_BOARD pico_w)

include(pico_sdk_import.cmake)
project(test_project C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()
add_executable(ghest main.c)

target_include_directories(ghest PRIVATE ${CMAKE_CURRENT_LIST_DIR})

pico_enable_stdio_uart(ghest 0)
pico_enable_stdio_usb(ghest 1)
add_definitions(-DPICO_STDIO_USB_CONNECTION_WITHOUT_DTR=1)

target_link_libraries(ghest
  pico_stdlib
  pico_cyw43_arch_none
  pico_btstack_classic
  pico_btstack_cyw43
)

pico_add_extra_outputs(ghest)
