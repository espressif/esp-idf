/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include "soc/soc_caps_full.h"
#include "soc/interrupts.h"

// helper macros to access module attributes
#define SOC_PCNT_ATTR(_attr) SOC_MODULE_ATTR(PCNT, _attr)

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_HAS(PCNT)

typedef struct {
    const char *module_name; // module name
    struct {
        struct {
            const int pulse_sig_id_matrix; // pulse signal ID in the GPIO matrix
            const int ctl_sig_id_matrix;   // control signal ID in the GPIO matrix
        } channels[SOC_PCNT_ATTR(CHANS_PER_UNIT)];
        const int clear_sig_id_matrix;     // clear signal ID in the GPIO matrix
    } units[SOC_PCNT_ATTR(UNITS_PER_INST)];
    const int irq_id; // interrupt source ID
} soc_pcnt_signal_desc_t;

extern const soc_pcnt_signal_desc_t soc_pcnt_signals[SOC_PCNT_ATTR(INST_NUM)];

#endif // SOC_HAS(PCNT)

#ifdef __cplusplus
}
#endif
