/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_openthread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Publishes the OpenThread meshcop service in mDNS
 *
 * @param[in] instance_name Instance name of meshcop mDNS service
 *
 * @return
 *      - ESP_OK success
 *      - ESP_ERR_NO_MEM memory error
 *      - ESP_FAIL failed to add service
 *
 */
esp_err_t esp_openthread_publish_meshcop_mdns(const char *instance_name);

/**
 * @brief   Removes the OpenThread meshcop service in mDNS
 *
 * @return
 *      - ESP_OK success
 *      - ESP_ERR_NO_MEM memory error
 *      - ESP_FAIL failed to remove service
 *
 */
esp_err_t esp_openthread_remove_meshcop_mdns(void);

#ifdef __cplusplus
}
#endif
