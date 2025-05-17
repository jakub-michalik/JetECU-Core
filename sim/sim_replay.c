#include "sim/sim_replay.h"
#include <stdlib.h>
#include <string.h>

static int parse_row(const char *line, sim_replay_row_t *row)
{
    return sscanf(line, "%f,%f,%f,%f",
                  &row->time_s, &row->rpm, &row->egt, &row->throttle) == 4 ? 0 : -1;
}

static int read_next(sim_replay_t *rep)
{
    char line[256];
    while (fgets(line, sizeof(line), rep->fp)) {
        rep->line_num++;
        if (line[0] == '#' || line[0] == '\n') continue;
        if (parse_row(line, &rep->next) == 0) {
            rep->has_next = 1;
            return 0;
        }
    }
    rep->has_next = 0;
    return -1;
}

int sim_replay_open(sim_replay_t *rep, const char *path)
{
    memset(rep, 0, sizeof(*rep));
    rep->fp = fopen(path, "r");
    if (!rep->fp) return -1;

    /* Skip header line */
    char line[256];
    if (fgets(line, sizeof(line), rep->fp)) {
        rep->line_num = 1;
    }

    /* Read first two rows */
    if (read_next(rep) != 0) return -1;
    rep->current = rep->next;
    read_next(rep);

    return 0;
}

int sim_replay_get(sim_replay_t *rep, float t, ecu_inputs_t *inputs)
{
    /* Advance to bracket the time */
    while (rep->has_next && rep->next.time_s <= t) {
        rep->current = rep->next;
        read_next(rep);
    }

    if (!rep->has_next) {
        /* Use last known values */
        inputs->rpm = rep->current.rpm;
        inputs->egt = rep->current.egt;
        inputs->throttle = rep->current.throttle;
        return 0;
    }

    /* Linear interpolation */
    float dt = rep->next.time_s - rep->current.time_s;
    if (dt < 0.0001f) {
        inputs->rpm = rep->current.rpm;
        inputs->egt = rep->current.egt;
        inputs->throttle = rep->current.throttle;
    } else {
        float frac = (t - rep->current.time_s) / dt;
        inputs->rpm = rep->current.rpm + frac * (rep->next.rpm - rep->current.rpm);
        inputs->egt = rep->current.egt + frac * (rep->next.egt - rep->current.egt);
        inputs->throttle = rep->current.throttle + frac * (rep->next.throttle - rep->current.throttle);
    }

    return 0;
}

void sim_replay_close(sim_replay_t *rep)
{
    if (rep->fp) {
        fclose(rep->fp);
        rep->fp = NULL;
    }
}
