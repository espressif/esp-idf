/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdio.h>
#include "esp_err.h"
#include "soc/soc_caps.h"
#include "hal/debug_probe_types.h"
#if SOC_DEBUG_PROBE_SUPPORTED
#include "soc/debug_probe_targets.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_DEBUG_PROBE_SUPPORTED
#define DEBUG_PROBE_MAX_OUTPUT_WIDTH   SOC_DEBUG_PROBE_MAX_OUTPUT_WIDTH
#define DEBUG_PROBE_GPIO_NUMS_NONE      { [0 ... (SOC_DEBUG_PROBE_MAX_OUTPUT_WIDTH - 1)] = -1 }
typedef soc_debug_probe_target_t debug_probe_target_t;
typedef soc_debug_probe_target_lp_t debug_probe_target_lp_t;
#else
#define DEBUG_PROBE_MAX_OUTPUT_WIDTH   16
#define DEBUG_PROBE_GPIO_NUMS_NONE     { [0 ... 15] = -1 }
typedef int debug_probe_target_t;
typedef int debug_probe_target_lp_t;
#endif

#ifdef __cplusplus
}
#endif
