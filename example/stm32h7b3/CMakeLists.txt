cmake_minimum_required(VERSION 3.21)
project(xrt_stm32h7b3 LANGUAGES C ASM)

set(CMAKE_C_STANDARD 11)

# Build strategy flags: apply to every target in this directory and below (incl. xrt)
add_compile_options(-ffunction-sections -fdata-sections)

file(GLOB HAL_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32H7xx_HAL_Driver/Src/*.c)

add_executable(stm32h7b3
    Core/Src/main.c
    Core/Src/stm32h7xx_it.c
    Core/Src/stm32h7xx_hal_msp.c
    Core/Src/system_stm32h7xx.c
    Core/Src/syscalls.c
    Core/Src/sysmem.c
    Core/Src/xrt_app.c
    Core/Startup/startup_stm32h7b3iikxq.s
    ${HAL_SOURCES}
)

# Single source of truth for device info, shared by the application and xrt
add_library(xrt_device INTERFACE)
target_include_directories(xrt_device INTERFACE
    Drivers/CMSIS/Include
    Drivers/CMSIS/Device/ST/STM32H7xx/Include
)
target_compile_definitions(xrt_device INTERFACE
    STM32H7B3xxQ
    XRT_PORT_DEVICE_HEADER="stm32h7xx.h"
)

set(XRT_PORT "cm7" CACHE STRING "")
add_subdirectory(../../xrt xrt)

target_link_libraries(stm32h7b3 PRIVATE xrt_device xrt)

target_compile_definitions(stm32h7b3 PRIVATE USE_HAL_DRIVER USE_PWR_DIRECT_SMPS_SUPPLY)

target_include_directories(stm32h7b3 PRIVATE
    Core/Inc
    Drivers/STM32H7xx_HAL_Driver/Inc
)

target_compile_options(stm32h7b3 PRIVATE -Wall)

target_link_options(stm32h7b3 PRIVATE
    -T${CMAKE_CURRENT_SOURCE_DIR}/STM32H7B3IIKXQ_FLASH.ld
    --specs=nano.specs
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/stm32h7b3.map
)

set_target_properties(stm32h7b3 PROPERTIES SUFFIX ".elf")

add_custom_command(TARGET stm32h7b3 POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:stm32h7b3> ${CMAKE_CURRENT_BINARY_DIR}/stm32h7b3.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:stm32h7b3> ${CMAKE_CURRENT_BINARY_DIR}/stm32h7b3.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:stm32h7b3>
    COMMENT "Generating hex/bin"
)