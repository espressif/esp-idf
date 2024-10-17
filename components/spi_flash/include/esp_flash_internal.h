/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "esp_err.h"
#include <stdint.h>
#include <stdbool.h>
#include "esp_private/spi_share_hw_ctrl.h"
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
esp_err_t esp_flash_init_default_chip(void);

/**
 *  Enable OS-level SPI flash protections in IDF
 *
 *  Called by OS startup code. You do not need to call this in your own applications.
 *
 * @return ESP_OK if success, otherwise failed. See return value of ``esp_flash_init_os_functions``.
 */
esp_err_t esp_flash_app_init(void);

/**
 *  Disable (or enable) OS-level SPI flash protections in IDF
 *
 *  Called by the IDF internal code (e.g. coredump). You do not need to call this in your own applications.
 *
 * @return always ESP_OK.
 */
esp_err_t esp_flash_app_disable_protect(bool disable);

/**
 *  Initialize OS-level functions for a specific chip.
 *
 * @param chip The chip to init os functions.
 * @param host_id Which SPI host to use, 1 for SPI1, 2 for SPI2 (HSPI), 3 for SPI3 (VSPI)
 * @param dev_handle SPI bus lock device handle to acquire during flash operations
 *
 * @return
 *      - ESP_OK if success
 *      - ESP_ERR_INVALID_ARG if host_id is invalid
 */
esp_err_t esp_flash_init_os_functions(esp_flash_t *chip, int host_id, spi_bus_lock_dev_handle_t dev_handle);

/**
 * @brief Deinitialize OS-level functions
 *
 * @param chip              The chip to deinit os functions
 * @param out_dev_handle    The SPI bus lock passed from `esp_flash_init_os_functions`. The caller should deinitialize
 *                          the lock.
 * @return always ESP_OK.
 */
esp_err_t esp_flash_deinit_os_functions(esp_flash_t* chip, spi_bus_lock_dev_handle_t* out_dev_handle);

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

/**
 * @brief Set or clear dangerous write protection check on the flash chip.
 *
 * This function sets the runtime option to allow or disallow writing to
 * dangerous areas such as the bootloader and partition table. If
 * CONFIG_SPI_FLASH_DANGEROUS_WRITE_ALLOWED is not set, this function allows
 * the caller to toggle the protection for specific areas.
 *
 * If CONFIG_SPI_FLASH_DANGEROUS_WRITE_ALLOWED is set, there is no protection
 * check in the system, and this function does nothing.
 *
 * @param chip The flash chip on which to set the write protection. Only
 *             "esp_flash_default_chip" is supported.
 * @param protect Set to true to enable protection against writing in dangerous
 *                areas (bootloader, partition table). Set to false to disable
 *                the protection.
 * @return
 *         - ESP_OK: Successful operation.
 *         - ESP_ERR_INVALID_ARG: The chip argument is null.
 */
esp_err_t esp_flash_set_dangerous_write_protection(esp_flash_t *chip, const bool protect);

#ifdef __cplusplus
}
#endif
