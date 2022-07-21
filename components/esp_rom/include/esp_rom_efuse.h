/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_caps.h"

#define ESP_ROM_EFUSE_FLASH_DEFAULT_SPI  (0)
#define ESP_ROM_EFUSE_FLASH_DEFAULT_HSPI (1)

/**
 * @brief A CRC8 algorithm used for MAC addresses stored in eFuse
 *
 * @param data Pointer to the original data
 * @param len Data length in byte
 * @return uint8_t CRC value
 */
uint8_t esp_rom_efuse_mac_address_crc8(const uint8_t *data, uint32_t len);

/**
 * @brief Get SPI Flash GPIO pin configurations from eFuse
 *
 * @return uint32_t
 *          - 0: default SPI pins (ESP_ROM_EFUSE_FLASH_DEFAULT_SPI)
 *          - 1: default HSPI pins (ESP_ROM_EFUSE_FLASH_DEFAULT_HSPI)
 *          - Others: Customized pin configuration mask. Pins are encoded as per the
 *                    EFUSE_SPICONFIG_RET_SPICLK, EFUSE_SPICONFIG_RET_SPIQ, EFUSE_SPICONFIG_RET_SPID,
 *                    EFUSE_SPICONFIG_RET_SPICS0, EFUSE_SPICONFIG_RET_SPIHD macros.
 *
 * @note WP pin (for quad I/O modes) is not saved in eFuse and not returned by this function.
 */
uint32_t esp_rom_efuse_get_flash_gpio_info(void);

/**
 * @brief Get SPI Flash WP pin information from eFuse
 *
 * @return uint32_t
 *      - 0x3F: invalid GPIO number
 *      - 0~46: valid GPIO number
 */
uint32_t esp_rom_efuse_get_flash_wp_gpio(void);

#if SOC_SPI_MEM_SUPPORT_OPI_MODE
/**
 * @brief Read opi flash pads configuration from Efuse
 *
 * @return
 * - 0 for default SPI pins.
 * - Other values define a custom pin configuration mask. From the LSB, every 6 bits represent a GPIO number which stand for:
 *   DQS, D4, D5, D6, D7 accordingly.
 */
uint32_t esp_rom_efuse_get_opiconfig(void);
#endif // SOC_SPI_MEM_SUPPORT_OPI_MODE

/**
 * @brief Read eFuse to check whether secure boot has been enabled or not
 *
 * @return true if secure boot is enabled, otherwise false
 */
bool esp_rom_efuse_is_secure_boot_enabled(void);

#ifdef __cplusplus
}
#endif
