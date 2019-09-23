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
#include <stdbool.h>

#include "osi/allocator.h"
#include "sdkconfig.h"

#include "mesh_types.h"
#include "mesh_kernel.h"
#include "mesh_trace.h"

#include "mesh.h"
#include "model_opcode.h"
#include "mesh_common.h"
#include "generic_client.h"

#include "btc_ble_mesh_generic_model.h"

/** The following are the macro definitions of generic client
 *  model messages length, and a message is composed of three
 *  parts: Opcode + msg_value + MIC
 */
/* Generic onoff client messages length */
#define BLE_MESH_GEN_ONOFF_GET_MSG_LEN              (2 + 0 + 4)
#define BLE_MESH_GEN_ONOFF_SET_MSG_LEN              (2 + 4 + 4)

/* Generic level client messages length */
#define BLE_MESH_GEN_LEVEL_GET_MSG_LEN              (2 + 0 + 4)
#define BLE_MESH_GEN_LEVEL_SET_MSG_LEN              (2 + 5 + 4)
#define BLE_MESH_GEN_DELTA_SET_MSG_LEN              (2 + 7 + 4)
#define BLE_MESH_GEN_MOVE_SET_MSG_LEN               (2 + 5 + 4)

/* Generic default transition time client messages length */
#define BLE_MESH_GEN_DEF_TRANS_TIME_GET_MSG_LEN     (2 + 0 + 4)
#define BLE_MESH_GEN_DEF_TRANS_TIME_SET_MSG_LEN     (2 + 1 + 4)

/* Generic power onoff client messages length */
#define BLE_MESH_GEN_ONPOWERUP_GET_MSG_LEN          (2 + 0 + 4)
#define BLE_MESH_GEN_ONPOWERUP_SET_MSG_LEN          (2 + 1 + 4)

/* Generic power level client messages length */
#define BLE_MESH_GEN_POWER_LEVEL_GET_MSG_LEN        (2 + 0 + 4)
#define BLE_MESH_GEN_POWER_LEVEL_SET_MSG_LEN        (2 + 5 + 4)
#define BLE_MESH_GEN_POWER_LAST_GET_MSG_LEN         (2 + 0 + 4)
#define BLE_MESH_GEN_POWER_DEFAULT_GET_MSG_LEN      (2 + 0 + 4)
#define BLE_MESH_GEN_POWER_DEFAULT_SET_MSG_LEN      (2 + 2 + 4)
#define BLE_MESH_GEN_POWER_RANGE_GET_MSG_LEN        (2 + 0 + 4)
#define BLE_MESH_GEN_POWER_RANGE_SET_MSG_LEN        (2 + 4 + 4)

/* Generic battery client messages length */
#define BLE_MESH_GEN_BATTERY_GET_MSG_LEN            (2 + 0 + 4)

/* Generic location client messages length */
#define BLE_MESH_GEN_LOC_GLOBAL_GET_MSG_LEN         (2 +  0 + 4)
#define BLE_MESH_GEN_LOC_GLOBAL_SET_MSG_LEN         (1 + 10 + 4)
#define BLE_MESH_GEN_LOC_LOCAL_GET_MSG_LEN          (2 +  0 + 4)
#define BLE_MESH_GEN_LOC_LOCAL_SET_MSG_LEN          (2 +  9 + 4)

/* Generic property client messages length */
#define BLE_MESH_GEN_USER_PROPERTIES_GET_MSG_LEN    (2 + 0 + 4)
#define BLE_MESH_GEN_USER_PROPERTY_GET_MSG_LEN      (2 + 2 + 4)
#define BLE_MESH_GEN_USER_PROPERTY_SET_MSG_LEN      /* variable */
#define BLE_MESH_GEN_ADMIN_PROPERTIES_GET_MSG_LEN   (2 + 0 + 4)
#define BLE_MESH_GEN_ADMIN_PROPERTY_GET_MSG_LEN     (2 + 2 + 4)
#define BLE_MESH_GEN_ADMIN_PROPERTY_SET_MSG_LEN     /* variable */
#define BLE_MESH_GEN_MANU_PROPERTIES_GET_MSG_LEN    (2 + 0 + 4)
#define BLE_MESH_GEN_MANU_PROPERTY_GET_MSG_LEN      (2 + 2 + 4)
#define BLE_MESH_GEN_MANU_PROPERTY_SET_MSG_LEN      (1 + 3 + 4)
#define BLE_MESH_GEN_CLINET_PROPERTIES_GET_MSG_LEN  (1 + 2 + 4)

#define BLE_MESH_GEN_GET_STATE_MSG_LEN              (2 + 2 + 4)

static const bt_mesh_client_op_pair_t gen_op_pair[] = {
    { BLE_MESH_MODEL_OP_GEN_ONOFF_GET,             BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS             },
    { BLE_MESH_MODEL_OP_GEN_ONOFF_SET,             BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS             },
    { BLE_MESH_MODEL_OP_GEN_LEVEL_GET,             BLE_MESH_MODEL_OP_GEN_LEVEL_STATUS             },
    { BLE_MESH_MODEL_OP_GEN_LEVEL_SET,             BLE_MESH_MODEL_OP_GEN_LEVEL_STATUS             },
    { BLE_MESH_MODEL_OP_GEN_DELTA_SET,             BLE_MESH_MODEL_OP_GEN_LEVEL_STATUS             },
    { BLE_MESH_MODEL_OP_GEN_MOVE_SET,              BLE_MESH_MODEL_OP_GEN_LEVEL_STATUS             },
    { BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_GET,    BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_STATUS    },
    { BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET,    BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_STATUS    },
    { BLE_MESH_MODEL_OP_GEN_ONPOWERUP_GET,         BLE_MESH_MODEL_OP_GEN_ONPOWERUP_STATUS         },
    { BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET,         BLE_MESH_MODEL_OP_GEN_ONPOWERUP_STATUS         },
    { BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_GET,       BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS       },
    { BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET,       BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS       },
    { BLE_MESH_MODEL_OP_GEN_POWER_LAST_GET,        BLE_MESH_MODEL_OP_GEN_POWER_LAST_STATUS        },
    { BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_GET,     BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_STATUS     },
    { BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET,     BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_STATUS     },
    { BLE_MESH_MODEL_OP_GEN_POWER_RANGE_GET,       BLE_MESH_MODEL_OP_GEN_POWER_RANGE_STATUS       },
    { BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET,       BLE_MESH_MODEL_OP_GEN_POWER_RANGE_STATUS       },
    { BLE_MESH_MODEL_OP_GEN_BATTERY_GET,           BLE_MESH_MODEL_OP_GEN_BATTERY_STATUS           },
    { BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_GET,        BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_STATUS        },
    { BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET,        BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_STATUS        },
    { BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_GET,         BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_STATUS         },
    { BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET,         BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_STATUS         },
    { BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_GET,   BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_STATUS   },
    { BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_GET,     BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_STATUS     },
    { BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET,     BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_STATUS     },
    { BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_GET,  BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_STATUS  },
    { BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_GET,    BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_STATUS    },
    { BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET,    BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_STATUS    },
    { BLE_MESH_MODEL_OP_GEN_MANU_PROPERTIES_GET,   BLE_MESH_MODEL_OP_GEN_MANU_PROPERTIES_STATUS   },
    { BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_GET,     BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_STATUS     },
    { BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET,     BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_STATUS     },
    { BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_GET, BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_STATUS },
};

static void timeout_handler(struct k_work *work)
{
    generic_internal_data_t *internal = NULL;
    bt_mesh_generic_client_t *client = NULL;
    bt_mesh_client_node_t *node = NULL;

    BT_WARN("Receive generic status message timeout");

    node = CONTAINER_OF(work, bt_mesh_client_node_t, timer.work);
    if (!node || !node->ctx.model) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    client = (bt_mesh_generic_client_t *)node->ctx.model->user_data;
    if (!client) {
        BT_ERR("%s, Generic Client user_data is NULL", __func__);
        return;
    }

    internal = (generic_internal_data_t *)client->internal_data;
    if (!internal) {
        BT_ERR("%s, Generic Client internal_data is NULL", __func__);
        return;
    }

    bt_mesh_generic_client_cb_evt_to_btc(node->opcode,
        BTC_BLE_MESH_EVT_GENERIC_CLIENT_TIMEOUT, node->ctx.model, &node->ctx, NULL, 0);

    bt_mesh_client_free_node(&internal->queue, node);

    return;
}

static void generic_status(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf)
{
    generic_internal_data_t *internal = NULL;
    bt_mesh_generic_client_t *client = NULL;
    bt_mesh_client_node_t *node = NULL;
    u8_t  *val = NULL;
    u8_t   evt = 0xFF;
    u32_t  rsp = 0;
    size_t len = 0;

    BT_DBG("%s, len %d, bytes %s", __func__, buf->len, bt_hex(buf->data, buf->len));

    client = (bt_mesh_generic_client_t *)model->user_data;
    if (!client) {
        BT_ERR("%s, Generic Client user_data is NULL", __func__);
        return;
    }

    internal = (generic_internal_data_t *)client->internal_data;
    if (!internal) {
        BT_ERR("%s, Generic Client internal_data is NULL", __func__);
        return;
    }

    rsp = ctx->recv_op;

    switch (rsp) {
    case BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS: {
        struct bt_mesh_gen_onoff_status *status = NULL;
        if (buf->len != 1 && buf->len != 3) {
            BT_ERR("Invalid Generic OnOff Status length %d", buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_gen_onoff_status));
        if (!status) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        status->present_onoff = net_buf_simple_pull_u8(buf);
        if (buf->len) {
            status->op_en = true;
            status->target_onoff = net_buf_simple_pull_u8(buf);
            status->remain_time = net_buf_simple_pull_u8(buf);
        }
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_gen_onoff_status);
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_LEVEL_STATUS: {
        struct bt_mesh_gen_level_status *status = NULL;
        if (buf->len != 2 && buf->len != 5) {
            BT_ERR("Invalid Generic Level Status length %d", buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_gen_level_status));
        if (!status) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        status->present_level = net_buf_simple_pull_le16(buf);
        if (buf->len) {
            status->op_en = true;
            status->target_level = net_buf_simple_pull_le16(buf);
            status->remain_time = net_buf_simple_pull_u8(buf);
        }
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_gen_level_status);
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_STATUS: {
        struct bt_mesh_gen_def_trans_time_status *status = NULL;
        if (buf->len != 1) {
            BT_ERR("Invalid Generic Default Trans Time Status length %d", buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_gen_def_trans_time_status));
        if (!status) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        status->trans_time = net_buf_simple_pull_u8(buf);
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_gen_def_trans_time_status);
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_ONPOWERUP_STATUS: {
        struct bt_mesh_gen_onpowerup_status *status = NULL;
        if (buf->len != 1) {
            BT_ERR("Invalid Generic OnPowerUp Status length %d", buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_gen_onpowerup_status));
        if (!status) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        status->onpowerup = net_buf_simple_pull_u8(buf);
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_gen_onpowerup_status);
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS: {
        struct bt_mesh_gen_power_level_status *status = NULL;
        if (buf->len != 2 && buf->len != 5) {
            BT_ERR("Invalid Generic Power Level Status length %d", buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_gen_power_level_status));
        if (!status) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        status->present_power = net_buf_simple_pull_le16(buf);
        if (buf->len) {
            status->op_en = true;
            status->target_power = net_buf_simple_pull_le16(buf);
            status->remain_time = net_buf_simple_pull_u8(buf);
        }
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_gen_power_level_status);
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_POWER_LAST_STATUS: {
        struct bt_mesh_gen_power_last_status *status = NULL;
        if (buf->len != 2) {
            BT_ERR("Invalid Generic Power Last Status length %d", buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_gen_power_last_status));
        if (!status) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        status->power = net_buf_simple_pull_le16(buf);
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_gen_power_last_status);
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_STATUS: {
        struct bt_mesh_gen_power_default_status *status = NULL;
        if (buf->len != 2) {
            BT_ERR("Invalid Generic Power Default Status length %d", buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_gen_power_default_status));
        if (!status) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        status->power = net_buf_simple_pull_le16(buf);
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_gen_power_default_status);
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_POWER_RANGE_STATUS: {
        struct bt_mesh_gen_power_range_status *status = NULL;
        if (buf->len != 5) {
            BT_ERR("Invalid Generic Power Range Status length %d", buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_gen_power_range_status));
        if (!status) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        status->status_code = net_buf_simple_pull_u8(buf);
        status->range_min = net_buf_simple_pull_le16(buf);
        status->range_max = net_buf_simple_pull_le16(buf);
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_gen_power_range_status);
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_BATTERY_STATUS: {
        struct bt_mesh_gen_battery_status *status = NULL;
        if (buf->len != 8) {
            BT_ERR("Invalid Generic Battery Status length %d", buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_gen_battery_status));
        if (!status) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        u32_t value = 0;
        value = net_buf_simple_pull_le32(buf);
        status->battery_level = (u8_t)value;
        status->time_to_discharge = (value >> 8);
        value = net_buf_simple_pull_le32(buf);
        status->time_to_charge = (value & 0xffffff);
        status->flags = (u8_t)(value >> 24);
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_gen_battery_status);
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_STATUS: {
        struct bt_mesh_gen_loc_global_status *status = NULL;
        if (buf->len != 10) {
            BT_ERR("Invalid Generic Location Global Status length %d", buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_gen_loc_global_status));
        if (!status) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        status->global_latitude = net_buf_simple_pull_le32(buf);
        status->global_longitude = net_buf_simple_pull_le32(buf);
        status->global_altitude = net_buf_simple_pull_le16(buf);
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_gen_loc_global_status);
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_STATUS: {
        struct bt_mesh_gen_loc_local_status *status = NULL;
        if (buf->len != 9) {
            BT_ERR("Invalid Generic Location Local Status length %d", buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_gen_loc_local_status));
        if (!status) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        status->local_north = net_buf_simple_pull_le16(buf);
        status->local_east = net_buf_simple_pull_le16(buf);
        status->local_altitude = net_buf_simple_pull_le16(buf);
        status->floor_number = net_buf_simple_pull_u8(buf);
        status->uncertainty = net_buf_simple_pull_le16(buf);
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_gen_loc_local_status);
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_STATUS: {
        struct bt_mesh_gen_user_properties_status *status = NULL;
        status = osi_calloc(sizeof(struct bt_mesh_gen_user_properties_status));
        if (!status) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        status->user_property_ids = bt_mesh_alloc_buf(buf->len);
        if (!status->user_property_ids) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            osi_free(status);
            return;
        }
        net_buf_simple_add_mem(status->user_property_ids, buf->data, buf->len);
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_gen_user_properties_status);
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_STATUS: {
        struct bt_mesh_gen_user_property_status *status = NULL;
        status = osi_calloc(sizeof(struct bt_mesh_gen_user_property_status));
        if (!status) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        status->user_property_id = net_buf_simple_pull_le16(buf);
        if (buf->len) {
            status->op_en = true;
            status->user_access = net_buf_simple_pull_u8(buf);
            status->user_property_value = bt_mesh_alloc_buf(buf->len);
            if (!status->user_property_value) {
                BT_ERR("%s, Failed to allocate memory", __func__);
                osi_free(status);
                return;
            }
            net_buf_simple_add_mem(status->user_property_value, buf->data, buf->len);
        }
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_gen_user_property_status);
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_STATUS: {
        struct bt_mesh_gen_admin_properties_status *status = NULL;
        status = osi_calloc(sizeof(struct bt_mesh_gen_admin_properties_status));
        if (!status) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        status->admin_property_ids = bt_mesh_alloc_buf(buf->len);
        if (!status->admin_property_ids) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            osi_free(status);
            return;
        }
        net_buf_simple_add_mem(status->admin_property_ids, buf->data, buf->len);
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_gen_admin_properties_status);
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_STATUS: {
        struct bt_mesh_gen_admin_property_status *status = NULL;
        status = osi_calloc(sizeof(struct bt_mesh_gen_admin_property_status));
        if (!status) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        status->admin_property_id = net_buf_simple_pull_le16(buf);
        if (buf->len) {
            status->op_en = true;
            status->admin_user_access = net_buf_simple_pull_u8(buf);
            status->admin_property_value = bt_mesh_alloc_buf(buf->len);
            if (!status->admin_property_value) {
                BT_ERR("%s, Failed to allocate memory", __func__);
                osi_free(status);
                return;
            }
            net_buf_simple_add_mem(status->admin_property_value, buf->data, buf->len);
        }
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_gen_admin_property_status);
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_MANU_PROPERTIES_STATUS: {
        struct bt_mesh_gen_manu_properties_status *status = NULL;
        status = osi_calloc(sizeof(struct bt_mesh_gen_manu_properties_status));
        if (!status) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        status->manu_property_ids = bt_mesh_alloc_buf(buf->len);
        if (!status->manu_property_ids) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            osi_free(status);
            return;
        }
        net_buf_simple_add_mem(status->manu_property_ids, buf->data, buf->len);
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_gen_manu_properties_status);
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_STATUS: {
        struct bt_mesh_gen_manu_property_status *status = NULL;
        status = osi_calloc(sizeof(struct bt_mesh_gen_manu_property_status));
        if (!status) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        status->manu_property_id = net_buf_simple_pull_le16(buf);
        if (buf->len) {
            status->op_en = true;
            status->manu_user_access = net_buf_simple_pull_u8(buf);
            status->manu_property_value = bt_mesh_alloc_buf(buf->len);
            if (!status->manu_property_value) {
                BT_ERR("%s, Failed to allocate memory", __func__);
                osi_free(status);
                return;
            }
            net_buf_simple_add_mem(status->manu_property_value, buf->data, buf->len);
        }
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_gen_manu_property_status);
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_STATUS: {
        struct bt_mesh_gen_client_properties_status *status = NULL;
        status = osi_calloc(sizeof(struct bt_mesh_gen_client_properties_status));
        if (!status) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return;
        }
        status->client_property_ids = bt_mesh_alloc_buf(buf->len);
        if (!status->client_property_ids) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            osi_free(status);
            return;
        }
        net_buf_simple_add_mem(status->client_property_ids, buf->data, buf->len);
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_gen_client_properties_status);
        break;
    }
    default:
        BT_ERR("%s, Not a Generic Status message opcode", __func__);
        return;
    }

    buf->data = val;
    buf->len  = len;
    node = bt_mesh_is_client_recv_publish_msg(model, ctx, buf, true);
    if (!node) {
        BT_DBG("Unexpected generic status message 0x%x", rsp);
    } else {
        switch (node->opcode) {
        case BLE_MESH_MODEL_OP_GEN_ONOFF_GET:
        case BLE_MESH_MODEL_OP_GEN_LEVEL_GET:
        case BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_GET:
        case BLE_MESH_MODEL_OP_GEN_ONPOWERUP_GET:
        case BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_GET:
        case BLE_MESH_MODEL_OP_GEN_POWER_LAST_GET:
        case BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_GET:
        case BLE_MESH_MODEL_OP_GEN_POWER_RANGE_GET:
        case BLE_MESH_MODEL_OP_GEN_BATTERY_GET:
        case BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_GET:
        case BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_GET:
        case BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_GET:
        case BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_GET:
        case BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_GET:
        case BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_GET:
        case BLE_MESH_MODEL_OP_GEN_MANU_PROPERTIES_GET:
        case BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_GET:
        case BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_GET:
            evt = BTC_BLE_MESH_EVT_GENERIC_CLIENT_GET_STATE;
            break;
        case BLE_MESH_MODEL_OP_GEN_ONOFF_SET:
        case BLE_MESH_MODEL_OP_GEN_LEVEL_SET:
        case BLE_MESH_MODEL_OP_GEN_DELTA_SET:
        case BLE_MESH_MODEL_OP_GEN_MOVE_SET:
        case BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET:
        case BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET:
        case BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET:
        case BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET:
        case BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET:
        case BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET:
        case BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET:
        case BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET:
        case BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET:
        case BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET:
            evt = BTC_BLE_MESH_EVT_GENERIC_CLIENT_SET_STATE;
            break;
        default:
            break;
        }

        bt_mesh_generic_client_cb_evt_to_btc(node->opcode, evt, model, ctx, val, len);
        // Don't forget to release the node at the end.
        bt_mesh_client_free_node(&internal->queue, node);
    }

    switch (rsp) {
    case BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_STATUS: {
        struct bt_mesh_gen_user_properties_status *status;
        status = (struct bt_mesh_gen_user_properties_status *)val;
        bt_mesh_free_buf(status->user_property_ids);
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_STATUS: {
        struct bt_mesh_gen_user_property_status *status;
        status = (struct bt_mesh_gen_user_property_status *)val;
        bt_mesh_free_buf(status->user_property_value);
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_STATUS: {
        struct bt_mesh_gen_admin_properties_status *status;
        status = (struct bt_mesh_gen_admin_properties_status *)val;
        bt_mesh_free_buf(status->admin_property_ids);
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_STATUS: {
        struct bt_mesh_gen_admin_property_status *status;
        status = (struct bt_mesh_gen_admin_property_status *)val;
        bt_mesh_free_buf(status->admin_property_value);
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_MANU_PROPERTIES_STATUS: {
        struct bt_mesh_gen_manu_properties_status *status;
        status = (struct bt_mesh_gen_manu_properties_status *)val;
        bt_mesh_free_buf(status->manu_property_ids);
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_STATUS: {
        struct bt_mesh_gen_manu_property_status *status;
        status = (struct bt_mesh_gen_manu_property_status *)val;
        bt_mesh_free_buf(status->manu_property_value);
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_STATUS: {
        struct bt_mesh_gen_client_properties_status *status;
        status = (struct bt_mesh_gen_client_properties_status *)val;
        bt_mesh_free_buf(status->client_property_ids);
        break;
    }
    default:
        break;
    }

    osi_free(val);

    return;
}

const struct bt_mesh_model_op gen_onoff_cli_op[] = {
    { BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS, 1, generic_status },
    BLE_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op gen_level_cli_op[] = {
    { BLE_MESH_MODEL_OP_GEN_LEVEL_STATUS, 2, generic_status },
    BLE_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op gen_def_trans_time_cli_op[] = {
    { BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_STATUS, 1, generic_status },
    BLE_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op gen_power_onoff_cli_op[] = {
    { BLE_MESH_MODEL_OP_GEN_ONPOWERUP_STATUS, 1, generic_status },
    BLE_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op gen_power_level_cli_op[] = {
    { BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS,   2, generic_status   },
    { BLE_MESH_MODEL_OP_GEN_POWER_LAST_STATUS,    2, generic_status    },
    { BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_STATUS, 2, generic_status },
    { BLE_MESH_MODEL_OP_GEN_POWER_RANGE_STATUS,   5, generic_status   },
    BLE_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op gen_battery_cli_op[] = {
    { BLE_MESH_MODEL_OP_GEN_BATTERY_STATUS, 8, generic_status },
    BLE_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op gen_location_cli_op[] = {
    { BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_STATUS, 10, generic_status },
    { BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_STATUS,  9,  generic_status },
    BLE_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op gen_property_cli_op[] = {
    { BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_STATUS,   2, generic_status },
    { BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_STATUS,     2, generic_status },
    { BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_STATUS,  2, generic_status },
    { BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_STATUS,    2, generic_status },
    { BLE_MESH_MODEL_OP_GEN_MANU_PROPERTIES_STATUS,   2, generic_status },
    { BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_STATUS,     2, generic_status },
    { BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_STATUS, 2, generic_status },
    BLE_MESH_MODEL_OP_END,
};

static int gen_get_state(bt_mesh_client_common_param_t *common, void *value)
{
    NET_BUF_SIMPLE_DEFINE(msg, BLE_MESH_GEN_GET_STATE_MSG_LEN);
    int err;

    bt_mesh_model_msg_init(&msg, common->opcode);

    if (value) {
        switch (common->opcode) {
        case BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_GET: {
            struct bt_mesh_gen_user_property_get *get;
            get = (struct bt_mesh_gen_user_property_get *)value;
            net_buf_simple_add_le16(&msg, get->user_property_id);
            break;
        }
        case BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_GET: {
            struct bt_mesh_gen_admin_property_get *get;
            get = (struct bt_mesh_gen_admin_property_get *)value;
            net_buf_simple_add_le16(&msg, get->admin_property_id);
            break;
        }
        case BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_GET: {
            struct bt_mesh_gen_manu_property_get *get;
            get = (struct bt_mesh_gen_manu_property_get *)value;
            net_buf_simple_add_le16(&msg, get->manu_property_id);
            break;
        }
        case BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_GET: {
            struct bt_mesh_gen_client_properties_get *get;
            get = (struct bt_mesh_gen_client_properties_get *)value;
            net_buf_simple_add_le16(&msg, get->client_property_id);
            break;
        }
        default:
            BT_DBG("This generic message should be sent with NULL get pointer");
            break;
        }
    }

    err = bt_mesh_client_send_msg(common->model, common->opcode, &common->ctx, &msg,
                                  timeout_handler, common->msg_timeout, true,
                                  common->cb, common->cb_data);
    if (err) {
        BT_ERR("%s, Failed to send Generic Get message (err %d)", __func__, err);
    }

    return err;
}

static int gen_set_state(bt_mesh_client_common_param_t *common,
                         void *value, u16_t value_len, bool need_ack)
{
    struct net_buf_simple *msg = NULL;
    int err;

    msg = bt_mesh_alloc_buf(value_len);
    if (!msg) {
        BT_ERR("%s, Failed to allocate memory", __func__);
        return -ENOMEM;
    }

    bt_mesh_model_msg_init(msg, common->opcode);

    switch (common->opcode) {
    case BLE_MESH_MODEL_OP_GEN_ONOFF_SET:
    case BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK: {
        struct bt_mesh_gen_onoff_set *set;
        set = (struct bt_mesh_gen_onoff_set *)value;
        net_buf_simple_add_u8(msg, set->onoff);
        net_buf_simple_add_u8(msg, set->tid);
        if (set->op_en) {
            net_buf_simple_add_u8(msg, set->trans_time);
            net_buf_simple_add_u8(msg, set->delay);
        }
        break;
    }

    case BLE_MESH_MODEL_OP_GEN_LEVEL_SET:
    case BLE_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK: {
        struct bt_mesh_gen_level_set *set;
        set = (struct bt_mesh_gen_level_set *)value;
        net_buf_simple_add_le16(msg, set->level);
        net_buf_simple_add_u8(msg,   set->tid);
        if (set->op_en) {
            net_buf_simple_add_u8(msg, set->trans_time);
            net_buf_simple_add_u8(msg, set->delay);
        }
        break;
    }

    case BLE_MESH_MODEL_OP_GEN_DELTA_SET:
    case BLE_MESH_MODEL_OP_GEN_DELTA_SET_UNACK: {
        struct bt_mesh_gen_delta_set *set;
        set = (struct bt_mesh_gen_delta_set *)value;
        net_buf_simple_add_le32(msg, set->delta_level);
        net_buf_simple_add_u8(msg, set->tid);
        if (set->op_en) {
            net_buf_simple_add_u8(msg, set->trans_time);
            net_buf_simple_add_u8(msg, set->delay);
        }
        break;
    }

    case BLE_MESH_MODEL_OP_GEN_MOVE_SET:
    case BLE_MESH_MODEL_OP_GEN_MOVE_SET_UNACK: {
        struct bt_mesh_gen_move_set *set;
        set = (struct bt_mesh_gen_move_set *)value;
        net_buf_simple_add_le16(msg, set->delta_level);
        net_buf_simple_add_u8(msg,   set->tid);
        if (set->op_en) {
            net_buf_simple_add_u8(msg, set->trans_time);
            net_buf_simple_add_u8(msg, set->delay);
        }
        break;
    }

    case BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET:
    case BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET_UNACK: {
        struct bt_mesh_gen_def_trans_time_set *set;
        set = (struct bt_mesh_gen_def_trans_time_set *)value;
        net_buf_simple_add_u8(msg, set->trans_time);
        break;
    }

    case BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET:
    case BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET_UNACK: {
        struct bt_mesh_gen_onpowerup_set *set;
        set = (struct bt_mesh_gen_onpowerup_set *)value;
        net_buf_simple_add_u8(msg, set->onpowerup);
        break;
    }

    case BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET:
    case BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET_UNACK: {
        struct bt_mesh_gen_power_level_set *set;
        set = (struct bt_mesh_gen_power_level_set *)value;
        net_buf_simple_add_le16(msg, set->power);
        net_buf_simple_add_u8(msg,   set->tid);
        if (set->op_en) {
            net_buf_simple_add_u8(msg, set->trans_time);
            net_buf_simple_add_u8(msg, set->delay);
        }
        break;
    }

    case BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET:
    case BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET_UNACK: {
        struct bt_mesh_gen_power_default_set *set;
        set = (struct bt_mesh_gen_power_default_set *)value;
        net_buf_simple_add_le16(msg, set->power);
        break;
    }

    case BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET:
    case BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET_UNACK: {
        struct bt_mesh_gen_power_range_set *set;
        set = (struct bt_mesh_gen_power_range_set *)value;
        net_buf_simple_add_le16(msg, set->range_min);
        net_buf_simple_add_le16(msg, set->range_max);
        break;
    }

    case BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET:
    case BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET_UNACK: {
        struct bt_mesh_gen_loc_global_set *set;
        set = (struct bt_mesh_gen_loc_global_set *)value;
        net_buf_simple_add_le32(msg, set->global_latitude);
        net_buf_simple_add_le32(msg, set->global_longitude);
        net_buf_simple_add_le16(msg, set->global_altitude);
        break;
    }

    case BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET:
    case BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET_UNACK: {
        struct bt_mesh_gen_loc_local_set *set;
        set = (struct bt_mesh_gen_loc_local_set *)value;
        net_buf_simple_add_le16(msg, set->local_north);
        net_buf_simple_add_le16(msg, set->local_east);
        net_buf_simple_add_le16(msg, set->local_altitude);
        net_buf_simple_add_u8(msg,   set->floor_number);
        net_buf_simple_add_le16(msg, set->uncertainty);
        break;
    }

    case BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET:
    case BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET_UNACK: {
        struct bt_mesh_gen_user_property_set *set;
        set = (struct bt_mesh_gen_user_property_set *)value;
        net_buf_simple_add_le16(msg, set->user_property_id);
        net_buf_simple_add_mem(msg,  set->user_property_value->data, set->user_property_value->len);
        break;
    }

    case BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET:
    case BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET_UNACK: {
        struct bt_mesh_gen_admin_property_set *set;
        set = (struct bt_mesh_gen_admin_property_set *)value;
        net_buf_simple_add_le16(msg, set->admin_property_id);
        net_buf_simple_add_u8(msg, set->admin_user_access);
        net_buf_simple_add_mem(msg, set->admin_property_value->data, set->admin_property_value->len);
        break;
    }

    case BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET:
    case BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET_UNACK: {
        struct bt_mesh_gen_manu_property_set *set;
        set = (struct bt_mesh_gen_manu_property_set *)value;
        net_buf_simple_add_le16(msg, set->manu_property_id);
        net_buf_simple_add_u8(msg, set->manu_user_access);
        break;
    }

    default:
        BT_ERR("%s, Not a Generic Client Set message opcode", __func__);
        err = -EINVAL;
        goto end;
    }

    err = bt_mesh_client_send_msg(common->model, common->opcode, &common->ctx, msg,
                                  timeout_handler, common->msg_timeout, need_ack,
                                  common->cb, common->cb_data);
    if (err) {
        BT_ERR("%s, Failed to send Generic Set message (err %d)", __func__, err);
    }

end:
    bt_mesh_free_buf(msg);

    return err;
}

int bt_mesh_generic_client_get_state(bt_mesh_client_common_param_t *common, void *get, void *status)
{
    bt_mesh_generic_client_t *client = NULL;

    if (!common || !common->model) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    client = (bt_mesh_generic_client_t *)common->model->user_data;
    if (!client || !client->internal_data) {
        BT_ERR("%s, Generic Client user data is NULL", __func__);
        return -EINVAL;
    }

    switch (common->opcode) {
    case BLE_MESH_MODEL_OP_GEN_ONOFF_GET:
    case BLE_MESH_MODEL_OP_GEN_LEVEL_GET:
    case BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_GET:
    case BLE_MESH_MODEL_OP_GEN_ONPOWERUP_GET:
    case BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_GET:
    case BLE_MESH_MODEL_OP_GEN_POWER_LAST_GET:
    case BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_GET:
    case BLE_MESH_MODEL_OP_GEN_POWER_RANGE_GET:
    case BLE_MESH_MODEL_OP_GEN_BATTERY_GET:
    case BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_GET:
    case BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_GET:
    case BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_GET:
    case BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_GET:
    case BLE_MESH_MODEL_OP_GEN_MANU_PROPERTIES_GET:
        break;
    case BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_GET:
        if (!get) {
            BT_ERR("%s, Generic user_property_get is NULL", __func__);
            return -EINVAL;
        }
        break;
    case BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_GET:
        if (!get) {
            BT_ERR("%s, Generic admin_property_get is NULL", __func__);
            return -EINVAL;
        }
        break;
    case BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_GET:
        if (!get) {
            BT_ERR("%s, Generic manu_property_get is NULL", __func__);
            return -EINVAL;
        }
        break;
    case BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_GET:
        if (!get) {
            BT_ERR("%s, Generic client_properties_get is NULL", __func__);
            return -EINVAL;
        }
        break;
    default:
        BT_ERR("%s, Not a Generic Client Get message opcode", __func__);
        return -EINVAL;
    }

    return gen_get_state(common, get);
}

int bt_mesh_generic_client_set_state(bt_mesh_client_common_param_t *common, void *set, void *status)
{
    bt_mesh_generic_client_t *client = NULL;
    u16_t length   = 0;
    bool  need_ack = false;

    if (!common || !common->model || !set) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    client = (bt_mesh_generic_client_t *)common->model->user_data;
    if (!client || !client->internal_data) {
        BT_ERR("%s, Generic Client user data is NULL", __func__);
        return -EINVAL;
    }

    switch (common->opcode) {
    case BLE_MESH_MODEL_OP_GEN_ONOFF_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK: {
        struct bt_mesh_gen_onoff_set *value;
        value = (struct bt_mesh_gen_onoff_set *)set;
        if (value->op_en) {
            if ((value->trans_time & 0x3F) > 0x3E) {
                BT_ERR("%s, Invalid Generic OnOff Set transition time", __func__);
                return -EINVAL;
            }
        }
        length = BLE_MESH_GEN_ONOFF_SET_MSG_LEN;
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_LEVEL_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK: {
        struct bt_mesh_gen_level_set *value;
        value = (struct bt_mesh_gen_level_set *)set;
        if (value->op_en) {
            if ((value->trans_time & 0x3F) > 0x3E) {
                BT_ERR("%s, Invalid Generic Level Set transition time", __func__);
                return -EINVAL;
            }
        }
        length = BLE_MESH_GEN_LEVEL_SET_MSG_LEN;
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_DELTA_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_GEN_DELTA_SET_UNACK: {
        struct bt_mesh_gen_delta_set *value;
        value = (struct bt_mesh_gen_delta_set *)set;
        if (value->op_en) {
            if ((value->trans_time & 0x3F) > 0x3E) {
                BT_ERR("%s, Invalid Generic Delta Set transition time", __func__);
                return -EINVAL;
            }
        }
        length = BLE_MESH_GEN_DELTA_SET_MSG_LEN;
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_MOVE_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_GEN_MOVE_SET_UNACK: {
        struct bt_mesh_gen_move_set *value;
        value = (struct bt_mesh_gen_move_set *)set;
        if (value->op_en) {
            if ((value->trans_time & 0x3F) > 0x3E) {
                BT_ERR("%s, Invalid Generic Move Set transition time", __func__);
                return -EINVAL;
            }
        }
        length = BLE_MESH_GEN_MOVE_SET_MSG_LEN;
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET_UNACK: {
        u8_t value = *(u8_t *)set;
        if ((value & 0x3F) > 0x3E) {
            BT_ERR("%s, Invalid Generic Default Trans Time Set transition time", __func__);
            return -EINVAL;
        }
        length = BLE_MESH_GEN_DEF_TRANS_TIME_SET_MSG_LEN;
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET_UNACK:
        length = BLE_MESH_GEN_ONPOWERUP_SET_MSG_LEN;
        break;
    case BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET_UNACK: {
        struct bt_mesh_gen_power_level_set *value;
        value = (struct bt_mesh_gen_power_level_set *)set;
        if (value->op_en) {
            if ((value->trans_time & 0x3F) > 0x3E) {
                BT_ERR("%s, Invalid Generic Power Level Set transition time", __func__);
                return -EINVAL;
            }
        }
        length = BLE_MESH_GEN_POWER_LEVEL_SET_MSG_LEN;
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET_UNACK:
        length = BLE_MESH_GEN_POWER_DEFAULT_SET_MSG_LEN;
        break;
    case BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET_UNACK: {
        struct bt_mesh_gen_power_range_set *value;
        value = (struct bt_mesh_gen_power_range_set *)set;
        if (value->range_min > value->range_max) {
            BT_ERR("%s, Generic Power Level Set range min is greater than range max", __func__);
            return -EINVAL;
        }
        length = BLE_MESH_GEN_POWER_RANGE_SET_MSG_LEN;
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET_UNACK:
        length = BLE_MESH_GEN_LOC_GLOBAL_SET_MSG_LEN;
        break;
    case BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET_UNACK:
        length = BLE_MESH_GEN_LOC_LOCAL_SET_MSG_LEN;
        break;
    case BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET_UNACK: {
        struct bt_mesh_gen_user_property_set *value;
        value = (struct bt_mesh_gen_user_property_set *)set;
        if (!value->user_property_value) {
            BT_ERR("%s, Generic user_property_value is NULL", __func__);
            return -EINVAL;
        }
        length = (1 + 2 + value->user_property_value->len + 4);
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET_UNACK: {
        struct bt_mesh_gen_admin_property_set *value;
        value = (struct bt_mesh_gen_admin_property_set *)set;
        if (!value->admin_property_value) {
            BT_ERR("%s, Generic admin_property_value is NULL", __func__);
            return -EINVAL;
        }
        length = (1 + 2 + 1 + value->admin_property_value->len + 4);
        break;
    }
    case BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET_UNACK:
        length = BLE_MESH_GEN_MANU_PROPERTY_SET_MSG_LEN;
        break;
    default:
        BT_ERR("%s, Not a Generic Client Set message opcode", __func__);
        return -EINVAL;
    }

    return gen_set_state(common, set, length, need_ack);
}

static int generic_client_init(struct bt_mesh_model *model, bool primary)
{
    generic_internal_data_t *internal = NULL;
    bt_mesh_generic_client_t *client = NULL;

    BT_DBG("primary %u", primary);

    if (!model) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    client = (bt_mesh_generic_client_t *)model->user_data;
    if (!client) {
        BT_ERR("%s, Generic Client user_data is NULL", __func__);
        return -EINVAL;
    }

    /* TODO: call osi_free() when deinit function is invoked*/
    internal = osi_calloc(sizeof(generic_internal_data_t));
    if (!internal) {
        BT_ERR("%s, Failed to allocate memory", __func__);
        return -ENOMEM;
    }

    sys_slist_init(&internal->queue);

    client->model = model;
    client->op_pair_size = ARRAY_SIZE(gen_op_pair);
    client->op_pair = gen_op_pair;
    client->internal_data = internal;

    return 0;
}

int bt_mesh_gen_onoff_cli_init(struct bt_mesh_model *model, bool primary)
{
    return generic_client_init(model, primary);
}

int bt_mesh_gen_level_cli_init(struct bt_mesh_model *model, bool primary)
{
    return generic_client_init(model, primary);
}

int bt_mesh_gen_def_trans_time_cli_init(struct bt_mesh_model *model, bool primary)
{
    return generic_client_init(model, primary);
}

int bt_mesh_gen_pwr_onoff_cli_init(struct bt_mesh_model *model, bool primary)
{
    return generic_client_init(model, primary);
}

int bt_mesh_gen_pwr_level_cli_init(struct bt_mesh_model *model, bool primary)
{
    return generic_client_init(model, primary);
}

int bt_mesh_gen_battery_cli_init(struct bt_mesh_model *model, bool primary)
{
    return generic_client_init(model, primary);
}

int bt_mesh_gen_location_cli_init(struct bt_mesh_model *model, bool primary)
{
    return generic_client_init(model, primary);
}

int bt_mesh_gen_property_cli_init(struct bt_mesh_model *model, bool primary)
{
    return generic_client_init(model, primary);
}
