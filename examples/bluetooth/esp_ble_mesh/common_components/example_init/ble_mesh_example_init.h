/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_MESH_EXAMPLE_INIT_H_
#define _BLE_MESH_EXAMPLE_INIT_H_

#ifdef __cplusplus
extern "C" {
#endif /**< __cplusplus */

#include "esp_err.h"

void ble_mesh_get_dev_uuid(uint8_t *dev_uuid);

esp_err_t bluetooth_init(void);

#ifdef __cplusplus
}
#endif /**< __cplusplus */

#endif /* _BLE_MESH_EXAMPLE_INIT_H_ */
