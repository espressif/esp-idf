/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include "esp_compiler.h"

struct cookie_wrap {
    void *cookie;
    cookie_io_functions_t fn;
};

static int fun_read(void *c, char *buf, int size)
{
    struct cookie_wrap *w = c;
    return (int)w->fn.read(w->cookie, buf, (size_t)size);
}

static int fun_write(void *c, const char *buf, int size)
{
    struct cookie_wrap *w = c;
    return (int)w->fn.write(w->cookie, buf, (size_t)size);
}

static off_t fun_seek(void *c, off_t off, int whence)
{
    struct cookie_wrap *w = c;
    return w->fn.seek(w->cookie, &off, whence);
}

static int fun_close(void *c)
{
    struct cookie_wrap *w = c;
    int r = w->fn.close ? w->fn.close(w->cookie) : 0;
    free(w);
    return r;
}

FILE *fopencookie(void *cookie, __attribute__((unused)) const char *mode,
                  cookie_io_functions_t fn)
{
    ESP_COMPILER_DIAGNOSTIC_PUSH_IGNORE("-Wanalyzer-malloc-leak")
    struct cookie_wrap *w = malloc(sizeof * w);
    if (!w) {
        return NULL;
    }

    w->cookie = cookie;
    w->fn = fn;

    FILE *fp = funopen(w,
                       fn.read  ? fun_read  : NULL,
                       fn.write ? fun_write : NULL,
                       fn.seek  ? fun_seek  : NULL,
                       fun_close);

    if (!fp) {
        free(w);
    }
    return fp;
    ESP_COMPILER_DIAGNOSTIC_POP("-Wanalyzer-malloc-leak")
}
