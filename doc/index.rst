JetECU-Core Documentation
=========================

Platform-independent Engine Control Unit for small jet (turbine) engines.

JetECU-Core provides a complete, testable, and portable ECU core written in
C11 with C++17 wrappers. It targets embedded platforms (STM32, ESP32) and
includes a full PC simulator for development and testing.

At a Glance
-----------

**What it controls.** JetECU-Core sits between an operator (throttle
input, host PC) and the jet engine itself, closing the loop on fuel,
ignition, and starter outputs based on RPM / EGT sensor feedback.

.. mermaid::

   flowchart LR
       Op([Operator /<br/>Host PC]) -->|throttle, commands| ECU{{JetECU-Core}}
       Sensors[/"RPM, EGT,<br/>throttle, battery"/] --> ECU
       ECU -->|fuel valve PWM| Engine([Jet engine])
       ECU -->|igniter, starter| Engine
       Engine --> Sensors
       ECU -->|telemetry frames| Op

       classDef ecu fill:#1976d2,stroke:#0d47a1,color:#fff,font-weight:bold;
       classDef ext fill:#eceff1,stroke:#546e7a;
       class ECU ecu;
       class Op,Sensors,Engine ext;

**How it is built.** Four clean layers, with the core logic kept free
of any hardware calls so it can be unit-tested on a PC.

.. mermaid::

   flowchart TB
       A["Application (main loop)"]
       C["Core ECU logic<br/><i>state machine - PID - fuel - faults - maps</i>"]
       H["HAL<br/><i>ADC - PWM - GPIO - UART - timers</i>"]
       P["Platform port<br/><i>POSIX sim - STM32 - ESP32</i>"]
       A --> C --> H --> P

       classDef app  fill:#e8f5e9,stroke:#2e7d32;
       classDef core fill:#e3f2fd,stroke:#1976d2;
       classDef hal  fill:#fff3e0,stroke:#ef6c00;
       classDef plat fill:#f3e5f5,stroke:#6a1b9a;
       class A app;
       class C core;
       class H hal;
       class P plat;

**What it does, in order.** The engine lifecycle, simplified to the
happy path — any phase can divert to ``FAULT`` on a safety violation
(see :doc:`architecture/state_machine` for the full diagram).

.. mermaid::

   flowchart LR
       OFF([OFF]) -->|throttle applied| START[START<br/>spin-up - ignition - ramp]
       START -->|RPM at target| RUN[RUN<br/>PID-controlled fuel]
       RUN -->|throttle released| STOP[COOLDOWN - SHUTDOWN]
       STOP --> OFF

       classDef idle fill:#eceff1,stroke:#546e7a;
       classDef act  fill:#e3f2fd,stroke:#1976d2;
       classDef run  fill:#e8f5e9,stroke:#2e7d32;
       class OFF,STOP idle;
       class START act;
       class RUN run;

.. toctree::
   :maxdepth: 2
   :caption: Getting Started

   getting_started/index

.. toctree::
   :maxdepth: 2
   :caption: Design

   architecture/index

.. toctree::
   :maxdepth: 2
   :caption: API Reference

   api/index

.. toctree::
   :maxdepth: 2
   :caption: Guides

   configuration/index
   fault_handling/index

.. toctree::
   :maxdepth: 1
   :caption: Development

   contributing/index
