#include "cpp/jetecu/Telemetry.h"

namespace jetecu {

Telemetry *Telemetry::active_instance_ = nullptr;

Telemetry::Telemetry(TransportFn transport)
    : transport_(std::move(transport))
{
    active_instance_ = this;
    tel_init(&ctx_, transportTrampoline);
}

int Telemetry::sendStatus(const ecu_t &ecu, const ecu_outputs_t &out,
                          const ecu_inputs_t &in)
{
    return tel_send_status(&ctx_, &ecu, &out, &in);
}

int Telemetry::processByte(uint8_t byte)
{
    return tel_process_byte(&ctx_, byte);
}

bool Telemetry::getThrottle(float &throttle) const
{
    return tel_get_throttle(&ctx_, &throttle) == 1;
}

std::vector<uint8_t> Telemetry::encode(const uint8_t *payload, size_t len)
{
    std::vector<uint8_t> frame(TEL_MAX_FRAME);
    int flen = tel_frame_encode(payload, len, frame.data(), frame.size());
    if (flen < 0) return {};
    frame.resize(static_cast<size_t>(flen));
    return frame;
}

uint16_t Telemetry::crc16(const uint8_t *data, size_t len)
{
    return tel_crc16(data, len);
}

int Telemetry::transportTrampoline(const uint8_t *data, size_t len)
{
    if (active_instance_ && active_instance_->transport_)
        return active_instance_->transport_(data, len);
    return -1;
}

} // namespace jetecu
