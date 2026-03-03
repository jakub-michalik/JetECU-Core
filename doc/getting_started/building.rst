Building
========

Prerequisites
-------------

- **CMake** >= 3.14
- **GCC** >= 10 or **Clang** >= 13 (C11 + C++17)
- **Google Test** is fetched automatically via CMake ``FetchContent``

Quick Build
-----------

.. code-block:: bash

   git clone https://github.com/jakub-michalik/JetECU-Core.git
   cd JetECU-Core
   cmake -B build -DCMAKE_BUILD_TYPE=Debug
   cmake --build build --parallel

This produces:

- ``build/jetecu_sim`` — the POSIX simulator
- ``build/test_*`` — individual test executables

Build Types
-----------

.. list-table::
   :header-rows: 1
   :widths: 25 75

   * - Type
     - Description
   * - ``Debug``
     - Full debug info, assertions enabled
   * - ``Release``
     - Optimised, no debug info
   * - ``RelWithDebInfo``
     - Optimised with debug info
   * - ``Asan``
     - Address Sanitizer + Undefined Behavior Sanitizer

To build with sanitizers:

.. code-block:: bash

   cmake -B build -DCMAKE_BUILD_TYPE=Debug \
         -DCMAKE_C_FLAGS="-fsanitize=address,undefined" \
         -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined"
   cmake --build build --parallel

Cross-Compilation
-----------------

The core library (``core/`` and ``telemetry/``) has no OS or hardware
dependencies and can be cross-compiled for any target with a C11
compiler.  Platform-specific code lives under ``platform/``.
