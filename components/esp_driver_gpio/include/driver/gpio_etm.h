/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "esp_etm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GPIO_ETM_EVENT_EDGE_TYPES   3   /*!< GPIO ETM edge events are POS/NEG/ANY */
#define GPIO_ETM_TASK_ACTION_TYPES  3   /*!< GPIO ETM action tasks are SET/CLEAR/TOGGLE */

/**
 * @brief GPIO edges that can be used as ETM event
 */
typedef enum {
    GPIO_ETM_EVENT_EDGE_POS = 1,    /*!< A rising edge on the GPIO will generate an ETM event signal */
    GPIO_ETM_EVENT_EDGE_NEG,        /*!< A falling edge on the GPIO will generate an ETM event signal */
    GPIO_ETM_EVENT_EDGE_ANY,        /*!< Any edge on the GPIO can generate an ETM event signal */
} gpio_etm_event_edge_t;

/**
 * @brief GPIO ETM event configuration
 *
 * If more than one kind of ETM edge event want to be triggered on the same GPIO pin, you can configure them together.
 * It helps to save GPIO ETM event channel resources for other GPIOs.
 */
typedef struct {
    union {
        gpio_etm_event_edge_t edge;                                 /*!< Which kind of edge can trigger the ETM event module */
        gpio_etm_event_edge_t edges[GPIO_ETM_EVENT_EDGE_TYPES];     /*!< Array of kinds of edges to trigger the ETM event module on the same GPIO */
    };
} gpio_etm_event_config_t;

/**
 * @brief Create an ETM event object for the GPIO peripheral
 *
 * @note The created ETM event object can be deleted later by calling `esp_etm_del_event`
 * @note The newly created ETM event object is not bind to any GPIO, you need to call `gpio_etm_event_bind_gpio` to bind the wanted GPIO
 * @note Every success call to this function will acquire a free GPIO ETM event channel
 *
 * @param[in] config GPIO ETM event configuration
 * @param[out] ret_event Returned ETM event handle
 * @param[out] ...  Other returned ETM event handles if any (the order of the returned event handles is aligned with the array order in field `edges` in `gpio_etm_event_config_t`)
 * @return
 *      - ESP_OK: Create ETM event successfully
 *      - ESP_ERR_INVALID_ARG: Create ETM event failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create ETM event failed because of out of memory
 *      - ESP_ERR_NOT_FOUND: Create ETM event failed because all events are used up and no more free one
 *      - ESP_FAIL: Create ETM event failed because of other reasons
 */
esp_err_t gpio_new_etm_event(const gpio_etm_event_config_t *config, esp_etm_event_handle_t *ret_event, ...);

/**
 * @brief Bind the GPIO with the ETM event
 *
 * @note Calling this function multiple times with different GPIO number can override the previous setting immediately.
 * @note Only GPIO ETM object can call this function
 *
 * @param[in] event ETM event handle that created by `gpio_new_etm_event`
 * @param[in] gpio_num GPIO number that can trigger the ETM event
 * @return
 *      - ESP_OK: Set the GPIO for ETM event successfully
 *      - ESP_ERR_INVALID_ARG: Set the GPIO for ETM event failed because of invalid argument, e.g. GPIO is not input capable, ETM event is not of GPIO type
 *      - ESP_FAIL: Set the GPIO for ETM event failed because of other reasons
 */
esp_err_t gpio_etm_event_bind_gpio(esp_etm_event_handle_t event, int gpio_num);

/**
 * @brief GPIO actions that can be taken by the ETM task
 */
typedef enum {
    GPIO_ETM_TASK_ACTION_SET = 1,   /*!< Set the GPIO level to high */
    GPIO_ETM_TASK_ACTION_CLR,       /*!< Clear the GPIO level to low */
    GPIO_ETM_TASK_ACTION_TOG,       /*!< Toggle the GPIO level */
} gpio_etm_task_action_t;

/**
 * @brief GPIO ETM task configuration
 *
 * If multiple actions wants to be added to the same GPIO pin, you have to configure all the GPIO ETM tasks together.
 */
typedef struct {
    union {
        gpio_etm_task_action_t action;                                  /*!< Action to take by the ETM task module */
        gpio_etm_task_action_t actions[GPIO_ETM_TASK_ACTION_TYPES];     /*!< Array of actions to take by the ETM task module on the same GPIO */
    };
} gpio_etm_task_config_t;

/**
 * @brief Create an ETM task object for the GPIO peripheral
 *
 * @note The created ETM task object can be deleted later by calling `esp_etm_del_task`
 * @note The GPIO ETM task works like a container, a newly created ETM task object doesn't have GPIO members to be managed.
 *       You need to call `gpio_etm_task_add_gpio` to put one or more GPIOs to the container.
 * @note Every success call to this function will acquire a free GPIO ETM task channel
 *
 * @param[in] config GPIO ETM task configuration
 * @param[out] ret_task Returned ETM task handle
 * @param[out] ...  Other returned ETM task handles if any (the order of the returned task handles is aligned with the array order in field `actions` in `gpio_etm_task_config_t`)
 * @return
 *      - ESP_OK: Create ETM task successfully
 *      - ESP_ERR_INVALID_ARG: Create ETM task failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create ETM task failed because of out of memory
 *      - ESP_ERR_NOT_FOUND: Create ETM task failed because all tasks are used up and no more free one
 *      - ESP_FAIL: Create ETM task failed because of other reasons
 */
esp_err_t gpio_new_etm_task(const gpio_etm_task_config_t *config, esp_etm_task_handle_t *ret_task, ...);

/**
 * @brief Add GPIO to the ETM task.
 *
 * @note You can call this function multiple times to add more GPIOs
 * @note Only GPIO ETM object can call this function
 *
 * @param[in] task ETM task handle that created by `gpio_new_etm_task`
 * @param[in] gpio_num GPIO number that can be controlled by the ETM task
 * @return
 *      - ESP_OK: Add GPIO to the ETM task successfully
 *      - ESP_ERR_INVALID_ARG: Add GPIO to the ETM task failed because of invalid argument, e.g. GPIO is not output capable, ETM task is not of GPIO type
 *      - ESP_ERR_INVALID_STATE: Add GPIO to the ETM task failed because the GPIO is used by other ETM task already
 *      - ESP_FAIL: Add GPIO to the ETM task failed because of other reasons
 */
esp_err_t gpio_etm_task_add_gpio(esp_etm_task_handle_t task, int gpio_num);

/**
 * @brief Remove the GPIO from the ETM task
 *
 * @note Before deleting the ETM task, you need to remove all the GPIOs from the ETM task by this function
 * @note Only GPIO ETM object can call this function
 *
 * @param[in] task ETM task handle that created by `gpio_new_etm_task`
 * @param[in] gpio_num GPIO number that to be remove from the ETM task
 * @return
 *      - ESP_OK: Remove the GPIO from the ETM task successfully
 *      - ESP_ERR_INVALID_ARG: Remove the GPIO from the ETM task failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Remove the GPIO from the ETM task failed because the GPIO is not controlled by this ETM task
 *      - ESP_FAIL: Remove the GPIO from the ETM task failed because of other reasons
 */
esp_err_t gpio_etm_task_rm_gpio(esp_etm_task_handle_t task, int gpio_num);

#ifdef __cplusplus
}
#endif
