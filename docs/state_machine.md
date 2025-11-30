# JetECU State Machine

## State Transition Diagram

```
                    +-------+
                    |  OFF  |<---------+
                    +---+---+          |
                        |              |
                   throttle > 5%       |
                        |              |
                    +---v---+          |
                    |PRESTART|         |
                    +---+---+          |
                        |              |
                  sensors OK           |
                        |              |
                    +---v---+      +---+----+
                    | SPINUP|----->|SHUTDOWN|
                    +---+---+      +--------+
                        |              ^
                  RPM > 30% idle       |
                        |              |
                    +---v----+     +---+---+
                    |IGNITION|---->| FAULT |
                    +---+----+     +-------+
                        |              ^
                  EGT > start_min      |
                        |              |
                    +---v---+          |
                    | RAMP  |----------+
                    +---+---+     (overspeed/
                        |          overtemp)
                  RPM > 95% target     |
                        |              |
                    +---v---+          |
                    |  RUN  |----------+
                    +---+---+
                        |
                  throttle < 1%
                        |
                    +---v----+
                    |COOLDOWN|
                    +---+----+
                        |
                  EGT < 100C
                        |
                    +---v----+
                    |SHUTDOWN|
                    +--------+
```

## State Details

| State    | Entry Condition | Exit Condition | Timeout | Outputs |
|----------|----------------|----------------|---------|---------|
| OFF      | Power on / shutdown complete | Throttle > 5% | None | All off |
| PRESTART | Throttle applied | Sensors valid | 5s | Sensor check |
| SPINUP   | Sensors OK | RPM > 30% idle | 10s | Starter ON |
| IGNITION | RPM threshold met | EGT > start_min | 8s | Starter + Igniter + Fuel |
| RAMP     | Ignition confirmed | RPM > 95% target | None | PID fuel control |
| RUN      | RPM target reached | Throttle < 1% | None | PID fuel control |
| COOLDOWN | Throttle removed | EGT < 100°C | None | Fuel OFF |
| SHUTDOWN | EGT cooled | Always → OFF | None | All off |
| FAULT    | Any safety violation | Manual → SHUTDOWN | None | All off |
