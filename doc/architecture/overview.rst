Architecture Overview
=====================

JetECU-Core is a platform-independent engine control unit for small jet
(turbine) engines, written in C11 with optional C++17 wrappers.

Layer Diagram
-------------

.. code-block:: text

   +-----------------------------+
   |     Application (main)      |
   +-----------------------------+
   |    Scheduler (ecu_sched)    |
   +-----------------------------+
   |    Telemetry (tel_frame)    |
   +-----------------------------+
   |       Core ECU Logic        |
   |  ecu_core  ecu_sm  ecu_pid |
   |  ecu_fuel  ecu_sensor      |
   |  ecu_fault ecu_map          |
   +-----------------------------+
   |    HAL (hardware abstract)  |
   |  ADC GPIO PWM Timer NVRAM   |
   |  Watchdog UART CAN          |
   +-----------------------------+
   |    Platform Port            |
   |  POSIX / ESP32 / STM32     |
   +-----------------------------+

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
