Telemetry
=========

Binary framing and high-level send / receive API.

Frame Layout
------------

Each frame is wrapped with two sync bytes, a 16-bit length, the payload
and a CRC-16/CCITT trailer.

.. mermaid::

   flowchart LR
       S1["SYNC1<br/>0xAA"] --> S2["SYNC2<br/>0x55"]
       S2 --> LH["LEN_HI"]
       LH --> LL["LEN_LO"]
       LL --> PL["PAYLOAD<br/>(0..256 bytes)"]
       PL --> CH["CRC_HI"]
       CH --> CL["CRC_LO"]

       classDef hdr fill:#e3f2fd,stroke:#1976d2,color:#0d47a1;
       classDef pay fill:#fff3e0,stroke:#ef6c00,color:#e65100;
       classDef crc fill:#e8f5e9,stroke:#2e7d32,color:#1b5e20;
       class S1,S2,LH,LL hdr;
       class PL pay;
       class CH,CL crc;

Host <-> ECU Exchange
---------------------

The host sends commands (e.g. ``TEL_CMD_SET_THROTTLE``) and receives
periodic status frames from the ECU.

.. mermaid::

   sequenceDiagram
       autonumber
       participant Host
       participant Tel as tel_send / tel_frame
       participant ECU as ecu_core

       loop every 20 ms (50 Hz)
           ECU->>Tel: status snapshot
           Tel-->>Host: STATUS frame (CRC ok)
       end

       Host->>Tel: CMD_SET_THROTTLE(45%)
       Tel->>ECU: dispatch command
       ECU-->>Tel: ACK / NACK
       Tel-->>Host: ACK frame

       Note over Tel,Host: CRC mismatch -> frame dropped<br/>(decoder resyncs on SYNC1/SYNC2)

Framing
-------

Header: ``telemetry/tel_frame.h``

.. doxygengroup:: tel_frame
   :content-only:
   :members:

Send / Receive
--------------

Header: ``telemetry/tel_send.h``

.. doxygengroup:: tel_send
   :content-only:
   :members:
