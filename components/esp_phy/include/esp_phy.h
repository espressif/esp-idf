/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 *          PHY API
 */

/**
 * @brief PHY antenna
 */
typedef enum {
    ESP_PHY_ANT_ANT0,          /*!< PHY antenna 0 */
    ESP_PHY_ANT_ANT1,          /*!< PHY antenna 1 */
    ESP_PHY_ANT_MAX,           /*!< Invalid PHY antenna */
} esp_phy_ant_t;

/**
 * @brief PHY antenna mode
 */
typedef enum {
    ESP_PHY_ANT_MODE_ANT0,          /*!< Enable PHY antenna 0 only */
    ESP_PHY_ANT_MODE_ANT1,          /*!< Enable PHY antenna 1 only */
    ESP_PHY_ANT_MODE_AUTO,          /*!< Enable PHY antenna 0 and 1, automatically select an antenna, suggest not use this mode */
    ESP_PHY_ANT_MODE_MAX,           /*!< Invalid PHY enabled antenna */
} esp_phy_ant_mode_t;

/**
 * @brief PHY GPIO configuration for antenna selection
 */
typedef struct {
    uint8_t gpio_select: 1,           /*!< Whether this GPIO is connected to external antenna switch */
            gpio_num:    7;           /*!< The GPIO number that connects to external antenna switch */
} esp_phy_ant_gpio_t;

/**
 * @brief PHY GPIOs configuration for antenna selection
 */
typedef struct {
    esp_phy_ant_gpio_t  gpio_cfg[4];  /*!< The configurations of GPIOs that connect to external antenna switch */
} esp_phy_ant_gpio_config_t;

/**
 * @brief PHY antenna configuration
 */
typedef struct {
    esp_phy_ant_mode_t rx_ant_mode;          /*!< PHY antenna mode for receiving */
    esp_phy_ant_t      rx_ant_default;       /*!< Default antenna mode for receiving, it's ignored if rx_ant_mode is not ESP_PHY_ANT_MODE_AUTO */
    esp_phy_ant_mode_t tx_ant_mode;          /*!< PHY antenna mode for transmission, it can be set to ESP_PHY_ANT_MODE_ANT1 or ESP_PHY_ANT_MODE_ANT0 */
    uint8_t           enabled_ant0: 4,      /*!< Index (in antenna GPIO configuration) of enabled ESP_PHY_ANT_MODE_ANT0 */
                      enabled_ant1: 4;      /*!< Index (in antenna GPIO configuration) of enabled ESP_PHY_ANT_MODE_ANT1 */
} esp_phy_ant_config_t;

/**
 * @brief Set antenna GPIO configuration
 *
 * @param config : Antenna GPIO configuration.
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 */
esp_err_t esp_phy_set_ant_gpio(esp_phy_ant_gpio_config_t *config);

/**
 * @brief Get current antenna GPIO configuration
 *
 * @param config : Antenna GPIO configuration.
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 */
esp_err_t esp_phy_get_ant_gpio(esp_phy_ant_gpio_config_t *config);

/**
 * @brief Set antenna configuration
 *
 * @param config : Antenna configuration.
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 */
esp_err_t esp_phy_set_ant(esp_phy_ant_config_t *config);

/**
 * @brief Get current antenna configuration
 *
 * @param config : Antenna configuration.
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 */
esp_err_t esp_phy_get_ant(esp_phy_ant_config_t *config);

#ifdef __cplusplus
}
#endif
