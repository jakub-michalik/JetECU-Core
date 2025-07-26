#include "test/test_runner.h"
#include "telemetry/tel_frame.h"

TEST(test_crc16)
{
    uint8_t data[] = { 0x01, 0x02, 0x03 };
    uint16_t crc = tel_crc16(data, 3);
    ASSERT(crc != 0);

    /* Same data should produce same CRC */
    uint16_t crc2 = tel_crc16(data, 3);
    ASSERT_EQ(crc, crc2);
}

TEST(test_encode_decode)
{
    uint8_t payload[] = { 0x10, 0x20, 0x30, 0x40, 0x50 };
    uint8_t frame[TEL_MAX_FRAME];

    int flen = tel_frame_encode(payload, 5, frame, sizeof(frame));
    ASSERT(flen > 0);
    ASSERT_EQ(flen, 5 + TEL_FRAME_OVERHEAD);

    /* Decode */
    tel_decoder_t dec;
    tel_decoder_init(&dec);

    int result = 0;
    for (int i = 0; i < flen; i++) {
        result = tel_decoder_feed(&dec, frame[i]);
    }
    ASSERT_EQ(result, 1);  /* valid frame */
    ASSERT_EQ(dec.payload_len, 5);
    ASSERT_EQ(dec.payload[0], 0x10);
    ASSERT_EQ(dec.payload[4], 0x50);
}

TEST(test_decode_bad_crc)
{
    uint8_t payload[] = { 0x01, 0x02 };
    uint8_t frame[TEL_MAX_FRAME];

    int flen = tel_frame_encode(payload, 2, frame, sizeof(frame));
    ASSERT(flen > 0);

    /* Corrupt CRC */
    frame[flen - 1] ^= 0xFF;

    tel_decoder_t dec;
    tel_decoder_init(&dec);

    int result = 0;
    for (int i = 0; i < flen; i++) {
        result = tel_decoder_feed(&dec, frame[i]);
    }
    ASSERT_EQ(result, 0);  /* should fail */
}

TEST(test_decode_garbage_prefix)
{
    uint8_t payload[] = { 0xAB, 0xCD };
    uint8_t frame[TEL_MAX_FRAME];

    int flen = tel_frame_encode(payload, 2, frame, sizeof(frame));

    tel_decoder_t dec;
    tel_decoder_init(&dec);

    /* Feed garbage first */
    tel_decoder_feed(&dec, 0x00);
    tel_decoder_feed(&dec, 0xFF);
    tel_decoder_feed(&dec, 0x12);

    /* Then the real frame */
    int result = 0;
    for (int i = 0; i < flen; i++) {
        result = tel_decoder_feed(&dec, frame[i]);
    }
    ASSERT_EQ(result, 1);
}

TEST(test_encode_too_long)
{
    uint8_t payload[TEL_MAX_PAYLOAD + 1];
    uint8_t frame[TEL_MAX_FRAME + 10];

    int flen = tel_frame_encode(payload, TEL_MAX_PAYLOAD + 1, frame, sizeof(frame));
    ASSERT_EQ(flen, -1);
}

int main(void)
{
    printf("=== Telemetry Tests ===\n");
    RUN_TEST(test_crc16);
    RUN_TEST(test_encode_decode);
    RUN_TEST(test_decode_bad_crc);
    RUN_TEST(test_decode_garbage_prefix);
    RUN_TEST(test_encode_too_long);
    TEST_SUMMARY();
}
