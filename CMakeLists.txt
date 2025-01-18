cmake_minimum_required(VERSION 3.14)
project(JetECU VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler warnings
add_compile_options(-Wall -Wextra -Wpedantic)

# Core library
add_library(ecu_core
    core/ecu_sm.c
)
target_include_directories(ecu_core PUBLIC ${CMAKE_SOURCE_DIR})

# Tests
enable_testing()

add_executable(test_sm test/test_sm.c)
target_link_libraries(test_sm ecu_core)
target_include_directories(test_sm PRIVATE ${CMAKE_SOURCE_DIR})
add_test(NAME state_machine COMMAND test_sm)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
