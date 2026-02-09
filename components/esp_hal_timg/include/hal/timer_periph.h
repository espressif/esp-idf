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
#include "soc/regdma.h"
#include "hal/timer_ll.h"

#if SOC_HAS(PAU)
#include "soc/retention_periph_defs.h"
#endif // SOC_HAS(PAU)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char *module_name;              // Module name
    const shared_periph_module_t parent_module;  // GPTimer is a submodule under the timer group
    const int irq_id;                     // interrupt source ID
} soc_timg_gptimer_signal_desc_t;

extern const soc_timg_gptimer_signal_desc_t soc_timg_gptimer_signals[TIMG_LL_GET(INST_NUM)][TIMG_LL_GET(GPTIMERS_PER_INST)];

#if SOC_HAS(PAU)
typedef struct {
    const periph_retention_module_t module;             // ID of the GPTimer as a retention module
    const regdma_entries_config_t *regdma_entry_array;  // Array of regdma entries for retention
    const size_t array_size;                            // Size of the regdma_entry_array
} soc_timg_gptimer_retention_desc_t;

extern const soc_timg_gptimer_retention_desc_t soc_timg_gptimer_retention_infos[TIMG_LL_GET(INST_NUM)][TIMG_LL_GET(GPTIMERS_PER_INST)];
#endif // SOC_HAS(PAU)

#ifdef __cplusplus
}
#endif
