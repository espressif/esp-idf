/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <stdarg.h>
#include <sys/cdefs.h>
#include "sdkconfig.h"
#if CONFIG_MCPWM_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "freertos/FreeRTOS.h"
#include "esp_attr.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_memory_utils.h"
#include "soc/soc_caps.h"
#include "soc/mcpwm_periph.h"
#include "hal/mcpwm_ll.h"
#include "driver/mcpwm_sync.h"
#include "driver/gpio.h"
#include "mcpwm_private.h"

static const char *TAG = "mcpwm";

static esp_err_t mcpwm_del_timer_sync_src(mcpwm_sync_t *sync_src);
static esp_err_t mcpwm_del_gpio_sync_src(mcpwm_sync_t *sync_src);
static esp_err_t mcpwm_del_soft_sync_src(mcpwm_sync_t *sync_src);

static esp_err_t mcpwm_timer_sync_src_register_to_timer(mcpwm_timer_sync_src_t *timer_sync_src, mcpwm_timer_t *timer)
{
    bool new_sync = false;
    portENTER_CRITICAL(&timer->spinlock);
    if (!timer->sync_src) {
        new_sync = true;
        timer->sync_src = timer_sync_src;
    }
    portEXIT_CRITICAL(&timer->spinlock);
    ESP_RETURN_ON_FALSE(new_sync, ESP_ERR_INVALID_STATE, TAG, "timer sync_src already installed for timer (%d,%d)",
                        timer->group->group_id, timer->timer_id);

    timer_sync_src->timer = timer;
    return ESP_OK;
}

static void mcpwm_timer_sync_src_unregister_from_timer(mcpwm_timer_sync_src_t *timer_sync_src)
{
    mcpwm_timer_t *timer = timer_sync_src->timer;

    portENTER_CRITICAL(&timer->spinlock);
    timer->sync_src = NULL;
    portEXIT_CRITICAL(&timer->spinlock);
}

static esp_err_t mcpwm_timer_sync_src_destroy(mcpwm_timer_sync_src_t *timer_sync_src)
{
    if (timer_sync_src->timer) {
        mcpwm_timer_sync_src_unregister_from_timer(timer_sync_src);
    }
    free(timer_sync_src);
    return ESP_OK;
}

esp_err_t mcpwm_new_timer_sync_src(mcpwm_timer_handle_t timer, const mcpwm_timer_sync_src_config_t *config, mcpwm_sync_handle_t *ret_sync)
{
    esp_err_t ret = ESP_OK;
    mcpwm_timer_sync_src_t *timer_sync_src = NULL;
    ESP_GOTO_ON_FALSE(timer && config && ret_sync, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    timer_sync_src = heap_caps_calloc(1, sizeof(mcpwm_timer_sync_src_t), MCPWM_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(timer_sync_src, ESP_ERR_NO_MEM, err, TAG, "no mem for timer sync_src");

    ESP_GOTO_ON_ERROR(mcpwm_timer_sync_src_register_to_timer(timer_sync_src, timer), err, TAG, "register timer sync_src failed");
    mcpwm_group_t *group = timer->group;
    mcpwm_hal_context_t *hal = &group->hal;
    int timer_id = timer->timer_id;

    if (config->flags.propagate_input_sync) {
        mcpwm_ll_timer_propagate_input_sync(hal->dev, timer_id);
    } else {
        switch (config->timer_event) {
        case MCPWM_TIMER_EVENT_EMPTY:
            mcpwm_ll_timer_sync_out_on_timer_event(hal->dev, timer_id, MCPWM_TIMER_EVENT_EMPTY);
            break;
        case MCPWM_TIMER_EVENT_FULL:
            mcpwm_ll_timer_sync_out_on_timer_event(hal->dev, timer_id, MCPWM_TIMER_EVENT_FULL);
            break;
        default:
            ESP_GOTO_ON_FALSE(false, ESP_ERR_INVALID_ARG, err, TAG, "unknown timer sync event:%d", config->timer_event);
            break;
        }
    }

    timer_sync_src->base.group = group;
    timer_sync_src->base.type = MCPWM_SYNC_TYPE_TIMER;
    timer_sync_src->base.del = mcpwm_del_timer_sync_src;
    *ret_sync = &timer_sync_src->base;
    ESP_LOGD(TAG, "new timer sync_src at %p in timer (%d,%d), event:%c", timer_sync_src, group->group_id, timer_id, "EP?"[config->timer_event]);
    return ESP_OK;

err:
    if (timer_sync_src) {
        mcpwm_timer_sync_src_destroy(timer_sync_src);
    }
    return ret;
}

static esp_err_t mcpwm_del_timer_sync_src(mcpwm_sync_t *sync_src)
{
    mcpwm_timer_sync_src_t *timer_sync_src = __containerof(sync_src, mcpwm_timer_sync_src_t, base);
    mcpwm_timer_t *timer = timer_sync_src->timer;
    int timer_id = timer->timer_id;
    mcpwm_group_t *group = sync_src->group;

    mcpwm_ll_timer_disable_sync_out(group->hal.dev, timer_id);
    ESP_LOGD(TAG, "del timer sync_src in timer (%d,%d)", group->group_id, timer_id);
    ESP_RETURN_ON_ERROR(mcpwm_timer_sync_src_destroy(timer_sync_src), TAG, "destroy timer sync_src failed");
    return ESP_OK;
}

static esp_err_t mcpwm_gpio_sync_src_register_to_group(mcpwm_gpio_sync_src_t *gpio_sync_src, int group_id)
{
    mcpwm_group_t *group = mcpwm_acquire_group_handle(group_id);
    ESP_RETURN_ON_FALSE(group, ESP_ERR_NO_MEM, TAG, "no mem for group (%d)", group_id);

    int sync_id = -1;
    portENTER_CRITICAL(&group->spinlock);
    for (int i = 0; i < SOC_MCPWM_GPIO_SYNCHROS_PER_GROUP; i++) {
        if (!group->gpio_sync_srcs[i]) {
            sync_id = i;
            group->gpio_sync_srcs[i] = gpio_sync_src;
            break;
        }
    }
    portEXIT_CRITICAL(&group->spinlock);

    if (sync_id < 0) {
        mcpwm_release_group_handle(group);
        group = NULL;
    } else {
        gpio_sync_src->base.group = group;
        gpio_sync_src->sync_id = sync_id;
    }
    ESP_RETURN_ON_FALSE(sync_id >= 0, ESP_ERR_NOT_FOUND, TAG, "no free gpio sync_src in group (%d)", group_id);

    return ESP_OK;
}

static void mcpwm_gpio_sync_src_unregister_from_group(mcpwm_gpio_sync_src_t *gpio_sync_src)
{
    mcpwm_group_t *group = gpio_sync_src->base.group;
    int sync_id = gpio_sync_src->sync_id;

    portENTER_CRITICAL(&group->spinlock);
    group->gpio_sync_srcs[sync_id] = NULL;
    portEXIT_CRITICAL(&group->spinlock);

    // sync_src has a reference on group, release it now
    mcpwm_release_group_handle(group);
}

static esp_err_t mcpwm_gpio_sync_src_destroy(mcpwm_gpio_sync_src_t *gpio_sync_src)
{
    if (gpio_sync_src->base.group) {
        mcpwm_gpio_sync_src_unregister_from_group(gpio_sync_src);
    }
    free(gpio_sync_src);
    return ESP_OK;
}

esp_err_t mcpwm_new_gpio_sync_src(const mcpwm_gpio_sync_src_config_t *config, mcpwm_sync_handle_t *ret_sync)
{
#if CONFIG_MCPWM_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    mcpwm_gpio_sync_src_t *gpio_sync_src = NULL;
    ESP_GOTO_ON_FALSE(config && ret_sync, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(config->group_id < SOC_MCPWM_GROUPS && config->group_id >= 0, ESP_ERR_INVALID_ARG,
                      err, TAG, "invalid group ID:%d", config->group_id);

    gpio_sync_src = heap_caps_calloc(1, sizeof(mcpwm_gpio_sync_src_t), MCPWM_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(gpio_sync_src, ESP_ERR_NO_MEM, err, TAG, "no mem for gpio sync_src");

    ESP_GOTO_ON_ERROR(mcpwm_gpio_sync_src_register_to_group(gpio_sync_src, config->group_id), err, TAG, "register gpio sync_src failed");
    mcpwm_group_t *group = gpio_sync_src->base.group;
    int group_id = group->group_id;
    int sync_id = gpio_sync_src->sync_id;

    // GPIO configuration
    gpio_config_t gpio_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT | (config->flags.io_loop_back ? GPIO_MODE_OUTPUT : 0), // also enable the output path if `io_loop_back` is enabled
        .pin_bit_mask = (1ULL << config->gpio_num),
        .pull_down_en = config->flags.pull_down,
        .pull_up_en = config->flags.pull_up,
    };
    ESP_GOTO_ON_ERROR(gpio_config(&gpio_conf), err, TAG, "config sync GPIO failed");
    esp_rom_gpio_connect_in_signal(config->gpio_num, mcpwm_periph_signals.groups[group_id].gpio_synchros[sync_id].sync_sig, 0);

    // different ext sync share the same config register, using a group level spin lock
    portENTER_CRITICAL(&group->spinlock);
    mcpwm_ll_invert_gpio_sync_input(group->hal.dev, sync_id, config->flags.active_neg);
    portEXIT_CRITICAL(&group->spinlock);

    // fill in other operator members
    gpio_sync_src->base.type = MCPWM_SYNC_TYPE_GPIO;
    gpio_sync_src->gpio_num = config->gpio_num;
    gpio_sync_src->base.del = mcpwm_del_gpio_sync_src;
    *ret_sync = &gpio_sync_src->base;
    ESP_LOGD(TAG, "new gpio sync_src (%d,%d) at %p, GPIO:%d", group_id, sync_id, gpio_sync_src, config->gpio_num);
    return ESP_OK;

err:
    if (gpio_sync_src) {
        mcpwm_gpio_sync_src_destroy(gpio_sync_src);
    }
    return ret;
}

static esp_err_t mcpwm_del_gpio_sync_src(mcpwm_sync_t *sync_src)
{
    mcpwm_gpio_sync_src_t *gpio_sync_src = __containerof(sync_src, mcpwm_gpio_sync_src_t, base);
    mcpwm_group_t *group = sync_src->group;

    ESP_LOGD(TAG, "del gpio sync_src (%d,%d)", group->group_id, gpio_sync_src->sync_id);
    gpio_reset_pin(gpio_sync_src->gpio_num);

    // recycle memory resource
    ESP_RETURN_ON_ERROR(mcpwm_gpio_sync_src_destroy(gpio_sync_src), TAG, "destroy GPIO sync_src failed");
    return ESP_OK;
}

esp_err_t mcpwm_new_soft_sync_src(const mcpwm_soft_sync_config_t *config, mcpwm_sync_handle_t *ret_sync)
{
    esp_err_t ret = ESP_OK;
    mcpwm_soft_sync_src_t *soft_sync = NULL;
    ESP_GOTO_ON_FALSE(config && ret_sync, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    soft_sync = heap_caps_calloc(1, sizeof(mcpwm_soft_sync_src_t), MCPWM_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(soft_sync, ESP_ERR_NO_MEM, err, TAG, "no mem for soft sync");

    // fill in other sync member
    soft_sync->soft_sync_from = MCPWM_SOFT_SYNC_FROM_NONE;
    soft_sync->base.type = MCPWM_SYNC_TYPE_SOFT;
    soft_sync->base.del = mcpwm_del_soft_sync_src;
    *ret_sync = &soft_sync->base;
    ESP_LOGD(TAG, "new soft sync at %p", soft_sync);
    return ESP_OK;

err:
    // soft_sync must be NULL in the error handling path, and it's a determined behaviour to free a NULL pointer in esp-idf
    free(soft_sync);
    return ret;
}

static esp_err_t mcpwm_del_soft_sync_src(mcpwm_sync_t *sync_src)
{
    mcpwm_soft_sync_src_t *soft_sync = __containerof(sync_src, mcpwm_soft_sync_src_t, base);
    ESP_LOGD(TAG, "del soft sync %p", soft_sync);
    free(soft_sync);
    return ESP_OK;
}

esp_err_t mcpwm_del_sync_src(mcpwm_sync_handle_t sync_src)
{
    ESP_RETURN_ON_FALSE(sync_src, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return sync_src->del(sync_src);
}

esp_err_t mcpwm_soft_sync_activate(mcpwm_sync_handle_t sync_src)
{
    ESP_RETURN_ON_FALSE(sync_src, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(sync_src->type == MCPWM_SYNC_TYPE_SOFT, ESP_ERR_INVALID_ARG, TAG, "not a valid soft sync");
    mcpwm_group_t *group = sync_src->group;
    mcpwm_soft_sync_src_t *soft_sync = __containerof(sync_src, mcpwm_soft_sync_src_t, base);

    switch (soft_sync->soft_sync_from) {
    case MCPWM_SOFT_SYNC_FROM_TIMER: {
        mcpwm_timer_t *timer = soft_sync->timer;
        mcpwm_ll_timer_trigger_soft_sync(group->hal.dev, timer->timer_id);
        break;
    }
    case MCPWM_SOFT_SYNC_FROM_CAP: {
        mcpwm_ll_capture_trigger_sw_sync(group->hal.dev);
        break;
    }
    default:
        ESP_RETURN_ON_FALSE(false, ESP_ERR_INVALID_STATE, TAG, "no soft sync generator is assigned");
        break;
    }
    return ESP_OK;
}
