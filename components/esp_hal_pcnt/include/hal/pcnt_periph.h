/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include "soc/soc_caps.h"
#include "soc/interrupts.h"
#if SOC_HAS(PCNT)
#include "hal/pcnt_ll.h"
#endif

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
        } channels[PCNT_LL_GET(CHANS_PER_UNIT)];
        const int clear_sig_id_matrix;     // clear signal ID in the GPIO matrix
    } units[PCNT_LL_GET(UNITS_PER_INST)];
    const int irq_id; // interrupt source ID
} soc_pcnt_signal_desc_t;

extern const soc_pcnt_signal_desc_t soc_pcnt_signals[PCNT_LL_GET(INST_NUM)];

#endif // SOC_HAS(PCNT)

#ifdef __cplusplus
}
#endif
