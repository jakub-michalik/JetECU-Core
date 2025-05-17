#ifndef SIM_REPLAY_H
#define SIM_REPLAY_H

#include "core/ecu_core.h"
#include <stdio.h>

/* CSV format: time_s,rpm,egt,throttle */
typedef struct {
    float time_s;
    float rpm;
    float egt;
    float throttle;
} sim_replay_row_t;

typedef struct {
    FILE *fp;
    sim_replay_row_t current;
    sim_replay_row_t next;
    int has_next;
    int line_num;
} sim_replay_t;

/* Open replay file */
int sim_replay_open(sim_replay_t *rep, const char *path);

/* Get interpolated inputs at time t */
int sim_replay_get(sim_replay_t *rep, float t, ecu_inputs_t *inputs);

/* Close replay file */
void sim_replay_close(sim_replay_t *rep);

#endif /* SIM_REPLAY_H */
