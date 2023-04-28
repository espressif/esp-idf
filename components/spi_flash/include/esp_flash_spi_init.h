/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "hal/spi_types.h"
#include "esp_flash.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Configurations for the SPI Flash to init
typedef struct {
    spi_host_device_t host_id;      ///< Bus to use
    int cs_io_num;                  ///< GPIO pin to output the CS signal
    esp_flash_io_mode_t io_mode;    ///< IO mode to read from the Flash
    enum esp_flash_speed_s speed __attribute__((deprecated));        ///< Speed of the Flash clock. Replaced by freq_mhz
    int input_delay_ns;             ///< Input delay of the data pins, in ns. Set to 0 if unknown.
    /**
     * CS line ID, ignored when not `host_id` is not SPI1_HOST, or
     * `CONFIG_SPI_FLASH_SHARE_SPI1_BUS` is enabled. In this case, the CS line used is
     * automatically assigned by the SPI bus lock.
     */
    int cs_id;
    int freq_mhz;                   ///< The frequency of flash chip(MHZ)
} esp_flash_spi_device_config_t;

/**
 *  Add a SPI Flash device onto the SPI bus.
 *
 * The bus should be already initialized by ``spi_bus_initialization``.
 *
 * @param out_chip Pointer to hold the initialized chip.
 * @param config Configuration of the chips to initialize.
 *
 * @return
 *      - ESP_ERR_INVALID_ARG: out_chip is NULL, or some field in the config is invalid.
 *      - ESP_ERR_NO_MEM: failed to allocate memory for the chip structures.
 *      - ESP_OK: success.
 */
esp_err_t spi_bus_add_flash_device(esp_flash_t **out_chip, const esp_flash_spi_device_config_t *config);

/**
 *  Remove a SPI Flash device from the SPI bus.
 *
 * @param chip The flash device to remove.
 *
 * @return
 *      - ESP_ERR_INVALID_ARG: The chip is invalid.
 *      - ESP_OK: success.
 */
esp_err_t spi_bus_remove_flash_device(esp_flash_t *chip);

#ifdef __cplusplus
}
#endif
