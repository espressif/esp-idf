/* board.h - Board-specific hooks */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BOARD_H_
#define _BOARD_H_

#ifdef __cplusplus
extern "C" {
#endif /**< __cplusplus */

#include "driver/gpio.h"

#if defined(CONFIG_BLE_MESH_ESP_WROOM_32)
#define LED_R GPIO_NUM_25
#define LED_G GPIO_NUM_26
#define LED_B GPIO_NUM_27
#elif defined(CONFIG_BLE_MESH_ESP_WROVER)
#define LED_R GPIO_NUM_0
#define LED_G GPIO_NUM_2
#define LED_B GPIO_NUM_4
#elif defined(CONFIG_BLE_MESH_ESP32C3_DEV)
#define BLE_MESH_LED_STRIP_IO GPIO_NUM_8
#elif defined(CONFIG_BLE_MESH_ESP32S3_DEV)
#define BLE_MESH_LED_STRIP_IO GPIO_NUM_47
#elif defined(CONFIG_BLE_MESH_ESP32C6_DEV)
#define BLE_MESH_LED_STRIP_IO GPIO_NUM_8
#elif defined(CONFIG_BLE_MESH_ESP32H2_DEV)
#define BLE_MESH_LED_STRIP_IO GPIO_NUM_8
#elif defined(CONFIG_BLE_MESH_ESP32C5_DEV)
#define BLE_MESH_LED_STRIP_IO GPIO_NUM_8
#endif

#define BUTTON_IO_NUM           GPIO_NUM_9

#ifndef BLE_MESH_LED_STRIP_IO
#define LED_ON  1
#else
#define LED_R 0
#define LED_G 1
#define LED_B 2
#define LED_ON  100
#endif

#define LED_OFF 0

void board_led_operation(uint8_t r, uint8_t g, uint8_t b);

void board_init(void);

#ifdef __cplusplus
}
#endif /**< __cplusplus */

#endif /* _BOARD_H_ */
