#ifndef TEL_FRAME_H
#define TEL_FRAME_H

#include <stdint.h>
#include <stddef.h>

/*
 * Binary telemetry frame format:
 *   [SYNC1][SYNC2][LENGTH_HI][LENGTH_LO][PAYLOAD...][CRC_HI][CRC_LO]
 *
 * SYNC: 0xAA 0x55
 * LENGTH: payload length (big-endian, max 256)
 * CRC: CRC-16/CCITT of payload
 */

#define TEL_SYNC1  0xAA
#define TEL_SYNC2  0x55
#define TEL_MAX_PAYLOAD  256
#define TEL_FRAME_OVERHEAD  6   /* sync(2) + len(2) + crc(2) */
#define TEL_MAX_FRAME  (TEL_MAX_PAYLOAD + TEL_FRAME_OVERHEAD)

/* Encode a frame. Returns total frame length, or -1 on error. */
int tel_frame_encode(const uint8_t *payload, size_t payload_len,
                     uint8_t *frame_buf, size_t frame_buf_len);

/* Decoder state machine */
typedef enum {
    TEL_DECODE_SYNC1,
    TEL_DECODE_SYNC2,
    TEL_DECODE_LEN_HI,
    TEL_DECODE_LEN_LO,
    TEL_DECODE_PAYLOAD,
    TEL_DECODE_CRC_HI,
    TEL_DECODE_CRC_LO,
} tel_decode_state_t;

typedef struct {
    tel_decode_state_t state;
    uint8_t  payload[TEL_MAX_PAYLOAD];
    uint16_t payload_len;
    uint16_t payload_idx;
    uint16_t crc_received;
} tel_decoder_t;

/* Initialize decoder */
void tel_decoder_init(tel_decoder_t *dec);

/* Feed one byte. Returns 1 when a complete valid frame is ready. */
int tel_decoder_feed(tel_decoder_t *dec, uint8_t byte);

/* CRC-16/CCITT */
uint16_t tel_crc16(const uint8_t *data, size_t len);

#endif /* TEL_FRAME_H */
