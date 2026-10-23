/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
*
* SPDX-License-Identifier: Apache-2.0
*/

#pragma once

#include "sdkconfig.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// @brief HCI VS Commands for Espressif's Bluetooth Host
//
// @note The following vendor-specific HCI commands are exclusively for Espressif's Bluetooth Host (ESP-Bluedroid Host or ESP-NimBLE Host).
//       If you are using a non-ESP host or HCI UART, these commands will remain disabled unless enabling the feature in the menuconfig (CONFIG_SW_COEXIST_ENABLE).
//       Note, these additional HCI VS commands are intended for Espressif's Bluetooth Host use only.
//       Application developers **should not** enable these commands in their applications.
//

/**
 * @brief Set or clear bluetooth status to coexistence module (OCF: 0x0082)
 *
 */
#define ESP_BT_VS_SET_COEX_STATUS_OCF                               (0x0082)

/**
* @brief Set/Clear coexistence status command parameters
*/
struct bt_hci_vs_set_coex_status {
    uint8_t type;    /*!< type of wireless protocol, BLE or BT Classic */
    uint8_t op;      /*!< operation code, to set or clear coex status */
    uint8_t status;  /*!< coexistence status */
};

#ifdef __cplusplus
}
#endif
