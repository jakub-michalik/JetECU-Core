#ifndef ECU_SENSOR_H
#define ECU_SENSOR_H

#include "core/ecu_types.h"

/* Sensor fault flags */
#define SENSOR_FAULT_NONE       0x00
#define SENSOR_FAULT_RANGE      0x01
#define SENSOR_FAULT_RATE       0x02
#define SENSOR_FAULT_STUCK      0x04
#define SENSOR_FAULT_PLAUSIBLE  0x08

/* Processed sensor reading */
typedef struct {
    float    value;
    float    raw;
    bool     valid;
    uint32_t faults;
} ecu_sensor_reading_t;

/* Sensor configuration */
typedef struct {
    float range_min;
    float range_max;
    float rate_limit;     /* max change per step */
    float stuck_tolerance;
    int   stuck_count_limit;
} ecu_sensor_config_t;

/* Sensor state (tracks history for stuck detection etc) */
typedef struct {
    float last_value;
    int   stuck_count;
    bool  initialized;
} ecu_sensor_state_t;

/* Initialize sensor state */
void ecu_sensor_init(ecu_sensor_state_t *state);

/* Validate a raw reading */
ecu_sensor_reading_t ecu_sensor_validate(
    float raw,
    const ecu_sensor_config_t *cfg,
    ecu_sensor_state_t *state
);

/* Cross-check: is EGT plausible given RPM? */
bool ecu_sensor_egt_plausible(float egt, float rpm, float rpm_idle);

#endif /* ECU_SENSOR_H */
