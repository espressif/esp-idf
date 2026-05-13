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

LOG_MODULE_REGISTER(ISO_NGRP, CONFIG_BT_ISO_LOG_LEVEL);

struct gatt_nrp_node {
    uint8_t type;

    union {
        struct {
            struct bt_gatt_read_params *params;
            /* read_by_uuid is multi-event (one per matching attr + EDONE);
             * cb may return BT_GATT_ITER_STOP early, after which we must
             * not invoke it again for trailing events on this procedure. */
            bool iter_stopped;
        } read_by_uuid;

        struct {
            struct bt_gatt_read_params *params;
            /* Same multi-event story as read_by_uuid; per-fragment events
             * may precede EDONE and the cb may STOP early. */
            bool iter_stopped;
        } read_long;

        struct {
            struct bt_gatt_read_params *params;
        } read_single;

        struct {
            struct bt_gatt_write_params *params;
            /* Heap copy of params->data captured at insert time. Callers may pass
             * a pointer to a stack-allocated buffer (e.g. NET_BUF_SIMPLE_DEFINE
             * inside a helper that returns immediately after bt_gatt_write); when
             * the request gets queued behind another in-flight GATT op, the
             * caller's buffer is gone by the time we re-dispatch. Hold our own
             * copy and dispatch from it. NULL when length == 0. */
            uint8_t *data_copy;
        } write_req;

        struct {
            struct bt_gatt_subscribe_params *params;
        } subscribe;

        struct {
            /* Deep-copied at insert: callers may reuse the params slot or
             * pass stack-backed data while we are queued/in-flight. */
            struct bt_gatt_indicate_params *params;
            struct bt_gatt_indicate_params params_copy;
            uint8_t *data_copy;
        } indicate;
    };

    sys_snode_t node;
};

static struct gatt_nrp {
    uint16_t conn_handle;

    sys_slist_t list;   /* List of GATT need response pdu (e.g., read req, write req, indication, etc.) */
} gatt_nrp[CONFIG_BT_MAX_CONN] = {
    [0 ...(CONFIG_BT_MAX_CONN - 1)] = {
        .conn_handle = UINT16_MAX,
    },
};

static struct gatt_nrp *gatt_nrp_find(uint16_t conn_handle);

static int gattc_nrp_read_by_uuid_cb_safe(uint16_t conn_handle,
                                          const struct ble_gatt_error *error,
                                          struct ble_gatt_attr *attr,
                                          void *arg)
{
    struct bt_gatt_read_params *read_params;
    struct gatt_nrp_node *nrp_node;
    struct gatt_nrp *nrp;
    struct bt_conn *conn;
    sys_snode_t *node;
    bool iter_stopped;
    int rc = 0;

    read_params = arg;
    assert(read_params);
    assert(read_params->func);
    assert(read_params->handle_count == 0);
    assert(read_params->by_uuid.uuid);
    assert(read_params->by_uuid.uuid->type == BT_UUID_TYPE_16);

    LOG_DBG("[N]GattcNrpRdByUuidCb[%u][%04x][%s]",
            conn_handle, error->status, bt_uuid_str(read_params->by_uuid.uuid));

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("[N]GattcNrpNotConn[%d]", __LINE__);
        rc = -ENOTCONN;
        goto end;
    }

    /* Peek (don't pop) the in-flight head; READ_BY_UUID delivers per-attr
     * events followed by EDONE, so the head must survive across events.
     * It is removed only on terminal status (EDONE / error) below. */
    nrp = gatt_nrp_find(conn->handle);
    assert(nrp);
    node = sys_slist_peek_head(&nrp->list);
    assert(node);
    nrp_node = CONTAINER_OF(node, struct gatt_nrp_node, node);
    assert(nrp_node->type == GATTC_NRP_READ_BY_UUID);
    assert(nrp_node->read_by_uuid.params == read_params);

    iter_stopped = nrp_node->read_by_uuid.iter_stopped;

    switch (error->status) {
    case 0:
        assert(attr);
        assert(attr->om);

        LOG_DBG("[N]GattcNrpHdl[%u][%u][%u]Len[%u]",
                attr->handle, read_params->by_uuid.start_handle,
                read_params->by_uuid.end_handle, attr->om->om_len);

        if (read_params->by_uuid.start_handle == BT_ATT_FIRST_ATTRIBUTE_HANDLE) {
            read_params->by_uuid.start_handle = attr->handle;
        }

        if (!iter_stopped) {
            uint8_t cb_ret = read_params->func(conn, 0, read_params,
                                               attr->om->om_data, attr->om->om_len);
            if (cb_ret == BT_GATT_ITER_STOP) {
                nrp_node->read_by_uuid.iter_stopped = true;
            }
        }
        break;

    case BLE_HS_EDONE:
        bt_le_nimble_gatt_nrp_remove(conn, GATTC_NRP_READ_BY_UUID, read_params, 0);
        if (!iter_stopped) {
            read_params->func(conn, 0, read_params, NULL, 0);
        }
        break;

    default:
        LOG_WRN("[N]GattcNrpStatus[%04x]", error->status);

        bt_le_nimble_gatt_nrp_remove(conn, GATTC_NRP_READ_BY_UUID, read_params, 0);
        if ((error->status & BLE_HS_ERR_ATT_BASE) && !iter_stopped) {
            read_params->func(conn, (uint8_t)error->status, read_params, NULL, 0);
        }

        rc = error->status;
        break;
    }

end:
    bt_le_host_unlock();

    return rc;
}

static int gattc_nrp_read_long_cb_safe(uint16_t conn_handle,
                                       const struct ble_gatt_error *error,
                                       struct ble_gatt_attr *attr,
                                       void *arg)
{
    struct bt_gatt_read_params *read_params;
    struct gatt_nrp_node *nrp_node;
    struct gatt_nrp *nrp;
    struct bt_conn *conn;
    sys_snode_t *node;
    bool iter_stopped;
    int rc = 0;

    read_params = arg;
    assert(read_params);
    assert(read_params->func);
    assert(read_params->handle_count == 1);
    assert(read_params->single.offset != 0);

    LOG_DBG("[N]GattcNrpRdLongCb[%u][%04x]", conn_handle, error->status);

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("[N]GattcNrpNotConn[%d]", __LINE__);
        rc = -ENOTCONN;
        goto end;
    }

    /* Peek (don't pop) — per-fragment events may precede EDONE; head is
     * removed only on terminal status below. */
    nrp = gatt_nrp_find(conn->handle);
    assert(nrp);
    node = sys_slist_peek_head(&nrp->list);
    assert(node);
    nrp_node = CONTAINER_OF(node, struct gatt_nrp_node, node);
    assert(nrp_node->type == GATTC_NRP_READ_LONG);
    assert(nrp_node->read_long.params == read_params);

    iter_stopped = nrp_node->read_long.iter_stopped;

    switch (error->status) {
    case 0:
        assert(attr);
        assert(attr->om);

        LOG_DBG("[N]GattcNrpHdl[%u][%u]Offset[%u]Len[%u]",
                attr->handle, read_params->single.handle, attr->offset, attr->om->om_len);

        if (!iter_stopped) {
            uint8_t cb_ret = read_params->func(conn, 0, read_params,
                                               attr->om->om_data, attr->om->om_len);
            if (cb_ret == BT_GATT_ITER_STOP) {
                nrp_node->read_long.iter_stopped = true;
            }
        }
        break;

    case BLE_HS_EDONE:
        bt_le_nimble_gatt_nrp_remove(conn, GATTC_NRP_READ_LONG, read_params, 0);
        if (!iter_stopped) {
            read_params->func(conn, 0, read_params, NULL, 0);
        }
        break;

    default:
        LOG_WRN("[N]GattcNrpStatus[%04x]", error->status);

        bt_le_nimble_gatt_nrp_remove(conn, GATTC_NRP_READ_LONG, read_params, 0);
        if ((error->status & BLE_HS_ERR_ATT_BASE) && !iter_stopped) {
            read_params->func(conn, (uint8_t)error->status, read_params, NULL, 0);
        }

        rc = error->status;
        break;
    }

end:
    bt_le_host_unlock();

    return rc;
}

static int gattc_nrp_read_single_cb_safe(uint16_t conn_handle,
                                         const struct ble_gatt_error *error,
                                         struct ble_gatt_attr *attr,
                                         void *arg)
{
    struct bt_gatt_read_params *read_params;
    struct bt_conn *conn;
    int rc = 0;

    read_params = arg;
    assert(read_params);
    assert(read_params->func);
    assert(read_params->handle_count == 1);
    assert(read_params->single.offset == 0);

    LOG_DBG("[N]GattcNrpRdSingleCb[%u][%04x]", conn_handle, error->status);

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("[N]GattcNrpNotConn[%d]", __LINE__);
        rc = -ENOTCONN;
        goto end;
    }

    /* Shall be invoked before the read_params->func is invoked */
    bt_le_nimble_gatt_nrp_remove(conn, GATTC_NRP_READ_SINGLE, read_params, 0);

    switch (error->status) {
    case 0:
        assert(attr);
        assert(attr->om);

        LOG_DBG("[N]GattcNrpHdl[%u][%u]Len[%u]",
                attr->handle, read_params->single.handle, attr->om->om_len);

        if (read_params->func(conn, 0, read_params,
                              attr->om->om_data,
                              attr->om->om_len) == BT_GATT_ITER_CONTINUE) {
            /* Workaround for completing the handling of ATT Read Response
             * by BAP Unicast Client.
             */
            read_params->func(conn, 0, read_params, NULL, 0);
        }
        break;

    case BLE_HS_EDONE:
        read_params->func(conn, 0, read_params, NULL, 0);
        break;

    default:
        LOG_WRN("[N]GattcNrpStatus[%04x]", error->status);

        if (error->status & BLE_HS_ERR_ATT_BASE) {
            read_params->func(conn, (uint8_t)error->status, read_params, NULL, 0);
        }

        rc = error->status;
        break;
    }

end:
    bt_le_host_unlock();

    return rc;
}

static int gattc_nrp_read(struct bt_conn *conn, struct bt_gatt_read_params *params)
{
    int rc;

    LOG_DBG("[N]GattcNrpRd[%u][%u]", conn->handle, params->handle_count);

    if (params->handle_count == 0) {
        LOG_DBG("[N]GattcNrpByUuid[%u][%u][%s]",
                params->by_uuid.start_handle, params->by_uuid.end_handle,
                bt_uuid_str(params->by_uuid.uuid));

        ble_uuid16_t uuid = {
            .u.type = BLE_UUID_TYPE_16,
            .value = BT_UUID_16(params->by_uuid.uuid)->val,
        };

        rc = ble_gattc_read_by_uuid(conn->handle,
                                    params->by_uuid.start_handle,
                                    params->by_uuid.end_handle,
                                    &uuid.u,
                                    gattc_nrp_read_by_uuid_cb_safe,
                                    params);
        if (rc) {
            LOG_ERR("[N]GattcNrpRdByUuidFail[%d]", rc);
        }
    } else if (params->single.offset) {
        LOG_DBG("[N]GattcNrpLong[%u][%u]", params->single.handle, params->single.offset);

        rc = ble_gattc_read_long(conn->handle,
                                 params->single.handle,
                                 params->single.offset,
                                 gattc_nrp_read_long_cb_safe,
                                 params);
        if (rc) {
            LOG_ERR("[N]GattcNrpRdLongFail[%d]", rc);
        }
    } else {
        LOG_DBG("[N]GattcNrpSingle[%u]", params->single.handle);

        rc = ble_gattc_read(conn->handle,
                            params->single.handle,
                            gattc_nrp_read_single_cb_safe,
                            params);
        if (rc) {
            LOG_ERR("[N]GattcNrpRdFail[%d]", rc);
        }
    }

    return rc;
}

static int gattc_nrp_write_cb_safe(uint16_t conn_handle,
                                   const struct ble_gatt_error *error,
                                   struct ble_gatt_attr *attr,
                                   void *arg)
{
    struct bt_gatt_write_params *write_params;
    struct bt_conn *conn;
    int rc = 0;

    write_params = arg;
    assert(write_params);
    assert(write_params->func);

    LOG_DBG("[N]GattcNrpWrCb[%u][%04x]", conn_handle, error->status);

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("[N]GattcNrpNotConn[%d]", __LINE__);
        rc = -ENOTCONN;
        goto end;
    }

    /* Shall be invoked before the write_params->func is invoked */
    bt_le_nimble_gatt_nrp_remove(conn, GATTC_NRP_WRITE_REQ, write_params, 0);

    switch (error->status) {
    case 0:
        assert(attr);
        LOG_DBG("[N]GattcNrpHdl[%u][%u]", attr->handle, write_params->handle);

        write_params->func(conn, 0, write_params);
        break;

    case BLE_HS_EDONE:
        /* Note: for sending write request, the EDONE status will not be reported. */
        break;

    default:
        LOG_WRN("[N]GattcNrpStatus[%04x]", error->status);

        if (error->status & BLE_HS_ERR_ATT_BASE) {
            write_params->func(conn, (uint8_t)error->status, write_params);
        }

        rc = error->status;
        break;
    }

end:
    bt_le_host_unlock();

    return rc;
}

static int gattc_nrp_write(struct bt_conn *conn, struct bt_gatt_write_params *params,
                           const uint8_t *data, uint16_t length)
{
    int rc;

    LOG_DBG("[N]GattcNrpWr[%u][%u][%u]",
            conn->handle, params->handle, length);

    rc = ble_gattc_write_flat(conn->handle,
                              params->handle,
                              data,
                              length,
                              gattc_nrp_write_cb_safe,
                              params);
    if (rc) {
        LOG_ERR("[N]GattcNrpWrFlatFail[%d]", rc);
    }

    return rc;
}

static int gattc_nrp_subscribe_cb_safe(uint16_t conn_handle,
                                       const struct ble_gatt_error *error,
                                       struct ble_gatt_attr *attr,
                                       void *arg)
{
    struct bt_gatt_subscribe_params *sub_params;
    struct bt_conn *conn;
    int rc = 0;

    sub_params = arg;
    assert(sub_params);

    LOG_DBG("[N]GattcNrpSubCb[%u][%04x]", conn_handle, error->status);

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("[N]GattcNrpNotConn[%d]", __LINE__);
        rc = -ENOTCONN;
        goto end;
    }

    /* Should be invoked here */
    bt_le_nimble_gatt_nrp_remove(conn, GATTC_NRP_SUBSCRIBE, sub_params, 0);

    switch (error->status) {
    case 0:
        assert(attr);
        LOG_DBG("[N]GattcNrpHdl[%u]", attr->handle);

#if 0
        /* Mask the callback invocation for now.
         * Check the bt_gatt_subscribe function for details.
         */
        if (sub_params->subscribe) {
            sub_params->subscribe(conn, 0, sub_params);
        }
#endif
        break;

    case BLE_HS_EDONE:
        break;

    default:
        LOG_WRN("[N]GattcNrpStatus[%04x]", error->status);

#if 0
        /* Mask the callback invocation for now.
         * Check the bt_gatt_subscribe function for details.
         */
        if (sub_params->subscribe && (error->status & BLE_HS_ERR_ATT_BASE)) {
            sub_params->subscribe(conn, (uint8_t)error->status, sub_params);
        }
#endif

        rc = error->status;
        break;
    }

end:
    bt_le_host_unlock();

    return rc;
}

static int gattc_nrp_subscribe(struct bt_conn *conn, struct bt_gatt_subscribe_params *params)
{
    int rc;

    LOG_DBG("[N]GattcNrpSub[%u][%u][%04x]",
            conn->handle, params->ccc_handle, params->value);

    rc = ble_gattc_write_flat(conn->handle,
                              params->ccc_handle,
                              &params->value,
                              sizeof(params->value),
                              gattc_nrp_subscribe_cb_safe,
                              params);
    if (rc && rc != BLE_HS_ENOTCONN) {
        LOG_ERR("[N]GattcNrpSubFail[%d]", rc);
    }

    return rc;
}

void bt_le_nimble_gatts_nrp_indicate_cb(uint16_t conn_handle,
                                        int16_t attr_handle,
                                        uint8_t status)
{
    struct bt_gatt_indicate_params params = {0};
    struct bt_gatt_attr attr = {0};
    struct bt_conn *conn;

    LOG_DBG("[N]GattsNrpIndCb[%u][%u][%u]", conn_handle, attr_handle, status);

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("[N]GattsNrpNotConn[%d]", __LINE__);
        return;
    }

    switch (status) {
    case 0:
        break;

    case BLE_HS_EDONE:
        break;

    default:
        LOG_WRN("[N]GattsNrpIndCbStatus[%u]", status);
        break;
    }

    if (status) {
        /* EDONE = peer confirmed → success; other non-zero = failure. */
        uint8_t err = (status == BLE_HS_EDONE) ? 0 : status;

        attr.handle = attr_handle;
        params.attr = &attr;

        bt_le_nimble_gatt_nrp_remove(conn, GATTS_NRP_INDICATE, &params, err);
    }
}

static int gatts_nrp_indicate(struct bt_conn *conn, struct bt_gatt_indicate_params *params)
{
    struct notify_data data = {0};
    struct os_mbuf *om = NULL;
    int rc;

    assert(conn && params);
    assert(params->attr);
    /* 0-length indications are valid (spec); ble_hs_mbuf_from_flat accepts
     * (NULL, 0). But (NULL, len>0) would manifest as a misleading -ENOMEM,
     * so catch the programmer error here as a defense-in-depth guard.
     */
    assert(params->len == 0 || params->data != NULL);

    data.attr = params->attr;
    data.handle = bt_gatt_attr_get_handle(data.attr);

    LOG_DBG("[N]GattsNrpInd[%u][%u]", conn->handle, data.handle);

    /* Lookup UUID if it was given */
    if (params->uuid) {
        if (bt_gatts_find_attr_by_uuid(&data, params->uuid) == false) {
            return -ENOENT;
        }

        params->attr = data.attr;
    } else {
        if (data.handle == 0) {
            LOG_WRN("[N]GattsNrpDataHdlZero");
            return -ENOENT;
        }
    }

    /* Check if attribute is a characteristic then adjust the handle */
    if (bt_uuid_cmp(data.attr->uuid, BT_UUID_GATT_CHRC) == 0) {
        struct bt_gatt_chrc *chrc;

        assert(data.attr->user_data);
        chrc = data.attr->user_data;

        if ((chrc->properties & BT_GATT_CHRC_INDICATE) == 0) {
            LOG_WRN("[N]GattsNrpIndNotSupp[%u][%02x]", data.handle, chrc->properties);
            return -EINVAL;
        }

        data.handle = bt_gatt_attr_value_handle(data.attr);

        LOG_INF("[N]GattsNrpDataHdl[%u][%u]", data.attr->handle, data.handle);
    }

    om = ble_hs_mbuf_from_flat(params->data, params->len);
    if (om == NULL) {
        LOG_ERR("[N]GattsNrpNoMemForInd[%u]", params->len);
        return -ENOMEM;
    }

    rc = ble_gatts_indicate_custom(conn->handle, data.handle, om);
    if (rc) {
        LOG_ERR("[N]GattsNrpIndCustomFail[%d]", rc);
    }

    return rc;
}

static struct gatt_nrp *gatt_nrp_add(uint16_t conn_handle)
{
    for (size_t i = 0; i < ARRAY_SIZE(gatt_nrp); i++) {
        if (gatt_nrp[i].conn_handle == UINT16_MAX) {
            gatt_nrp[i].conn_handle = conn_handle;
            sys_slist_init(&gatt_nrp[i].list);

            return &gatt_nrp[i];
        }
    }

    LOG_ERR("[N]GattNrpNoFreeGattNrp[%u]", conn_handle);
    return NULL;
}

static struct gatt_nrp *gatt_nrp_find(uint16_t conn_handle)
{
    for (size_t i = 0; i < ARRAY_SIZE(gatt_nrp); i++) {
        if (gatt_nrp[i].conn_handle == conn_handle) {
            LOG_DBG("[N]GattNrpFound[%u]", conn_handle);
            return &gatt_nrp[i];
        }
    }

    return NULL;
}

static void gatt_nrp_del(struct gatt_nrp *nrp)
{
    struct gatt_nrp_node *nrp_head;
    sys_snode_t *node;

    assert(nrp);

    while (1) {
        node = sys_slist_get(&nrp->list);
        if (node == NULL) {
            break;
        }

        nrp_head = CONTAINER_OF(node, struct gatt_nrp_node, node);

        LOG_DBG("[N]GattNrpFree[%p]", nrp_head);

        if (nrp_head->type == GATTC_NRP_WRITE_REQ) {
            free(nrp_head->write_req.data_copy);
        } else if (nrp_head->type == GATTS_NRP_INDICATE) {
            free(nrp_head->indicate.data_copy);
        }
        free(nrp_head);
    }

    nrp->conn_handle = UINT16_MAX;
}

static int gatt_nrp_insert(struct bt_conn *conn, uint8_t type, void *params)
{
    struct gatt_nrp_node *nrp_node;
    struct gatt_nrp *nrp;
    int rc = 0;

    LOG_DBG("[N]GattNrpInsert[%u][%u]", conn->handle, type);

    nrp = gatt_nrp_find(conn->handle);
    if (nrp == NULL) {
        nrp = gatt_nrp_add(conn->handle);
    }

    if (nrp == NULL) {
        return -ENOMEM;
    }

    nrp_node = calloc(1, sizeof(*nrp_node));
    assert(nrp_node);

    nrp_node->type = type;

    switch (type) {
    case GATTC_NRP_READ_BY_UUID:
        nrp_node->read_by_uuid.params = params;
        break;
    case GATTC_NRP_READ_LONG:
        nrp_node->read_long.params = params;
        break;
    case GATTC_NRP_READ_SINGLE:
        nrp_node->read_single.params = params;
        break;
    case GATTC_NRP_WRITE_REQ: {
        struct bt_gatt_write_params *wp = params;
        nrp_node->write_req.params = wp;
        nrp_node->write_req.data_copy = NULL;
        if (wp->length > 0) {
            assert(wp->data);
            nrp_node->write_req.data_copy = malloc(wp->length);
            if (nrp_node->write_req.data_copy == NULL) {
                LOG_ERR("[N]GattNrpWrDataAllocFail[%u]", wp->length);
                free(nrp_node);
                return -ENOMEM;
            }
            memcpy(nrp_node->write_req.data_copy, wp->data, wp->length);
        }
        break;
    }
    case GATTC_NRP_SUBSCRIBE:
        nrp_node->subscribe.params = params;
        break;
    case GATTS_NRP_INDICATE: {
        struct bt_gatt_indicate_params *ip = params;

        nrp_node->indicate.params_copy = *ip;
        nrp_node->indicate.data_copy = NULL;
        if (ip->len > 0) {
            assert(ip->data);
            nrp_node->indicate.data_copy = malloc(ip->len);
            if (nrp_node->indicate.data_copy == NULL) {
                LOG_ERR("[N]GattNrpIndDataAllocFail[%u]", ip->len);
                free(nrp_node);
                return -ENOMEM;
            }
            memcpy(nrp_node->indicate.data_copy, ip->data, ip->len);
        }
        nrp_node->indicate.params_copy.data = nrp_node->indicate.data_copy;
        nrp_node->indicate.params = &nrp_node->indicate.params_copy;
        break;
    }
    default:
        assert(0);
    }

    if (sys_slist_is_empty(&nrp->list)) {
        LOG_DBG("[N]GattNrpListEmpty");

        if (type == GATTC_NRP_READ_BY_UUID ||
                type == GATTC_NRP_READ_LONG ||
                type == GATTC_NRP_READ_SINGLE) {
            rc = gattc_nrp_read(conn, params);
        } else if (type == GATTC_NRP_WRITE_REQ) {
            rc = gattc_nrp_write(conn, params,
                                 nrp_node->write_req.data_copy,
                                 ((struct bt_gatt_write_params *)params)->length);
        } else if (type == GATTC_NRP_SUBSCRIBE) {
            rc = gattc_nrp_subscribe(conn, params);
        } else if (type == GATTS_NRP_INDICATE) {
            rc = gatts_nrp_indicate(conn, nrp_node->indicate.params);
        }
    } else {
        LOG_DBG("[N]GattNrpListNotEmpty");
        rc = 0;
    }

    if (rc == 0) {
        LOG_DBG("[N]GattNrpAppend[%p]", nrp_node);
        sys_slist_append(&nrp->list, &nrp_node->node);
    } else {
        if (rc != BLE_HS_ENOTCONN) {
            LOG_ERR("[N]GattNrpInsertFail[%d]", rc);
        }
        if (type == GATTC_NRP_WRITE_REQ) {
            free(nrp_node->write_req.data_copy);
        } else if (type == GATTS_NRP_INDICATE) {
            free(nrp_node->indicate.data_copy);
        }
        free(nrp_node);
    }

    return rc;
}

static int gatts_nrp_indicate_insert(struct bt_conn *conn, struct bt_gatt_indicate_params *params)
{
    struct bt_conn *conns;
    uint8_t conns_count;
    int rc = 0;

    LOG_DBG("[N]GattsNrpIndInsert[%p]", conn);

    if (conn) {
        return gatt_nrp_insert(conn, GATTS_NRP_INDICATE, params);
    }

    bt_conn_get_acl_conns(&conns, &conns_count);

    for (size_t i = 0; i < conns_count; i++) {
        LOG_DBG("[N]GattsNrpConn[%u][%u][%u]", i, conns[i].handle, conns[i].state);

        if (conns[i].state == BT_CONN_CONNECTED) {
            rc = gatt_nrp_insert(&conns[i], GATTS_NRP_INDICATE, params);
            if (rc) {
                return rc;
            }
        }
    }

    return 0;
}

int bt_le_nimble_gatt_nrp_insert(struct bt_conn *conn, uint8_t type, void *params)
{
    int rc;
    if (type == GATTS_NRP_INDICATE) {
        rc = gatts_nrp_indicate_insert(conn, params);
    } else {
        rc = gatt_nrp_insert(conn, type, params);
    }

    return nimble_err_to_errno(rc);
}

int bt_le_nimble_gatt_nrp_remove(struct bt_conn *conn, uint8_t type, void *params, uint8_t err)
{
    struct gatt_nrp_node *nrp_head;
    struct gatt_nrp *nrp;
    sys_snode_t *node;
    int rc = 0;

    LOG_DBG("[N]GattNrpRemove[%u][%u]", conn->handle, type);

    nrp = gatt_nrp_find(conn->handle);
    if (nrp == NULL) {
        LOG_ERR("[N]GattNrpNotFound[%u]", conn->handle);
        return -ENODEV;
    }

    /* Fetch and remove the first node from list */
    node = sys_slist_get(&nrp->list);
    assert(node);

    nrp_head = CONTAINER_OF(node, struct gatt_nrp_node, node);
    assert(nrp_head->type == type);

    switch (type) {
    case GATTC_NRP_READ_BY_UUID:
        assert(nrp_head->read_by_uuid.params == params);
        break;
    case GATTC_NRP_READ_LONG:
        assert(nrp_head->read_long.params == params);
        break;
    case GATTC_NRP_READ_SINGLE:
        assert(nrp_head->read_single.params == params);
        break;
    case GATTC_NRP_WRITE_REQ:
        assert(nrp_head->write_req.params == params);
        free(nrp_head->write_req.data_copy);
        nrp_head->write_req.data_copy = NULL;
        break;
    case GATTC_NRP_SUBSCRIBE:
        assert(nrp_head->subscribe.params == params);
        break;
    case GATTS_NRP_INDICATE:
        assert(nrp_head->indicate.params);

        if (nrp_head->indicate.params->attr->handle !=
                ((struct bt_gatt_indicate_params *)params)->attr->handle) {
            LOG_ERR("[N]GattNrpMismatchIndHdl[%u][%u]",
                    ((struct bt_gatt_indicate_params *)params)->attr->handle,
                    nrp_head->indicate.params->attr->handle);
            assert(0);  /* Should not happen */
        }

        /* params is the deep copy taken at insert time; cb sees the copy's
         * address, not the caller's original (which may have been reused).
         * err propagates the mapped NimBLE status (0 on EDONE, else err). */
        nrp_head->indicate.params->func(conn, nrp_head->indicate.params, err);

        free(nrp_head->indicate.data_copy);
        nrp_head->indicate.data_copy = NULL;
        break;
    default:
        assert(0);
    }

    LOG_DBG("[N]GattNrpFree[%p]", nrp_head);
    free(nrp_head);

    while (1) {
        if (sys_slist_is_empty(&nrp->list)) {
            break;
        }

        /* Fetch the first node from list */
        node = sys_slist_peek_head(&nrp->list);

        nrp_head = CONTAINER_OF(node, struct gatt_nrp_node, node);

        if (nrp_head->type == GATTC_NRP_READ_BY_UUID) {
            rc = gattc_nrp_read(conn, nrp_head->read_by_uuid.params);
            if (rc) {
                LOG_ERR("[N]GattcNrpRdByUuidFail[%d]", rc);

                assert(nrp_head->read_by_uuid.params->func);
                nrp_head->read_by_uuid.params->func(conn, rc, nrp_head->read_by_uuid.params, NULL, 0);
            }
        } else if (nrp_head->type == GATTC_NRP_READ_LONG) {
            rc = gattc_nrp_read(conn, nrp_head->read_long.params);
            if (rc) {
                LOG_ERR("[N]GattcNrpRdLongFail[%d]", rc);

                assert(nrp_head->read_long.params->func);
                nrp_head->read_long.params->func(conn, rc, nrp_head->read_long.params, NULL, 0);
            }
        } else if (nrp_head->type == GATTC_NRP_READ_SINGLE) {
            rc = gattc_nrp_read(conn, nrp_head->read_single.params);
            if (rc) {
                LOG_ERR("[N]GattcNrpRdSingleFail[%d]", rc);

                assert(nrp_head->read_single.params->func);
                nrp_head->read_single.params->func(conn, rc, nrp_head->read_single.params, NULL, 0);
            }
        } else if (nrp_head->type == GATTC_NRP_WRITE_REQ) {
            rc = gattc_nrp_write(conn, nrp_head->write_req.params,
                                 nrp_head->write_req.data_copy,
                                 nrp_head->write_req.params->length);
            if (rc) {
                LOG_ERR("[N]GattcNrpWrFail[%d]", rc);

                assert(nrp_head->write_req.params->func);
                nrp_head->write_req.params->func(conn, rc, nrp_head->write_req.params);
            }
        } else if (nrp_head->type == GATTC_NRP_SUBSCRIBE) {
            rc = gattc_nrp_subscribe(conn, nrp_head->subscribe.params);
            if (rc) {
                LOG_ERR("[N]GattcNrpSubFail[%d]", rc);
            }
        } else if (nrp_head->type == GATTS_NRP_INDICATE) {
            rc = gatts_nrp_indicate(conn, nrp_head->indicate.params);
            if (rc) {
                LOG_ERR("[N]GattsNrpIndFail[%d]", rc);

                assert(nrp_head->indicate.params->func);
                nrp_head->indicate.params->func(conn, nrp_head->indicate.params, rc);
            }
        }

        if (rc == 0) {
            LOG_DBG("[N]GattNrpOnGoing[%p]", nrp_head);
            break;
        }

        LOG_DBG("[N]GattNrpRemoveAndFree[%p]", nrp_head);

        /* If this nrp operation failed, then remove it from list,
         * peek the next node and start.
         */
        sys_slist_remove(&nrp->list, NULL, node);

        if (nrp_head->type == GATTC_NRP_WRITE_REQ) {
            free(nrp_head->write_req.data_copy);
        } else if (nrp_head->type == GATTS_NRP_INDICATE) {
            free(nrp_head->indicate.data_copy);
        }
        free(nrp_head);
    }

    return 0;
}

void bt_le_nimble_gatt_nrp_clear(uint16_t conn_handle)
{
    struct gatt_nrp *nrp;

    LOG_DBG("[N]GattNrpClear[%u]", conn_handle);

    nrp = gatt_nrp_find(conn_handle);
    if (nrp == NULL) {
        return;
    }

    gatt_nrp_del(nrp);
}
