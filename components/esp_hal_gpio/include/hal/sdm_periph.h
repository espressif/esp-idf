/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"

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

#endif // SOC_HAS(SDM)

#ifdef __cplusplus
}
#endif
