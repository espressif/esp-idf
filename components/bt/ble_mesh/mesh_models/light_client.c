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
#include <errno.h>
#include <stdbool.h>

#include "osi/allocator.h"

#include "mesh_types.h"
#include "mesh_kernel.h"
#include "mesh_trace.h"
#include "mesh.h"

#include "model_op.h"
#include "common.h"
#include "light_client.h"

#include "sdkconfig.h"

#include "btc_ble_mesh_light_client.h"

/** The following are the macro definitions of lighting client
 *  model messages length, and a message is composed of three
 *  parts: Opcode + msg_value + MIC
 */
/* Light lightness client messages length */
#define BT_MESH_LIGHT_LIGHTNESS_GET_MSG_LEN              (2 + 0 + 4)
#define BT_MESH_LIGHT_LIGHTNESS_SET_MSG_LEN              (2 + 5 + 4)
#define BT_MESH_LIGHT_LIGHTNESS_LINEAR_GET_MSG_LEN       (2 + 0 + 4)
#define BT_MESH_LIGHT_LIGHTNESS_LINEAR_SET_MSG_LEN       (2 + 5 + 4)
#define BT_MESH_LIGHT_LIGHTNESS_LAST_GET_MSG_LEN         (2 + 0 + 4)
#define BT_MESH_LIGHT_LIGHTNESS_DEFAULT_GET_MSG_LEN      (2 + 0 + 4)
#define BT_MESH_LIGHT_LIGHTNESS_DEFAULT_SET_MSG_LEN      (2 + 2 + 4)
#define BT_MESH_LIGHT_LIGHTNESS_RANGE_GET_MSG_LEN        (2 + 0 + 4)
#define BT_MESH_LIGHT_LIGHTNESS_RANGE_SET_MSG_LEN        (2 + 4 + 4)

/* Light CTL client messages length */
#define BT_MESH_LIGHT_CTL_GET_MSG_LEN                    (2 + 0 + 4)
#define BT_MESH_LIGHT_CTL_SET_MSG_LEN                    (2 + 9 + 4)
#define BT_MESH_LIGHT_CTL_TEMPERATURE_GET_MSG_LEN        (2 + 0 + 4)
#define BT_MESH_LIGHT_CTL_TEMPERATURE_SET_MSG_LEN        (2 + 7 + 4)
#define BT_MESH_LIGHT_CTL_TEMPERATURE_RANGE_GET_MSG_LEN  (2 + 0 + 4)
#define BT_MESH_LIGHT_CTL_TEMPERATURE_RANGE_SET_MSG_LEN  (2 + 4 + 4)
#define BT_MESH_LIGHT_CTL_DEFAULT_GET_MSG_LEN            (2 + 0 + 4)
#define BT_MESH_LIGHT_CTL_DEFAULT_SET_MSG_LEN            (2 + 6 + 4)

/* Light HSL client messages length */
#define BT_MESH_LIGHT_HSL_GET_MSG_LEN                    (2 + 0 + 4)
#define BT_MESH_LIGHT_HSL_SET_MSG_LEN                    (2 + 9 + 4)
#define BT_MESH_LIGHT_HSL_TARGET_GET_MSG_LEN             (2 + 0 + 4)
#define BT_MESH_LIGHT_HSL_HUE_GET_MSG_LEN                (2 + 0 + 4)
#define BT_MESH_LIGHT_HSL_HUE_SET_MSG_LEN                (2 + 5 + 4)
#define BT_MESH_LIGHT_HSL_SATURATION_GET_MSG_LEN         (2 + 0 + 4)
#define BT_MESH_LIGHT_HSL_SATURATION_SET_MSG_LEN         (2 + 5 + 4)
#define BT_MESH_LIGHT_HSL_DEFAULT_GET_MSG_LEN            (2 + 0 + 4)
#define BT_MESH_LIGHT_HSL_DEFAULT_SET_MSG_LEN            (2 + 6 + 4)
#define BT_MESH_LIGHT_HSL_RANGE_GET_MSG_LEN              (2 + 0 + 4)
#define BT_MESH_LIGHT_HSL_RANGE_SET_MSG_LEN              (2 + 8 + 4)

#define BT_MESH_LIGHT_GET_STATE_MSG_LEN                  (2 + 0 + 4)

/* Light xyL client messages length */

/* Light LC client messages length */

static const bt_mesh_client_op_pair_t light_op_pair[] = {
    { BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET,             BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS             },
    { BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET,             BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS             },
    { BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_GET,      BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS      },
    { BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET,      BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS      },
    { BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_GET,        BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_STATUS        },
    { BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_GET,     BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS     },
    { BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET,     BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS     },
    { BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_GET,       BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS       },
    { BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET,       BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS       },
    { BT_MESH_MODEL_OP_LIGHT_CTL_GET,                   BT_MESH_MODEL_OP_LIGHT_CTL_STATUS                   },
    { BT_MESH_MODEL_OP_LIGHT_CTL_SET,                   BT_MESH_MODEL_OP_LIGHT_CTL_STATUS                   },
    { BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_GET,       BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS       },
    { BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET,       BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS       },
    { BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_GET, BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_STATUS },
    { BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET, BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_STATUS },
    { BT_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_GET,           BT_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_STATUS           },
    { BT_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET,           BT_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_STATUS           },
    { BT_MESH_MODEL_OP_LIGHT_HSL_GET,                   BT_MESH_MODEL_OP_LIGHT_HSL_STATUS                   },
    { BT_MESH_MODEL_OP_LIGHT_HSL_SET,                   BT_MESH_MODEL_OP_LIGHT_HSL_STATUS                   },
    { BT_MESH_MODEL_OP_LIGHT_HSL_TARGET_GET,            BT_MESH_MODEL_OP_LIGHT_HSL_TARGET_STATUS            },
    { BT_MESH_MODEL_OP_LIGHT_HSL_HUE_GET,               BT_MESH_MODEL_OP_LIGHT_HSL_HUE_STATUS               },
    { BT_MESH_MODEL_OP_LIGHT_HSL_HUE_SET,               BT_MESH_MODEL_OP_LIGHT_HSL_HUE_STATUS               },
    { BT_MESH_MODEL_OP_LIGHT_HSL_SATURATION_GET,        BT_MESH_MODEL_OP_LIGHT_HSL_SATURATION_STATUS        },
    { BT_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET,        BT_MESH_MODEL_OP_LIGHT_HSL_SATURATION_STATUS        },
    { BT_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_GET,           BT_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_STATUS           },
    { BT_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET,           BT_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_STATUS           },
    { BT_MESH_MODEL_OP_LIGHT_HSL_RANGE_GET,             BT_MESH_MODEL_OP_LIGHT_HSL_RANGE_STATUS             },
    { BT_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET,             BT_MESH_MODEL_OP_LIGHT_HSL_RANGE_STATUS             },
};

static void timeout_handler(struct k_work *work)
{
    bt_mesh_light_client_t *client   = NULL;
    light_internal_data_t  *internal = NULL;
    bt_mesh_client_node_t  *node     = NULL;

    BT_WARN("Receive light status message timeout");

    node = CONTAINER_OF(work, bt_mesh_client_node_t, timer.work);
    if (!node || !node->ctx.model) {
        BT_ERR("%s: node parameter is NULL", __func__);
        return;
    }

    client = (bt_mesh_light_client_t *)node->ctx.model->user_data;
    if (!client) {
        BT_ERR("%s: model user_data is NULL", __func__);
        return;
    }

    internal = (light_internal_data_t *)client->internal_data;
    if (!internal) {
        BT_ERR("%s: internal_data is NULL", __func__);
        return;
    }

    bt_mesh_callback_light_status_to_btc(node->opcode, 0x03, node->ctx.model,
                                         &node->ctx, NULL, 0);

    sys_slist_find_and_remove(&internal->queue, &node->client_node);
    osi_free(node);

    return;
}

static void light_status(struct bt_mesh_model *model,
                         struct bt_mesh_msg_ctx *ctx,
                         struct net_buf_simple *buf)
{
    bt_mesh_light_client_t *client   = NULL;
    light_internal_data_t  *internal = NULL;
    bt_mesh_client_node_t  *node     = NULL;
    u8_t  *val = NULL;
    u8_t   evt = 0xFF;
    u32_t  rsp = 0;
    size_t len = 0;

    BT_DBG("%s: len %d, bytes %s", __func__, buf->len, bt_hex(buf->data, buf->len));

    client = (bt_mesh_light_client_t *)model->user_data;
    if (!client) {
        BT_ERR("%s: model user_data is NULL", __func__);
        return;
    }

    internal = (light_internal_data_t *)client->internal_data;
    if (!internal) {
        BT_ERR("%s: model internal_data is NULL", __func__);
        return;
    }

    rsp = ctx->recv_op;

    switch (rsp) {
    case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS: {
        struct bt_mesh_light_lightness_status *status = NULL;
        if (buf->len != 2 && buf->len != 5) {
            BT_ERR("Wrong Light lightness status length %d", buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_light_lightness_status));
        if (!status) {
            BT_ERR("%s: allocate memory fail", __func__);
            return;
        }
        status->present_lightness    = net_buf_simple_pull_le16(buf);
        if (buf->len) {
            status->op_en            = true;
            status->target_lightness = net_buf_simple_pull_le16(buf);
            status->remain_time      = net_buf_simple_pull_u8(buf);
        }
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_light_lightness_status);
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS: {
        struct bt_mesh_light_lightness_linear_status *status = NULL;
        if (buf->len != 2 && buf->len != 5) {
            BT_ERR("Wrong Light lightness status length %d", buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_light_lightness_linear_status));
        if (!status) {
            BT_ERR("%s: allocate memory fail", __func__);
            return;
        }
        status->present_lightness    = net_buf_simple_pull_le16(buf);
        if (buf->len) {
            status->op_en            = true;
            status->target_lightness = net_buf_simple_pull_le16(buf);
            status->remain_time      = net_buf_simple_pull_u8(buf);
        }
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_light_lightness_linear_status);
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_STATUS: {
        struct bt_mesh_light_lightness_last_status *status = NULL;
        if (buf->len != 2) {
            BT_ERR("Wrong Light last status length %d", buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_light_lightness_last_status));
        if (!status) {
            BT_ERR("%s: allocate memory fail", __func__);
            return;
        }
        status->lightness = net_buf_simple_pull_le16(buf);
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_light_lightness_last_status);
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS: {
        struct bt_mesh_light_lightness_default_status *status = NULL;
        if (buf->len != 2) {
            BT_ERR("Wrong Light default status length %d", buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_light_lightness_default_status));
        if (!status) {
            BT_ERR("%s: allocate memory fail", __func__);
            return;
        }
        status->lightness = net_buf_simple_pull_le16(buf);
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_light_lightness_default_status);
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS: {
        struct bt_mesh_light_lightness_range_status *status = NULL;
        if (buf->len != 5) {
            BT_ERR("Wrong Light last status length %d", buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_light_lightness_range_status));
        if (!status) {
            BT_ERR("%s: allocate memory fail", __func__);
            return;
        }
        status->status_code = net_buf_simple_pull_u8(buf);
        status->range_min   = net_buf_simple_pull_le16(buf);
        status->range_max   = net_buf_simple_pull_le16(buf);
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_light_lightness_range_status);
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_CTL_STATUS: {
        struct bt_mesh_light_ctl_status *status = NULL;
        if (buf->len != 4 && buf->len != 9) {
            BT_ERR("Wrong Light CTL status length %d", buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_light_ctl_status));
        if (!status) {
            BT_ERR("%s: allocate memory fail", __func__);
            return;
        }
        status->present_ctl_lightness      = net_buf_simple_pull_le16(buf);
        status->present_ctl_temperature    = net_buf_simple_pull_le16(buf);
        if (buf->len) {
            status->op_en                  = true;
            status->target_ctl_lightness   = net_buf_simple_pull_le16(buf);
            status->target_ctl_temperature = net_buf_simple_pull_le16(buf);
            status->remain_time            = net_buf_simple_pull_u8(buf);
        }
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_light_ctl_status);
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS: {
        struct bt_mesh_light_ctl_temperature_status *status = NULL;
        if (buf->len != 4 && buf->len != 9) {
            BT_ERR("Wrong Light CTL temperature status length %d", buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_light_ctl_temperature_status));
        if (!status) {
            BT_ERR("%s: allocate memory fail", __func__);
            return;
        }
        status->present_ctl_temperature = net_buf_simple_pull_le16(buf);
        status->present_ctl_delta_uv    = net_buf_simple_pull_le16(buf);
        if (buf->len) {
            status->op_en                  = true;
            status->target_ctl_temperature = net_buf_simple_pull_le16(buf);
            status->target_ctl_delta_uv    = net_buf_simple_pull_le16(buf);
            status->remain_time            = net_buf_simple_pull_u8(buf);
        }
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_light_ctl_temperature_status);
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_STATUS: {
        struct bt_mesh_light_ctl_temperature_range_status *status = NULL;
        if (buf->len != 5) {
            BT_ERR("Wrong Light CTL temperature range status length %d", buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_light_ctl_temperature_range_status));
        if (!status) {
            BT_ERR("%s: allocate memory fail", __func__);
            return;
        }
        status->status_code = net_buf_simple_pull_u8(buf);
        status->range_min   = net_buf_simple_pull_le16(buf);
        status->range_max   = net_buf_simple_pull_le16(buf);
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_light_ctl_temperature_range_status);
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_STATUS: {
        struct bt_mesh_light_ctl_default_status *status = NULL;
        if (buf->len != 6) {
            BT_ERR("Wrong Light CTL default status length %d", buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_light_ctl_default_status));
        if (!status) {
            BT_ERR("%s: allocate memory fail", __func__);
            return;
        }
        status->lightness   = net_buf_simple_pull_le16(buf);
        status->temperature = net_buf_simple_pull_le16(buf);
        status->delta_uv    = net_buf_simple_pull_le16(buf);
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_light_ctl_default_status);
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_HSL_STATUS: {
        struct bt_mesh_light_hsl_status *status = NULL;
        if (buf->len != 6 && buf->len != 7) {
            BT_ERR("Wrong Light HSL status length %d", buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_light_hsl_status));
        if (!status) {
            BT_ERR("%s: allocate memory fail", __func__);
            return;
        }
        status->hsl_lightness  = net_buf_simple_pull_le16(buf);
        status->hsl_hue        = net_buf_simple_pull_le16(buf);
        status->hsl_saturation = net_buf_simple_pull_le16(buf);
        if (buf->len) {
            status->op_en       = true;
            status->remain_time = net_buf_simple_pull_u8(buf);
        }
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_light_hsl_status);
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_HSL_TARGET_STATUS: {
        struct bt_mesh_light_hsl_target_status *status = NULL;
        if (buf->len != 6 && buf->len != 7) {
            BT_ERR("Wrong Light HSL target status length %d", buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_light_hsl_target_status));
        if (!status) {
            BT_ERR("%s: allocate memory fail", __func__);
            return;
        }
        status->hsl_lightness_target  = net_buf_simple_pull_le16(buf);
        status->hsl_hue_target        = net_buf_simple_pull_le16(buf);
        status->hsl_saturation_target = net_buf_simple_pull_le16(buf);
        if (buf->len) {
            status->op_en       = true;
            status->remain_time = net_buf_simple_pull_u8(buf);
        }
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_light_hsl_target_status);
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_HSL_HUE_STATUS: {
        struct bt_mesh_light_hsl_hue_status *status = NULL;
        if (buf->len != 2 && buf->len != 5) {
            BT_ERR("Wrong Light HSL Hue status length %d", buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_light_hsl_hue_status));
        if (!status) {
            BT_ERR("%s: allocate memory fail", __func__);
            return;
        }
        status->present_hue = net_buf_simple_pull_le16(buf);
        if (buf->len) {
            status->op_en       = true;
            status->target_hue  = net_buf_simple_pull_le16(buf);
            status->remain_time = net_buf_simple_pull_u8(buf);
        }
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_light_hsl_hue_status);
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_HSL_SATURATION_STATUS: {
        struct bt_mesh_light_hsl_saturation_status *status = NULL;
        if (buf->len != 2 && buf->len != 5) {
            BT_ERR("Wrong Light HSL saturation status length %d", buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_light_hsl_saturation_status));
        if (!status) {
            BT_ERR("%s: allocate memory fail", __func__);
            return;
        }
        status->present_saturation = net_buf_simple_pull_le16(buf);
        if (buf->len) {
            status->op_en             = true;
            status->target_saturation = net_buf_simple_pull_le16(buf);
            status->remain_time       = net_buf_simple_pull_u8(buf);
        }
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_light_hsl_saturation_status);
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_STATUS: {
        struct bt_mesh_light_hsl_default_status *status = NULL;
        if (buf->len != 6) {
            BT_ERR("Wrong Light HSL default status length %d", buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_light_hsl_default_status));
        if (!status) {
            BT_ERR("%s: allocate memory fail", __func__);
            return;
        }
        status->lightness  = net_buf_simple_pull_le16(buf);
        status->hue        = net_buf_simple_pull_le16(buf);
        status->saturation = net_buf_simple_pull_le16(buf);
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_light_hsl_default_status);
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_HSL_RANGE_STATUS: {
        struct bt_mesh_light_hsl_range_status *status = NULL;
        if (buf->len != 9) {
            BT_ERR("Wrong Light HSL range status length %d", buf->len);
            return;
        }
        status = osi_calloc(sizeof(struct bt_mesh_light_hsl_range_status));
        if (!status) {
            BT_ERR("%s: allocate memory fail", __func__);
            return;
        }
        status->status_code          = net_buf_simple_pull_u8(buf);
        status->hue_range_min        = net_buf_simple_pull_le16(buf);
        status->hue_range_max        = net_buf_simple_pull_le16(buf);
        status->saturation_range_min = net_buf_simple_pull_le16(buf);
        status->saturation_range_max = net_buf_simple_pull_le16(buf);
        val = (u8_t *)status;
        len = sizeof(struct bt_mesh_light_hsl_range_status);
        break;
    }
    default:
        BT_ERR("Not a lighting status message opcode");
        return;
    }

    buf->data = val;
    buf->len  = len;
    node = bt_mesh_is_model_message_publish(model, ctx, buf, true);
    if (!node) {
        BT_DBG("Unexpected light status message 0x%x", rsp);
    } else {
        switch (node->opcode) {
        case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET:
        case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_GET:
        case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_GET:
        case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_GET:
        case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_GET:
        case BT_MESH_MODEL_OP_LIGHT_CTL_GET:
        case BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_GET:
        case BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_GET:
        case BT_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_GET:
        case BT_MESH_MODEL_OP_LIGHT_HSL_GET:
        case BT_MESH_MODEL_OP_LIGHT_HSL_TARGET_GET:
        case BT_MESH_MODEL_OP_LIGHT_HSL_HUE_GET:
        case BT_MESH_MODEL_OP_LIGHT_HSL_SATURATION_GET:
        case BT_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_GET:
        case BT_MESH_MODEL_OP_LIGHT_HSL_RANGE_GET:
            evt = 0x00;
            break;
        case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET:
        case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET:
        case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET:
        case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET:
        case BT_MESH_MODEL_OP_LIGHT_CTL_SET:
        case BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET:
        case BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET:
        case BT_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET:
        case BT_MESH_MODEL_OP_LIGHT_HSL_SET:
        case BT_MESH_MODEL_OP_LIGHT_HSL_HUE_SET:
        case BT_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET:
        case BT_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET:
        case BT_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET:
            evt = 0x01;
            break;
        default:
            break;
        }

        bt_mesh_callback_light_status_to_btc(node->opcode, evt, model, ctx, val, len);
        // Don't forget to release the node at the end.
        bt_mesh_client_free_node(&internal->queue, node);
    }

    osi_free(val);

    return;
}

const struct bt_mesh_model_op light_lightness_cli_op[] = {
    { BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS,         2, light_status },
    { BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS,  2, light_status },
    { BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_STATUS,    2, light_status },
    { BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS, 2, light_status },
    { BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS,   5, light_status },
    BT_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op light_ctl_cli_op[] = {
    { BT_MESH_MODEL_OP_LIGHT_CTL_STATUS,                   4, light_status },
    { BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS,       4, light_status },
    { BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_STATUS, 5, light_status },
    { BT_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_STATUS,           6, light_status },
    BT_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op light_hsl_cli_op[] = {
    { BT_MESH_MODEL_OP_LIGHT_HSL_STATUS,            6, light_status },
    { BT_MESH_MODEL_OP_LIGHT_HSL_TARGET_STATUS,     6, light_status },
    { BT_MESH_MODEL_OP_LIGHT_HSL_HUE_STATUS,        2, light_status },
    { BT_MESH_MODEL_OP_LIGHT_HSL_SATURATION_STATUS, 2, light_status },
    { BT_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_STATUS,    6, light_status },
    { BT_MESH_MODEL_OP_LIGHT_HSL_RANGE_STATUS,      9, light_status },
    BT_MESH_MODEL_OP_END,
};

static int light_get_state(struct bt_mesh_common_param *common, void *value)
{
    struct net_buf_simple *msg = NET_BUF_SIMPLE(BT_MESH_LIGHT_GET_STATE_MSG_LEN);
    int err;

    bt_mesh_model_msg_init(msg, common->opcode);

    if (value) {
        /* TODO: if other light client model get messages contain parameters */
    }

    err = bt_mesh_client_send_msg(common->model, common->opcode, &common->ctx, msg,
                                  timeout_handler, common->msg_timeout, true,
                                  common->cb, common->cb_data);
    if (err) {
        BT_ERR("Light get message send failed (err %d)", err);
    }

    return err;
}

static int light_set_state(struct bt_mesh_common_param *common,
                           void *value, u16_t value_len, bool need_ack)
{
    struct net_buf_simple *msg = NULL;
    int err;

    msg = bt_mesh_alloc_buf(value_len);
    if (!msg) {
        BT_ERR("Light set allocate memory fail");
        return -ENOMEM;
    }

    bt_mesh_model_msg_init(msg, common->opcode);

    switch (common->opcode) {
    case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET:
    case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK: {
        struct bt_mesh_light_lightness_set *set;
        set = (struct bt_mesh_light_lightness_set *)value;
        net_buf_simple_add_le16(msg, set->lightness);
        net_buf_simple_add_u8(msg,   set->tid);
        if (set->op_en) {
            net_buf_simple_add_u8(msg, set->trans_time);
            net_buf_simple_add_u8(msg, set->delay);
        }
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET:
    case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET_UNACK: {
        struct bt_mesh_light_lightness_linear_set *set;
        set = (struct bt_mesh_light_lightness_linear_set *)value;
        net_buf_simple_add_le16(msg, set->lightness);
        net_buf_simple_add_u8(msg,   set->tid);
        if (set->op_en) {
            net_buf_simple_add_u8(msg, set->trans_time);
            net_buf_simple_add_u8(msg, set->delay);
        }
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET:
    case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET_UNACK: {
        struct bt_mesh_light_lightness_default_set *set;
        set = (struct bt_mesh_light_lightness_default_set *)value;
        net_buf_simple_add_le16(msg, set->lightness);
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET:
    case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET_UNACK: {
        struct bt_mesh_light_lightness_range_set *set;
        set = (struct bt_mesh_light_lightness_range_set *)value;
        net_buf_simple_add_le16(msg, set->range_min);
        net_buf_simple_add_le16(msg, set->range_max);
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_CTL_SET:
    case BT_MESH_MODEL_OP_LIGHT_CTL_SET_UNACK: {
        struct bt_mesh_light_ctl_set *set;
        set = (struct bt_mesh_light_ctl_set *)value;
        net_buf_simple_add_le16(msg, set->ctl_lightness);
        net_buf_simple_add_le16(msg, set->ctl_temperature);
        net_buf_simple_add_le16(msg, set->ctl_delta_uv);
        net_buf_simple_add_u8(msg,   set->tid);
        if (set->op_en) {
            net_buf_simple_add_u8(msg, set->trans_time);
            net_buf_simple_add_u8(msg, set->delay);
        }
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET:
    case BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET_UNACK: {
        struct bt_mesh_light_ctl_temperature_set *set;
        set = (struct bt_mesh_light_ctl_temperature_set *)value;
        net_buf_simple_add_le16(msg, set->ctl_temperature);
        net_buf_simple_add_le16(msg, set->ctl_delta_uv);
        net_buf_simple_add_u8(msg,   set->tid);
        if (set->op_en) {
            net_buf_simple_add_u8(msg, set->trans_time);
            net_buf_simple_add_u8(msg, set->delay);
        }
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET:
    case BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACK: {
        struct bt_mesh_light_ctl_temperature_range_set *set;
        set = (struct bt_mesh_light_ctl_temperature_range_set *)value;
        net_buf_simple_add_le16(msg, set->range_min);
        net_buf_simple_add_le16(msg, set->range_max);
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET:
    case BT_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET_UNACK: {
        struct bt_mesh_light_ctl_default_set *set;
        set = (struct bt_mesh_light_ctl_default_set *)value;
        net_buf_simple_add_le16(msg, set->lightness);
        net_buf_simple_add_le16(msg, set->temperature);
        net_buf_simple_add_le16(msg, set->delta_uv);
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_HSL_SET:
    case BT_MESH_MODEL_OP_LIGHT_HSL_SET_UNACK: {
        struct bt_mesh_light_hsl_set *set;
        set = (struct bt_mesh_light_hsl_set *)value;
        net_buf_simple_add_le16(msg, set->hsl_lightness);
        net_buf_simple_add_le16(msg, set->hsl_hue);
        net_buf_simple_add_le16(msg, set->hsl_saturation);
        net_buf_simple_add_u8(msg,   set->tid);
        if (set->op_en) {
            net_buf_simple_add_u8(msg, set->trans_time);
            net_buf_simple_add_u8(msg, set->delay);
        }
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_HSL_HUE_SET:
    case BT_MESH_MODEL_OP_LIGHT_HSL_HUE_SET_UNACK: {
        struct bt_mesh_light_hsl_hue_set *set;
        set = (struct bt_mesh_light_hsl_hue_set *)value;
        net_buf_simple_add_le16(msg, set->hue);
        net_buf_simple_add_u8(msg,   set->tid);
        if (set->op_en) {
            net_buf_simple_add_u8(msg, set->trans_time);
            net_buf_simple_add_u8(msg, set->delay);
        }
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET:
    case BT_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET_UNACK: {
        struct bt_mesh_light_hsl_saturation_set *set;
        set = (struct bt_mesh_light_hsl_saturation_set *)value;
        net_buf_simple_add_le16(msg, set->saturation);
        net_buf_simple_add_u8(msg,   set->tid);
        if (set->op_en) {
            net_buf_simple_add_u8(msg, set->trans_time);
            net_buf_simple_add_u8(msg, set->delay);
        }
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET:
    case BT_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET_UNACK: {
        struct bt_mesh_light_hsl_default_set *set;
        set = (struct bt_mesh_light_hsl_default_set *)value;
        net_buf_simple_add_le16(msg, set->lightness);
        net_buf_simple_add_le16(msg, set->hue);
        net_buf_simple_add_le16(msg, set->saturation);
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET:
    case BT_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET_UNACK: {
        struct bt_mesh_light_hsl_range_set *set;
        set = (struct bt_mesh_light_hsl_range_set *)value;
        net_buf_simple_add_le16(msg, set->hue_range_min);
        net_buf_simple_add_le16(msg, set->hue_range_max);
        net_buf_simple_add_le16(msg, set->saturation_range_min);
        net_buf_simple_add_le16(msg, set->saturation_range_max);
        break;
    }
    default:
        BT_ERR("Not a lighting client model set message opcode");
        err = -EINVAL;
        goto end;
    }

    err = bt_mesh_client_send_msg(common->model, common->opcode, &common->ctx, msg,
                                  timeout_handler, common->msg_timeout, need_ack,
                                  common->cb, common->cb_data);
    if (err) {
        BT_ERR("Light set message send failed (err %d)", err);
    }

end:
    bt_mesh_free_buf(msg);

    return err;
}

int bt_mesh_light_client_get_state(struct bt_mesh_common_param *common, void *get, void *status)
{
    bt_mesh_light_client_t *client = NULL;

    if (!common || !common->model) {
        BT_ERR("%s: common parameter is NULL", __func__);
        return -EINVAL;
    }

    client = (bt_mesh_light_client_t *)common->model->user_data;
    if (!client || !client->internal_data) {
        BT_ERR("%s: client parameter is NULL", __func__);
        return -EINVAL;
    }

    switch (common->opcode) {
    case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET:
    case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_GET:
    case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_GET:
    case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_GET:
    case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_GET:
    case BT_MESH_MODEL_OP_LIGHT_CTL_GET:
    case BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_GET:
    case BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_GET:
    case BT_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_GET:
    case BT_MESH_MODEL_OP_LIGHT_HSL_GET:
    case BT_MESH_MODEL_OP_LIGHT_HSL_TARGET_GET:
    case BT_MESH_MODEL_OP_LIGHT_HSL_HUE_GET:
    case BT_MESH_MODEL_OP_LIGHT_HSL_SATURATION_GET:
    case BT_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_GET:
    case BT_MESH_MODEL_OP_LIGHT_HSL_RANGE_GET:
        break;
    default:
        BT_ERR("Not a lighting get message opcode");
        return -EINVAL;
    }

    return light_get_state(common, get);
}

int bt_mesh_light_client_set_state(struct bt_mesh_common_param *common, void *set, void *status)
{
    bt_mesh_light_client_t *client = NULL;
    u16_t length   = 0;
    bool  need_ack = false;

    if (!common || !common->model || !set) {
        BT_ERR("%s: common parameter is NULL", __func__);
        return -EINVAL;
    }

    client = (bt_mesh_light_client_t *)common->model->user_data;
    if (!client || !client->internal_data) {
        BT_ERR("%s: client parameter is NULL", __func__);
        return -EINVAL;
    }

    switch (common->opcode) {
    case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET:
        need_ack = true;
    case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK: {
        struct bt_mesh_light_lightness_set *value;
        value = (struct bt_mesh_light_lightness_set *)set;
        if (value->op_en) {
            if ((value->trans_time & 0x3F) > 0x3E) {
                BT_ERR("Light lightness set transition time is bigger than 0x3E");
                return -EINVAL;
            }
        }
        length = BT_MESH_LIGHT_LIGHTNESS_SET_MSG_LEN;
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET:
        need_ack = true;
    case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET_UNACK: {
        struct bt_mesh_light_lightness_linear_set *value;
        value = (struct bt_mesh_light_lightness_linear_set *)set;
        if (value->op_en) {
            if ((value->trans_time & 0x3F) > 0x3E) {
                BT_ERR("Light lightness linear set transition time is bigger than 0x3E");
                return -EINVAL;
            }
        }
        length = BT_MESH_LIGHT_LIGHTNESS_LINEAR_SET_MSG_LEN;
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET:
        need_ack = true;
    case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET_UNACK:
        length = BT_MESH_LIGHT_LIGHTNESS_DEFAULT_SET_MSG_LEN;
        break;
    case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET:
        need_ack = true;
    case BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET_UNACK: {
        struct bt_mesh_light_lightness_range_set *value;
        value = (struct bt_mesh_light_lightness_range_set *)set;
        if (value->range_min > value->range_max) {
            BT_ERR("Light lightness range set range min is greater than range max");
            return -EINVAL;
        }
        length = BT_MESH_LIGHT_LIGHTNESS_RANGE_SET_MSG_LEN;
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_CTL_SET:
        need_ack = true;
    case BT_MESH_MODEL_OP_LIGHT_CTL_SET_UNACK: {
        struct bt_mesh_light_ctl_set *value;
        value = (struct bt_mesh_light_ctl_set *)set;
        if (value->op_en) {
            if ((value->trans_time & 0x3F) > 0x3E) {
                BT_ERR("Light ctl set transition time is bigger than 0x3E");
                return -EINVAL;
            }
        }
        length = BT_MESH_LIGHT_CTL_SET_MSG_LEN;
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET:
        need_ack = true;
    case BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET_UNACK: {
        struct bt_mesh_light_ctl_temperature_set *value;
        value = (struct bt_mesh_light_ctl_temperature_set *)set;
        if (value->op_en) {
            if ((value->trans_time & 0x3F) > 0x3E) {
                BT_ERR("Light ctl temperature set transition time is bigger than 0x3E");
                return -EINVAL;
            }
        }
        length = BT_MESH_LIGHT_CTL_TEMPERATURE_SET_MSG_LEN;
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET:
        need_ack = true;
    case BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACK: {
        struct bt_mesh_light_ctl_temperature_range_set *value;
        value = (struct bt_mesh_light_ctl_temperature_range_set *)set;
        if (value->range_min > value->range_max) {
            BT_ERR("Light ctl temperature range set range min is greater than range max");
            return -EINVAL;
        }
        length = BT_MESH_LIGHT_CTL_TEMPERATURE_RANGE_SET_MSG_LEN;
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET:
        need_ack = true;
    case BT_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET_UNACK:
        length = BT_MESH_LIGHT_CTL_DEFAULT_SET_MSG_LEN;
        break;
    case BT_MESH_MODEL_OP_LIGHT_HSL_SET:
        need_ack = true;
    case BT_MESH_MODEL_OP_LIGHT_HSL_SET_UNACK: {
        struct bt_mesh_light_hsl_set *value;
        value = (struct bt_mesh_light_hsl_set *)set;
        if (value->op_en) {
            if ((value->trans_time & 0x3F) > 0x3E) {
                BT_ERR("Light hsl set transition time is bigger than 0x3E");
                return -EINVAL;
            }
        }
        length = BT_MESH_LIGHT_HSL_SET_MSG_LEN;
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_HSL_HUE_SET:
        need_ack = true;
    case BT_MESH_MODEL_OP_LIGHT_HSL_HUE_SET_UNACK: {
        struct bt_mesh_light_hsl_hue_set *value;
        value = (struct bt_mesh_light_hsl_hue_set *)set;
        if (value->op_en) {
            if ((value->trans_time & 0x3F) > 0x3E) {
                BT_ERR("Light hsl hue set transition time is bigger than 0x3E");
                return -EINVAL;
            }
        }
        length = BT_MESH_LIGHT_HSL_HUE_SET_MSG_LEN;
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET:
        need_ack = true;
    case BT_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET_UNACK: {
        struct bt_mesh_light_hsl_saturation_set *value;
        value = (struct bt_mesh_light_hsl_saturation_set *)set;
        if (value->op_en) {
            if ((value->trans_time & 0x3F) > 0x3E) {
                BT_ERR("Light hsl saturation set transition time is bigger than 0x3E");
                return -EINVAL;
            }
        }
        length = BT_MESH_LIGHT_HSL_SATURATION_SET_MSG_LEN;
        break;
    }
    case BT_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET:
        need_ack = true;
    case BT_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET_UNACK:
        length = BT_MESH_LIGHT_HSL_DEFAULT_SET_MSG_LEN;
        break;
    case BT_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET:
        need_ack = true;
    case BT_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET_UNACK: {
        struct bt_mesh_light_hsl_range_set *value;
        value = (struct bt_mesh_light_hsl_range_set *)set;
        if (value->hue_range_min > value->hue_range_max ||
                value->saturation_range_min > value->saturation_range_max) {
            BT_ERR("Light hsl range set range min is greater than range max");
            return -EINVAL;
        }
        length = BT_MESH_LIGHT_HSL_RANGE_SET_MSG_LEN;
        break;
    }
    default:
        BT_ERR("Not a lighting set message opcode");
        return -EINVAL;
    }

    return light_set_state(common, set, length, need_ack);
}

static int light_client_init(struct bt_mesh_model *model, bool primary)
{
    bt_mesh_light_client_t *client   = NULL;
    light_internal_data_t  *internal = NULL;

    BT_DBG("primary %u", primary);

    if (!model) {
        BT_ERR("Light client model is NULL");
        return -EINVAL;
    }

    client = (bt_mesh_light_client_t *)model->user_data;
    if (!client) {
        BT_ERR("Light client model user_data is NULL");
        return -EINVAL;
    }

    /* TODO: call osi_free() when deinit function is invoked*/
    internal = osi_calloc(sizeof(light_internal_data_t));
    if (!internal) {
        BT_ERR("Allocate memory for generic onoff internal data fail");
        return -ENOMEM;
    }

    sys_slist_init(&internal->queue);

    client->model         = model;
    client->op_pair_size  = ARRAY_SIZE(light_op_pair);
    client->op_pair       = light_op_pair;
    client->internal_data = internal;

    return 0;
}

int bt_mesh_light_lightness_cli_init(struct bt_mesh_model *model, bool primary)
{
    return light_client_init(model, primary);
}

int bt_mesh_light_ctl_cli_init(struct bt_mesh_model *model, bool primary)
{
    return light_client_init(model, primary);
}

int bt_mesh_light_hsl_cli_init(struct bt_mesh_model *model, bool primary)
{
    return light_client_init(model, primary);
}