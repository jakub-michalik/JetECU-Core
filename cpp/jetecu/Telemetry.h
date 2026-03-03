#pragma once

#ifndef JETECU_TELEMETRY_H
#define JETECU_TELEMETRY_H

#ifdef __cplusplus
extern "C" {
#endif
#include "core/ecu_core.h"
#include "telemetry/tel_frame.h"
#include "telemetry/tel_send.h"
#ifdef __cplusplus
}
#endif

#include <cstdint>
#include <vector>
#include <functional>

namespace jetecu {

class Telemetry {
public:
    using TransportFn = std::function<int(const uint8_t *, size_t)>;

    explicit Telemetry(TransportFn transport);

    int sendStatus(const ecu_t &ecu, const ecu_outputs_t &out,
                   const ecu_inputs_t &in);
    int processByte(uint8_t byte);
    bool getThrottle(float &throttle) const;

    static std::vector<uint8_t> encode(const uint8_t *payload, size_t len);
    static uint16_t crc16(const uint8_t *data, size_t len);

    tel_ctx_t &raw() { return ctx_; }

private:
    tel_ctx_t ctx_;
    TransportFn transport_;

    static Telemetry *active_instance_;
    static int transportTrampoline(const uint8_t *data, size_t len);
};

} // namespace jetecu

#endif // JETECU_TELEMETRY_H
