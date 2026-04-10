/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2020-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include "btc_ble_mesh_ble.h"

#include "mesh/config.h"
#include "mesh/trace.h"
#include "mesh/buf.h"
#include "mesh/uuid.h"
#include "scan.h"
#include "beacon.h"
#include "net.h"
#include "prov_node.h"
#include "proxy_client.h"
#include "proxy_server.h"
#include "prov_pvnr.h"
#include "mesh/adapter.h"

#if CONFIG_BLE_MESH_V11_SUPPORT
#include "mesh_v1.1/utils.h"
#endif /* CONFIG_BLE_MESH_V11_SUPPORT */

/* Scan Window and Interval are equal for continuous scanning */
#define SCAN_INTERVAL   0x20
#define SCAN_WINDOW     0x20

#define PROV_SVC_DATA_LEN                   0x12
#define PROXY_SVC_DATA_LEN_NET_ID           0x09
#define PROXY_SVC_DATA_LEN_NODE_ID          0x11
#define PROXY_SVC_DATA_LEN_PRIVATE_NET_ID   0x11
#define PROXY_SVC_DATA_LEN_PRIVATE_NODE_ID  0x11

static struct bt_mesh_scan_param scan_param = {
#if CONFIG_BLE_MESH_RPR_SRV_ACTIVE_SCAN
    .type       = BLE_MESH_SCAN_ACTIVE,
#else /* CONFIG_BLE_MESH_RPR_SRV_ACTIVE_SCAN */
    .type       = BLE_MESH_SCAN_PASSIVE,
#endif /* CONFIG_BLE_MESH_RPR_SRV_ACTIVE_SCAN */
#if CONFIG_BLE_MESH_USE_DUPLICATE_SCAN
    .filter_dup = BLE_MESH_SCAN_FILTER_DUP_ENABLE,
#else /* CONFIG_BLE_MESH_USE_DUPLICATE_SCAN */
    .filter_dup = BLE_MESH_SCAN_FILTER_DUP_DISABLE,
#endif /* CONFIG_BLE_MESH_USE_DUPLICATE_SCAN */
    .interval   = SCAN_INTERVAL,
    .window     = SCAN_WINDOW,
    .scan_fil_policy = BLE_MESH_SP_ADV_ALL,
};

#if (CONFIG_BLE_MESH_PROVISIONER || CONFIG_BLE_MESH_RPR_SRV)
static const bt_mesh_addr_t *unprov_dev_addr;
static uint8_t current_adv_type;

static struct {
    uint8_t start_idx;
    uint8_t end_idx;
    uint8_t pair_num;
    struct {
        uint8_t uuid[16];
        uint8_t adv_type;
        uint8_t addr[6];
    } info[BLE_MESH_STORE_UNPROV_INFO_MAX_NUM];
} unprov_dev_info_fifo;

int bt_mesh_unprov_dev_fifo_dequeue(uint8_t *uuid, uint8_t *addr)
{
    uint8_t idx = 0;

    BT_DBG("UnprovDevFifoDequeue, PairNum %u StartIdx %u EndIdx %u",
           unprov_dev_info_fifo.pair_num,
           unprov_dev_info_fifo.start_idx,
           unprov_dev_info_fifo.end_idx);

    if (unprov_dev_info_fifo.pair_num == 0) {
        return 0;
    }

    idx = unprov_dev_info_fifo.start_idx;

    if (uuid) {
        memcpy(uuid, unprov_dev_info_fifo.info[idx].uuid, 16);
    }

    if (addr) {
        memcpy(addr, unprov_dev_info_fifo.info[idx].addr, 6);
    }

    idx = (idx + 1) % BLE_MESH_STORE_UNPROV_INFO_MAX_NUM;
    unprov_dev_info_fifo.start_idx = idx;

    unprov_dev_info_fifo.pair_num--;
    return 0;
}

int bt_mesh_unprov_dev_info_query(uint8_t uuid[16], uint8_t addr[6],
                                  uint8_t *adv_type, uint8_t query_type)
{
    uint8_t pair_num = unprov_dev_info_fifo.pair_num;
    uint8_t idx = 0;
    uint8_t cnt = 0;

    BT_DBG("UnprovDevInfoQuery, QueryType 0x%02x PairNum %u", query_type, pair_num);

    if (uuid == NULL && addr == NULL) {
        BT_WARN("No available information to query");
        return -1;
    }

    while (cnt < pair_num) {
        idx = (cnt + unprov_dev_info_fifo.start_idx) % BLE_MESH_STORE_UNPROV_INFO_MAX_NUM;

        BT_DBG("Count %u StartIdx %u Idx %u", cnt, unprov_dev_info_fifo.start_idx, idx);

        if (query_type & BLE_MESH_STORE_UNPROV_INFO_QUERY_TYPE_UUID) {
            if (!memcmp(unprov_dev_info_fifo.info[idx].addr, addr, 6)) {
                if (query_type & BLE_MESH_STORE_UNPROV_INFO_QUERY_TYPE_EXISTS) {
                    return 0;
                }

                memcpy(uuid, unprov_dev_info_fifo.info[idx].uuid, 16);
                *adv_type = unprov_dev_info_fifo.info[idx].adv_type;
                break;
            }
        } else {
            if (!memcmp(unprov_dev_info_fifo.info[idx].uuid, uuid, 16)) {
                if (query_type & BLE_MESH_STORE_UNPROV_INFO_QUERY_TYPE_EXISTS) {
                    return 0;
                }

                memcpy(addr, unprov_dev_info_fifo.info[idx].addr, 6);
                *adv_type = unprov_dev_info_fifo.info[idx].adv_type;
                break;
            }
        }

        cnt++;
    }

    if (cnt == pair_num) {
        BT_DBG("Count == PairNum");
        return -1;
    }

    return 0;
}

int bt_mesh_unprov_dev_fifo_enqueue(uint8_t uuid[16], const uint8_t addr[6], uint8_t adv_type)
{
    uint8_t idx = 0;

    BT_DBG("UnprovDevFifoEnqueue, EndIdx %u PairNum %u",
           unprov_dev_info_fifo.end_idx, unprov_dev_info_fifo.pair_num);

    if (uuid == NULL || addr == NULL) {
        BT_ERR("Invalid argument %s", __func__);
        return -EINVAL;
    }

    if (!bt_mesh_unprov_dev_info_query(uuid, NULL, NULL, BLE_MESH_STORE_UNPROV_INFO_QUERY_TYPE_ADDR |
                                                         BLE_MESH_STORE_UNPROV_INFO_QUERY_TYPE_EXISTS)) {
        return 0;
    }

    if (unprov_dev_info_fifo.pair_num == BLE_MESH_STORE_UNPROV_INFO_MAX_NUM) {
        bt_mesh_unprov_dev_fifo_dequeue(NULL, NULL);
    }

    idx = unprov_dev_info_fifo.end_idx;

    memcpy(unprov_dev_info_fifo.info[idx].uuid, uuid, 16);
    memcpy(unprov_dev_info_fifo.info[idx].addr, addr, 6);
    unprov_dev_info_fifo.info[idx].adv_type = adv_type;

    idx = (idx + 1) % BLE_MESH_STORE_UNPROV_INFO_MAX_NUM;
    unprov_dev_info_fifo.end_idx = idx;
    unprov_dev_info_fifo.pair_num++;

    BT_DBG("EndIdx %u PairNum %u", unprov_dev_info_fifo.end_idx, unprov_dev_info_fifo.pair_num);

    return 0;
}

const bt_mesh_addr_t *bt_mesh_get_unprov_dev_addr(void)
{
    return unprov_dev_addr;
}

uint8_t bt_mesh_get_adv_type(void)
{
    BT_DBG("CurrentAdvType %u", current_adv_type);

    return current_adv_type;
}
#endif /* (CONFIG_BLE_MESH_PROVISIONER || CONFIG_BLE_MESH_RPR_SRV) */

#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_CLIENT || \
     CONFIG_BLE_MESH_PROXY_SOLIC_PDU_RX || \
    (CONFIG_BLE_MESH_RPR_SRV && CONFIG_BLE_MESH_PB_GATT)
static bool adv_flags_valid(struct net_buf_simple *buf)
{
    uint8_t flags = 0U;

    BT_DBG("IsAdvFlagsValid");

    if (buf->len != 1U) {
        BT_DBG("Unexpected adv flags length %d", buf->len);
        return false;
    }

    flags = net_buf_simple_pull_u8(buf);

    BT_DBG("Flags 0x%02x", flags);

    /* Flags context will not be checked currently */
    ARG_UNUSED(flags);

    return true;
}

static bool adv_service_uuid_valid(struct net_buf_simple *buf, uint16_t *uuid)
{
    BT_DBG("IsAdvServiceUUIDValid");

    if (buf->len != 2U) {
        BT_DBG("Length not match mesh service uuid");
        return false;
    }

    *uuid = net_buf_simple_pull_le16(buf);

    BT_DBG("UUID 0x%04x", *uuid);

    if (*uuid != BLE_MESH_UUID_MESH_PROV_VAL &&
        *uuid != BLE_MESH_UUID_MESH_PROXY_VAL &&
        *uuid != BLE_MESH_UUID_MESH_PROXY_SOLIC_VAL) {
        BT_DBG("UnexpectMeshUUID");
        return false;
    }

    /* In remote provisioning, Node could handle unprovisioned device beacon.
     * CASE: MESH/SR/RPR/SCN/BV-01-C
     */
#if CONFIG_BLE_MESH_RPR_SRV
    if (*uuid == BLE_MESH_UUID_MESH_PROV_VAL &&
        !IS_ENABLED(CONFIG_BLE_MESH_PB_GATT)) {
        BT_DBG("IgnorePBGattUUID");
        return false;
    }
#else /* CONFIG_BLE_MESH_RPR_SRV */
    if (*uuid == BLE_MESH_UUID_MESH_PROV_VAL &&
        (bt_mesh_is_provisioner_en() == false ||
        !IS_ENABLED(CONFIG_BLE_MESH_PB_GATT))) {
        BT_DBG("IgnorePBGattUUID");
        return false;
    }
#endif /* CONFIG_BLE_MESH_RPR_SRV */

    if (*uuid == BLE_MESH_UUID_MESH_PROXY_VAL &&
        !IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_CLIENT)) {
        BT_DBG("IgnoreProxyUUID");
        return false;
    }

    if (*uuid == BLE_MESH_UUID_MESH_PROXY_SOLIC_VAL &&
        !IS_ENABLED(CONFIG_BLE_MESH_PROXY_SOLIC_PDU_RX)) {
        BT_DBG("IgnoreProxySolicUUID");
        return false;
    }

    return true;
}

static void handle_adv_service_data(struct net_buf_simple *buf,
                                    const bt_mesh_addr_t *addr,
                                    uint16_t uuid, int8_t rssi)
{
    uint16_t type = 0U;

    BT_DBG("HandleAdvServiceData, UUID 0x%04x", uuid);

    if (!buf || !addr) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    type = net_buf_simple_pull_le16(buf);
    if (type != uuid) {
        BT_DBG("UnexpectMeshUUID 0x%04x", type);
        return;
    }

    switch (type) {
#if (CONFIG_BLE_MESH_PROVISIONER || CONFIG_BLE_MESH_RPR_SRV) && \
     CONFIG_BLE_MESH_PB_GATT
    case BLE_MESH_UUID_MESH_PROV_VAL: {
        struct net_buf_simple_state state = {0};

        net_buf_simple_save(buf, &state);

#if CONFIG_BLE_MESH_PROVISIONER
        if (bt_mesh_is_provisioner_en()) {
            if (buf->len != PROV_SVC_DATA_LEN) {
                BT_WARN("Invalid Mesh Prov Service Data length %d", buf->len);
                return;
            }

            BT_DBG("Start to handle Mesh Prov Service Data");
            bt_mesh_provisioner_prov_adv_recv(buf, addr, rssi);
        }
#endif /* CONFIG_BLE_MESH_PROVISIONER */

        net_buf_simple_restore(buf, &state);

#if CONFIG_BLE_MESH_RPR_SRV
        if (bt_mesh_is_provisioned()) {
            const bt_mesh_addr_t *addr = NULL;

            if (buf->len != PROV_SVC_DATA_LEN) {
                BT_WARN("Invalid Mesh Prov Service Data length %d", buf->len);
                return;
            }

            addr = bt_mesh_get_unprov_dev_addr();
            assert(addr);

            bt_mesh_unprov_dev_fifo_enqueue(buf->data, addr->val, bt_mesh_get_adv_type());

            bt_mesh_rpr_srv_unprov_beacon_recv(buf, bt_mesh_get_adv_type(), addr, rssi);
        }
#endif /* CONFIG_BLE_MESH_RPR_SRV */

        break;
    }
#endif /* (CONFIG_BLE_MESH_PROVISIONER || CONFIG_BLE_MESH_RPR_SRV) &&
           CONFIG_BLE_MESH_PB_GATT */

#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
    case BLE_MESH_UUID_MESH_PROXY_VAL: {
        if (buf->len != PROXY_SVC_DATA_LEN_NET_ID &&
            buf->len != PROXY_SVC_DATA_LEN_NODE_ID) {
            /* PROXY_SVC_DATA_LEN_NODE_ID,
             * PROXY_SVC_DATA_LEN_PRIVATE_NET_ID and
             * PROXY_SVC_DATA_LEN_PRIVATE_NODE_ID are equal to 0x11
             */
            BT_WARN("Invalid Mesh Proxy Service Data length %d", buf->len);
            return;
        }

        BT_DBG("Start to handle Mesh Proxy Service Data");
        bt_mesh_proxy_client_gatt_adv_recv(buf, addr, rssi);
        break;
    }
#endif /* CONFIG_BLE_MESH_GATT_PROXY_CLIENT */

#if CONFIG_BLE_MESH_PROXY_SOLIC_PDU_RX
    case BLE_MESH_UUID_MESH_PROXY_SOLIC_VAL: {
        if (buf->len != (1 + BLE_MESH_NET_HDR_LEN + 8)) {
            BT_WARN("Invalid Mesh Proxy Solic Service Data length %d", buf->len);
            return;
        }

        BT_DBG("Start to handle Mesh Proxy Solic Service Data");
        bt_mesh_proxy_server_solic_recv(buf, addr, rssi);
        break;
    }
#endif /* CONFIG_BLE_MESH_PROXY_SOLIC_PDU_RX */

    default:
        break;
    }
}
#endif /* (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
           CONFIG_BLE_MESH_GATT_PROXY_CLIENT  || \
           CONFIG_BLE_MESH_PROXY_SOLIC_PDU_RX || \
           (CONFIG_BLE_MESH_RPR_SRV && CONFIG_BLE_MESH_PB_GATT) */

#if CONFIG_BLE_MESH_SUPPORT_BLE_SCAN
static bool ble_scan_en;

int bt_mesh_start_ble_scan(struct bt_mesh_ble_scan_param *param)
{
    BT_DBG("StartBLEScan");

    if (ble_scan_en == true) {
        BT_WARN("%s, Already", __func__);
        return -EALREADY;
    }

    ble_scan_en = true;

    return 0;
}

int bt_mesh_stop_ble_scan(void)
{
    BT_DBG("StopBLEScan");

    if (ble_scan_en == false) {
        BT_WARN("%s, Already", __func__);
        return -EALREADY;
    }

    ble_scan_en = false;

    return 0;
}

bool bt_mesh_ble_scan_state_get(void)
{
    BT_DBG("BLEScanEn %u", ble_scan_en);

    return ble_scan_en;
}

static void inline callback_ble_adv_pkt(const bt_mesh_addr_t *addr,
                                        uint8_t adv_type, uint8_t data[],
                                        uint16_t length, int8_t rssi)
{
#if !CONFIG_BLE_MESH_USE_BLE_50
    bt_mesh_ble_adv_report_t adv_rpt = {0};

    if (ble_scan_en) {
        memcpy(adv_rpt.addr, addr->val, BD_ADDR_LEN);
        adv_rpt.addr_type = addr->type;
        adv_rpt.adv_type = adv_type;
        adv_rpt.length = length;
        adv_rpt.data = length ? data : NULL;
        adv_rpt.rssi = rssi;

        BT_DBG("CallbackBLEAdvPkt, AdvType 0x%02x Len %u", adv_type, length);

        bt_mesh_ble_scan_cb_evt_to_btc(&adv_rpt);
    }
#endif /* !CONFIG_BLE_MESH_USE_BLE_50 */
}
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_SCAN */

#if CONFIG_BLE_MESH_RPR_SRV
static bool rpr_ext_scan_handle_adv_pkt(const bt_mesh_addr_t *addr,
                                        uint8_t data[], uint16_t length)
{
    struct net_buf_simple buf = {0};
    bool rpr_adv = false;

    BT_DBG("RPRExtScanHandleAdvPkt, Provisioned %u", bt_mesh_is_provisioned());

    if (bt_mesh_is_provisioned() == false) {
        return false;
    }

    net_buf_simple_init_with_data(&buf, data, length);
    bt_mesh_rpr_srv_extended_scan(&buf, addr, &rpr_adv);

    return rpr_adv;
}
#endif /* CONFIG_BLE_MESH_RPR_SRV */

static void bt_mesh_scan_cb(struct bt_mesh_adv_report *adv_rpt)
{
    struct net_buf_simple *buf = &adv_rpt->adv_data;
    struct net_buf_simple_state buf_state = {0};

#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_CLIENT || \
     CONFIG_BLE_MESH_PROXY_SOLIC_PDU_RX || \
    (CONFIG_BLE_MESH_RPR_SRV && CONFIG_BLE_MESH_PB_GATT)
    uint16_t uuid = 0U;
#endif
#if (CONFIG_BLE_MESH_RPR_SRV || CONFIG_BLE_MESH_SUPPORT_BLE_SCAN)
    uint8_t *adv_data = buf->data;
    uint16_t adv_len = buf->len;
#endif /* (CONFIG_BLE_MESH_RPR_SRV || CONFIG_BLE_MESH_SUPPORT_BLE_SCAN) */

    net_buf_simple_save(buf, &buf_state);

    if (
#if CONFIG_BLE_MESH_LONG_PACKET
        adv_rpt->adv_type != 0 &&
#endif
        adv_rpt->adv_type != BLE_MESH_ADV_NONCONN_IND &&
        adv_rpt->adv_type != BLE_MESH_ADV_IND
#if CONFIG_BLE_MESH_EXT_ADV
        && adv_rpt->adv_type != BLE_MESH_EXT_ADV_NONCONN_IND
#endif
#if CONFIG_BLE_MESH_RPR_SRV && CONFIG_BLE_MESH_RPR_SRV_ACTIVE_SCAN
        && adv_rpt->adv_type != BLE_MESH_ADV_SCAN_RSP
#endif /* CONFIG_BLE_MESH_RPR_SRV && CONFIG_BLE_MESH_RPR_SRV_ACTIVE_SCAN */
        ) {
#if CONFIG_BLE_MESH_SUPPORT_BLE_SCAN
        callback_ble_adv_pkt(&adv_rpt->addr, adv_rpt->adv_type, adv_data, adv_len, adv_rpt->rssi);
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_SCAN */

        net_buf_simple_restore(buf, &buf_state);
        return;
    }

    BT_DBG("MeshScan");
    BT_DBG("Len %u: %s", buf->len, bt_hex(buf->data, buf->len));

#if (CONFIG_BLE_MESH_PROVISIONER || CONFIG_BLE_MESH_RPR_SRV)
    unprov_dev_addr = &adv_rpt->addr;
    current_adv_type = adv_rpt->adv_type;
#endif /* (CONFIG_BLE_MESH_PROVISIONER || CONFIG_BLE_MESH_RPR_SRV) */

#if CONFIG_BLE_MESH_RPR_SRV && CONFIG_BLE_MESH_RPR_SRV_ACTIVE_SCAN
    if (adv_rpt->adv_type == BLE_MESH_ADV_SCAN_RSP) {
        /* scan response is only visible for remote provisioning extend scan */
        if (rpr_ext_scan_handle_adv_pkt(&adv_rpt->addr, adv_data, adv_len)) {
            return;
        }

#if CONFIG_BLE_MESH_SUPPORT_BLE_SCAN
        callback_ble_adv_pkt(&adv_rpt->addr, adv_rpt->adv_type, adv_data, adv_len, adv_rpt->rssi);
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_SCAN */

        net_buf_simple_restore(buf, &buf_state);
    }
#endif /* CONFIG_BLE_MESH_RPR_SRV && CONFIG_BLE_MESH_RPR_SRV_ACTIVE_SCAN */

    while (buf->len > 1) {
        struct net_buf_simple_state state;
        uint8_t len, type;

        len = net_buf_simple_pull_u8(buf);

        /* Check for early termination */
        if (len == 0U) {
#if CONFIG_BLE_MESH_SUPPORT_BLE_SCAN
            callback_ble_adv_pkt(&adv_rpt->addr, adv_rpt->adv_type, adv_data, adv_len, adv_rpt->rssi);
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_SCAN */

            net_buf_simple_restore(buf, &buf_state);
            return;
        }

        if (len > buf->len) {
            BT_DBG("MalformedAD");

#if CONFIG_BLE_MESH_SUPPORT_BLE_SCAN
            callback_ble_adv_pkt(&adv_rpt->addr, adv_rpt->adv_type, adv_data, adv_len, adv_rpt->rssi);
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_SCAN */

            net_buf_simple_restore(buf, &buf_state);
            return;
        }

        net_buf_simple_save(buf, &state);

        type = net_buf_simple_pull_u8(buf);

        buf->len = len - 1;

        if ((type == BLE_MESH_DATA_MESH_PROV ||
            type == BLE_MESH_DATA_MESH_MESSAGE ||
            type == BLE_MESH_DATA_MESH_BEACON) &&
            (adv_rpt->adv_type != BLE_MESH_ADV_NONCONN_IND
#if CONFIG_BLE_MESH_EXT_ADV
            && adv_rpt->adv_type != BLE_MESH_EXT_ADV_NONCONN_IND
#endif
        )) {
            BT_DBG("IgnorePkt, Type 0x%02x AdvType 0x%02x", type, adv_rpt->adv_type);
            return;
        }

        switch (type) {
        case BLE_MESH_DATA_MESH_MESSAGE:
            struct bt_mesh_net_rx rx = {
                .ctx.recv_rssi = adv_rpt->rssi,
            };
#if CONFIG_BLE_MESH_EXT_ADV
            if (adv_rpt->adv_type == BLE_MESH_EXT_ADV_NONCONN_IND) {
                rx.ctx.enh.ext_adv_cfg_used = true;
                rx.ctx.enh.ext_adv_cfg.primary_phy = adv_rpt->primary_phy;
                rx.ctx.enh.ext_adv_cfg.secondary_phy = adv_rpt->secondary_phy;
                if (adv_rpt->tx_power == 0x7f) {
                    rx.ctx.enh.ext_adv_cfg.include_tx_power = false;
                } else {
                    rx.ctx.enh.ext_adv_cfg.include_tx_power = true;
                }
                rx.ctx.enh.ext_adv_cfg.tx_power = adv_rpt->tx_power;
            }
#endif  /* CONFIG_BLE_MESH_EXT_ADV */
            bt_mesh_generic_net_recv(buf, &rx, BLE_MESH_NET_IF_ADV);
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
            bt_mesh_beacon_recv(buf, adv_rpt->rssi);
            break;

#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_CLIENT || \
     CONFIG_BLE_MESH_PROXY_SOLIC_PDU_RX || \
    (CONFIG_BLE_MESH_RPR_SRV && CONFIG_BLE_MESH_PB_GATT)
        case BLE_MESH_DATA_FLAGS:
            if (!adv_flags_valid(buf)) {
                BT_DBG("Adv Flags mismatch, ignore this adv pkt");

#if CONFIG_BLE_MESH_SUPPORT_BLE_SCAN
                callback_ble_adv_pkt(&adv_rpt->addr, adv_rpt->adv_type, adv_data, adv_len, adv_rpt->rssi);
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_SCAN */

                net_buf_simple_restore(buf, &buf_state);
                return;
            }
            break;

        case BLE_MESH_DATA_UUID16_ALL:
            if (!adv_service_uuid_valid(buf, &uuid)) {
                BT_DBG("Adv Service UUID mismatch, ignore this adv pkt");

#if CONFIG_BLE_MESH_RPR_SRV
                if (rpr_ext_scan_handle_adv_pkt(&adv_rpt->addr, adv_data, adv_len)) {
                    /* If handled as extended scan report successfully, then not
                     * notify to the application layer as normal BLE adv packet.
                     */
                    return;
                }
#endif /* CONFIG_BLE_MESH_RPR_SRV */

#if CONFIG_BLE_MESH_SUPPORT_BLE_SCAN
                callback_ble_adv_pkt(&adv_rpt->addr, adv_rpt->adv_type, adv_data, adv_len, adv_rpt->rssi);
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_SCAN */

                net_buf_simple_restore(buf, &buf_state);
                return;
            }
            break;

        case BLE_MESH_DATA_SVC_DATA16:
            handle_adv_service_data(buf, &adv_rpt->addr, uuid, adv_rpt->rssi);
            break;
#endif /* (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
           CONFIG_BLE_MESH_GATT_PROXY_CLIENT || \
           CONFIG_BLE_MESH_PROXY_SOLIC_PDU_RX || \
          (CONFIG_BLE_MESH_RPR_SRV && CONFIG_BLE_MESH_PB_GATT) */

        default:
#if CONFIG_BLE_MESH_RPR_SRV
            if (rpr_ext_scan_handle_adv_pkt(&adv_rpt->addr, adv_data, adv_len)) {
                /* If handled as extended scan report successfully, then not
                 * notify to the application layer as normal BLE adv packet.
                 */
                return;
            }
#endif /* CONFIG_BLE_MESH_RPR_SRV */

#if CONFIG_BLE_MESH_SUPPORT_BLE_SCAN
            callback_ble_adv_pkt(&adv_rpt->addr, adv_rpt->adv_type, adv_data, adv_len, adv_rpt->rssi);
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_SCAN */

            net_buf_simple_restore(buf, &buf_state);
            return;
        }

        net_buf_simple_restore(buf, &state);
        net_buf_simple_pull(buf, len);
    }
}

int bt_mesh_scan_enable(void)
{
    int err = 0;

    BT_DBG("ScanEnable");

    err = bt_le_scan_start(&scan_param, bt_mesh_scan_cb);
    if (err && err != -EALREADY) {
        BT_ERR("StartScanFailed, Err %d", err);
        return err;
    }

    return 0;
}

int bt_mesh_scan_disable(void)
{
    int err = 0;

    BT_DBG("ScanDisable");

    err = bt_le_scan_stop();
    if (err && err != -EALREADY) {
        BT_ERR("StopScanFailed, Err %d", err);
        return err;
    }

    return 0;
}

int bt_mesh_scan_param_update(struct bt_mesh_scan_param *param)
{
    int err = 0;

    if (param == NULL ||
        param->interval == 0 ||
        param->interval < param->window) {
        BT_ERR("InvalidScanParam");
        return -EINVAL;
    }

    BT_DBG("ScanParamUpdate, Type %u Interval %u Window %u",
           param->type, param->interval, param->window);

    scan_param.interval = param->interval;
    scan_param.window = param->window;

    err = bt_le_scan_stop();
    if (err) {
        if (err == -EALREADY) {
            BT_INFO("New scan parameters will take effect after scan starts");
            return 0;
        }

        BT_ERR("StopScanFailed, Err %d", err);
        return err;
    }

    /* Since the user only needs to set the scan interval and scan window,
     * only the interval and window parameters in the `param` are correct.
     *
     * For the aforementioned reason, when updating the scan parameters,
     * the other parameters also need to be set correctly, and these other
     * parameters are saved in the `scan_param`. Therefore, `scan_param`
     * must be used instead of `param` here.
     */
    err = bt_le_scan_start(&scan_param, bt_mesh_scan_cb);
    if (err && err != -EALREADY) {
        BT_ERR("StartScanFailed, Err %d", err);
        return err;
    }

    return 0;
}

#if CONFIG_BLE_MESH_TEST_USE_WHITE_LIST
int bt_mesh_scan_with_wl_enable(void)
{
    struct bt_mesh_scan_param scan_param = {
        .type       = BLE_MESH_SCAN_PASSIVE,
#if CONFIG_BLE_MESH_USE_DUPLICATE_SCAN
        .filter_dup = BLE_MESH_SCAN_FILTER_DUP_ENABLE,
#else /* CONFIG_BLE_MESH_USE_DUPLICATE_SCAN */
        .filter_dup = BLE_MESH_SCAN_FILTER_DUP_DISABLE,
#endif /* CONFIG_BLE_MESH_USE_DUPLICATE_SCAN */
        .interval   = SCAN_INTERVAL,
        .window     = SCAN_WINDOW,
        .scan_fil_policy = BLE_MESH_SP_ADV_WL,
    };
    int err = 0;

    BT_DBG("ScanWithWLEnable");

    err = bt_le_scan_start(&scan_param, bt_mesh_scan_cb);
    if (err && err != -EALREADY) {
        BT_ERR("StartScanFailed, Err %d", err);
        return err;
    }

    return 0;
}
#endif /* CONFIG_BLE_MESH_TEST_USE_WHITE_LIST */
