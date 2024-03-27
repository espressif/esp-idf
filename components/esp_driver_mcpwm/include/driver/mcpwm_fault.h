/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include "esp_err.h"
#include "driver/mcpwm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MCPWM GPIO fault configuration structure
 */
typedef struct {
    int group_id;                 /*!< In which MCPWM group that the GPIO fault belongs to */
    int intr_priority;            /*!< MCPWM GPIO fault interrupt priority,
                                       if set to 0, the driver will try to allocate an interrupt with a relative low priority (1,2,3) */
    int gpio_num;                 /*!< GPIO used by the fault signal */
    struct {
        uint32_t active_level: 1; /*!< On which level the fault signal is treated as active */
        uint32_t io_loop_back: 1; /*!< For debug/test, the signal output from the GPIO will be fed to the input path as well */
        uint32_t pull_up: 1;      /*!< Whether to pull up internally */
        uint32_t pull_down: 1;    /*!< Whether to pull down internally */
    } flags;                      /*!< Extra configuration flags for GPIO fault */
} mcpwm_gpio_fault_config_t;

/**
 * @brief Create MCPWM GPIO fault
 *
 * @param[in] config MCPWM GPIO fault configuration
 * @param[out] ret_fault Returned GPIO fault handle
 * @return
 *      - ESP_OK: Create MCPWM GPIO fault successfully
 *      - ESP_ERR_INVALID_ARG: Create MCPWM GPIO fault failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create MCPWM GPIO fault failed because out of memory
 *      - ESP_ERR_NOT_FOUND: Create MCPWM GPIO fault failed because can't find free resource
 *      - ESP_FAIL: Create MCPWM GPIO fault failed because of other error
 */
esp_err_t mcpwm_new_gpio_fault(const mcpwm_gpio_fault_config_t *config, mcpwm_fault_handle_t *ret_fault);

/**
 * @brief MCPWM software fault configuration structure
 */
typedef struct {
} mcpwm_soft_fault_config_t;

/**
 * @brief Create MCPWM software fault
 *
 * @param[in] config MCPWM software fault configuration
 * @param[out] ret_fault Returned software fault handle
 * @return
 *      - ESP_OK: Create MCPWM software fault successfully
 *      - ESP_ERR_INVALID_ARG: Create MCPWM software fault failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create MCPWM software fault failed because out of memory
 *      - ESP_FAIL: Create MCPWM software fault failed because of other error
 */
esp_err_t mcpwm_new_soft_fault(const mcpwm_soft_fault_config_t *config, mcpwm_fault_handle_t *ret_fault);

/**
 * @brief Delete MCPWM fault
 *
 * @param[in] fault MCPWM fault handle allocated by `mcpwm_new_gpio_fault()` or `mcpwm_new_soft_fault()`
 * @return
 *      - ESP_OK: Delete MCPWM fault successfully
 *      - ESP_ERR_INVALID_ARG: Delete MCPWM fault failed because of invalid argument
 *      - ESP_FAIL: Delete MCPWM fault failed because of other error
 */
esp_err_t mcpwm_del_fault(mcpwm_fault_handle_t fault);

/**
 * @brief Activate the software fault, trigger the fault event for once
 *
 * @param[in] fault MCPWM soft fault, allocated by `mcpwm_new_soft_fault()`
 * @return
 *      - ESP_OK: Trigger MCPWM software fault event successfully
 *      - ESP_ERR_INVALID_ARG: Trigger MCPWM software fault event failed because of invalid argument
 *      - ESP_FAIL: Trigger MCPWM software fault event failed because of other error
 */
esp_err_t mcpwm_soft_fault_activate(mcpwm_fault_handle_t fault);

/**
 * @brief Group of supported MCPWM fault event callbacks
 * @note The callbacks are all running under ISR environment
 */
typedef struct {
    mcpwm_fault_event_cb_t on_fault_enter; /*!< ISR callback function that would be invoked when fault signal becomes active */
    mcpwm_fault_event_cb_t on_fault_exit;  /*!< ISR callback function that would be invoked when fault signal becomes inactive */
} mcpwm_fault_event_callbacks_t;

/**
 * @brief Set event callbacks for MCPWM fault
 *
 * @note User can deregister a previously registered callback by calling this function and setting the callback member in the `cbs` structure to NULL.
 *
 * @param[in] fault MCPWM GPIO fault handle, allocated by `mcpwm_new_gpio_fault()`
 * @param[in] cbs Group of callback functions
 * @param[in] user_data User data, which will be passed to callback functions directly
 * @return
 *      - ESP_OK: Set event callbacks successfully
 *      - ESP_ERR_INVALID_ARG: Set event callbacks failed because of invalid argument
 *      - ESP_FAIL: Set event callbacks failed because of other error
 */
esp_err_t mcpwm_fault_register_event_callbacks(mcpwm_fault_handle_t fault, const mcpwm_fault_event_callbacks_t *cbs, void *user_data);

#ifdef __cplusplus
}
#endif
