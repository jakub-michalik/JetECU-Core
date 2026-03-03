Testing
=======

JetECU-Core ships with 12 test suites covering unit, integration, fuzz,
and performance scenarios.

Running Tests
-------------

.. code-block:: bash

   cd build
   ctest --output-on-failure

Or run individual suites:

.. code-block:: bash

   ./test_sm          # state machine
   ./test_sensor      # sensor validation
   ./test_pid         # PID controller
   ./test_fuel        # fuel scheduling
   ./test_fault       # fault manager
   ./test_core        # full ECU integration
   ./test_map         # lookup maps
   ./test_config      # configuration loading
   ./test_scheduler   # task scheduler
   ./test_telemetry   # telemetry framing
   ./test_fuzz        # random/adversarial inputs
   ./test_perf        # timing / WCET

Test Categories
---------------

.. list-table::
   :header-rows: 1
   :widths: 25 75

   * - Category
     - Scope
   * - Unit tests
     - Core logic functions in isolation
   * - Integration tests
     - Multi-module interaction (ECU step)
   * - Scenario tests
     - Full start -> run -> fault -> shutdown
   * - Fuzz tests
     - Random and malformed inputs
   * - Performance tests
     - Loop timing, worst-case execution

CI Matrix
---------

The GitHub Actions CI runs the full test suite across:

- **GCC** (Ubuntu latest, Debug)
- **Clang** (Ubuntu latest, Debug)
- **Sanitizers** — Address Sanitizer + Undefined Behavior Sanitizer
- **Static analysis** — ``clang-tidy`` on ``core/``, ``telemetry/``, ``cpp/``

See ``.github/workflows/ci.yml`` for the full configuration.
