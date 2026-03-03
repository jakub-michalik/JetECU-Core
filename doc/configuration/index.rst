.. _configuration:

Configuration
=============

JetECU-Core uses a flat JSON file for all tuning parameters.  No
constants are hard-coded in the control logic — everything is data-driven
and can be changed without recompilation.

JSON Format
-----------

.. code-block:: json

   {
       "rpm_idle": 45000,
       "rpm_max": 120000,
       "rpm_start_target": 60000,
       "egt_max": 1100,
       "egt_start_min": 150,
       "fuel_start_pct": 8.0,
       "fuel_max_pct": 100.0,
       "fuel_ramp_rate": 5.0,
       "prestart_timeout_ms": 5000,
       "spinup_timeout_ms": 10000,
       "ignition_timeout_ms": 8000,
       "rpm_ramp_rate": 400,
       "pid_kp": 0.5,
       "pid_ki": 0.1,
       "pid_kd": 0.01
   }

The default configuration is stored in ``config/default.json``.

Parameters
----------

.. list-table::
   :header-rows: 1
   :widths: 28 12 60

   * - Parameter
     - Unit
     - Description
   * - ``rpm_idle``
     - RPM
     - Idle shaft speed
   * - ``rpm_max``
     - RPM
     - Maximum allowable RPM (overspeed threshold)
   * - ``rpm_start_target``
     - RPM
     - Target RPM for the startup sequence
   * - ``egt_max``
     - deg C
     - Absolute exhaust gas temperature limit
   * - ``egt_start_min``
     - deg C
     - Minimum EGT confirming successful ignition
   * - ``fuel_start_pct``
     - %
     - Fuel valve position during ignition phase
   * - ``fuel_max_pct``
     - %
     - Maximum fuel output
   * - ``fuel_ramp_rate``
     - %/s
     - Maximum rate of fuel change
   * - ``prestart_timeout_ms``
     - ms
     - Prestart phase timeout
   * - ``spinup_timeout_ms``
     - ms
     - Spinup phase timeout
   * - ``ignition_timeout_ms``
     - ms
     - Ignition phase timeout
   * - ``rpm_ramp_rate``
     - RPM/s
     - RPM acceleration rate during ramp
   * - ``pid_kp``
     - \—
     - PID proportional gain
   * - ``pid_ki``
     - \—
     - PID integral gain
   * - ``pid_kd``
     - \—
     - PID derivative gain

Loading
-------

On the POSIX simulator, configuration is loaded from a file path:

.. code-block:: c

   ecu_config_t cfg;
   ecu_config_load_file("config/default.json", &cfg);

On embedded targets, the JSON string can come from NVRAM or a host
command:

.. code-block:: c

   ecu_config_load_str(json_buf, &cfg);

Both functions parse the JSON and populate an ``ecu_config_t`` struct.
Unknown keys are silently ignored; missing keys retain their defaults
(from ``ecu_config_defaults()``).
