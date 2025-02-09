#include "core/ecu_config_load.h"
#include "core/ecu_json.h"
#include <stdio.h>
#include <stdlib.h>

static void apply_json(const json_obj_t *obj, ecu_config_t *cfg)
{
    const float *v;

    if ((v = json_get(obj, "rpm_idle")))          cfg->rpm_idle = *v;
    if ((v = json_get(obj, "rpm_max")))           cfg->rpm_max = *v;
    if ((v = json_get(obj, "rpm_start_target")))  cfg->rpm_start_target = *v;
    if ((v = json_get(obj, "egt_max")))           cfg->egt_max = *v;
    if ((v = json_get(obj, "egt_start_min")))     cfg->egt_start_min = *v;
    if ((v = json_get(obj, "fuel_start_pct")))    cfg->fuel_start_pct = *v;
    if ((v = json_get(obj, "fuel_max_pct")))      cfg->fuel_max_pct = *v;
    if ((v = json_get(obj, "fuel_ramp_rate")))    cfg->fuel_ramp_rate = *v;
    if ((v = json_get(obj, "prestart_timeout_ms")))  cfg->prestart_timeout_ms = (uint32_t)*v;
    if ((v = json_get(obj, "spinup_timeout_ms")))    cfg->spinup_timeout_ms = (uint32_t)*v;
    if ((v = json_get(obj, "ignition_timeout_ms")))  cfg->ignition_timeout_ms = (uint32_t)*v;
    if ((v = json_get(obj, "rpm_ramp_rate")))     cfg->rpm_ramp_rate = *v;
}

ecu_err_t ecu_config_load_str(const char *json, ecu_config_t *cfg)
{
    /* Start from defaults */
    ecu_config_defaults(cfg);

    json_obj_t obj;
    if (json_parse(json, &obj) != 0) {
        return ECU_ERR_CONFIG;
    }

    apply_json(&obj, cfg);
    return ECU_OK;
}

ecu_err_t ecu_config_load_file(const char *path, ecu_config_t *cfg)
{
    FILE *f = fopen(path, "r");
    if (!f) {
        /* Fall back to defaults */
        ecu_config_defaults(cfg);
        return ECU_ERR_CONFIG;
    }

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buf = malloc((size_t)len + 1);
    if (!buf) {
        fclose(f);
        ecu_config_defaults(cfg);
        return ECU_ERR_CONFIG;
    }

    size_t nread = fread(buf, 1, (size_t)len, f);
    buf[nread] = '\0';
    fclose(f);

    ecu_err_t err = ecu_config_load_str(buf, cfg);
    free(buf);
    return err;
}
