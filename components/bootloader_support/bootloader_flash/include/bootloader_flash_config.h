/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "esp_image_format.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Update the flash id in g_rom_flashchip(global esp_rom_spiflash_chip_t structure).
 *
 * @return None
 */
void bootloader_flash_update_id(void);

/**
 * @brief Update the flash size in g_rom_flashchip (global esp_rom_spiflash_chip_t structure).
 *
 * @param size The size to store, in bytes.
 * @return None
 */
void bootloader_flash_update_size(uint32_t size);

/**
 * @brief Set the flash CS setup and hold time.
 *
 * @note CS setup time is recomemded to be 1.5T, and CS hold time is recommended to be 2.5T.
 *       cs_setup = 1, cs_setup_time = 0; cs_hold = 1, cs_hold_time = 1.
 *
 * @return None
 */
void bootloader_flash_cs_timing_config(void);

/**
 * @brief Configure SPI flash clock.
 *
 * @note This function only set clock frequency for SPI0.
 *
 * @param pfhdr Pointer to App image header, from where to fetch flash settings.
 *
 * @return None
 */
void bootloader_flash_clock_config(const esp_image_header_t* pfhdr);

/**
 * @brief Configure SPI flash gpio, include the IO matrix and drive strength configuration.
 *
 * @param pfhdr Pointer to App image header, from where to fetch flash settings.
 *
 * @return None
 */
void bootloader_flash_gpio_config(const esp_image_header_t* pfhdr);

#ifdef CONFIG_IDF_TARGET_ESP32
/**
 * @brief Configure SPI flash read dummy based on different mode and frequency.
 *
 * @param pfhdr Pointer to App image header, from where to fetch flash settings.
 *
 * @return None
 */
void bootloader_flash_dummy_config(const esp_image_header_t* pfhdr);
#else
// The meaning has changed on this chip. Deprecated, Call `bootloader_configure_spi_pins()` and `bootloader_flash_set_dummy_out()` directly.
void bootloader_flash_dummy_config(const esp_image_header_t* pfhdr) __attribute__((deprecated));
#endif

#ifdef CONFIG_IDF_TARGET_ESP32
/**
 * @brief Return the pin number used for custom SPI flash and/or SPIRAM WP pin
 *
 * Can be determined by eFuse values in most cases, or overriden in configuration
 *
 * This value is only meaningful if the other SPI flash pins are overriden via eFuse.
 *
 * This value is only meaningful if flash is set to QIO or QOUT mode, or if
 * SPIRAM is enabled.
 *
 * @return Pin number to use, or -1 if the default should be kept
 */
int bootloader_flash_get_wp_pin(void);
#endif

#ifdef __cplusplus
}
#endif
