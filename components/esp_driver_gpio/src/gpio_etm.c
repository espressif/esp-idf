/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
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
    uint8_t tasks[GPIO_LL_ETM_TASK_CHANNELS_PER_GROUP];           // Array of the acquired action masks in each GPIO ETM task channel
    uint8_t events[GPIO_LL_ETM_EVENT_CHANNELS_PER_GROUP];         // Array of the acquired event masks in each GPIO ETM event channel
    uint8_t actions[SOC_GPIO_PIN_COUNT];                          // Array of the masks of the added actions to each GPIO
    uint8_t edges[GPIO_LL_ETM_EVENT_CHANNELS_PER_GROUP];          // Array of the masks of the bound event edges in each GPIO ETM event channel
} gpio_etm_group_t;

struct gpio_etm_event_t {
    esp_etm_event_t base;
    int chan_id;
    gpio_etm_event_edge_t edge_id;
    gpio_etm_group_t *group;
};

struct gpio_etm_task_t {
    esp_etm_task_t base;
    int chan_id;
    gpio_etm_task_action_t action_id;
    gpio_etm_group_t *group;
    size_t num_of_gpios;     // record the number of GPIOs that are bound to the etm task
};

static gpio_etm_group_t s_gpio_etm_group = {
    .dev = &GPIO_ETM,
    .spinlock = portMUX_INITIALIZER_UNLOCKED,
};

static esp_err_t gpio_etm_acquire_event_channel(uint8_t event_mask, int *chan_id)
{
    assert(chan_id);
    gpio_etm_group_t *group = &s_gpio_etm_group;

    int free_chan_id = -1;
    // loop to search free event channel in the group
    portENTER_CRITICAL(&group->spinlock);
    for (int j = 0; j < GPIO_LL_ETM_EVENT_CHANNELS_PER_GROUP; j++) {
        if (!group->events[j]) {
            free_chan_id = j;
            group->events[j] = event_mask;
            break;
        }
    }
    portEXIT_CRITICAL(&group->spinlock);

    ESP_RETURN_ON_FALSE(free_chan_id != -1, ESP_ERR_NOT_FOUND, TAG, "no free event channel");
    *chan_id = free_chan_id;
    return ESP_OK;
}

static esp_err_t gpio_etm_release_event_channel(int chan_id, uint8_t event_mask)
{
    gpio_etm_group_t *group = &s_gpio_etm_group;
    portENTER_CRITICAL(&group->spinlock);
    group->events[chan_id] &= ~event_mask;
    portEXIT_CRITICAL(&group->spinlock);
    return ESP_OK;
}

static esp_err_t gpio_etm_acquire_task_channel(uint8_t task_mask, int *chan_id)
{
    assert(chan_id);
    gpio_etm_group_t *group = &s_gpio_etm_group;

    int free_chan_id = -1;
    // loop to search free task channel in the group
    portENTER_CRITICAL(&group->spinlock);
    for (int j = 0; j < GPIO_LL_ETM_TASK_CHANNELS_PER_GROUP; j++) {
        if (!group->tasks[j]) {
            free_chan_id = j;
            group->tasks[j] = task_mask;
            break;
        }
    }
    portEXIT_CRITICAL(&group->spinlock);

    ESP_RETURN_ON_FALSE(free_chan_id != -1, ESP_ERR_NOT_FOUND, TAG, "no free task channel");
    *chan_id = free_chan_id;
    return ESP_OK;
}

static esp_err_t gpio_etm_release_task_channel(int chan_id, uint8_t task_mask)
{
    gpio_etm_group_t *group = &s_gpio_etm_group;
    portENTER_CRITICAL(&group->spinlock);
    group->tasks[chan_id] &= ~task_mask;
    portEXIT_CRITICAL(&group->spinlock);
    return ESP_OK;
}

static esp_err_t gpio_del_etm_event(esp_etm_event_t *event)
{
    gpio_etm_event_t *gpio_event = __containerof(event, gpio_etm_event_t, base);
    gpio_etm_group_t *group = gpio_event->group;
    // unbind it from the GPIO and check if the GPIO ETM event channel can be disabled
    portENTER_CRITICAL(&group->spinlock);
    group->edges[gpio_event->chan_id] &= ~(1 << gpio_event->edge_id);
    if (!group->edges[gpio_event->chan_id]) {
        gpio_ll_etm_enable_event_channel(group->dev, gpio_event->chan_id, false);
    }
    portEXIT_CRITICAL(&group->spinlock);

    gpio_etm_release_event_channel(gpio_event->chan_id, 1 << gpio_event->edge_id);
    free(gpio_event);
    return ESP_OK;
}

static esp_err_t gpio_del_etm_task(esp_etm_task_t *task)
{
    gpio_etm_task_t *gpio_task = __containerof(task, gpio_etm_task_t, base);
    // make sure user has called `gpio_etm_task_rm_gpio` to clean the etm task channel
    ESP_RETURN_ON_FALSE(gpio_task->num_of_gpios == 0, ESP_ERR_INVALID_STATE, TAG, "some GPIO till bounded to the etm task");
    gpio_etm_release_task_channel(gpio_task->chan_id, 1 << gpio_task->action_id);
    free(gpio_task);
    return ESP_OK;
}

esp_err_t gpio_new_etm_event(const gpio_etm_event_config_t *config, esp_etm_event_handle_t *ret_event, ...)
{
#if CONFIG_ETM_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    int chan_id = -1;
    uint8_t event_mask = 0;
    esp_etm_event_handle_t *ret_event_itor = ret_event;
    va_list args;
    ESP_RETURN_ON_FALSE(config && ret_event, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    for (int i = 0; i < GPIO_ETM_EVENT_EDGE_TYPES; i++) {
        if (config->edges[i]) {
            uint8_t msk = (1 << config->edges[i]);
            ESP_RETURN_ON_FALSE(!(msk & event_mask), ESP_ERR_INVALID_ARG, TAG, "no identical edge event allowed in one call");
            event_mask |= msk;
        }
    }
    ESP_RETURN_ON_FALSE(event_mask, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    uint32_t event_num = __builtin_popcount(event_mask);
    gpio_etm_event_t *events[event_num];

    for (int i = 0; i < event_num; i++) {
        events[i] = (gpio_etm_event_t *)heap_caps_calloc(1, sizeof(gpio_etm_event_t), ETM_MEM_ALLOC_CAPS);
        ESP_GOTO_ON_FALSE(events[i], ESP_ERR_NO_MEM, err, TAG, "no mem for event channel(s)");
    }

    // register the event channel to the group
    ESP_GOTO_ON_ERROR(gpio_etm_acquire_event_channel(event_mask, &chan_id), err, TAG, "register event channel to group failed");

    bool no_avail_ret_arg = false;
    va_start(args, ret_event);
    for (int i = 0, j = 0; i < event_num; i++, j++) {
        if (!ret_event_itor) {
            no_avail_ret_arg = true;
            break;
        }
        // assign to the ret_event handles in the configuration order
        gpio_etm_event_edge_t event_edge;
        do {
            event_edge = config->edges[j];
        } while (!event_edge && ++j);
        uint32_t event_id = 0;
        switch (event_edge) {
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
            va_end(args);
            ESP_GOTO_ON_FALSE(false, ESP_ERR_INVALID_ARG, err, TAG, "invalid edge");
        }

        gpio_etm_event_t *event = events[i];
        event->base.del = gpio_del_etm_event;
        event->base.event_id = event_id;
        event->base.trig_periph = ETM_TRIG_PERIPH_GPIO;
        event->group = &s_gpio_etm_group;
        event->chan_id = chan_id;
        event->edge_id = event_edge;
        ESP_LOGD(TAG, "new event @%p, event_id=%"PRIu32", chan_id=%d", event, event_id, chan_id);
        *ret_event_itor = &event->base;
        ret_event_itor = va_arg(args, esp_etm_event_handle_t *);
    }
    va_end(args);
    ESP_GOTO_ON_FALSE(!no_avail_ret_arg, ESP_ERR_INVALID_ARG, err, TAG, "mismatch number of events with number of pointers to store event handles");
    return ESP_OK;

err:
    if (chan_id != -1) {
        gpio_etm_release_event_channel(chan_id, event_mask);
    }
    for (int i = 0; i < event_num; i++) {
        if (events[i]) {
            free(events[i]);
        }
    }
    ret_event_itor = ret_event;
    va_start(args, ret_event);
    while (ret_event_itor) {
        *ret_event_itor = NULL;
        ret_event_itor = va_arg(args, esp_etm_event_handle_t *);
    }
    va_end(args);
    return ret;
}

esp_err_t gpio_new_etm_task(const gpio_etm_task_config_t *config, esp_etm_task_handle_t *ret_task, ...)
{
#if CONFIG_ETM_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    int chan_id = -1;
    uint8_t task_mask = 0;
    esp_etm_task_handle_t *ret_task_itor = ret_task;
    va_list args;
    ESP_RETURN_ON_FALSE(config && ret_task, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    for (int i = 0; i < GPIO_ETM_TASK_ACTION_TYPES; i++) {
        if (config->actions[i]) {
            uint8_t msk = (1 << config->actions[i]);
            ESP_RETURN_ON_FALSE(!(msk & task_mask), ESP_ERR_INVALID_ARG, TAG, "no identical action allowed in one call");
            task_mask |= msk;
        }
    }
    ESP_RETURN_ON_FALSE(task_mask, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    uint32_t task_num = __builtin_popcount(task_mask);
    gpio_etm_task_t *tasks[task_num];

    for (int i = 0; i < task_num; i++) {
        tasks[i] = (gpio_etm_task_t *)heap_caps_calloc(1, sizeof(gpio_etm_task_t), ETM_MEM_ALLOC_CAPS);
        ESP_GOTO_ON_FALSE(tasks[i], ESP_ERR_NO_MEM, err, TAG, "no mem for task channel(s)");
    }

    // register the task channel to the group
    ESP_GOTO_ON_ERROR(gpio_etm_acquire_task_channel(task_mask, &chan_id), err, TAG, "register task channel to group failed");

    bool no_avail_ret_arg = false;
    va_start(args, ret_task);
    for (int i = 0, j = 0; i < task_num; i++, j++) {
        if (!ret_task_itor) {
            no_avail_ret_arg = true;
            break;
        }
        // assign to the ret_task handles in the configuration order
        gpio_etm_task_action_t action;
        do {
            action = config->actions[i];
        } while (!action && ++j);
        uint32_t task_id = 0;
        switch (action) {
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
            va_end(args);
            ESP_GOTO_ON_FALSE(false, ESP_ERR_INVALID_ARG, err, TAG, "invalid action");
        }

        gpio_etm_task_t *task = tasks[i];
        task->base.del = gpio_del_etm_task;
        task->base.task_id = task_id;
        task->base.trig_periph = ETM_TRIG_PERIPH_GPIO;
        task->group = &s_gpio_etm_group;
        task->chan_id = chan_id;
        task->action_id = action;
        ESP_LOGD(TAG, "new task @%p, task_id=%"PRIu32", gpio_etm_task_chan_id=%d", task, task_id, chan_id);
        *ret_task_itor = &task->base;
        ret_task_itor = va_arg(args, esp_etm_task_handle_t *);
    }
    va_end(args);
    ESP_GOTO_ON_FALSE(!no_avail_ret_arg, ESP_ERR_INVALID_ARG, err, TAG, "mismatch number of tasks with number of pointers to store task handles");
    return ESP_OK;

err:
    if (chan_id != -1) {
        gpio_etm_release_task_channel(chan_id, task_mask);
    }
    for (int i = 0; i < task_num; i++) {
        if (tasks[i]) {
            free(tasks[i]);
        }
    }
    ret_task_itor = ret_task;
    va_start(args, ret_task);
    while (ret_task_itor) {
        *ret_task_itor = NULL;
        ret_task_itor = va_arg(args, esp_etm_task_handle_t *);
    }
    va_end(args);
    return ret;
}

esp_err_t gpio_etm_event_bind_gpio(esp_etm_event_handle_t event, int gpio_num)
{
    ESP_RETURN_ON_FALSE(event, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(event->trig_periph == ETM_TRIG_PERIPH_GPIO, ESP_ERR_INVALID_ARG, TAG, "not a gpio etm event");
    ESP_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(gpio_num), ESP_ERR_INVALID_ARG, TAG, "gpio is not input capable");
    gpio_etm_event_t *gpio_event = __containerof(event, gpio_etm_event_t, base);
    gpio_etm_group_t *group = gpio_event->group;

    bool allowed = true;
    portENTER_CRITICAL(&group->spinlock);
    // check if the GPIO ETM event channel where the new event belongs to has previously been bound to another GPIO
    // one GPIO ETM event channel can only be bound to one GPIO
    if (group->edges[gpio_event->chan_id]) {
        if (gpio_ll_etm_event_channel_get_gpio(group->dev, gpio_event->chan_id) != gpio_num) {
            allowed = false;
        }
    } else {
        // set the GPIO number
        gpio_ll_etm_event_channel_set_gpio(group->dev, gpio_event->chan_id, gpio_num);
        // enable GPIO ETM event channel
        gpio_ll_etm_enable_event_channel(group->dev, gpio_event->chan_id, true);
    }

    if (allowed) {
        group->edges[gpio_event->chan_id] |= (1 << gpio_event->edge_id);
    }
    portEXIT_CRITICAL(&group->spinlock);
    ESP_RETURN_ON_FALSE(allowed, ESP_ERR_INVALID_ARG, TAG, "the GPIO ETM event channel where the event belongs to has already been bound to another GPIO");
    return ESP_OK;
}

esp_err_t gpio_etm_task_add_gpio(esp_etm_task_handle_t task, int gpio_num)
{
    ESP_RETURN_ON_FALSE(task, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(task->trig_periph == ETM_TRIG_PERIPH_GPIO, ESP_ERR_INVALID_ARG, TAG, "not a gpio etm task");
    ESP_RETURN_ON_FALSE(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), ESP_ERR_INVALID_ARG, TAG, "gpio is not output capable");
    gpio_etm_task_t *gpio_task = __containerof(task, gpio_etm_task_t, base);
    gpio_etm_group_t *group = gpio_task->group;

    bool allowed = true;
    // use spinlock as this function may be called with different task object in different threads
    // and the gpio_num might reside in the same register
    portENTER_CRITICAL(&group->spinlock);
    // check if the new task is compatible with the tasks that has previously been added to the GPIO
    // the tasks have to be from the same GPIO ETM task channel
    if (group->actions[gpio_num]) {
        if (gpio_ll_etm_gpio_get_task_channel(group->dev, gpio_num) != gpio_task->chan_id) {
            allowed = false;
        }
    } else {
        // first action added to the GPIO
        gpio_ll_etm_gpio_set_task_channel(group->dev, gpio_num, gpio_task->chan_id);
        gpio_ll_etm_enable_task_gpio(group->dev, gpio_num, true);
    }

    if (allowed) {
        group->actions[gpio_num] |= (1 << gpio_task->action_id);
    }
    portEXIT_CRITICAL(&group->spinlock);
    ESP_RETURN_ON_FALSE(allowed, ESP_ERR_INVALID_ARG, TAG, "the task does not belong to the GPIO ETM task channel that the GPIO has already binded to");
    gpio_task->num_of_gpios++;
    return ESP_OK;
}

esp_err_t gpio_etm_task_rm_gpio(esp_etm_task_handle_t task, int gpio_num)
{
    ESP_RETURN_ON_FALSE(task, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), ESP_ERR_INVALID_ARG, TAG, "gpio is not output capable");
    gpio_etm_task_t *gpio_task = __containerof(task, gpio_etm_task_t, base);
    gpio_etm_group_t *group = gpio_task->group;

    bool allowed = true;
    // use spinlock as this function may be called with different task object in different threads
    // and the gpio_num might reside in the same register
    portENTER_CRITICAL(&group->spinlock);
    if ((group->actions[gpio_num] & (1 << gpio_task->action_id)) &&
            gpio_ll_etm_is_task_gpio_enabled(group->dev, gpio_num) &&
            (gpio_ll_etm_gpio_get_task_channel(group->dev, gpio_num) == gpio_task->chan_id)) {
        group->actions[gpio_num] &= ~(1 << gpio_task->action_id);
        if (!group->actions[gpio_num]) {
            // all actions removed from the GPIO
            gpio_ll_etm_enable_task_gpio(group->dev, gpio_num, false);
        }
    } else {
        allowed = false;
    }
    portEXIT_CRITICAL(&group->spinlock);
    ESP_RETURN_ON_FALSE(allowed, ESP_ERR_INVALID_ARG, TAG, "the task was not added to the GPIO");
    gpio_task->num_of_gpios--;
    return ESP_OK;
}
