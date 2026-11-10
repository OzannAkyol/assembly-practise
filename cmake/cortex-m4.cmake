include(${CMAKE_CURRENT_LIST_DIR}/arm-none-eabi.cmake)

set(MCU_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=soft") # FPU off for now. 

set(CMAKE_C_FLAGS_INIT   "${MCU_FLAGS}")
set(CMAKE_ASM_FLAGS_INIT "${MCU_FLAGS}")