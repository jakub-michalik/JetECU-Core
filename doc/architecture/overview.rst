Architecture Overview
=====================

JetECU-Core is a platform-independent engine control unit for small jet
(turbine) engines, written in C11 with optional C++17 wrappers.

Layer Diagram
-------------

.. mermaid::

   flowchart TB
       App["Application (main)"]
       Sched["Scheduler<br/>(ecu_sched)"]
       Tel["Telemetry<br/>(tel_frame, tel_send)"]
       subgraph Core["Core ECU Logic"]
           direction LR
           ECU["ecu_core"]
           SM["ecu_sm"]
           PID["ecu_pid"]
           Fuel["ecu_fuel"]
           Sens["ecu_sensor"]
           Flt["ecu_fault"]
           Map["ecu_map"]
       end
       subgraph HAL["HAL (hardware abstraction)"]
           direction LR
           ADC["ADC"]
           GPIO["GPIO"]
           PWM["PWM"]
           Timer["Timer"]
           NVRAM["NVRAM"]
           WDT["Watchdog"]
           UART["UART"]
           CAN["CAN"]
       end
       subgraph Plat["Platform Port"]
           direction LR
           POSIX["POSIX (sim)"]
           ESP32["ESP32"]
           STM32["STM32F4"]
       end

       App --> Sched
       Sched --> Tel
       Sched --> Core
       Tel --> HAL
       Core --> HAL
       HAL --> Plat

       classDef core fill:#e3f2fd,stroke:#1976d2,stroke-width:1px,color:#0d47a1;
       classDef hal  fill:#fff3e0,stroke:#ef6c00,stroke-width:1px,color:#e65100;
       classDef plat fill:#f3e5f5,stroke:#6a1b9a,stroke-width:1px,color:#4a148c;
       class Core,ECU,SM,PID,Fuel,Sens,Flt,Map core;
       class HAL,ADC,GPIO,PWM,Timer,NVRAM,WDT,UART,CAN hal;
       class Plat,POSIX,ESP32,STM32 plat;

Module Dependencies
-------------------

The ECU step function orchestrates the core modules each control tick.
Solid arrows show data flow within a single 1 kHz tick.

.. mermaid::

   flowchart LR
       Sensors[/"Raw ADC samples"/] --> ecu_sensor
       ecu_sensor -->|"validated RPM/EGT/throttle"| ecu_core
       ecu_core --> ecu_sm
       ecu_sm -->|"requested fuel %"| ecu_pid
       ecu_map -->|"feed-forward<br/>fuel schedule"| ecu_pid
       ecu_pid -->|"valve command"| ecu_fuel
       ecu_fuel --> PWM[/"PWM duty"/]
       ecu_sensor -.->|"out-of-range"| ecu_fault
       ecu_sm -.->|"timeout"| ecu_fault
       ecu_fault -.->|"degrade/shutdown"| ecu_sm
       ecu_core -->|"snapshot"| tel_send
       tel_send --> UART[/"UART / CAN"/]

       classDef io fill:#eeeeee,stroke:#666,stroke-dasharray:3 3,color:#212121;
       class Sensors,PWM,UART io;

Layers
------

**Application** — top-level ``main()`` that initialises the ECU, registers
scheduler tasks, and enters the run loop.

**Scheduler** — deterministic fixed-priority cooperative scheduler with
worst-case execution time (WCET) tracking and overrun detection.

**Telemetry** — binary frame protocol with CRC-16 validation.  Supports
periodic status messages, host commands, and pluggable transport (UART,
CAN, socket).

**Core ECU Logic** — all platform-independent control logic:

- **State Machine** (``ecu_sm``) — deterministic start/run/shutdown cycle
  with entry/exit callbacks and per-state timeouts.
- **Sensor Processing** (``ecu_sensor``) — range, rate-of-change,
  stuck-at, and plausibility validation.
- **PID Controller** (``ecu_pid``) — RPM tracking with anti-windup.
- **Fuel Scheduling** (``ecu_fuel``) — rate-limited valve command with
  emergency cutoff.
- **Fault Manager** (``ecu_fault``) — severity-based fault reporting,
  latching, retry policies, and degrade mode.
- **Lookup Maps** (``ecu_map``) — 1-D linear and 2-D bilinear
  interpolation tables for fuel and EGT limits.

**HAL** — thin hardware abstraction (ADC, GPIO, PWM, timers, NVRAM,
watchdog, UART, CAN).

**Platform Ports** — concrete HAL implementations:

.. list-table::
   :header-rows: 1
   :widths: 20 20 60

   * - Platform
     - Status
     - Notes
   * - POSIX
     - Complete
     - Full simulator with engine model
   * - ESP32
     - HAL done
     - Needs hardware testing
   * - STM32F4
     - HAL done
     - CAN bus support

Design Principles
-----------------

- **No dynamic allocation** — all buffers are statically sized.
- **Zero hardware calls in core** — core logic only touches ``core/``
  headers; hardware access goes through the HAL.
- **Data-driven configuration** — all tuning parameters loaded from JSON
  at startup; no hardcoded constants in control logic.
- **Testable on PC** — full POSIX simulator with thermodynamic engine
  model and CSV replay, enabling unit, integration, fuzz, and performance
  tests without hardware.
