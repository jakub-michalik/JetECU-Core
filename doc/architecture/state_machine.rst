State Machine
=============

The engine lifecycle is managed by a deterministic state machine
(``ecu_sm``).  Each state enforces entry/exit actions, timeouts, and
safe exit paths.

State Transition Diagram
------------------------

.. code-block:: text

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

States
------

.. list-table::
   :header-rows: 1
   :widths: 12 22 22 10 22

   * - State
     - Entry Condition
     - Exit Condition
     - Timeout
     - Outputs
   * - OFF
     - Power on / shutdown complete
     - Throttle > 5 %
     - None
     - All off
   * - PRESTART
     - Throttle applied
     - Sensors valid
     - 5 s
     - Sensor check
   * - SPINUP
     - Sensors OK
     - RPM > 30 % idle
     - 10 s
     - Starter ON
   * - IGNITION
     - RPM threshold met
     - EGT > start_min
     - 8 s
     - Starter + Igniter + Fuel
   * - RAMP
     - Ignition confirmed
     - RPM > 95 % target
     - None
     - PID fuel control
   * - RUN
     - RPM target reached
     - Throttle < 1 %
     - None
     - PID fuel control
   * - COOLDOWN
     - Throttle removed
     - EGT < 100 deg C
     - None
     - Fuel OFF
   * - SHUTDOWN
     - EGT cooled
     - Always -> OFF
     - None
     - All off
   * - FAULT
     - Any safety violation
     - Manual -> SHUTDOWN
     - None
     - All off

Timeout Handling
----------------

Each phase with a finite timeout (PRESTART, SPINUP, IGNITION) will
transition to FAULT if the exit condition is not met within the allowed
window.  Timeout values are set in the engine configuration
(:ref:`configuration <configuration>`).

Callbacks
---------

The state machine supports entry and exit callbacks via
:c:func:`ecu_sm_set_callbacks`.  These are used by the core loop to
activate or deactivate hardware (starter motor, igniter relay, fuel
valve) at the boundaries of each state.
