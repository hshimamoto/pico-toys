cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

set(prog_name blink_10_25)

project(${prog_name})

add_executable(${prog_name})

pico_sdk_init()

target_sources(${prog_name} PRIVATE main.c)
target_link_libraries(${prog_name} PRIVATE pico_stdlib)

pico_add_extra_outputs(${prog_name})
