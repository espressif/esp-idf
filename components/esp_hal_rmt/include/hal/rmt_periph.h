/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#if SOC_HAS(RMT)
#include "hal/rmt_ll.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_HAS(RMT)

typedef struct {
    const int irq;
    const char *module_name;
    struct {
        struct {
            const int tx_sig;
            const int rx_sig;
        };
    } channels[RMT_LL_GET(CHANS_PER_INST)];
} soc_rmt_signal_desc_t;

extern const soc_rmt_signal_desc_t soc_rmt_signals[RMT_LL_GET(INST_NUM)];

#endif // SOC_HAS(RMT)

#ifdef __cplusplus
}
#endif
