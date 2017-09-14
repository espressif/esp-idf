// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#ifndef _PSRAM_H
#define _PSRAM_H
#include "soc/spi_reg.h"
#include "esp_err.h"
#include "sdkconfig.h"

typedef enum {
    PSRAM_CACHE_F80M_S40M = 0,
    PSRAM_CACHE_F40M_S40M,
    PSRAM_CACHE_F80M_S80M,
    PSRAM_CACHE_MAX,
} psram_cache_mode_t;


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

#endif
