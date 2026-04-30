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
#include <zephyr/bluetooth/common/bt_str.h>

#include <../host/conn_internal.h>

#include "host/ble_hs.h"
#include "host/ble_att.h"
#include "host/ble_gatt.h"
#include "host/ble_hs_mbuf.h"

#include "common/host.h"

#include "nimble/hs_error.h"
#include "common/app/gatt.h"

LOG_MODULE_REGISTER(ISO_NGAT, CONFIG_BT_ISO_LOG_LEVEL);

void bt_le_nimble_gatt_post_event(void *param)
{
    struct bt_le_gatt_event_param *qev;
    struct ble_gap_event *ev;
    int err;

    ev = param;

    /* LOG_DBG("[N]PostGattEvt[%u]", ev->type); */

    /* Currently we only handle this event for indication */
    if (ev->type == BLE_GAP_EVENT_NOTIFY_TX && ev->notify_tx.indication == 0) {
        return;
    }

    qev = calloc(1, sizeof(*qev));
    assert(qev);

    switch (ev->type) {
    case BLE_GAP_EVENT_MTU:
        qev->type = BT_LE_GATT_MTU_CHANGE_EVENT;

        qev->gatt_mtu_change.conn_handle = ev->mtu.conn_handle;
        qev->gatt_mtu_change.mtu = ev->mtu.value;
        break;

    case BLE_GAP_EVENT_SUBSCRIBE:
        qev->type = BT_LE_GATTS_SUBSCRIBE_EVENT;

        qev->gatts_subscribe.conn_handle = ev->subscribe.conn_handle;
        qev->gatts_subscribe.attr_handle = ev->subscribe.attr_handle;
        qev->gatts_subscribe.prev_notify = ev->subscribe.prev_notify;
        qev->gatts_subscribe.cur_notify = ev->subscribe.cur_notify;
        qev->gatts_subscribe.prev_indicate = ev->subscribe.prev_indicate;
        qev->gatts_subscribe.cur_indicate = ev->subscribe.cur_indicate;
        qev->gatts_subscribe.reason = ev->subscribe.reason;
        break;

    case BLE_GAP_EVENT_NOTIFY_RX:
        qev->type = BT_LE_GATTC_NOTIFY_RX_EVENT;

        qev->gattc_notify_rx.conn_handle = ev->notify_rx.conn_handle;
        qev->gattc_notify_rx.attr_handle = ev->notify_rx.attr_handle;
        qev->gattc_notify_rx.is_notify = (ev->notify_rx.indication == 0);

        if (ev->notify_rx.om && OS_MBUF_PKTLEN(ev->notify_rx.om)) {
            uint16_t total_len = OS_MBUF_PKTLEN(ev->notify_rx.om);
            qev->gattc_notify_rx.len = total_len;

            qev->gattc_notify_rx.value = calloc(1, total_len);
            assert(qev->gattc_notify_rx.value);

            os_mbuf_copydata(ev->notify_rx.om, 0, total_len, qev->gattc_notify_rx.value);
        }
        break;

    case BLE_GAP_EVENT_NOTIFY_TX:
        qev->type = BT_LE_GATTS_NOTIFY_TX_EVENT;

        qev->gatts_notify_tx.is_notify = (ev->notify_tx.indication == 0);
        qev->gatts_notify_tx.conn_handle = ev->notify_tx.conn_handle;
        qev->gatts_notify_tx.attr_handle = ev->notify_tx.attr_handle;
        qev->gatts_notify_tx.status = ev->notify_tx.status;
        break;

    default:
        free(qev);
        assert(0);
        break;
    }

    err = bt_le_iso_task_post(ISO_QUEUE_ITEM_TYPE_GATT_EVENT, qev, sizeof(*qev));
    if (err) {
        LOG_ERR("[N]GattPostEvtFail[%d][%u]", err, qev->type);

        if (qev->type == BT_LE_GATTC_NOTIFY_RX_EVENT &&
                qev->gattc_notify_rx.value) {
            free(qev->gattc_notify_rx.value);
            qev->gattc_notify_rx.value = NULL;
        }

        free(qev);
    }
}

int bt_le_nimble_gatt_post_disc_event(uint16_t conn_handle, ble_uuid16_t *uuid,
                                      struct bt_gatt_discover_params *params,
                                      uint8_t type)
{
    struct bt_le_gatt_event_param *qev;
    int err;

    LOG_DBG("[N]GattcDiscHdlr[%u][%u]", conn_handle, type);

    if (type == GATTC_DISC_TYPE_DUMMY || type >= GATTC_DISC_TYPE_MAX) {
        LOG_ERR("[N]InvDiscType[%u]", type);
        return -EINVAL;
    }

    qev = calloc(1, sizeof(*qev));
    assert(qev);

    qev->type = BT_LE_GATTC_DISCOVER_EVENT;

    qev->gattc_discover.type = type;
    qev->gattc_discover.conn_handle = conn_handle;
    qev->gattc_discover.uuid = (uuid ? uuid->value : 0x0000);
    qev->gattc_discover.params = params;

    err = bt_le_iso_task_post(ISO_QUEUE_ITEM_TYPE_GATT_EVENT, qev, sizeof(*qev));
    if (err) {
        LOG_ERR("[N]GattPostDiscEvtFail[%d][%u]", err, type);
        free(qev);
        return err;
    }

    return 0;
}

int bt_le_nimble_gatt_post_disc_cmpl_event(uint16_t conn_handle, uint8_t status)
{
    struct bt_le_gatt_event_param *qev;
    int err;

    LOG_DBG("[N]GattcDiscCmplEvtHdlr[%u][%02x]", conn_handle, status);

    qev = calloc(1, sizeof(*qev));
    assert(qev);

    qev->type = BT_LE_GATTC_DISC_CMPL_EVENT;

    qev->gattc_disc_cmpl.status = status;
    qev->gattc_disc_cmpl.conn_handle = conn_handle;

    err = bt_le_iso_task_post(ISO_QUEUE_ITEM_TYPE_GATT_EVENT, qev, sizeof(*qev));
    if (err) {
        LOG_ERR("[N]GattPostDiscCmplEvtFail[%d][%u][%02x]", err, conn_handle, status);
        free(qev);
        return err;
    }

    return 0;
}

static void handle_gatts_subscribe_event_safe(struct bt_le_gatts_subscribe_event *event)
{
    /* LOG_DBG("[N]HdlGattsSubEvt[%u][%u][%u][%u][%02x]", */
    /*         event->conn_handle, event->attr_handle, event->cur_notify, */
    /*         event->cur_indicate, event->reason); */

    /* Note:
     * For NimBLE Host, the attr_handle is the handle of characteristic value
     * declaration. For CCCD, its attr_handle shall be incremented with 1.
     */
    bt_le_host_lock();

    bt_gatts_sub_changed(event->conn_handle,
                         event->attr_handle + 1,
                         event->cur_notify,
                         event->cur_indicate,
                         event->reason);

    bt_le_host_unlock();

    bt_le_gatts_app_subscribe_event(event);
}

static void handle_gattc_notify_rx_event_safe(struct bt_le_gattc_notify_rx_event *event)
{
    struct bt_gatt_subscribe_params *params;
    struct gattc_sub *sub;
    struct bt_conn *conn;

    LOG_DBG("[N]HdlGattcNtfRxEvt[%u][%u][%u]",
            event->conn_handle, event->attr_handle, event->len);

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(event->conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("[N]NotConn[%d]", __LINE__);
        goto end;
    }

    sub = bt_gattc_sub_find(conn);
    if (sub == NULL) {
        goto end;
    }

    SYS_SLIST_FOR_EACH_CONTAINER(&sub->list, params, node) {
        if (params->ccc_handle != BT_GATT_AUTO_DISCOVER_CCC_HANDLE &&
                params->value_handle == event->attr_handle) {
            if (params->notify) {
                params->notify(conn, params, event->value, event->len);
            }
        }
    }

end:
    if (event->value) {
        free(event->value);
    }

    bt_le_host_unlock();
}

static void handle_gatts_notify_tx_event_safe(struct bt_le_gatts_notify_tx_event *event)
{
    LOG_DBG("[N]HdlGattsNtfTxEvt[%u][%u][%u][%02x]",
            event->conn_handle, event->attr_handle, event->is_notify, event->status);

    /* Currently we only handle this event for indication */
    if (event->is_notify) {
        return;
    }

    /* The event->status could be:
     * - 0: Indication is sent successfully;
     * - BLE_HS_EDONE: Confirm is received;
     * - BLE_HS_ENOTCONN: Not connected;
     * - BLE_HS_ETIMEOUT: ATT procedure times out.
     */
    bt_le_host_lock();

    bt_le_nimble_gatts_nrp_indicate_cb(event->conn_handle, event->attr_handle, event->status);

    bt_le_host_unlock();
}

void bt_le_nimble_gatt_handle_event(uint8_t *data, size_t data_len)
{
    struct bt_le_gatt_event_param *param;

    param = (struct bt_le_gatt_event_param *)data;

    /* LOG_DBG("[N]HdlGattEvt[%u]", param->type); */

    switch (param->type) {
    case BT_LE_GATT_MTU_CHANGE_EVENT:
        bt_le_gatt_app_mtu_change_event(param->gatt_mtu_change.conn_handle,
                                        param->gatt_mtu_change.mtu);
        break;

    case BT_LE_GATTC_DISCOVER_EVENT:
        handle_gattc_db_disc_event_safe(&param->gattc_discover);
        break;

    case BT_LE_GATTC_DISC_CMPL_EVENT:
        bt_le_gattc_app_disc_cmpl_event(&param->gattc_disc_cmpl);
        break;

    case BT_LE_GATTS_SUBSCRIBE_EVENT:
        handle_gatts_subscribe_event_safe(&param->gatts_subscribe);
        break;

    case BT_LE_GATTC_NOTIFY_RX_EVENT:
        handle_gattc_notify_rx_event_safe(&param->gattc_notify_rx);
        break;

    case BT_LE_GATTS_NOTIFY_TX_EVENT:
        handle_gatts_notify_tx_event_safe(&param->gatts_notify_tx);
        break;

    default:
        assert(0);
        break;
    }

    free(data);
}

uint16_t bt_le_nimble_gatt_get_mtu(struct bt_conn *conn)
{
    LOG_DBG("[N]GattGetMtu[%u]", conn->handle);

    return ble_att_mtu(conn->handle);
}

ssize_t bt_le_nimble_gatts_attr_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                     void *buf, uint16_t buf_len, uint16_t offset,
                                     const void *value, uint16_t value_len)
{
    uint16_t len;

    if (buf_len == UINT16_MAX) {
        /* TODO: A better solution for using a variable for the cb pointer */
        struct bt_le_nimble_gatt_read_cb *cb = buf;
        assert(cb->read_cb);

        len = cb->read_cb(cb->read_arg, offset, value, value_len);
    } else {
        assert(attr);

        LOG_DBG("[N]Hdl[%u]", attr->handle);

        if (offset >= value_len) {
            LOG_WRN("[N]OffsetTooLarge[%u][%u]", offset, value_len);
            return 0;
        }

        len = MIN(buf_len, value_len - offset);
        memcpy(buf, (uint8_t *)value + offset, len);
    }

    LOG_DBG("[N]GattsAttrRd[%u]", len);

    return len;
}

static int gatts_notify(struct bt_conn *conn, struct bt_gatt_notify_params *params)
{
    struct notify_data data;
    struct bt_conn *conns;
    uint8_t conns_count;
    struct os_mbuf *om;
    int rc = 0;

    memset(&data, 0, sizeof(data));

    data.attr = params->attr;
    data.handle = bt_gatt_attr_get_handle(data.attr);

    /* Lookup UUID if it was given */
    if (params->uuid) {
        if (bt_gatts_find_attr_by_uuid(&data, params->uuid) == false) {
            return -ENOENT;
        }

        params->attr = data.attr;
    } else {
        if (data.handle == 0) {
            LOG_WRN("[N]DataHdlZero");
            return -ENOENT;
        }
    }

    /* Check if attribute is a characteristic then adjust the handle */
    if (bt_uuid_cmp(data.attr->uuid, BT_UUID_GATT_CHRC) == 0) {
        struct bt_gatt_chrc *chrc;

        assert(data.attr->user_data);
        chrc = data.attr->user_data;

        if ((chrc->properties & BT_GATT_CHRC_NOTIFY) == 0) {
            LOG_WRN("[N]NtfNotSupp[%u][%02x]", data.handle, chrc->properties);
            return -EINVAL;
        }

        data.handle = bt_gatt_attr_value_handle(data.attr);

        LOG_INF("[N]DataHdl[%u][%u]", data.attr->handle, data.handle);
    }

    if (conn) {
        om = ble_hs_mbuf_from_flat(params->data, params->len);
        if (om == NULL) {
            LOG_ERR("[N]NoMemForNtf[%u]", params->len);
            return -ENOMEM;
        }

        rc = ble_gatts_notify_custom(conn->handle, data.handle, om);
        if (rc && rc != BLE_HS_ENOTCONN) {
            LOG_ERR("[N]GattsNtfCustomFail[%d]", rc);
        }
    } else {
        bt_conn_get_acl_conns(&conns, &conns_count);

        for (size_t i = 0; i < conns_count; i++) {
            if (conns[i].state == BT_CONN_CONNECTED) {
                om = ble_hs_mbuf_from_flat(params->data, params->len);
                if (om == NULL) {
                    LOG_ERR("[N]NoMemForNtf[%u]", params->len);
                    return -ENOMEM;
                }

                rc = ble_gatts_notify_custom(conns[i].handle, data.handle, om);
                if (rc && rc != BLE_HS_ENOTCONN) {
                    LOG_ERR("[N]GattsNtfCustomFail[%d]", rc);
                }
            }
        }

        /* For sending notifications to all connected peers, always return success */
        rc = 0;
    }

    return rc;
}

int bt_le_nimble_gatts_notify(struct bt_conn *conn, struct bt_gatt_notify_params *params)
{
    LOG_DBG("[N]GattsNtf[%u]", conn ? conn->handle : UINT16_MAX);

    return nimble_err_to_errno(gatts_notify(conn, params));
}

int bt_le_nimble_gatts_indicate(struct bt_conn *conn, struct bt_gatt_indicate_params *params)
{
    LOG_DBG("[N]GattsInd[%u]", conn ? conn->handle : UINT16_MAX);

    return bt_le_nimble_gatt_nrp_insert(conn, GATTS_NRP_INDICATE, params);
}

int bt_le_nimble_gattc_disc_start(uint16_t conn_handle)
{
    LOG_DBG("[N]GattcDiscStart[%u]", conn_handle);

    return bt_le_nimble_gattc_db_auto_disc(conn_handle);
}

int bt_le_nimble_gattc_discover(struct bt_conn *conn, struct bt_gatt_discover_params *params)
{
    ble_uuid16_t uuid = {0};

    LOG_DBG("[N]GattcDisc[%u]", conn->handle);

    if (params->uuid) {
        uuid.u.type = BLE_UUID_TYPE_16;
        uuid.value = BT_UUID_16(params->uuid)->val;
    }

    switch (params->type) {
    case BT_GATT_DISCOVER_PRIMARY:
        if (params->uuid) {
            return bt_le_nimble_gattc_db_disc_svc_by_uuid(conn, &uuid, params);
        }

        LOG_ERR("[N]NotSuppDiscAllPrimarySvcs");
        return -ENOTSUP;

    case BT_GATT_DISCOVER_INCLUDE:
        return bt_le_nimble_gattc_db_find_inc_svcs(conn, params);

    case BT_GATT_DISCOVER_CHARACTERISTIC:
        if (params->uuid) {
            return bt_le_nimble_gattc_db_disc_chrs_by_uuid(conn, &uuid, params);
        }

        return bt_le_nimble_gattc_db_disc_all_chrs(conn, params);

    case BT_GATT_DISCOVER_DESCRIPTOR:
        assert(params->uuid);

        /* Only descriptors can be filtered */
        if (bt_uuid_cmp(params->uuid, BT_UUID_GATT_PRIMARY) == 0 ||
                bt_uuid_cmp(params->uuid, BT_UUID_GATT_SECONDARY) == 0 ||
                bt_uuid_cmp(params->uuid, BT_UUID_GATT_INCLUDE) == 0 ||
                bt_uuid_cmp(params->uuid, BT_UUID_GATT_CHRC) == 0) {
            LOG_ERR("[N]InvDscToDisc[%02x]", params->uuid->type);
            return -EINVAL;
        }

        return bt_le_nimble_gattc_db_disc_all_dscs(conn, params);

    default:
        LOG_ERR("[N]DiscNotSupp[%u]", params->type);
        return -ENOTSUP;
    }
}

int bt_le_nimble_gattc_read(struct bt_conn *conn, struct bt_gatt_read_params *params)
{
    uint8_t type;

    if (params->handle_count == 0) {
        type = GATTC_NRP_READ_BY_UUID;
    } else if (params->single.offset) {
        type = GATTC_NRP_READ_LONG;
    } else {
        type = GATTC_NRP_READ_SINGLE;
    }

    LOG_DBG("[N]GattcRd[%u][%u]", conn->handle, type);

    return bt_le_nimble_gatt_nrp_insert(conn, type, params);
}

int bt_le_nimble_gattc_write(struct bt_conn *conn, struct bt_gatt_write_params *params)
{
    LOG_DBG("[N]GattcWr[%u]", conn->handle);

    return bt_le_nimble_gatt_nrp_insert(conn, GATTC_NRP_WRITE_REQ, params);
}

int bt_le_nimble_gattc_write_without_rsp(struct bt_conn *conn, uint16_t handle,
                                         const void *data, uint16_t length)
{
    LOG_DBG("[N]GattcWrCmd[%u][%u][%u]", conn->handle, handle, length);

    return nimble_err_to_errno(ble_gattc_write_no_rsp_flat(conn->handle, handle, data, length));
}

int bt_le_nimble_gattc_write_ccc(struct bt_conn *conn, struct bt_gatt_subscribe_params *params)
{
    /* LOG_DBG("[N]GattcWrCcc[%u]", conn->handle); */

    return bt_le_nimble_gatt_nrp_insert(conn, GATTC_NRP_SUBSCRIBE, params);
}
