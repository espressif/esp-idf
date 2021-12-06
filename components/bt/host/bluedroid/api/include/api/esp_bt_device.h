/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_BT_DEVICE_H__
#define __ESP_BT_DEVICE_H__

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_bt_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * @brief      Get bluetooth device address.  Must use after "esp_bluedroid_enable".
 *
 * @return     bluetooth device address (six bytes), or NULL if bluetooth stack is not enabled
 */
const uint8_t *esp_bt_dev_get_address(void);


/**
 * @brief           Set bluetooth device name. This function should be called after esp_bluedroid_enable()
 *                  completes successfully.
 *
 *                  A BR/EDR/LE device type shall have a single Bluetooth device name which shall be
 *                  identical irrespective of the physical channel used to perform the name discovery procedure.
 *
 * @param[in]       name : device name to be set
 *
 * @return
 *                  - ESP_OK : Succeed
 *                  - ESP_ERR_INVALID_ARG : if name is NULL pointer or empty, or string length out of limit
 *                  - ESP_ERR_INVALID_STATE : if bluetooth stack is not yet enabled
 *                  - ESP_FAIL : others
 */
esp_err_t esp_bt_dev_set_device_name(const char *name);

#ifdef __cplusplus
}
#endif


#endif /* __ESP_BT_DEVICE_H__ */
