/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#include "soc/pcnt_reg.h"
#include "soc/pcnt_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    struct {
        struct {
            struct {
                const uint32_t pulse_sig;
                const uint32_t control_sig;
            } channels[SOC_PCNT_CHANNELS_PER_UNIT];
        } units[SOC_PCNT_UNITS_PER_GROUP];
        const uint32_t irq;
        const periph_module_t module;
    } groups[SOC_PCNT_GROUPS];
} pcnt_signal_conn_t;

extern const pcnt_signal_conn_t pcnt_periph_signals;

#ifdef __cplusplus
}
#endif
