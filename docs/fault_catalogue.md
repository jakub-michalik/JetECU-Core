# JetECU Fault Catalogue

## Fault Codes

| Code   | Name           | Severity | Action   | Description |
|--------|----------------|----------|----------|-------------|
| 0x0001 | SENSOR_FAIL    | Critical | Shutdown | Sensor reading invalid during prestart |
| 0x0002 | PRESTART_TMO   | Critical | Shutdown | Prestart phase timeout |
| 0x0004 | SPINUP_TMO     | Critical | Shutdown | Spinup phase timeout — RPM not reached |
| 0x0008 | IGNITION_FAIL  | Critical | Shutdown | No EGT rise during ignition |
| 0x0010 | OVERSPEED      | Fatal    | Shutdown | RPM exceeded maximum limit |
| 0x0020 | OVERTEMP       | Fatal    | Shutdown | EGT exceeded limit for current RPM |
| 0x0040 | FUEL_CUTOFF    | Warning  | Degrade  | Fuel cutoff activated |
| 0x0080 | SENSOR_STUCK   | Warning  | Degrade  | Sensor value unchanging |
| 0x0100 | PLAUSIBILITY   | Warning  | Degrade  | EGT/RPM cross-check failed |

## Severity Levels

- **Info**: Logged but no action taken
- **Warning**: Degrade mode (reduce power), may auto-clear
- **Critical**: Latched, requires shutdown sequence
- **Fatal**: Immediate safe shutdown, cannot be cleared without reset

## Degrade Mode

When a warning-level fault is active:
1. Maximum throttle limited to 50%
2. RPM limit reduced by 20%
3. Additional sensor monitoring enabled
4. Telemetry sends fault notification to host

## Recovery

- Warning faults: auto-clear when condition resolves
- Critical faults with retry: up to 3 retry attempts
- Fatal faults: require power cycle
