#ifndef JETECU_ENGINE_H
#define JETECU_ENGINE_H

#include "cpp/jetecu/Types.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "core/ecu_core.h"
#include "core/ecu_config.h"
#ifdef __cplusplus
}
#endif

namespace jetecu {

class Engine {
public:
    Engine();
    explicit Engine(const ecu_config_t &cfg);
    ~Engine() = default;

    Engine(const Engine &) = delete;
    Engine &operator=(const Engine &) = delete;
    Engine(Engine &&) = default;
    Engine &operator=(Engine &&) = default;

    Outputs step(const Inputs &in, float dt);
    State state() const;
    const char *stateName() const;

    ecu_t &raw() { return ecu_; }
    const ecu_t &raw() const { return ecu_; }

private:
    ecu_t ecu_;
};

} // namespace jetecu

#endif // JETECU_ENGINE_H
