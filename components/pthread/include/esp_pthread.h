/*
 * SPDX-FileCopyrightText: 2018-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "freertos/FreeRTOSConfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN    CONFIG_PTHREAD_STACK_MIN
#endif

/** pthread configuration structure that influences pthread creation */
typedef struct {
    size_t stack_size;          /**< The stack size of the pthread */
    size_t prio;                /**< The thread's priority */
    bool inherit_cfg;           /**< Inherit this configuration further */
    const char* thread_name;    /**< The thread name. */
    int pin_to_core;            /**< The core id to pin the thread to. Has the same value range as xCoreId
                                     argument of xTaskCreatePinnedToCore. */
    uint32_t stack_alloc_caps;  /**< A bit mask of memory capabilities (MALLOC_CAPS*) to use when
                                     allocating the stack. The memory must be 8 bit accessible (MALLOC_CAP_8BIT).
                                     The developer is responsible for the correctenss of \c stack_alloc_caps. */
} esp_pthread_cfg_t;

/**
 * @brief Creates a default pthread configuration based
 * on the values set via menuconfig.
 *
 * @return
 *      A default configuration structure.
 */
esp_pthread_cfg_t esp_pthread_get_default_config(void);

/**
 * @brief Configure parameters for creating pthread
 *
 * This API allows you to configure how the subsequent
 * pthread_create() call will behave. This call can be used to setup
 * configuration parameters like stack size, priority, configuration
 * inheritance etc.
 *
 * If the 'inherit' flag in the configuration structure is enabled,
 * then the same configuration is also inherited in the thread
 * subtree.
 *
 * @note If cfg->stack_alloc_caps is 0, it is automatically set to valid default stack memory
 *       capabilities. If cfg->stack_alloc_caps is non-zero, the developer is responsible for its correctenss.
 *       This function only checks that the capabilities are MALLOC_CAP_8BIT, the rest is unchecked.
 * @note Passing non-NULL attributes to pthread_create() will override
 *       the stack_size parameter set using this API
 *
 * @param cfg The pthread config parameters
 *
 * @return
 *      - ESP_OK if configuration was successfully set
 *      - ESP_ERR_NO_MEM if out of memory
 *      - ESP_ERR_INVALID_ARG if stack_size is less than PTHREAD_STACK_MIN
 *      - ESP_ERR_INVALID_ARG if stack_alloc_caps does not include MALLOC_CAP_8BIT
 */
esp_err_t esp_pthread_set_cfg(const esp_pthread_cfg_t *cfg);

/**
 * @brief Get current pthread creation configuration
 *
 * This will retrieve the current configuration that will be used for
 * creating threads.
 *
 * @param p Pointer to the pthread config structure that will be
 * updated with the currently configured parameters
 *
 * @return
 *      - ESP_OK if the configuration was available
 *      - ESP_ERR_NOT_FOUND if a configuration wasn't previously set
 */
esp_err_t esp_pthread_get_cfg(esp_pthread_cfg_t *p);

/**
 * @brief Initialize pthread library
 */
esp_err_t esp_pthread_init(void);

#ifdef __cplusplus
}
#endif
