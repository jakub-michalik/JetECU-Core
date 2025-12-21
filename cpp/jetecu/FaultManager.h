#ifndef JETECU_FAULT_MANAGER_H
#define JETECU_FAULT_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif
#include "core/ecu_fault.h"
#ifdef __cplusplus
}
#endif

#include <cstdint>

namespace jetecu {

class FaultManager {
public:
    FaultManager();

    void report(uint16_t code, ecu_fault_severity_t severity,
                ecu_fault_action_t action, uint32_t now);
    void clear(uint16_t code);
    bool isActive(uint16_t code) const;
    int count() const;
    ecu_fault_action_t action() const;
    bool tryRetry(uint16_t code);
    void setRetries(uint16_t code, uint8_t max_retries);

    ecu_fault_mgr_t &raw() { return mgr_; }
    const ecu_fault_mgr_t &raw() const { return mgr_; }

private:
    ecu_fault_mgr_t mgr_;
};

} // namespace jetecu

#endif // JETECU_FAULT_MANAGER_H
