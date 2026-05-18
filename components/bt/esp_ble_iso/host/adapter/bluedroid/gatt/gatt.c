/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <zephyr/autoconf.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>

#include <../host/conn_internal.h>

#include "bta/bta_api.h"
#include "bta/bta_gatt_api.h"
#include "bta/bta_gatt_common.h"
#include "bta_gattc_int.h"
#include "btc_gatt_util.h"
#include "stack/btm_ble_api.h"

#include "common/host.h"
#include "common/app/gap.h"
#include "common/app/gatt.h"

LOG_MODULE_REGISTER(ISO_BGAT, CONFIG_BT_ISO_LOG_LEVEL);

/* Use UUID with a fixed pattern 0x98 for ISO & LE Audio GATT Server.
 * 0x96/0x97 are reserved by BLE Mesh; pick a distinct byte for any new
 * module that follows this pattern. */
#define GATTS_APP_UUID_BYTE     0x98

static const tBT_UUID gatts_app_uuid = {
    .len = LEN_UUID_128,
    .uu.uuid128 = {
        [0 ... 15] = GATTS_APP_UUID_BYTE,
    },
};
static tBTA_GATTS_IF gatts_if;

/* Use UUID with a fixed pattern 0x99 for ISO & LE Audio GATT Client */
#define GATTC_APP_UUID_BYTE     0x99

static tBT_UUID gattc_app_uuid = {
    .len = LEN_UUID_128,
    .uu.uuid128 = {
        [0 ... 15] = GATTC_APP_UUID_BYTE,
    },
};
static tBTA_GATTC_IF gattc_if;

static struct gatts_svc_cb *gatts_svc_cb;

static struct gatt_conn gatt_conns[CONFIG_BT_MAX_CONN];

/* Sems block bt_le_bluedroid_gatt_init() until the BTA app registrations
 * report back. gatts_sem is also reused by the audio adapter — only one
 * gatts_svc_cb is registered at a time, so sequential reuse is safe. */
static struct k_sem gatts_sem;
static struct k_sem gattc_sem;

/* Set by deinit before deleting the sems so a late BTA_*_REG_EVT skips the
 * give on a deleted handle. Same accepted residual race as hci.c's
 * direct_hci_shutting_down; the init-timeout window is near-unreachable. */
static volatile bool gatt_shutting_down;

enum {
    GATTC_OP_READ,
    GATTC_OP_WRITE,
};

/* One in-flight GATTC op (read or write). FIFO matches BTA p_cmd_list. */
struct gattc_list_node {
    sys_snode_t node;
    uint8_t type;
    union {
        struct bt_gatt_read_params  *read_params;
        struct bt_gatt_write_params *write_params;
    };
};

static struct gattc_list_node *gattc_list_node_alloc(uint8_t type, void *params)
{
    struct gattc_list_node *op;

    op = calloc(1, sizeof(*op));
    if (op == NULL) {
        return NULL;
    }

    op->type = type;
    /* Both arms of the union are pointer-typed and same size — assigning via
     * either field is equivalent. */
    op->read_params = params;

    return op;
}

static void gattc_list_drain(struct gatt_conn *gatt_conn)
{
    struct gattc_list_node *op;
    sys_snode_t *snode;

    while ((snode = sys_slist_get(&gatt_conn->gattc_list)) != NULL) {
        op = CONTAINER_OF(snode, struct gattc_list_node, node);

        LOG_DBG("[B]GattcOpDrain[%u]", op->type);
        /* lib clears its own state via disconn cb; don't invoke params->func
         * here (BTA also doesn't deliver cmpl_evt for queued ops on close). */
        free(op);
    }
}

/* One pending indication. Deep-copies params + data because the caller may
 * reuse the params slot or pass a stack-backed data buffer while we are
 * queued behind another in-flight indication. Mirrors NimBLE NRP. */
struct gatts_list_node {
    sys_snode_t node;
    struct bt_gatt_indicate_params params_copy;
    uint16_t value_handle;   /* resolved value handle for BTA dispatch */
    uint8_t *data_copy;      /* NULL if params->len == 0 */
};

/* Resolve attr (possibly by uuid lookup), validate INDICATE property for
 * CHRC declarations, and emit the value handle to dispatch on. */
static int gatts_indicate_resolve(const struct bt_gatt_attr *attr_in,
                                  const struct bt_uuid *uuid,
                                  const struct bt_gatt_attr **resolved_attr,
                                  uint16_t *value_handle)
{
    struct bt_gatt_chrc *chrc;
    struct notify_data data;
    uint16_t handle;

    memset(&data, 0, sizeof(data));
    data.attr = attr_in;
    data.handle = bt_gatt_attr_get_handle(data.attr);

    if (uuid) {
        if (bt_gatts_find_attr_by_uuid(&data, uuid) == false) {
            return -ENOENT;
        }
    } else if (data.handle == 0) {
        return -ENOENT;
    }

    handle = data.handle;

    if (bt_uuid_cmp(data.attr->uuid, BT_UUID_GATT_CHRC) == 0) {
        assert(data.attr->user_data);
        chrc = data.attr->user_data;

        if ((chrc->properties & BT_GATT_CHRC_INDICATE) == 0) {
            return -EINVAL;
        }

        handle = bt_gatt_attr_value_handle(data.attr);
    }

    *resolved_attr = data.attr;
    *value_handle = handle;

    return 0;
}

static struct gatts_list_node *gatts_list_node_alloc(struct bt_gatt_indicate_params *ip,
                                                     const struct bt_gatt_attr *attr,
                                                     uint16_t value_handle)
{
    struct gatts_list_node *n;

    n = calloc(1, sizeof(*n));
    if (n == NULL) {
        return NULL;
    }

    n->params_copy = *ip;
    n->params_copy.attr = attr;        /* freeze the resolved attr in the copy */
    n->value_handle = value_handle;
    n->data_copy = NULL;

    if (ip->len > 0) {
        assert(ip->data);

        n->data_copy = malloc(ip->len);
        if (n->data_copy == NULL) {
            free(n);
            return NULL;
        }

        memcpy(n->data_copy, ip->data, ip->len);
    }
    /* lib sees the copy in func cb, so point copy's data at our backing buf */
    n->params_copy.data = n->data_copy;

    return n;
}

static void gatts_list_node_free(struct gatts_list_node *n)
{
    if (n == NULL) {
        return;
    }

    free(n->data_copy);
    free(n);
}

/* Marker for one in-flight notify. BTA fires CONF_EVT for notify too — we
 * push one of these per send and pop in handle_gatts_notify_tx_event to keep
 * the indication queue (gatts_list) unmolested. */
struct gatts_notify_node {
    sys_snode_t node;
    uint16_t value_handle;
};

static void gatts_notify_list_drain(struct gatt_conn *gatt_conn)
{
    struct gatts_notify_node *n;
    sys_snode_t *snode;

    while ((snode = sys_slist_get(&gatt_conn->gatts_notify_list)) != NULL) {
        n = CONTAINER_OF(snode, struct gatts_notify_node, node);
        free(n);
    }
}

static void gatts_indicate_dispatch(struct bt_conn *conn,
                                    struct gatts_list_node *n)
{
    uint16_t conn_id = BTC_GATT_CREATE_CONN_ID(gatts_if, conn->handle);

    LOG_DBG("[B]GattsIndDispatch[%u][%u]", conn->handle, n->value_handle);

    BTA_GATTS_HandleValueIndication(conn_id, n->value_handle,
                                    n->params_copy.len,
                                    n->data_copy, true);
}

/* Silent drain — used by reset_gatt_conn as a safety net. The disconnect
 * handler is expected to have already fired func for each pending node
 * (mirrors NimBLE stack's ble_gatts_indicate_fail_notconn on disconn). */
static void gatts_list_drain(struct gatt_conn *gatt_conn)
{
    struct gatts_list_node *n;
    sys_snode_t *snode;

    while ((snode = sys_slist_get(&gatt_conn->gatts_list)) != NULL) {
        n = CONTAINER_OF(snode, struct gatts_list_node, node);

        LOG_DBG("[B]GattsIndDrain[%u]", n->value_handle);
        gatts_list_node_free(n);
    }
}

uint8_t bt_le_bluedroid_gattc_get_if(void)
{
    /* BTA assigns a positive client_if on successful registration; 0 means
     * the engine hasn't completed BTA_GATTC_AppRegister yet. Returning
     * 0xFF (ESP_GATT_IF_NONE) signals the caller to bail rather than feed
     * a junk handle into esp_ble_gattc_aux_open() / etc. */
    if (gattc_if == 0) {
        LOG_WRN("[B]GetGattcIfBeforeInit");
        return 0xFF; /* ESP_GATT_IF_NONE */
    }

    return gattc_if;
}

uint8_t bt_le_bluedroid_gatts_get_if(void)
{
    return gatts_if;
}

void bt_le_bluedroid_gatts_sem_reset(void)
{
    /* Must run BEFORE the BTA op that triggers the producer. Drains any
     * stale give from a prior timed-out op and clears .result so the
     * next take reads exactly what this op's producer writes. */
    k_sem_reset(&gatts_sem);
}

int bt_le_bluedroid_gatts_sem_take(void)
{
    int rc;

    rc = k_sem_take(&gatts_sem, K_SEM_SHORT);
    if (rc) {
        return rc;
    }

    return gatts_sem.result;
}

void bt_le_bluedroid_gatts_sem_give(int result)
{
    gatts_sem.result = result;
    k_sem_give(&gatts_sem);
}

struct gatt_conn *bt_le_bluedroid_find_gatt_conn_with_addr(uint8_t addr_type,
                                                           const uint8_t addr[6],
                                                           bool ignore_type)
{
    for (size_t i = 0; i < ARRAY_SIZE(gatt_conns); i++) {
        if (gatt_conns[i].used &&
                (ignore_type || gatt_conns[i].peer.type == addr_type) &&
                memcmp(gatt_conns[i].peer.val, addr, BT_ADDR_SIZE) == 0) {
            return &gatt_conns[i];
        }
    }

    return NULL;
}

struct gatt_conn *bt_le_bluedroid_find_gatt_conn_with_handle(uint16_t conn_handle)
{
    for (size_t i = 0; i < ARRAY_SIZE(gatt_conns); i++) {
        if (gatt_conns[i].used &&
                gatt_conns[i].conn_handle == conn_handle) {
            return &gatt_conns[i];
        }
    }

    return NULL;
}

struct gatt_conn *bt_le_bluedroid_find_free_gatt_conn(void)
{
    for (size_t i = 0; i < ARRAY_SIZE(gatt_conns); i++) {
        if (gatt_conns[i].used == 0) {
            memset(&gatt_conns[i], 0, sizeof(gatt_conns[i]));
            return &gatt_conns[i];
        }
    }

    return NULL;
}

static void reset_gatt_conn(struct gatt_conn *gatt_conn)
{
    gattc_list_drain(gatt_conn);
    gatts_list_drain(gatt_conn);
    gatts_notify_list_drain(gatt_conn);
    memset(gatt_conn, 0, sizeof(*gatt_conn));
    gatt_conn->conn_handle = UINT16_MAX;
}

/* Bluedroid surfaces a 16-bit disconnect reason mixing 8-bit HCI codes with
 * host-side extensions (gatt_api.h: GATT_CONN_CANCEL/_NONE). Downstream cb
 * mirrors Zephyr's uint8_t signature — map the extensions to the closest HCI
 * Vol 1 Part F equivalent rather than letting them silently truncate to 0x00
 * (which would look like a graceful disconnect). */
static uint8_t bta_disconn_reason_to_hci(uint16_t reason)
{
    if (reason <= 0xFF) {
        return (uint8_t)reason;
    }

    switch (reason) {
    case 0x0100:                  /* GATT_CONN_CANCEL — local-initiated cancel.
                                   * Macro aliases L2CAP_CONN_CANCEL whose def
                                   * lives in l2cdefs.h (not transitively
                                   * included), so use the numeric literal. */
        return HCI_ERR_CONN_CAUSE_LOCAL_HOST;   /* 0x16 */
    case BTA_GATT_CONN_NONE:      /* 0x0101 — no link to cancel */
        return HCI_ERR_NO_CONNECTION;           /* 0x02 */
    default:
        LOG_WRN("[B]UnknownDisconnReason[0x%04x]", reason);
        return HCI_ERR_UNSPECIFIED;             /* 0x1F */
    }
}

static void gattc_connect_event_handler(tBTA_GATTC_CONNECT *connect)
{
    struct bt_le_gatt_event_param *qev;
    int err;

    qev = calloc(1, sizeof(*qev));
    assert(qev);

    qev->type = BT_LE_GATTC_CONNECT_EVENT;

    qev->gattc_connect.conn_handle = BTC_GATT_GET_CONN_ID(connect->conn_id);
    qev->gattc_connect.role = connect->link_role;
    qev->gattc_connect.peer.type = connect->ble_addr_type;
    memcpy(qev->gattc_connect.peer.val, connect->remote_bda, BT_ADDR_SIZE);

    err = bt_le_iso_task_post(ISO_QUEUE_ITEM_TYPE_GATT_EVENT, qev, sizeof(*qev));
    if (err) {
        LOG_ERR("[B]GattcConnPostFail[%d]", err);
        free(qev);
    }
}

static void gattc_disconnect_event_handler(tBTA_GATTC_DISCONNECT *disconnect)
{
    struct bt_le_gatt_event_param *qev;
    int err;

    qev = calloc(1, sizeof(*qev));
    assert(qev);

    qev->type = BT_LE_GATTC_DISCONNECT_EVENT;

    qev->gattc_disconnect.conn_handle = BTC_GATT_GET_CONN_ID(disconnect->conn_id);
    qev->gattc_disconnect.reason = bta_disconn_reason_to_hci(disconnect->reason);

    err = bt_le_iso_task_post(ISO_QUEUE_ITEM_TYPE_GATT_EVENT, qev, sizeof(*qev));
    if (err) {
        LOG_ERR("[B]GattcDiscPostFail[%d]", err);
        free(qev);
    }
}

static void gattc_open_event_handler(tBTA_GATTC_OPEN *open)
{
    struct bt_le_gatt_event_param *qev;
    int err;

    qev = calloc(1, sizeof(*qev));
    assert(qev);

    qev->type = BT_LE_GATTC_OPEN_EVENT;

    qev->gattc_open.status = open->status;
    qev->gattc_open.conn_handle = BTC_GATT_GET_CONN_ID(open->conn_id);

    err = bt_le_iso_task_post(ISO_QUEUE_ITEM_TYPE_GATT_EVENT, qev, sizeof(*qev));
    if (err) {
        LOG_ERR("[B]GattcOpenPostFail[%d]", err);
        free(qev);
    }
}

static void gattc_mtu_event_handler(tBTA_GATTC_CFG_MTU *cfg_mtu)
{
    struct bt_le_gatt_event_param *qev;
    int err;

    qev = calloc(1, sizeof(*qev));
    assert(qev);

    qev->type = BT_LE_GATTC_MTU_EVENT;

    qev->gattc_mtu.status = cfg_mtu->status;
    qev->gattc_mtu.conn_handle = BTC_GATT_GET_CONN_ID(cfg_mtu->conn_id);
    qev->gattc_mtu.mtu = cfg_mtu->mtu;

    err = bt_le_iso_task_post(ISO_QUEUE_ITEM_TYPE_GATT_EVENT, qev, sizeof(*qev));
    if (err) {
        LOG_ERR("[B]GattcMtuPostFail[%d]", err);
        free(qev);
    }
}

static void gattc_disc_cmpl_event_handler(tBTA_GATTC_DIS_CMPL *disc_cmpl)
{
    struct bt_le_gatt_event_param *qev;
    int err;

    qev = calloc(1, sizeof(*qev));
    assert(qev);

    qev->type = BT_LE_GATTC_DISC_CMPL_EVENT;

    qev->gattc_disc_cmpl.status = disc_cmpl->status;
    qev->gattc_disc_cmpl.conn_handle = BTC_GATT_GET_CONN_ID(disc_cmpl->conn_id);

    err = bt_le_iso_task_post(ISO_QUEUE_ITEM_TYPE_GATT_EVENT, qev, sizeof(*qev));
    if (err) {
        LOG_ERR("[B]GattcDiscCmplPostFail[%d]", err);
        free(qev);
    }
}

static void gattc_read_chrc_event_handler(tBTA_GATTC_READ *read)
{
    struct bt_le_gatt_event_param *qev;
    int err;

    qev = calloc(1, sizeof(*qev));
    assert(qev);

    qev->type = BT_LE_GATTC_READ_CHRC_EVENT;

    qev->gattc_read_chrc.status = read->status;
    qev->gattc_read_chrc.conn_handle = BTC_GATT_GET_CONN_ID(read->conn_id);
    qev->gattc_read_chrc.attr_handle = read->handle;

    if (read->p_value &&
            read->p_value->len &&
            read->p_value->p_value) {
        qev->gattc_read_chrc.len = read->p_value->len;

        qev->gattc_read_chrc.value = calloc(1, read->p_value->len);
        assert(qev->gattc_read_chrc.value);

        memcpy(qev->gattc_read_chrc.value, read->p_value->p_value, read->p_value->len);
    }

    err = bt_le_iso_task_post(ISO_QUEUE_ITEM_TYPE_GATT_EVENT, qev, sizeof(*qev));
    if (err) {
        LOG_ERR("[B]GattcReadChrcPostFail[%d]", err);
        if (qev->gattc_read_chrc.value) {
            free(qev->gattc_read_chrc.value);
        }
        free(qev);
    }
}

static void gattc_write_chrc_event_handler(tBTA_GATTC_WRITE *write)
{
    struct bt_le_gatt_event_param *qev;
    int err;

    qev = calloc(1, sizeof(*qev));
    assert(qev);

    qev->type = BT_LE_GATTC_WRITE_CHRC_EVENT;

    qev->gattc_write_chrc.status = write->status;
    qev->gattc_write_chrc.conn_handle = BTC_GATT_GET_CONN_ID(write->conn_id);
    qev->gattc_write_chrc.attr_handle = write->handle;
    qev->gattc_write_chrc.offset = write->offset;

    err = bt_le_iso_task_post(ISO_QUEUE_ITEM_TYPE_GATT_EVENT, qev, sizeof(*qev));
    if (err) {
        LOG_ERR("[B]GattcWriteChrcPostFail[%d]", err);
        free(qev);
    }
}

static void gatts_notify_tx_event_handler(tBTA_GATTS_REQ *req)
{
    struct bt_le_gatt_event_param *qev;
    int err;

    qev = calloc(1, sizeof(*qev));
    assert(qev);

    qev->type = BT_LE_GATTS_NOTIFY_TX_EVENT;

    /* BTA fires CONF_EVT for both indication acks and immediate notify
     * completions; tBTA_GATTS_REQ carries no flag to tell them apart. The
     * bluedroid handler disambiguates by popping a parallel notify marker
     * list first (see gatts_notify_enqueue / handle_gatts_notify_tx_event),
     * so is_notify here is left unused on this path. */
    qev->gatts_notify_tx.is_notify = false;
    qev->gatts_notify_tx.conn_handle = BTC_GATT_GET_CONN_ID(req->conn_id);
    qev->gatts_notify_tx.attr_handle = req->handle;
    qev->gatts_notify_tx.status = req->status;

    err = bt_le_iso_task_post(ISO_QUEUE_ITEM_TYPE_GATT_EVENT, qev, sizeof(*qev));
    if (err) {
        LOG_ERR("[B]GattsNotifyTxPostFail[%d]", err);
        free(qev);
    }
}

static void gattc_notify_rx_event_handler(tBTA_GATTC_NOTIFY *notify)
{
    struct bt_le_gatt_event_param *qev;
    int err;

    qev = calloc(1, sizeof(*qev));
    assert(qev);

    qev->type = BT_LE_GATTC_NOTIFY_RX_EVENT;

    qev->gattc_notify_rx.is_notify = notify->is_notify;
    qev->gattc_notify_rx.conn_handle = BTC_GATT_GET_CONN_ID(notify->conn_id);
    qev->gattc_notify_rx.attr_handle = notify->handle;

    if (notify->len) {
        qev->gattc_notify_rx.len = notify->len;

        qev->gattc_notify_rx.value = calloc(1, notify->len);
        assert(qev->gattc_notify_rx.value);

        memcpy(qev->gattc_notify_rx.value, notify->value, notify->len);
    }

    err = bt_le_iso_task_post(ISO_QUEUE_ITEM_TYPE_GATT_EVENT, qev, sizeof(*qev));
    if (err) {
        LOG_ERR("[B]GattcNotifyRxPostFail[%d]", err);
        if (qev->gattc_notify_rx.value) {
            free(qev->gattc_notify_rx.value);
        }
        free(qev);
    }
}

static void gatts_connect_event_handler(tBTA_GATTS_CONN *connect)
{
    struct bt_le_gatt_event_param *qev;
    int err;

    qev = calloc(1, sizeof(*qev));
    assert(qev);

    qev->type = BT_LE_GATTS_CONNECT_EVENT;

    qev->gatts_connect.conn_handle = BTC_GATT_GET_CONN_ID(connect->conn_id);
    qev->gatts_connect.role = connect->link_role;
    qev->gatts_connect.peer.type = connect->ble_addr_type;
    memcpy(qev->gatts_connect.peer.val, connect->remote_bda, BT_ADDR_SIZE);

    err = bt_le_iso_task_post(ISO_QUEUE_ITEM_TYPE_GATT_EVENT, qev, sizeof(*qev));
    if (err) {
        LOG_ERR("[B]GattsConnPostFail[%d]", err);
        free(qev);
    }
}

static void gatts_disconnect_event_handler(tBTA_GATTS_CONN *disconnect)
{
    struct bt_le_gatt_event_param *qev;
    int err;

    qev = calloc(1, sizeof(*qev));
    assert(qev);

    qev->type = BT_LE_GATTS_DISCONNECT_EVENT;

    qev->gatts_disconnect.conn_handle = BTC_GATT_GET_CONN_ID(disconnect->conn_id);
    qev->gatts_disconnect.reason = bta_disconn_reason_to_hci(disconnect->reason);

    err = bt_le_iso_task_post(ISO_QUEUE_ITEM_TYPE_GATT_EVENT, qev, sizeof(*qev));
    if (err) {
        LOG_ERR("[B]GattsDiscPostFail[%d]", err);
        free(qev);
    }
}

static void gatts_mtu_event_handler(tBTA_GATTS_REQ *req)
{
    struct bt_le_gatt_event_param *qev;
    int err;

    /* req->p_data is non-NULL here: BTA always passes a stack object. */
    qev = calloc(1, sizeof(*qev));
    assert(qev);

    qev->type = BT_LE_GATTS_MTU_EVENT;

    qev->gatts_mtu.conn_handle = BTC_GATT_GET_CONN_ID(req->conn_id);
    qev->gatts_mtu.mtu = req->p_data->mtu;

    err = bt_le_iso_task_post(ISO_QUEUE_ITEM_TYPE_GATT_EVENT, qev, sizeof(*qev));
    if (err) {
        LOG_ERR("[B]GattsMtuPostFail[%d]", err);
        free(qev);
    }
}

static void gatts_read_req_handler(tBTA_GATTS_REQ *req)
{
    struct bt_le_gatt_event_param *qev;
    int err;

    qev = calloc(1, sizeof(*qev));
    assert(qev);

    qev->type = BT_LE_GATTS_READ_EVENT;

    qev->gatts_read.conn_handle = BTC_GATT_GET_CONN_ID(req->conn_id);
    qev->gatts_read.trans_id = req->trans_id;
    memcpy(qev->gatts_read.peer, req->remote_bda, BT_ADDR_SIZE);
    qev->gatts_read.attr_handle = req->p_data->read_req.handle;
    qev->gatts_read.offset = req->p_data->read_req.offset;
    qev->gatts_read.is_long = req->p_data->read_req.is_long;
    qev->gatts_read.need_rsp = req->p_data->read_req.need_rsp;

    err = bt_le_iso_task_post(ISO_QUEUE_ITEM_TYPE_GATT_EVENT, qev, sizeof(*qev));
    if (err) {
        LOG_ERR("[B]GattsReadPostFail[%d]", err);
        free(qev);
    }
}

static void gatts_write_req_handler(tBTA_GATTS_REQ *req)
{
    struct bt_le_gatt_event_param *qev;
    int err;

    /* req->p_data is non-NULL here: BTA always passes a stack object. */
    qev = calloc(1, sizeof(*qev));
    assert(qev);

    qev->type = BT_LE_GATTS_WRITE_EVENT;

    qev->gatts_write.conn_handle = BTC_GATT_GET_CONN_ID(req->conn_id);
    qev->gatts_write.trans_id = req->trans_id;
    memcpy(qev->gatts_write.peer, req->remote_bda, BT_ADDR_SIZE);
    qev->gatts_write.attr_handle = req->p_data->write_req.handle;
    qev->gatts_write.offset = req->p_data->write_req.offset;
    qev->gatts_write.is_prep = req->p_data->write_req.is_prep;
    qev->gatts_write.need_rsp = req->p_data->write_req.need_rsp;

    if (req->p_data->write_req.len) {
        qev->gatts_write.len = req->p_data->write_req.len;

        qev->gatts_write.value = calloc(1, req->p_data->write_req.len);
        assert(qev->gatts_write.value);

        memcpy(qev->gatts_write.value, req->p_data->write_req.value, req->p_data->write_req.len);
    }

    err = bt_le_iso_task_post(ISO_QUEUE_ITEM_TYPE_GATT_EVENT, qev, sizeof(*qev));
    if (err) {
        LOG_ERR("[B]GattsWritePostFail[%d]", err);
        if (qev->gatts_write.value) {
            free(qev->gatts_write.value);
        }
        free(qev);
    }
}

static void gatts_app_cb(tBTA_GATTS_EVT event, tBTA_GATTS *p_data)
{
    switch (event) {
    case BTA_GATTS_REG_EVT:
        LOG_DBG("[B]GattsRegEvt[%u][%u]",
                p_data->reg_oper.status, p_data->reg_oper.server_if);
        LOG_DBG("[B]GattsRegUuid[%u][%02x][%02x]",
                p_data->reg_oper.uuid.len,
                p_data->reg_oper.uuid.uu.uuid128[0],
                p_data->reg_oper.uuid.uu.uuid128[1]);

        if (gatt_shutting_down) {
            break;
        }

        if (p_data->reg_oper.status == BTA_GATT_OK &&
                memcmp(&p_data->reg_oper.uuid, &gatts_app_uuid, sizeof(tBT_UUID)) == 0) {
            gatts_if = p_data->reg_oper.server_if;
        }

        gatts_sem.result = p_data->reg_oper.status;
        k_sem_give(&gatts_sem);
        break;

    case BTA_GATTS_DEREG_EVT:
        LOG_DBG("[B]GattsDeregEvt[%u]", p_data->reg_oper.server_if);
        break;

    case BTA_GATTS_CONNECT_EVT:
        LOG_INF("[B]GattsConnEvt[%u][0x%04x][0x%03x]",
                p_data->conn.server_if, p_data->conn.conn_id, p_data->conn.conn_handle);
        LOG_DBG("[B]GattsConnBda[%u][%02x:%02x:%02x:%02x:%02x:%02x]",
                p_data->conn.ble_addr_type,
                p_data->conn.remote_bda[0], p_data->conn.remote_bda[1],
                p_data->conn.remote_bda[2], p_data->conn.remote_bda[3],
                p_data->conn.remote_bda[4], p_data->conn.remote_bda[5]);
        LOG_DBG("[B]GattsConnParams[%u][%u][%u][%u][%u]",
                p_data->conn.transport, p_data->conn.link_role,
                p_data->conn.conn_params.interval, p_data->conn.conn_params.latency,
                p_data->conn.conn_params.timeout);

        if (p_data->conn.server_if != gatts_if) {
            LOG_ERR("[B]GattsConnUnknownIf[%u]", p_data->conn.server_if);
            break;
        }

        if (BTC_GATT_GET_CONN_ID(p_data->conn.conn_id) != p_data->conn.conn_handle) {
            LOG_ERR("[B]GattsConnHdlMismatch[%u][%u]",
                    BTC_GATT_GET_CONN_ID(p_data->conn.conn_id), p_data->conn.conn_handle);
            break;
        }

        gatts_connect_event_handler(&p_data->conn);
        break;

    case BTA_GATTS_DISCONNECT_EVT:
        LOG_INF("[B]GattsDisconnEvt[%u][0x%04x][%u]",
                p_data->conn.server_if, p_data->conn.conn_id, p_data->conn.reason);
        LOG_DBG("[B]GattsDisconnBda[%02x:%02x:%02x:%02x:%02x:%02x]",
                p_data->conn.remote_bda[0], p_data->conn.remote_bda[1],
                p_data->conn.remote_bda[2], p_data->conn.remote_bda[3],
                p_data->conn.remote_bda[4], p_data->conn.remote_bda[5]);

        if (p_data->conn.server_if != gatts_if) {
            LOG_ERR("[B]GattsDisconnUnknownIf[%u]", p_data->conn.server_if);
            break;
        }

        gatts_disconnect_event_handler(&p_data->conn);
        break;

    case BTA_GATTS_OPEN_EVT:
        LOG_INF("[B]GattsOpenEvt[%u][%u]",
                p_data->open.status, p_data->open.server_if);
        break;

    case BTA_GATTS_CANCEL_OPEN_EVT:
        LOG_DBG("[B]GattsCancelOpenEvt[%u][%u]",
                p_data->cancel_open.status, p_data->cancel_open.server_if);
        break;

    case BTA_GATTS_CLOSE_EVT:
        LOG_INF("[B]GattsCloseEvt[%u][0x%04x]",
                p_data->close.status, p_data->close.conn_id);
        break;

    case BTA_GATTS_MTU_EVT:
        LOG_INF("[B]GattsMtuEvt[0x%04x][%u][%u]",
                p_data->req_data.conn_id, p_data->req_data.trans_id,
                p_data->req_data.p_data ? p_data->req_data.p_data->mtu : 0);

        if (BTC_GATT_GET_GATT_IF(p_data->req_data.conn_id) != gatts_if) {
            LOG_ERR("[B]GattsMtuUnknownIf[%u]",
                    BTC_GATT_GET_GATT_IF(p_data->req_data.conn_id));
            break;
        }

        gatts_mtu_event_handler(&p_data->req_data);
        break;

    case BTA_GATTS_DELELTE_EVT:
        LOG_DBG("[B]GattsDeleteEvt[%u][%u][%u]",
                p_data->srvc_oper.status, p_data->srvc_oper.server_if,
                p_data->srvc_oper.service_id);
        break;

    case BTA_GATTS_START_EVT:
        LOG_DBG("[B]GattsStartEvt[%u][%u][%u]",
                p_data->srvc_oper.status, p_data->srvc_oper.server_if,
                p_data->srvc_oper.service_id);

        if (gatts_svc_cb && gatts_svc_cb->svc_start_cb) {
            gatts_svc_cb->svc_start_cb(p_data->srvc_oper.service_id, p_data->srvc_oper.status);
        }
        break;

    case BTA_GATTS_STOP_EVT:
        LOG_DBG("[B]GattsStopEvt[%u][%u][%u]",
                p_data->srvc_oper.status, p_data->srvc_oper.server_if,
                p_data->srvc_oper.service_id);
        break;

    case BTA_GATTS_CREATE_EVT:
        LOG_DBG("[B]GattsCreateEvt[%u][%u]",
                p_data->create.status, p_data->create.server_if);
        LOG_DBG("[B]GattsCreateSvc[%u][%u][%u]",
                p_data->create.service_id, p_data->create.svc_instance,
                p_data->create.is_primary);
        LOG_DBG("[B]GattsCreateUuid[%u][%02x][%02x]",
                p_data->create.uuid.len,
                p_data->create.uuid.uu.uuid128[0],
                p_data->create.uuid.uu.uuid128[1]);

        if (p_data->create.server_if != gatts_if) {
            LOG_ERR("[B]GattsCreateUnknownIf[%u]", p_data->create.server_if);
            break;
        }

        if (gatts_svc_cb && gatts_svc_cb->svc_create_cb) {
            gatts_svc_cb->svc_create_cb(p_data->create.service_id, p_data->create.svc_instance,
                                        p_data->create.is_primary, p_data->create.status,
                                        &p_data->create.uuid);
        }
        break;

    case BTA_GATTS_ADD_INCL_SRVC_EVT:
        LOG_DBG("[B]GattsAddInclSvcEvt[%u][%u][%u][%u]",
                p_data->add_result.status, p_data->add_result.server_if,
                p_data->add_result.attr_id, p_data->add_result.service_id);

        if (p_data->add_result.server_if != gatts_if) {
            LOG_ERR("[B]GattsAddInclSvcUnknownIf[%u]", p_data->add_result.server_if);
            break;
        }

        if (gatts_svc_cb && gatts_svc_cb->inc_svc_add_cb) {
            gatts_svc_cb->inc_svc_add_cb(p_data->add_result.service_id, p_data->add_result.attr_id,
                                         p_data->add_result.status);
        }
        break;

    case BTA_GATTS_ADD_CHAR_EVT:
        LOG_DBG("[B]GattsAddCharEvt[%u][%u][%u][%u]",
                p_data->add_result.status, p_data->add_result.server_if,
                p_data->add_result.attr_id, p_data->add_result.service_id);
        LOG_DBG("[B]GattsAddCharUuid[%u][0x%04x]",
                p_data->add_result.char_uuid.len, p_data->add_result.char_uuid.uu.uuid16);

        if (p_data->add_result.server_if != gatts_if) {
            LOG_ERR("[B]GattsAddCharUnknownIf[%u]", p_data->add_result.server_if);
            break;
        }

        if (gatts_svc_cb && gatts_svc_cb->chrc_add_cb) {
            gatts_svc_cb->chrc_add_cb(p_data->add_result.service_id, p_data->add_result.attr_id,
                                      p_data->add_result.status, &p_data->add_result.char_uuid);
        }
        break;

    case BTA_GATTS_ADD_CHAR_DESCR_EVT:
        LOG_DBG("[B]GattsAddDescrEvt[%u][%u][%u][%u]",
                p_data->add_result.status, p_data->add_result.server_if,
                p_data->add_result.attr_id, p_data->add_result.service_id);
        LOG_DBG("[B]GattsAddDescrUuid[%u][0x%04x]",
                p_data->add_result.char_uuid.len, p_data->add_result.char_uuid.uu.uuid16);

        if (p_data->add_result.server_if != gatts_if) {
            LOG_ERR("[B]GattsAddDescrUnknownIf[%u]", p_data->add_result.server_if);
            break;
        }

        if (gatts_svc_cb && gatts_svc_cb->chrc_add_cb) {
            gatts_svc_cb->chrc_add_cb(p_data->add_result.service_id, p_data->add_result.attr_id,
                                      p_data->add_result.status, &p_data->add_result.char_uuid);
        }
        break;

    case BTA_GATTS_READ_EVT:
        LOG_DBG("[B]GattsReadEvt[0x%04x][%u]",
                p_data->req_data.conn_id, p_data->req_data.trans_id);
        LOG_DBG("[B]GattsReadBda[%02x:%02x:%02x:%02x:%02x:%02x]",
                p_data->req_data.remote_bda[0], p_data->req_data.remote_bda[1],
                p_data->req_data.remote_bda[2], p_data->req_data.remote_bda[3],
                p_data->req_data.remote_bda[4], p_data->req_data.remote_bda[5]);

        /* req_data.p_data is non-NULL here: BTA always passes a stack object. */
        LOG_DBG("[B]GattsReadReq[%u][%u][%u][%u]",
                p_data->req_data.p_data->read_req.handle,
                p_data->req_data.p_data->read_req.offset,
                p_data->req_data.p_data->read_req.is_long,
                p_data->req_data.p_data->read_req.need_rsp);

        if (BTC_GATT_GET_GATT_IF(p_data->req_data.conn_id) != gatts_if) {
            LOG_ERR("[B]GattsReadUnknownIf[%u]",
                    BTC_GATT_GET_GATT_IF(p_data->req_data.conn_id));
            break;
        }

        gatts_read_req_handler(&p_data->req_data);
        break;

    case BTA_GATTS_WRITE_EVT:
        LOG_DBG("[B]GattsWriteEvt[0x%04x][%u]",
                p_data->req_data.conn_id, p_data->req_data.trans_id);
        LOG_DBG("[B]GattsWriteBda[%02x:%02x:%02x:%02x:%02x:%02x]",
                p_data->req_data.remote_bda[0], p_data->req_data.remote_bda[1],
                p_data->req_data.remote_bda[2], p_data->req_data.remote_bda[3],
                p_data->req_data.remote_bda[4], p_data->req_data.remote_bda[5]);
        if (p_data->req_data.p_data) {
            LOG_DBG("[B]GattsWriteReq[%u][%u][%u][%u]",
                    p_data->req_data.p_data->write_req.handle,
                    p_data->req_data.p_data->write_req.offset,
                    p_data->req_data.p_data->write_req.is_prep,
                    p_data->req_data.p_data->write_req.need_rsp);
        }

        if (BTC_GATT_GET_GATT_IF(p_data->req_data.conn_id) != gatts_if) {
            LOG_ERR("[B]GattsWriteUnknownIf[%u]",
                    BTC_GATT_GET_GATT_IF(p_data->req_data.conn_id));
            break;
        }

        gatts_write_req_handler(&p_data->req_data);
        break;

    case BTA_GATTS_EXEC_WRITE_EVT:
        LOG_DBG("[B]GattsExecWriteEvt[0x%04x][%u]",
                p_data->req_data.conn_id, p_data->req_data.trans_id);
        LOG_DBG("[B]GattsExecWriteBda[%02x:%02x:%02x:%02x:%02x:%02x]",
                p_data->req_data.remote_bda[0], p_data->req_data.remote_bda[1],
                p_data->req_data.remote_bda[2], p_data->req_data.remote_bda[3],
                p_data->req_data.remote_bda[4], p_data->req_data.remote_bda[5]);
        if (p_data->req_data.p_data) {
            LOG_DBG("[B]GattsExecFlag[%u]", p_data->req_data.p_data->exec_write);
        }
        break;

    case BTA_GATTS_CONF_EVT:
        LOG_DBG("[B]GattsConfEvt[0x%04x][%u][%u][%u]",
                p_data->req_data.conn_id, p_data->req_data.status,
                p_data->req_data.handle, p_data->req_data.data_len);

        gatts_notify_tx_event_handler(&p_data->req_data);
        break;

    case BTA_GATTS_SET_ATTR_VAL_EVT:
        LOG_DBG("[B]GattsSetAttrValEvt[%u][%u][%u][%u]",
                p_data->attr_val.status, p_data->attr_val.server_if,
                p_data->attr_val.service_id, p_data->attr_val.attr_id);
        break;

    case BTA_GATTS_CONGEST_EVT:
        LOG_DBG("[B]GattsCongestEvt[0x%04x][%u]",
                p_data->congest.conn_id, p_data->congest.congested);
        break;

    case BTA_GATTS_SEND_SERVICE_CHANGE_EVT:
        LOG_DBG("[B]GattsSvcChgEvt[%u][%u]",
                p_data->service_change.status, p_data->service_change.server_if);
        break;

    default:
        break;
    }
}

static void gattc_app_cb(tBTA_GATTC_EVT event, tBTA_GATTC *p_data)
{
    switch (event) {
    case BTA_GATTC_REG_EVT:
        LOG_DBG("[B]GattcRegEvt[%u][%u]",
                p_data->reg_oper.status, p_data->reg_oper.client_if);
        LOG_DBG("[B]GattcRegUuid[%u][%02x][%02x]",
                p_data->reg_oper.app_uuid.len,
                p_data->reg_oper.app_uuid.uu.uuid128[0],
                p_data->reg_oper.app_uuid.uu.uuid128[1]);

        if (gatt_shutting_down) {
            break;
        }

        if (p_data->reg_oper.status == BTA_GATT_OK &&
                memcmp(&p_data->reg_oper.app_uuid, &gattc_app_uuid, sizeof(tBT_UUID)) == 0) {
            gattc_if = p_data->reg_oper.client_if;
        }

        gattc_sem.result = p_data->reg_oper.status;
        k_sem_give(&gattc_sem);
        break;

    case BTA_GATTC_DEREG_EVT:
        LOG_DBG("[B]GattcDeregEvt[%u]", p_data->reg_oper.client_if);
        break;

    case BTA_GATTC_CONNECT_EVT:
        LOG_INF("[B]GattcConnEvt[%u][0x%04x][0x%03x]",
                p_data->connect.client_if, p_data->connect.conn_id, p_data->connect.conn_handle);
        LOG_DBG("[B]GattcConnBda[%u][%02x:%02x:%02x:%02x:%02x:%02x]",
                p_data->connect.ble_addr_type,
                p_data->connect.remote_bda[0], p_data->connect.remote_bda[1],
                p_data->connect.remote_bda[2], p_data->connect.remote_bda[3],
                p_data->connect.remote_bda[4], p_data->connect.remote_bda[5]);
        LOG_DBG("[B]GattcConnParams[%u][%u][%u][%u]",
                p_data->connect.link_role, p_data->connect.conn_params.interval,
                p_data->connect.conn_params.latency, p_data->connect.conn_params.timeout);

        if (p_data->connect.client_if != gattc_if) {
            LOG_ERR("[B]GattcConnUnknownIf[%u]", p_data->connect.client_if);
            break;
        }

        if (BTC_GATT_GET_CONN_ID(p_data->connect.conn_id) != p_data->connect.conn_handle) {
            LOG_ERR("[B]GattcConnHdlMismatch[%u][%u]",
                    BTC_GATT_GET_CONN_ID(p_data->connect.conn_id), p_data->connect.conn_handle);
            break;
        }

        gattc_connect_event_handler(&p_data->connect);
        break;

    case BTA_GATTC_DISCONNECT_EVT:
        LOG_INF("[B]GattcDisconnEvt[%u][0x%04x][%u]",
                p_data->disconnect.client_if, p_data->disconnect.conn_id, p_data->disconnect.reason);
        LOG_DBG("[B]GattcDisconnBda[%02x:%02x:%02x:%02x:%02x:%02x]",
                p_data->disconnect.remote_bda[0], p_data->disconnect.remote_bda[1],
                p_data->disconnect.remote_bda[2], p_data->disconnect.remote_bda[3],
                p_data->disconnect.remote_bda[4], p_data->disconnect.remote_bda[5]);

        if (p_data->disconnect.client_if != gattc_if) {
            LOG_ERR("[B]GattcDisconnUnknownIf[%u]", p_data->disconnect.client_if);
            break;
        }

        gattc_disconnect_event_handler(&p_data->disconnect);
        break;

    case BTA_GATTC_OPEN_EVT:
        LOG_INF("[B]GattcOpenEvt[%u][%u][0x%04x][%u]",
                p_data->open.status, p_data->open.client_if,
                p_data->open.conn_id, p_data->open.mtu);
        LOG_DBG("[B]GattcOpenBda[%02x:%02x:%02x:%02x:%02x:%02x]",
                p_data->open.remote_bda[0], p_data->open.remote_bda[1],
                p_data->open.remote_bda[2], p_data->open.remote_bda[3],
                p_data->open.remote_bda[4], p_data->open.remote_bda[5]);

        if (p_data->open.client_if != gattc_if) {
            LOG_ERR("[B]GattcOpenUnknownIf[%u]", p_data->open.client_if);
            break;
        }

        gattc_open_event_handler(&p_data->open);
        break;

    case BTA_GATTC_CLOSE_EVT:
        LOG_INF("[B]GattcCloseEvt[%u][%u][0x%04x][%u]",
                p_data->close.status, p_data->close.client_if,
                p_data->close.conn_id, p_data->close.reason);
        LOG_DBG("[B]GattcCloseBda[%02x:%02x:%02x:%02x:%02x:%02x]",
                p_data->close.remote_bda[0], p_data->close.remote_bda[1],
                p_data->close.remote_bda[2], p_data->close.remote_bda[3],
                p_data->close.remote_bda[4], p_data->close.remote_bda[5]);

        bta_gattc_clcb_dealloc_by_conn_id(p_data->close.conn_id);
        break;

    case BTA_GATTC_CFG_MTU_EVT:
        LOG_INF("[B]GattcCfgMtuEvt[%u][0x%04x][%u]",
                p_data->cfg_mtu.status, p_data->cfg_mtu.conn_id, p_data->cfg_mtu.mtu);

        if (BTC_GATT_GET_GATT_IF(p_data->cfg_mtu.conn_id) != gattc_if) {
            LOG_ERR("[B]GattcMtuUnknownIf[%u]",
                    BTC_GATT_GET_GATT_IF(p_data->cfg_mtu.conn_id));
            break;
        }

        if (p_data->cfg_mtu.status != BTA_GATT_OK) {
            LOG_WRN("[B]GattcMtuExchFail[%u]", p_data->cfg_mtu.status);
            /* Spec: when MTU exchange fails the link keeps the default
             * MTU. Override here so handle_gattc_mtu_event records 23 in
             * gatt_conn->mtu instead of leaving it at 0, which would
             * make get_mtu() fall back to the local-configured max. */
            p_data->cfg_mtu.mtu = GATT_DEF_BLE_MTU_SIZE;
        }

        gattc_mtu_event_handler(&p_data->cfg_mtu);
        break;

    case BTA_GATTC_DIS_SRVC_CMPL_EVT:
        LOG_INF("[B]GattcDiscSvcCmplEvt[%u][0x%04x]",
                p_data->dis_cmpl.status, p_data->dis_cmpl.conn_id);

        if (BTC_GATT_GET_GATT_IF(p_data->dis_cmpl.conn_id) != gattc_if) {
            LOG_ERR("[B]GattcDiscSvcCmplUnknownIf[%u]",
                    BTC_GATT_GET_GATT_IF(p_data->dis_cmpl.conn_id));
            break;
        }

        gattc_disc_cmpl_event_handler(&p_data->dis_cmpl);
        break;

    case BTA_GATTC_SEARCH_RES_EVT:
        LOG_INF("[B]GattcSearchResEvt[0x%04x]", p_data->srvc_res.conn_id);
        LOG_DBG("[B]GattcSvcResHdls[%u][%u][%u]",
                p_data->srvc_res.start_handle, p_data->srvc_res.end_handle,
                p_data->srvc_res.is_primary);
        LOG_DBG("[B]GattcSvcResUuid[%u][0x%04x][%u]",
                p_data->srvc_res.service_uuid.uuid.len,
                p_data->srvc_res.service_uuid.uuid.uu.uuid16,
                p_data->srvc_res.service_uuid.inst_id);

        if (BTC_GATT_GET_GATT_IF(p_data->srvc_res.conn_id) != gattc_if) {
            LOG_ERR("[B]GattcSearchResUnknownIf[%u]",
                    BTC_GATT_GET_GATT_IF(p_data->srvc_res.conn_id));
            break;
        }
        break;

    case BTA_GATTC_SEARCH_CMPL_EVT:
        LOG_INF("[B]GattcSearchCmplEvt[%u][0x%04x][%s]",
                p_data->search_cmpl.status, p_data->search_cmpl.conn_id,
                (p_data->search_cmpl.searched_service_source ==
                 BTA_GATTC_SERVICE_INFO_FROM_REMOTE_DEVICE) ? "Remote" : "NVS");

        if (BTC_GATT_GET_GATT_IF(p_data->search_cmpl.conn_id) != gattc_if) {
            LOG_ERR("[B]GattcSearchCmplUnknownIf[%u]",
                    BTC_GATT_GET_GATT_IF(p_data->search_cmpl.conn_id));
            break;
        }
        break;

    case BTA_GATTC_READ_CHAR_EVT:
        LOG_INF("[B]GattcReadCharEvt[%u][0x%04x][%u]",
                p_data->read.status, p_data->read.conn_id, p_data->read.handle);

        if (BTC_GATT_GET_GATT_IF(p_data->read.conn_id) != gattc_if) {
            LOG_ERR("[B]GattcReadCharUnknownIf[%u]",
                    BTC_GATT_GET_GATT_IF(p_data->read.conn_id));
            break;
        }

        gattc_read_chrc_event_handler(&p_data->read);
        break;

    case BTA_GATTC_READ_DESCR_EVT:
        LOG_DBG("[B]GattcReadDescrEvt[%u][0x%04x][%u]",
                p_data->read.status, p_data->read.conn_id, p_data->read.handle);
        break;

    case BTA_GATTC_READ_MULTIPLE_EVT:
        LOG_DBG("[B]GattcReadMultiEvt[%u][0x%04x][%u]",
                p_data->read.status, p_data->read.conn_id, p_data->read.handle);
        break;

    case BTA_GATTC_READ_MULTI_VAR_EVT:
        LOG_DBG("[B]GattcReadMultiVarEvt[%u][0x%04x][%u]",
                p_data->read.status, p_data->read.conn_id, p_data->read.handle);
        break;

    case BTA_GATTC_WRITE_CHAR_EVT:
        LOG_INF("[B]GattcWriteCharEvt[%u][0x%04x][%u][%u]",
                p_data->write.status, p_data->write.conn_id,
                p_data->write.handle, p_data->write.offset);

        if (BTC_GATT_GET_GATT_IF(p_data->write.conn_id) != gattc_if) {
            LOG_ERR("[B]GattcWriteCharUnknownIf[%u]",
                    BTC_GATT_GET_GATT_IF(p_data->write.conn_id));
            break;
        }

        gattc_write_chrc_event_handler(&p_data->write);
        break;

    case BTA_GATTC_WRITE_DESCR_EVT:
        LOG_DBG("[B]GattcWriteDescrEvt[%u][0x%04x][%u]",
                p_data->write.status, p_data->write.conn_id, p_data->write.handle);
        break;

    case BTA_GATTC_PREP_WRITE_EVT:
        LOG_DBG("[B]GattcPrepWriteEvt[%u][0x%04x][%u][%u]",
                p_data->write.status, p_data->write.conn_id,
                p_data->write.handle, p_data->write.offset);
        break;

    case BTA_GATTC_EXEC_EVT:
        LOG_DBG("[B]GattcExecEvt[%u][0x%04x]",
                p_data->exec_cmpl.status, p_data->exec_cmpl.conn_id);
        break;

    case BTA_GATTC_NOTIF_EVT:
        LOG_INF("[B]GattcNotifEvt[0x%04x][%u][%u]",
                p_data->notify.conn_id, p_data->notify.handle, p_data->notify.is_notify);
        LOG_DBG("[B]GattcNotifBda[%02x:%02x:%02x:%02x:%02x:%02x]",
                p_data->notify.bda[0], p_data->notify.bda[1],
                p_data->notify.bda[2], p_data->notify.bda[3],
                p_data->notify.bda[4], p_data->notify.bda[5]);

        if (BTC_GATT_GET_GATT_IF(p_data->notify.conn_id) != gattc_if) {
            LOG_ERR("[B]GattcNotifUnknownIf[%u]",
                    BTC_GATT_GET_GATT_IF(p_data->notify.conn_id));
            break;
        }

        gattc_notify_rx_event_handler(&p_data->notify);
        break;

    case BTA_GATTC_SRVC_CHG_EVT:
        LOG_INF("[B]GattcSvcChgEvt[0x%04x]", p_data->srvc_chg.conn_id);
        LOG_DBG("[B]GattcSvcChgBda[%02x:%02x:%02x:%02x:%02x:%02x]",
                p_data->srvc_chg.remote_bda[0], p_data->srvc_chg.remote_bda[1],
                p_data->srvc_chg.remote_bda[2], p_data->srvc_chg.remote_bda[3],
                p_data->srvc_chg.remote_bda[4], p_data->srvc_chg.remote_bda[5]);
        break;

    case BTA_GATTC_CONGEST_EVT:
        LOG_DBG("[B]GattcCongestEvt[0x%04x][%u]",
                p_data->congest.conn_id, p_data->congest.congested);
        break;

    case BTA_GATTC_QUEUE_FULL_EVT:
        /* BTA p_cmd_list hit GATTC_COMMAND_QUEUE_SIZE_MAX (30); the rejected op
         * gets no cmpl_evt, so its gattc_list node would orphan until disconnect.
         * lib profiles are sequential and should never reach this; flag loudly
         * if it ever fires. */
        LOG_ERR("[B]GattcQueueFullEvt[%u][0x%04x][%u]",
                p_data->queue_full.status, p_data->queue_full.conn_id,
                p_data->queue_full.is_full);
        break;

    case BTA_GATTC_ASSOC_EVT:
        LOG_DBG("[B]GattcAssocEvt[%u][%u]",
                p_data->set_assoc.status, p_data->set_assoc.client_if);
        break;

    case BTA_GATTC_GET_ADDR_LIST_EVT:
        LOG_DBG("[B]GattcGetAddrListEvt[%u][%u][%u]",
                p_data->get_addr_list.status, p_data->get_addr_list.client_if,
                p_data->get_addr_list.num_addr);
        break;

    default:
        break;
    }
}

/* Bluedroid has no GAP-level ACL event; we synthesize one from BTA's GATTS/
 * GATTC connect/disconnect callbacks and post it through the iso GAP queue
 * — same path bt_le_nimble_gap_post_event() uses for BLE_GAP_EVENT_CONNECT/
 * DISCONNECT. The iso task then runs handle_acl_connect_event_safe() / the
 * disconnect equivalent inside common/app/gap.c. */
static void post_acl_connect_app_event(struct gatt_conn *gatt_conn)
{
    struct bt_le_gap_app_param *qev;
    int err;

    qev = calloc(1, sizeof(*qev));
    assert(qev);

    qev->type = BT_LE_GAP_APP_PARAM_ACL_CONNECT;

    qev->acl_connect.status = gatt_conn->status;

    /* Populate identity unconditionally: connect_event_handler fills these
     * fields before OPEN_EVT arrives, so failure events still carry valid
     * conn_handle / role / dst to the application — matches NimBLE's
     * BLE_GAP_EVENT_CONNECT, which always exposes the full descriptor. */
    qev->acl_connect.conn_handle = gatt_conn->conn_handle;
    qev->acl_connect.role = gatt_conn->role;
    qev->acl_connect.dst.type = gatt_conn->peer.type;
    memcpy(qev->acl_connect.dst.val, gatt_conn->peer.val, BT_ADDR_SIZE);

    err = bt_le_iso_task_post(ISO_QUEUE_ITEM_TYPE_GAP_EVENT, qev, sizeof(*qev));
    if (err) {
        LOG_ERR("[B]AclConnPostFail[%d]", err);
        free(qev);
    }
}

static void post_acl_disconnect_app_event(uint16_t conn_handle, uint8_t reason)
{
    struct bt_le_gap_app_param *qev;
    int err;

    qev = calloc(1, sizeof(*qev));
    assert(qev);

    qev->type = BT_LE_GAP_APP_PARAM_ACL_DISCONNECT;

    qev->acl_disconnect.conn_handle = conn_handle;
    qev->acl_disconnect.reason = reason;

    err = bt_le_iso_task_post(ISO_QUEUE_ITEM_TYPE_GAP_EVENT, qev, sizeof(*qev));
    if (err) {
        LOG_ERR("[B]AclDiscPostFail[%d]", err);
        free(qev);
    }
}

static void handle_gattc_connect_event(struct bt_le_gattc_connect_event *event)
{
    struct gatt_conn *gatt_conn;

    /* GATTS_CONNECT (the SLAVE path) owns peripheral-side gatt_conn; if BTA
     * ever dispatched a SLAVE-role event here we'd double-allocate. */
    if (event->role != BTM_ROLE_MASTER) {
        return;
    }

    gatt_conn = bt_le_bluedroid_find_gatt_conn_with_addr(event->peer.type, event->peer.val, false);
    if (gatt_conn) {
        LOG_ERR("[B]DevAlreadyExists");
        return;
    }

    /* App initiated via esp_ble_gattc_aux_open(engine_gattc_if, ...). BTA
     * dispatches CONNECT here first, OPEN follows. We allocate the slot now
     * so the OPEN handler finds it and posts ACL_CONNECT. */
    gatt_conn = bt_le_bluedroid_find_free_gatt_conn();
    if (gatt_conn == NULL) {
        LOG_ERR("[B]NoFreeConnInfo");
        return;
    }

    gatt_conn->used = 1;
    gatt_conn->conn_create = 0;
    gatt_conn->status = 0x00;
    gatt_conn->gatt_if = gattc_if;
    gatt_conn->conn_handle = event->conn_handle;
    gatt_conn->role = event->role;
    gatt_conn->peer.type = event->peer.type;
    memcpy(gatt_conn->peer.val, event->peer.val, BT_ADDR_SIZE);
}

static void handle_gattc_disconnect_event(struct bt_le_gattc_disconnect_event *event)
{
    struct gatt_conn *gatt_conn;
    struct gatts_list_node *n;
    struct bt_conn *conn;
    sys_snode_t *snode;

    gatt_conn = bt_le_bluedroid_find_gatt_conn_with_handle(event->conn_handle);
    if (gatt_conn == NULL) {
        LOG_DBG("[B]GattcDisconnUnknownDev");
        return;
    }

    if (gatt_conn->role == BTM_ROLE_MASTER) {
        /* CENTRAL may also run a GATT server (e.g. CAP Initiator with PACS).
         * Mirror handle_gatts_disconnect_event: fire func(err) for every
         * pending indication so lib state machines don't stall. */
        conn = bt_le_acl_conn_find(event->conn_handle);

        while ((snode = sys_slist_get(&gatt_conn->gatts_list)) != NULL) {
            n = CONTAINER_OF(snode, struct gatts_list_node, node);
            if (conn && n->params_copy.func) {
                n->params_copy.func(conn, &n->params_copy, BT_ATT_ERR_UNLIKELY);
            }
            gatts_list_node_free(n);
        }

        post_acl_disconnect_app_event(gatt_conn->conn_handle, event->reason);

        /* Reset the corresponding gatt_conn */
        reset_gatt_conn(gatt_conn);
    }
}

static void handle_gattc_open_event(struct bt_le_gattc_open_event *event)
{
    struct gatt_conn *gatt_conn;
    uint16_t conn_id;

    gatt_conn = bt_le_bluedroid_find_gatt_conn_with_handle(event->conn_handle);
    if (gatt_conn == NULL) {
        LOG_ERR("[B]GattcOpenUnknownDev");
        return;
    }

    gatt_conn->status = event->status;

    if (gatt_conn->role == BTM_ROLE_MASTER) {
        post_acl_connect_app_event(gatt_conn);

        if (gatt_conn->status) {
            /* If failed to create connection, reset the corresponding gatt_conn */
            reset_gatt_conn(gatt_conn);
            return;
        }
    } else {
        if (gatt_conn->gattc_open && gatt_conn->status != BTA_GATT_OK) {
            /* Failed to open gatt client, post an event to notify the app layer */
            bt_le_gattc_app_open_event(event, gattc_if);
            return;
        }
    }

    /* At this moment, the peer device may has not initiated MTU exchange */
    if (gatt_conn->mtu == 0) {
        conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, gatt_conn->conn_handle);

        BTA_GATTC_ConfigureMTU(conn_id);

        /* Mark MTU exchange in progress. The MTU change event will be
         * posted later when BTA_GATTC_CFG_MTU_EVT arrives (the open event
         * below still goes out unconditionally). */
        gatt_conn->cfg_mtu = 1;
    }

    /* Post an event to the app layer, we need this event to discover
     * services and characteristics, etc.
     */
    bt_le_gattc_app_open_event(event, gattc_if);
}

static void handle_gattc_mtu_event(struct bt_le_gattc_mtu_event *event)
{
    struct gatt_conn *gatt_conn;

    gatt_conn = bt_le_bluedroid_find_gatt_conn_with_handle(event->conn_handle);
    if (gatt_conn == NULL) {
        LOG_ERR("[B]GattcMtuUnknownConn[%u]", event->conn_handle);
        return;
    }

    /* The device may works as GATT client or server or both, which is not
     * related to the Link Layer role, hence we don't check the Link layer
     * role here and only check if the MTU has already been exchanged here.
     */
    if (gatt_conn->mtu != 0) {
        LOG_INF("[B]GattcMtuExchanged[%u][%u][%u]",
                gatt_conn->conn_handle, gatt_conn->mtu, event->mtu);

        gatt_conn->mtu = MIN(event->mtu, gatt_conn->mtu);
    } else {
        gatt_conn->mtu = event->mtu;
    }

    if (gatt_conn->cfg_mtu && gatt_conn->mtu_posted == 0) {
        /* Post an event to the app layer, we need this event to discover
         * services and characteristics, etc. Report the clamped value so
         * the app stays consistent with gatt_conn state (mirrors NimBLE's
         * BLE_GAP_EVENT_MTU which already carries the negotiated value). */
        bt_le_gatt_app_mtu_change_event(event->conn_handle, gatt_conn->mtu);

        gatt_conn->mtu_posted = 1;  /* Mark MTU event as posted */
    }
}

static void handle_gattc_disc_cmpl_event(struct bt_le_gattc_disc_cmpl_event *event)
{
    struct gatt_conn *gatt_conn;

    gatt_conn = bt_le_bluedroid_find_gatt_conn_with_handle(event->conn_handle);
    if (gatt_conn == NULL) {
        LOG_ERR("[B]GattcDiscSvcUnknownConn[%u]", event->conn_handle);
        return;
    }

    bt_le_gattc_app_disc_cmpl_event(event);
}

static void handle_gattc_read_chrc_event(struct bt_le_gattc_read_chrc_event *event)
{
    struct bt_gatt_read_params *params;
    struct gatt_conn *gatt_conn;
    struct gattc_list_node *op;
    struct bt_conn *conn;
    const uint8_t *val;
    sys_snode_t *snode;
    uint16_t vlen;
    uint16_t off;
    uint8_t ret;

    conn = bt_le_acl_conn_find(event->conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_WRN("[B]GattcRdCharNotConn");
        goto end;
    }

    gatt_conn = bt_le_bluedroid_find_gatt_conn_with_handle(event->conn_handle);
    if (gatt_conn == NULL) {
        LOG_WRN("[B]GattcRdCharUnknownConn[%u]", event->conn_handle);
        goto end;
    }

    snode = sys_slist_get(&gatt_conn->gattc_list);
    if (snode == NULL) {
        LOG_ERR("[B]GattcRdCharNoOp[%u]", event->conn_handle);
        goto end;
    }

    op = CONTAINER_OF(snode, struct gattc_list_node, node);
    if (op->type != GATTC_OP_READ) {
        LOG_ERR("[B]GattcOpHeadMismatch[%u]exp_rd_got[%u]",
                event->conn_handle, op->type);
        /* BTA delivers cmpl_evt in p_cmd_list FIFO order; type mismatch
         * means this EVT is spurious — put the head back (still a legitimate
         * in-flight op) and drop the spurious EVT. */
        sys_slist_prepend(&gatt_conn->gattc_list, &op->node);
        goto end;
    }

    params = op->read_params;
    free(op);

    if (params == NULL || params->func == NULL) {
        LOG_ERR("[B]GattcRdCharNoFunc");
        goto end;
    }

    if ((params->handle_count == 0 && (event->attr_handle < params->by_uuid.start_handle ||
                                       event->attr_handle > params->by_uuid.end_handle)) ||
            (params->handle_count == 1 && event->attr_handle != params->single.handle)) {
        LOG_ERR("[B]GattcRdCharInvRsp[%u][%u][%u][%u][%u]",
                params->handle_count, event->attr_handle, params->by_uuid.start_handle,
                params->by_uuid.end_handle, params->single.handle);
        /* BTA fires READ_CHAR_EVT exactly once per read; the op was just
         * popped above so no further EVT will arrive. Fire func with err
         * so caller's state machine doesn't stall waiting forever. */
        params->func(conn, BT_ATT_ERR_UNLIKELY, params, NULL, 0);
        goto end;
    }

    val = event->value;
    vlen = event->len;

    /* Long read: BTA's auto Read-Blob already fetched the full value on the
     * BTU task; hand the caller only the requested [offset:] tail. */
    if (params->handle_count == 1 && params->single.offset != 0 && event->status == 0) {
        off = params->single.offset;
        val = (off < event->len) ? event->value + off : NULL;
        vlen = (off < event->len) ? (event->len - off) : 0;
    }

    ret = params->func(conn, event->status, params, val, vlen);

    /* (0, NULL, 0) is the success-completion signal per bt_gatt_read_func_t.
     * Skip it on error paths — the error call above is already terminal;
     * sending a follow-up "success" would let lib treat the read as OK and
     * keep stale data. Mirrors NimBLE nrp.c, which fires func(err, NULL, 0)
     * once on the default branch and never the success-completion. */
    if (ret == BT_GATT_ITER_CONTINUE && event->status == 0) {
        params->func(conn, 0, params, NULL, 0);
    }

end:
    if (event->value) {
        free(event->value);
    }
}

static void handle_gattc_write_chrc_event(struct bt_le_gattc_write_chrc_event *event)
{
    struct bt_gatt_write_params *params;
    struct gatt_conn *gatt_conn;
    struct gattc_list_node *op;
    struct bt_conn *conn;
    sys_snode_t *snode;

    conn = bt_le_acl_conn_find(event->conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_WRN("[B]GattcWrCharNotConn");
        return;
    }

    gatt_conn = bt_le_bluedroid_find_gatt_conn_with_handle(event->conn_handle);
    if (gatt_conn == NULL) {
        LOG_WRN("[B]GattcWrCharUnknownConn[%u]", event->conn_handle);
        return;
    }

    snode = sys_slist_get(&gatt_conn->gattc_list);
    if (snode == NULL) {
        /* write_without_rsp does not enqueue (no cmpl_evt expected); if BTA
         * still fires one (e.g. internal write-cmd path uses cmpl_evt), there
         * is no head to consume. */
        LOG_DBG("[B]GattcWrCharNoOp[%u]", event->conn_handle);
        return;
    }

    op = CONTAINER_OF(snode, struct gattc_list_node, node);
    if (op->type != GATTC_OP_WRITE) {
        LOG_ERR("[B]GattcOpHeadMismatch[%u]exp_wr_got[%u]",
                event->conn_handle, op->type);
        /* Put it back at head — this EVT doesn't belong to us, leave the
         * read op for handle_gattc_read_chrc_event to consume. */
        sys_slist_prepend(&gatt_conn->gattc_list, &op->node);
        return;
    }

    params = op->write_params;
    free(op);

    assert(params && params->func);

    if (event->attr_handle != params->handle) {
        LOG_ERR("[B]GattcWrCharInvRsp[%u][%u]", event->attr_handle, params->handle);
        /* Op was already popped and freed; fire func with err so caller's
         * state machine doesn't stall waiting for a completion that won't
         * come. Mirrors handle_gattc_read_chrc_event's mismatch path. */
        params->func(conn, BT_ATT_ERR_UNLIKELY, params);
        return;
    }

    params->func(conn, event->status, params);
}

static void handle_gattc_notify_event(struct bt_le_gattc_notify_rx_event *event)
{
    struct bt_gatt_subscribe_params *params;
    struct bt_gatt_subscribe_params *tmp;
    struct gatt_conn *gatt_conn;
    struct gattc_sub *sub;
    struct bt_conn *conn;

    conn = bt_le_acl_conn_find(event->conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_WRN("[B]GattcNotifNotConn");
        goto end;
    }

    gatt_conn = bt_le_bluedroid_find_gatt_conn_with_handle(event->conn_handle);
    if (gatt_conn == NULL) {
        LOG_WRN("[B]GattcNotifUnknownConn[%u]", event->conn_handle);
        goto end;
    }

    sub = bt_gattc_sub_find(conn);
    if (sub == NULL) {
        LOG_WRN("[B]GattcNotifNoSub[%u]", event->conn_handle);
        goto end;
    }

    /* Caller bt_le_bluedroid_gatt_handle_event already holds host_lock
     * across the whole switch — sub->list iteration runs under that.
     * Zephyr bt_gatt_subscribe_params API permits the notify cb to
     * unsubscribe inline, which NULLs the current node's next pointer
     * and breaks the unsafe walker; use _SAFE to pre-cache next. */
    SYS_SLIST_FOR_EACH_CONTAINER_SAFE(&sub->list, params, tmp, node) {
        if (params->ccc_handle != BT_GATT_AUTO_DISCOVER_CCC_HANDLE &&
                params->value_handle == event->attr_handle) {
            if (params->notify) {
                /* Return value intentionally ignored. Zephyr unsubscribes on
                 * BT_GATT_ITER_STOP, but lib clients return STOP on a single
                 * malformed/short notify (e.g. unicast_client_cp_notify);
                 * tearing down a core subscription like the ASCS control point
                 * over one bad PDU would drop every later notification. Tolerate
                 * the bad PDU and keep the subscription. */
                params->notify(conn, params, event->value, event->len);
            }
        }
    }

end:
    if (event->value) {
        free(event->value);
    }
}

static void handle_gatts_connect_event(struct bt_le_gatts_connect_event *event)
{
    struct gatt_conn *gatt_conn;

    /* GATTS_CONNECT is broadcast to every registered GATTS app; central side
     * (MASTER role) also receives it. Central-side gatt_conn creation is
     * owned by handle_gattc_connect_event so we only handle SLAVE here. */
    if (event->role != BTM_ROLE_SLAVE) {
        return;
    }

    gatt_conn = bt_le_bluedroid_find_gatt_conn_with_addr(event->peer.type, event->peer.val, false);
    if (gatt_conn) {
        LOG_ERR("[B]DevAlreadyExists");
        return;
    }

    gatt_conn = bt_le_bluedroid_find_free_gatt_conn();
    if (gatt_conn == NULL) {
        LOG_ERR("[B]NoFreeConnInfo");
        return;
    }

    gatt_conn->used = 1;
    gatt_conn->conn_create = 0;
    gatt_conn->status = 0x00;
    gatt_conn->gatt_if = gatts_if;
    gatt_conn->conn_handle = event->conn_handle;
    gatt_conn->role = event->role;
    gatt_conn->peer.type = event->peer.type;
    memcpy(gatt_conn->peer.val, event->peer.val, BT_ADDR_SIZE);

    post_acl_connect_app_event(gatt_conn);
}

static void handle_gatts_disconnect_event(struct bt_le_gatts_disconnect_event *event)
{
    struct gatts_list_node *n;
    struct gatt_conn *gatt_conn;
    struct bt_conn *conn;
    sys_snode_t *snode;

    gatt_conn = bt_le_bluedroid_find_gatt_conn_with_handle(event->conn_handle);
    if (gatt_conn == NULL) {
        LOG_WRN("[B]GattsDisconnUnknownDev");
        return;
    }

    /* Mirror handle_gatts_connect_event: SLAVE-side teardown happens here;
     * MASTER teardown is in handle_gattc_disconnect_event. */
    if (gatt_conn->role != BTM_ROLE_SLAVE) {
        return;
    }

    /* conn may already have been torn down by the ACL layer before this event
     * arrives — best-effort lookup, NULL is tolerated and the drain still runs
     * (params->func is just skipped for nodes we can't address). */
    conn = bt_le_acl_conn_find(event->conn_handle);

    /* Fire func with err for every pending indication. Mirrors NimBLE stack's
     * ble_gatts_indicate_fail_notconn on disconn — BTA never delivers CONF_EVT
     * for in-flight indications when the conn drops, so the adapter has to
     * simulate that fail path to keep lib state machines from stalling. */
    while ((snode = sys_slist_get(&gatt_conn->gatts_list)) != NULL) {
        n = CONTAINER_OF(snode, struct gatts_list_node, node);
        if (conn && n->params_copy.func) {
            n->params_copy.func(conn, &n->params_copy, BT_ATT_ERR_UNLIKELY);
        }
        gatts_list_node_free(n);
    }

    post_acl_disconnect_app_event(gatt_conn->conn_handle, event->reason);

    /* Reset the corresponding gatt_conn */
    reset_gatt_conn(gatt_conn);
}

static void handle_gatts_mtu_event(struct bt_le_gatts_mtu_event *event)
{
    struct gatt_conn *gatt_conn;

    gatt_conn = bt_le_bluedroid_find_gatt_conn_with_handle(event->conn_handle);
    if (gatt_conn == NULL) {
        LOG_ERR("[B]GattsMtuUnknownConn[%u]", event->conn_handle);
        return;
    }

    /* The device may works as GATT client or server or both, which is not
     * related to the Link Layer role, hence we don't check the Link layer
     * role here and only check if the MTU has already been exchanged here.
     */
    if (gatt_conn->mtu != 0) {
        LOG_INF("[B]GattsMtuExchanged[%u][%u][%u]",
                gatt_conn->conn_handle, gatt_conn->mtu, event->mtu);

        gatt_conn->mtu = MIN(event->mtu, gatt_conn->mtu);
    } else {
        gatt_conn->mtu = event->mtu;
    }

    if (gatt_conn->mtu_posted == 0) {
        /* Report clamped value — see handle_gattc_mtu_event. */
        bt_le_gatt_app_mtu_change_event(event->conn_handle, gatt_conn->mtu);

        gatt_conn->mtu_posted = 1;  /* Mark MTU event as posted */
    }
}

static void handle_gatts_read_event(struct bt_le_gatts_read_event *event)
{
    struct bt_gatt_attr *attr;
    tBTA_GATT_STATUS status;
    struct bt_conn *conn;
    tBTA_GATTS_RSP *rsp;
    uint16_t conn_id;
    ssize_t ret;

    status = BTA_GATT_OK;
    rsp = NULL;

    conn = bt_le_acl_conn_find(event->conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_WRN("[B]GattsRdEvtNotConn");
        return;
    }

    attr = bt_gatts_find_attr_by_handle(event->attr_handle);
    if (attr == NULL) {
        LOG_ERR("[B]GattsRdAttrNotFound[%u]", event->attr_handle);
        status = GATT_INVALID_HANDLE;
        goto end;
    }

    /* The tBTA_GATT_STATUS structure is too large, hence use
     * dynamic memory here to avoid stack overflow.
     */
    rsp = calloc(1, sizeof(*(rsp)));
    assert(rsp);

    rsp->attr_value.handle = event->attr_handle;

    if (attr->read) {
        if (event->offset) {
            LOG_WRN("[B]GattsRdEvtNotSupp");

            status = GATT_REQ_NOT_SUPPORTED;
        } else {
            ret = attr->read(conn, attr, (void *)rsp, GATT_MAX_ATTR_LEN, 0);
            if (ret < 0) {
                LOG_ERR("[B]GattsRdEvtFail[%u][%d]", event->attr_handle, ret);

                status = BT_GATT_ERR(ret);
            }
        }
    } else {
        status = GATT_READ_NOT_PERMIT;
    }

end:
    conn_id = BTC_GATT_CREATE_CONN_ID(gatts_if, conn->handle);

    BTA_GATTS_SendRsp(conn_id, event->trans_id, status, rsp);

    if (rsp) {
        free(rsp);
    }
}

static void handle_gatts_write_event(struct bt_le_gatts_write_event *event)
{
    struct bt_le_gatts_subscribe_event sub_event;
    struct bt_gatt_attr *attr;
    tBTA_GATT_STATUS status;
    struct bt_conn *conn;
    uint16_t conn_id;
    ssize_t ret;

    status = BTA_GATT_OK;

    conn = bt_le_acl_conn_find(event->conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_WRN("[B]GattsWrEvtNotConn");
        if (event->value) {
            free(event->value);
        }
        return;
    }

    attr = bt_gatts_find_attr_by_handle(event->attr_handle);
    if (attr == NULL) {
        LOG_ERR("[B]GattsWrAttrNotFound[%u]", event->attr_handle);
        status = GATT_INVALID_HANDLE;
        goto end;
    }

    /* Check if the attribute UUID is CCCD */
    if (bt_uuid_cmp(attr->uuid, BT_UUID_GATT_CCC) == 0) {
        if (event->len != 2) {
            LOG_ERR("[B]GattsWrInvPdu[%u]", event->len);
            status = GATT_INVALID_PDU;
            goto end;
        }

        uint16_t value = sys_get_le16(event->value);

        /* The CCC cfg pool is BT_GATT_CCC_MAX deep; a full pool returns
         * nonzero here. Don't ACK OK in that case, else the client believes
         * notify is enabled while the server tracks nothing. */
        if (bt_gatts_sub_changed(conn->handle, event->attr_handle,
                                 value & 0x01, (value >> 1) & 0x01, 0x00) != 0) {
            LOG_ERR("[B]GattsSubChangedFail[%u]", event->attr_handle);
            status = GATT_INSUF_RESOURCE;
            goto end;
        }

        /* Mirror NimBLE adapter's subscribe path so the lib's subscribe cb
         * fires on Bluedroid too. attr_handle follows NimBLE convention
         * (chrc value handle = CCCD handle - 1). Bluedroid stack does not
         * track previous CCCD value, so prev_* are reported as 0.
         */
        sub_event.conn_handle   = conn->handle;
        sub_event.attr_handle   = event->attr_handle - 1;
        sub_event.prev_notify   = 0;
        sub_event.cur_notify    = value & 0x01;
        sub_event.prev_indicate = 0;
        sub_event.cur_indicate  = (value >> 1) & 0x01;
        sub_event.reason        = 0;
        bt_le_gatts_app_subscribe_event(&sub_event);

        goto end;
    }

    if (attr->write) {
        if (event->offset) {
            LOG_WRN("[B]GattsWrEvtNotSupp");

            status = GATT_REQ_NOT_SUPPORTED;
        } else {
            ret = attr->write(conn, attr, event->value, event->len, 0, 0);
            if (ret < 0) {
                LOG_ERR("[B]GattsWrEvtFail[%u][%d]", event->attr_handle, ret);

                status = BT_GATT_ERR(ret);
            }
        }
    } else {
        status = GATT_WRITE_NOT_PERMIT;
    }

end:
    if (event->need_rsp) {
        conn_id = BTC_GATT_CREATE_CONN_ID(gatts_if, conn->handle);

        BTA_GATTS_SendRsp(conn_id, event->trans_id, status, NULL);
    }

    if (event->value) {
        free(event->value);
    }
}

static void handle_gatts_notify_tx_event(struct bt_le_gatts_notify_tx_event *event)
{
    struct gatts_list_node *n;
    struct gatt_conn *gatt_conn;
    struct bt_conn *conn;
    sys_snode_t *snode;

    /* Find conn once up front. If the acl_conn was already torn down (race
     * with a queued CLOSE_EVT behind this CONF_EVT), leave the head in the
     * list — disconnect drain will fire its func with BT_ATT_ERR_UNLIKELY. */
    conn = bt_le_acl_conn_find(event->conn_handle);
    if (conn == NULL) {
        LOG_WRN("[B]NotifyTxNoConn[%u]", event->conn_handle);
        return;
    }

    gatt_conn = bt_le_bluedroid_find_gatt_conn_with_handle(event->conn_handle);
    if (gatt_conn == NULL) {
        LOG_WRN("[B]NotifyTxUnknownConn[%u]", event->conn_handle);
        return;
    }

    /* Notify CONF_EVT first: BTA fires CONF_EVT immediately after send for
     * notifications; pop the matching marker so we don't mistake it for an
     * indication ack and corrupt the indication queue. */
    snode = sys_slist_get(&gatt_conn->gatts_notify_list);
    if (snode != NULL) {
        struct gatts_notify_node *nn = CONTAINER_OF(snode, struct gatts_notify_node, node);

        if (nn->value_handle != event->attr_handle) {
            LOG_DBG("[B]NotifyTxHdlSkew[%u][%u]", event->attr_handle, nn->value_handle);
        }
        free(nn);
        return;
    }

    /* No notify pending — this CONF_EVT is an indication ack. */
    snode = sys_slist_get(&gatt_conn->gatts_list);
    if (snode == NULL) {
        LOG_WRN("[B]NotifyTxNoPending[%u]", event->conn_handle);
        return;
    }

    n = CONTAINER_OF(snode, struct gatts_list_node, node);

    /* Submit next pending BEFORE firing the completion cb. host_mutex is
     * recursive, so a cb that re-enters bt_gatt_indicate isn't blocked;
     * if dispatch_next ran AFTER the cb, gatts_indicate_enqueue would
     * see an empty list (head just popped) and dispatch the new node
     * inline — then this post-cb dispatch would fire it again, sending
     * the same indication twice on the wire. Doing the peek-and-dispatch
     * first leaves the head in place so re-entrant enqueue sees a
     * non-empty list and skips its inline dispatch. Keep the node in the
     * list — its own CONF_EVT will pop it later. */
    {
        sys_snode_t *next_snode = sys_slist_peek_head(&gatt_conn->gatts_list);
        if (next_snode != NULL) {
            struct gatts_list_node *next_n =
                CONTAINER_OF(next_snode, struct gatts_list_node, node);
            gatts_indicate_dispatch(conn, next_n);
        }
    }

    if (n->value_handle != event->attr_handle) {
        LOG_ERR("[B]NotifyTxHdlMismatch[%u][%u]", event->attr_handle, n->value_handle);
        /* Mismatch is fatal here: BTA delivers CONF_EVT in FIFO order, so a
         * head-vs-event handle disagreement means our list is out of sync.
         * Drop the head to keep advancing (alternative is freezing the queue).
         * Notify lib with err so its state machine doesn't stall waiting
         * for a callback that will never match. */
        if (n->params_copy.func) {
            n->params_copy.func(conn, &n->params_copy, BT_ATT_ERR_UNLIKELY);
        }
        gatts_list_node_free(n);
        return;
    }

    if (n->params_copy.func) {
        n->params_copy.func(conn, &n->params_copy, event->status);
    }

    gatts_list_node_free(n);
}

void bt_le_bluedroid_gatt_handle_event(uint8_t *data, size_t data_len)
{
    struct bt_le_gatt_event_param *param;

    param = (struct bt_le_gatt_event_param *)data;

    bt_le_host_lock();

    switch (param->type) {
    case BT_LE_GATTC_CONNECT_EVENT:
        handle_gattc_connect_event(&param->gattc_connect);
        break;

    case BT_LE_GATTC_DISCONNECT_EVENT:
        handle_gattc_disconnect_event(&param->gattc_disconnect);
        break;

    case BT_LE_GATTC_OPEN_EVENT:
        handle_gattc_open_event(&param->gattc_open);
        break;

    case BT_LE_GATTC_MTU_EVENT:
        handle_gattc_mtu_event(&param->gattc_mtu);
        break;

    case BT_LE_GATTC_DISC_CMPL_EVENT:
        handle_gattc_disc_cmpl_event(&param->gattc_disc_cmpl);
        break;

    case BT_LE_GATTC_READ_CHRC_EVENT:
        handle_gattc_read_chrc_event(&param->gattc_read_chrc);
        break;

    case BT_LE_GATTC_WRITE_CHRC_EVENT:
        handle_gattc_write_chrc_event(&param->gattc_write_chrc);
        break;

    case BT_LE_GATTC_NOTIFY_RX_EVENT:
        handle_gattc_notify_event(&param->gattc_notify_rx);
        break;

    case BT_LE_GATTS_CONNECT_EVENT:
        handle_gatts_connect_event(&param->gatts_connect);
        break;

    case BT_LE_GATTS_DISCONNECT_EVENT:
        handle_gatts_disconnect_event(&param->gatts_disconnect);
        break;

    case BT_LE_GATTS_MTU_EVENT:
        handle_gatts_mtu_event(&param->gatts_mtu);
        break;

    case BT_LE_GATTS_READ_EVENT:
        handle_gatts_read_event(&param->gatts_read);
        break;

    case BT_LE_GATTS_WRITE_EVENT:
        handle_gatts_write_event(&param->gatts_write);
        break;

    case BT_LE_GATTS_NOTIFY_TX_EVENT:
        handle_gatts_notify_tx_event(&param->gatts_notify_tx);
        break;

    default:
        assert(0);
        break;
    }

    bt_le_host_unlock();

    free(data);
}

void bt_le_bluedroid_gatts_svc_cb_register(struct gatts_svc_cb *cb)
{
    bt_le_host_lock();
    gatts_svc_cb = cb;
    bt_le_host_unlock();
}

void bt_le_bluedroid_gatt_uuid_convert(const struct bt_uuid *uuid_in, void *uuid_out)
{
    assert(uuid_out && uuid_in);

    if (uuid_in->type == BT_UUID_TYPE_16) {
        ((tBT_UUID *)uuid_out)->len = LEN_UUID_16;
        ((tBT_UUID *)uuid_out)->uu.uuid16 = BT_UUID_16(uuid_in)->val;
    } else if (uuid_in->type == BT_UUID_TYPE_32) {
        ((tBT_UUID *)uuid_out)->len = LEN_UUID_32;
        ((tBT_UUID *)uuid_out)->uu.uuid32 = BT_UUID_32(uuid_in)->val;
    } else if (uuid_in->type == BT_UUID_TYPE_128) {
        ((tBT_UUID *)uuid_out)->len = LEN_UUID_128;
        memcpy(((tBT_UUID *)uuid_out)->uu.uuid128, BT_UUID_128(uuid_in)->val, LEN_UUID_128);
    } else {
        assert(0);
    }
}

uint16_t bt_le_bluedroid_gatt_perm_convert(uint16_t perm_in)
{
    tBTA_GATT_PERM perm_out = 0;

    if ((perm_in & BT_GATT_PERM_READ) == BT_GATT_PERM_READ) {
        perm_out |= BTA_GATT_PERM_READ;
    }

    if ((perm_in & BT_GATT_PERM_WRITE) == BT_GATT_PERM_WRITE) {
        perm_out |= BTA_GATT_PERM_WRITE;
    }

    if ((perm_in & BT_GATT_PERM_READ_ENCRYPT) == BT_GATT_PERM_READ_ENCRYPT) {
        perm_out |= BTA_GATT_PERM_READ_ENCRYPTED;
    }

    if ((perm_in & BT_GATT_PERM_WRITE_ENCRYPT) == BT_GATT_PERM_WRITE_ENCRYPT) {
        perm_out |= BTA_GATT_PERM_WRITE_ENCRYPTED;
    }

    if ((perm_in & BT_GATT_PERM_READ_AUTHEN) == BT_GATT_PERM_READ_AUTHEN) {
        perm_out |= BTA_GATT_PERM_READ_ENC_MITM;
    }

    if ((perm_in & BT_GATT_PERM_WRITE_AUTHEN) == BT_GATT_PERM_WRITE_AUTHEN) {
        perm_out |= BTA_GATT_PERM_WRITE_ENC_MITM;
    }

    return perm_out;
}

uint16_t bt_le_bluedroid_gatt_get_mtu(struct bt_conn *conn)
{
    struct gatt_conn *gatt_conn;

    gatt_conn = bt_le_bluedroid_find_gatt_conn_with_handle(conn->handle);
    if (gatt_conn && gatt_conn->mtu) {
        return gatt_conn->mtu;
    }

    /* Not yet negotiated — return local advertised MTU as best-effort. */
    return BTA_GATT_GetLocalMTU();
}

ssize_t bt_le_bluedroid_gatts_attr_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                        void *buf, uint16_t buf_len, uint16_t offset,
                                        const void *value, uint16_t value_len)
{
    ssize_t len;

    if (value == NULL || value_len == 0) {
        len = 0;
    } else if (offset >= value_len) {
        /* Without this guard `value_len - offset` would underflow uint16 and
         * MIN would feed memcpy an out-of-bounds length.
         */
        LOG_WRN("[B]OffsetTooLarge[%u][%u]", offset, value_len);
        len = 0;
    } else {
        len = MIN(buf_len, value_len - offset);

        LOG_DBG("[B]AttrRd[%u][%u][%u]", attr->handle, offset, len);

        /* Dispatch by caller: a real peer GATT read comes via handle_gatts_read_event
         * with conn != NULL and buf = tBTA_GATTS_RSP*; internal raw-byte reads
         * (bt_gatt_is_subscribed, ccid.c, etc.) come with conn == NULL and buf = a
         * plain byte buffer. Without this branch the raw-byte case would clobber
         * stack memory by writing through rsp->attr_value.value past the caller's
         * 1-byte storage. */
        if (conn != NULL) {
            tBTA_GATTS_RSP *rsp = buf;

            memcpy(rsp->attr_value.value, (uint8_t *)value + offset, len);
            rsp->attr_value.len = len;
        } else {
            memcpy(buf, (uint8_t *)value + offset, len);
        }
    }

    return len;
}

/* Append a notify marker so handle_gatts_notify_tx_event can pop it off the
 * notify list ahead of any pending indication head. Returns -ENOTCONN when
 * the gatt_conn is already torn down (caller should skip the BTA send to
 * avoid a "Unknown connection ID" log from BTA), -ENOMEM on alloc fail. */
static int gatts_notify_enqueue(struct bt_conn *conn, uint16_t value_handle)
{
    struct gatts_notify_node *n;
    struct gatt_conn *gatt_conn;

    /* Common during ACL teardown — lib drains its own send queue racing with
     * the disconnect event walking up. Not actionable, keep at DBG. */
    gatt_conn = bt_le_bluedroid_find_gatt_conn_with_handle(conn->handle);
    if (gatt_conn == NULL) {
        LOG_INF("[B]GattsNotifyAfterDisconn[%u]", conn->handle);
        return -ENOTCONN;
    }

    n = calloc(1, sizeof(*n));
    if (n == NULL) {
        LOG_ERR("[B]GattsNotifyNodeAllocFail[%u]", conn->handle);
        return -ENOMEM;
    }

    n->value_handle = value_handle;
    sys_slist_append(&gatt_conn->gatts_notify_list, &n->node);

    return 0;
}

static int gatts_notify(struct bt_conn *conn,
                        const struct bt_gatt_attr **attr,
                        const struct bt_uuid *uuid,
                        const void *value, uint16_t len,
                        bool need_cfm)
{
    struct notify_data data;
    struct bt_conn *conns;
    uint8_t conns_count;
    uint16_t conn_id;

    /* BTA_GATTS_HandleValueIndication is void and silently drops payloads
     * larger than BTA_GATT_MAX_ATTR_LEN. Without this guard, the notify
     * marker / indicate node would be enqueued but no CONF_EVT would ever
     * arrive, permanently stalling the queue. Catch oversize here so the
     * marker is never pushed in that case. (osi_malloc OOM inside BTA is
     * still possible but not catchable from this layer.) */
    if (len > BTA_GATT_MAX_ATTR_LEN) {
        LOG_ERR("[B]GattsValTooBig[%u]", len);
        return -EMSGSIZE;
    }

    memset(&data, 0, sizeof(data));

    data.attr = *attr;
    data.handle = bt_gatt_attr_get_handle(data.attr);

    /* Lookup UUID if it was given */
    if (uuid) {
        if (bt_gatts_find_attr_by_uuid(&data, uuid) == false) {
            return -ENOENT;
        }

        *attr = data.attr;
    } else {
        if (data.handle == 0) {
            return -ENOENT;
        }
    }

    /* Check if attribute is a characteristic then adjust the handle */
    if (bt_uuid_cmp(data.attr->uuid, BT_UUID_GATT_CHRC) == 0) {
        struct bt_gatt_chrc *chrc;
        uint16_t required;

        assert(data.attr->user_data);
        chrc = data.attr->user_data;

        required = need_cfm ? BT_GATT_CHRC_INDICATE : BT_GATT_CHRC_NOTIFY;
        if ((chrc->properties & required) == 0) {
            return -EINVAL;
        }

        data.handle = bt_gatt_attr_value_handle(data.attr);
    }

    if (conn) {
        /* For notify, enqueue marker first; any non-zero return means the
         * marker is not on the list (gatt_conn gone or alloc failure) so
         * skip the BTA send too — otherwise BTA's CONF_EVT pop would have
         * no matching marker and corrupt the indication queue head. */
        if (!need_cfm) {
            int rc = gatts_notify_enqueue(conn, data.handle);
            if (rc) {
                return rc;
            }
        }

        conn_id = BTC_GATT_CREATE_CONN_ID(gatts_if, conn->handle);
        BTA_GATTS_HandleValueIndication(conn_id, data.handle, len,
                                        (uint8_t *)value, need_cfm);
    } else {
        bt_conn_get_acl_conns(&conns, &conns_count);

        for (size_t i = 0; i < conns_count; i++) {
            if (conns[i].state != BT_CONN_CONNECTED) {
                continue;
            }

            if (!need_cfm && gatts_notify_enqueue(&conns[i], data.handle) != 0) {
                continue;
            }

            conn_id = BTC_GATT_CREATE_CONN_ID(gatts_if, conns[i].handle);
            BTA_GATTS_HandleValueIndication(conn_id, data.handle, len,
                                            (uint8_t *)value, need_cfm);
        }
    }

    return 0;
}

int bt_le_bluedroid_gatts_notify(struct bt_conn *conn, struct bt_gatt_notify_params *params)
{
    return gatts_notify(conn, &(params->attr), params->uuid, params->data, params->len, false);
}

/* Enqueue one node onto a conn's gatts_list; dispatch if it became head.
 * BTA spec: per-conn single outstanding indication — queueing here lets the
 * caller pipeline; we serialize dispatch one-at-a-time via CONF_EVT path. */
static int gatts_indicate_enqueue(struct bt_conn *conn,
                                  struct bt_gatt_indicate_params *params,
                                  const struct bt_gatt_attr *attr,
                                  uint16_t value_handle)
{
    struct gatts_list_node *n;
    struct gatt_conn *gatt_conn;
    bool was_empty;

    gatt_conn = bt_le_bluedroid_find_gatt_conn_with_handle(conn->handle);
    if (gatt_conn == NULL) {
        LOG_ERR("[B]GattsIndNoConnInfo[%u]", conn->handle);
        return -ENODEV;
    }

    n = gatts_list_node_alloc(params, attr, value_handle);
    if (n == NULL) {
        LOG_ERR("[B]GattsIndNodeAllocFail[%u]", conn->handle);
        return -ENOMEM;
    }

    was_empty = sys_slist_is_empty(&gatt_conn->gatts_list);
    sys_slist_append(&gatt_conn->gatts_list, &n->node);

    if (was_empty) {
        gatts_indicate_dispatch(conn, n);
    }

    return 0;
}

int bt_le_bluedroid_gatts_indicate(struct bt_conn *conn, struct bt_gatt_indicate_params *params)
{
    const struct bt_gatt_attr *attr;
    struct bt_conn *conns;
    uint16_t value_handle;
    uint8_t conns_count;
    int rc;

    /* Same BTA silent-drop hazard as gatts_notify: an oversize payload is
     * dropped with no CONF_EVT, permanently stalling the per-conn queue. */
    if (params->len > BTA_GATT_MAX_ATTR_LEN) {
        LOG_ERR("[B]GattsIndValTooBig[%u]", params->len);
        return -EMSGSIZE;
    }

    rc = gatts_indicate_resolve(params->attr, params->uuid, &attr, &value_handle);
    if (rc) {
        LOG_ERR("[B]GattsIndResolveFail[%d]", rc);
        return rc;
    }

    /* lib reads params->attr after this call (e.g. to obtain the matched
     * attr for uuid lookups). Mirror the legacy gatts_notify behavior. */
    params->attr = attr;

    if (conn) {
        return gatts_indicate_enqueue(conn, params, attr, value_handle);
    }

    /* Broadcast: expand to each CONNECTED ACL conn; deep copy happens per
     * node so each conn's queue owns its own data. Mirrors NimBLE NRP. */
    bt_conn_get_acl_conns(&conns, &conns_count);

    for (size_t i = 0; i < conns_count; i++) {
        if (conns[i].state != BT_CONN_CONNECTED) {
            continue;
        }

        rc = gatts_indicate_enqueue(&conns[i], params, attr, value_handle);
        if (rc) {
            /* Per-conn failure (e.g. -ENODEV for a torn-down conn) — skip
             * and keep delivering to remaining conns. Partial-success
             * semantics, same as NimBLE NRP broadcast and the sibling
             * gatts_notify broadcast path above. */
            LOG_ERR("[B]GattsIndEnqueueFail[%u][%d]", conns[i].handle, rc);
            continue;
        }
    }

    return 0;
}

int bt_le_bluedroid_gattc_disc_start(uint16_t conn_handle)
{
    struct gatt_conn *gatt_conn;

    gatt_conn = bt_le_bluedroid_find_gatt_conn_with_handle(conn_handle);
    if (gatt_conn == NULL) {
        LOG_ERR("[B]NoConnInfo[%u]", conn_handle);
        return -ENODEV;
    }

    /* Central path: BTA_GATTC already opened + cached the peer DB inside
     * esp_ble_gattc_aux_open. Re-issuing Enh_Open here would either be a
     * no-op or trigger a duplicate open — warn the caller and skip so
     * role-agnostic callers can invoke this unconditionally.
     */
    if (gatt_conn->role == BTM_ROLE_MASTER) {
        LOG_INF("[B]DiscStartCentralSkip[%u]", conn_handle);
        return 0;
    }

    /* Peripheral path: BTA_GATTC has no prior knowledge of the peer (we did
     * not initiate the link). Enh_Open establishes the client view so the
     * stack can auto-discover the remote DB.
     */
    /* Non-PAWR connection: is_pawr_synced=false, adv_handle/subevent=0xff
     * sentinel (matches btc_gattc_open non-PAWR branch). phy_mask=0 means
     * "no PHY preference"; phy_*_conn_params NULL → controller defaults. */
    BTA_GATTC_Enh_Open(gattc_if, gatt_conn->peer.val, gatt_conn->peer.type,
                       true, BTA_GATT_TRANSPORT_LE, false, 0,
                       false, 0xff, 0xff, 0, NULL, NULL, NULL);

    /* Mark the gattc open as in progress */
    gatt_conn->gattc_open = 1;

    return 0;
}

static int gattc_disc_primary_svc(struct bt_conn *conn, struct bt_gatt_discover_params *params)
{
    struct bt_gatt_service_val svc = {0};
    struct bt_uuid_16 svc_uuid = {0};
    struct bt_gatt_attr attr = {0};
    btgatt_db_element_t *db = NULL;
    tBT_UUID uuid = {0};
    uint16_t count = 0;
    uint16_t conn_id;
    uint8_t ret;

    if (params->uuid == NULL) {
        LOG_ERR("[B]DiscAllPrimarySvcsNotSupp");
        return -ENOTSUP;
    }

    conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn->handle);
    bt_le_bluedroid_gatt_uuid_convert(params->uuid, &uuid);

    BTA_GATTC_GetServiceWithUUID(conn_id, &uuid, &db, &count);

    LOG_INF("[B]FoundPrimarySvc[%u][0x%04x][%u][%u]",
            count, uuid.uu.uuid16,
            params->start_handle, params->end_handle);

    if (params->func == NULL) {
        goto end;
    }

    if (db == NULL || count == 0) {
        params->func(conn, NULL, params);
        goto end;
    }

    ret = BT_GATT_ITER_CONTINUE;

    for (size_t i = 0; i < count; i++) {
        LOG_INF("[B]SvcInst[%u][%u][%u][%u][%u][%02x]",
                i, db[i].type, db[i].start_handle, db[i].end_handle,
                db[i].attribute_handle, db[i].properties);

        /* GetServiceWithUUID returns both primary and secondary services with
         * a matching UUID; this is primary-service discovery, so skip
         * secondaries. */
        if (db[i].type != BTGATT_DB_PRIMARY_SERVICE) {
            continue;
        }

        /* BTA's GetServiceWithUUID has no handle-range filter, so honor
         * the caller's [start_handle, end_handle] manually. Sibling
         * gattc_disc_included_svc / gattc_disc_chrc pass the range into
         * BTA so they don't need this. */
        if (db[i].start_handle < params->start_handle ||
                db[i].end_handle > params->end_handle) {
            continue;
        }

        svc_uuid.uuid.type = BT_UUID_TYPE_16;
        svc_uuid.val = BT_UUID_16(params->uuid)->val;

        svc.uuid = &svc_uuid.uuid;
        svc.end_handle = db[i].end_handle;

        attr.user_data = &svc;
        attr.handle = db[i].start_handle;
        attr.uuid = params->uuid;

        ret = params->func(conn, &attr, params);
        if (ret == BT_GATT_ITER_STOP) {
            break;
        }
    }

    if (ret == BT_GATT_ITER_CONTINUE) {
        params->func(conn, NULL, params);
    }

end:
    if (db) {
        free(db);
    }
    return 0;
}

static int gattc_disc_included_svc(struct bt_conn *conn, struct bt_gatt_discover_params *params)
{
    struct bt_gatt_include inc_svc = {0};
    struct bt_uuid_16 svc_uuid = {0};
    struct bt_gatt_attr attr = {0};
    btgatt_db_element_t *db = NULL;
    tBT_UUID uuid = {0};
    uint16_t count = 0;
    uint16_t conn_id;
    uint8_t ret;

    conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn->handle);
    if (params->uuid) {
        bt_le_bluedroid_gatt_uuid_convert(params->uuid, &uuid);
    }

    BTA_GATTC_GetIncludeService(conn_id, params->start_handle, params->end_handle,
                                params->uuid ? &uuid : NULL, &db, &count);

    if (params->uuid) {
        LOG_INF("[B]FoundInclSvc[%u][0x%04x][%u][%u]",
                count, uuid.uu.uuid16,
                params->start_handle, params->end_handle);
    } else {
        LOG_INF("[B]FoundInclSvc[%u][All][%u][%u]",
                count, params->start_handle, params->end_handle);
    }

    if (params->func == NULL) {
        goto end;
    }

    if (db == NULL || count == 0) {
        params->func(conn, NULL, params);
        goto end;
    }

    ret = BT_GATT_ITER_CONTINUE;

    for (size_t i = 0; i < count; i++) {
        LOG_INF("[B]InclSvcInst[%u][%u][%u][%u][%u][%02x]",
                i, db[i].type, db[i].start_handle, db[i].end_handle,
                db[i].attribute_handle, db[i].properties);

        svc_uuid.uuid.type = BT_UUID_TYPE_16;
        /* The LSB 12-octets is Bluetooth_Base_UUID, and the remaining
         * 2-octets or 4-octets is used by 16-bit or 32-bit UUID.
         */
        svc_uuid.val = sys_get_le16(db[i].uuid.uu + 12);

        inc_svc.uuid = &svc_uuid.uuid;
        inc_svc.start_handle = db[i].start_handle;
        inc_svc.end_handle = db[i].end_handle;

        /* Include declaration's own attribute type. Caller may dereference
         * attr->uuid (Zephyr bt_gatt_attr contract); leaving it NULL would
         * crash discovery callbacks. The included service's own UUID is
         * conveyed via attr.user_data (bt_gatt_include). */
        attr.uuid = BT_UUID_GATT_INCLUDE;
        attr.user_data = &inc_svc;
        /* Real include declaration handle from BTA cache. Callers (e.g.
         * vcp_vol_ctlr / cap_common) use attr->handle + 1 to advance the
         * next disc range; reporting params->start_handle would let BTA
         * re-emit the same include on every continuation pass.
         *
         * NimBLE side still reports params->start_handle (TODO until
         * CONFIG_BT_NIMBLE_INCL_SVC_DISCOVERY is wired). Lib tolerates
         * that via inst_cnt gates, so the dual-host divergence stays
         * functional — Bluedroid is just the more efficient path. */
        attr.handle = db[i].attribute_handle;

        ret = params->func(conn, &attr, params);
        if (ret == BT_GATT_ITER_STOP) {
            break;
        }
    }

    if (ret == BT_GATT_ITER_CONTINUE) {
        params->func(conn, NULL, params);
    }

end:
    if (db) {
        free(db);
    }
    return 0;
}

static int gattc_disc_chrc(struct bt_conn *conn, struct bt_gatt_discover_params *params)
{
    struct bt_uuid_16 chrc_uuid = {0};
    struct bt_gatt_attr attr = {0};
    struct bt_gatt_chrc chrc = {0};
    btgatt_db_element_t *db = NULL;
    tBT_UUID uuid = {0};
    uint16_t count = 0;
    uint16_t conn_id;
    uint8_t ret;

    conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn->handle);

    /* Note:
     * Increment start_handle with 1 here because, for example, if 2 characteristics
     * are found with characteristic value handle equals to A and B. After handling
     * the characteristic with value handle A, the LE Audio stack may try to find the
     * 2nd characteristic with value handle B with the start_handle set to A.
     * In this case, the characteristic with value handle A will still be found which
     * will cause infinite discovery loop.
     */
    /* The +1 above is intentionally disabled: the LE Audio lib callers already
     * re-invoke discovery with start_handle = attr->handle + 1 (e.g. cap_common.c,
     * mcc.c, vcp_vol_ctlr.c), and BTA's '>= start_handle' filter (bta_gattc_cache.c)
     * skips the previously found characteristic as-is. The nimble adapter uses the
     * same '>=' filter (gatt.db.c::handle_gattc_disc_chrs) and runs without issue.
     * Adding another +1 here would double-skip the attribute adjacent to the value
     * handle (e.g. the next chrc declaration).
     */
    /* params->start_handle += (params->start_handle == BT_ATT_FIRST_ATTRIBUTE_HANDLE ? 0 : 1); */

    if (params->uuid) {
        bt_le_bluedroid_gatt_uuid_convert(params->uuid, &uuid);

        BTA_GATTC_GetCharByUUID(conn_id, params->start_handle, params->end_handle, uuid, &db, &count);
    } else {
        BTA_GATTC_GetAllChar(conn_id, params->start_handle, params->end_handle, &db, &count);
    }

    if (params->uuid) {
        LOG_INF("[B]FoundChrc[%u][0x%04x][%u][%u]",
                count, uuid.uu.uuid16,
                params->start_handle, params->end_handle);
    } else {
        LOG_INF("[B]FoundChrc[%u][All][%u][%u]",
                count, params->start_handle, params->end_handle);
    }

    if (params->func == NULL) {
        goto end;
    }

    if (db == NULL || count == 0) {
        params->func(conn, NULL, params);
        goto end;
    }

    ret = BT_GATT_ITER_CONTINUE;

    for (size_t i = 0; i < count; i++) {
        LOG_INF("[B]ChrcInst[%u][%u][%u][%u][%u][%02x]",
                i, db[i].type, db[i].start_handle, db[i].end_handle,
                db[i].attribute_handle, db[i].properties);

        chrc_uuid.uuid.type = BT_UUID_TYPE_16;
        /* The LSB 12-octets is Bluetooth_Base_UUID, and the remaining
         * 2-octets or 4-octets is used by 16-bit or 32-bit UUID.
         */
        chrc_uuid.val = sys_get_le16(db[i].uuid.uu + 12);

        chrc.uuid = &chrc_uuid.uuid;
        chrc.value_handle = db[i].attribute_handle; /* Char val handle */
        chrc.properties = db[i].properties;

        /* Char declaration's own attribute type (Zephyr attr->uuid contract,
         * same as gattc_disc_included_svc); the char's UUID is in chrc.uuid. */
        attr.uuid = BT_UUID_GATT_CHRC;
        attr.user_data = &chrc;
        attr.handle = db[i].attribute_handle - 1;   /* Char def handle */

        ret = params->func(conn, &attr, params);
        if (ret == BT_GATT_ITER_STOP) {
            break;
        }
    }

    if (ret == BT_GATT_ITER_CONTINUE) {
        params->func(conn, NULL, params);
    }

end:
    if (db) {
        free(db);
    }
    return 0;
}

static int gattc_disc_chrc_desc(struct bt_conn *conn, struct bt_gatt_discover_params *params)
{
    struct bt_gatt_attr attr = {0};
    btgatt_db_element_t *db = NULL;
    tBTA_GATT_UNFMT write = {0};
    tBTA_GATT_STATUS status;
    uint16_t chrc_handle;
    tBT_UUID uuid = {0};
    uint16_t count = 0;
    uint16_t conn_id;
    int err = 0;

    assert(params);
    assert(params->uuid);
    assert(params->func);
    assert(params->sub_params);

    /* Only descriptors can be filtered */
    if (bt_uuid_cmp(params->uuid, BT_UUID_GATT_PRIMARY) == 0 ||
            bt_uuid_cmp(params->uuid, BT_UUID_GATT_SECONDARY) == 0 ||
            bt_uuid_cmp(params->uuid, BT_UUID_GATT_INCLUDE) == 0 ||
            bt_uuid_cmp(params->uuid, BT_UUID_GATT_CHRC) == 0) {
        LOG_ERR("[B]InvDescUuidToDisc[%02x]", params->uuid->type);
        return -EINVAL;
    }

    conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn->handle);
    chrc_handle = params->sub_params->value_handle;
    bt_le_bluedroid_gatt_uuid_convert(params->uuid, &uuid);

    BTA_GATTC_GetDescrByCharHandle(conn_id, chrc_handle, uuid, &db, &count);

    LOG_INF("[B]FoundChrcDesc[%u][%u][%u]",
            count, chrc_handle, params->end_handle);

    if (db == NULL || count == 0) {
        params->func(conn, NULL, params);

        err = -ENODEV;
        goto end;
    }

    if (count != 1) {
        LOG_ERR("[B]TooMuchChrcDesc[%u][%u]",
                count, chrc_handle);
        params->func(conn, NULL, params);

        err = -EINVAL;
        goto end;
    }

    LOG_INF("[B]DescInst[%u][%u][%u][%u][%02x]",
            db[0].type, db[0].start_handle, db[0].end_handle,
            db[0].attribute_handle, db[0].properties);

    attr.handle = db[0].attribute_handle;

    status = BTA_GATTC_RegisterForNotifications(gattc_if, conn->le.dst.a.val, chrc_handle);
    if (status != BTA_GATT_OK) {
        LOG_ERR("[B]EnableNtfFail[%u][%u]", chrc_handle, attr.handle);

        params->func(conn, NULL, params);

        err = -EIO;
        goto end;
    }

    write.len = sizeof(params->sub_params->value);
    write.p_value = (uint8_t *)&params->sub_params->value;

    LOG_INF("[B]EnableCcc[%u][%u]", attr.handle, write.len);

    BTA_GATTC_WriteCharDescr(conn_id, attr.handle, BTA_GATTC_TYPE_WRITE,
                             &write, BTA_GATT_AUTH_REQ_NONE);

    params->func(conn, &attr, params);

end:
    if (db) {
        free(db);
    }
    return err;
}

int bt_le_bluedroid_gattc_discover(struct bt_conn *conn, struct bt_gatt_discover_params *params)
{
    struct gatt_conn *gatt_conn;

    LOG_DBG("[B]GattcDisc[%u][%u]", conn->handle, params->type);

    gatt_conn = bt_le_bluedroid_find_gatt_conn_with_handle(conn->handle);
    if (gatt_conn == NULL) {
        LOG_ERR("[B]NoConnInfo[%u]", conn->handle);
        return -ENODEV;
    }

    /* Reject non-16-bit caller UUIDs up front — sibling disc helpers all
     * read the result UUID via sys_get_le16(db[i].uuid.uu + 12), which only
     * yields a valid value for 16-bit Bluetooth Base UUIDs; 32/128-bit
     * would silently truncate. BTA itself supports all three widths, but
     * the callback contract here is 16-bit only. Matches NimBLE's gate in
     * bt_le_nimble_gattc_discover. */
    if (params->uuid && params->uuid->type != BT_UUID_TYPE_16) {
        LOG_ERR("[B]DiscNon16BitUuid[%u]", params->uuid->type);
        return -ENOTSUP;
    }

    switch (params->type) {
    case BT_GATT_DISCOVER_PRIMARY:
        return gattc_disc_primary_svc(conn, params);

    case BT_GATT_DISCOVER_INCLUDE:
        return gattc_disc_included_svc(conn, params);

    case BT_GATT_DISCOVER_CHARACTERISTIC:
        return gattc_disc_chrc(conn, params);

    case BT_GATT_DISCOVER_DESCRIPTOR:
        return gattc_disc_chrc_desc(conn, params);

    default:
        LOG_ERR("[B]DiscTypeNotSupp[%u]", params->type);
        return -ENOTSUP;
    }
}

int bt_le_bluedroid_gattc_read(struct bt_conn *conn, struct bt_gatt_read_params *params)
{
    struct gatt_conn *gatt_conn;
    struct gattc_list_node *op;
    tBT_UUID uuid = {0};
    uint16_t conn_id;

    LOG_DBG("[B]GattcRd[%u]", conn->handle);

    gatt_conn = bt_le_bluedroid_find_gatt_conn_with_handle(conn->handle);
    if (gatt_conn == NULL) {
        LOG_ERR("[B]NoConnInfo[%u]", conn->handle);
        return -ENODEV;
    }

    /* BTA serializes via p_cmd_list (FIFO); we mirror with gattc_list so the
     * EVT can recover the caller's params. Append before submit so the head
     * is set when BTA delivers the cmpl synchronously on error paths. */
    op = gattc_list_node_alloc(GATTC_OP_READ, params);
    if (op == NULL) {
        LOG_ERR("[B]GattcRdOpAllocFail[%u]", conn->handle);
        return -ENOMEM;
    }

    sys_slist_append(&gatt_conn->gattc_list, &op->node);

    conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn->handle);

    if (params->handle_count == 0) {
        LOG_INF("[B]RdByTypeReq[0x%04x]", BT_UUID_16(params->by_uuid.uuid)->val);

        bt_le_bluedroid_gatt_uuid_convert(params->by_uuid.uuid, &uuid);

        BTA_GATTC_Read_by_type(conn_id, params->by_uuid.start_handle,
                               params->by_uuid.end_handle, &uuid,
                               BTA_GATT_AUTH_REQ_NONE);
    } else {
        LOG_INF("[B]RdReq[%u][%u]", params->single.handle, params->single.offset);

        /* A non-zero offset is a long read: the lib re-reads the remainder of
         * an ASE/BASS value whose notification was MTU-truncated. BTA's
         * ReadLongChar would run GATTC_Read inline on the ISO task, racing the
         * BTU task that owns gatt_cb. Always issue ReadCharacteristic instead
         * (GATT_READ_BY_HANDLE) — BTA auto-continues it with Read Blob on the
         * BTU task to fetch the full value, and handle_gattc_read_chrc_event
         * slices the requested [offset:] tail back to the caller. */
        BTA_GATTC_ReadCharacteristic(conn_id, params->single.handle,
                                     BTA_GATT_AUTH_REQ_NONE);
    }

    return 0;
}

int bt_le_bluedroid_gattc_write(struct bt_conn *conn, struct bt_gatt_write_params *params)
{
    struct gatt_conn *gatt_conn;
    struct gattc_list_node *op;
    uint16_t conn_id;

    LOG_DBG("[B]GattcWr[%u]", conn->handle);

    gatt_conn = bt_le_bluedroid_find_gatt_conn_with_handle(conn->handle);
    if (gatt_conn == NULL) {
        LOG_ERR("[B]NoConnInfo[%u]", conn->handle);
        return -ENODEV;
    }

    op = gattc_list_node_alloc(GATTC_OP_WRITE, params);
    if (op == NULL) {
        LOG_ERR("[B]GattcWrOpAllocFail[%u]", conn->handle);
        return -ENOMEM;
    }

    sys_slist_append(&gatt_conn->gattc_list, &op->node);

    conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn->handle);

    BTA_GATTC_WriteCharValue(conn_id, params->handle, BTA_GATTC_TYPE_WRITE,
                             params->length, (void *)params->data,
                             BTA_GATT_AUTH_REQ_NONE);

    return 0;
}

int bt_le_bluedroid_gattc_write_without_rsp(struct bt_conn *conn, uint16_t handle,
                                            const void *data, uint16_t length)
{
    struct gatt_conn *gatt_conn;
    uint16_t conn_id;

    LOG_DBG("[B]GattcWrCmd[%u][%u][%u]", conn->handle, handle, length);

    /* Mirror sibling GATT ops: refuse to forward doomed writes after
     * disconnect cleared the gatt_conn slot (bt_conn->state may briefly
     * still look CONNECTED during the teardown race). */
    gatt_conn = bt_le_bluedroid_find_gatt_conn_with_handle(conn->handle);
    if (gatt_conn == NULL) {
        LOG_ERR("[B]NoConnInfo[%u]", conn->handle);
        return -ENODEV;
    }

    conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn->handle);

    /* TODO: BTA_GATTC_WriteCharValue is void and silently drops the cmd on
     * osi_malloc failure. NimBLE returns the host status here, so the API
     * contract documents -ENOMEM as reachable, but this path can never
     * report it. Acceptable today because write_without_rsp is wire-level
     * best-effort anyway; revisit if BTA gains a status-returning variant. */
    BTA_GATTC_WriteCharValue(conn_id, handle, BTA_GATTC_TYPE_WRITE_NO_RSP,
                             length, (void *)data, BTA_GATT_AUTH_REQ_NONE);

    return 0;
}

int bt_le_bluedroid_gattc_write_ccc(struct bt_conn *conn, struct bt_gatt_subscribe_params *params)
{
    tBTA_GATT_UNFMT write = {0};
    tBTA_GATT_STATUS status;
    uint16_t chrc_handle;
    uint16_t conn_id;

    conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn->handle);
    chrc_handle = params->value_handle;

    /* Unsubscribe path: caller (bt_gatt_unsubscribe) forces params->value to
     * 0x0000. BTA's notif_reg[] is a purely local routing table — Dereg only
     * clears our callback slot for (bda, handle); a not-found error here
     * (BTA_GATT_ERROR) just means the slot was already gone, e.g.:
     *   - disconnect cleanup wiped notif_reg before the host caught up;
     *   - a previous unsubscribe already ran;
     *   - BTA bookkeeping diverged from ours for any reason.
     * It does NOT mean the CCC write to the peer can be skipped: the peer
     * still needs CCC=0 to stop notifications on the wire, and the upper
     * layer needs success so it removes the node from its subscription
     * list — otherwise the node is stuck forever.
     */
    status = BTA_GATTC_DeregisterForNotifications(gattc_if, conn->le.dst.a.val, chrc_handle);
    if (status != BTA_GATT_OK) {
        LOG_WRN("[B]DeregNtfFail[%u][%u][%d]", chrc_handle, params->ccc_handle, status);
    }

    write.len = sizeof(params->value);
    write.p_value = (uint8_t *)&params->value;

    BTA_GATTC_WriteCharDescr(conn_id, params->ccc_handle, BTA_GATTC_TYPE_WRITE,
                             &write, BTA_GATT_AUTH_REQ_NONE);

    return 0;
}

int bt_le_bluedroid_gatt_init(void)
{
    gatt_shutting_down = false;
    k_sem_create(&gatts_sem);
    k_sem_create(&gattc_sem);

    /* Threading: gatts_app_cb / gattc_app_cb run in BTU (BTA dispatches
     * inline, no BTC hop). BTU is shared with classic BT, so handlers only
     * marshal the event (calloc + memcpy) and post to the ISO task — heavy
     * work stays off BTU. Classic BT is unaffected: GATT is LE-only, no
     * shared BTA module or BTC path.
     *
     * BTU task (single task) layout:
     *   ├─ HCI event dispatch     classic + LE
     *   ├─ BTM  ble_*, btm_*      LE
     *   ├─ BTM  btm_acl_*, sec    classic + LE
     *   ├─ BTA_AV/AG/SPP…         classic
     *   ├─ BTA_GATTS/GATTC        LE (iso)
     *   ├─ BTA_DM                 classic + LE
     *   └─ btc_transfer_context   classic app cb hops to BTC task here
     */

    k_sem_reset(&gatts_sem);
    BTA_GATTS_AppRegister(&gatts_app_uuid, gatts_app_cb);

    if (k_sem_take(&gatts_sem, K_SEM_SHORT) || gatts_sem.result) {
        LOG_ERR("[B]GattsRegFail");
        return -1;
    }

    k_sem_reset(&gattc_sem);
    BTA_GATTC_AppRegister(&gattc_app_uuid, gattc_app_cb);

    if (k_sem_take(&gattc_sem, K_SEM_SHORT) || gattc_sem.result) {
        LOG_ERR("[B]GattcRegFail");
        return -1;
    }

    for (size_t i = 0; i < ARRAY_SIZE(gatt_conns); i++) {
        reset_gatt_conn(&gatt_conns[i]);
    }

    return 0;
}

void bt_le_bluedroid_gatt_deinit(void)
{
    /* Block late REG_EVT gives before tearing down the sems. */
    gatt_shutting_down = true;

    if (gattc_if != 0) {
        BTA_GATTC_AppDeregister(gattc_if);
        gattc_if = 0;
    }

    if (gatts_if != 0) {
        BTA_GATTS_AppDeregister(gatts_if);
        gatts_if = 0;
    }

    /* Free any operation nodes still queued on per-conn lists (gattc_list,
     * gatts_list, gatts_notify_list). Mirrors the reset in init so an
     * init/deinit/re-init cycle doesn't accumulate leaks. */
    for (size_t i = 0; i < ARRAY_SIZE(gatt_conns); i++) {
        reset_gatt_conn(&gatt_conns[i]);
    }

    k_sem_delete(&gattc_sem);
    k_sem_delete(&gatts_sem);
}
