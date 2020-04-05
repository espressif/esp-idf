// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
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

#include <string.h>
#include <errno.h>

#include "mesh_main.h"
#include "client_common.h"
#include "mesh_common.h"

struct net_buf_simple *bt_mesh_alloc_buf(u16_t size)
{
    struct net_buf_simple *buf = NULL;
    u8_t *data = NULL;

    buf = (struct net_buf_simple *)bt_mesh_calloc(sizeof(struct net_buf_simple) + size);
    if (!buf) {
        BT_ERR("%s, Failed to allocate memory", __func__);
        return NULL;
    }

    data = (u8_t *)buf + sizeof(struct net_buf_simple);

    buf->data = data;
    buf->len = 0;
    buf->size = size;
    buf->__buf = data;

    return buf;
}

void bt_mesh_free_buf(struct net_buf_simple *buf)
{
    if (buf) {
        bt_mesh_free(buf);
    }
}

u8_t bt_mesh_get_device_role(struct bt_mesh_model *model, bool srv_send)
{
    bt_mesh_client_user_data_t *client = NULL;

    if (srv_send) {
        BT_DBG("%s, Message is sent by a server model", __func__);
        return NODE;
    }

    if (!model || !model->user_data) {
        BT_ERR("%s, Invalid parameter", __func__);
        return ROLE_NVAL;
    }

    client = (bt_mesh_client_user_data_t *)model->user_data;

    return client->msg_role;
}

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
