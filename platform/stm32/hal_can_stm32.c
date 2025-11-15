#ifdef STM32F4
#include "hal/hal_can.h"
#include "stm32f4xx_hal.h"

static CAN_HandleTypeDef hcan;

int hal_can_init(uint32_t bitrate)
{
    __HAL_RCC_CAN1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* CAN pins: PA11 (RX), PA12 (TX) */
    GPIO_InitTypeDef gpio = {
        .Pin = GPIO_PIN_11 | GPIO_PIN_12,
        .Mode = GPIO_MODE_AF_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_HIGH,
        .Alternate = GPIO_AF9_CAN1,
    };
    HAL_GPIO_Init(GPIOA, &gpio);

    hcan.Instance = CAN1;
    hcan.Init.Mode = CAN_MODE_NORMAL;
    hcan.Init.AutoBusOff = ENABLE;
    hcan.Init.AutoRetransmission = ENABLE;
    hcan.Init.TransmitFifoPriority = ENABLE;

    /* Timing for 42MHz APB1: 500kbps */
    if (bitrate == 500000) {
        hcan.Init.Prescaler = 6;
        hcan.Init.TimeSeg1 = CAN_BS1_11TQ;
        hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
    } else {
        /* Default 250kbps */
        hcan.Init.Prescaler = 12;
        hcan.Init.TimeSeg1 = CAN_BS1_11TQ;
        hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
    }

    if (HAL_CAN_Init(&hcan) != HAL_OK) return -1;

    /* Accept all messages */
    CAN_FilterTypeDef filter = {
        .FilterBank = 0,
        .FilterMode = CAN_FILTERMODE_IDMASK,
        .FilterScale = CAN_FILTERSCALE_32BIT,
        .FilterIdHigh = 0,
        .FilterIdLow = 0,
        .FilterMaskIdHigh = 0,
        .FilterMaskIdLow = 0,
        .FilterFIFOAssignment = CAN_RX_FIFO0,
        .FilterActivation = ENABLE,
    };
    HAL_CAN_ConfigFilter(&hcan, &filter);

    return (HAL_CAN_Start(&hcan) == HAL_OK) ? 0 : -1;
}

int hal_can_send(const hal_can_msg_t *msg)
{
    CAN_TxHeaderTypeDef header;
    if (msg->is_extended) {
        header.ExtId = msg->id;
        header.IDE = CAN_ID_EXT;
    } else {
        header.StdId = msg->id;
        header.IDE = CAN_ID_STD;
    }
    header.RTR = CAN_RTR_DATA;
    header.DLC = msg->dlc;

    uint32_t mailbox;
    HAL_StatusTypeDef s = HAL_CAN_AddTxMessage(&hcan, &header, (uint8_t *)msg->data, &mailbox);
    return (s == HAL_OK) ? 0 : -1;
}

int hal_can_receive(hal_can_msg_t *msg)
{
    if (HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_RX_FIFO0) == 0) return 0;

    CAN_RxHeaderTypeDef header;
    HAL_StatusTypeDef s = HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &header, msg->data);
    if (s != HAL_OK) return -1;

    msg->id = (header.IDE == CAN_ID_EXT) ? header.ExtId : header.StdId;
    msg->dlc = (uint8_t)header.DLC;
    msg->is_extended = (header.IDE == CAN_ID_EXT) ? 1 : 0;

    return 1;
}

int hal_can_available(void)
{
    return (int)HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_RX_FIFO0);
}
#endif /* STM32F4 */
