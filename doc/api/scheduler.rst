Scheduler
=========

Fixed-priority cooperative task scheduler with WCET tracking.

Task Timing
-----------

Tasks are registered with a period (ms) and run in priority order.
The diagram below shows one second of nominal execution for the four
canonical task rates.

.. mermaid::

   gantt
       title Scheduler tick activity (1 s window, 1 kHz base rate)
       dateFormat  X
       axisFormat  %L ms

       section 1 kHz control
       sm + pid + fuel        :active, ctrl1, 0,    250
       sm + pid + fuel        :active, ctrl2, 250,  500
       sm + pid + fuel        :active, ctrl3, 500,  750
       sm + pid + fuel        :active, ctrl4, 750,  1000

       section 100 Hz sensors
       sensor validation      :sens1, 0,    100
       sensor validation      :sens2, 200,  300
       sensor validation      :sens3, 500,  600
       sensor validation      :sens4, 800,  900

       section 20 Hz telemetry
       tel_send_status        :tel1, 0,    50
       tel_send_status        :tel2, 500,  550

       section 1 Hz housekeeping
       fault log flush        :crit, hk1, 0,    20

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
