/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/timer_group_reg.h"
#include "soc/timer_group_struct.h"
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The value that needs to be written to TIMG_WDT_WKEY to write-enable the wdt registers */
#define TIMG_WDT_WKEY_VALUE 0x50D83AA1

typedef struct {
    struct {
        const periph_module_t module; // Peripheral module
        const int timer_irq_id[SOC_TIMER_GROUP_TIMERS_PER_GROUP]; // interrupt source ID
    } groups[SOC_TIMER_GROUPS];
} timer_group_signal_conn_t;

extern const timer_group_signal_conn_t timer_group_periph_signals;

#ifdef __cplusplus
}
#endif
