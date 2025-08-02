#ifndef TEL_SEND_H
#define TEL_SEND_H

#include "core/ecu_core.h"
#include "telemetry/tel_frame.h"
#include "telemetry/tel_protocol.h"

/* Transport callback: platform provides this */
typedef int (*tel_transport_fn)(const uint8_t *data, size_t len);

typedef struct {
    tel_transport_fn send;
    tel_decoder_t    decoder;
    float            throttle_override;
    int              has_throttle_override;
} tel_ctx_t;

/* Initialize telemetry context */
void tel_init(tel_ctx_t *ctx, tel_transport_fn send_fn);

/* Send status update */
int tel_send_status(tel_ctx_t *ctx, const ecu_t *ecu, const ecu_outputs_t *out,
                    const ecu_inputs_t *in);

/* Process received byte (from UART/socket) */
int tel_process_byte(tel_ctx_t *ctx, uint8_t byte);

/* Get throttle override value, if any */
int tel_get_throttle(const tel_ctx_t *ctx, float *throttle);

#endif /* TEL_SEND_H */
