/**
 * @file tel_frame.h
 * @brief Binary telemetry framing with CRC-16 validation.
 *
 * Frame format:
 * @code
 * [SYNC1=0xAA][SYNC2=0x55][LENGTH_HI][LENGTH_LO][PAYLOAD...][CRC_HI][CRC_LO]
 * @endcode
 */

#ifndef TEL_FRAME_H
#define TEL_FRAME_H

#include <stdint.h>
#include <stddef.h>

/**
 * @defgroup tel_frame Telemetry Framing
 * @brief CRC-protected binary frame encoder and streaming decoder.
 * @{
 */

#define TEL_SYNC1  0xAA  /**< First sync byte. */
#define TEL_SYNC2  0x55  /**< Second sync byte. */
#define TEL_MAX_PAYLOAD  256  /**< Maximum payload bytes. */
#define TEL_FRAME_OVERHEAD  6   /**< sync(2) + len(2) + crc(2). */
#define TEL_MAX_FRAME  (TEL_MAX_PAYLOAD + TEL_FRAME_OVERHEAD) /**< Max frame size. */

/**
 * @brief Encode a payload into a framed packet.
 *
 * @param[in]  payload        Payload data.
 * @param[in]  payload_len    Payload length in bytes.
 * @param[out] frame_buf      Output buffer for the complete frame.
 * @param[in]  frame_buf_len  Size of @p frame_buf.
 * @return Total frame length on success, or -1 on error.
 */
int tel_frame_encode(const uint8_t *payload, size_t payload_len,
                     uint8_t *frame_buf, size_t frame_buf_len);

/** @brief Decoder state machine states. */
typedef enum {
    TEL_DECODE_SYNC1,    /**< Waiting for first sync byte. */
    TEL_DECODE_SYNC2,    /**< Waiting for second sync byte. */
    TEL_DECODE_LEN_HI,   /**< Reading length high byte. */
    TEL_DECODE_LEN_LO,   /**< Reading length low byte. */
    TEL_DECODE_PAYLOAD,  /**< Accumulating payload bytes. */
    TEL_DECODE_CRC_HI,   /**< Reading CRC high byte. */
    TEL_DECODE_CRC_LO,   /**< Reading CRC low byte. */
} tel_decode_state_t;

/** @brief Streaming frame decoder context. */
typedef struct {
    tel_decode_state_t state;                 /**< Current decoder state. */
    uint8_t  payload[TEL_MAX_PAYLOAD];        /**< Payload accumulator. */
    uint16_t payload_len;                     /**< Expected payload length. */
    uint16_t payload_idx;                     /**< Bytes received so far. */
    uint16_t crc_received;                    /**< CRC from the frame. */
} tel_decoder_t;

/**
 * @brief Initialise the frame decoder.
 * @param[out] dec  Decoder context.
 */
void tel_decoder_init(tel_decoder_t *dec);

/**
 * @brief Feed one byte to the decoder.
 *
 * @param[in,out] dec   Decoder context.
 * @param[in]     byte  Received byte.
 * @return 1 when a complete valid frame is ready in @c dec->payload,
 *         0 otherwise.
 */
int tel_decoder_feed(tel_decoder_t *dec, uint8_t byte);

/**
 * @brief Compute CRC-16/CCITT over a data buffer.
 *
 * @param[in] data  Input data.
 * @param[in] len   Data length in bytes.
 * @return CRC-16 value.
 */
uint16_t tel_crc16(const uint8_t *data, size_t len);

/** @} */ /* end tel_frame */

#endif /* TEL_FRAME_H */
