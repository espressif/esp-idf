/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _NVS_H
#define _NVS_H

#include <stdio.h>
#include "nvs.h"
#include "nimble/storage_port.h"

static int
nvs_open_custom(const char* namespace_name, open_mode_t open_mode, cache_handle_t *out_handle)
{
    switch (open_mode) {
    case READONLY:
        return nvs_open(namespace_name, NVS_READONLY, out_handle);

    case READWRITE:
        return nvs_open(namespace_name, NVS_READWRITE, out_handle);

    default:
        return -1;
    }
}

struct cache_fn_mapping
link_storage_fn(void *storage_cb)
{
    struct cache_fn_mapping cache_fn;
    cache_fn.open = nvs_open_custom;
    cache_fn.close = nvs_close;
    cache_fn.erase_all = nvs_erase_all;
    cache_fn.write = nvs_set_blob;
    cache_fn.read = nvs_get_blob;
    return cache_fn;
}
#endif
