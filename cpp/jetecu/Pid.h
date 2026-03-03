#pragma once

#ifndef JETECU_PID_H
#define JETECU_PID_H

#ifdef __cplusplus
extern "C" {
#endif
#include "core/ecu_pid.h"
#ifdef __cplusplus
}
#endif

namespace jetecu {

class Pid {
public:
    Pid(float kp, float ki, float kd,
        float out_min, float out_max,
        float integral_max = 0.0f);

    float update(float setpoint, float measured, float dt);
    void reset();

    ecu_pid_t &raw() { return pid_; }
    const ecu_pid_t &raw() const { return pid_; }

private:
    ecu_pid_t pid_;
};

} // namespace jetecu

#endif // JETECU_PID_H
