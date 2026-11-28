cmake_minimum_required(VERSION 3.20)
project(xrt LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(XRT_PORT "armv7m" CACHE STRING "Port to build: host, armv7m")

add_library(xrt STATIC
        src/xrt_kernel.c
        src/xrt_list.c
        src/xrt_mutex.c
        src/xrt_queue.c
        src/xrt_semaphore.c
        src/xrt_system_calls.c
        src/xrt_thread.c
)

target_include_directories(xrt
    PRIVATE src
    PUBLIC  inc
    PUBLIC  port
    PUBLIC  port/${XRT_PORT}
)

target_compile_options(xrt PRIVATE -Wall -Wextra)

if(XRT_PORT STREQUAL "host")
    target_sources(xrt PRIVATE port/host/xrt_port_host.c)
    target_compile_definitions(xrt PUBLIC XRT_PORT_HOST)
elseif(XRT_PORT STREQUAL "armv7m")
    if(NOT TARGET xrt_device)
        message(FATAL_ERROR "XRT_PORT=armv7m requires an 'xrt_device' INTERFACE target defined before add_subdirectory(xrt)")
    endif()
    target_sources(xrt PRIVATE port/armv7m/xrt_port_armv7m.c)
    target_compile_definitions(xrt PUBLIC XRT_PORT_ARMV7M)
    target_link_libraries(xrt PUBLIC xrt_device)
else()
    message(FATAL_ERROR "Unknown XRT_PORT: ${XRT_PORT}")
endif()