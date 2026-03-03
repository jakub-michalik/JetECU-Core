#include "cpp/jetecu/Fuel.h"

namespace jetecu {

Fuel::Fuel()
{
    ecu_fuel_init(&state_);
}

FuelOutput Fuel::compute(float target_pct, float dt, const ecu_config_t &cfg)
{
    ecu_fuel_output_t out = ecu_fuel_compute(target_pct, dt, &cfg, &state_);
    return {out.command_pct, out.actual_pct, out.cutoff};
}

void Fuel::cutoff()
{
    ecu_fuel_cutoff(&state_);
}

void Fuel::release()
{
    ecu_fuel_release(&state_);
}

bool Fuel::isCutoff() const
{
    return state_.cutoff_active;
}

Fuel::CutoffGuard::CutoffGuard(Fuel &fuel) : fuel_(fuel)
{
    fuel_.cutoff();
}

Fuel::CutoffGuard::~CutoffGuard()
{
    fuel_.release();
}

} // namespace jetecu
