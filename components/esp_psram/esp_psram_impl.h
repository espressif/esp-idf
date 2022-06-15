/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PSRAM_SIZE_2MB                  (2 * 1024 * 1024)
#define PSRAM_SIZE_4MB                  (4 * 1024 * 1024)
#define PSRAM_SIZE_8MB                  (8 * 1024 * 1024)
#define PSRAM_SIZE_16MB                 (16 * 1024 * 1024)
#define PSRAM_SIZE_32MB                 (32 * 1024 * 1024)


/*
See the TRM, chapter PID/MPU/MMU, header 'External RAM' for the definitions of these modes.

Important is that NORMAL works with the app CPU cache disabled, but gives huge cache coherency
issues when both app and pro CPU are enabled. LOWHIGH and EVENODD do not have these coherency
issues but cannot be used when the app CPU cache is disabled.
*/
typedef enum {
    PSRAM_VADDR_MODE_NORMAL=0, ///< App and pro CPU use their own flash cache for external RAM access
    PSRAM_VADDR_MODE_LOWHIGH,  ///< App and pro CPU share external RAM caches: pro CPU has low 2M, app CPU has high 2M
    PSRAM_VADDR_MODE_EVENODD,  ///< App and pro CPU share external RAM caches: pro CPU does even 32yte ranges, app does odd ones.
} psram_vaddr_mode_t;

/**
 * @brief To get the physical psram size in bytes.
 *
 * @param[out] out_size_bytes    physical psram size in bytes.
 */
esp_err_t esp_psram_impl_get_physical_size(uint32_t *out_size_bytes);

/**
 * @brief To get the available physical psram size in bytes.
 *
 * @param[out] out_size_bytes    availabe physical psram size in bytes.
 */
esp_err_t esp_psram_impl_get_available_size(uint32_t *out_size_bytes);

/**
 * @brief Enable psram and configure it to a ready state
 *
 * @param vaddrmode  Mode the psram cache works in.
 * @return
 *        - ESP_OK:                On success,
 *        - ESP_ERR_INVALID_STATE: On esp32, when VSPI peripheral is needed but cannot be claimed.
 */
esp_err_t esp_psram_impl_enable(psram_vaddr_mode_t vaddrmode);

/**
 * @brief get psram CS IO
 *
 * @return psram CS IO
 */
uint8_t esp_psram_impl_get_cs_io(void);
