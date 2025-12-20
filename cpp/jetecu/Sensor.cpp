#include "cpp/jetecu/Sensor.h"

namespace jetecu {

Sensor::Sensor(float range_min, float range_max,
               float rate_limit, float stuck_tolerance,
               int stuck_count_limit)
{
    cfg_.range_min = range_min;
    cfg_.range_max = range_max;
    cfg_.rate_limit = rate_limit;
    cfg_.stuck_tolerance = stuck_tolerance;
    cfg_.stuck_count_limit = stuck_count_limit;
    ecu_sensor_init(&state_);
}

SensorReading Sensor::validate(float raw)
{
    ecu_sensor_reading_t r = ecu_sensor_validate(raw, &cfg_, &state_);
    return { r.value, r.raw, r.valid, r.faults };
}

bool Sensor::egtPlausible(float egt, float rpm, float rpm_idle)
{
    return ecu_sensor_egt_plausible(egt, rpm, rpm_idle);
}

} // namespace jetecu
