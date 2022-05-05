/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_cpu.h"
#include "esp_memory_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline void __attribute__((always_inline)) compare_and_set_native(volatile uint32_t *addr, uint32_t compare, uint32_t *set)
{
    cpu_ll_compare_and_set_native(addr, compare, set);
}

void compare_and_set_extram(volatile uint32_t *addr, uint32_t compare, uint32_t *set);

#ifdef __cplusplus
}
#endif
