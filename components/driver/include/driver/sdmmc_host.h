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

#include "soc/soc_caps.h"

#ifndef SOC_SDMMC_HOST_SUPPORTED
#error SDMMC host is not supported in this chip target
#endif

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
    .flags = SDMMC_HOST_FLAG_8BIT | \
             SDMMC_HOST_FLAG_4BIT | \
             SDMMC_HOST_FLAG_1BIT | \
             SDMMC_HOST_FLAG_DDR, \
    .slot = SDMMC_HOST_SLOT_1, \
    .max_freq_khz = SDMMC_FREQ_DEFAULT, \
    .io_voltage = 3.3f, \
    .init = &sdmmc_host_init, \
    .set_bus_width = &sdmmc_host_set_bus_width, \
    .get_bus_width = &sdmmc_host_get_slot_width, \
    .set_bus_ddr_mode = &sdmmc_host_set_bus_ddr_mode, \
    .set_card_clk = &sdmmc_host_set_card_clk, \
    .do_transaction = &sdmmc_host_do_transaction, \
    .deinit = &sdmmc_host_deinit, \
    .io_int_enable = sdmmc_host_io_int_enable, \
    .io_int_wait = sdmmc_host_io_int_wait, \
    .command_timeout_ms = 0, \
}

/**
 * Extra configuration for SDMMC peripheral slot
 */
typedef struct {
    gpio_num_t gpio_cd;     ///< GPIO number of card detect signal
    gpio_num_t gpio_wp;     ///< GPIO number of write protect signal
    uint8_t width;          ///< Bus width used by the slot (might be less than the max width supported)
    uint32_t flags;         ///< Features used by this slot
#define SDMMC_SLOT_FLAG_INTERNAL_PULLUP  BIT(0)
        /**< Enable internal pullups on enabled pins. The internal pullups
         are insufficient however, please make sure external pullups are
         connected on the bus. This is for debug / example purpose only.
         */
} sdmmc_slot_config_t;

#define SDMMC_SLOT_NO_CD      GPIO_NUM_NC     ///< indicates that card detect line is not used
#define SDMMC_SLOT_NO_WP      GPIO_NUM_NC     ///< indicates that write protect line is not used
#define SDMMC_SLOT_WIDTH_DEFAULT 0 ///< use the default width for the slot (8 for slot 0, 4 for slot 1)

/**
 * Macro defining default configuration of SDMMC host slot
 */
#define SDMMC_SLOT_CONFIG_DEFAULT() {\
    .gpio_cd = SDMMC_SLOT_NO_CD, \
    .gpio_wp = SDMMC_SLOT_NO_WP, \
    .width   = SDMMC_SLOT_WIDTH_DEFAULT, \
    .flags = 0, \
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
esp_err_t sdmmc_host_init(void);

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
 * @brief Get bus width configured in ``sdmmc_host_init_slot`` to be used for data transfer
 *
 * @param slot  slot number (SDMMC_HOST_SLOT_0 or SDMMC_HOST_SLOT_1)
 * @return configured bus width of the specified slot.
 */
size_t sdmmc_host_get_slot_width(int slot);

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
 * @brief Enable or disable DDR mode of SD interface
 * @param slot  slot number (SDMMC_HOST_SLOT_0 or SDMMC_HOST_SLOT_1)
 * @param ddr_enabled  enable or disable DDR mode
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NOT_SUPPORTED if DDR mode is not supported on this slot
 */
esp_err_t sdmmc_host_set_bus_ddr_mode(int slot, bool ddr_enabled);

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
 * @attention Data buffer passed in cmdinfo->data must be in DMA capable memory
 *
 * @param slot  slot number (SDMMC_HOST_SLOT_0 or SDMMC_HOST_SLOT_1)
 * @param cmdinfo   pointer to structure describing command and data to transfer
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_TIMEOUT if response or data transfer has timed out
 *      - ESP_ERR_INVALID_CRC if response or data transfer CRC check has failed
 *      - ESP_ERR_INVALID_RESPONSE if the card has sent an invalid response
 *      - ESP_ERR_INVALID_SIZE if the size of data transfer is not valid in SD protocol
 *      - ESP_ERR_INVALID_ARG if the data buffer is not in DMA capable memory
 */
esp_err_t sdmmc_host_do_transaction(int slot, sdmmc_command_t* cmdinfo);

/**
 * @brief Enable IO interrupts
 *
 * This function configures the host to accept SDIO interrupts.
 *
 * @param slot  slot number (SDMMC_HOST_SLOT_0 or SDMMC_HOST_SLOT_1)
 * @return returns ESP_OK, other errors possible in the future
 */
esp_err_t sdmmc_host_io_int_enable(int slot);

/**
 * @brief Block until an SDIO interrupt is received, or timeout occurs
 * @param slot  slot number (SDMMC_HOST_SLOT_0 or SDMMC_HOST_SLOT_1)
 * @param timeout_ticks  number of RTOS ticks to wait for the interrupt
 * @return
 *  - ESP_OK on success (interrupt received)
 *  - ESP_ERR_TIMEOUT if the interrupt did not occur within timeout_ticks
 */
esp_err_t sdmmc_host_io_int_wait(int slot, TickType_t timeout_ticks);

/**
 * @brief Disable SDMMC host and release allocated resources
 *
 * @note This function is not thread safe
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if sdmmc_host_init function has not been called
 */
esp_err_t sdmmc_host_deinit(void);

/**
 * @brief Enable the pull-ups of sd pins.
 *
 * @note You should always place actual pullups on the lines instead of using
 * this function. Internal pullup resistance are high and not sufficient, may
 * cause instability in products. This is for debug or examples only.
 *
 * @param slot Slot to use, normally set it to 1.
 * @param width Bit width of your configuration, 1 or 4.
 *
 * @return
 *      - ESP_OK: if success
 *      - ESP_ERR_INVALID_ARG: if configured width larger than maximum the slot can
 *              support
 */
esp_err_t sdmmc_host_pullup_en(int slot, int width);

#ifdef __cplusplus
}
#endif
