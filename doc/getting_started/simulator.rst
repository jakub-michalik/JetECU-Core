Simulator
=========

JetECU-Core includes a POSIX-based simulator that runs the full ECU
control loop against a thermodynamic engine model, allowing development
and testing without hardware.

Running
-------

.. code-block:: bash

   cd build
   ./jetecu_sim                               # default 30 s run
   ./jetecu_sim -d 60                         # 60 second run
   ./jetecu_sim -c ../config/default.json     # with config file
   ./jetecu_sim -r ../test/data/start_sequence.csv  # CSV replay
   ./jetecu_sim -q                            # quiet (no CSV output)

Output Format
-------------

The simulator prints CSV to stdout:

.. code-block:: text

   time_s,state,rpm,egt,fuel_pct,throttle,igniter,starter

This can be piped to a file or directly into a plotting tool.

Engine Model
------------

The built-in engine model (``sim/sim_engine.h``) simulates:

- Rotational inertia and aerodynamic drag
- Combustion efficiency proportional to fuel flow
- EGT thermal mass and cooling
- Starter torque injection

The model parameters are tuned for a small turbine in the 45k–120k RPM
range.

CSV Replay
----------

Replay mode (``-r``) reads a CSV file with columns:

.. code-block:: text

   time_s,rpm,egt,throttle

The replay interpolates between rows and feeds the values to
``ecu_step()`` at each time step, allowing post-flight analysis and
regression testing against recorded data.
