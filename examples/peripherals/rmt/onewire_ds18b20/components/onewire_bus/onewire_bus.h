/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_err.h"
#include "onewire_bus_rmt.h"

#define ONEWIRE_CMD_SEARCH_ROM 0xF0
#define ONEWIRE_CMD_READ_ROM 0x33
#define ONEWIRE_CMD_MATCH_ROM 0x55
#define ONEWIRE_CMD_SKIP_ROM 0xCC
#define ONEWIRE_CMD_ALARM_SEARCH_ROM 0xEC

#define ONEWIRE_ROM_ID(id) (id)[0], (id)[1], (id)[2], (id)[3], (id)[4], (id)[5], (id)[6], (id)[7]
#define ONEWIRE_ROM_ID_STR "%02X%02X%02X%02X%02X%02X%02X%02X"

/**
 * @brief Type of 1-wire ROM search algorithm context
 *
 */
typedef struct onewire_rom_search_context_t *onewire_rom_search_context_handler_t;

/**
 * @brief Calculate Dallas CRC value of given buffer
 *
 * @param[in] input Input buffer to calculate CRC value
 * @param[in] input_size Size of input buffer
 * @return CRC result of input buffer
 */
uint8_t onewire_check_crc8(uint8_t *input, size_t input_size);

/**
 * @brief Create context for 1-wire ROM search algorithm
 *
 * @param[in] handle 1-wire handle used for ROM search
 * @param[out] context_out Created context for ROM search algorithm
 * @return
 *         - ESP_OK                1-wire ROM search context is created successfully.
 *         - ESP_ERR_INVALID_ARG   Invalid argument.
 */
esp_err_t onewire_rom_search_context_create(onewire_bus_handle_t handle, onewire_rom_search_context_handler_t *context_out);

/**
 * @brief Delete context for 1-wire ROM search algorithm
 *
 * @param[in] context Context for ROM search algorithm
 * @return
 *         - ESP_OK                1-wire ROM search context is deleted successfully.
 *         - ESP_ERR_INVALID_ARG   Invalid argument.
 */
esp_err_t onewire_rom_search_context_delete(onewire_rom_search_context_handler_t context);

/**
 * @brief Search next device on 1-wire bus
 *
 * @param[in] context Context for ROM search algorithm
 * @return
 *         - ESP_OK                Successfully found a device
 *         - ESP_ERR_NOT_FOUND     There are no device on the bus
 *         - ESP_ERR_INVALID_CRC   Bad CRC value of found device
 *         - ESP_FAIL              Reached last device on the bus, search algorighm finishes
 */
esp_err_t onewire_rom_search(onewire_rom_search_context_handler_t context);

/**
 * @brief Get device ROM number from ROM search context
 *
 * @param[in] context Context for ROM search algorithm
 * @param[out] rom_number_out Device ROM number
 * @return
 *         - ESP_OK                Get ROM numbuer from 1-wire ROM search context success.
 *         - ESP_ERR_INVALID_ARG   Invalid argument.
 */
esp_err_t onewire_rom_get_number(onewire_rom_search_context_handler_t context, uint8_t *rom_number_out);
