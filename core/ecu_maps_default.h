#ifndef ECU_MAPS_DEFAULT_H
#define ECU_MAPS_DEFAULT_H

#include "core/ecu_map.h"

/* Default fuel map: RPM -> base fuel flow % */
static const ecu_map1d_t default_fuel_map = {
    .x = { 0, 30000, 45000, 60000, 80000, 100000, 120000 },
    .y = { 0,   5,    10,    25,    50,     75,    100   },
    .count = 7,
};

/* Default EGT limit map: RPM -> max EGT */
static const ecu_map1d_t default_egt_limit_map = {
    .x = { 0,     30000,  60000,  80000,  120000 },
    .y = { 1100,  1100,   1000,   950,    900    },
    .count = 5,
};

#endif /* ECU_MAPS_DEFAULT_H */
