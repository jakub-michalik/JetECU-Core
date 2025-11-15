#ifndef HAL_CAN_H
#define HAL_CAN_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint32_t id;
    uint8_t  data[8];
    uint8_t  dlc;
    uint8_t  is_extended;
} hal_can_msg_t;

int hal_can_init(uint32_t bitrate);
int hal_can_send(const hal_can_msg_t *msg);
int hal_can_receive(hal_can_msg_t *msg);  /* non-blocking */
int hal_can_available(void);

#endif /* HAL_CAN_H */
