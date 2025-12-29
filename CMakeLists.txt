cmake_minimum_required(VERSION 3.14)
project(JetECU VERSION 0.3.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)

option(ENABLE_SANITIZERS "Enable ASan + UBSan" OFF)
if(ENABLE_SANITIZERS)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

# Core library (platform-independent)
add_library(ecu_core
    core/ecu_sm.c
    core/ecu_sensor.c
    core/ecu_config.c
    core/ecu_json.c
    core/ecu_config_load.c
    core/ecu_pid.c
    core/ecu_fuel.c
    core/ecu_core.c
    core/ecu_map.c
    core/ecu_fault.c
    core/ecu_scheduler.c
)
target_include_directories(ecu_core PUBLIC ${CMAKE_SOURCE_DIR})

# Telemetry
add_library(ecu_telemetry
    telemetry/tel_frame.c
    telemetry/tel_send.c
)
target_include_directories(ecu_telemetry PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(ecu_telemetry ecu_core)

# POSIX HAL
add_library(hal_posix
    platform/posix/hal_adc_posix.c
    platform/posix/hal_gpio_posix.c
    platform/posix/hal_pwm_posix.c
    platform/posix/hal_timer_posix.c
    platform/posix/hal_nvram_posix.c
    platform/posix/hal_watchdog_posix.c
    platform/posix/hal_uart_posix.c
    platform/posix/hal_system_posix.c
)
target_include_directories(hal_posix PUBLIC ${CMAKE_SOURCE_DIR})

# C++ wrapper library
add_library(ecu_cpp
    cpp/jetecu/Engine.cpp
    cpp/jetecu/StateMachine.cpp
    cpp/jetecu/Pid.cpp
    cpp/jetecu/Sensor.cpp
    cpp/jetecu/FaultManager.cpp
    cpp/jetecu/Map.cpp
    cpp/jetecu/Scheduler.cpp
    cpp/jetecu/Config.cpp
    cpp/jetecu/Telemetry.cpp
)
target_include_directories(ecu_cpp PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(ecu_cpp PUBLIC ecu_core ecu_telemetry)

# Simulator
add_executable(jetecu_sim
    sim/sim_main.c
    sim/sim_engine.c
    sim/sim_replay.c
)
target_link_libraries(jetecu_sim ecu_core hal_posix)

# Tests
enable_testing()

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

macro(add_gtest_test name src)
    add_executable(${name} ${src})
    target_link_libraries(${name} ecu_cpp GTest::gtest_main)
    add_test(NAME ${name} COMMAND ${name})
endmacro()

macro(add_ecu_test name src)
    add_executable(${name} ${src})
    target_link_libraries(${name} ecu_core)
    add_test(NAME ${name} COMMAND ${name})
endmacro()

add_gtest_test(test_sm test/test_sm.cpp)
add_gtest_test(test_sensor test/test_sensor.cpp)
add_gtest_test(test_config test/test_config.cpp)
add_gtest_test(test_pid test/test_pid.cpp)
add_gtest_test(test_fuel test/test_fuel.cpp)
add_gtest_test(test_core test/test_core.cpp)
add_gtest_test(test_map test/test_map.cpp)
add_gtest_test(test_fault test/test_fault.cpp)
add_gtest_test(test_fuzz test/test_fuzz.cpp)
add_gtest_test(test_perf test/test_perf.cpp)
add_gtest_test(test_scheduler test/test_scheduler.cpp)

add_executable(test_telemetry test/test_telemetry.c)
target_link_libraries(test_telemetry ecu_telemetry)
add_test(NAME test_telemetry COMMAND test_telemetry)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Export compile commands for clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "JetECU v${PROJECT_VERSION} — Build type: ${CMAKE_BUILD_TYPE}")
