/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * System level MSPI APIs (private)
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

#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_err.h"
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/spi_flash.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/spi_flash.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/spi_flash.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/spi_flash.h"
#endif
#include "esp_flash.h"
#include "hal/spi_flash_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief To setup Flash chip
 */
esp_err_t spi_flash_init_chip_state(void);

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
 * @brief Initialize main flash
 * @param chip Pointer to main SPI flash(SPI1 CS0) chip to use..
 */
esp_err_t esp_flash_init_main(esp_flash_t *chip);

/**
 * @brief Should be only used by SPI1 Flash driver to know the necessary timing registers
 * @param out_timing_config Pointer to timing_tuning parameters.
 */
void spi_timing_get_flash_timing_param(spi_flash_hal_timing_config_t *out_timing_config);

/**
 * @brief Judge if the flash in tuned
 */
bool spi_timine_config_flash_is_tuned(void);

/**
 * @brief Set Flash chip specifically required MSPI register settings here
 */
void spi_flash_set_vendor_required_regs(void);


#ifdef __cplusplus
}
#endif
