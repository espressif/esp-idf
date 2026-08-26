/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_gdbstub_memory_regions_common.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline bool is_valid_memory_region(intptr_t addr)
{
    /* We shouldn't read transport registers since it will disturb the debugging.  */
    return (!is_transport_memory_region(addr)) &&
           addr >= 0x20000000 && addr < 0x80000000;
}

#ifdef __cplusplus
}
#endif
