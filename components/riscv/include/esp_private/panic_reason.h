/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

typedef enum {
    PANIC_RSN_NONE = 0,
    PANIC_RSN_INTWDT_CPU0,
#if SOC_CPU_NUM > 1
    PANIC_RSN_INTWDT_CPU1,
#endif
    PANIC_RSN_CACHEERR,
#if CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
    PANIC_RSN_MEMPROT,
#endif
    PANIC_RSN_COUNT
} panic_reasons_t;
