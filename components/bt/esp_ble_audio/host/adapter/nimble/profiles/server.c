/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <zephyr/autoconf.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#include <../host/conn_internal.h>

#include "os/os_mbuf.h"
#include "os/os_mempool.h"
#include "host/ble_hs.h"
#include "host/ble_att.h"
#include "host/ble_gatt.h"
#include "host/ble_hs_mbuf.h"

#include "common/host.h"

LOG_MODULE_REGISTER(LEA_GSRV, CONFIG_BT_ISO_LOG_LEVEL);

static ssize_t gatts_read_cb(void *arg, uint16_t offset, const void *data, uint16_t len)
{
    struct os_mbuf *om;
    int rc;

    ARG_UNUSED(offset);

    om = (struct os_mbuf *)arg;
    assert(om);

    LOG_DBG("[N]GattsRdCb[%u][%u]", offset, len);

    if (data == NULL || len == 0) {
        rc = 0;
    } else {
        rc = os_mbuf_append(om, data, len);
        if (rc) {
            LOG_ERR("[N]MbufAppendFail[%d]", rc);
        }
    }

    return (rc == 0 ? len : 0);
}

static int gatts_access_cb(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctx, void *arg)
{
    struct bt_le_nimble_gatt_read_cb cb;
    const struct bt_gatt_attr *attr;
    struct bt_conn *conn;
    uint8_t *data;
    ssize_t rc;

    assert(ctx);

    LOG_DBG("[N]GattsAccessCb[%u][%u][%02x]", conn_handle, attr_handle, ctx->op);

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("[N]NotConn[%d]", __LINE__);
        return -ENOTCONN;
    }

    switch (ctx->op) {
    case BLE_GATT_ACCESS_OP_READ_CHR:
        attr = bt_gatts_find_attr_by_handle(attr_handle);
        if (attr == NULL) {
            LOG_WRN("[N]RdInvHdl[%u]", attr_handle);
            return BT_GATT_ERR(BT_ATT_ERR_INVALID_HANDLE);
        }

        if (attr->read == NULL) {
            LOG_WRN("[N]RdNotPermit");
            return BT_GATT_ERR(BT_ATT_ERR_READ_NOT_PERMITTED);
        }

        cb.read_cb = gatts_read_cb;
        cb.read_arg = ctx->om;

        rc = attr->read(conn, attr, (void *)&cb, UINT16_MAX, 0);
        if (rc < 0) {
            LOG_ERR("[N]RdFail[%u][%d]", attr_handle, rc);
            return BT_GATT_ERR(rc);
        }

        return 0;

    case BLE_GATT_ACCESS_OP_WRITE_CHR:
        attr = bt_gatts_find_attr_by_handle(attr_handle);
        if (attr == NULL) {
            LOG_WRN("[N]WrInvHdl[%u]", attr_handle);
            return BT_GATT_ERR(BT_ATT_ERR_INVALID_HANDLE);
        }

        if (attr->write == NULL) {
            LOG_WRN("[N]WrNotPermit");
            return BT_GATT_ERR(BT_ATT_ERR_WRITE_NOT_PERMITTED);
        }

        if (BT_UUID_16(attr->uuid)->val == BT_UUID_BASS_CONTROL_POINT_VAL) {
            /* Receive Write Long Characteristic for assistant adding source.
             * TODO:
             * Check if the following operation could be directly used?
             * "write(conn, attr, ctx->om->om_data, OS_MBUF_PKTLEN(ctx->om), 0, 0)"
             */
            uint16_t alloc_len = 16 + CONFIG_BT_BAP_BASS_MAX_SUBGROUPS *
                                 (5 + CONFIG_BT_AUDIO_CODEC_CFG_MAX_METADATA_SIZE);

            LOG_DBG("[N]WrBassControlPoint[%u]", alloc_len);

            if (OS_MBUF_PKTLEN(ctx->om) > alloc_len) {
                LOG_ERR("[N]WrBassCtrlPtTooLong[%u > %u]",
                        OS_MBUF_PKTLEN(ctx->om), alloc_len);
                return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
            }

            data = calloc(1, alloc_len);
            assert(data);

            rc = os_mbuf_copydata(ctx->om, 0, OS_MBUF_PKTLEN(ctx->om), data);
            assert(rc == 0);

            rc = attr->write(conn, attr, data, OS_MBUF_PKTLEN(ctx->om), 0, 0);

            free(data);
            data = NULL;
        } else {
            LOG_DBG("[N]Wr[%u]", OS_MBUF_PKTLEN(ctx->om));

            data = calloc(1, OS_MBUF_PKTLEN(ctx->om));
            assert(data);

            rc = os_mbuf_copydata(ctx->om, 0, OS_MBUF_PKTLEN(ctx->om), data);
            assert(rc == 0);

            rc = attr->write(conn, attr, data, OS_MBUF_PKTLEN(ctx->om), 0, 0);

            free(data);
            data = NULL;
        }
        if (rc < 0) {
            LOG_ERR("[N]WrFail[%u][%d]", attr_handle, rc);
            return BT_GATT_ERR(rc);
        }

        return 0;

    case BLE_GATT_ACCESS_OP_READ_DSC:
        return 0;

    case BLE_GATT_ACCESS_OP_WRITE_DSC:
        return 0;

    default:
        return BLE_ATT_ERR_UNLIKELY;
    }
}

int bt_le_nimble_gatts_access_cb_safe(uint16_t conn_handle, uint16_t attr_handle,
                                      struct ble_gatt_access_ctxt *ctx, void *arg)
{
    int err;
    bt_le_host_lock();
    err = gatts_access_cb(conn_handle, attr_handle, ctx, arg);
    bt_le_host_unlock();
    return err;
}
