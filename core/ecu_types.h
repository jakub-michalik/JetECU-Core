#ifndef ECU_TYPES_H
#define ECU_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* Error codes */
typedef enum {
    ECU_OK = 0,
    ECU_ERR_INVALID_PARAM,
    ECU_ERR_TIMEOUT,
    ECU_ERR_SENSOR,
    ECU_ERR_OVERTEMP,
    ECU_ERR_OVERSPEED,
    ECU_ERR_FUEL,
    ECU_ERR_CONFIG,
    ECU_ERR_STATE,
} ecu_err_t;

/* Common result type */
typedef struct {
    ecu_err_t err;
    float     value;
} ecu_result_t;

/* Timestamp in milliseconds */
typedef uint32_t ecu_time_t;

#endif /* ECU_TYPES_H */
