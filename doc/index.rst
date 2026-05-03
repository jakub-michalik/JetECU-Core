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
       classDef ext fill:#eceff1,stroke:#546e7a,color:#212121;
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

       classDef app  fill:#e8f5e9,stroke:#2e7d32,color:#1b5e20;
       classDef core fill:#e3f2fd,stroke:#1976d2,color:#0d47a1;
       classDef hal  fill:#fff3e0,stroke:#ef6c00,color:#e65100;
       classDef plat fill:#f3e5f5,stroke:#6a1b9a,color:#4a148c;
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

       classDef idle fill:#eceff1,stroke:#546e7a,color:#263238;
       classDef act  fill:#e3f2fd,stroke:#1976d2,color:#0d47a1;
       classDef run  fill:#e8f5e9,stroke:#2e7d32,color:#1b5e20;
       class OFF,STOP idle;
       class START act;
       class RUN run;

Signal Conditioning (DSP)
-------------------------

Raw ADC samples from the RPM tachometer and EGT thermocouple are noisy
and prone to single-sample spikes from EMC events.  Feeding them
straight into the PID would cause derivative-kick and false rate-limit
faults, so each sensor channel is run through a small DSP chain before
:doc:`validation <api/sensor>`.

**The chain — three stages, ~12 floating-point ops per sample at 1 kHz:**

1. **3-sample median filter** (:c:func:`ecu_median3_step`) — kills
   isolated impulses (ADC glitches, single-sample EMI bursts) without
   smearing real step responses.  O(1) memory, two comparisons per
   sample.
2. **2nd-order Butterworth low-pass IIR** (:c:func:`ecu_biquad_step`,
   designed by :c:func:`ecu_dsp_design_lowpass`) — removes wideband
   sensor noise.  Cutoff is set per channel (e.g. 50 Hz for RPM,
   5 Hz for EGT).  Coefficients are computed once at boot via the
   bilinear transform with frequency pre-warping so the digital
   :math:`-3 \text{dB}` point matches the design value to within
   < 0.1 %.
3. **Optional notch** (:c:func:`ecu_dsp_design_notch`) — cancels a
   known mechanical harmonic (shaft 1× or 2× resonance) when the
   tachometer pickup is contaminated by structural vibration.

The biquad runs in **Direct Form II Transposed**, which keeps the
intermediate state within the same order of magnitude as the input —
important when implementing the filter on a Cortex-M4F using
single-precision float.  The full per-sample cost is *5 multiplies +
4 adds*, well under 1 µs on an STM32F4 at 168 MHz.

.. mermaid::

   flowchart LR
       ADC[/"ADC sample<br/>(RPM / EGT / throttle)"/]
       MED["Median-of-3<br/><i>spike rejection</i>"]
       LPF["Butterworth LP IIR<br/><i>fc = 50 Hz / 5 Hz</i>"]
       NOTCH["Optional notch<br/><i>shaft harmonic</i>"]
       VAL["ecu_sensor_validate<br/><i>range / rate / stuck</i>"]
       CTRL[/"to PID + state machine"/]

       ADC --> MED --> LPF --> NOTCH --> VAL --> CTRL

       classDef io  fill:#eceff1,stroke:#546e7a,color:#263238;
       classDef dsp fill:#e3f2fd,stroke:#1976d2,color:#0d47a1;
       classDef val fill:#fff3e0,stroke:#ef6c00,color:#e65100;
       class ADC,CTRL io;
       class MED,LPF,NOTCH dsp;
       class VAL val;

The design is verified by the ``test_dsp`` suite: DC gain unity to
``1e-4``, > 20 dB rejection at 4× cutoff for the Butterworth section,
> 26 dB notch depth, and end-to-end spike rejection on the median
stage.  See :doc:`api/dsp` for the full reference.

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
