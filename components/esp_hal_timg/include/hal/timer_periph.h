/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include "soc/timer_group_reg.h"
#include "soc/timer_group_struct.h"
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#include "hal/timer_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char *module_name;              // Module name
    const shared_periph_module_t parent_module;  // GPTimer is a submodule under the timer group
    const int irq_id;                     // interrupt source ID
} soc_timg_gptimer_signal_desc_t;

extern const soc_timg_gptimer_signal_desc_t soc_timg_gptimer_signals[TIMG_LL_GET(INST_NUM)][TIMG_LL_GET(GPTIMERS_PER_INST)];

#ifdef __cplusplus
}
#endif
