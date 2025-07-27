#ifndef TEL_PROTOCOL_H
#define TEL_PROTOCOL_H

#include <stdint.h>

/* Message type (first byte of payload) */
typedef enum {
    /* ECU -> Host */
    TEL_MSG_STATUS       = 0x01,   /* periodic sensor dump */
    TEL_MSG_FAULT        = 0x02,   /* fault notification */
    TEL_MSG_ACK          = 0x10,   /* command acknowledgement */
    TEL_MSG_NACK         = 0x11,   /* command rejection */
    TEL_MSG_CONFIG_RESP  = 0x12,   /* config read response */

    /* Host -> ECU */
    TEL_CMD_SET_THROTTLE = 0x80,   /* set throttle % */
    TEL_CMD_READ_CONFIG  = 0x81,   /* read config value */
    TEL_CMD_READ_FAULTS  = 0x82,   /* read fault log */
    TEL_CMD_START        = 0x83,   /* start engine */
    TEL_CMD_STOP         = 0x84,   /* stop engine */
} tel_msg_type_t;

/* Status message payload */
typedef struct __attribute__((packed)) {
    uint8_t  msg_type;     /* TEL_MSG_STATUS */
    uint8_t  state;        /* ecu_state_t */
    uint16_t rpm;          /* RPM (scaled) */
    uint16_t egt;          /* EGT in 0.1 degC */
    uint8_t  fuel_pct;     /* fuel 0-100 */
    uint8_t  throttle_pct; /* throttle 0-100 */
    uint32_t fault_code;   /* fault bitmask */
    uint32_t uptime_ms;    /* milliseconds since boot */
} tel_status_msg_t;

/* Set throttle command */
typedef struct __attribute__((packed)) {
    uint8_t msg_type;     /* TEL_CMD_SET_THROTTLE */
    uint8_t throttle_pct; /* 0-100 */
} tel_throttle_cmd_t;

#endif /* TEL_PROTOCOL_H */
