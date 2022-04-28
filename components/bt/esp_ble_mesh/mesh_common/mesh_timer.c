/*
 * SPDX-FileCopyrightText: 2016 Intel Corporation
 * SPDX-FileCopyrightText: 2016 Wind River Systems, Inc.
 * SPDX-FileContributor: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>

#include "osi/hash_map.h"
#include "osi/alarm.h"
#include "osi/hash_functions.h"

#include "mesh_common.h"
#include "provisioner_prov.h"

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

int64_t k_uptime_get(void)
{
    /* k_uptime_get_32 is in in milliseconds,
     * but esp_timer_get_time is in microseconds
     */
    return (esp_timer_get_time() / 1000);
}

uint32_t k_uptime_get_32(void)
{
    /* k_uptime_get_32 is in in milliseconds,
     * but esp_timer_get_time is in microseconds
     */
    return (uint32_t)(esp_timer_get_time() / 1000);
}

void bt_mesh_timer_init(void)
{
    bm_alarm_hash_map = hash_map_new(BLE_MESH_GENERAL_ALARM_HASH_MAP_SIZE,
                                     hash_function_pointer, NULL,
                                     (data_free_fn)osi_alarm_free, NULL);
    __ASSERT(bm_alarm_hash_map, "Failed to create hash map");
}

#if CONFIG_BLE_MESH_DEINIT
void bt_mesh_timer_deinit(void)
{
    if (bm_alarm_hash_map) {
        hash_map_free(bm_alarm_hash_map);
        bm_alarm_hash_map = NULL;
    }
}
#endif /* CONFIG_BLE_MESH_DEINIT */

int k_delayed_work_init(struct k_delayed_work *work, k_work_handler_t handler)
{
    osi_alarm_t *alarm = NULL;

    if (!work || !bm_alarm_hash_map) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    k_work_init(&work->work, handler);

    bt_mesh_alarm_lock();
    if (!hash_map_has_key(bm_alarm_hash_map, (void *)work)) {
        alarm = osi_alarm_new("bt_mesh", (osi_alarm_callback_t)handler, (void *)&work->work, 0);
        if (alarm == NULL) {
            BT_ERR("Alarm not created");
            bt_mesh_alarm_unlock();
            return -EIO;
        }
        if (!hash_map_set(bm_alarm_hash_map, work, (void *)alarm)) {
            BT_ERR("Alarm not set");
            bt_mesh_alarm_unlock();
            return -EIO;
        }
    }

    alarm = hash_map_get(bm_alarm_hash_map, work);
    if (alarm == NULL) {
        BT_ERR("Init, alarm not found");
        bt_mesh_alarm_unlock();
        return -ENODEV;
    }

    // Just init the work timer only, don't start it.
    osi_alarm_cancel(alarm);
    bt_mesh_alarm_unlock();
    return 0;
}

int k_delayed_work_submit(struct k_delayed_work *work, int32_t delay)
{
    if (!work || !bm_alarm_hash_map) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    bt_mesh_alarm_lock();
    osi_alarm_t *alarm = hash_map_get(bm_alarm_hash_map, (void *)work);
    if (alarm == NULL) {
        BT_WARN("Submit, alarm not found");
        bt_mesh_alarm_unlock();
        return -EINVAL;
    }

    // Cancel the alarm first, before start the alarm.
    osi_alarm_cancel(alarm);
    osi_alarm_set(alarm, delay);
    bt_mesh_alarm_unlock();
    return 0;
}

int k_delayed_work_submit_periodic(struct k_delayed_work *work, int32_t period)
{
    if (!work || !bm_alarm_hash_map) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    bt_mesh_alarm_lock();
    osi_alarm_t *alarm = hash_map_get(bm_alarm_hash_map, (void *)work);
    if (alarm == NULL) {
        BT_WARN("Submit, alarm not found");
        bt_mesh_alarm_unlock();
        return -EINVAL;
    }

    /* Cancel the alarm first before starting it. */
    osi_alarm_cancel(alarm);
    osi_alarm_set_periodic(alarm, period);
    bt_mesh_alarm_unlock();
    return 0;
}

int k_delayed_work_cancel(struct k_delayed_work *work)
{
    if (!work || !bm_alarm_hash_map) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    bt_mesh_alarm_lock();
    osi_alarm_t *alarm = hash_map_get(bm_alarm_hash_map, (void *)work);
    if (alarm == NULL) {
        BT_WARN("Cancel, alarm not found");
        bt_mesh_alarm_unlock();
        return -EINVAL;
    }

    osi_alarm_cancel(alarm);
    alarm->deadline_us = 0;
    bt_mesh_alarm_unlock();
    return 0;
}

int k_delayed_work_free(struct k_delayed_work *work)
{
    if (!work || !bm_alarm_hash_map) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    bt_mesh_alarm_lock();
    osi_alarm_t *alarm = hash_map_get(bm_alarm_hash_map, work);
    if (alarm == NULL) {
        BT_WARN("Free, alarm not found");
        bt_mesh_alarm_unlock();
        return -EINVAL;
    }

    osi_alarm_cancel(alarm);
    hash_map_erase(bm_alarm_hash_map, work);
    bt_mesh_alarm_unlock();
    return 0;
}

int32_t k_delayed_work_remaining_get(struct k_delayed_work *work)
{
    int32_t time = 0;

    if (!work || !bm_alarm_hash_map) {
        BT_ERR("%s, Invalid parameter", __func__);
        return 0;
    }

    bt_mesh_alarm_lock();
    osi_alarm_t *alarm = hash_map_get(bm_alarm_hash_map, (void *)work);
    if (alarm == NULL) {
        BT_WARN("Get time, alarm not found");
        bt_mesh_alarm_unlock();
        return 0;
    }

    time = osi_alarm_get_remaining_ms(alarm);
    bt_mesh_alarm_unlock();
    return time;
}
