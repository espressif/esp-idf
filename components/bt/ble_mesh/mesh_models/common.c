// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
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
#include <stddef.h>

#include "osi/allocator.h"
#include "errno.h"

#include "mesh_types.h"
#include "mesh_kernel.h"
#include "mesh_buf.h"
#include "mesh_trace.h"
#include "common.h"
#include "bt_mesh_client_common.h"

struct net_buf_simple *bt_mesh_alloc_buf(u16_t size)
{
    struct net_buf_simple *buf;

    buf = (struct net_buf_simple *)osi_calloc(sizeof(struct net_buf_simple) + size);
    if (!buf) {
        BT_ERR("%s: allocate buf memory fail", __func__);
        return NULL;
    }

    /* buf->data will be initialized in net_buf_simple_init() */
    buf->len  = 0;
    buf->size = size;

    return buf;
}

void bt_mesh_free_buf(struct net_buf_simple *buf)
{
    if (buf) {
        osi_free(buf);
    }
}

int bt_mesh_copy_msg_role(bt_mesh_role_param_t *common)
{
    bt_mesh_client_common_t *client = NULL;

    if (!common || !common->model || !common->model->user_data) {
        BT_ERR("%s: invalid parameter", __func__);
        return -EINVAL;
    }

    client = (bt_mesh_client_common_t *)common->model->user_data;

    switch (common->role) {
#if CONFIG_BT_MESH_NODE
    case NODE:
        /* no matter provisioner is enabled/disabled , we can use node role to send messages */
        client->msg_role = NODE;
        break;
#endif
#if CONFIG_BT_MESH_PROVISIONER
    case PROVISIONER:
        /* if provisioner is not enabled, we can't use provisioner role to send messages */
        if (!bt_mesh_is_provisioner_en()) {
            BT_WARN("%s: send provisioner msg with provisioner disabled", __func__);
            return -EINVAL;
        }
        client->msg_role = PROVISIONER;
        break;
#endif
    default:
        BT_WARN("%s: role mismatch", __func__);
        return -EINVAL;
    }

    return 0;
}

u8_t bt_mesh_get_msg_role(struct bt_mesh_model *model, bool srv_send)
{
    bt_mesh_client_common_t *client = NULL;

    if (srv_send) {
        BT_DBG("%s: msg sent by a server model", __func__);
        return NODE;
    }

    if (!model || !model->user_data) {
        BT_ERR("%s: invalid parameter", __func__);
        return ROLE_NVAL;
    }

    client = (bt_mesh_client_common_t *)model->user_data;

    return client->msg_role;
}