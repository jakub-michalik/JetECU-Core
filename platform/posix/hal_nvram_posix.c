#include "hal/hal_nvram.h"
#include <stdio.h>
#include <string.h>

/* Simple file-based NVRAM for POSIX */
static char nvram_path[256] = "/tmp/jetecu_nvram";

int hal_nvram_init(void) { return 0; }

static void make_filename(char *buf, size_t buflen, const char *key)
{
    snprintf(buf, buflen, "%s_%s.bin", nvram_path, key);
}

int hal_nvram_read(const char *key, void *buf, size_t len)
{
    char fname[512];
    make_filename(fname, sizeof(fname), key);
    FILE *f = fopen(fname, "rb");
    if (!f) return -1;
    int n = (int)fread(buf, 1, len, f);
    fclose(f);
    return n;
}

int hal_nvram_write(const char *key, const void *buf, size_t len)
{
    char fname[512];
    make_filename(fname, sizeof(fname), key);
    FILE *f = fopen(fname, "wb");
    if (!f) return -1;
    fwrite(buf, 1, len, f);
    fclose(f);
    return 0;
}

int hal_nvram_erase(const char *key)
{
    char fname[512];
    make_filename(fname, sizeof(fname), key);
    return remove(fname);
}
