/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_openthread_types.h"
#include "openthread/error.h"
#include "openthread/instance.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WORKFLOW_MAX_NAMELEN 16

/**
 * @brief update function declaration
 *
 * @param[inout]    mainloop    The main loop context.
 *
 */
typedef void (*esp_openthread_update_func)(esp_openthread_mainloop_context_t *mainloop);

/**
 * @brief process function declaration
 *
 * @param[in]    instance   The OpenThread instance.
 * @param[in]    mainloop    The main loop context.
 *
 * @return
 *      - ESP_OK
 *      - ESP_FAIL
 *
 */
typedef esp_err_t (*esp_openthread_process_func)(otInstance *instance,
                                                 const esp_openthread_mainloop_context_t *mainloop);

/**
 * @brief This struct contains a platform update function, a platform process function
 *        and the workflow name
 *
 * @note The structs will form a list, and the update functions and process functions in the list
 *       will be called in esp_openthread_platform_update and esp_openthread_platform_process.
 *
 */
typedef struct esp_openthread_platform_workflow {
    char name[WORKFLOW_MAX_NAMELEN];
    esp_openthread_update_func update_func;
    esp_openthread_process_func process_func;
    struct esp_openthread_platform_workflow *next;
} esp_openthread_platform_workflow_t;

/**
 * @brief This function adds a esp_openthread_platform_workflow to the workflow list
 *
 *
 * @param[in]  updatefcn   The update function of the workflow added to the list.
 * @param[in]  processfcn  The process function of the workflow added to the list.
 * @param[in]  name        The name of the added workflow
 *
 * @return
 *   - ESP_OK on success
 *   - ESP_ERR_NO_MEM on allocation failure
 *   - ESP_FAIL on other failures
 *
 */
esp_err_t esp_openthread_platform_workflow_register(esp_openthread_update_func update_func,
                                                    esp_openthread_process_func process_func, const char *name);

/**
 * @brief This function removes a esp_openthread_platform_workflow according the input name
 *        from the workflow list
 *
 * @param[in]  name The name of the removed workflow
 *
 */
void esp_openthread_platform_workflow_unregister(const char *name);

/**
 * @brief   Initializes the platform-specific support for the OpenThread stack.
 *
 * @note This function is not called by and will not call the OpenThread library.
 *       The user needs to call otInstanceInitSingle to intialize the OpenThread
 *       stack after calling this function.
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
esp_err_t esp_openthread_platform_init(const esp_openthread_platform_config_t *init_config);

/**
 * This function performs all platform-specific deinitialization for OpenThread's drivers.
 *
 * @note This function is not called by the OpenThread library. Instead, the user should
 *       call this function when deinitialization of OpenThread's drivers is most appropriate.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if not initialized
 *
 */
esp_err_t esp_openthread_platform_deinit(void);

/**
 * @brief This function updates the platform fds and timeouts
 *
 * @note This function will not update the OpenThread core stack pending events.
 *       The users need to call `otTaskletsArePending` to check whether there being
 *       pending OpenThread tasks.
 *
 * @param[inout]    mainloop    The main loop context.
 *
 */
void esp_openthread_platform_update(esp_openthread_mainloop_context_t *mainloop);

/**
 * @brief This function performs the OpenThread related platform process (radio, uart, alarm etc.)
 *
 * @note This function will call the OpenThread core stack process functions.
 *       The users need to call `otTaskletsProcess` by self.
 *
 * @param[in]    instance   The OpenThread instance.
 * @param[in]    mainloop   The main loop context.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failure
 *
 */
esp_err_t esp_openthread_platform_process(otInstance *instance, const esp_openthread_mainloop_context_t *mainloop);

/**
 * @brief This function set the OpenThread storage name
 *
 * @param[in]    name   The OpenThread storage name.
 *
 */
void esp_openthread_set_storage_name(const char *name);
#ifdef __cplusplus
} // end of extern "C"
#endif
