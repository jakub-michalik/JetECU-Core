# JetECU-Core

Platform-independent Engine Control Unit for small jet (turbine) engines.

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

## Next Steps

1. Define the repo directory layout (`core/`, `hal/`, `platform/`, `sim/`, `test/`, `config/`)
2. Implement the state machine skeleton in plain C
3. Write the HAL interface headers (ADC, GPIO, PWM, timer, NVRAM, watchdog)
4. Build the POSIX simulator port as the first platform target
5. Set up the test harness and write initial scenario tests (startup, nominal run, EGT fault)
6. Define the telemetry frame format and implement UART transport
7. Implement the configuration loader (JSON parser for PC, NVRAM reader for MCU)
8. Port to ESP32 as the first embedded target
9. Set up CI pipeline with the POSIX build + tests

---

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
