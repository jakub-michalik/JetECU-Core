#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core/ecu_core.h"
#include "core/ecu_config_load.h"
#include "sim/sim_engine.h"

#define SIM_DT 0.001f   /* 1ms step */
#define SIM_DURATION 30.0f  /* seconds */

static void print_header(void)
{
    printf("time_s,state,rpm,egt,fuel_pct,throttle,igniter,starter\n");
}

static void print_row(float t, const ecu_outputs_t *out, const ecu_inputs_t *in)
{
    printf("%.3f,%d,%.0f,%.1f,%.1f,%.0f,%d,%d\n",
           t, out->state, in->rpm, in->egt,
           out->fuel_pct, in->throttle,
           out->igniter_on, out->starter_on);
}

int main(int argc, char *argv[])
{
    ecu_config_t cfg;
    const char *config_file = NULL;
    float duration = SIM_DURATION;

    /* Parse args */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
            config_file = argv[++i];
        } else if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
            duration = (float)atof(argv[++i]);
        }
    }

    if (config_file) {
        if (ecu_config_load_file(config_file, &cfg) != ECU_OK) {
            fprintf(stderr, "Warning: failed to load %s, using defaults\n", config_file);
            ecu_config_defaults(&cfg);
        }
    } else {
        ecu_config_defaults(&cfg);
    }

    ecu_t ecu;
    ecu_init(&ecu, &cfg);

    sim_engine_t engine;
    sim_engine_init(&engine);

    print_header();

    float t = 0.0f;
    int print_interval = 100;  /* print every 100 steps (100ms) */
    int step = 0;

    while (t < duration) {
        /* Simple throttle profile */
        float throttle = 0.0f;
        if (t > 1.0f) throttle = 80.0f;         /* start at t=1s */
        if (t > 20.0f) throttle = 50.0f;        /* reduce */
        if (t > 25.0f) throttle = 0.0f;         /* shutdown */

        ecu_inputs_t in = {
            .rpm = engine.rpm,
            .egt = engine.egt,
            .throttle = throttle,
        };

        ecu_outputs_t out = ecu_step(&ecu, &in, SIM_DT);

        /* Feed outputs back to engine model */
        float starter = out.starter_on ? 500.0f : 0.0f;
        sim_engine_step(&engine, out.fuel_pct, starter, SIM_DT);

        if (step % print_interval == 0) {
            print_row(t, &out, &in);
        }

        t += SIM_DT;
        step++;
    }

    if (ecu.fault_code) {
        fprintf(stderr, "Simulation ended with fault code: 0x%02x\n", ecu.fault_code);
        return 1;
    }
    return 0;
}
