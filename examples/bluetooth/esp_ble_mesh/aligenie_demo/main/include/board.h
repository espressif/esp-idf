/*
 * AliGenie - Example
 *
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef _BOARD_H_
#define _BOARD_H_

#ifdef __cplusplus
extern "C" {
#endif /**< __cplusplus */

#include "driver/gpio.h"
#include "lightbulb.h"

#define LED_ON    1
#define LED_OFF   0

#define POWERUP_KEY  "powerup"

/**
 * @brief
 */
void board_init(void);

/**
 * @brief
 */
void board_led_hsl(uint8_t elem_index, uint16_t hue, uint16_t saturation, uint16_t lightness);

/**
 * @brief
 */
void board_led_temperature(uint8_t elem_index, uint16_t temperature);

/**
 * @brief
 */
void board_led_lightness(uint8_t elem_index, uint16_t actual);

/**
 * @brief
 */
void board_led_switch(uint8_t elem_index, uint8_t onoff);

/**
 * @brief
 */
uint16_t convert_lightness_actual_to_linear(uint16_t actual);

/**
 * @brief
 */
uint16_t convert_lightness_linear_to_actual(uint16_t linear);

/**
 * @brief
 */
int16_t convert_temperature_to_level(uint16_t temp, uint16_t min, uint16_t max);

/**
 * @brief
 */
uint16_t covert_level_to_temperature(int16_t level, uint16_t min, uint16_t max);

/**
 * @brief
 */
void swap_buf(uint8_t *dst, const uint8_t *src, int len);

/**
 * @brief
 */
uint8_t *mac_str2hex(const char *mac_str, uint8_t *mac_hex);

#ifdef __cplusplus
}
#endif /**< __cplusplus */

#endif /* _BOARD_H_ */
