#include <gtest/gtest.h>

extern "C" {
#include "telemetry/tel_frame.h"
}

TEST(Telemetry, CRC16) {
    uint8_t data[] = { 0x01, 0x02, 0x03 };
    uint16_t crc = tel_crc16(data, 3);
    EXPECT_NE(crc, 0u);

    uint16_t crc2 = tel_crc16(data, 3);
    EXPECT_EQ(crc, crc2);
}

TEST(Telemetry, EncodeDecode) {
    uint8_t payload[] = { 0x10, 0x20, 0x30, 0x40, 0x50 };
    uint8_t frame[TEL_MAX_FRAME];

    int flen = tel_frame_encode(payload, 5, frame, sizeof(frame));
    ASSERT_GT(flen, 0);
    EXPECT_EQ(flen, 5 + TEL_FRAME_OVERHEAD);

    tel_decoder_t dec;
    tel_decoder_init(&dec);

    int result = 0;
    for (int i = 0; i < flen; i++) {
        result = tel_decoder_feed(&dec, frame[i]);
    }
    EXPECT_EQ(result, 1);
    EXPECT_EQ(dec.payload_len, 5u);
    EXPECT_EQ(dec.payload[0], 0x10);
    EXPECT_EQ(dec.payload[4], 0x50);
}

TEST(Telemetry, DecodeBadCRC) {
    uint8_t payload[] = { 0x01, 0x02 };
    uint8_t frame[TEL_MAX_FRAME];

    int flen = tel_frame_encode(payload, 2, frame, sizeof(frame));
    ASSERT_GT(flen, 0);

    frame[flen - 1] ^= 0xFF;

    tel_decoder_t dec;
    tel_decoder_init(&dec);

    int result = 0;
    for (int i = 0; i < flen; i++) {
        result = tel_decoder_feed(&dec, frame[i]);
    }
    EXPECT_EQ(result, 0);
}

TEST(Telemetry, DecodeGarbagePrefix) {
    uint8_t payload[] = { 0xAB, 0xCD };
    uint8_t frame[TEL_MAX_FRAME];

    int flen = tel_frame_encode(payload, 2, frame, sizeof(frame));

    tel_decoder_t dec;
    tel_decoder_init(&dec);

    tel_decoder_feed(&dec, 0x00);
    tel_decoder_feed(&dec, 0xFF);
    tel_decoder_feed(&dec, 0x12);

    int result = 0;
    for (int i = 0; i < flen; i++) {
        result = tel_decoder_feed(&dec, frame[i]);
    }
    EXPECT_EQ(result, 1);
}

TEST(Telemetry, EncodeTooLong) {
    uint8_t payload[TEL_MAX_PAYLOAD + 1] = {};
    uint8_t frame[TEL_MAX_FRAME + 10] = {};

    int flen = tel_frame_encode(payload, TEL_MAX_PAYLOAD + 1, frame, sizeof(frame));
    EXPECT_EQ(flen, -1);
}

/* ---- C++ wrapper tests ---- */

#include "cpp/jetecu/Telemetry.h"

TEST(TelemetryWrapper, CRC16) {
    uint8_t data[] = { 0x01, 0x02, 0x03 };
    uint16_t crc = jetecu::Telemetry::crc16(data, 3);
    EXPECT_NE(crc, 0u);
    EXPECT_EQ(crc, jetecu::Telemetry::crc16(data, 3));
}

TEST(TelemetryWrapper, EncodeRoundtrip) {
    uint8_t payload[] = { 0xAA, 0xBB, 0xCC };
    auto frame = jetecu::Telemetry::encode(payload, 3);
    EXPECT_EQ(frame.size(), 3u + TEL_FRAME_OVERHEAD);
}
