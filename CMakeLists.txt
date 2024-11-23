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
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Ghost C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(Ghost 
    ghost.c 
    command_sequence.c
    state_machine.c
    sas_token_generator.c
    config/A_configuration.c
    src/az_precondition.c
    src/az_iot_hub_client.c
    src/az_iot_hub_client_sas.c
    src/az_iot_hub_client_properties.c
    src/az_iot_common.c
    src/az_context.c
    src/az_span.c
    src/az_base64.c
    )

# Includi il file header generato nel progetto
target_include_directories(Ghost PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

pico_set_program_name(Ghost "Ghost")
pico_set_program_version(Ghost "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Ghost 1)
pico_enable_stdio_usb(Ghost 0)

# Add the standard library to the build
target_link_libraries(Ghost
        pico_stdlib 
        pico_multicore)

pico_add_extra_outputs(Ghost)

