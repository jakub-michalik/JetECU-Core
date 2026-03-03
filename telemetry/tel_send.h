/**
 * @file tel_send.h
 * @brief High-level telemetry send/receive API.
 */

#ifndef TEL_SEND_H
#define TEL_SEND_H

#include "core/ecu_core.h"
#include "telemetry/tel_frame.h"
#include "telemetry/tel_protocol.h"

/**
 * @defgroup tel_send Telemetry Send
 * @brief Status transmission, command reception, and throttle override.
 * @{
 */

/**
 * @brief Platform transport callback.
 *
 * @param[in] data  Bytes to transmit.
 * @param[in] len   Number of bytes.
 * @return Number of bytes actually sent.
 */
typedef int (*tel_transport_fn)(const uint8_t *data, size_t len);

/** @brief Telemetry context (one per link). */
typedef struct {
    tel_transport_fn send;              /**< Platform transport function. */
    tel_decoder_t    decoder;           /**< Incoming frame decoder. */
    float            throttle_override; /**< Throttle value from host. */
    int              has_throttle_override; /**< Non-zero if override is set. */
} tel_ctx_t;

/**
 * @brief Initialise telemetry context.
 *
 * @param[out] ctx      Telemetry context.
 * @param[in]  send_fn  Platform transport callback.
 */
void tel_init(tel_ctx_t *ctx, tel_transport_fn send_fn);

/**
 * @brief Send a status update frame.
 *
 * @param[in,out] ctx  Telemetry context.
 * @param[in]     ecu  Current ECU state.
 * @param[in]     out  Latest ECU outputs.
 * @param[in]     in   Latest ECU inputs.
 * @return Number of bytes sent, or -1 on error.
 */
int tel_send_status(tel_ctx_t *ctx, const ecu_t *ecu, const ecu_outputs_t *out,
                    const ecu_inputs_t *in);

/**
 * @brief Process a received byte (from UART / socket).
 *
 * @param[in,out] ctx   Telemetry context.
 * @param[in]     byte  Received byte.
 * @return 1 if a complete command was decoded, 0 otherwise.
 */
int tel_process_byte(tel_ctx_t *ctx, uint8_t byte);

/**
 * @brief Retrieve the throttle override value, if any.
 *
 * @param[in]  ctx       Telemetry context.
 * @param[out] throttle  Receives the override value (0–100).
 * @return 1 if an override is present, 0 otherwise.
 */
int tel_get_throttle(const tel_ctx_t *ctx, float *throttle);

/** @} */ /* end tel_send */

#endif /* TEL_SEND_H */
