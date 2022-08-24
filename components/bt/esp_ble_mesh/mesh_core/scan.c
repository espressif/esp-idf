/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include "btc_ble_mesh_ble.h"

#include "mesh_config.h"
#include "mesh_trace.h"
#include "mesh_buf.h"
#include "mesh_uuid.h"
#include "scan.h"
#include "beacon.h"
#include "net.h"
#include "prov.h"
#include "proxy_client.h"
#include "proxy_server.h"
#include "provisioner_prov.h"
#include "mesh_bearer_adapt.h"

/* Scan Window and Interval are equal for continuous scanning */
#define SCAN_INTERVAL   0x20
#define SCAN_WINDOW     0x20

#define PROV_SVC_DATA_LEN           0x12
#define PROXY_SVC_DATA_LEN_NET_ID   0x09
#define PROXY_SVC_DATA_LEN_NODE_ID  0x11

#if CONFIG_BLE_MESH_PROVISIONER
static const bt_mesh_addr_t *unprov_dev_addr;

const bt_mesh_addr_t *bt_mesh_get_unprov_dev_addr(void)
{
    return unprov_dev_addr;
}
#endif /* CONFIG_BLE_MESH_PROVISIONER */

#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_CLIENT
static bool adv_flags_valid(struct net_buf_simple *buf)
{
    uint8_t flags = 0U;

    if (buf->len != 1U) {
        BT_DBG("Unexpected adv flags length %d", buf->len);
        return false;
    }

    flags = net_buf_simple_pull_u8(buf);

    BT_DBG("Received adv pkt with flags: 0x%02x", flags);

    /* Flags context will not be checked currently */
    ARG_UNUSED(flags);

    return true;
}

static bool adv_service_uuid_valid(struct net_buf_simple *buf, uint16_t *uuid)
{
    if (buf->len != 2U) {
        BT_DBG("Length not match mesh service uuid");
        return false;
    }

    *uuid = net_buf_simple_pull_le16(buf);

    BT_DBG("Received adv pkt with service UUID: %d", *uuid);

    if (*uuid != BLE_MESH_UUID_MESH_PROV_VAL &&
        *uuid != BLE_MESH_UUID_MESH_PROXY_VAL) {
        return false;
    }

    if (*uuid == BLE_MESH_UUID_MESH_PROV_VAL &&
        bt_mesh_is_provisioner_en() == false) {
        return false;
    }

    if (*uuid == BLE_MESH_UUID_MESH_PROXY_VAL &&
        !IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_CLIENT)) {
        return false;
    }

    return true;
}

static void handle_adv_service_data(struct net_buf_simple *buf,
                                    const bt_mesh_addr_t *addr,
                                    uint16_t uuid, int8_t rssi)
{
    uint16_t type = 0U;

    if (!buf || !addr) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    type = net_buf_simple_pull_le16(buf);
    if (type != uuid) {
        BT_DBG("Invalid Mesh Service Data UUID 0x%04x", type);
        return;
    }

    switch (type) {
#if CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT
    case BLE_MESH_UUID_MESH_PROV_VAL:
        if (bt_mesh_is_provisioner_en()) {
            if (buf->len != PROV_SVC_DATA_LEN) {
                BT_WARN("Invalid Mesh Prov Service Data length %d", buf->len);
                return;
            }

            BT_DBG("Start to handle Mesh Prov Service Data");
            bt_mesh_provisioner_prov_adv_recv(buf, addr, rssi);
        }
        break;
#endif
#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
    case BLE_MESH_UUID_MESH_PROXY_VAL:
        if (buf->len != PROXY_SVC_DATA_LEN_NET_ID &&
            buf->len != PROXY_SVC_DATA_LEN_NODE_ID) {
            BT_WARN("Invalid Mesh Proxy Service Data length %d", buf->len);
            return;
        }

        BT_DBG("Start to handle Mesh Proxy Service Data");
        bt_mesh_proxy_client_gatt_adv_recv(buf, addr, rssi);
        break;
#endif
    default:
        break;
    }
}
#endif /* (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
           CONFIG_BLE_MESH_GATT_PROXY_CLIENT */

#if CONFIG_BLE_MESH_SUPPORT_BLE_SCAN
static bool ble_scan_en;

int bt_mesh_start_ble_scan(struct bt_mesh_ble_scan_param *param)
{
    if (ble_scan_en == true) {
        BT_WARN("%s, Already", __func__);
        return -EALREADY;
    }

    ble_scan_en = true;

    return 0;
}

int bt_mesh_stop_ble_scan(void)
{
    if (ble_scan_en == false) {
        BT_WARN("%s, Already", __func__);
        return -EALREADY;
    }

    ble_scan_en = false;

    return 0;
}

static void inline callback_ble_adv_pkt(const bt_mesh_addr_t *addr,
                                        uint8_t adv_type, uint8_t data[],
                                        uint16_t length, int8_t rssi)
{
    if (ble_scan_en) {
        bt_mesh_ble_scan_cb_evt_to_btc(addr, adv_type, data, length, rssi);
    }
}
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_SCAN */

static void bt_mesh_scan_cb(const bt_mesh_addr_t *addr,
                            int8_t rssi, uint8_t adv_type,
                            struct net_buf_simple *buf)
{
#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_CLIENT
    uint16_t uuid = 0U;
#endif
#if CONFIG_BLE_MESH_SUPPORT_BLE_SCAN
    uint8_t *adv_data = buf->data;
    uint16_t adv_len = buf->len;
#endif

    if (adv_type != BLE_MESH_ADV_NONCONN_IND && adv_type != BLE_MESH_ADV_IND) {
#if CONFIG_BLE_MESH_SUPPORT_BLE_SCAN
        callback_ble_adv_pkt(addr, adv_type, adv_data, adv_len, rssi);
#endif
        return;
    }

    BT_DBG("scan, len %u: %s", buf->len, bt_hex(buf->data, buf->len));

#if CONFIG_BLE_MESH_PROVISIONER
    unprov_dev_addr = addr;
#endif

    while (buf->len > 1) {
        struct net_buf_simple_state state;
        uint8_t len, type;

        len = net_buf_simple_pull_u8(buf);
        /* Check for early termination */
        if (len == 0U) {
#if CONFIG_BLE_MESH_SUPPORT_BLE_SCAN
            callback_ble_adv_pkt(addr, adv_type, adv_data, adv_len, rssi);
#endif
            return;
        }

        if (len > buf->len) {
            BT_DBG("AD malformed");
#if CONFIG_BLE_MESH_SUPPORT_BLE_SCAN
            callback_ble_adv_pkt(addr, adv_type, adv_data, adv_len, rssi);
#endif
            return;
        }

        net_buf_simple_save(buf, &state);

        type = net_buf_simple_pull_u8(buf);

        buf->len = len - 1;

        if ((type == BLE_MESH_DATA_MESH_PROV || type == BLE_MESH_DATA_MESH_MESSAGE ||
            type == BLE_MESH_DATA_MESH_BEACON) && (adv_type != BLE_MESH_ADV_NONCONN_IND)) {
            BT_DBG("Ignore mesh packet (type 0x%02x) with adv_type 0x%02x", type, adv_type);
            return;
        }

        switch (type) {
        case BLE_MESH_DATA_MESH_MESSAGE:
            bt_mesh_net_recv(buf, rssi, BLE_MESH_NET_IF_ADV);
            break;
#if CONFIG_BLE_MESH_PB_ADV
        case BLE_MESH_DATA_MESH_PROV:
            if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_node()) {
                bt_mesh_pb_adv_recv(buf);
            }
            if (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) && bt_mesh_is_provisioner_en()) {
                bt_mesh_provisioner_pb_adv_recv(buf);
            }
            break;
#endif /* CONFIG_BLE_MESH_PB_ADV */
        case BLE_MESH_DATA_MESH_BEACON:
            bt_mesh_beacon_recv(buf, rssi);
            break;
#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_CLIENT
        case BLE_MESH_DATA_FLAGS:
            if (!adv_flags_valid(buf)) {
                BT_DBG("Adv Flags mismatch, ignore this adv pkt");
#if CONFIG_BLE_MESH_SUPPORT_BLE_SCAN
                callback_ble_adv_pkt(addr, adv_type, adv_data, adv_len, rssi);
#endif
                return;
            }
            break;
        case BLE_MESH_DATA_UUID16_ALL:
            if (!adv_service_uuid_valid(buf, &uuid)) {
                BT_DBG("Adv Service UUID mismatch, ignore this adv pkt");
#if CONFIG_BLE_MESH_SUPPORT_BLE_SCAN
                callback_ble_adv_pkt(addr, adv_type, adv_data, adv_len, rssi);
#endif
                return;
            }
            break;
        case BLE_MESH_DATA_SVC_DATA16:
            handle_adv_service_data(buf, addr, uuid, rssi);
            break;
#endif
        default:
#if CONFIG_BLE_MESH_SUPPORT_BLE_SCAN
            callback_ble_adv_pkt(addr, adv_type, adv_data, adv_len, rssi);
#endif
            return;
        }

        net_buf_simple_restore(buf, &state);
        net_buf_simple_pull(buf, len);
    }
}

int bt_mesh_scan_enable(void)
{
    int err = 0;

    struct bt_mesh_scan_param scan_param = {
        .type       = BLE_MESH_SCAN_PASSIVE,
#if CONFIG_BLE_MESH_USE_DUPLICATE_SCAN
        .filter_dup = BLE_MESH_SCAN_FILTER_DUP_ENABLE,
#else
        .filter_dup = BLE_MESH_SCAN_FILTER_DUP_DISABLE,
#endif
        .interval   = SCAN_INTERVAL,
        .window     = SCAN_WINDOW,
        .scan_fil_policy = BLE_MESH_SP_ADV_ALL,
    };

    BT_DBG("%s", __func__);

    err = bt_le_scan_start(&scan_param, bt_mesh_scan_cb);
    if (err && err != -EALREADY) {
        BT_ERR("starting scan failed (err %d)", err);
        return err;
    }

    return 0;
}

int bt_mesh_scan_disable(void)
{
    int err = 0;

    BT_DBG("%s", __func__);

    err = bt_le_scan_stop();
    if (err && err != -EALREADY) {
        BT_ERR("stopping scan failed (err %d)", err);
        return err;
    }

    return 0;
}

#if CONFIG_BLE_MESH_TEST_USE_WHITE_LIST
int bt_mesh_scan_with_wl_enable(void)
{
    int err = 0;

    struct bt_mesh_scan_param scan_param = {
        .type       = BLE_MESH_SCAN_PASSIVE,
#if CONFIG_BLE_MESH_USE_DUPLICATE_SCAN
        .filter_dup = BLE_MESH_SCAN_FILTER_DUP_ENABLE,
#else
        .filter_dup = BLE_MESH_SCAN_FILTER_DUP_DISABLE,
#endif
        .interval   = SCAN_INTERVAL,
        .window     = SCAN_WINDOW,
        .scan_fil_policy = BLE_MESH_SP_ADV_WL,
    };

    BT_DBG("%s", __func__);

    err = bt_le_scan_start(&scan_param, bt_mesh_scan_cb);
    if (err && err != -EALREADY) {
        BT_ERR("starting scan failed (err %d)", err);
        return err;
    }

    return 0;
}
#endif /* CONFIG_BLE_MESH_TEST_USE_WHITE_LIST */
