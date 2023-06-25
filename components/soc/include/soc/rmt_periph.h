/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/periph_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_RMT_SUPPORTED

typedef struct {
    struct {
        const int irq;
        const periph_module_t module;
        struct {
            struct {
                const int tx_sig;
                const int rx_sig;
            };
        } channels[SOC_RMT_CHANNELS_PER_GROUP];
    } groups[SOC_RMT_GROUPS];
} rmt_signal_conn_t;

extern const rmt_signal_conn_t rmt_periph_signals;

#endif // SOC_RMT_SUPPORTED

#ifdef __cplusplus
}
#endif
