/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/*
 * Design of antenna soft switching : 2023.02.21
 *
 * version:v1.0
 */
#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ANT_TOTAL_TWO = 2,      /**< Two antennas participate in the switch */
    ANT_TOTAL_THREE,        /**< Three antennas participate in the switch */
    ANT_TOTAL_MAX
} ant_mun_t;
/**
  * @brief WiFi  antenna auto switch configuration
  *
  */
typedef struct {
    ant_mun_t   ant_num;            /**< WiFi use antenna numbers 2 or 3*/
    uint8_t     ant_zero;           /**< WiFi antenna zero select*/
    uint8_t     ant_one;            /**< WiFi antenna one select*/
    uint8_t     ant_two;            /**< WiFi antenna two select*/
    int16_t     ant_switch;        /**<  WiFi antenna auto switch sensitive*/
} wifi_antenna_auto_switch_config_t;

/**
  * @brief     Set antenna auto configuration
  *
  * @param     config  Antenna auto configuration.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_FAIL: error
  */
esp_err_t esp_wifi_set_ant_soft_switch(const wifi_antenna_auto_switch_config_t *config);

/**
  * @brief     Get antenna auto configuration
  *
  * @param     config  Antenna auto configuration.
  *
  * @return
  *    - ESP_OK: succeed
  */
esp_err_t esp_wifi_get_ant_soft_switch_config(wifi_antenna_auto_switch_config_t *config);

/**
  * @brief     Delete antenna soft switching task
  */
void esp_deinit_ant_soft_switch(void);

#ifdef __cplusplus
}
#endif
