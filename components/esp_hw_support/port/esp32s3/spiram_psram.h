/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#ifndef _PSRAM_H
#define _PSRAM_H
#include "soc/spi_mem_reg.h"
#include "esp_err.h"
#include "sdkconfig.h"

typedef enum {
    PSRAM_CACHE_S80M = 1,
    PSRAM_CACHE_S40M,
    PSRAM_CACHE_MAX,
} psram_cache_mode_t;

typedef enum {
    PSRAM_SIZE_16MBITS  = 0,
    PSRAM_SIZE_32MBITS  = 1,
    PSRAM_SIZE_64MBITS  = 2,
    PSRAM_SIZE_128MBITS = 3,
    PSRAM_SIZE_256MBITS = 4,
    PSRAM_SIZE_MAX,
} psram_size_t;

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
 * @brief get psram size
 * @return
 *     - PSRAM_SIZE_MAX if psram not enabled or not valid
 *     - PSRAM size
 */
psram_size_t psram_get_size(void);

/**
 * @brief psram cache enable function
 *
 * Esp-idf uses this to initialize cache for psram, mapping it into the main memory
 * address space.
 *
 * @param mode       SPI mode to access psram in
 * @param vaddrmode  Mode the psram cache works in.
 * @return ESP_OK on success, ESP_ERR_INVALID_STATE when VSPI peripheral is needed but cannot be claimed.
 */
esp_err_t psram_enable(psram_cache_mode_t mode, psram_vaddr_mode_t vaddrmode);

typedef enum {
    SPIRAM_WRAP_MODE_16B,
    SPIRAM_WRAP_MODE_32B,
    SPIRAM_WRAP_MODE_64B,
    SPIRAM_WRAP_MODE_DISABLE
} spiram_wrap_mode_t;

esp_err_t esp_spiram_wrap_set(spiram_wrap_mode_t mode);

/**
 * @brief get psram CS IO
 *
 * @return psram CS IO
 */
uint8_t psram_get_cs_io(void);

#endif
