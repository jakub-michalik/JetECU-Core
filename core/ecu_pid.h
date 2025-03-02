#ifndef ECU_PID_H
#define ECU_PID_H

#include <stdbool.h>

typedef struct {
    float kp;
    float ki;
    float kd;
    float output_min;
    float output_max;
    float integral_max;   /* anti-windup limit (0 = auto from output_max) */
} ecu_pid_config_t;

typedef struct {
    ecu_pid_config_t cfg;
    float integral;
    float prev_error;
    bool  initialized;
} ecu_pid_t;

void ecu_pid_init(ecu_pid_t *pid, const ecu_pid_config_t *cfg);
void ecu_pid_reset(ecu_pid_t *pid);
float ecu_pid_update(ecu_pid_t *pid, float setpoint, float measured, float dt);

#endif /* ECU_PID_H */
