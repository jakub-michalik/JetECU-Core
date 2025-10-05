cmake_minimum_required(VERSION 3.14)
project(JetECU VERSION 0.2.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)

option(ENABLE_SANITIZERS "Enable ASan + UBSan" OFF)
if(ENABLE_SANITIZERS)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

# Core library
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

# Simulator
add_executable(jetecu_sim
    sim/sim_main.c
    sim/sim_engine.c
    sim/sim_replay.c
)
target_link_libraries(jetecu_sim ecu_core hal_posix)

# Tests
enable_testing()

macro(add_ecu_test name src)
    add_executable(${name} ${src})
    target_link_libraries(${name} ecu_core)
    add_test(NAME ${name} COMMAND ${name})
endmacro()

add_ecu_test(test_sm test/test_sm.c)
add_ecu_test(test_sensor test/test_sensor.c)
add_ecu_test(test_config test/test_config.c)
add_ecu_test(test_pid test/test_pid.c)
add_ecu_test(test_fuel test/test_fuel.c)
add_ecu_test(test_core test/test_core.c)
add_ecu_test(test_map test/test_map.c)
add_ecu_test(test_fault test/test_fault.c)
add_ecu_test(test_fuzz test/test_fuzz.c)
add_ecu_test(test_perf test/test_perf.c)

add_executable(test_telemetry test/test_telemetry.c)
target_link_libraries(test_telemetry ecu_telemetry)
add_test(NAME test_telemetry COMMAND test_telemetry)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
