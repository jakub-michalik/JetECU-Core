#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core/ecu_core.h"
#include "core/ecu_config_load.h"
#include "sim/sim_engine.h"
#include "sim/sim_replay.h"

#define SIM_DT 0.001f
#define DEFAULT_DURATION 30.0f

static void usage(const char *prog)
{
    fprintf(stderr, "Usage: %s [options]\n", prog);
    fprintf(stderr, "  -c <file>   Config JSON file\n");
    fprintf(stderr, "  -r <file>   Replay CSV file (overrides engine model)\n");
    fprintf(stderr, "  -d <secs>   Simulation duration (default: %.0f)\n", DEFAULT_DURATION);
    fprintf(stderr, "  -q          Quiet (no CSV output)\n");
    fprintf(stderr, "  -h          Help\n");
}

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
    const char *replay_file = NULL;
    float duration = DEFAULT_DURATION;
    int quiet = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0 && i + 1 < argc)
            config_file = argv[++i];
        else if (strcmp(argv[i], "-r") == 0 && i + 1 < argc)
            replay_file = argv[++i];
        else if (strcmp(argv[i], "-d") == 0 && i + 1 < argc)
            duration = (float)atof(argv[++i]);
        else if (strcmp(argv[i], "-q") == 0)
            quiet = 1;
        else if (strcmp(argv[i], "-h") == 0) {
            usage(argv[0]);
            return 0;
        }
    }

    if (config_file) {
        if (ecu_config_load_file(config_file, &cfg) != ECU_OK) {
            fprintf(stderr, "Warning: failed to load %s, using defaults\n", config_file);
        }
    } else {
        ecu_config_defaults(&cfg);
    }

    ecu_t ecu;
    ecu_init(&ecu, &cfg);

    sim_engine_t engine;
    sim_engine_init(&engine);

    sim_replay_t replay;
    int use_replay = 0;
    if (replay_file) {
        if (sim_replay_open(&replay, replay_file) == 0) {
            use_replay = 1;
        } else {
            fprintf(stderr, "Error: failed to open replay file %s\n", replay_file);
            return 1;
        }
    }

    if (!quiet) print_header();

    float t = 0.0f;
    int print_interval = 100;
    int step = 0;

    while (t < duration) {
        ecu_inputs_t in;

        if (use_replay) {
            sim_replay_get(&replay, t, &in);
        } else {
            float throttle = 0.0f;
            if (t > 1.0f) throttle = 80.0f;
            if (t > 20.0f) throttle = 50.0f;
            if (t > 25.0f) throttle = 0.0f;

            in.rpm = engine.rpm;
            in.egt = engine.egt;
            in.throttle = throttle;
        }

        ecu_outputs_t out = ecu_step(&ecu, &in, SIM_DT);

        if (!use_replay) {
            float starter = out.starter_on ? 500.0f : 0.0f;
            sim_engine_step(&engine, out.fuel_pct, starter, SIM_DT);
        }

        if (!quiet && step % print_interval == 0) {
            print_row(t, &out, &in);
        }

        t += SIM_DT;
        step++;
    }

    if (use_replay) sim_replay_close(&replay);

    if (ecu.fault_code) {
        fprintf(stderr, "Fault code: 0x%02x\n", ecu.fault_code);
        return 1;
    }
    return 0;
}
