/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "btc_ble_mesh_lighting_model.h"

#include "mesh/config.h"
#include "mesh/model_opcode.h"

#if CONFIG_BLE_MESH_LIGHTING_CLIENT
#include "mesh/lighting_client.h"

/* The followings are the macro definitions of Lighting client
 * model message length, and a message is composed of 3 parts:
 * Opcode + Payload + MIC
 */
/* Light lightness client messages length */
#define BLE_MESH_LIGHT_LIGHTNESS_GET_MSG_LEN              (2 + 0 + 4)
#define BLE_MESH_LIGHT_LIGHTNESS_SET_MSG_LEN              (2 + 5 + 4)
#define BLE_MESH_LIGHT_LIGHTNESS_LINEAR_GET_MSG_LEN       (2 + 0 + 4)
#define BLE_MESH_LIGHT_LIGHTNESS_LINEAR_SET_MSG_LEN       (2 + 5 + 4)
#define BLE_MESH_LIGHT_LIGHTNESS_LAST_GET_MSG_LEN         (2 + 0 + 4)
#define BLE_MESH_LIGHT_LIGHTNESS_DEFAULT_GET_MSG_LEN      (2 + 0 + 4)
#define BLE_MESH_LIGHT_LIGHTNESS_DEFAULT_SET_MSG_LEN      (2 + 2 + 4)
#define BLE_MESH_LIGHT_LIGHTNESS_RANGE_GET_MSG_LEN        (2 + 0 + 4)
#define BLE_MESH_LIGHT_LIGHTNESS_RANGE_SET_MSG_LEN        (2 + 4 + 4)

/* Light CTL client messages length */
#define BLE_MESH_LIGHT_CTL_GET_MSG_LEN                    (2 + 0 + 4)
#define BLE_MESH_LIGHT_CTL_SET_MSG_LEN                    (2 + 9 + 4)
#define BLE_MESH_LIGHT_CTL_TEMPERATURE_GET_MSG_LEN        (2 + 0 + 4)
#define BLE_MESH_LIGHT_CTL_TEMPERATURE_SET_MSG_LEN        (2 + 7 + 4)
#define BLE_MESH_LIGHT_CTL_TEMPERATURE_RANGE_GET_MSG_LEN  (2 + 0 + 4)
#define BLE_MESH_LIGHT_CTL_TEMPERATURE_RANGE_SET_MSG_LEN  (2 + 4 + 4)
#define BLE_MESH_LIGHT_CTL_DEFAULT_GET_MSG_LEN            (2 + 0 + 4)
#define BLE_MESH_LIGHT_CTL_DEFAULT_SET_MSG_LEN            (2 + 6 + 4)

/* Light HSL client messages length */
#define BLE_MESH_LIGHT_HSL_GET_MSG_LEN                    (2 + 0 + 4)
#define BLE_MESH_LIGHT_HSL_SET_MSG_LEN                    (2 + 9 + 4)
#define BLE_MESH_LIGHT_HSL_TARGET_GET_MSG_LEN             (2 + 0 + 4)
#define BLE_MESH_LIGHT_HSL_HUE_GET_MSG_LEN                (2 + 0 + 4)
#define BLE_MESH_LIGHT_HSL_HUE_SET_MSG_LEN                (2 + 5 + 4)
#define BLE_MESH_LIGHT_HSL_SATURATION_GET_MSG_LEN         (2 + 0 + 4)
#define BLE_MESH_LIGHT_HSL_SATURATION_SET_MSG_LEN         (2 + 5 + 4)
#define BLE_MESH_LIGHT_HSL_DEFAULT_GET_MSG_LEN            (2 + 0 + 4)
#define BLE_MESH_LIGHT_HSL_DEFAULT_SET_MSG_LEN            (2 + 6 + 4)
#define BLE_MESH_LIGHT_HSL_RANGE_GET_MSG_LEN              (2 + 0 + 4)
#define BLE_MESH_LIGHT_HSL_RANGE_SET_MSG_LEN              (2 + 8 + 4)

/* Light xyL client messages length */
#define BLE_MESH_LIGHT_XYL_SET_MSG_LEN                    (2 + 9 + 4)
#define BLE_MESH_LIGHT_XYL_DEFAULT_SET_MSG_LEN            (2 + 6 + 4)
#define BLE_MESH_LIGHT_XYL_RANGE_SET_MSG_LEN              (2 + 8 + 4)

/* Light LC client messages length */
#define BLE_MESH_LIGHT_LC_MODE_SET_MSG_LEN                (2 + 1 + 4)
#define BLE_MESH_LIGHT_LC_OM_SET_MSG_LEN                  (2 + 1 + 4)
#define BLE_MESH_LIGHT_LC_LIGHT_ONOFF_SET_MSG_LEN         (2 + 4 + 4)
#define BLE_MESH_LIGHT_LC_PROPERTY_GET_MSG_LEN            (2 + 2 + 4)
#define BLE_MESH_LIGHT_LC_PROPERTY_SET_MSG_LEN            /* variable */

#define BLE_MESH_LIGHT_GET_STATE_MSG_LEN                  (2 + 2 + 4)

static const bt_mesh_client_op_pair_t light_op_pair[] = {
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET,             BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS             },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET,             BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS             },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_GET,      BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS      },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET,      BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS      },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_GET,        BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_STATUS        },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_GET,     BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS     },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET,     BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS     },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_GET,       BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS       },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET,       BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS       },
    { BLE_MESH_MODEL_OP_LIGHT_CTL_GET,                   BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS                   },
    { BLE_MESH_MODEL_OP_LIGHT_CTL_SET,                   BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS                   },
    { BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_GET,       BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS       },
    { BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET,       BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS       },
    { BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_GET, BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_STATUS },
    { BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET, BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_STATUS },
    { BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_GET,           BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_STATUS           },
    { BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET,           BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_STATUS           },
    { BLE_MESH_MODEL_OP_LIGHT_HSL_GET,                   BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS                   },
    { BLE_MESH_MODEL_OP_LIGHT_HSL_SET,                   BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS                   },
    { BLE_MESH_MODEL_OP_LIGHT_HSL_TARGET_GET,            BLE_MESH_MODEL_OP_LIGHT_HSL_TARGET_STATUS            },
    { BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_GET,               BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_STATUS               },
    { BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET,               BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_STATUS               },
    { BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_GET,        BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_STATUS        },
    { BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET,        BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_STATUS        },
    { BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_GET,           BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_STATUS           },
    { BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET,           BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_STATUS           },
    { BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_GET,             BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_STATUS             },
    { BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET,             BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_STATUS             },
    { BLE_MESH_MODEL_OP_LIGHT_XYL_GET,                   BLE_MESH_MODEL_OP_LIGHT_XYL_STATUS                   },
    { BLE_MESH_MODEL_OP_LIGHT_XYL_SET,                   BLE_MESH_MODEL_OP_LIGHT_XYL_STATUS                   },
    { BLE_MESH_MODEL_OP_LIGHT_XYL_TARGET_GET,            BLE_MESH_MODEL_OP_LIGHT_XYL_TARGET_STATUS            },
    { BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_GET,           BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_STATUS           },
    { BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET,           BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_STATUS           },
    { BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_GET,             BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_STATUS             },
    { BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET,             BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_STATUS             },
    { BLE_MESH_MODEL_OP_LIGHT_LC_MODE_GET,               BLE_MESH_MODEL_OP_LIGHT_LC_MODE_STATUS               },
    { BLE_MESH_MODEL_OP_LIGHT_LC_MODE_SET,               BLE_MESH_MODEL_OP_LIGHT_LC_MODE_STATUS               },
    { BLE_MESH_MODEL_OP_LIGHT_LC_OM_GET,                 BLE_MESH_MODEL_OP_LIGHT_LC_OM_STATUS                 },
    { BLE_MESH_MODEL_OP_LIGHT_LC_OM_SET,                 BLE_MESH_MODEL_OP_LIGHT_LC_OM_STATUS                 },
    { BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_GET,        BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_STATUS        },
    { BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_SET,        BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_STATUS        },
    { BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_GET,           BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_STATUS           },
    { BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET,           BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_STATUS           },
};

static bt_mesh_mutex_t lighting_client_lock;

static void timeout_handler(struct k_work *work)
{
    struct k_delayed_work *timer = NULL;
    bt_mesh_client_node_t *node = NULL;
    struct bt_mesh_model *model = NULL;
    struct bt_mesh_msg_ctx ctx = {0};
    uint32_t opcode = 0U;

    BT_WARN("Receive light status message timeout");

    bt_mesh_mutex_lock(&lighting_client_lock);

    timer = CONTAINER_OF(work, struct k_delayed_work, work);

    if (timer && !k_delayed_work_free(timer)) {
        node = CONTAINER_OF(work, bt_mesh_client_node_t, timer.work);
        if (node) {
            memcpy(&ctx, &node->ctx, sizeof(ctx));
            opcode = node->opcode;
            model = node->model;
            bt_mesh_client_free_node(node);
            bt_mesh_lighting_client_cb_evt_to_btc(
                opcode, BTC_BLE_MESH_EVT_LIGHTING_CLIENT_TIMEOUT, model, &ctx, NULL, 0);
        }
    }

    bt_mesh_mutex_unlock(&lighting_client_lock);
}

static void light_status(struct bt_mesh_model *model,
                         struct bt_mesh_msg_ctx *ctx,
                         struct net_buf_simple *buf)
{
    bt_mesh_client_node_t *node = NULL;
    uint8_t *val = NULL;
    uint8_t evt = 0xFF;
    size_t len = 0U;

    BT_DBG("len %d, bytes %s", buf->len, bt_hex(buf->data, buf->len));

    switch (ctx->recv_op) {
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS: {
        struct bt_mesh_light_lightness_status *status = NULL;
        if (buf->len != 2 && buf->len != 5) {
            BT_ERR("Invalid Light Lightness Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_light_lightness_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->present_lightness = net_buf_simple_pull_le16(buf);
        if (buf->len) {
            status->op_en = true;
            status->target_lightness = net_buf_simple_pull_le16(buf);
            status->remain_time = net_buf_simple_pull_u8(buf);
        }
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_light_lightness_status);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS: {
        struct bt_mesh_light_lightness_linear_status *status = NULL;
        if (buf->len != 2 && buf->len != 5) {
            BT_ERR("Invalid Light Lightness Linear Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_light_lightness_linear_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->present_lightness = net_buf_simple_pull_le16(buf);
        if (buf->len) {
            status->op_en = true;
            status->target_lightness = net_buf_simple_pull_le16(buf);
            status->remain_time = net_buf_simple_pull_u8(buf);
        }
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_light_lightness_linear_status);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_STATUS: {
        struct bt_mesh_light_lightness_last_status *status = NULL;
        if (buf->len != 2) {
            BT_ERR("Invalid Light Lightness Last Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_light_lightness_last_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->lightness = net_buf_simple_pull_le16(buf);
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_light_lightness_last_status);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS: {
        struct bt_mesh_light_lightness_default_status *status = NULL;
        if (buf->len != 2) {
            BT_ERR("Invalid Light Lightness Default Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_light_lightness_default_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->lightness = net_buf_simple_pull_le16(buf);
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_light_lightness_default_status);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS: {
        struct bt_mesh_light_lightness_range_status *status = NULL;
        if (buf->len != 5) {
            BT_ERR("Invalid Light Lightness Range Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_light_lightness_range_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->status_code = net_buf_simple_pull_u8(buf);
        status->range_min = net_buf_simple_pull_le16(buf);
        status->range_max = net_buf_simple_pull_le16(buf);
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_light_lightness_range_status);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS: {
        struct bt_mesh_light_ctl_status *status = NULL;
        if (buf->len != 4 && buf->len != 9) {
            BT_ERR("Invalid Light CTL Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_light_ctl_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->present_ctl_lightness = net_buf_simple_pull_le16(buf);
        status->present_ctl_temperature = net_buf_simple_pull_le16(buf);
        if (buf->len) {
            status->op_en = true;
            status->target_ctl_lightness = net_buf_simple_pull_le16(buf);
            status->target_ctl_temperature = net_buf_simple_pull_le16(buf);
            status->remain_time = net_buf_simple_pull_u8(buf);
        }
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_light_ctl_status);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS: {
        struct bt_mesh_light_ctl_temperature_status *status = NULL;
        if (buf->len != 4 && buf->len != 9) {
            BT_ERR("Invalid Light CTL Temperature Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_light_ctl_temperature_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->present_ctl_temperature = net_buf_simple_pull_le16(buf);
        status->present_ctl_delta_uv = net_buf_simple_pull_le16(buf);
        if (buf->len) {
            status->op_en = true;
            status->target_ctl_temperature = net_buf_simple_pull_le16(buf);
            status->target_ctl_delta_uv = net_buf_simple_pull_le16(buf);
            status->remain_time = net_buf_simple_pull_u8(buf);
        }
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_light_ctl_temperature_status);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_STATUS: {
        struct bt_mesh_light_ctl_temperature_range_status *status = NULL;
        if (buf->len != 5) {
            BT_ERR("Invalid Light CTL Temperature Range Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_light_ctl_temperature_range_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->status_code = net_buf_simple_pull_u8(buf);
        status->range_min = net_buf_simple_pull_le16(buf);
        status->range_max = net_buf_simple_pull_le16(buf);
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_light_ctl_temperature_range_status);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_STATUS: {
        struct bt_mesh_light_ctl_default_status *status = NULL;
        if (buf->len != 6) {
            BT_ERR("Invalid Light CTL Default Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_light_ctl_default_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->lightness = net_buf_simple_pull_le16(buf);
        status->temperature = net_buf_simple_pull_le16(buf);
        status->delta_uv = net_buf_simple_pull_le16(buf);
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_light_ctl_default_status);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS: {
        struct bt_mesh_light_hsl_status *status = NULL;
        if (buf->len != 6 && buf->len != 7) {
            BT_ERR("Invalid Light HSL Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_light_hsl_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->hsl_lightness = net_buf_simple_pull_le16(buf);
        status->hsl_hue = net_buf_simple_pull_le16(buf);
        status->hsl_saturation = net_buf_simple_pull_le16(buf);
        if (buf->len) {
            status->op_en = true;
            status->remain_time = net_buf_simple_pull_u8(buf);
        }
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_light_hsl_status);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_HSL_TARGET_STATUS: {
        struct bt_mesh_light_hsl_target_status *status = NULL;
        if (buf->len != 6 && buf->len != 7) {
            BT_ERR("Invalid Light HSL Target Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_light_hsl_target_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->hsl_lightness_target = net_buf_simple_pull_le16(buf);
        status->hsl_hue_target = net_buf_simple_pull_le16(buf);
        status->hsl_saturation_target = net_buf_simple_pull_le16(buf);
        if (buf->len) {
            status->op_en = true;
            status->remain_time = net_buf_simple_pull_u8(buf);
        }
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_light_hsl_target_status);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_STATUS: {
        struct bt_mesh_light_hsl_hue_status *status = NULL;
        if (buf->len != 2 && buf->len != 5) {
            BT_ERR("Invalid Light HSL Hue Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_light_hsl_hue_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->present_hue = net_buf_simple_pull_le16(buf);
        if (buf->len) {
            status->op_en = true;
            status->target_hue = net_buf_simple_pull_le16(buf);
            status->remain_time = net_buf_simple_pull_u8(buf);
        }
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_light_hsl_hue_status);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_STATUS: {
        struct bt_mesh_light_hsl_saturation_status *status = NULL;
        if (buf->len != 2 && buf->len != 5) {
            BT_ERR("Invalid Light HSL Saturation Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_light_hsl_saturation_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->present_saturation = net_buf_simple_pull_le16(buf);
        if (buf->len) {
            status->op_en = true;
            status->target_saturation = net_buf_simple_pull_le16(buf);
            status->remain_time = net_buf_simple_pull_u8(buf);
        }
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_light_hsl_saturation_status);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_STATUS: {
        struct bt_mesh_light_hsl_default_status *status = NULL;
        if (buf->len != 6) {
            BT_ERR("Invalid Light HSL Default Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_light_hsl_default_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->lightness = net_buf_simple_pull_le16(buf);
        status->hue = net_buf_simple_pull_le16(buf);
        status->saturation = net_buf_simple_pull_le16(buf);
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_light_hsl_default_status);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_STATUS: {
        struct bt_mesh_light_hsl_range_status *status = NULL;
        if (buf->len != 9) {
            BT_ERR("Invalid Light HSL Range Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_light_hsl_range_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->status_code = net_buf_simple_pull_u8(buf);
        status->hue_range_min = net_buf_simple_pull_le16(buf);
        status->hue_range_max = net_buf_simple_pull_le16(buf);
        status->saturation_range_min = net_buf_simple_pull_le16(buf);
        status->saturation_range_max = net_buf_simple_pull_le16(buf);
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_light_hsl_range_status);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_XYL_STATUS: {
        struct bt_mesh_light_xyl_status *status = NULL;
        if (buf->len != 6 && buf->len != 7) {
            BT_ERR("Invalid Light xyL Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_light_xyl_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->xyl_lightness = net_buf_simple_pull_le16(buf);
        status->xyl_x = net_buf_simple_pull_le16(buf);
        status->xyl_y = net_buf_simple_pull_le16(buf);
        if (buf->len) {
            status->op_en = true;
            status->remain_time = net_buf_simple_pull_u8(buf);
        }
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_light_xyl_status);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_XYL_TARGET_STATUS: {
        struct bt_mesh_light_xyl_target_status *status = NULL;
        if (buf->len != 6 && buf->len != 7) {
            BT_ERR("Invalid Light xyL Target Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_light_xyl_target_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->target_xyl_lightness = net_buf_simple_pull_le16(buf);
        status->target_xyl_x = net_buf_simple_pull_le16(buf);
        status->target_xyl_y = net_buf_simple_pull_le16(buf);
        if (buf->len) {
            status->op_en = true;
            status->remain_time = net_buf_simple_pull_u8(buf);
        }
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_light_xyl_target_status);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_STATUS: {
        struct bt_mesh_light_xyl_default_status *status = NULL;
        if (buf->len != 6) {
            BT_ERR("Invalid Light xyL Default Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_light_xyl_default_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->lightness = net_buf_simple_pull_le16(buf);
        status->xyl_x = net_buf_simple_pull_le16(buf);
        status->xyl_y = net_buf_simple_pull_le16(buf);
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_light_xyl_default_status);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_STATUS: {
        struct bt_mesh_light_xyl_range_status *status = NULL;
        if (buf->len != 9) {
            BT_ERR("Invalid Light xyL Range Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_light_xyl_range_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->status_code = net_buf_simple_pull_u8(buf);
        status->xyl_x_range_min = net_buf_simple_pull_le16(buf);
        status->xyl_x_range_max = net_buf_simple_pull_le16(buf);
        status->xyl_y_range_min = net_buf_simple_pull_le16(buf);
        status->xyl_y_range_max = net_buf_simple_pull_le16(buf);
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_light_xyl_range_status);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_LC_MODE_STATUS: {
        struct bt_mesh_light_lc_mode_status *status = NULL;
        if (buf->len != 1) {
            BT_ERR("Invalid Light LC Mode Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_light_lc_mode_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->mode = net_buf_simple_pull_u8(buf);
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_light_lc_mode_status);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_LC_OM_STATUS: {
        struct bt_mesh_light_lc_om_status *status = NULL;
        if (buf->len != 1) {
            BT_ERR("Invalid Light LC OM Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_light_lc_om_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->mode = net_buf_simple_pull_u8(buf);
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_light_lc_om_status);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_STATUS: {
        struct bt_mesh_light_lc_light_onoff_status *status = NULL;
        if (buf->len != 1 && buf->len != 3) {
            BT_ERR("Invalid Light LC Light OnOff Status length %d", buf->len);
            return;
        }
        status = bt_mesh_calloc(sizeof(struct bt_mesh_light_lc_light_onoff_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->present_light_onoff = net_buf_simple_pull_u8(buf);
        if (buf->len) {
            status->op_en = true;
            status->target_light_onoff = net_buf_simple_pull_u8(buf);
            status->remain_time = net_buf_simple_pull_u8(buf);
        }
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_light_lc_light_onoff_status);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_STATUS: {
        struct bt_mesh_light_lc_property_status *status = NULL;
        status = bt_mesh_calloc(sizeof(struct bt_mesh_light_lc_property_status));
        if (!status) {
            BT_ERR("%s, Out of memory", __func__);
            return;
        }
        status->light_lc_property_id = net_buf_simple_pull_le16(buf);
        status->light_lc_property_value = bt_mesh_alloc_buf(buf->len);
        if (!status->light_lc_property_value) {
            BT_ERR("%s, Out of memory", __func__);
            bt_mesh_free(status);
            return;
        }
        net_buf_simple_add_mem(status->light_lc_property_value, buf->data, buf->len);
        val = (uint8_t *)status;
        len = sizeof(struct bt_mesh_light_lc_property_status);
        break;
    }
    default:
        BT_ERR("Invalid Lighting Status opcode 0x%04x", ctx->recv_op);
        return;
    }

    buf->data = val;
    buf->len  = len;

    bt_mesh_mutex_lock(&lighting_client_lock);

    node = bt_mesh_is_client_recv_publish_msg(model, ctx, buf, true);
    if (!node) {
        BT_DBG("Unexpected Lighting Status 0x%04x", ctx->recv_op);
    } else {
        switch (node->opcode) {
        case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET:
        case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_GET:
        case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_GET:
        case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_GET:
        case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_GET:
        case BLE_MESH_MODEL_OP_LIGHT_CTL_GET:
        case BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_GET:
        case BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_GET:
        case BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_GET:
        case BLE_MESH_MODEL_OP_LIGHT_HSL_GET:
        case BLE_MESH_MODEL_OP_LIGHT_HSL_TARGET_GET:
        case BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_GET:
        case BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_GET:
        case BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_GET:
        case BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_GET:
        case BLE_MESH_MODEL_OP_LIGHT_XYL_GET:
        case BLE_MESH_MODEL_OP_LIGHT_XYL_TARGET_GET:
        case BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_GET:
        case BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_GET:
        case BLE_MESH_MODEL_OP_LIGHT_LC_MODE_GET:
        case BLE_MESH_MODEL_OP_LIGHT_LC_OM_GET:
        case BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_GET:
        case BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_GET:
            evt = BTC_BLE_MESH_EVT_LIGHTING_CLIENT_GET_STATE;
            break;
        case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET:
        case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET:
        case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET:
        case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET:
        case BLE_MESH_MODEL_OP_LIGHT_CTL_SET:
        case BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET:
        case BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET:
        case BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET:
        case BLE_MESH_MODEL_OP_LIGHT_HSL_SET:
        case BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET:
        case BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET:
        case BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET:
        case BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET:
        case BLE_MESH_MODEL_OP_LIGHT_XYL_SET:
        case BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET:
        case BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET:
        case BLE_MESH_MODEL_OP_LIGHT_LC_MODE_SET:
        case BLE_MESH_MODEL_OP_LIGHT_LC_OM_SET:
        case BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_SET:
        case BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET:
            evt = BTC_BLE_MESH_EVT_LIGHTING_CLIENT_SET_STATE;
            break;
        default:
            break;
        }

        if (!k_delayed_work_free(&node->timer)) {
            uint32_t opcode = node->opcode;
            bt_mesh_client_free_node(node);
            bt_mesh_lighting_client_cb_evt_to_btc(opcode, evt, model, ctx, val, len);
        }
    }

    bt_mesh_mutex_unlock(&lighting_client_lock);

    switch (ctx->recv_op) {
    case BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_STATUS: {
        struct bt_mesh_light_lc_property_status *status;
        status = (struct bt_mesh_light_lc_property_status *)val;
        bt_mesh_free_buf(status->light_lc_property_value);
        break;
    }
    default:
        break;
    }

    bt_mesh_free(val);
}

const struct bt_mesh_model_op bt_mesh_light_lightness_cli_op[] = {
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS,         2, light_status },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS,  2, light_status },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_STATUS,    2, light_status },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS, 2, light_status },
    { BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS,   5, light_status },
    BLE_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op bt_mesh_light_ctl_cli_op[] = {
    { BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS,                   4, light_status },
    { BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS,       4, light_status },
    { BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_STATUS, 5, light_status },
    { BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_STATUS,           6, light_status },
    BLE_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op bt_mesh_light_hsl_cli_op[] = {
    { BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS,            6, light_status },
    { BLE_MESH_MODEL_OP_LIGHT_HSL_TARGET_STATUS,     6, light_status },
    { BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_STATUS,        2, light_status },
    { BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_STATUS, 2, light_status },
    { BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_STATUS,    6, light_status },
    { BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_STATUS,      9, light_status },
    BLE_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op bt_mesh_light_xyl_cli_op[] = {
    { BLE_MESH_MODEL_OP_LIGHT_XYL_STATUS,         6, light_status },
    { BLE_MESH_MODEL_OP_LIGHT_XYL_TARGET_STATUS,  6, light_status },
    { BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_STATUS, 6, light_status },
    { BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_STATUS,   9, light_status },
    BLE_MESH_MODEL_OP_END,
};

const struct bt_mesh_model_op bt_mesh_light_lc_cli_op[] = {
    { BLE_MESH_MODEL_OP_LIGHT_LC_MODE_STATUS,        1, light_status },
    { BLE_MESH_MODEL_OP_LIGHT_LC_OM_STATUS,          1, light_status },
    { BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_STATUS, 1, light_status },
    { BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_STATUS,    2, light_status },
    BLE_MESH_MODEL_OP_END,
};

static int light_get_state(bt_mesh_client_common_param_t *common, void *value)
{
    NET_BUF_SIMPLE_DEFINE(msg, BLE_MESH_LIGHT_GET_STATE_MSG_LEN);

    bt_mesh_model_msg_init(&msg, common->opcode);

    if (value) {
        switch (common->opcode) {
        case BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_GET: {
            struct bt_mesh_light_lc_property_get *get;
            get = (struct bt_mesh_light_lc_property_get *)value;
            net_buf_simple_add_le16(&msg, get->light_lc_property_id);
            break;
        }
        default:
            BT_DBG("No parameters for Lighting Get 0x%04x", common->opcode);
            break;
        }
    }

    return bt_mesh_client_send_msg(common, &msg, true, timeout_handler);
}

static int light_set_state(bt_mesh_client_common_param_t *common,
                           void *value, uint16_t value_len, bool need_ack)
{
    struct net_buf_simple *msg = NULL;
    int err = 0;

    msg = bt_mesh_alloc_buf(value_len);
    if (!msg) {
        BT_ERR("%s, Out of memory", __func__);
        return -ENOMEM;
    }

    bt_mesh_model_msg_init(msg, common->opcode);

    switch (common->opcode) {
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET:
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK: {
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
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET:
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET_UNACK: {
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
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET:
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET_UNACK: {
        struct bt_mesh_light_lightness_default_set *set;
        set = (struct bt_mesh_light_lightness_default_set *)value;
        net_buf_simple_add_le16(msg, set->lightness);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET:
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET_UNACK: {
        struct bt_mesh_light_lightness_range_set *set;
        set = (struct bt_mesh_light_lightness_range_set *)value;
        net_buf_simple_add_le16(msg, set->range_min);
        net_buf_simple_add_le16(msg, set->range_max);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_CTL_SET:
    case BLE_MESH_MODEL_OP_LIGHT_CTL_SET_UNACK: {
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
    case BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET:
    case BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET_UNACK: {
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
    case BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET:
    case BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACK: {
        struct bt_mesh_light_ctl_temperature_range_set *set;
        set = (struct bt_mesh_light_ctl_temperature_range_set *)value;
        net_buf_simple_add_le16(msg, set->range_min);
        net_buf_simple_add_le16(msg, set->range_max);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET:
    case BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET_UNACK: {
        struct bt_mesh_light_ctl_default_set *set;
        set = (struct bt_mesh_light_ctl_default_set *)value;
        net_buf_simple_add_le16(msg, set->lightness);
        net_buf_simple_add_le16(msg, set->temperature);
        net_buf_simple_add_le16(msg, set->delta_uv);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_HSL_SET:
    case BLE_MESH_MODEL_OP_LIGHT_HSL_SET_UNACK: {
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
    case BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET:
    case BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET_UNACK: {
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
    case BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET:
    case BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET_UNACK: {
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
    case BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET:
    case BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET_UNACK: {
        struct bt_mesh_light_hsl_default_set *set;
        set = (struct bt_mesh_light_hsl_default_set *)value;
        net_buf_simple_add_le16(msg, set->lightness);
        net_buf_simple_add_le16(msg, set->hue);
        net_buf_simple_add_le16(msg, set->saturation);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET:
    case BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET_UNACK: {
        struct bt_mesh_light_hsl_range_set *set;
        set = (struct bt_mesh_light_hsl_range_set *)value;
        net_buf_simple_add_le16(msg, set->hue_range_min);
        net_buf_simple_add_le16(msg, set->hue_range_max);
        net_buf_simple_add_le16(msg, set->saturation_range_min);
        net_buf_simple_add_le16(msg, set->saturation_range_max);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_XYL_SET:
    case BLE_MESH_MODEL_OP_LIGHT_XYL_SET_UNACK: {
        struct bt_mesh_light_xyl_set *set;
        set = (struct bt_mesh_light_xyl_set *)value;
        net_buf_simple_add_le16(msg, set->xyl_lightness);
        net_buf_simple_add_le16(msg, set->xyl_x);
        net_buf_simple_add_le16(msg, set->xyl_y);
        net_buf_simple_add_u8(msg, set->tid);
        if (set->op_en) {
            net_buf_simple_add_u8(msg, set->trans_time);
            net_buf_simple_add_u8(msg, set->delay);
        }
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET:
    case BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET_UNACK: {
        struct bt_mesh_light_xyl_default_set *set;
        set = (struct bt_mesh_light_xyl_default_set *)value;
        net_buf_simple_add_le16(msg, set->lightness);
        net_buf_simple_add_le16(msg, set->xyl_x);
        net_buf_simple_add_le16(msg, set->xyl_y);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET:
    case BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET_UNACK: {
        struct bt_mesh_light_xyl_range_set *set;
        set = (struct bt_mesh_light_xyl_range_set *)value;
        net_buf_simple_add_le16(msg, set->xyl_x_range_min);
        net_buf_simple_add_le16(msg, set->xyl_x_range_max);
        net_buf_simple_add_le16(msg, set->xyl_y_range_min);
        net_buf_simple_add_le16(msg, set->xyl_y_range_max);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_LC_MODE_SET:
    case BLE_MESH_MODEL_OP_LIGHT_LC_MODE_SET_UNACK: {
        struct bt_mesh_light_lc_mode_set *set;
        set = (struct bt_mesh_light_lc_mode_set *)value;
        net_buf_simple_add_u8(msg, set->mode);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_LC_OM_SET:
    case BLE_MESH_MODEL_OP_LIGHT_LC_OM_SET_UNACK: {
        struct bt_mesh_light_lc_om_set *set;
        set = (struct bt_mesh_light_lc_om_set *)value;
        net_buf_simple_add_u8(msg, set->mode);
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_SET:
    case BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_SET_UNACK: {
        struct bt_mesh_light_lc_light_onoff_set *set;
        set = (struct bt_mesh_light_lc_light_onoff_set *)value;
        net_buf_simple_add_u8(msg, set->light_onoff);
        net_buf_simple_add_u8(msg, set->tid);
        if (set->op_en) {
            net_buf_simple_add_u8(msg, set->trans_time);
            net_buf_simple_add_u8(msg, set->delay);
        }
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET:
    case BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET_UNACK: {
        struct bt_mesh_light_lc_property_set *set;
        set = (struct bt_mesh_light_lc_property_set *)value;
        net_buf_simple_add_le16(msg, set->light_lc_property_id);
        net_buf_simple_add_mem(msg, set->light_lc_property_value->data, set->light_lc_property_value->len);
        break;
    }
    default:
        BT_ERR("Invalid Lighting Set opcode 0x%04x", common->opcode);
        err = -EINVAL;
        goto end;
    }

    err = bt_mesh_client_send_msg(common, msg, need_ack, timeout_handler);

end:
    bt_mesh_free_buf(msg);
    return err;
}

int bt_mesh_light_client_get_state(bt_mesh_client_common_param_t *common, void *get)
{
    bt_mesh_light_client_t *client = NULL;

    if (!common || !common->model) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    client = (bt_mesh_light_client_t *)common->model->user_data;
    if (!client || !client->internal_data) {
        BT_ERR("Invalid Lighting client data");
        return -EINVAL;
    }

    switch (common->opcode) {
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET:
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_GET:
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_GET:
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_GET:
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_GET:
    case BLE_MESH_MODEL_OP_LIGHT_CTL_GET:
    case BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_GET:
    case BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_GET:
    case BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_GET:
    case BLE_MESH_MODEL_OP_LIGHT_HSL_GET:
    case BLE_MESH_MODEL_OP_LIGHT_HSL_TARGET_GET:
    case BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_GET:
    case BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_GET:
    case BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_GET:
    case BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_GET:
    case BLE_MESH_MODEL_OP_LIGHT_XYL_GET:
    case BLE_MESH_MODEL_OP_LIGHT_XYL_TARGET_GET:
    case BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_GET:
    case BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_GET:
    case BLE_MESH_MODEL_OP_LIGHT_LC_MODE_GET:
    case BLE_MESH_MODEL_OP_LIGHT_LC_OM_GET:
    case BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_GET:
        break;
    case BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_GET:
        if (!get) {
            BT_ERR("Invalid Lighting LC Property Get");
            return -EINVAL;
        }
        break;
    default:
        BT_ERR("Invalid Lighting Get opcode 0x%04x", common->opcode);
        return -EINVAL;
    }

    return light_get_state(common, get);
}

int bt_mesh_light_client_set_state(bt_mesh_client_common_param_t *common, void *set)
{
    bt_mesh_light_client_t *client = NULL;
    uint16_t length = 0U;
    bool need_ack = false;

    if (!common || !common->model || !set) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    client = (bt_mesh_light_client_t *)common->model->user_data;
    if (!client || !client->internal_data) {
        BT_ERR("Invalid Lighting client data");
        return -EINVAL;
    }

    switch (common->opcode) {
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK: {
        struct bt_mesh_light_lightness_set *value;
        value = (struct bt_mesh_light_lightness_set *)set;
        if (value->op_en) {
            if ((value->trans_time & 0x3F) > 0x3E) {
                BT_ERR("Invalid Light Lightness Set transition time");
                return -EINVAL;
            }
        }
        length = BLE_MESH_LIGHT_LIGHTNESS_SET_MSG_LEN;
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET_UNACK: {
        struct bt_mesh_light_lightness_linear_set *value;
        value = (struct bt_mesh_light_lightness_linear_set *)set;
        if (value->op_en) {
            if ((value->trans_time & 0x3F) > 0x3E) {
                BT_ERR("Invalid Light Lightness Linear Set transition time");
                return -EINVAL;
            }
        }
        length = BLE_MESH_LIGHT_LIGHTNESS_LINEAR_SET_MSG_LEN;
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET_UNACK:
        length = BLE_MESH_LIGHT_LIGHTNESS_DEFAULT_SET_MSG_LEN;
        break;
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET_UNACK: {
        struct bt_mesh_light_lightness_range_set *value;
        value = (struct bt_mesh_light_lightness_range_set *)set;
        if (value->range_min > value->range_max) {
            BT_ERR("Light Lightness Range Set range min is greater than range max");
            return -EINVAL;
        }
        length = BLE_MESH_LIGHT_LIGHTNESS_RANGE_SET_MSG_LEN;
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_CTL_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_LIGHT_CTL_SET_UNACK: {
        struct bt_mesh_light_ctl_set *value;
        value = (struct bt_mesh_light_ctl_set *)set;
        if (value->op_en) {
            if ((value->trans_time & 0x3F) > 0x3E) {
                BT_ERR("Invalid Light CTL Set transition time");
                return -EINVAL;
            }
        }
        length = BLE_MESH_LIGHT_CTL_SET_MSG_LEN;
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET_UNACK: {
        struct bt_mesh_light_ctl_temperature_set *value;
        value = (struct bt_mesh_light_ctl_temperature_set *)set;
        if (value->op_en) {
            if ((value->trans_time & 0x3F) > 0x3E) {
                BT_ERR("Invalid Light CTL Temperature Set transition time");
                return -EINVAL;
            }
        }
        length = BLE_MESH_LIGHT_CTL_TEMPERATURE_SET_MSG_LEN;
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACK: {
        struct bt_mesh_light_ctl_temperature_range_set *value;
        value = (struct bt_mesh_light_ctl_temperature_range_set *)set;
        if (value->range_min > value->range_max) {
            BT_ERR("Light CTL Temperature Range Set range min is greater than range max");
            return -EINVAL;
        }
        length = BLE_MESH_LIGHT_CTL_TEMPERATURE_RANGE_SET_MSG_LEN;
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET_UNACK:
        length = BLE_MESH_LIGHT_CTL_DEFAULT_SET_MSG_LEN;
        break;
    case BLE_MESH_MODEL_OP_LIGHT_HSL_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_LIGHT_HSL_SET_UNACK: {
        struct bt_mesh_light_hsl_set *value;
        value = (struct bt_mesh_light_hsl_set *)set;
        if (value->op_en) {
            if ((value->trans_time & 0x3F) > 0x3E) {
                BT_ERR("Invalid Light HSL Set transition time");
                return -EINVAL;
            }
        }
        length = BLE_MESH_LIGHT_HSL_SET_MSG_LEN;
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET_UNACK: {
        struct bt_mesh_light_hsl_hue_set *value;
        value = (struct bt_mesh_light_hsl_hue_set *)set;
        if (value->op_en) {
            if ((value->trans_time & 0x3F) > 0x3E) {
                BT_ERR("Invalid Light HSL Hue Set transition time");
                return -EINVAL;
            }
        }
        length = BLE_MESH_LIGHT_HSL_HUE_SET_MSG_LEN;
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET_UNACK: {
        struct bt_mesh_light_hsl_saturation_set *value;
        value = (struct bt_mesh_light_hsl_saturation_set *)set;
        if (value->op_en) {
            if ((value->trans_time & 0x3F) > 0x3E) {
                BT_ERR("Invalid Light HSL Saturation Set transition time");
                return -EINVAL;
            }
        }
        length = BLE_MESH_LIGHT_HSL_SATURATION_SET_MSG_LEN;
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET_UNACK:
        length = BLE_MESH_LIGHT_HSL_DEFAULT_SET_MSG_LEN;
        break;
    case BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET_UNACK: {
        struct bt_mesh_light_hsl_range_set *value;
        value = (struct bt_mesh_light_hsl_range_set *)set;
        if (value->hue_range_min > value->hue_range_max ||
                value->saturation_range_min > value->saturation_range_max) {
            BT_ERR("Light HSL Range Set range min is greater than range max");
            return -EINVAL;
        }
        length = BLE_MESH_LIGHT_HSL_RANGE_SET_MSG_LEN;
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_XYL_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_LIGHT_XYL_SET_UNACK: {
        struct bt_mesh_light_xyl_set *value;
        value = (struct bt_mesh_light_xyl_set *)set;
        if (value->op_en) {
            if ((value->trans_time & 0x3F) > 0x3E) {
                BT_ERR("Invalid Light xyL Set transition time");
                return -EINVAL;
            }
        }
        length = BLE_MESH_LIGHT_XYL_SET_MSG_LEN;
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET_UNACK:
        length = BLE_MESH_LIGHT_XYL_DEFAULT_SET_MSG_LEN;
        break;
    case BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET_UNACK: {
        struct bt_mesh_light_xyl_range_set *value;
        value = (struct bt_mesh_light_xyl_range_set *)set;
        if (value->xyl_x_range_min > value->xyl_x_range_max ||
                value->xyl_y_range_min > value->xyl_y_range_max) {
            BT_ERR("Light xyL Range Set range min is greater than range max");
            return -EINVAL;
        }
        length = BLE_MESH_LIGHT_XYL_RANGE_SET_MSG_LEN;
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_LC_MODE_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_LIGHT_LC_MODE_SET_UNACK:
        length = BLE_MESH_LIGHT_LC_MODE_SET_MSG_LEN;
        break;
    case BLE_MESH_MODEL_OP_LIGHT_LC_OM_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_LIGHT_LC_OM_SET_UNACK:
        length = BLE_MESH_LIGHT_LC_OM_SET_MSG_LEN;
        break;
    case BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_SET_UNACK: {
        struct bt_mesh_light_lc_light_onoff_set *value;
        value = (struct bt_mesh_light_lc_light_onoff_set *)set;
        if (value->op_en) {
            if ((value->trans_time & 0x3F) > 0x3E) {
                BT_ERR("Invalid Light LC Light OnOff Set transition time");
                return -EINVAL;
            }
        }
        length = BLE_MESH_LIGHT_LC_LIGHT_ONOFF_SET_MSG_LEN;
        break;
    }
    case BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET:
        need_ack = true;
    case BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET_UNACK: {
        struct bt_mesh_light_lc_property_set *value;
        value = (struct bt_mesh_light_lc_property_set *)set;
        if (!value->light_lc_property_value) {
            BT_ERR("Invalid Lighting Light LC Property value");
            return -EINVAL;
        }
        length = (1 + 2 + value->light_lc_property_value->len + 4);
        break;
    }
    default:
        BT_ERR("Invalid Lighting Set opcode 0x%04x", common->opcode);
        return -EINVAL;
    }

    return light_set_state(common, set, length, need_ack);
}

static int lighting_client_init(struct bt_mesh_model *model)
{
    light_internal_data_t *internal = NULL;
    bt_mesh_light_client_t *client = NULL;

    if (!model) {
        BT_ERR("Invalid Lighting client model");
        return -EINVAL;
    }

    client = (bt_mesh_light_client_t *)model->user_data;
    if (!client) {
        BT_ERR("No Lighting client context provided");
        return -EINVAL;
    }

    if (client->internal_data) {
        BT_WARN("%s, Already", __func__);
        return -EALREADY;
    }

    internal = bt_mesh_calloc(sizeof(light_internal_data_t));
    if (!internal) {
        BT_ERR("%s, Out of memory", __func__);
        return -ENOMEM;
    }

    sys_slist_init(&internal->queue);

    client->model = model;
    client->op_pair_size = ARRAY_SIZE(light_op_pair);
    client->op_pair = light_op_pair;
    client->internal_data = internal;

    bt_mesh_mutex_create(&lighting_client_lock);

    return 0;
}

#if CONFIG_BLE_MESH_DEINIT
static int lighting_client_deinit(struct bt_mesh_model *model)
{
    bt_mesh_light_client_t *client = NULL;

    if (!model) {
        BT_ERR("Invalid Lighting client model");
        return -EINVAL;
    }

    client = (bt_mesh_light_client_t *)model->user_data;
    if (!client) {
        BT_ERR("No Lighting client context provided");
        return -EINVAL;
    }

    if (client->internal_data) {
        /* Remove items from the list */
        bt_mesh_client_clear_list(client->internal_data);

        /* Free the allocated internal data */
        bt_mesh_free(client->internal_data);
        client->internal_data = NULL;
    }

    bt_mesh_mutex_free(&lighting_client_lock);

    return 0;
}
#endif /* CONFIG_BLE_MESH_DEINIT */

const struct bt_mesh_model_cb bt_mesh_lighting_client_cb = {
    .init = lighting_client_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = lighting_client_deinit,
#endif /* CONFIG_BLE_MESH_DEINIT */
};

#endif /* CONFIG_BLE_MESH_LIGHTING_CLIENT */
