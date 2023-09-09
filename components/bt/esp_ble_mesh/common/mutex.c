/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mesh/common.h"

static bt_mesh_mutex_t alarm_lock;
static bt_mesh_mutex_t list_lock;
static bt_mesh_mutex_t buf_lock;
static bt_mesh_mutex_t atomic_lock;

void bt_mesh_mutex_create(bt_mesh_mutex_t *mutex)
{
    if (!mutex) {
        BT_ERR("Create, invalid mutex");
        return;
    }

#if CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC
#if CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC_EXTERNAL
    mutex->buffer = heap_caps_calloc_prefer(1, sizeof(StaticQueue_t), 2, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#elif CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC_IRAM_8BIT
    mutex->buffer = heap_caps_calloc_prefer(1, sizeof(StaticQueue_t), 2, MALLOC_CAP_INTERNAL|MALLOC_CAP_IRAM_8BIT, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#endif
    __ASSERT(mutex->buffer, "Failed to create mutex buffer");
    mutex->mutex = xSemaphoreCreateMutexStatic(mutex->buffer);
    __ASSERT(mutex->mutex, "Failed to create static mutex");
#else /* CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC */
    mutex->mutex = xSemaphoreCreateMutex();
    __ASSERT(mutex->mutex, "Failed to create mutex");
#endif /* CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC */
}

void bt_mesh_mutex_free(bt_mesh_mutex_t *mutex)
{
    if (!mutex) {
        BT_ERR("Free, invalid mutex");
        return;
    }

    if (mutex->mutex) {
        vSemaphoreDelete(mutex->mutex);
        mutex->mutex = NULL;
#if CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC
        heap_caps_free(mutex->buffer);
        mutex->buffer = NULL;
#endif
    }
}

void bt_mesh_mutex_lock(bt_mesh_mutex_t *mutex)
{
    if (!mutex) {
        BT_ERR("Lock, invalid mutex");
        return;
    }

    if (mutex->mutex) {
        xSemaphoreTake(mutex->mutex, portMAX_DELAY);
    }
}

void bt_mesh_mutex_unlock(bt_mesh_mutex_t *mutex)
{
    if (!mutex) {
        BT_ERR("Unlock, invalid mutex");
        return;
    }

    if (mutex->mutex) {
        xSemaphoreGive(mutex->mutex);
    }
}

void bt_mesh_r_mutex_create(bt_mesh_mutex_t *mutex)
{
    if (!mutex) {
        BT_ERR("Create, invalid recursive mutex");
        return;
    }

#if CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC
#if CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC_EXTERNAL
    mutex->buffer = heap_caps_calloc_prefer(1, sizeof(StaticQueue_t), 2, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#elif CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC_IRAM_8BIT
    mutex->buffer = heap_caps_calloc_prefer(1, sizeof(StaticQueue_t), 2, MALLOC_CAP_INTERNAL|MALLOC_CAP_IRAM_8BIT, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#endif
    __ASSERT(mutex->buffer, "Failed to create recursive mutex buffer");
    mutex->mutex = xSemaphoreCreateRecursiveMutexStatic(mutex->buffer);
    __ASSERT(mutex->mutex, "Failed to create static recursive mutex");
#else /* CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC */
    mutex->mutex = xSemaphoreCreateRecursiveMutex();
    __ASSERT(mutex->mutex, "Failed to create recursive mutex");
#endif /* CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC */
}

void bt_mesh_r_mutex_free(bt_mesh_mutex_t *mutex)
{
    bt_mesh_mutex_free(mutex);
}

void bt_mesh_r_mutex_lock(bt_mesh_mutex_t *mutex)
{
    if (!mutex) {
        BT_ERR("Lock, invalid recursive mutex");
        return;
    }

    if (mutex->mutex) {
        xSemaphoreTakeRecursive(mutex->mutex, portMAX_DELAY);
    }
}

void bt_mesh_r_mutex_unlock(bt_mesh_mutex_t *mutex)
{
    if (!mutex) {
        BT_ERR("Unlock, invalid recursive mutex");
        return;
    }

    if (mutex->mutex) {
        xSemaphoreGiveRecursive(mutex->mutex);
    }
}

void bt_mesh_alarm_lock(void)
{
    bt_mesh_mutex_lock(&alarm_lock);
}

void bt_mesh_alarm_unlock(void)
{
    bt_mesh_mutex_unlock(&alarm_lock);
}

void bt_mesh_list_lock(void)
{
    bt_mesh_mutex_lock(&list_lock);
}

void bt_mesh_list_unlock(void)
{
    bt_mesh_mutex_unlock(&list_lock);
}

void bt_mesh_buf_lock(void)
{
    bt_mesh_mutex_lock(&buf_lock);
}

void bt_mesh_buf_unlock(void)
{
    bt_mesh_mutex_unlock(&buf_lock);
}

void bt_mesh_atomic_lock(void)
{
    bt_mesh_mutex_lock(&atomic_lock);
}

void bt_mesh_atomic_unlock(void)
{
    bt_mesh_mutex_unlock(&atomic_lock);
}

void bt_mesh_mutex_init(void)
{
    bt_mesh_mutex_create(&alarm_lock);
    bt_mesh_mutex_create(&list_lock);
    bt_mesh_mutex_create(&buf_lock);
    bt_mesh_mutex_create(&atomic_lock);
}

#if CONFIG_BLE_MESH_DEINIT
void bt_mesh_mutex_deinit(void)
{
    bt_mesh_mutex_free(&alarm_lock);
    bt_mesh_mutex_free(&list_lock);
    bt_mesh_mutex_free(&buf_lock);
    bt_mesh_mutex_free(&atomic_lock);
}
#endif /* CONFIG_BLE_MESH_DEINIT */
