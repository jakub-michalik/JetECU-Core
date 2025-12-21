#ifndef JETECU_CONFIG_H
#define JETECU_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif
#include "core/ecu_config.h"
#include "core/ecu_config_load.h"
#ifdef __cplusplus
}
#endif

#include <string>

namespace jetecu {

class Config {
public:
    Config();

    bool loadFile(const std::string &path);
    bool loadString(const std::string &json);

    const ecu_config_t &get() const { return cfg_; }
    ecu_config_t &get() { return cfg_; }

private:
    ecu_config_t cfg_;
};

} // namespace jetecu

#endif // JETECU_CONFIG_H
