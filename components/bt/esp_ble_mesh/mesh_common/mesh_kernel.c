/*
 * Copyright (c) 2016 Intel Corporation
 * Copyright (c) 2016 Wind River Systems, Inc.
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>

#include "sdkconfig.h"

#include "osi/hash_map.h"
#include "osi/alarm.h"
#include "osi/hash_functions.h"

#include "bt_common.h"

#include "esp_timer.h"

#include "mesh_kernel.h"
#include "mesh_trace.h"

#include "provisioner_prov.h"

static osi_mutex_t bm_alarm_lock;
static osi_mutex_t bm_irq_lock;
static hash_map_t *bm_alarm_hash_map;
static const size_t BLE_MESH_GENERAL_ALARM_HASH_MAP_SIZE = 20 + CONFIG_BLE_MESH_PBA_SAME_TIME + \
        CONFIG_BLE_MESH_PBG_SAME_TIME;

typedef struct alarm_t {
    /* timer id point to here */
    esp_timer_handle_t alarm_hdl;
    osi_alarm_callback_t cb;
    void *cb_data;
    int64_t deadline_us;
} osi_alarm_t;

unsigned int bt_mesh_irq_lock(void)
{
    /* Changed by Espressif. In BLE Mesh, in order to improve the real-time
     * requirements of bt controller, we use task lock instead of IRQ lock.
     */
    osi_mutex_lock(&bm_irq_lock, OSI_MUTEX_MAX_TIMEOUT);
    return 0;
}

void bt_mesh_irq_unlock(unsigned int key)
{
    osi_mutex_unlock(&bm_irq_lock);
}

s64_t k_uptime_get(void)
{
    /** k_uptime_get_32 is in in milliseconds,
     * but esp_timer_get_time is in microseconds
     */
    return (esp_timer_get_time() / 1000);
}

u32_t k_uptime_get_32(void)
{
    /** k_uptime_get_32 is in in milliseconds,
     * but esp_timer_get_time is in microseconds
     */
    return (u32_t)(esp_timer_get_time() / 1000);
}

void k_sleep(s32_t duration)
{
    vTaskDelay(duration / portTICK_PERIOD_MS);
    return;
}

void bt_mesh_k_init(void)
{
    osi_mutex_new(&bm_alarm_lock);
    osi_mutex_new(&bm_irq_lock);
    bm_alarm_hash_map = hash_map_new(BLE_MESH_GENERAL_ALARM_HASH_MAP_SIZE,
                                     hash_function_pointer, NULL,
                                     (data_free_fn)osi_alarm_free, NULL);
    assert(bm_alarm_hash_map != NULL);
}

void k_delayed_work_init(struct k_delayed_work *work, k_work_handler_t handler)
{
    osi_alarm_t *alarm = NULL;

    assert(work != NULL && bm_alarm_hash_map != NULL);

    k_work_init(&work->work, handler);

    osi_mutex_lock(&bm_alarm_lock, OSI_MUTEX_MAX_TIMEOUT);
    if (!hash_map_has_key(bm_alarm_hash_map, (void *)work)) {
        alarm = osi_alarm_new("bt_mesh", (osi_alarm_callback_t)handler, (void *)&work->work, 0);
        if (alarm == NULL) {
            BT_ERR("%s, Unable to create alarm", __func__);
            return;
        }
        if (!hash_map_set(bm_alarm_hash_map, work, (void *)alarm)) {
            BT_ERR("%s Unable to add the timer to hash map.", __func__);
        }
    }
    osi_mutex_unlock(&bm_alarm_lock);

    alarm = hash_map_get(bm_alarm_hash_map, work);
    if (alarm == NULL) {
        BT_WARN("%s, Unable to find expected alarm in hash map", __func__);
        return;
    }

    // Just init the work timer only, don't start it.
    osi_alarm_cancel(alarm);
    return;
}

int k_delayed_work_submit(struct k_delayed_work *work, s32_t delay)
{
    assert(work != NULL && bm_alarm_hash_map != NULL);

    osi_alarm_t *alarm = hash_map_get(bm_alarm_hash_map, (void *)work);
    if (alarm == NULL) {
        BT_WARN("%s, Unable to find expected alarm in hash map", __func__);
        return -EINVAL;
    }

    // Cancel the alarm first, before start the alarm.
    osi_alarm_cancel(alarm);
    osi_alarm_set(alarm, delay);
    return 0;
}

int k_delayed_work_submit_periodic(struct k_delayed_work *work, s32_t period)
{
    assert(work != NULL && bm_alarm_hash_map != NULL);

    osi_alarm_t *alarm = hash_map_get(bm_alarm_hash_map, (void *)work);
    if (alarm == NULL) {
        BT_WARN("%s, Unable to find expected alarm in hash map", __func__);
        return -EINVAL;
    }

    /* Cancel the alarm first before starting it. */
    osi_alarm_cancel(alarm);
    osi_alarm_set_periodic(alarm, period);

    return 0;
}

int k_delayed_work_cancel(struct k_delayed_work *work)
{
    assert(work != NULL && bm_alarm_hash_map != NULL);

    osi_alarm_t *alarm = hash_map_get(bm_alarm_hash_map, (void *)work);
    if (alarm == NULL) {
        BT_WARN("%s, Unable to find expected alarm in hash map", __func__);
        return -EINVAL;
    }

    osi_alarm_cancel(alarm);
    alarm->deadline_us = 0;
    return 0;
}

int k_delayed_work_free(struct k_delayed_work *work)
{
    assert(work != NULL && bm_alarm_hash_map != NULL);

    osi_alarm_t *alarm = hash_map_get(bm_alarm_hash_map, work);
    if (alarm == NULL) {
        BT_WARN("%s Unable to find expected alarm in hash map", __func__);
        return -EINVAL;
    }

    hash_map_erase(bm_alarm_hash_map, work);
    return 0;
}

s32_t k_delayed_work_remaining_get(struct k_delayed_work *work)
{
    assert(work != NULL && bm_alarm_hash_map != NULL);

    osi_alarm_t *alarm = hash_map_get(bm_alarm_hash_map, (void *)work);
    if (alarm == NULL) {
        BT_WARN("%s Unable to find expected alarm in hash map", __func__);
        return 0;
    }

    return osi_alarm_get_remaining_ms(alarm);
}
