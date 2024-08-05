/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_BT_MAIN_H__
#define __ESP_BT_MAIN_H__

#include <stdbool.h>
#include <stdint.h>

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
 * @brief Bluetooth stack configuration
 */
typedef struct {
    bool ssp_en; /*!< Whether SSP(secure simple pairing) or legacy pairing is used for Classic Bluetooth */
} esp_bluedroid_config_t;

#define BT_BLUEDROID_INIT_CONFIG_DEFAULT()                                                                             \
    {                                                                                                                  \
        .ssp_en = true,                                                                                                \
    }

/**
 * @brief     Get bluetooth stack status
 *
 * @return    Bluetooth stack status
 *
 */
esp_bluedroid_status_t esp_bluedroid_get_status(void);

/**
 * @brief     Enable bluetooth, must after esp_bluedroid_init()/esp_bluedroid_init_with_cfg().
 *
 * @return
 *            - ESP_OK : Succeed
 *            - Other  : Failed
 */
esp_err_t esp_bluedroid_enable(void);

/**
 * @brief     Disable Bluetooth, must be called prior to esp_bluedroid_deinit().
 *
 * @note      Before calling this API, ensure that all activities related to
 *            the application, such as connections, scans, etc., are properly closed.
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
 * @brief     Init and alloc the resource for bluetooth, must be prior to every bluetooth stuff.
 *
 * @param cfg Initial configuration of ESP Bluedroid stack.
 *
 * @return
 *            - ESP_OK : Succeed
 *            - Other  : Failed
 */
esp_err_t esp_bluedroid_init_with_cfg(esp_bluedroid_config_t *cfg);

/**
 * @brief     Deinit and free the resource for bluetooth, must be after every bluetooth stuff.
 *
 * @return
 *            - ESP_OK : Succeed
 *            - Other  : Failed
 */
esp_err_t esp_bluedroid_deinit(void);

#if defined(CONFIG_EXAMPLE_CI_ID) && defined(CONFIG_EXAMPLE_CI_PIPELINE_ID)
// Only for internal used (CI example test)
char *esp_bluedroid_get_example_name(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __ESP_BT_MAIN_H__ */
