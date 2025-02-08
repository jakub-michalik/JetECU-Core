#ifndef ECU_JSON_H
#define ECU_JSON_H

/*
 * Dead simple JSON parser — only handles flat { "key": number } objects.
 * No arrays, no nesting, no strings. Just enough for config loading.
 */

#include <stddef.h>

#define JSON_MAX_KEYS 32
#define JSON_KEY_LEN  64

typedef struct {
    char  key[JSON_KEY_LEN];
    float value;
} json_entry_t;

typedef struct {
    json_entry_t entries[JSON_MAX_KEYS];
    int count;
} json_obj_t;

/* Parse flat JSON object from string. Returns 0 on success, -1 on error. */
int json_parse(const char *str, json_obj_t *obj);

/* Find a key. Returns pointer to value or NULL if not found. */
const float *json_get(const json_obj_t *obj, const char *key);

#endif /* ECU_JSON_H */
