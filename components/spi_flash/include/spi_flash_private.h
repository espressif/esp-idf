/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


/**
 * Currently the MSPI timing tuning related APIs are designed to be private.
 * Because:
 * 1. now we don't split SPI0 and SPI1, we don't have a component for SPI0, including PSRAM, Cache, etc..
 * 2. SPI0 and SPI1 are strongly coupling.
 *
 * In the future, we may consider creating a component for SPI0, and spi_flash component will only work on SPI1 (and it
 * can rely on SPI0). Therefore, we can put these APIs there.
 *
 */
#pragma once

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/spi_flash.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/spi_flash.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/spi_flash.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/spi_flash.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This struct provide MSPI Flash necessary timing related config
 */
typedef struct {
    uint8_t flash_clk_div;      /*!< clock divider of Flash module. */
    uint8_t flash_extra_dummy;  /*!< timing required extra dummy length for Flash */
    bool    flash_setup_en;     /*!< SPI0/1 Flash setup enable or not */
    uint8_t flash_setup_time;   /*!< SPI0/1 Flash setup time. This value should be set to register directly */
    bool    flash_hold_en;      /*!< SPI0/1 Flash hold enable or not */
    uint8_t flash_hold_time;    /*!< SPI0/1 Flash hold time. This value should be set to register directly */
} spi_timing_flash_config_t;

/**
 * @brief Register ROM functions and init flash device registers to make use of octal flash
 */
esp_err_t esp_opiflash_init(void);

/**
 * @brief Make MSPI work under 20Mhz
 * @param control_spi1  Select whether to control SPI1. For tuning, we need to use SPI1. After tuning (during startup stage), let the flash driver to control SPI1
 */
void spi_timing_enter_mspi_low_speed_mode(bool control_spi1);

/**
 * @brief Make MSPI work under the frequency as users set
 * @param control_spi1  Select whether to control SPI1. For tuning, we need to use SPI1. After tuning (during startup stage), let the flash driver to control SPI1
 */
void spi_timing_enter_mspi_high_speed_mode(bool control_spi1);

/**
 * @brief Tune MSPI flash timing to make it work under high frequency
 */
void spi_timing_flash_tuning(void);

/**
 * @brief Tune MSPI psram timing to make it work under high frequency
 */
void spi_timing_psram_tuning(void);

/**
 * @brief To initislize the MSPI pins
 */
void esp_mspi_pin_init(void);

/**
 * @brief Set SPI1 registers to make ROM functions work
 * @note This function is used for setting SPI1 registers to the state that ROM SPI functions work
 */
void spi_flash_set_rom_required_regs(void);

/**
 * @brief Get MSPI Flash necessary timing related config
 * @param config see `spi_timing_flash_config_t`
 */
void spi_timing_get_flash_regs(spi_timing_flash_config_t *config);

#ifdef __cplusplus
}
#endif
