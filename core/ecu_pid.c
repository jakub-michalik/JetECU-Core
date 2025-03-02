#include "core/ecu_pid.h"

static float clampf(float val, float lo, float hi)
{
    if (val < lo) return lo;
    if (val > hi) return hi;
    return val;
}

void ecu_pid_init(ecu_pid_t *pid, const ecu_pid_config_t *cfg)
{
    pid->cfg = *cfg;
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->initialized = false;
}

void ecu_pid_reset(ecu_pid_t *pid)
{
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->initialized = false;
}

float ecu_pid_update(ecu_pid_t *pid, float setpoint, float measured, float dt)
{
    float error = setpoint - measured;

    if (!pid->initialized) {
        pid->prev_error = error;
        pid->initialized = true;
    }

    float p = pid->cfg.kp * error;

    pid->integral += error * dt;

    /* Anti-windup */
    float i_limit = pid->cfg.integral_max;
    if (i_limit <= 0.0f) {
        /* Auto-derive from output limits and ki */
        float ki = pid->cfg.ki;
        i_limit = (ki > 0.001f) ? (pid->cfg.output_max / ki) : 1000.0f;
    }
    pid->integral = clampf(pid->integral, -i_limit, i_limit);
    float i = pid->cfg.ki * pid->integral;

    float derivative = (dt > 0.0f) ? (error - pid->prev_error) / dt : 0.0f;
    float d = pid->cfg.kd * derivative;

    pid->prev_error = error;

    float output = p + i + d;
    return clampf(output, pid->cfg.output_min, pid->cfg.output_max);
}
