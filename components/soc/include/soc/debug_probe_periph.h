/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_DEBUG_PROBE_SUPPORTED
typedef struct {
    struct {
        const int out_sig[SOC_DEBUG_PROBE_MAX_OUTPUT_WIDTH];
    } units[SOC_DEBUG_PROBE_NUM_UNIT];
} debug_probe_signal_conn_t;

extern const debug_probe_signal_conn_t debug_probe_periph_signals;

#endif // SOC_DEBUG_PROBE_SUPPORTED
#ifdef __cplusplus
}
#endif
