#include "cpp/jetecu/Config.h"

namespace jetecu {

Config::Config()
{
    ecu_config_defaults(&cfg_);
}

bool Config::loadFile(const std::string &path)
{
    return ecu_config_load_file(path.c_str(), &cfg_) == ECU_OK;
}

bool Config::loadString(const std::string &json)
{
    return ecu_config_load_str(json.c_str(), &cfg_) == ECU_OK;
}

} // namespace jetecu
