#include "hal/hal_system.h"
#include <stdlib.h>

int hal_system_init(void) { return 0; }
void hal_system_deinit(void) {}
void hal_system_reset(void) { exit(0); }
uint32_t hal_system_get_id(void) { return 0x12345678; }
uint32_t hal_system_get_clock_hz(void) { return 1000000; }
