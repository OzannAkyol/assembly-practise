cmake_minimum_required(VERSION 3.20)
project(xrt LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(xrt STATIC
    xrt/src/xrt_kernel.c
    xrt/src/xrt_list.c
    xrt/src/xrt_mutex.c
    xrt/src/xrt_queue.c
    xrt/src/xrt_semaphore.c
    xrt/src/xrt_system_calls.c
    xrt/src/xrt_thread.c
    xrt/port/host/xrt_port_host.c
)

target_include_directories(xrt
    PRIVATE xrt/src
    PUBLIC  xrt/inc
    PUBLIC  xrt/port
    PUBLIC xrt/port/host
)

target_compile_definitions(xrt PUBLIC XRT_PORT_HOST)

target_compile_options(xrt PRIVATE -Wall -Wextra)