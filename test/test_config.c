#include "test/test_runner.h"
#include "core/ecu_config.h"
#include "core/ecu_config_load.h"

TEST(test_config_defaults)
{
    ecu_config_t cfg;
    ecu_config_defaults(&cfg);
    ASSERT(cfg.rpm_idle > 0);
    ASSERT(cfg.egt_max > 0);
    ASSERT(cfg.fuel_max_pct > 0);
}

TEST(test_config_from_json)
{
    const char *json = "{ \"rpm_idle\": 50000, \"egt_max\": 900 }";
    ecu_config_t cfg;
    ecu_err_t err = ecu_config_load_str(json, &cfg);
    ASSERT_EQ(err, ECU_OK);
    ASSERT(cfg.rpm_idle > 49999.0f && cfg.rpm_idle < 50001.0f);
    ASSERT(cfg.egt_max > 899.0f && cfg.egt_max < 901.0f);
    /* unspecified keys should be defaults */
    ASSERT(cfg.fuel_start_pct > 0);
}

TEST(test_config_bad_json)
{
    const char *json = "this is not json";
    ecu_config_t cfg;
    ecu_err_t err = ecu_config_load_str(json, &cfg);
    ASSERT_EQ(err, ECU_ERR_CONFIG);
}

TEST(test_config_empty_json)
{
    const char *json = "{}";
    ecu_config_t cfg;
    ecu_err_t err = ecu_config_load_str(json, &cfg);
    ASSERT_EQ(err, ECU_OK);
    /* everything should be defaults */
    ASSERT(cfg.rpm_idle > 0);
}

int main(void)
{
    printf("=== Config Tests ===\n");
    RUN_TEST(test_config_defaults);
    RUN_TEST(test_config_from_json);
    RUN_TEST(test_config_bad_json);
    RUN_TEST(test_config_empty_json);
    TEST_SUMMARY();
}
