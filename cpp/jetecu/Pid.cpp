#include "cpp/jetecu/Pid.h"

namespace jetecu {

Pid::Pid(float kp, float ki, float kd,
         float out_min, float out_max, float integral_max)
{
    ecu_pid_config_t cfg = {};
    cfg.kp = kp;
    cfg.ki = ki;
    cfg.kd = kd;
    cfg.output_min = out_min;
    cfg.output_max = out_max;
    cfg.integral_max = integral_max;
    ecu_pid_init(&pid_, &cfg);
}

float Pid::update(float setpoint, float measured, float dt)
{
    return ecu_pid_update(&pid_, setpoint, measured, dt);
}

void Pid::reset()
{
    ecu_pid_reset(&pid_);
}

} // namespace jetecu
