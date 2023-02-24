cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

set(prog_name picoinfo)
project(${prog_name})

add_executable(${prog_name})

pico_sdk_init()

target_sources(${prog_name} PRIVATE main.c)
target_link_libraries(${prog_name} PRIVATE pico_stdlib hardware_clocks)

# enable usb output and uart output
pico_enable_stdio_usb(${prog_name} 1)
pico_enable_stdio_uart(${prog_name} 1)

pico_add_extra_outputs(${prog_name})
