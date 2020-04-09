// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

#pragma once
#include "esp_err.h"
#include <stdint.h>
#include <stdbool.h>
#include <driver/spi_common_internal.h>
#include "sdkconfig.h"

#include "esp_flash.h"

/** Internal API, don't use in the applications */

#ifdef __cplusplus
extern "C" {
#endif


/** @brief Initialise the default SPI flash chip
 *
 * Called by OS startup code. You do not need to call this in your own applications.
 */
#ifdef CONFIG_SPI_FLASH_USE_LEGACY_IMPL
#define esp_flash_init_default_chip(...) ({ESP_OK;})
#else
esp_err_t esp_flash_init_default_chip(void);
#endif

/**
 *  Enable OS-level SPI flash protections in IDF
 *
 *  Called by OS startup code. You do not need to call this in your own applications.
 *
 * @return ESP_OK if success, otherwise failed. See return value of ``esp_flash_init_os_functions``.
 */
#ifdef CONFIG_SPI_FLASH_USE_LEGACY_IMPL
#define esp_flash_app_init(...) ({ESP_OK;})
#else
esp_err_t esp_flash_app_init(void);
#endif

/**
 *  Disable (or enable) OS-level SPI flash protections in IDF
 *
 *  Called by the IDF internal code (e.g. coredump). You do not need to call this in your own applications.
 *
 * @return always ESP_OK.
 */
#ifdef CONFIG_SPI_FLASH_USE_LEGACY_IMPL
#define esp_flash_app_disable_protect(...) ({ESP_OK;})
#else
esp_err_t esp_flash_app_disable_protect(bool disable);
#endif

/**
 *  Initialize OS-level functions for a specific chip.
 *
 * @param chip The chip to init os functions.
 * @param host_id Which SPI host to use, 1 for SPI1, 2 for SPI2 (HSPI), 3 for SPI3 (VSPI)
 * @param out_dev_id Output of occupied device slot
 *
 * @return
 *      - ESP_OK if success
 *      - ESP_ERR_INVALID_ARG if host_id is invalid
 */
esp_err_t esp_flash_init_os_functions(esp_flash_t *chip, int host_id, int *out_dev_id);

/**
 * @brief Deinitialize OS-level functions
 *
 * @param chip  The chip to deinit os functions
 * @return always ESP_OK.
 */
esp_err_t esp_flash_deinit_os_functions(esp_flash_t* chip);

/**
 * @brief Initialize the bus lock on the SPI1 bus. Should be called if drivers (including esp_flash)
 * wants to use SPI1 bus.
 *
 * @note When using legacy spi flash API, the bus lock will not be available on SPI1 bus.
 *
 * @return esp_err_t always ESP_OK.
 */
esp_err_t esp_flash_init_main_bus_lock(void);

/**
 *  Initialize OS-level functions for the main flash chip.
 *
 * @param chip The chip to init os functions. Only pointer to the default chip is supported now.
 *
 * @return always ESP_OK
 */
esp_err_t esp_flash_app_enable_os_functions(esp_flash_t* chip);

/**
 *  Disable OS-level functions for the main flash chip during special phases (e.g. coredump)
 *
 * @param chip The chip to init os functions. Only "esp_flash_default_chip" is supported now.
 *
 * @return always ESP_OK
 */
esp_err_t esp_flash_app_disable_os_functions(esp_flash_t* chip);



#ifdef __cplusplus
}
#endif
