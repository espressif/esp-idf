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
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

// Type of MSPI IO
typedef enum {
    ESP_MSPI_IO_CLK = 0,
    ESP_MSPI_IO_Q,
    ESP_MSPI_IO_D,
    ESP_MSPI_IO_CS0, /* cs for spi flash */
    ESP_MSPI_IO_HD,
    ESP_MSPI_IO_WP,
#if SOC_SPI_MEM_SUPPORT_OPI_MODE
    ESP_MSPI_IO_DQS,
    ESP_MSPI_IO_D4,
    ESP_MSPI_IO_D5,
    ESP_MSPI_IO_D6,
    ESP_MSPI_IO_D7,
#endif // SOC_SPI_MEM_SUPPORT_OPI_MODE
#if CONFIG_SPIRAM
    ESP_MSPI_IO_CS1 /* cs for spi ram */
#endif
} esp_mspi_io_t;

/**
 * @brief To setup Flash chip
 */
esp_err_t spi_flash_init_chip_state(void);

/**
 * @brief Make MSPI work under 20Mhz, remove the timing tuning required delays.
 * @param control_spi1  Select whether to control SPI1. For tuning, we need to use SPI1. After tuning (during startup stage), let the flash driver to control SPI1
 */
void spi_timing_enter_mspi_low_speed_mode(bool control_spi1);

/**
 * @brief Make MSPI work under the frequency as users set, may add certain delays to MSPI RX direction to meet timing requirements.
 * @param control_spi1  Select whether to control SPI1. For tuning, we need to use SPI1. After tuning (during startup stage), let the flash driver to control SPI1
 */
void spi_timing_enter_mspi_high_speed_mode(bool control_spi1);

/**
 * @brief Switch MSPI into low speed mode / high speed mode.
 * @note This API is cache safe, it will freeze both D$ and I$ and restore them after MSPI is switched
 * @note For some of the MSPI high frequency settings (e.g. 80M DDR mode Flash or PSRAM), timing tuning is required.
 *       Certain delays will be added to the MSPI RX direction. When CPU clock switches from PLL to XTAL, should call
 *       this API first to enter MSPI low speed mode to remove the delays, and vice versa.
 */
void spi_timing_change_speed_mode_cache_safe(bool switch_down);

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
 * @brief Get the number of the GPIO corresponding to the given MSPI io
 *
 * @param[in] io  MSPI io
 *
 * @return MSPI IO number
 */
uint8_t esp_mspi_get_io(esp_mspi_io_t io);

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
 * @brief Get the knowledge if the MSPI timing is tuned or not
 */
bool spi_timing_is_tuned(void);

/**
 * @brief Set Flash chip specifically required MSPI register settings here
 */
void spi_flash_set_vendor_required_regs(void);


#ifdef __cplusplus
}
#endif
