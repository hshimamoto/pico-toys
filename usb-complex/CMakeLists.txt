cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

set(prog_name usb-complex)

project(${prog_name})
add_executable(${prog_name})

pico_sdk_init()

target_sources(${prog_name} PRIVATE main.c hid.c video.c usb_descriptors.c)
target_link_libraries(${prog_name} PRIVATE pico_stdlib pico_rand tinyusb_device tinyusb_board)
include_directories(${prog_name} PRIVATE ${CMAKE_CURRENT_LIST_DIR})

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(${prog_name})
