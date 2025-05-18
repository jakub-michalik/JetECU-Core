cmake_minimum_required(VERSION 3.14)
project(JetECU VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)

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
)
target_include_directories(ecu_core PUBLIC ${CMAKE_SOURCE_DIR})

# POSIX HAL
add_library(hal_posix
    platform/posix/hal_adc_posix.c
    platform/posix/hal_gpio_posix.c
    platform/posix/hal_pwm_posix.c
    platform/posix/hal_timer_posix.c
    platform/posix/hal_nvram_posix.c
    platform/posix/hal_watchdog_posix.c
    platform/posix/hal_uart_posix.c
)
target_include_directories(hal_posix PUBLIC ${CMAKE_SOURCE_DIR})

# Simulator
add_executable(jetecu_sim
    sim/sim_main.c
    sim/sim_engine.c
    sim/sim_replay.c
)
target_link_libraries(jetecu_sim ecu_core hal_posix)

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

add_executable(test_pid test/test_pid.c)
target_link_libraries(test_pid ecu_core)
add_test(NAME pid COMMAND test_pid)

add_executable(test_fuel test/test_fuel.c)
target_link_libraries(test_fuel ecu_core)
add_test(NAME fuel COMMAND test_fuel)

add_executable(test_core test/test_core.c)
target_link_libraries(test_core ecu_core)
add_test(NAME core COMMAND test_core)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
