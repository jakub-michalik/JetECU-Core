#include "core/ecu_json.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

static const char *skip_ws(const char *p)
{
    while (*p && isspace((unsigned char)*p)) p++;
    return p;
}

int json_parse(const char *str, json_obj_t *obj)
{
    obj->count = 0;
    const char *p = skip_ws(str);

    if (*p != '{') return -1;
    p = skip_ws(p + 1);

    while (*p && *p != '}') {
        if (obj->count >= JSON_MAX_KEYS) return -1;

        /* expect "key" */
        if (*p != '"') return -1;
        p++;
        const char *key_start = p;
        while (*p && *p != '"') p++;
        if (!*p) return -1;

        size_t klen = (size_t)(p - key_start);
        if (klen >= JSON_KEY_LEN) klen = JSON_KEY_LEN - 1;
        memcpy(obj->entries[obj->count].key, key_start, klen);
        obj->entries[obj->count].key[klen] = '\0';
        p = skip_ws(p + 1);

        /* expect : */
        if (*p != ':') return -1;
        p = skip_ws(p + 1);

        /* expect number */
        char *end;
        obj->entries[obj->count].value = (float)strtod(p, &end);
        if (end == p) return -1;
        p = skip_ws(end);

        obj->count++;

        if (*p == ',') {
            p = skip_ws(p + 1);
        }
    }

    if (*p != '}') return -1;
    return 0;
}

const float *json_get(const json_obj_t *obj, const char *key)
{
    for (int i = 0; i < obj->count; i++) {
        if (strcmp(obj->entries[i].key, key) == 0) {
            return &obj->entries[i].value;
        }
    }
    return NULL;
}
