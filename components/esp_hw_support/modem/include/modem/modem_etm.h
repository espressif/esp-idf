/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Modem ETM event type
 *
 * @note The event type is used to identify the event type.
 */
typedef enum {
    MODEM_ETM_EVENT_G0 = 0,  /*!< Modem ETM event group 0 */
    MODEM_ETM_EVENT_G1 = 1,  /*!< Modem ETM event group 1 */
    MODEM_ETM_EVENT_G2 = 2,  /*!< Modem ETM event group 2 */
    MODEM_ETM_EVENT_G3 = 3,  /*!< Modem ETM event group 3 */
    MODEM_ETM_EVENT_MAX,
} modem_etm_event_type_t;

/**
 * @brief Modem ETM task type
 *
 * @note The task type is used to identify the task type.
 */
typedef enum {
    MODEM_ETM_TASK_G0 = 0,  /*!< Modem ETM task group 0 */
    MODEM_ETM_TASK_G1 = 1,  /*!< Modem ETM task group 1 */
    MODEM_ETM_TASK_G2 = 2,  /*!< Modem ETM task group 2 */
    MODEM_ETM_TASK_G3 = 3,  /*!< Modem ETM task group 3 */
    MODEM_ETM_TASK_MAX,
} modem_etm_task_type_t;

/**
 * @brief Modem ETM event configuration
 *
 * @note The event configuration is used to configure the event.
 */
typedef struct {
    modem_etm_event_type_t event_type;  /*!< Modem ETM event type */
} modem_etm_event_config_t;

/**
 * @brief Modem ETM task configuration
 *
 * @note The task configuration is used to configure the task.
 */
typedef struct {
    modem_etm_task_type_t task_type;  /*!< Modem ETM task type */
} modem_etm_task_config_t;


/**
 * @brief Create a new modem ETM event
 *
 * @param config The modem ETM event configuration
 * @param out_event The output modem ETM event handle
 * @return
 *   - ESP_OK: Success
 *   - ESP_ERR_INVALID_ARG: Invalid argument
 *   - ESP_ERR_NO_MEM: No memory
 */
esp_err_t modem_new_etm_event(const modem_etm_event_config_t *config, esp_etm_event_handle_t *out_event);

/**
 * @brief Create a new modem ETM task
 *
 * @param config The modem ETM task configuration
 * @param out_task The output modem ETM task handle
 * @return
 *   - ESP_OK: Success
 *   - ESP_ERR_INVALID_ARG: Invalid argument
 *   - ESP_ERR_NO_MEM: No memory
 */
esp_err_t modem_new_etm_task(const modem_etm_task_config_t *config, esp_etm_task_handle_t *out_task);

#ifdef __cplusplus
}
#endif
