/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_rom_caps.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

//executables are placed on flash
#define MMAP_EXECUTABLES_FROM_FLASH (!((CONFIG_SPIRAM_FETCH_INSTRUCTIONS && CONFIG_SPIRAM_RODATA) || CONFIG_APP_BUILD_TYPE_RAM))
#define MMAP_ROM_IMPL_ENABLED   (CONFIG_SPI_FLASH_ROM_IMPL && ESP_ROM_HAS_SPI_FLASH_MMAP)

#if !CONFIG_IDF_TARGET_ESP32
//No mmap lock for ESP32, since it's useless to block while can't read from flash with SPI0 (cache) and SPI1 (driver) at the same time.

/**
 * Initialize the internal lock earlier to bypass the memory leak check.
 */
esp_err_t flash_mmap_lock_init(void);

/**
 *  Freeze the mmap table
 */
void flash_mmap_lock_freeze(void);

/**
 * Unfreeze the mmap table
 */
void flash_mmap_lock_unfreeze(void);

/**
 * Return whether there is remaining mmap regions, must be called between flash_mmap_lock_freeze() and flash_mmap_lock_unfreeze().
 */
bool flash_mmap_remain(void);

#endif //!CONFIG_IDF_TARGET_ESP32

#ifdef __cplusplus
}
#endif
