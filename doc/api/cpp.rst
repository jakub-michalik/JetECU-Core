C++ Wrappers
============

The ``cpp/jetecu/`` directory provides modern C++17 RAII wrappers around
the C core.  Each class holds (or wraps) the corresponding C struct and
exposes a ``raw()`` accessor for direct interop.

Quick Example
-------------

.. code-block:: cpp

   #include "cpp/jetecu/Engine.h"

   jetecu::Engine engine;
   jetecu::Inputs in{.rpm = 50000.f, .egt = 600.f, .throttle = 80.f};

   auto out = engine.step(in, 0.001f);
   printf("state=%s fuel=%.1f%%\n", engine.stateName(), out.fuel_pct);

Available Classes
-----------------

.. list-table::
   :header-rows: 1
   :widths: 22 18 60

   * - Class
     - Wraps
     - Key Methods
   * - ``Engine``
     - ``ecu_t``
     - ``step()``, ``state()``, ``stateName()``
   * - ``StateMachine``
     - ``ecu_sm_t``
     - ``transition()``, ``setCallbacks()``
   * - ``Pid``
     - ``ecu_pid_t``
     - ``update()``, ``reset()``
   * - ``Sensor``
     - ``ecu_sensor_*``
     - ``validate()``, ``egtPlausible()``
   * - ``FaultManager``
     - ``ecu_fault_mgr_t``
     - ``report()``, ``clear()``, ``isActive()``
   * - ``Map1D`` / ``Map2D``
     - ``ecu_map*_t``
     - ``lookup()`` with ``initializer_list`` construction
   * - ``Scheduler``
     - ``ecu_scheduler_t``
     - ``add()`` with ``std::function``, ``tick()``
   * - ``Config``
     - ``ecu_config_t``
     - ``loadFile()``, ``loadString()``
   * - ``Telemetry``
     - ``tel_ctx_t``
     - ``sendStatus()``, ``processByte()``

Engine
------

Header: ``cpp/jetecu/Engine.h``

.. code-block:: cpp

   class Engine {
   public:
       Engine();                              // default config
       explicit Engine(const ecu_config_t &cfg);
       Outputs step(const Inputs &in, float dt);
       State state() const;
       const char *stateName() const;
       ecu_t &raw();
   };

StateMachine
------------

Header: ``cpp/jetecu/StateMachine.h``

.. code-block:: cpp

   class StateMachine {
   public:
       using Callback = std::function<void(State)>;
       State state() const;
       const char *stateName() const;
       bool transition(State next, uint32_t now);
       bool timedOut(uint32_t now) const;
       void setTimeout(uint32_t ms);
       void setCallbacks(Callback on_enter, Callback on_exit);
   };

Sensor
------

Header: ``cpp/jetecu/Sensor.h``

.. code-block:: cpp

   class Sensor {
   public:
       Sensor(float range_min, float range_max,
              float rate_limit = 0.f,
              float stuck_tolerance = 0.5f,
              int stuck_count_limit = 5);
       SensorReading validate(float raw);
       static bool egtPlausible(float egt, float rpm, float rpm_idle);
   };

Pid
---

Header: ``cpp/jetecu/Pid.h``

.. code-block:: cpp

   class Pid {
   public:
       Pid(float kp, float ki, float kd,
           float out_min, float out_max,
           float integral_max = 0.f);
       float update(float setpoint, float measured, float dt);
       void reset();
   };

FaultManager
------------

Header: ``cpp/jetecu/FaultManager.h``

.. code-block:: cpp

   class FaultManager {
   public:
       void report(uint16_t code, ecu_fault_severity_t sev,
                   ecu_fault_action_t act, uint32_t now);
       void clear(uint16_t code);
       bool isActive(uint16_t code) const;
       int count() const;
       ecu_fault_action_t action() const;
   };

Map1D / Map2D
-------------

Header: ``cpp/jetecu/Map.h``

.. code-block:: cpp

   class Map1D {
   public:
       Map1D(std::initializer_list<std::pair<float, float>> points);
       float lookup(float x) const;
   };

   class Map2D {
   public:
       Map2D(std::initializer_list<float> row_keys,
             std::initializer_list<float> col_keys,
             std::initializer_list<std::initializer_list<float>> values);
       float lookup(float row, float col) const;
   };

Scheduler
---------

Header: ``cpp/jetecu/Scheduler.h``

.. code-block:: cpp

   class Scheduler {
   public:
       int add(const std::string &name, std::function<void()> fn,
               sched_priority_t prio, uint32_t interval_ms);
       void tick(uint32_t now_ms);
       bool anyOverrun() const;
       uint32_t wcet(int task_idx) const;
   };

Config
------

Header: ``cpp/jetecu/Config.h``

.. code-block:: cpp

   class Config {
   public:
       bool loadFile(const std::string &path);
       bool loadString(const std::string &json);
       const ecu_config_t &get() const;
       ecu_config_t &get();
   };

Telemetry
---------

Header: ``cpp/jetecu/Telemetry.h``

.. code-block:: cpp

   class Telemetry {
   public:
       using TransportFn = std::function<int(const uint8_t *, size_t)>;
       explicit Telemetry(TransportFn transport);
       int sendStatus(const ecu_t &ecu, const ecu_outputs_t &out,
                      const ecu_inputs_t &in);
       int processByte(uint8_t byte);
       bool getThrottle(float &throttle) const;
   };
