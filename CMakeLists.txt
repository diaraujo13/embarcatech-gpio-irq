# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(gpio_irq C CXX ASM)


# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(gpio_irq
        gpio_irq.c
        )

pico_generate_pio_header(gpio_irq ${CMAKE_CURRENT_LIST_DIR}/pio_matrix.pio)

target_sources(gpio_irq PRIVATE gpio_irq.c)

# pull in common dependencies
target_link_libraries(gpio_irq pico_stdlib hardware_pio hardware_adc pico_bootrom)

target_include_directories(gpio_irq PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(gpio_irq)
# create map/bin/hex file etc.
pico_add_extra_outputs(gpio_irq)

# add url via pico_set_program_url
