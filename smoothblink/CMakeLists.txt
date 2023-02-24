cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

set(prog_name smoothblink)
project(${prog_name})

add_executable(${prog_name})

pico_sdk_init()

target_sources(${prog_name} PRIVATE main.c)
target_link_libraries(smoothblink PRIVATE pico_stdlib hardware_pwm)

pico_add_extra_outputs(smoothblink)
