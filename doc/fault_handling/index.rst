Fault Handling
==============

The fault manager (``ecu_fault``) provides priority-based fault
reporting, latching, retry policies, and degrade mode support.

.. toctree::
   :maxdepth: 1

   catalogue

Severity Levels
---------------

.. list-table::
   :header-rows: 1
   :widths: 15 15 70

   * - Level
     - Action
     - Behaviour
   * - Info
     - None
     - Logged but no system action taken
   * - Warning
     - Degrade
     - Reduce power, may auto-clear when condition resolves
   * - Critical
     - Shutdown
     - Latched — requires safe shutdown sequence
   * - Fatal
     - Shutdown
     - Immediate safe shutdown, cannot clear without reset

Degrade Mode
------------

When a warning-level fault is active the system enters degrade mode:

1. Maximum throttle limited to 50 %
2. RPM limit reduced by 20 %
3. Additional sensor monitoring enabled
4. Telemetry sends fault notification to host

Recovery
--------

- **Warning faults** — auto-clear when the triggering condition resolves.
- **Critical faults with retry** — up to 3 retry attempts (configurable
  via :c:func:`ecu_fault_set_retries`).
- **Fatal faults** — require a power cycle.

Fault Log
---------

All reported faults are recorded in a circular log buffer
(``FAULT_LOG_SIZE = 32``).  Each entry includes the fault code,
severity, action, and timestamp.  The log can be read back via the
telemetry ``TEL_CMD_READ_FAULTS`` command for post-mortem analysis.
