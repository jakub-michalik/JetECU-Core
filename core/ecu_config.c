#include "core/ecu_config.h"

void ecu_config_defaults(ecu_config_t *cfg)
{
    cfg->rpm_idle = 45000.0f;
    cfg->rpm_max = 120000.0f;
    cfg->rpm_start_target = 60000.0f;

    cfg->egt_max = 1100.0f;
    cfg->egt_start_min = 150.0f;

    cfg->fuel_start_pct = 8.0f;
    cfg->fuel_max_pct = 100.0f;
    cfg->fuel_ramp_rate = 5.0f;

    cfg->prestart_timeout_ms = 5000;
    cfg->spinup_timeout_ms = 10000;
    cfg->ignition_timeout_ms = 8000;

    cfg->rpm_ramp_rate = 400.0f;

    cfg->pid_kp = 0.5f;
    cfg->pid_ki = 0.1f;
    cfg->pid_kd = 0.01f;
}
