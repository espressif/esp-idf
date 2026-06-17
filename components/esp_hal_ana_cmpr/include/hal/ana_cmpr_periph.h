/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/interrupts.h"
#if SOC_ANA_CMPR_SUPPORTED
#include "hal/ana_cmpr_ll.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_ANA_CMPR_SUPPORTED
typedef struct {
    union {
        struct {
            const int src_gpio;     // Source GPIO number
            const int ext_ref_gpio; // External reference GPIO number
        };
        const int pad_gpios[ANALOG_CMPR_LL_GET(PAD_NUM)]; // Array of GPIO numbers for the pads, indexed by pad number
    };
    const int intr_src;      // Interrupt source ID
    const char *module_name; // Module name
} ana_cmpr_periph_t;

extern const ana_cmpr_periph_t ana_cmpr_periph[ANALOG_CMPR_LL_GET(INST_NUM)];
#endif

#ifdef __cplusplus
}
#endif
