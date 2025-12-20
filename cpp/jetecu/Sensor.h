#ifndef JETECU_SENSOR_H
#define JETECU_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif
#include "core/ecu_sensor.h"
#ifdef __cplusplus
}
#endif

#include <cstdint>

namespace jetecu {

struct SensorReading {
    float    value;
    float    raw;
    bool     valid;
    uint32_t faults;
};

class Sensor {
public:
    Sensor(float range_min, float range_max,
           float rate_limit = 0.0f,
           float stuck_tolerance = 0.5f,
           int stuck_count_limit = 5);

    SensorReading validate(float raw);
    static bool egtPlausible(float egt, float rpm, float rpm_idle);

    const ecu_sensor_config_t &config() const { return cfg_; }

private:
    ecu_sensor_config_t cfg_;
    ecu_sensor_state_t state_;
};

} // namespace jetecu

#endif // JETECU_SENSOR_H
