/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_BT_MAIN_H__
#define __ESP_BT_MAIN_H__

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Bluetooth stack status type, to indicate whether the bluetooth stack is ready.
 */
typedef enum {
    ESP_BLUEDROID_STATUS_UNINITIALIZED   = 0,        /*!< Bluetooth not initialized */
    ESP_BLUEDROID_STATUS_INITIALIZED,                /*!< Bluetooth initialized but not enabled */
    ESP_BLUEDROID_STATUS_ENABLED                     /*!< Bluetooth initialized and enabled */
} esp_bluedroid_status_t;

/**
 * @brief     Get bluetooth stack status
 *
 * @return    Bluetooth stack status
 *
 */
esp_bluedroid_status_t esp_bluedroid_get_status(void);

/**
 * @brief     Enable bluetooth, must after esp_bluedroid_init().
 *
 * @return
 *            - ESP_OK : Succeed
 *            - Other  : Failed
 */
esp_err_t esp_bluedroid_enable(void);

/**
 * @brief     Disable bluetooth, must prior to esp_bluedroid_deinit().
 *
 * @return
 *            - ESP_OK : Succeed
 *            - Other  : Failed
 */
esp_err_t esp_bluedroid_disable(void);

/**
 * @brief     Init and alloc the resource for bluetooth, must be prior to every bluetooth stuff.
 *
 * @return
 *            - ESP_OK : Succeed
 *            - Other  : Failed
 */
esp_err_t esp_bluedroid_init(void);

/**
 * @brief     Deinit and free the resource for bluetooth, must be after every bluetooth stuff.
 *
 * @return
 *            - ESP_OK : Succeed
 *            - Other  : Failed
 */
esp_err_t esp_bluedroid_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_BT_MAIN_H__ */
