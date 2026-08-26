/*
 * SPDX-FileCopyrightText: 2018-2020 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2020-2022 Espressif Systems (Shanghai) CO LTD
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "esp_log.h"
#include "esp_random.h"
#include "nvs_flash.h"

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_local_data_operation_api.h"

#include "genie_mesh.h"
#include "genie_event.h"
#include "genie_util.h"
#include "genie_timer.h"
#include "genie_model_srv.h"

static const char *TAG  = "genie_model_srv";

/**
 * g_vnd_msg_list was used to save the unconfirmed vendor messages
 * g_vnd_msg_timer was used to control when the vendor messages saved in g_vnd_msg_list will be resent
 **/
static genie_dlist_t g_vnd_msg_list = {0};
static util_timer_t g_vnd_msg_timer = {0};

static void genie_model_retry_timer_cb(void *args);

/**
 * @brief Get Genie Vendor Model Appkey ID
 *
 * @param elem
 * @param p_model
 * @return uint16_t
 */
static uint16_t bt_mesh_model_get_appkey_id(esp_ble_mesh_elem_t *elem, esp_ble_mesh_model_t *p_model)
{
    ENTER_FUNC();

    return 0;
}

/**
 * @brief Get Genie Vendor Model Netkey ID
 *
 * @param elem
 *
 * @return uint16_t
 */
static uint16_t bt_mesh_model_get_netkey_id(esp_ble_mesh_elem_t *elem)
{
    ENTER_FUNC();

    return 0;
}

/** @def genie_model_msg_gen_tid
 *
 *  @brief generate tid used in vendor model message
 *
 *  @param NULL
 *
 *  @return tid with range of [0x80, 0xff]
 */
static uint8_t genie_model_msg_gen_tid(void)
{
    ENTER_FUNC();
    static uint8_t tid = 0xFF;

    if (tid == 0xFF) {
        esp_fill_random(&tid, 1);
        tid &= 0x3F;
    } else {
        tid = (tid + 1) & 0x3F;
    }

    return (tid | 0x80);
}

/** @def genie_model_init
 *
 *  @brief vendor model server related global parameter initialization
 *
 *  @param NULL
 *
 *  @return always return true
 */
static uint16_t genie_model_init(void)
{
    ENTER_FUNC();
    static bool init_flag = false;

    if (!init_flag) {
        ESP_LOGD(TAG, "init g_vnd_msg_timer");
        util_timer_init(&g_vnd_msg_timer, genie_model_retry_timer_cb, &g_vnd_msg_list);
        genie_dlist_init(&g_vnd_msg_list);
    }
    init_flag = true;

    return true;
}

/** @def genie_model_msg_node_free
 *
 *  @brief free the vendor model message node struct's memory
 *
 *  @param pointer to the vendor model message node to be freed
 *
 *  @return 0 for success; negative for failure
 */
static esp_err_t genie_model_msg_node_free(genie_model_msg_node_t *p_node)
{
    ENTER_FUNC();

    free(p_node);

    return ESP_OK;
}

/** @def genie_model_msg_node_generate
 *
 *  @brief duplicate genie_model_msg_t and save to genie_model_msg_node_t
 *
 *  @param pointer to the vendor model message to be duplicated
 *
 *  @return pointer to genie_model_msg_node_t for success, NULL for failure
 */
static genie_model_msg_node_t *genie_model_msg_node_generate(genie_model_msg_t *p_model_msg)
{
    ENTER_FUNC();
    genie_model_msg_node_t *p_node = NULL;

    if (!p_model_msg->retry) {
        p_model_msg->retry = GENIE_MODEL_MSG_DFT_RETRY_TIMES;
    } else if (p_model_msg->retry > GENIE_MODEL_MSG_MAX_RETRY_TIMES) {
        p_model_msg->retry = GENIE_MODEL_MSG_MAX_RETRY_TIMES;
    }

    p_node = malloc(sizeof(genie_model_msg_node_t) + p_model_msg->len);
    if (!p_node) {
        ESP_LOGE(TAG, "malloc for genie_model_msg_node_t failed");
        return NULL;
    }

    memcpy(&p_node->msg, p_model_msg, sizeof(genie_model_msg_t));
    ESP_LOGD(TAG, "p_node->msg: %p, data: %p, %p", &p_node->msg, &p_node->msg.data, &p_node->msg.data + 1);
    p_node->msg.data = (uint8_t *)(&p_node->msg.data + 1);
    memcpy(p_node->msg.data, p_model_msg->data, p_model_msg->len);
    ESP_LOGD(TAG, "p_model_msg->data: %p, data: 0x%s, p_node->msg.data: %p, data: 0x%s",
             p_model_msg->data, util_hex2str(p_model_msg->data, p_model_msg->len),
             p_node->msg.data, util_hex2str(p_node->msg.data, p_node->msg.len));
    p_node->timeout = esp_timer_get_time() + p_model_msg->retry_period;

    p_node->left_retry = p_model_msg->retry;

    return p_node;
}

/** @def genie_model_msg_list_append
 *
 *  @brief duplicate genie_model_msg_t and append it to vendor model message list to be monitored
 *
 *  @param pointer to the vendor model message to be duplicated
 *
 *  @return 0 for success; negative for failure
 */
static esp_err_t genie_model_msg_list_append(genie_model_msg_t *p_model_msg)
{
    ENTER_FUNC();
    genie_model_msg_node_t *p_msg_node = NULL;

    p_msg_node = genie_model_msg_node_generate(p_model_msg);

    if (!p_msg_node) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGD(TAG, "append msg: %p, opid: 0x%02x, retry: %d, head: %p, node: %p", p_model_msg, p_model_msg->opid, p_model_msg->retry, &g_vnd_msg_list, &p_msg_node->node);
    if (genie_dlist_node_number(&g_vnd_msg_list) >= GENIE_VENDOR_MSG_LIST_MAXSIZE) {
        ESP_LOGW(TAG, "List Full, discard!!!");
        return ESP_OK;
    }

    genie_dlist_append(&g_vnd_msg_list, &p_msg_node->node);

    // Check retry timer, if timer is not started yet, start it
    if (!util_timer_is_started(&g_vnd_msg_timer)) {
        util_timer_start(&g_vnd_msg_timer, p_model_msg->retry_period);
    }

    return ESP_OK;
}

/** @def genie_model_retry_timer_cb
 *
 *  @brief timeout handler for the g_vnd_msg_timer
 *
 *  @param p_timer - pointer to the timer; args - pointer to g_vnd_msg_list
 *
 *  @return N/A
 */
static void genie_model_retry_timer_cb(void *args)
{
    ENTER_FUNC();
    uint32_t               nearest     = 0;
    genie_dnode_t          *p_node     = NULL;
    genie_model_msg_node_t *p_msg_node = NULL;
    genie_model_msg_t      *p_msg      = NULL;
    genie_dlist_t          *p_head     = (genie_dlist_t *)args;

    ESP_LOGD(TAG, "g_vnd_msg_timer timeout, p_head: %p", p_head);

    /**
     * 1. go through p_head
     * 2. resend the no responsed messages if timeout happens and refresh timeout value
     * */
    GENIE_DLIST_FOR_EACH_NODE(p_head, p_node) {
        p_msg_node = CONTAINER_OF(p_node, genie_model_msg_node_t, node);
        nearest    = p_msg_node->msg.retry_period;
        p_msg      = &p_msg_node->msg;
        ESP_LOGD(TAG, "msg: %p, opid: 0x%02x, left: %d", p_msg, p_msg->opid, p_msg_node->left_retry);

        if (p_msg_node->timeout <= esp_timer_get_time()) {
            ESP_LOGD(TAG, "timeout - msg: %p, opid: 0x%02x, left: %d", p_msg, p_msg->opid, p_msg_node->left_retry);
            genie_model_msg_send(p_msg);
            if (--p_msg_node->left_retry <= 0) {
                genie_dlist_remove(p_node);
                genie_model_msg_node_free((genie_model_msg_node_t *)p_node);
                break;
            }
            p_msg_node->timeout = esp_timer_get_time() + p_msg_node->msg.retry_period;
        } else {
            if (nearest > p_msg_node->timeout) {
                nearest = p_msg_node->timeout;
            }
        }
    }

    /* start new timer */
    if (!genie_dlist_is_empty(p_head)) {
        util_timer_start(&g_vnd_msg_timer, nearest);
        ESP_LOGD(TAG, "restart retry timer, timeout: %" PRIu32, nearest);
    } else {
        util_timer_stop(&g_vnd_msg_timer);
        ESP_LOGD(TAG, "list empty, stop timer");
    }

    return;
}

/** @def genie_model_msg_check_tid
 *
 *  @brief check received vendor message's tid
 *
 *  @param pointer to g_vnd_msg_list, tid of the received vendor model message
 *
 *  @return 0 for success; negative for failure
 */
static esp_err_t genie_model_msg_check_tid(genie_dlist_t *p_head, uint8_t tid)
{
    ENTER_FUNC();
    genie_dnode_t *p_node = NULL;

    if (!p_head) {
        return ESP_FAIL;
    }

    if (genie_dlist_is_empty(p_head)) {
        return ESP_OK;
    }

    /**
     * go through message list and dequeue the vendor model's message and free it if received message
     * s tid equals this message's tid
     * */
    GENIE_DLIST_FOR_EACH_NODE(p_head, p_node) {
        genie_model_msg_t      *p_msg      = NULL;
        genie_model_msg_node_t *p_msg_node = NULL;

        p_msg_node = CONTAINER_OF(p_node, genie_model_msg_node_t, node);
        p_msg      = &p_msg_node->msg;

        if (p_msg->tid == tid) {
            ESP_LOGD(TAG, "dequeue msg: %p, opid: 0x%02x, retry: %2d", p_msg, p_msg->opid, p_msg->retry);
            genie_dlist_remove(p_node);
            genie_model_msg_node_free((genie_model_msg_node_t *)p_node);
            break;
        }
    }

    return ESP_OK;
}


/** @def genie_model_msg_send
 *
 *  @brief send the vendor model message
 *
 *  @param pointer to the message to be sent
 *
 *  @return 0 for success; negative for failure
 */
esp_err_t genie_model_msg_send(genie_model_msg_t *p_model_msg)
{
    ENTER_FUNC();
    esp_err_t err                   = ESP_FAIL;
    bool resend_flag                = false;
    esp_ble_mesh_msg_ctx_t ctx      = {0};
    esp_ble_mesh_model_t *p_model   = esp_ble_mesh_find_vendor_model(p_model_msg->p_elem, CID_ALIBABA, GENIE_VENDOR_MODEL_SRV_ID);

    genie_model_init();

    if (!p_model) {
        ESP_LOGE(TAG, "cannot find vendor model server %p", p_model_msg->p_elem);
        return err;
    }

    ESP_LOGD(TAG, "p_model: %p, cid: 0x%04x, id: 0x%04x, opcode: 0x%02x, retry: %d", p_model, p_model->vnd.company_id, p_model->vnd.model_id, p_model_msg->opid, p_model_msg->retry);

    if(p_model_msg->tid == 0) {
        p_model_msg->tid = genie_model_msg_gen_tid();
        ESP_LOGD(TAG, "genie_model_msg_gen_tid: 0x%02x", p_model_msg->tid);
    }

    /**
     * no need to duplicate the following messages
     * 1. retry <= 0 - the message won't want to be resent
     * 2. tid is in valid range (0x00, 0x7F] [0xC0, 0xff]
     * 3. opcode is not VENDOR_OP_ATTR_INDICATE/VENDOR_OP_ATTR_INDICATE_TG/VENDOR_OP_ATTR_TRANS_INDICATE
     * 4. already duplicated or CONFIME/CONFIME_TG
     * */
    if ((p_model_msg->retry > 1) &&
        (p_model_msg->tid >= 0x7F && p_model_msg->tid < 0xC0) &&
        ((p_model_msg->opid == GENIE_OP_ATTR_INDICATE) ||
        (p_model_msg->opid == GENIE_OP_ATTR_INDICATE_TG) ||
        (p_model_msg->opid == GENIE_OP_ATTR_TRANS_INDICATE))) {
        resend_flag = true;
        ESP_LOGD(TAG, "set resend flag");

        genie_dnode_t *p_node = NULL;
        GENIE_DLIST_FOR_EACH_NODE(&g_vnd_msg_list, p_node) {
            genie_model_msg_t *p_msg = NULL;
            genie_model_msg_node_t *p_msg_node = NULL;

            p_msg_node = CONTAINER_OF(p_node, genie_model_msg_node_t, node);
            p_msg = &p_msg_node->msg;

            if (p_msg->tid == p_model_msg->tid) {
                ESP_LOGI(TAG, "no resend");
                resend_flag = false;
                break;
            }
        }
    }

    // prepare buffer
    uint8_t *data = NULL;
    data = malloc(p_model_msg->len + 1);
    if (!data) {
        ESP_LOGE(TAG, "malloc failed");
        return ESP_FAIL;
    }
    data[0] = p_model_msg->tid;
    memcpy(data + 1, p_model_msg->data, p_model_msg->len);

    p_model_msg->retry--;

    ESP_LOGD(TAG, "p_model_msg->opid: 0x%02x, p_model_msg->data: %p, len: %d, data: 0x%s",
             p_model_msg->opid, p_model_msg, p_model_msg->len, util_hex2str(p_model_msg->data, p_model_msg->len));

    if (resend_flag) {
        ESP_LOGD(TAG, "genie_model_msg_list_append");
        genie_model_msg_list_append(p_model_msg);
    }

    ctx.app_idx  = bt_mesh_model_get_appkey_id(p_model_msg->p_elem, p_model);
    ctx.net_idx  = bt_mesh_model_get_netkey_id(p_model_msg->p_elem);
    ctx.addr     = GENIE_RECV_ADDR;
    ctx.send_ttl = BLE_MESH_TTL_DEFAULT;

    ESP_LOGI(TAG, "vendor message send: tid: 0x%02x, retry: %02d, len: %02d, opcode: 0x%02x, data: 0x%s", p_model_msg->tid, p_model_msg->retry, p_model_msg->len, p_model_msg->opid, util_hex2str(p_model_msg->data, p_model_msg->len));
    ESP_LOGD(TAG, "vendor message send: element: %p, app_idx: %d, net_idx: %d, tid: 0x%02x, retry: %02d, len: %02d, opcode: 0x%02x, data: 0x%s",
             p_model_msg->p_elem, ctx.app_idx, ctx.net_idx, p_model_msg->tid, p_model_msg->retry, p_model_msg->len, p_model_msg->opid, util_hex2str(p_model_msg->data, p_model_msg->len));

    err = esp_ble_mesh_server_model_send_msg(p_model, &ctx, ESP_BLE_MESH_MODEL_OP_3(p_model_msg->opid, CID_ALIBABA), p_model_msg->len + 1, data);

    if (err) {
        ESP_LOGE(TAG, "mesh model send msg, err: %d", err);
    } else {
        ESP_LOGD(TAG, "mesh model send msg success");
    }

    if (p_model_msg->retry == 0) {
        ESP_LOGW(TAG, "The message has been retried 9 times and may be lost. This is the last retry. tid: 0x%02x", p_model_msg->tid);
    }

    free(data);

    return err;
}

/** @def genie_model_analyze
 *
 *  @brief analyze the received message and notify genie SDK
 *
 *  @param pointer to the received message (vendor model, context and the message buffer) and opid
 *
 *  @return if success return 0; if fails return error no.
 */
static esp_err_t genie_model_analyze(esp_ble_mesh_model_t *p_model,
                                   esp_ble_mesh_msg_ctx_t *p_ctx,
                                   struct net_buf_simple *p_buf,
                                   uint8_t opid)
{
    ENTER_FUNC();
    genie_model_msg_t msg = {0};

    if (!p_model || !p_buf) {
        return GENIE_MESH_ANALYZE_ARGS_ERROR;
    }

    if (p_buf->len < 3) {
        ESP_LOGE(TAG, "invalid buf len: %d", p_buf->len);
        return GENIE_MESH_ANALYZE_SIZE_ERROR;
    }

    memset(&msg, 0, sizeof(genie_model_msg_t));

    msg.opid = opid;
    msg.tid  = net_buf_simple_pull_u8(p_buf);

    if (genie_msg_check_tid(p_ctx->addr, msg.tid) != GENIE_MESH_SUCCESS) {
        ESP_LOGE(TAG, "GENIE_MESH_TID_REPEAT_ERROR src_addr: 0x%04x, tid: 0x%02x", p_ctx->addr, msg.tid);
        return GENIE_MESH_TID_REPEAT_ERROR;
    }

    msg.len = p_buf->len;
    ESP_LOGD(TAG, "opcode: 0x%02x, tid: 0x%02x, len: %d", msg.opid, msg.tid, msg.len);

    if (msg.len) {
        msg.data = (uint8_t *)p_buf->data;
        net_buf_simple_pull(p_buf, msg.len);
        ESP_LOGD(TAG, "payload: %s", util_hex2str(msg.data, msg.len));
    } else {
        msg.data = NULL;
    }

    genie_event(GENIE_EVT_SDK_VENDOR_MSG, (void *)&msg);

    return ESP_OK;
}

/** @def genie_model_get_status
 *
 *  @brief handle GENIE_OP_ATTR_GET_STATUS message
 *
 *  @param pointer to the received message (vendor model, context and the message buffer)
 *
 *  @return N/A
 */
static void genie_model_get_status(esp_ble_mesh_model_t *model,
                                   esp_ble_mesh_msg_ctx_t *ctx,
                                   struct net_buf_simple *buf)
{
    ENTER_FUNC();
    genie_model_analyze(model, ctx, buf, GENIE_OP_ATTR_GET_STATUS);
}

/** @def genie_model_set_ack
 *
 *  @brief handle GENIE_OP_ATTR_SET_ACK message
 *
 *  @param pointer to the received message (vendor model, context and the message buffer)
 *
 *  @return N/A
 */
static void genie_model_set_ack(esp_ble_mesh_model_t *model,
                                esp_ble_mesh_msg_ctx_t *ctx,
                                struct net_buf_simple *buf)
{
    ENTER_FUNC();
    genie_model_analyze(model, ctx, buf, GENIE_OP_ATTR_SET_ACK);
}

/** @def genie_model_set_unack
 *
 *  @brief handle GENIE_OP_ATTR_SET_UNACK message
 *
 *  @param pointer to the received message (vendor model, context and the message buffer)
 *
 *  @return N/A
 */
static void genie_model_set_unack(esp_ble_mesh_model_t *model,
                                  esp_ble_mesh_msg_ctx_t *ctx,
                                  struct net_buf_simple *buf)
{
    ENTER_FUNC();
    genie_model_analyze(model, ctx, buf, GENIE_OP_ATTR_SET_UNACK);
}

/** @def genie_model_confirm
 *
 *  @brief handle GENIE_OP_ATTR_CONFIME message
 *
 *  @param pointer to the received message (vendor model, context and the message buffer)
 *
 *  @return N/A
 */
static void genie_model_confirm(esp_ble_mesh_model_t *model,
                                esp_ble_mesh_msg_ctx_t *ctx,
                                struct net_buf_simple *buf)
{
    ENTER_FUNC();
    uint8_t tid = 0x0;

    if (buf->len != 1) {
        ESP_LOGE(TAG, "invalid buf len: %d", buf->len);
        return;
    }

    tid = net_buf_simple_pull_u8(buf);
    ESP_LOGI(TAG, "confirm tid: 0x%02x", tid);
    genie_model_msg_check_tid(&g_vnd_msg_list, tid);
}

/** @def genie_model_confirm_tg
 *
 *  @brief handle GENIE_OP_ATTR_CONFIME_TG message
 *
 *  @param pointer to the received message (vendor model, context and the message buffer)
 *
 *  @return N/A
 */
static void genie_model_confirm_tg(esp_ble_mesh_model_t *model,
                                   esp_ble_mesh_msg_ctx_t *ctx,
                                   struct net_buf_simple *buf)
{
    ENTER_FUNC();
    uint8_t tid = 0x0;

    if (buf->len != 1) {
        ESP_LOGE(TAG, "invalid buf len: %d", buf->len);
        return;
    }

    tid = net_buf_simple_pull_u8(buf);
    ESP_LOGI(TAG, "confirm_tg tid: 0x%02x", tid);
    genie_model_msg_check_tid(&g_vnd_msg_list, tid);
}

/** @def genie_model_transparent
 *
 *  @brief handle GENIE_OP_ATTR_TRANS_MSG message
 *
 *  @param pointer to the received message (vendor model, context and the message buffer)
 *
 *  @return N/A
 */
static void genie_model_transparent(esp_ble_mesh_model_t *model,
                                    esp_ble_mesh_msg_ctx_t *ctx,
                                    struct net_buf_simple *buf)
{
    ENTER_FUNC();
    genie_model_analyze(model, ctx, buf, GENIE_OP_ATTR_TRANS_MSG);
}

/** @def genie_model_transparent_ack
 *
 *  @brief handle GENIE_OP_ATTR_TRANS_ACK message
 *
 *  @param pointer to the received message (vendor model, context and the message buffer)
 *
 *  @return N/A
 */
static void genie_model_transparent_ack(esp_ble_mesh_model_t *model,
                                        esp_ble_mesh_msg_ctx_t *ctx,
                                        struct net_buf_simple *buf)
{
    ENTER_FUNC();
    uint8_t tid = 0x0;

    if (buf->len != 1) {
        ESP_LOGE(TAG, "invalid buf len: %d", buf->len);
        return;
    }

    tid = net_buf_simple_pull_u8(buf);
    genie_model_msg_check_tid(&g_vnd_msg_list, tid);
}

static genie_opcode_cb_t genie_opcode_cb[] = {
    {GENIE_MESSAGE_OP_ATTR_GET_STATUS,         genie_model_get_status},
    {GENIE_MESSAGE_OP_ATTR_SET_ACK,            genie_model_set_ack},
    {GENIE_MESSAGE_OP_ATTR_SET_UNACK,          genie_model_set_unack},
    {GENIE_MESSAGE_OP_ATTR_CONFIRMATION,       genie_model_confirm},
    {GENIE_MESSAGE_OP_ATTR_CONFIRMATION_TG,    genie_model_confirm_tg},
    {GENIE_MESSAGE_OP_ATTR_TRANSPARENT_MSG,    genie_model_transparent},
    {GENIE_MESSAGE_OP_ATTR_TRANSPARENT_ACK,    genie_model_transparent_ack},
};

/**
 * @brief Dispatch message according opcode.
 *
 * @param opcode
 * @param model
 * @param ctx
 * @param msg
 * @param length
 */
void genie_model_dispatch(uint32_t opcode, esp_ble_mesh_model_t *model,
                          esp_ble_mesh_msg_ctx_t *ctx, uint8_t *msg, uint16_t length)
{
    ENTER_FUNC();

    NET_BUF_SIMPLE_DEFINE(buf, length);
    memcpy(buf.__buf, msg, length);
    buf.len = length;

    for (uint8_t i = 0; i < ARRAY_SIZE(genie_opcode_cb); i++) {
        if (genie_opcode_cb[i].opcode == opcode) {
            genie_opcode_cb[i].cb(model, ctx, &buf);
            return;
        }
    }
    ESP_LOGW(TAG, "not find callback function for opcode: 0x%02" PRIu32, opcode);
}
