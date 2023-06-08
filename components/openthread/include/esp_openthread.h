/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_openthread_types.h"
#include "openthread/dataset.h"
#include "openthread/error.h"
#include "openthread/instance.h"
#include "lwip/ip_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initializes the full OpenThread stack.
 *
 * @note The OpenThread instance will also be initialized in this function.
 *
 * @param[in]  init_config      The initialization configuration.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM if allocation has failed
 *      - ESP_ERR_INVALID_ARG if radio or host connection mode not supported
 *      - ESP_ERR_INVALID_STATE if already initialized
 *
 */
esp_err_t esp_openthread_init(const esp_openthread_platform_config_t *init_config);

/**
 * @brief   Starts the Thread protocol operation and attaches to a Thread network.
 *
 * @param[in]  datasetTlvs    The operational dataset (TLV encoded), if it's NULL, the function will generate the dataset
 *                            based on the configurations from kconfig.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failures
 *
 */
esp_err_t esp_openthread_auto_start(otOperationalDatasetTlvs *datasetTlvs);

/**
 * @brief   Launches the OpenThread main loop.
 *
 * @note This function will not return unless error happens when running the OpenThread stack.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM if allocation has failed
 *      - ESP_FAIL on other failures
 *
 */
esp_err_t esp_openthread_launch_mainloop(void);

/**
 * @brief This function performs OpenThread stack and platform driver deinitialization.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if not initialized
 *
 */
esp_err_t esp_openthread_deinit(void);

/**
 * @brief This function acquires the underlying OpenThread instance.
 *
 * @note This function can be called on other tasks without lock.
 *
 * @return The OpenThread instance pointer
 *
 */
otInstance *esp_openthread_get_instance(void);


#ifdef __cplusplus
} // end of extern "C"
#endif
