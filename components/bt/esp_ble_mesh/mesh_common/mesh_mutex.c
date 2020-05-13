// Copyright 2017-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "mesh_common.h"

static bt_mesh_mutex_t alarm_lock;
static bt_mesh_mutex_t list_lock;
static bt_mesh_mutex_t buf_lock;
static bt_mesh_mutex_t atomic_lock;

void bt_mesh_mutex_create(bt_mesh_mutex_t *mutex)
{
    if (!mutex) {
        BT_ERR("%s, Invalid mutex", __func__);
        return;
    }

#if CONFIG_SPIRAM_USE_MALLOC
    mutex->buffer = heap_caps_calloc(1, sizeof(StaticQueue_t), MALLOC_CAP_DEFAULT|MALLOC_CAP_SPIRAM);
    __ASSERT(mutex->buffer, "%s, Failed to create queue buffer", __func__);
    mutex->mutex = xSemaphoreCreateMutexStatic(mutex->buffer);
    __ASSERT(mutex->mutex, "%s, Failed to create static mutex", __func__);
#else
    mutex->mutex = xSemaphoreCreateMutex();
    __ASSERT(mutex->mutex, "%s, Failed to create mutex", __func__);
#endif
}

void bt_mesh_mutex_free(bt_mesh_mutex_t *mutex)
{
    if (!mutex) {
        BT_ERR("%s, Invalid mutex", __func__);
        return;
    }

    if (mutex->mutex) {
        vSemaphoreDelete(mutex->mutex);
        mutex->mutex = NULL;
#if CONFIG_SPIRAM_USE_MALLOC
        heap_caps_free(mutex->buffer);
        mutex->buffer = NULL;
#endif
    }
}

void bt_mesh_mutex_lock(bt_mesh_mutex_t *mutex)
{
    if (!mutex) {
        BT_ERR("%s, Invalid mutex", __func__);
        return;
    }

    if (mutex->mutex) {
        xSemaphoreTake(mutex->mutex, portMAX_DELAY);
    }
}

void bt_mesh_mutex_unlock(bt_mesh_mutex_t *mutex)
{
    if (!mutex) {
        BT_ERR("%s, Invalid mutex", __func__);
        return;
    }

    if (mutex->mutex) {
        xSemaphoreGive(mutex->mutex);
    }
}

static void bt_mesh_alarm_mutex_new(void)
{
    if (!alarm_lock.mutex) {
        bt_mesh_mutex_create(&alarm_lock);
    }
}

static void bt_mesh_alarm_mutex_free(void)
{
    bt_mesh_mutex_free(&alarm_lock);
}

void bt_mesh_alarm_lock(void)
{
    bt_mesh_mutex_lock(&alarm_lock);
}

void bt_mesh_alarm_unlock(void)
{
    bt_mesh_mutex_unlock(&alarm_lock);
}

static void bt_mesh_list_mutex_new(void)
{
    if (!list_lock.mutex) {
        bt_mesh_mutex_create(&list_lock);
    }
}

static void bt_mesh_list_mutex_free(void)
{
    bt_mesh_mutex_free(&list_lock);
}

void bt_mesh_list_lock(void)
{
    bt_mesh_mutex_lock(&list_lock);
}

void bt_mesh_list_unlock(void)
{
    bt_mesh_mutex_unlock(&list_lock);
}

static void bt_mesh_buf_mutex_new(void)
{
    if (!buf_lock.mutex) {
        bt_mesh_mutex_create(&buf_lock);
    }
}

static void bt_mesh_buf_mutex_free(void)
{
    bt_mesh_mutex_free(&buf_lock);
}

void bt_mesh_buf_lock(void)
{
    bt_mesh_mutex_lock(&buf_lock);
}

void bt_mesh_buf_unlock(void)
{
    bt_mesh_mutex_unlock(&buf_lock);
}

static void bt_mesh_atomic_mutex_new(void)
{
    if (!atomic_lock.mutex) {
        bt_mesh_mutex_create(&atomic_lock);
    }
}

static void bt_mesh_atomic_mutex_free(void)
{
    bt_mesh_mutex_free(&atomic_lock);
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
    bt_mesh_alarm_mutex_new();
    bt_mesh_list_mutex_new();
    bt_mesh_buf_mutex_new();
    bt_mesh_atomic_mutex_new();
}

void bt_mesh_mutex_deinit(void)
{
    bt_mesh_alarm_mutex_free();
    bt_mesh_list_mutex_free();
    bt_mesh_buf_mutex_free();
    bt_mesh_atomic_mutex_free();
}
