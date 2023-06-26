/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include "sdkconfig.h"
#if CONFIG_ETM_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "driver/gpio_etm.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_check.h"
#include "soc/soc_caps.h"
#include "hal/gpio_ll.h"
#include "hal/gpio_etm_ll.h"
#include "esp_private/etm_interface.h"

#define ETM_MEM_ALLOC_CAPS   MALLOC_CAP_DEFAULT

static const char *TAG = "gpio-etm";

typedef struct gpio_etm_task_t gpio_etm_task_t;
typedef struct gpio_etm_event_t gpio_etm_event_t;

typedef struct gpio_etm_group_t {
    portMUX_TYPE spinlock;
    gpio_etm_dev_t *dev;
    gpio_etm_task_t *tasks[SOC_GPIO_ETM_TASKS_PER_GROUP];
    gpio_etm_event_t *events[SOC_GPIO_ETM_EVENTS_PER_GROUP];
} gpio_etm_group_t;

struct gpio_etm_event_t {
    esp_etm_event_t base;
    int chan_id;
    gpio_etm_group_t *group;
};

struct gpio_etm_task_t {
    esp_etm_task_t base;
    int chan_id;
    gpio_etm_group_t *group;
    size_t num_of_gpios;     // record the number of GPIOs that are bound to the etm task
};

static gpio_etm_group_t s_gpio_etm_group = {
    .dev = &GPIO_ETM,
    .spinlock = portMUX_INITIALIZER_UNLOCKED,
};

static esp_err_t gpio_etm_event_register_to_group(gpio_etm_event_t *event)
{
    gpio_etm_group_t *group = &s_gpio_etm_group;
    int chan_id = -1;
    // loop to search free one in the group
    portENTER_CRITICAL(&group->spinlock);
    for (int j = 0; j < SOC_GPIO_ETM_EVENTS_PER_GROUP; j++) {
        if (!group->events[j]) {
            chan_id = j;
            group->events[j] = event;
            break;
        }
    }
    portEXIT_CRITICAL(&group->spinlock);

    ESP_RETURN_ON_FALSE(chan_id != -1, ESP_ERR_NOT_FOUND, TAG, "no free event channel");
    event->group = group;
    event->chan_id = chan_id;
    return ESP_OK;
}

static esp_err_t gpio_etm_task_register_to_group(gpio_etm_task_t *task)
{
    gpio_etm_group_t *group = &s_gpio_etm_group;
    int chan_id = -1;
    // loop to search free one in the group
    portENTER_CRITICAL(&group->spinlock);
    for (int j = 0; j < SOC_GPIO_ETM_TASKS_PER_GROUP; j++) {
        if (!group->tasks[j]) {
            chan_id = j;
            group->tasks[j] = task;
            break;
        }
    }
    portEXIT_CRITICAL(&group->spinlock);

    ESP_RETURN_ON_FALSE(chan_id != -1, ESP_ERR_NOT_FOUND, TAG, "no free task channel");
    task->group = group;
    task->chan_id = chan_id;
    return ESP_OK;
}

static void gpio_etm_event_unregister_from_group(gpio_etm_event_t *event)
{
    gpio_etm_group_t *group = event->group;
    int chan_id = event->chan_id;
    portENTER_CRITICAL(&group->spinlock);
    group->events[chan_id] = NULL;
    portEXIT_CRITICAL(&group->spinlock);
}

static void gpio_etm_task_unregister_from_group(gpio_etm_task_t *task)
{
    gpio_etm_group_t *group = task->group;
    int chan_id = task->chan_id;
    portENTER_CRITICAL(&group->spinlock);
    group->tasks[chan_id] = NULL;
    portEXIT_CRITICAL(&group->spinlock);
}

static esp_err_t gpio_etm_event_destroy(gpio_etm_event_t *event)
{
    if (event->group) {
        gpio_etm_event_unregister_from_group(event);
    }
    free(event);
    return ESP_OK;
}

static esp_err_t gpio_etm_task_destroy(gpio_etm_task_t *task)
{
    if (task->group) {
        gpio_etm_task_unregister_from_group(task);
    }
    free(task);
    return ESP_OK;
}

static esp_err_t gpio_del_etm_event(esp_etm_event_t *event)
{
    gpio_etm_event_t *gpio_event = __containerof(event, gpio_etm_event_t, base);
    gpio_etm_group_t *group = gpio_event->group;
    // disable event channel
    gpio_ll_etm_enable_event_channel(group->dev, gpio_event->chan_id, false);
    gpio_etm_event_destroy(gpio_event);
    return ESP_OK;
}

static esp_err_t gpio_del_etm_task(esp_etm_task_t *task)
{
    gpio_etm_task_t *gpio_task = __containerof(task, gpio_etm_task_t, base);
    // make sure user has called `gpio_etm_task_rm_gpio` to clean the etm task channel
    ESP_RETURN_ON_FALSE(gpio_task->num_of_gpios == 0, ESP_ERR_INVALID_STATE, TAG, "some GPIO till bounded to the etm task");
    gpio_etm_task_destroy(gpio_task);
    return ESP_OK;
}

esp_err_t gpio_new_etm_event(const gpio_etm_event_config_t *config, esp_etm_event_handle_t *ret_event)
{
#if CONFIG_ETM_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    gpio_etm_event_t *event = NULL;
    ESP_GOTO_ON_FALSE(config && ret_event, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");

    event = heap_caps_calloc(1, sizeof(gpio_etm_event_t), ETM_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(event, ESP_ERR_NO_MEM, err, TAG, "no mem for event channel");
    // register the event channel to the group
    ESP_GOTO_ON_ERROR(gpio_etm_event_register_to_group(event), err, TAG, "register event channel to group failed");
    int chan_id = event->chan_id;

    uint32_t event_id = 0;
    switch (config->edge) {
    case GPIO_ETM_EVENT_EDGE_ANY:
        event_id = GPIO_LL_ETM_EVENT_ID_ANY_EDGE(chan_id);
        break;
    case GPIO_ETM_EVENT_EDGE_POS:
        event_id = GPIO_LL_ETM_EVENT_ID_POS_EDGE(chan_id);
        break;
    case GPIO_ETM_EVENT_EDGE_NEG:
        event_id = GPIO_LL_ETM_EVENT_ID_NEG_EDGE(chan_id);
        break;
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_INVALID_ARG, err, TAG, "invalid edge");
    }

    event->base.del = gpio_del_etm_event;
    event->base.event_id = event_id;
    event->base.trig_periph = ETM_TRIG_PERIPH_GPIO;
    ESP_LOGD(TAG, "new event @%p, event_id=%"PRIu32", chan_id=%d", event, event_id, chan_id);
    *ret_event = &event->base;
    return ESP_OK;

err:
    if (event) {
        gpio_etm_event_destroy(event);
    }
    return ret;
}

esp_err_t gpio_new_etm_task(const gpio_etm_task_config_t *config, esp_etm_task_handle_t *ret_task)
{
#if CONFIG_ETM_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    gpio_etm_task_t *task = NULL;
    ESP_GOTO_ON_FALSE(config && ret_task, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");

    task = heap_caps_calloc(1, sizeof(gpio_etm_task_t), ETM_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(task, ESP_ERR_NO_MEM, err, TAG, "no mem for task channel");
    // register the task channel to the group
    ESP_GOTO_ON_ERROR(gpio_etm_task_register_to_group(task), err, TAG, "register task channel to group failed");
    int chan_id = task->chan_id;

    uint32_t task_id = 0;
    switch (config->action) {
    case GPIO_ETM_TASK_ACTION_SET:
        task_id = GPIO_LL_ETM_TASK_ID_SET(chan_id);
        break;
    case GPIO_ETM_TASK_ACTION_CLR:
        task_id = GPIO_LL_ETM_TASK_ID_CLR(chan_id);
        break;
    case GPIO_ETM_TASK_ACTION_TOG:
        task_id = GPIO_LL_ETM_TASK_ID_TOG(chan_id);
        break;
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_INVALID_ARG, err, TAG, "invalid action");
    }

    task->base.del = gpio_del_etm_task;
    task->base.task_id = task_id;
    task->base.trig_periph = ETM_TRIG_PERIPH_GPIO;
    ESP_LOGD(TAG, "new task @%p, task_id=%"PRIu32", chan_id=%d", task, task_id, chan_id);
    *ret_task = &task->base;
    return ESP_OK;

err:
    if (task) {
        gpio_etm_task_destroy(task);
    }
    return ret;
}

esp_err_t gpio_etm_event_bind_gpio(esp_etm_event_handle_t event, int gpio_num)
{
    ESP_RETURN_ON_FALSE(event, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(event->trig_periph == ETM_TRIG_PERIPH_GPIO, ESP_ERR_INVALID_ARG, TAG, "not a gpio etm event");
    ESP_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(gpio_num), ESP_ERR_INVALID_ARG, TAG, "gpio is not input capable");
    gpio_etm_event_t *gpio_event = __containerof(event, gpio_etm_event_t, base);
    gpio_etm_group_t *group = gpio_event->group;
    // disable gpio etm event channel first
    gpio_ll_etm_enable_event_channel(group->dev, gpio_event->chan_id, false);
    // then set the gpio number
    gpio_ll_etm_event_channel_set_gpio(group->dev, gpio_event->chan_id, gpio_num);
    // enable gpio etm event channel again
    gpio_ll_etm_enable_event_channel(group->dev, gpio_event->chan_id, true);
    return ESP_OK;
}

esp_err_t gpio_etm_task_add_gpio(esp_etm_task_handle_t task, int gpio_num)
{
    ESP_RETURN_ON_FALSE(task, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(task->trig_periph == ETM_TRIG_PERIPH_GPIO, ESP_ERR_INVALID_ARG, TAG, "not a gpio etm task");
    ESP_RETURN_ON_FALSE(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), ESP_ERR_INVALID_ARG, TAG, "gpio is not output capable");
    gpio_etm_task_t *gpio_task = __containerof(task, gpio_etm_task_t, base);
    gpio_etm_group_t *group = gpio_task->group;
    bool gpio_not_enabled = true;
    // use spinlock as this function may be called with different task object in different threads
    // and the gpio_num might reside in the same register
    portENTER_CRITICAL(&group->spinlock);
    // check if the gpio has been enabled
    if (!gpio_ll_etm_is_task_gpio_enabled(group->dev, gpio_num)) {
        gpio_ll_etm_gpio_set_task_channel(group->dev, gpio_num, gpio_task->chan_id);
        gpio_ll_etm_enable_task_gpio(group->dev, gpio_num, true);
    } else {
        gpio_not_enabled = false;
    }
    portEXIT_CRITICAL(&group->spinlock);
    ESP_RETURN_ON_FALSE(gpio_not_enabled, ESP_ERR_INVALID_STATE, TAG, "gpio already enabled by other task channel");
    gpio_task->num_of_gpios++;
    return ESP_OK;
}

esp_err_t gpio_etm_task_rm_gpio(esp_etm_task_handle_t task, int gpio_num)
{
    ESP_RETURN_ON_FALSE(task, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), ESP_ERR_INVALID_ARG, TAG, "gpio is not output capable");
    gpio_etm_task_t *gpio_task = __containerof(task, gpio_etm_task_t, base);
    gpio_etm_group_t *group = gpio_task->group;
    bool gpio_enabled_by_this_task = true;
    // use spinlock as this function may be called with different task object in different threads
    // and the gpio_num might reside in the same register
    portENTER_CRITICAL(&group->spinlock);
    // check if the gpio is managed by this etm task channel
    if (gpio_ll_etm_is_task_gpio_enabled(group->dev, gpio_num) &&
            (gpio_ll_etm_gpio_get_task_channel(group->dev, gpio_num) == gpio_task->chan_id)) {
        gpio_ll_etm_enable_task_gpio(group->dev, gpio_num, false);
    } else {
        gpio_enabled_by_this_task = false;
    }
    portEXIT_CRITICAL(&group->spinlock);
    ESP_RETURN_ON_FALSE(gpio_enabled_by_this_task, ESP_ERR_INVALID_STATE, TAG, "gpio is not enabled by this task channel");
    gpio_task->num_of_gpios--;
    return ESP_OK;
}
