Scheduler
=========

Fixed-priority cooperative task scheduler with WCET tracking.

Task Timing
-----------

Tasks are registered with a period (ms) and run in priority order.
The diagram below shows a representative **100 ms slice** of nominal
execution; the 1 Hz housekeeping task (fault-log flush, NVRAM sync)
is not captured at this zoom level — it fires once per second outside
the window.

.. mermaid::

   gantt
       title Scheduler activity over a 100 ms slice
       dateFormat  X
       axisFormat  %L ms

       section 1 kHz control
       sm + pid + fuel - 100 x 1 ms ticks :active, ctrl, 0, 100

       section 100 Hz sensors
       validate :s1,  0,  1
       validate :s2,  10, 11
       validate :s3,  20, 21
       validate :s4,  30, 31
       validate :s5,  40, 41
       validate :s6,  50, 51
       validate :s7,  60, 61
       validate :s8,  70, 71
       validate :s9,  80, 81
       validate :s10, 90, 91

       section 50 Hz telemetry
       tel_send_status :t1, 0,  2
       tel_send_status :t2, 20, 22
       tel_send_status :t3, 40, 42
       tel_send_status :t4, 60, 62
       tel_send_status :t5, 80, 82

Canonical task rates:

.. list-table::
   :header-rows: 1
   :widths: 25 15 20 40

   * - Task
     - Period
     - Frequency
     - Notes
   * - Control loop (SM + PID + fuel)
     - 1 ms
     - 1 kHz
     - Hard-real-time; budget 800 us per tick.
   * - Sensor validation
     - 10 ms
     - 100 Hz
     - Range / rate / stuck-at checks downstream of DSP.
   * - Telemetry TX (``tel_send_status``)
     - 20 ms
     - 50 Hz
     - Periodic status frame to host.
   * - Watchdog kick
     - 100 ms
     - 10 Hz
     - Independent of control-loop overruns.
   * - Housekeeping (fault flush, NVRAM)
     - 1 s
     - 1 Hz
     - Low-priority background work.

WCET Overrun Path
-----------------

Each task records its measured worst-case execution time.  When a task
exceeds its declared budget the scheduler reports the overrun and the
fault manager decides whether to degrade or shut down.

.. mermaid::

   flowchart LR
       Tick([Timer tick]) --> Pick[Select highest-priority<br/>ready task]
       Pick --> Run[Run task]
       Run --> Meas{exec_us &gt; budget_us?}
       Meas -->|no| Done([Update WCET stats])
       Meas -->|yes| Report[ecu_fault_report<br/>FAULT_SCHED_OVERRUN]
       Report --> Done

Header: ``core/ecu_scheduler.h``

.. doxygengroup:: ecu_scheduler
   :content-only:
   :members:
