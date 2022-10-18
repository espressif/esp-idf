/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <esp_err.h>
#include "spi_flash_mmap.h" /* including in bootloader for error values */
#include "esp_private/spi_flash_os.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "bootloader_flash_override.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Read flash ID by sending RDID command (0x9F)
 * @return flash raw ID
 *     mfg_id = (ID >> 16) & 0xFF;
       flash_id = ID & 0xffff;
 */
uint32_t bootloader_read_flash_id(void);

#if SOC_CACHE_SUPPORT_WRAP
/**
 * @brief Set the burst mode setting command for specified wrap mode.
 *
 * @param mode The specified warp mode.
 * @return always ESP_OK
 */
esp_err_t bootloader_flash_wrap_set(spi_flash_wrap_mode_t mode);
#endif

/**
 * @brief Startup flow recommended by XMC. Call at startup before any erase/write operation.
 *
 * @return ESP_OK When startup successfully, otherwise ESP_FAIL (indiciating you should reboot before erase/write).
 */
esp_err_t bootloader_flash_xmc_startup(void);

/**
  * @brief Unlock Flash write protect.
  *        Please do not call this function in SDK.
  *
  * @note This can be overridden because it's attribute weak.
  */
esp_err_t  __attribute__((weak)) bootloader_flash_unlock(void);

/**
 * @brief Reset the flash chip (66H + 99H).
 *
 * @return ESP_OK if success, otherwise ESP_FAIL.
 */
esp_err_t bootloader_flash_reset_chip(void);

/**
 * @brief Check if octal flash mode is enabled in eFuse
 *
 * @return True if flash is in octal mode, false else
 */
bool bootloader_flash_is_octal_mode_enabled(void);

#ifdef __cplusplus
}
#endif
