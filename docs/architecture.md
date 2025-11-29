# JetECU-Core Architecture

## Overview

JetECU-Core is a platform-independent engine control unit for small jet (turbine) engines, written in C11.

## Layer Diagram

```
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
```

## Core Modules

### State Machine (`ecu_sm`)
- Defines engine states: OFF → PRESTART → SPINUP → IGNITION → RAMP → RUN → COOLDOWN → SHUTDOWN
- FAULT state reachable from any active state
- Entry/exit callbacks for state change side effects
- Timeout enforcement per state

### Sensor Processing (`ecu_sensor`)
- Range validation with clamping
- Rate-of-change limiting (dV/dt)
- Stuck-at detection (unchanged value counter)
- Cross-check (EGT vs RPM plausibility)

### PID Controller (`ecu_pid`)
- Generic PID with anti-windup
- Configurable gains from JSON config
- Used for RPM control in RUN state

### Fuel Scheduling (`ecu_fuel`)
- Rate-limited fuel output changes
- Emergency cutoff with positive shutoff
- Maximum fuel percentage enforcement

### Fault Manager (`ecu_fault`)
- Severity-based classification
- Latching for critical/fatal faults
- Retry policies for recoverable faults
- Degrade mode support
- Circular log buffer

### Lookup Maps (`ecu_map`)
- 1D linear interpolation tables
- 2D bilinear interpolation
- Used for fuel scheduling and EGT limits

## Configuration

JSON-based configuration loaded at startup. Supports:
- RPM/EGT thresholds
- Fuel parameters
- PID tuning gains
- Timeout values

## Telemetry

Binary frame protocol with:
- Sync bytes + CRC-16 validation
- Status messages (periodic sensor dump)
- Host commands (throttle, config read, fault read)
- Pluggable transport (UART, CAN, socket)

## Platforms

| Platform | Status | Notes |
|----------|--------|-------|
| POSIX    | Complete | Full simulator with engine model |
| ESP32    | HAL done | Needs testing on hardware |
| STM32F4  | HAL done | CAN bus support |
