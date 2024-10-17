/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
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
#define DEBUG_PROBE_MAX_OUTPUT_WIDTH SOC_DEBUG_PROBE_MAX_OUTPUT_WIDTH
typedef soc_debug_probe_target_t debug_probe_target_t;
#else
#define DEBUG_PROBE_MAX_OUTPUT_WIDTH 16
typedef int debug_probe_target_t;
#endif

#ifdef __cplusplus
}
#endif
