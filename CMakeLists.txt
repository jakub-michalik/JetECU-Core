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

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
