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

#include "hal/spi_types.h"
#include "esp_flash.h"

/// Configurations for the SPI Flash to init
typedef struct {
    spi_host_device_t host_id;      ///< Bus to use
    int cs_id;                      ///< CS pin (signal) to use
    int cs_io_num;                  ///< GPIO pin to output the CS signal
    esp_flash_io_mode_t io_mode;  ///< IO mode to read from the Flash
    esp_flash_speed_t speed;        ///< Speed of the Flash clock
    int input_delay_ns;             ///< Input delay of the data pins, in ns. Set to 0 if unknown.
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

