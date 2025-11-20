/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include "soc/soc_caps_full.h"
#include "soc/periph_defs.h"
#include "soc/regdma.h"

#if SOC_HAS(PAU)
#include "soc/retention_periph_defs.h"
#endif // SOC_HAS(PAU)

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_HAS(SDM)
#include "hal/sdm_caps.h"

typedef struct {
    const char *module_name; // Module name
    struct {
        const int sig_id_matrix; // signal ID in the GPIO matrix
    } channels[SDM_CAPS_GET(CHANS_PER_INST)];
} soc_sdm_signal_desc_t;

extern const soc_sdm_signal_desc_t soc_sdm_signals[SDM_CAPS_GET(INST_NUM)];

#if SOC_HAS(PAU)

typedef struct {
    const periph_retention_module_t module;             // ID of the SDM as a retention module
    const regdma_entries_config_t *regdma_entry_array;  // Array of regdma entries for retention
    const size_t array_size;                            // Size of the regdma_entry_array
} soc_sdm_retention_desc_t;

extern const soc_sdm_retention_desc_t soc_sdm_retention_infos[SDM_CAPS_GET(INST_NUM)];

#endif // SOC_HAS(PAU)

#endif // SOC_HAS(SDM)

#ifdef __cplusplus
}
#endif
