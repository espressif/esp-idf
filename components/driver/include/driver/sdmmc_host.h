// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"
#include "sdmmc_types.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SDMMC_HOST_SLOT_0     0     ///< SDMMC slot 0
#define SDMMC_HOST_SLOT_1     1     ///< SDMMC slot 1

/**
 * @brief Default sdmmc_host_t structure initializer for SDMMC peripheral
 *
 * Uses SDMMC peripheral, with 4-bit mode enabled, and max frequency set to 20MHz
 */
#define SDMMC_HOST_DEFAULT() {\
    .flags = SDMMC_HOST_FLAG_4BIT, \
    .slot = SDMMC_HOST_SLOT_1, \
    .max_freq_khz = SDMMC_FREQ_DEFAULT, \
    .io_voltage = 3.3f, \
    .init = &sdmmc_host_init, \
    .set_bus_width = &sdmmc_host_set_bus_width, \
    .set_card_clk = &sdmmc_host_set_card_clk, \
    .do_transaction = &sdmmc_host_do_transaction, \
    .deinit = &sdmmc_host_deinit, \
}

/**
 * Extra configuration for SDMMC peripheral slot
 */
typedef struct {
    gpio_num_t gpio_cd;     ///< GPIO number of card detect signal
    gpio_num_t gpio_wp;     ///< GPIO number of write protect signal
} sdmmc_slot_config_t;

#define SDMMC_SLOT_NO_CD      ((gpio_num_t) -1)     ///< indicates that card detect line is not used
#define SDMMC_SLOT_NO_WP      ((gpio_num_t) -1)     ///< indicates that write protect line is not used

/**
 * Macro defining default configuration of SDMMC host slot
 */
#define SDMMC_SLOT_CONFIG_DEFAULT() {\
    .gpio_cd = SDMMC_SLOT_NO_CD, \
    .gpio_wp = SDMMC_SLOT_NO_WP, \
}

/**
 * @brief Initialize SDMMC host peripheral
 *
 * @note This function is not thread safe
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if sdmmc_host_init was already called
 *      - ESP_ERR_NO_MEM if memory can not be allocated
 */
esp_err_t sdmmc_host_init();

/**
 * @brief Initialize given slot of SDMMC peripheral
 *
 * On the ESP32, SDMMC peripheral has two slots:
 *  - Slot 0: 8-bit wide, maps to HS1_* signals in PIN MUX
 *  - Slot 1: 4-bit wide, maps to HS2_* signals in PIN MUX
 *
 * Card detect and write protect signals can be routed to
 * arbitrary GPIOs using GPIO matrix.
 *
 * @note This function is not thread safe
 *
 * @param slot  slot number (SDMMC_HOST_SLOT_0 or SDMMC_HOST_SLOT_1)
 * @param slot_config  additional configuration for the slot
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if host has not been initialized using sdmmc_host_init
 */
esp_err_t sdmmc_host_init_slot(int slot, const sdmmc_slot_config_t* slot_config);

/**
 * @brief Select bus width to be used for data transfer
 *
 * SD/MMC card must be initialized prior to this command, and a command to set
 * bus width has to be sent to the card (e.g. SD_APP_SET_BUS_WIDTH)
 *
 * @note This function is not thread safe
 *
 * @param slot  slot number (SDMMC_HOST_SLOT_0 or SDMMC_HOST_SLOT_1)
 * @param width  bus width (1, 4, or 8 for slot 0; 1 or 4 for slot 1)
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if slot number or width is not valid
 */
esp_err_t sdmmc_host_set_bus_width(int slot, size_t width);

/**
 * @brief Set card clock frequency
 *
 * Currently only integer fractions of 40MHz clock can be used.
 * For High Speed cards, 40MHz can be used.
 * For Default Speed cards, 20MHz can be used.
 *
 * @note This function is not thread safe
 *
 * @param slot  slot number (SDMMC_HOST_SLOT_0 or SDMMC_HOST_SLOT_1)
 * @param freq_khz  card clock frequency, in kHz
 * @return
 *      - ESP_OK on success
 *      - other error codes may be returned in the future
 */
esp_err_t sdmmc_host_set_card_clk(int slot, uint32_t freq_khz);

/**
 * @brief Send command to the card and get response
 *
 * This function returns when command is sent and response is received,
 * or data is transferred, or timeout occurs.
 *
 * @note This function is not thread safe w.r.t. init/deinit functions,
 *       and bus width/clock speed configuration functions. Multiple tasks
 *       can call sdmmc_host_do_transaction as long as other sdmmc_host_*
 *       functions are not called.
 *
 * @param slot  slot number (SDMMC_HOST_SLOT_0 or SDMMC_HOST_SLOT_1)
 * @param cmdinfo   pointer to structure describing command and data to transfer
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_TIMEOUT if response or data transfer has timed out
 *      - ESP_ERR_INVALID_CRC if response or data transfer CRC check has failed
 *      - ESP_ERR_INVALID_RESPONSE if the card has sent an invalid response
 */
esp_err_t sdmmc_host_do_transaction(int slot, sdmmc_command_t* cmdinfo);

/**
 * @brief Disable SDMMC host and release allocated resources
 *
 * @note This function is not thread safe
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if sdmmc_host_init function has not been called
 */
esp_err_t sdmmc_host_deinit();

#ifdef __cplusplus
}
#endif
