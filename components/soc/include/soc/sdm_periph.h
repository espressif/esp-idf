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

// helper macros to access module attributes
#define SOC_SDM_ATTR(_attr) SOC_MODULE_ATTR(SDM, _attr)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char *module_name; // Module name
    struct {
        const int sig_id_matrix; // signal ID in the GPIO matrix
    } channels[SOC_SDM_ATTR(CHANS_PER_INST)];
} soc_sdm_signal_desc_t;

extern const soc_sdm_signal_desc_t soc_sdm_signals[SOC_SDM_ATTR(INST_NUM)];

#ifdef __cplusplus
}
#endif
