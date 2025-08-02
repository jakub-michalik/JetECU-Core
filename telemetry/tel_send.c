#include "telemetry/tel_send.h"
#include <string.h>

void tel_init(tel_ctx_t *ctx, tel_transport_fn send_fn)
{
    ctx->send = send_fn;
    tel_decoder_init(&ctx->decoder);
    ctx->throttle_override = 0.0f;
    ctx->has_throttle_override = 0;
}

int tel_send_status(tel_ctx_t *ctx, const ecu_t *ecu, const ecu_outputs_t *out,
                    const ecu_inputs_t *in)
{
    tel_status_msg_t msg;
    msg.msg_type = TEL_MSG_STATUS;
    msg.state = (uint8_t)out->state;
    msg.rpm = (uint16_t)(in->rpm);
    msg.egt = (uint16_t)(in->egt * 10.0f);
    msg.fuel_pct = (uint8_t)out->fuel_pct;
    msg.throttle_pct = (uint8_t)in->throttle;
    msg.fault_code = ecu->fault_code;
    msg.uptime_ms = ecu->time_ms;

    uint8_t frame[TEL_MAX_FRAME];
    int flen = tel_frame_encode((const uint8_t *)&msg, sizeof(msg), frame, sizeof(frame));
    if (flen < 0) return -1;

    return ctx->send(frame, (size_t)flen);
}

int tel_process_byte(tel_ctx_t *ctx, uint8_t byte)
{
    if (tel_decoder_feed(&ctx->decoder, byte) != 1) {
        return 0;
    }

    /* Process received command */
    if (ctx->decoder.payload_len < 1) return 0;

    uint8_t msg_type = ctx->decoder.payload[0];
    switch (msg_type) {
    case TEL_CMD_SET_THROTTLE:
        if (ctx->decoder.payload_len >= sizeof(tel_throttle_cmd_t)) {
            tel_throttle_cmd_t *cmd = (tel_throttle_cmd_t *)ctx->decoder.payload;
            ctx->throttle_override = (float)cmd->throttle_pct;
            ctx->has_throttle_override = 1;
        }
        break;
    default:
        break;
    }

    return 1;
}

int tel_get_throttle(const tel_ctx_t *ctx, float *throttle)
{
    if (ctx->has_throttle_override) {
        *throttle = ctx->throttle_override;
        return 1;
    }
    return 0;
}
