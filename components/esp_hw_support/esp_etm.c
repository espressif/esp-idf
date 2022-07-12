/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <sys/cdefs.h>
#include <sys/lock.h>
#include "sdkconfig.h"
#if CONFIG_ETM_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_etm.h"
#include "hal/etm_hal.h"
#include "hal/etm_ll.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/etm_interface.h"

#define ETM_MEM_ALLOC_CAPS   MALLOC_CAP_DEFAULT

static const char *TAG = "etm";

typedef struct etm_platform_t etm_platform_t;
typedef struct etm_group_t etm_group_t;
typedef struct esp_etm_channel_t esp_etm_channel_t;

struct etm_platform_t {
    _lock_t mutex;                        // platform level mutex lock
    etm_group_t *groups[SOC_ETM_GROUPS];  // etm group pool
    int group_ref_counts[SOC_ETM_GROUPS]; // reference count used to protect group install/uninstall
};

struct etm_group_t {
    int group_id;          // hardware group id
    etm_hal_context_t hal; // hardware abstraction layer context
    portMUX_TYPE spinlock; // to protect per-group register level concurrent access
    esp_etm_channel_t *chans[SOC_ETM_CHANNELS_PER_GROUP];
};

typedef enum {
    ETM_CHAN_FSM_INIT,
    ETM_CHAN_FSM_ENABLE,
} etm_chan_fsm_t;

struct esp_etm_channel_t {
    int chan_id;        // Channel ID
    etm_group_t *group; // which group this channel belongs to
    etm_chan_fsm_t fsm; // record ETM channel's driver state
    esp_etm_event_handle_t event; // which event is connect to the channel
    esp_etm_task_handle_t task;   // which task is connect to the channel
};

// ETM driver platform, it's always a singleton
static etm_platform_t s_platform;

static etm_group_t *etm_acquire_group_handle(int group_id)
{
    bool new_group = false;
    etm_group_t *group = NULL;

    // prevent install ETM group concurrently
    _lock_acquire(&s_platform.mutex);
    if (!s_platform.groups[group_id]) {
        group = heap_caps_calloc(1, sizeof(etm_group_t), ETM_MEM_ALLOC_CAPS);
        if (group) {
            new_group = true;
            s_platform.groups[group_id] = group; // register to platform
            // initialize ETM group members
            group->group_id = group_id;
            group->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
            // enable APB access ETM registers
            // if we have multiple ETM groups/instances, we assume the peripheral defines are continuous
            periph_module_enable(PERIPH_ETM_MODULE + group_id);
            periph_module_reset(PERIPH_ETM_MODULE + group_id);
            // initialize HAL context
            etm_hal_init(&group->hal);
        }
    } else {
        group = s_platform.groups[group_id];
    }
    if (group) {
        // someone acquired the group handle means we have a new object that refer to this group
        s_platform.group_ref_counts[group_id]++;
    }
    _lock_release(&s_platform.mutex);

    if (new_group) {
        ESP_LOGD(TAG, "new group (%d) at %p", group_id, group);
    }

    return group;
}

static void etm_release_group_handle(etm_group_t *group)
{
    int group_id = group->group_id;
    bool do_deinitialize = false;

    _lock_acquire(&s_platform.mutex);
    s_platform.group_ref_counts[group_id]--;
    if (s_platform.group_ref_counts[group_id] == 0) {
        assert(s_platform.groups[group_id]);
        do_deinitialize = true;
        s_platform.groups[group_id] = NULL; // deregister from platform
        periph_module_disable(PERIPH_ETM_MODULE + group_id);
    }
    _lock_release(&s_platform.mutex);

    if (do_deinitialize) {
        free(group);
        ESP_LOGD(TAG, "del group (%d)", group_id);
    }
}

static esp_err_t etm_chan_register_to_group(esp_etm_channel_t *chan)
{
    etm_group_t *group = NULL;
    int chan_id = -1;
    for (int i = 0; i < SOC_ETM_GROUPS; i++) {
        group = etm_acquire_group_handle(i);
        ESP_RETURN_ON_FALSE(group, ESP_ERR_NO_MEM, TAG, "no mem for group (%d)", i);
        // loop to search free channel in the group
        portENTER_CRITICAL(&group->spinlock);
        for (int j = 0; j < SOC_ETM_CHANNELS_PER_GROUP; j++) {
            if (!group->chans[j]) {
                chan_id = j;
                group->chans[j] = chan;
                break;
            }
        }
        portEXIT_CRITICAL(&group->spinlock);
        if (chan_id < 0) {
            etm_release_group_handle(group);
            group = NULL;
        } else {
            chan->chan_id = chan_id;
            chan->group = group;
            break;;
        }
    }
    ESP_RETURN_ON_FALSE(chan_id != -1, ESP_ERR_NOT_FOUND, TAG, "no free channel");
    return ESP_OK;
}

static void etm_chan_unregister_from_group(esp_etm_channel_t *chan)
{
    etm_group_t *group = chan->group;
    int chan_id = chan->chan_id;
    portENTER_CRITICAL(&group->spinlock);
    group->chans[chan_id] = NULL;
    portEXIT_CRITICAL(&group->spinlock);
    // channel has a reference on group, release it now
    etm_release_group_handle(group);
}

static esp_err_t etm_chan_destroy(esp_etm_channel_t *chan)
{
    if (chan->group) {
        etm_chan_unregister_from_group(chan);
    }
    free(chan);
    return ESP_OK;
}

esp_err_t esp_etm_new_channel(const esp_etm_channel_config_t *config, esp_etm_channel_handle_t *ret_chan)
{
#if CONFIG_ETM_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    esp_etm_channel_t *chan = NULL;
    ESP_GOTO_ON_FALSE(config && ret_chan, ESP_ERR_INVALID_ARG, err, TAG, "invalid args");

    chan = heap_caps_calloc(1, sizeof(esp_etm_channel_t), ETM_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(chan, ESP_ERR_NO_MEM, err, TAG, "no mem for channel");
    // register channel to the group, one group can have multiple channels
    ESP_GOTO_ON_ERROR(etm_chan_register_to_group(chan), err, TAG, "register channel failed");
    etm_group_t *group = chan->group;
    int group_id = group->group_id;
    int chan_id = chan->chan_id;

    chan->fsm = ETM_CHAN_FSM_INIT;
    ESP_LOGD(TAG, "new etm channel (%d,%d) at %p", group_id, chan_id, chan);
    *ret_chan = chan;
    return ESP_OK;

err:
    if (chan) {
        etm_chan_destroy(chan);
    }
    return ret;
}

esp_err_t esp_etm_del_channel(esp_etm_channel_handle_t chan)
{
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_INVALID_ARG, TAG, "invalid args");
    ESP_RETURN_ON_FALSE(chan->fsm == ETM_CHAN_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "channel is not in init state");
    etm_group_t *group = chan->group;
    int group_id = group->group_id;
    int chan_id = chan->chan_id;

    // disconnect the channel from any event or task
    etm_ll_channel_set_event(group->hal.regs, chan_id, 0);
    etm_ll_channel_set_task(group->hal.regs, chan_id, 0);

    ESP_LOGD(TAG, "del etm channel (%d,%d)", group_id, chan_id);
    // recycle memory resource
    ESP_RETURN_ON_ERROR(etm_chan_destroy(chan), TAG, "destroy etm channel failed");
    return ESP_OK;
}

esp_err_t esp_etm_channel_enable(esp_etm_channel_handle_t chan)
{
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(chan->fsm == ETM_CHAN_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "channel is not in init state");
    etm_group_t *group = chan->group;
    etm_ll_enable_channel(group->hal.regs, chan->chan_id);
    chan->fsm = ETM_CHAN_FSM_ENABLE;
    return ESP_OK;
}

esp_err_t esp_etm_channel_disable(esp_etm_channel_handle_t chan)
{
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(chan->fsm == ETM_CHAN_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "channel not in enable state");
    etm_group_t *group = chan->group;
    etm_ll_disable_channel(group->hal.regs, chan->chan_id);
    chan->fsm = ETM_CHAN_FSM_INIT;
    return ESP_OK;
}

esp_err_t esp_etm_channel_connect(esp_etm_channel_handle_t chan, esp_etm_event_handle_t event, esp_etm_task_handle_t task)
{
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    etm_group_t *group = chan->group;
    uint32_t event_id = 0;
    uint32_t task_id = 0;

    // if the event/task is NULL, then the channel will disconnect from the event/task
    if (event) {
        event_id = event->event_id;
    }
    if (task) {
        task_id = task->task_id;
    }
    etm_ll_channel_set_event(group->hal.regs, chan->chan_id, event_id);
    etm_ll_channel_set_task(group->hal.regs, chan->chan_id, task_id);
    chan->event = event;
    chan->task = task;
    ESP_LOGD(TAG, "event %"PRIu32" => channel %d", event_id, chan->chan_id);
    ESP_LOGD(TAG, "channel %d => task %"PRIu32, chan->chan_id, task_id);
    return ESP_OK;
}

esp_err_t esp_etm_del_event(esp_etm_event_handle_t event)
{
    ESP_RETURN_ON_FALSE(event, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return event->del(event);
}

esp_err_t esp_etm_del_task(esp_etm_task_handle_t task)
{
    ESP_RETURN_ON_FALSE(task, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return task->del(task);
}

esp_err_t esp_etm_dump(FILE *out_stream)
{
    etm_group_t *group = NULL;
    esp_etm_channel_handle_t etm_chan = NULL;
    ESP_RETURN_ON_FALSE(out_stream, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    fprintf(out_stream, "===========ETM Dump Start==========\r\n");
    char line[80];
    size_t len = sizeof(line);
    for (int i = 0; i < SOC_ETM_GROUPS; i++) {
        group = etm_acquire_group_handle(i);
        ESP_RETURN_ON_FALSE(group, ESP_ERR_NO_MEM, TAG, "no mem for group (%d)", i);
        etm_hal_context_t *hal = &group->hal;
        for (int j = 0; j < SOC_ETM_CHANNELS_PER_GROUP; j++) {
            bool print_line = true;
            portENTER_CRITICAL(&group->spinlock);
            etm_chan = group->chans[j];
            if (etm_ll_is_channel_enabled(hal->regs, j)) {
                if (!etm_chan) {
                    // in case the etm driver is bypassed and some channel is enabled in another way (e.g. by hal driver)
                    snprintf(line, len, "channel %d is enabled but not recorded\r\n", j);
                } else {
                    // print which event and task the channel is connected to
                    snprintf(line, len, "channel %d: event %"PRIu32" ==> task %"PRIu32"\r\n", j,
                             etm_chan->event ? etm_chan->event->event_id : 0,
                             etm_chan->task ? etm_chan->task->task_id : 0);
                }
            } else {
                if (etm_chan) {
                    // channel is created, but not enabled by `esp_etm_channel_enable` yet
                    snprintf(line, len, "channel %d is created but not enabled\r\n", j);
                } else {
                    // a free channel, don't print anything
                    print_line = false;
                }
            }
            portEXIT_CRITICAL(&group->spinlock);
            if (print_line) {
                fputs(line, out_stream);
            }
        }
        etm_release_group_handle(group);
    }
    fprintf(out_stream, "===========ETM Dump End============\r\n");
    return ESP_OK;
}
