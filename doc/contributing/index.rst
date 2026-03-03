Contributing
============

Development Workflow
--------------------

1. Fork the repository and create a feature branch.
2. Make your changes — keep commits small and focused.
3. Ensure all tests pass:

   .. code-block:: bash

      cmake -B build -DCMAKE_BUILD_TYPE=Debug
      cmake --build build --parallel
      cd build && ctest --output-on-failure

4. Run clang-tidy (optional but recommended):

   .. code-block:: bash

      clang-tidy core/*.h core/*.c telemetry/*.h telemetry/*.c \
          -checks='bugprone-*' -warnings-as-errors='bugprone-*' \
          -- -Icore -Itelemetry -I.

5. Open a pull request against ``main``.

Code Style
----------

- **C** — C11, no compiler extensions.  All core logic must be
  platform-independent (no OS or hardware calls).
- **C++** — C++17.  Wrappers live in ``cpp/jetecu/`` and use the
  ``jetecu`` namespace.
- **Naming** — ``snake_case`` for C functions and types, ``PascalCase``
  for C++ classes, ``camelCase`` for C++ methods.
- **Include guards** — ``#ifndef FILE_H`` / ``#define FILE_H`` (no
  ``#pragma once``).
- **No dynamic allocation** in core or HAL code.
- **Comments** — Doxygen-style (``/** @brief ... */``) on all public
  API.

Project Layout
--------------

.. code-block:: text

   core/           Platform-independent C core
   telemetry/      Telemetry protocol (C)
   cpp/jetecu/     C++17 wrappers
   hal/            Hardware abstraction headers
   platform/       HAL implementations (POSIX, ESP32, STM32)
   sim/            PC simulator engine model
   test/           Test suites (Google Test)
   config/         Default JSON configuration
   doc/            Sphinx + Doxygen documentation

Adding a New Module
-------------------

1. Create ``core/ecu_<name>.h`` with Doxygen ``@defgroup``.
2. Add the implementation in ``core/ecu_<name>.c``.
3. Write tests in ``test/test_<name>.cpp``.
4. Add a Breathe page at ``doc/api/<name>.rst``.
5. Register the test in ``CMakeLists.txt``.

License
-------

JetECU-Core is licensed under Apache 2.0.  By submitting a pull request
you agree to license your contribution under the same terms.
