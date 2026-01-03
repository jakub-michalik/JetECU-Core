#include <gtest/gtest.h>

extern "C" {
#include "core/ecu_config.h"
#include "core/ecu_config_load.h"
}

TEST(Config, Defaults) {
    ecu_config_t cfg;
    ecu_config_defaults(&cfg);
    EXPECT_GT(cfg.rpm_idle, 0);
    EXPECT_GT(cfg.egt_max, 0);
    EXPECT_GT(cfg.fuel_max_pct, 0);
}

TEST(Config, FromJson) {
    const char *json = "{ \"rpm_idle\": 50000, \"egt_max\": 900 }";
    ecu_config_t cfg;
    ecu_err_t err = ecu_config_load_str(json, &cfg);
    EXPECT_EQ(err, ECU_OK);
    EXPECT_NEAR(cfg.rpm_idle, 50000.0f, 1.0f);
    EXPECT_NEAR(cfg.egt_max, 900.0f, 1.0f);
    EXPECT_GT(cfg.fuel_start_pct, 0);
}

TEST(Config, BadJson) {
    const char *json = "this is not json";
    ecu_config_t cfg;
    ecu_err_t err = ecu_config_load_str(json, &cfg);
    EXPECT_EQ(err, ECU_ERR_CONFIG);
}

TEST(Config, EmptyJson) {
    const char *json = "{}";
    ecu_config_t cfg;
    ecu_err_t err = ecu_config_load_str(json, &cfg);
    EXPECT_EQ(err, ECU_OK);
    EXPECT_GT(cfg.rpm_idle, 0);
}

/* ---- C++ wrapper tests ---- */

#include "cpp/jetecu/Config.h"

TEST(ConfigWrapper, Defaults) {
    jetecu::Config cfg;
    EXPECT_GT(cfg.get().rpm_idle, 0);
    EXPECT_GT(cfg.get().egt_max, 0);
}

TEST(ConfigWrapper, LoadString) {
    jetecu::Config cfg;
    EXPECT_TRUE(cfg.loadString("{ \"rpm_idle\": 50000 }"));
    EXPECT_NEAR(cfg.get().rpm_idle, 50000.0f, 1.0f);
}

TEST(ConfigWrapper, BadJson) {
    jetecu::Config cfg;
    EXPECT_FALSE(cfg.loadString("not json"));
}
