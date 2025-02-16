cmake_minimum_required(VERSION 3.14)
project(JetECU VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler warnings
add_compile_options(-Wall -Wextra -Wpedantic)

# Core library
add_library(ecu_core
    core/ecu_sm.c
    core/ecu_sensor.c
    core/ecu_config.c
    core/ecu_json.c
    core/ecu_config_load.c
)
target_include_directories(ecu_core PUBLIC ${CMAKE_SOURCE_DIR})

# Tests
enable_testing()

add_executable(test_sm test/test_sm.c)
target_link_libraries(test_sm ecu_core)
add_test(NAME state_machine COMMAND test_sm)

add_executable(test_sensor test/test_sensor.c)
target_link_libraries(test_sensor ecu_core)
add_test(NAME sensor COMMAND test_sensor)

add_executable(test_config test/test_config.c)
target_link_libraries(test_config ecu_core)
add_test(NAME config COMMAND test_config)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
