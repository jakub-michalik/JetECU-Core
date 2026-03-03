# JetECU-Core

[![CI](https://github.com/jakub-michalik/JetECU-Core/actions/workflows/ci.yml/badge.svg)](https://github.com/jakub-michalik/JetECU-Core/actions/workflows/ci.yml)
[![Docs](https://github.com/jakub-michalik/JetECU-Core/actions/workflows/docs.yml/badge.svg)](https://jakub-michalik.github.io/JetECU-Core/)

Platform-independent Engine Control Unit for small jet (turbine) engines.

**[Read the documentation](https://jakub-michalik.github.io/JetECU-Core/)**

## Motivation

After reviewing existing open-source jet ECU projects (Arduino-based, ESP32-based, etc.), a clear pattern emerges — they all share the same fundamental limitations:

**What they do well:**
- RPM + EGT (Exhaust Gas Temperature) readout
- Basic startup sequence
- Fuel valve control
- Simple watchdog / safety thresholds

**What they all lack:**
- Hardware abstraction layer (HAL) — code is welded to one platform
- PC simulator — debugging only possible on live hardware
- Formal state machine — startup/run/fault logic is ad-hoc
- Unit / scenario / fuzz tests
- Data-driven configuration and calibration
- Robust fault handling with degraded modes
- Proper telemetry protocol
- Modular architecture
- Any path toward certification or qualification

None of the existing projects provide a real foundation for a portable, testable, certifiable ECU core.
JetECU-Core is designed to fill that gap from day one.

---

## Architecture Overview

### Layered Design

```
┌─────────────────────────────────┐
│        Application / UI         │
├─────────────────────────────────┤
│     Telemetry + Comms Layer     │
│   (UART / CAN / USB / CBOR)    │
├─────────────────────────────────┤
│        Core ECU Logic           │
│  State Machine · PID · Faults   │
│  Fuel Scheduling · Rate Limit   │
├─────────────────────────────────┤
│    HAL (Hardware Abstraction)   │
│  ADC · GPIO · PWM · Timers     │
│  NVRAM · Watchdog               │
├─────────────────────────────────┤
│        Platform Ports           │
│  ESP32 · STM32 · POSIX · ...   │
└─────────────────────────────────┘
```

**A) Core ECU Logic** — platform-independent C, no hardware calls.
PID / control algorithms, state machine, fault detection & protections, fuel scheduling, rate limiting.

**B) HAL** — thin abstraction over hardware peripherals.
ADC, GPIO, PWM/servo, timers, NVRAM/filesystem, watchdog.

**C) Platform Ports** — concrete HAL implementations.
ESP32/Arduino, STM32 (HAL/LL), POSIX simulator, Teensy, Linux SBC.

**D) Telemetry + Comms** — structured protocol, not `Serial.println`.
UART / CAN / USB transport, binary framing (CBOR), CRC + versioning, host ↔ ECU commands, timestamped error log.

---

## Why C and C++

JetECU-Core uses a deliberate two-language architecture. The split follows a simple rule: everything that might run on a microcontroller is C; everything that exists purely for developer convenience is C++.

### Written in C (C11)

| Layer | Directory | Reason |
|-------|-----------|--------|
| Core ECU logic | `core/` | State machine, PID, fuel scheduling, sensor validation, fault manager, scheduler, config, lookup maps |
| HAL interfaces | `hal/` | Abstract hardware API (GPIO, ADC, PWM, UART, timers, watchdog, NVRAM, CAN) |
| Platform ports | `platform/` | Concrete HAL implementations for STM32, ESP32, and POSIX simulator |
| Telemetry | `telemetry/` | Binary framing protocol and transmission |
| Simulator | `sim/` | PC-based engine model and CSV replay |

**Why C for all of the above:**
- **Portability** — compiles on every embedded toolchain without fuss. No name mangling, no ABI surprises.
- **Zero runtime overhead** — no vtables, exceptions, or RTTI. Critical for a 1 kHz control loop on a Cortex-M4.
- **No heap allocation** — all core structures are fixed-size. Deterministic memory usage is a hard requirement.
- **Certification path** — C is the standard language for safety-critical and avionics software (DO-178C, MISRA). Starting in C keeps that door open.
- **Tooling** — static analyzers, MISRA checkers, and formal verification tools have the deepest C support.

### Written in C++ (C++17)

| Layer | Directory | Reason |
|-------|-----------|--------|
| Wrapper classes | `cpp/jetecu/` | `Engine`, `Pid`, `Sensor`, `Fuel`, `FaultManager`, `Map1D`/`Map2D`, `Scheduler`, `Config`, `Telemetry`, `StateMachine` |
| Test suites | `test/` | Unit, integration, fuzz, and performance tests using Google Test |

**Why C++ for the above:**
- **RAII** — resource-safe patterns like `Fuel::CutoffGuard` that guarantee cleanup even if something goes wrong.
- **Ergonomics** — `std::function` callbacks, initializer-list construction for maps, range-based iteration over scheduler tasks.
- **Google Test** — the test framework is C++; writing tests in C++ gives access to `EXPECT_*` / `ASSERT_*` macros and fixtures.
- **Optional layer** — the C++ wrappers are never required. Embedded targets can link `ecu_core` directly and skip C++ entirely.

Every C++ wrapper holds its C struct by value and exposes it via `raw()` — composition, not inheritance. The core has zero knowledge of whether C++ wrappers exist.

---

## State Machine

| State       | Description                        | Transitions              |
|-------------|------------------------------------|--------------------------|
| `OFF`       | Controller inactive                | → `PRESTART`             |
| `PRESTART`  | Sensor warmup, ADC self-check      | → `SPINUP` / `FAULT`    |
| `SPINUP`    | Spool up to minimum RPM            | → `IGNITION` / `FAULT`  |
| `IGNITION`  | Igniter on, first fuel             | → `RAMP` / `FAULT`      |
| `RAMP`      | Accelerate to target power         | → `RUN` / `FAULT`       |
| `RUN`       | Stable operation                   | → `COOLDOWN` / `FAULT`  |
| `COOLDOWN`  | Lower EGT before shutdown          | → `SHUTDOWN`             |
| `SHUTDOWN`  | Engine stopped, outputs safe       | → `OFF`                 |
| `FAULT`     | Any critical failure               | → `SHUTDOWN` (safe stop) |

Every state enforces:
- entry/exit actions
- timeouts
- safe exit paths
- priority-based transition rules

---

## Configuration — Data, Not Code

No more hardcoded constants scattered across source files.

```json
{
  "rpm_idle": 45000,
  "rpm_start_target": 60000,
  "egt_max": 1100,
  "scaler_fuel": 1.0,
  "rpm_ramp_rate": 400
}
```

Loaded from:
- JSON file on PC simulator
- NVRAM / flash on MCU
- Host command at runtime

This makes tuning, profiling, and multi-engine support trivial.

---

## PC Simulator & Replay

Instead of debugging on live hardware:
- Simulate the full engine cycle on a PC
- Drive throttle input from a file or script
- Replay real flight logs through `ecu_step(inputs, dt)`
- Compare outputs against expected baselines

This enables:
- Unit tests on core logic
- Regression tests on every commit
- Fuzz testing with random / adversarial inputs
- Zero risk to hardware

---

## Sensor Model

Raw ADC reads with threshold comparisons are not enough.

```c
typedef struct {
    float value;
    bool  valid;
    uint32_t faults;  // bitmask
} SensorReading;
```

Every sensor reading goes through:
- Range validation
- Rate-of-change limiting
- Plausibility cross-checks
- Stuck-at detection
- Signal filtering

---

## Lookup Maps

Beyond simple PID + thresholds:
- 2D maps (e.g. RPM vs EGT → fuel command)
- 3D maps where needed
- Bilinear interpolation
- Swappable performance profiles

---

## Fault Handling

Not just `if (egt > max) → shutdown`.

- Contextual reactions (different response per state)
- Fault latch with description, timestamp, severity
- Priority-based fault arbitration
- Retry policies where safe
- Degraded operating modes
- Guaranteed safe-stop path
- Fault matrix designed with future certification in mind

---

## Deterministic Scheduler

`loop()` with WiFi yields and random delays is not acceptable for engine control.

| Task              | Frequency | Priority |
|-------------------|-----------|----------|
| Control loop      | 1 kHz     | Critical |
| Telemetry TX      | 50 Hz     | Medium   |
| Housekeeping      | 1 Hz      | Low      |
| Watchdog kick     | 10 Hz     | Critical |

No dynamic memory allocation. Deterministic timing.

---

## Test Suite

| Category         | Scope                                    |
|------------------|------------------------------------------|
| Unit tests       | Core logic functions in isolation         |
| Integration tests| Multi-module interaction                  |
| Scenario tests   | Full start → run → fault → shutdown      |
| Fuzz tests       | Random / malformed inputs                |
| Performance tests| Loop timing, worst-case execution        |

---

## CI/CD

- GitHub Actions
- Test matrix: POSIX (GCC + Clang + sanitizers), cross-compile STM32, cross-compile ESP32
- Static analysis (clang-tidy)
- Build + test on every push

---

## Roadmap

| Milestone | Scope |
|-----------|-------|
| **v0.1**  | Core logic + PC simulator + JSON config + state machine + unit tests |
| **v0.2**  | HAL layer + ESP32 port + basic telemetry |
| **v0.3**  | STM32 port + CAN bus telemetry + lookup maps |
| **v1.0**  | Certification-grade core — full test coverage, fault matrix, documentation |

---

## Current Status (v0.3)

- Core logic: state machine, PID, fuel scheduling, sensor validation
- Fault manager with severity, latching, retry, and degrade modes
- Lookup maps (1D, 2D) with interpolation
- POSIX simulator with engine model and CSV replay
- Telemetry protocol with CRC-16 framing
- ESP32 HAL implementation
- STM32F4 HAL implementation with CAN bus
- Static task scheduler with overrun detection
- 12 test suites: unit, integration, fuzz, performance
- CI: GCC, Clang, sanitizers, clang-tidy
- Architecture and fault documentation

## C++ API

The `cpp/jetecu/` directory provides modern C++17 wrappers around the C core:

```cpp
#include "cpp/jetecu/Engine.h"

jetecu::Engine engine;
jetecu::Inputs in;
in.throttle = 80.0f;
in.rpm = 50000.0f;
in.egt = 600.0f;

auto out = engine.step(in, 0.001f);
printf("state=%s fuel=%.1f%%\n", engine.stateName(), out.fuel_pct);
```

Available wrapper classes:

| Class | Wraps | Key Methods |
|-------|-------|-------------|
| `Engine` | `ecu_t` | `step()`, `state()`, `stateName()` |
| `StateMachine` | `ecu_sm_t` | `transition()`, `setCallbacks()` |
| `Pid` | `ecu_pid_t` | `update()`, `reset()` |
| `Sensor` | `ecu_sensor_*` | `validate()`, `egtPlausible()` |
| `FaultManager` | `ecu_fault_mgr_t` | `report()`, `clear()`, `isActive()` |
| `Map1D` / `Map2D` | `ecu_map*_t` | `lookup()` with initializer_list construction |
| `Scheduler` | `ecu_scheduler_t` | `add()` with `std::function`, `tick()` |
| `Config` | `ecu_config_t` | `loadFile()`, `loadString()` |
| `Telemetry` | `tel_ctx_t` | `sendStatus()`, `processByte()` |

All wrappers expose `raw()` for direct access to the underlying C struct.

### Testing with Google Test

Tests use [Google Test](https://github.com/google/googletest) (fetched via CMake FetchContent):

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
cd build && ctest --output-on-failure
```

Each test suite covers both the C API directly and the C++ wrapper classes.

---

## Next Steps

1. Hardware testing on ESP32 and STM32
2. Full test coverage audit
3. CAN telemetry transport implementation
4. v1.0 certification-grade release

---

## Fault Handling

The fault manager provides:
- **Severity levels**: Info, Warning, Critical, Fatal
- **Latching**: Critical/Fatal faults cannot be cleared without reset
- **Priority arbitration**: Highest severity determines system action
- **Retry policies**: Configurable retry count for recoverable faults
- **Degrade modes**: Warning faults reduce power instead of shutdown
- **Fault log**: Circular buffer with timestamps for post-mortem analysis

---

## Simulator

Run the POSIX simulator:
```bash
cd build
./jetecu_sim                        # default 30s run
./jetecu_sim -d 60                  # 60 second run
./jetecu_sim -c ../config/default.json  # with config
./jetecu_sim -r ../test/data/start_sequence.csv  # replay mode
./jetecu_sim -q                     # quiet (no CSV output)
```

Output is CSV: `time_s,state,rpm,egt,fuel_pct,throttle,igniter,starter`

---

## Testing

The test suite covers:
- State machine transitions and edge cases
- Sensor validation (range, rate, stuck-at)
- Configuration loading and defaults
- PID controller behavior
- Fuel scheduling and safety cutoff
- Full ECU step integration (start, fault, overspeed)

## Building

```bash
mkdir build && cd build
cmake ..
make
```

### Running Tests

```bash
cd build
ctest --output-on-failure
```

---

## License

Apache 2.0
