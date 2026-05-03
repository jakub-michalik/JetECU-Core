DSP
===

Digital signal-processing primitives that run *before* sensor validation
(:doc:`sensor`).  All routines are allocation-free and use only
single-precision float arithmetic so they fit comfortably on a Cortex-M4F.

Header: ``core/ecu_dsp.h``

Building Blocks
---------------

- **Biquad IIR** in Direct Form II Transposed — one structure, two
  designer functions, covers >90 % of typical control-loop needs.
- **Butterworth low-pass designer** — bilinear transform with cutoff
  pre-warping, so the digital ``-3 dB`` frequency matches the spec
  exactly.
- **Notch designer** — RBJ cookbook form for cancelling a known
  mechanical harmonic.
- **3-sample running median** — O(1) memory, rejects single-sample
  spikes while preserving step edges far better than a moving average.

Reference
---------

.. doxygengroup:: ecu_dsp
   :content-only:
   :members:
