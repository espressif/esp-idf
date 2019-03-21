// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#ifndef __ESP_SPIRAM_H
#define __ESP_SPIRAM_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

typedef enum {
    ESP_SPIRAM_SIZE_16MBITS = 0,   /*!< SPI RAM size is 16 MBits */
    ESP_SPIRAM_SIZE_32MBITS = 1,   /*!< SPI RAM size is 32 MBits */
    ESP_SPIRAM_SIZE_64MBITS = 2,   /*!< SPI RAM size is 64 MBits */
    ESP_SPIRAM_SIZE_INVALID,       /*!< SPI RAM size is invalid */
} esp_spiram_size_t;

/**
 * @brief get SPI RAM size
 * @return
 *     - ESP_SPIRAM_SIZE_INVALID if SPI RAM not enabled or not valid
 *     - SPI RAM size
 */
esp_spiram_size_t esp_spiram_get_chip_size();

/**
 * @brief Initialize spiram interface/hardware. Normally called from cpu_start.c.
 *
 * @return ESP_OK on success
 */
esp_err_t esp_spiram_init();

/**
 * @brief Configure Cache/MMU for access to external SPI RAM.
 *
 * Normally this function is called from cpu_start, if CONFIG_SPIRAM_BOOT_INIT
 * option is enabled. Applications which need to enable SPI RAM at run time
 * can disable CONFIG_SPIRAM_BOOT_INIT, and call this function later.
 *
 * @attention this function must be called with flash cache disabled.
 */
void esp_spiram_init_cache();


/**
 * @brief Memory test for SPI RAM. Should be called after SPI RAM is initialized and
 * (in case of a dual-core system) the app CPU is online. This test overwrites the
 * memory with crap, so do not call after e.g. the heap allocator has stored important
 * stuff in SPI RAM.
 *
 * @return true on success, false on failed memory test
 */
bool esp_spiram_test();


/**
 * @brief Add the initialized SPI RAM to the heap allocator.
 */
esp_err_t esp_spiram_add_to_heapalloc();


/**
 * @brief Get the size of the attached SPI RAM chip selected in menuconfig
 *
 * @return Size in bytes, or 0 if no external RAM chip support compiled in.
 */
size_t esp_spiram_get_size();


/**
 * @brief Force a writeback of the data in the SPI RAM cache. This is to be called whenever
 * cache is disabled, because disabling cache on the ESP32 discards the data in the SPI
 * RAM cache.
 *
 * This is meant for use from within the SPI flash code.
 */
void esp_spiram_writeback_cache();



/**
 * @brief Reserve a pool of internal memory for specific DMA/internal allocations
 *
 * @param size Size of reserved pool in bytes
 *
 * @return
 *          - ESP_OK on success
 *          - ESP_ERR_NO_MEM when no memory available for pool
 */
esp_err_t esp_spiram_reserve_dma_pool(size_t size);


/**
 * @brief If SPI RAM(PSRAM) has been initialized
 *
 * @return
 *          - true SPI RAM has been initialized successfully
 *          - false SPI RAM hasn't been initialized or initialized failed
 */
bool esp_spiram_is_initialized(void);


#endif
