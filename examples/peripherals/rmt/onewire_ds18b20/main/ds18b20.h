/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#include "onewire_bus.h"

#define DS18B20_CMD_CONVERT_TEMP 0x44
#define DS18B20_CMD_WRITE_SCRATCHPAD 0x4E
#define DS18B20_CMD_READ_SCRATCHPAD 0xBE

/**
 * @brief Structure of DS18B20's scratchpad
 *
 */
typedef struct  {
    uint8_t temp_lsb; /*!< lsb of temperature */
    uint8_t temp_msb; /*!< msb of temperature */
    uint8_t th_user1; /*!< th register or user byte 1 */
    uint8_t tl_user2; /*!< tl register or user byte 2 */
    uint8_t configuration; /*!< configuration register */
    uint8_t _reserved1;
    uint8_t _reserved2;
    uint8_t _reserved3;
    uint8_t crc_value; /*!< crc value of scratchpad data */
} ds18b20_scratchpad_t;

/**
 * @brief Enumeration of DS18B20's resolution config
 *
 */
typedef enum {
    DS18B20_RESOLUTION_12B = 0x7F, /*!< 750ms convert time */
    DS18B20_RESOLUTION_11B = 0x5F, /*!< 375ms convert time */
    DS18B20_RESOLUTION_10B = 0x3F, /*!< 187.5ms convert time */
    DS18B20_RESOLUTION_9B = 0x1F, /*!< 93.75ms convert time */
} ds18b20_resolution_t;

/**
 * @brief Trigger temperature conversion of DS18B20
 *
 * @param[in] handle 1-wire handle with DS18B20 on
 * @param[in] rom_number ROM number to specify which DS18B20 to send command, NULL to skip ROM
 * @return
 *         - ESP_OK                Trigger tempreture convertsion success.
 *         - ESP_ERR_INVALID_ARG   Invalid argument.
 *         - ESP_ERR_NOT_FOUND     There is no device present on 1-wire bus.
 */
esp_err_t ds18b20_trigger_temperature_conversion(onewire_bus_handle_t handle, const uint8_t *rom_number);

/**
 * @brief Get temperature from DS18B20
 *
 * @param[in] handle 1-wire handle with DS18B20 on
 * @param[in] rom_number ROM number to specify which DS18B20 to read from, NULL to skip ROM
 * @param[out] temperature result from DS18B20
 * @return
 *         - ESP_OK                Get tempreture from DS18B20 success.
 *         - ESP_ERR_INVALID_ARG   Invalid argument.
 *         - ESP_ERR_NOT_FOUND     There is no device present on 1-wire bus.
 *         - ESP_ERR_INVALID_CRC   CRC check failed.
 */
esp_err_t ds18b20_get_temperature(onewire_bus_handle_t handle, const uint8_t *rom_number, float *temperature);

/**
 * @brief Set DS18B20's temperation conversion resolution
 *
 * @param[in] handle 1-wire handle with DS18B20 on
 * @param[in] rom_number ROM number to specify which DS18B20 to read from, NULL to skip ROM
 * @param[in] resolution resolution of DS18B20's temperation conversion
 * @return
 *         - ESP_OK                Set DS18B20 resolution success.
 *         - ESP_ERR_INVALID_ARG   Invalid argument.
 *         - ESP_ERR_NOT_FOUND     There is no device present on 1-wire bus.
 */
esp_err_t ds18b20_set_resolution(onewire_bus_handle_t handle, const uint8_t *rom_number, ds18b20_resolution_t resolution);
