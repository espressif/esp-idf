/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/*
Note: This is a compatibility header. Call the interfaces in esp_cpu.h instead
*/

#include "soc/soc_caps.h"
#include "hal/soc_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_CPU_CORES_NUM > 1   // We only allow stalling/unstalling of other cores
/**
 * Stall the specified CPU core.
 *
 * @note Has no effect if the core is already stalled - does not return an
 * ESP_ERR_INVALID_STATE.
 *
 * @param core core to stall [0..SOC_CPU_CORES_NUM - 1]
 */
#define soc_hal_stall_core(core)        soc_ll_stall_core(core)

/**
 * Unstall the specified CPU core.
 *
 * @note Has no effect if the core is already unstalled - does not return an
 * ESP_ERR_INVALID_STATE.
 *
 * @param core core to unstall [0..SOC_CPU_CORES_NUM - 1]
 */
#define soc_hal_unstall_core(core)      soc_ll_unstall_core(core)
#endif // SOC_CPU_CORES_NUM > 1

/**
 * Reset the specified core.
 *
 * @param core core to reset [0..SOC_CPU_CORES_NUM - 1]
 */
#define soc_hal_reset_core(core)        soc_ll_reset_core((core))

#ifdef __cplusplus
}
#endif
