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

#include "client_common.h"
#include "mesh_common.h"

struct net_buf_simple *bt_mesh_alloc_buf(u16_t size)
{
    struct net_buf_simple *buf = NULL;
    u8_t *data = NULL;

    buf = (struct net_buf_simple *)osi_calloc(sizeof(struct net_buf_simple) + size);
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
        osi_free(buf);
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
