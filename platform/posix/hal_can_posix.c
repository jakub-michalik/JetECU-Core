#include "hal/hal_can.h"

/* POSIX stub — CAN not available, all ops return error */

int hal_can_init(uint32_t bitrate)
{
    (void)bitrate;
    return 0;  /* succeed silently */
}

int hal_can_send(const hal_can_msg_t *msg)
{
    (void)msg;
    return 0;
}

int hal_can_receive(hal_can_msg_t *msg)
{
    (void)msg;
    return 0;  /* nothing available */
}

int hal_can_available(void)
{
    return 0;
}
