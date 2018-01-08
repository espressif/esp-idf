/*
 * Copyright (c) 2016 Intel Corporation
 * Copyright (c) 2016 Wind River Systems, Inc.
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <xtensa/xtruntime.h>
#include <errno.h>

#include "mesh_kernel.h"
#include "esp_timer.h"
#include "osi/hash_map.h"
#include "osi/alarm.h"
#include "common/bt_trace.h"
#include "common/bt_defs.h"
#include "osi/hash_functions.h"
#include "mesh_trace.h"
#include "sdkconfig.h"

#if CONFIG_BT_MESH
#include "provisioner_prov.h"

osi_mutex_t ble_mesh_alarm_lock;
static osi_mutex_t mesh_irq_lock;
static hash_map_t *ble_mesh_alarm_hash_map;
static const size_t BLE_MESH_GENERAL_ALARM_HASH_MAP_SIZE = 20 + CONFIG_BT_MESH_PBA_SAME_TIME + \
        CONFIG_BT_MESH_PBG_SAME_TIME;


typedef struct alarm_t {
    /* timer id point to here */
    esp_timer_handle_t alarm_hdl;
    osi_alarm_callback_t cb;
    void *cb_data;
    int64_t deadline_us;
} osi_alarm_t;

static void ble_mesh_alarm_cb(void *data)
{
    assert(data != NULL);
    struct k_delayed_work *work = (struct k_delayed_work *)data;
    work->work.handler(&work->work);
    return;
}

unsigned int irq_lock(void)
{
#if defined(CONFIG_BT_MESH_IRQ_LOCK) && CONFIG_BT_MESH_IRQ_LOCK
    unsigned int key = XTOS_SET_INTLEVEL(XCHAL_EXCM_LEVEL);
    return key;
#else /* !CONFIG_BT_MESH_IRQ_LOCK */
    /* Change by Espressif. In BLE Mesh, in order to improve the real-time
     * requirements of bt controller, we use task lock to replace IRQ lock.
     */
    osi_mutex_lock(&mesh_irq_lock, OSI_MUTEX_MAX_TIMEOUT);
    return 0;
#endif /*#if (CONFIG_BT_MESH_IRQ_LOCK) */
}

void irq_unlock(unsigned int key)
{
#if defined(CONFIG_BT_MESH_IRQ_LOCK) && CONFIG_BT_MESH_IRQ_LOCK
    XTOS_RESTORE_INTLEVEL(key);
#else /* !CONFIG_BT_MESH_IRQ_LOCK */
    osi_mutex_unlock(&mesh_irq_lock);
#endif /*#if (CONFIG_BT_MESH_IRQ_LOCK) && CONFIG_BT_MESH_IRQ_LOCK */
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

void mesh_k_init(void)
{
    osi_mutex_new(&ble_mesh_alarm_lock);
    osi_mutex_new(&mesh_irq_lock);
    ble_mesh_alarm_hash_map = hash_map_new(BLE_MESH_GENERAL_ALARM_HASH_MAP_SIZE,
                                           hash_function_pointer, NULL, (data_free_fn)osi_alarm_free, NULL);
    if (ble_mesh_alarm_hash_map == NULL) {
        goto error_exit;
    }

    return;
error_exit:
    LOG_ERROR("%s Unable to allocate resources for ble mesh alarm hash map", __func__);
    return;
}

void k_delayed_work_init(struct k_delayed_work *work, k_work_handler_t handler)
{
    assert(work != NULL && ble_mesh_alarm_hash_map != NULL);

    k_work_init(&work->work, handler);
    _init_timeout(&work->timeout, NULL);
    work->work_q = NULL;

    osi_alarm_t *alarm = NULL;

    // Get the alarm for the timer list entry.
    osi_mutex_lock(&ble_mesh_alarm_lock, OSI_MUTEX_MAX_TIMEOUT);
    if (!hash_map_has_key(ble_mesh_alarm_hash_map, (void *)work)) {
        alarm = osi_alarm_new("ble_mesh", ble_mesh_alarm_cb, (void *)work, 0);
        if (!hash_map_set(ble_mesh_alarm_hash_map, work, (void *)alarm)) {
            LOG_ERROR("%s Unable to add the work timer to the mesh alram hash map.", __func__);
        }
    }
    osi_mutex_unlock(&ble_mesh_alarm_lock);

    alarm = hash_map_get(ble_mesh_alarm_hash_map, work);
    if (alarm == NULL) {
        LOG_ERROR("%s Unable to create alarm", __func__);
        return;
    }
    // Just init the work timer only, don't start it.
    osi_alarm_cancel(alarm);
    // The following line code just for testing.
    return;

}

int k_delayed_work_submit(struct k_delayed_work *work,
                          s32_t delay)
{
    assert(work != NULL);

    osi_alarm_t *alarm = hash_map_get(ble_mesh_alarm_hash_map, (void *)work);
    if (alarm == NULL) {
        LOG_WARN("%s, The ble mesh hash map didn't find the alram., work = %p", __func__, work);
        return -EINVAL;
    }
    //cancel the alarm first, before start the alarm.
    osi_alarm_cancel(alarm);

    osi_alarm_set(alarm, delay);
    return 0;
}


int k_delayed_work_cancel(struct k_delayed_work *work)
{
    assert(work != NULL);

    // Check the work have been store in the ble_mesh timer list or not.
    osi_alarm_t *alarm = hash_map_get(ble_mesh_alarm_hash_map, (void *)work);
    if (alarm == NULL) {
        LOG_WARN("%s, The ble mesh hash map didn't find the alram.", __func__);
        return -EINVAL;
    }
    osi_alarm_cancel(alarm);
    return 0;
}

int k_delayed_work_free(struct k_delayed_work *work)
{
    assert(work != NULL);

    // Get the alarm for the timer list entry.
    osi_alarm_t *alarm = hash_map_get(ble_mesh_alarm_hash_map, work);
    if (alarm == NULL) {
        LOG_WARN("%s Unable to find expected alarm in hashmap", __func__);
        return -EINVAL;
    }

    hash_map_erase(ble_mesh_alarm_hash_map, work);

    return 0;
}

s32_t k_delayed_work_remaining_get(struct k_delayed_work *work)
{
    assert(work != NULL);
    s32_t remain_time = 0;
    int64_t now = esp_timer_get_time();
    osi_alarm_t *alarm = hash_map_get(ble_mesh_alarm_hash_map, (void *)work);
    if (alarm == NULL) {
        LOG_WARN("%s, The mesh hash map didn't find the alarm.", __func__);
        return 0;
    }
    if ((alarm->deadline_us - now) < 0x1FFFFFFFFFF) {
        remain_time = (alarm->deadline_us - now) / 1000;
    } else {
        return 0;
    }
    return remain_time;
}


void k_sem_give(struct k_sem *sem)
{
    assert(sem != NULL);
    osi_mutex_unlock(sem->mutex);
    return;
}

void k_sem_init(struct k_sem *sem, unsigned int initial_count,
                unsigned int limit)
{
    assert(sem != NULL);
    UNUSED(initial_count);
    UNUSED(limit);
    sem->mutex = xSemaphoreCreateBinary();
    if (sem->mutex == NULL) {
        LOG_WARN("%s, the mutex alloc fail", __func__);
        return;
    }

    return;
}

int k_sem_take(struct k_sem *sem, s32_t timeout)
{
    assert(sem != NULL);
    return osi_mutex_lock(sem->mutex, timeout);
}

#endif /* #if CONFIG_BT_MESH */
