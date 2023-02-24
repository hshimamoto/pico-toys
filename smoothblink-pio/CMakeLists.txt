cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

set(prog_name smoothblink-pio)

project(${prog_name})

pico_sdk_init()

add_executable(${prog_name})

pico_generate_pio_header(${prog_name} ${CMAKE_CURRENT_LIST_DIR}/smoothblink.pio)

target_sources(${prog_name} PRIVATE main.c)

target_link_libraries(${prog_name} PRIVATE pico_stdlib hardware_pio)

pico_add_extra_outputs(${prog_name})
