#include "telemetry/tel_frame.h"

uint16_t tel_crc16(const uint8_t *data, size_t len)
{
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }
    return crc;
}

int tel_frame_encode(const uint8_t *payload, size_t payload_len,
                     uint8_t *frame_buf, size_t frame_buf_len)
{
    if (payload_len > TEL_MAX_PAYLOAD) return -1;
    if (frame_buf_len < payload_len + TEL_FRAME_OVERHEAD) return -1;

    size_t idx = 0;
    frame_buf[idx++] = TEL_SYNC1;
    frame_buf[idx++] = TEL_SYNC2;
    frame_buf[idx++] = (uint8_t)(payload_len >> 8);
    frame_buf[idx++] = (uint8_t)(payload_len & 0xFF);

    for (size_t i = 0; i < payload_len; i++) {
        frame_buf[idx++] = payload[i];
    }

    uint16_t crc = tel_crc16(payload, payload_len);
    frame_buf[idx++] = (uint8_t)(crc >> 8);
    frame_buf[idx++] = (uint8_t)(crc & 0xFF);

    return (int)idx;
}

void tel_decoder_init(tel_decoder_t *dec)
{
    dec->state = TEL_DECODE_SYNC1;
    dec->payload_len = 0;
    dec->payload_idx = 0;
    dec->crc_received = 0;
}

int tel_decoder_feed(tel_decoder_t *dec, uint8_t byte)
{
    switch (dec->state) {
    case TEL_DECODE_SYNC1:
        if (byte == TEL_SYNC1) dec->state = TEL_DECODE_SYNC2;
        break;
    case TEL_DECODE_SYNC2:
        if (byte == TEL_SYNC2)
            dec->state = TEL_DECODE_LEN_HI;
        else
            dec->state = TEL_DECODE_SYNC1;
        break;
    case TEL_DECODE_LEN_HI:
        dec->payload_len = (uint16_t)(byte << 8);
        dec->state = TEL_DECODE_LEN_LO;
        break;
    case TEL_DECODE_LEN_LO:
        dec->payload_len |= byte;
        if (dec->payload_len > TEL_MAX_PAYLOAD) {
            dec->state = TEL_DECODE_SYNC1;
        } else if (dec->payload_len == 0) {
            dec->state = TEL_DECODE_CRC_HI;
        } else {
            dec->payload_idx = 0;
            dec->state = TEL_DECODE_PAYLOAD;
        }
        break;
    case TEL_DECODE_PAYLOAD:
        dec->payload[dec->payload_idx++] = byte;
        if (dec->payload_idx >= dec->payload_len) {
            dec->state = TEL_DECODE_CRC_HI;
        }
        break;
    case TEL_DECODE_CRC_HI:
        dec->crc_received = (uint16_t)(byte << 8);
        dec->state = TEL_DECODE_CRC_LO;
        break;
    case TEL_DECODE_CRC_LO:
        dec->crc_received |= byte;
        dec->state = TEL_DECODE_SYNC1;
        /* Verify CRC */
        if (tel_crc16(dec->payload, dec->payload_len) == dec->crc_received) {
            return 1;  /* valid frame */
        }
        break;
    }
    return 0;
}
