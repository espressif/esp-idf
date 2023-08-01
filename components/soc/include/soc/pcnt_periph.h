/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_PCNT_SUPPORTED

typedef struct {
    struct {
        struct {
            struct {
                const uint32_t pulse_sig;
                const uint32_t control_sig;
            } channels[SOC_PCNT_CHANNELS_PER_UNIT];
            const uint32_t clear_sig;
        } units[SOC_PCNT_UNITS_PER_GROUP];
        const uint32_t irq;
        const periph_module_t module;
    } groups[SOC_PCNT_GROUPS];
} pcnt_signal_conn_t;

extern const pcnt_signal_conn_t pcnt_periph_signals;

#endif // SOC_PCNT_SUPPORTED

#ifdef __cplusplus
}
#endif
