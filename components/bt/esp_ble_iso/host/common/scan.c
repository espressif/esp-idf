/*
 * SPDX-FileCopyrightText: 2017-2021 Nordic Semiconductor
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/iso.h>
#include <zephyr/bluetooth/common/bt_str.h>

#include <../host/hci_core.h>

#include "common/host.h"

static sys_slist_t scan_cbs = SYS_SLIST_STATIC_INIT(&scan_cbs);

_LIB_ONLY
int bt_le_scan_cb_register(struct bt_le_scan_cb *cb)
{
    LOG_DBG("ScanCbReg");

    if (cb == NULL) {
        LOG_ERR("ScanCbNull");
        return -EINVAL;
    }

    if (sys_slist_find(&scan_cbs, &cb->node, NULL)) {
        LOG_WRN("ScanCbExist");
        return -EEXIST;
    }

    sys_slist_append(&scan_cbs, &cb->node);

    return 0;
}

_NOT_USED
void bt_le_scan_cb_unregister(struct bt_le_scan_cb *cb)
{
    LOG_DBG("ScanCbUnreg");

    if (cb == NULL) {
        LOG_ERR("ScanCbNull");
        return;
    }

    sys_slist_find_and_remove(&scan_cbs, &cb->node);
}

static uint8_t get_adv_type(uint8_t evt_type)
{
    /* LOG_DBG("GetAdvType[%02x]", evt_type); */

    switch (evt_type) {
    case (BT_HCI_LE_ADV_EVT_TYPE_CONN |
                    BT_HCI_LE_ADV_EVT_TYPE_SCAN |
                    BT_HCI_LE_ADV_EVT_TYPE_LEGACY):
        return BT_GAP_ADV_TYPE_ADV_IND;

    case (BT_HCI_LE_ADV_EVT_TYPE_CONN |
                    BT_HCI_LE_ADV_EVT_TYPE_DIRECT |
                    BT_HCI_LE_ADV_EVT_TYPE_LEGACY):
        return BT_GAP_ADV_TYPE_ADV_DIRECT_IND;

    case (BT_HCI_LE_ADV_EVT_TYPE_SCAN |
                    BT_HCI_LE_ADV_EVT_TYPE_LEGACY):
        return BT_GAP_ADV_TYPE_ADV_SCAN_IND;

    case BT_HCI_LE_ADV_EVT_TYPE_LEGACY:
        return BT_GAP_ADV_TYPE_ADV_NONCONN_IND;

    case (BT_HCI_LE_ADV_EVT_TYPE_SCAN_RSP |
                    BT_HCI_LE_ADV_EVT_TYPE_CONN |
                    BT_HCI_LE_ADV_EVT_TYPE_SCAN |
                    BT_HCI_LE_ADV_EVT_TYPE_LEGACY):
    case (BT_HCI_LE_ADV_EVT_TYPE_SCAN_RSP |
                    BT_HCI_LE_ADV_EVT_TYPE_SCAN |
                    BT_HCI_LE_ADV_EVT_TYPE_LEGACY):
        return BT_GAP_ADV_TYPE_SCAN_RSP;

    default:
        return BT_GAP_ADV_TYPE_EXT_ADV;
    }
}

_IDF_ONLY
void bt_le_scan_recv_listener(uint16_t event_type,
                              uint8_t addr_type,
                              uint8_t addr_val[6],
                              int8_t rssi,
                              int8_t tx_power,
                              uint8_t sid,
                              uint8_t pri_phy,
                              uint8_t sec_phy,
                              uint16_t per_adv_itvl,
                              uint8_t data_len,
                              uint8_t *data)
{
    struct net_buf_simple_state state = {0};
    struct bt_le_scan_recv_info info = {0};
    struct bt_le_scan_cb *listener = NULL;
    struct net_buf_simple buf = {0};
    bt_addr_le_t addr = {0};

    /* LOG_DBG("ScanRecvListener[%u]", data_len); */

    if (data == NULL || data_len == 0) {
        LOG_DBG("ScanRecvDataNull[%u]", data_len);
        return;
    }

    addr.type = addr_type;
    memcpy(addr.a.val, addr_val, BT_ADDR_SIZE);

    info.addr = &addr;
    info.sid = sid;
    info.rssi = rssi;
    info.tx_power = tx_power;
    info.adv_type = get_adv_type(event_type);
    info.adv_props = event_type;
    info.interval = per_adv_itvl;
    info.primary_phy = pri_phy;
    info.secondary_phy = sec_phy;

    net_buf_simple_init_with_data(&buf, (void *)data, data_len);

    SYS_SLIST_FOR_EACH_CONTAINER(&scan_cbs, listener, node) {
        if (listener->recv) {
            net_buf_simple_save(&buf, &state);
            listener->recv(&info, &buf);
            net_buf_simple_restore(&buf, &state);
        }
    }
}

static struct bt_le_per_adv_sync per_adv_sync_pool[CONFIG_BT_PER_ADV_SYNC_MAX];

static sys_slist_t pa_sync_cbs = SYS_SLIST_STATIC_INIT(&pa_sync_cbs);

_LIB_ONLY
int bt_le_per_adv_sync_cb_register(struct bt_le_per_adv_sync_cb *cb)
{
    LOG_DBG("PaSyncCbReg");

    if (cb == NULL) {
        LOG_ERR("PaSyncCbNull");
        return -EINVAL;
    }

    if (sys_slist_find(&pa_sync_cbs, &cb->node, NULL)) {
        LOG_WRN("PaSyncCbExist");
        return -EEXIST;
    }

    sys_slist_append(&pa_sync_cbs, &cb->node);

    return 0;
}

_LIB_ONLY
int bt_le_per_adv_sync_get_info(struct bt_le_per_adv_sync *per_adv_sync,
                                struct bt_le_per_adv_sync_info *info)
{
    LOG_DBG("PaSyncGetInfo");

    if (per_adv_sync == NULL || info == NULL) {
        LOG_ERR("PaSyncInfoNull[%p][%p]", per_adv_sync, info);
        return -EINVAL;
    }

    bt_addr_le_copy(&info->addr, &per_adv_sync->addr);
    info->sid = per_adv_sync->sid;
    info->phy = per_adv_sync->phy;
    info->interval = per_adv_sync->interval;

    LOG_DBG("[%s][%u][%u][%u]",
            bt_addr_le_str(&info->addr), info->sid, info->phy, info->interval);

    return 0;
}

_LIB_ONLY
struct bt_le_per_adv_sync *bt_le_per_adv_sync_lookup_addr(const bt_addr_le_t *adv_addr,
                                                          uint8_t sid)
{
    struct bt_le_per_adv_sync *per_adv_sync = NULL;

    assert(adv_addr);

    LOG_DBG("PaSyncLookupAddr[%s][%u]", bt_addr_le_str(adv_addr), sid);

    for (size_t i = 0; i < ARRAY_SIZE(per_adv_sync_pool); i++) {
        if (atomic_test_bit(per_adv_sync_pool[i].flags, BT_PER_ADV_SYNC_SYNCED) &&
                bt_addr_le_eq(&per_adv_sync_pool[i].addr, adv_addr) &&
                per_adv_sync_pool[i].sid == sid) {
            per_adv_sync = &per_adv_sync_pool[i];
            break;
        }
    }

    if (per_adv_sync == NULL) {
        LOG_INF("PaSyncNotFound");
    }

    return per_adv_sync;
}

static struct bt_le_per_adv_sync *per_adv_sync_find(uint16_t handle)
{
    struct bt_le_per_adv_sync *per_adv_sync = NULL;

    for (size_t i = 0; i < ARRAY_SIZE(per_adv_sync_pool); i++) {
        if (atomic_test_bit(per_adv_sync_pool[i].flags,
                            BT_PER_ADV_SYNC_SYNCED) &&
                per_adv_sync_pool[i].handle == handle) {
            per_adv_sync = &per_adv_sync_pool[i];
            break;
        }
    }

    return per_adv_sync;
}

_IDF_ONLY
struct bt_le_per_adv_sync *bt_le_per_adv_sync_find_safe(uint16_t sync_handle)
{
    struct bt_le_per_adv_sync *per_adv_sync = NULL;
    LOG_DBG("PaSyncFind[%u]", sync_handle);
    bt_le_host_lock();
    per_adv_sync = per_adv_sync_find(sync_handle);
    bt_le_host_unlock();
    return per_adv_sync;
}

static struct bt_le_per_adv_sync *per_adv_sync_new(void)
{
    struct bt_le_per_adv_sync *per_adv_sync = NULL;

    for (size_t i = 0; i < ARRAY_SIZE(per_adv_sync_pool); i++) {
        if (atomic_test_bit(per_adv_sync_pool[i].flags,
                            BT_PER_ADV_SYNC_SYNCED) == false) {
            per_adv_sync = &per_adv_sync_pool[i];
            break;
        }
    }

    if (per_adv_sync) {
        (void)memset(per_adv_sync, 0, sizeof(*per_adv_sync));
        atomic_set_bit(per_adv_sync->flags, BT_PER_ADV_SYNC_SYNCED);
    }

    return per_adv_sync;
}

_IDF_ONLY
int bt_le_per_adv_sync_new(uint16_t sync_handle,
                           uint8_t sid,
                           uint8_t phy,
                           uint16_t interval,
                           uint8_t addr_type,
                           const uint8_t addr[6],
                           struct bt_le_per_adv_sync **out_sync)
{
    struct bt_le_per_adv_sync *per_adv_sync;

    LOG_DBG("PaSyncNew[%u][%u][%u][%u]", sync_handle, sid, phy, interval);

    if (addr_type > BT_ADDR_LE_RANDOM_ID || addr == NULL) {
        LOG_ERR("InvAddr[%02x][%p]", addr_type, addr);
        return -EINVAL;
    }

    per_adv_sync = per_adv_sync_find(sync_handle);
    if (per_adv_sync) {
        LOG_WRN("PaSyncExist[%u]", sync_handle);
        return -EEXIST;
    }

    per_adv_sync = per_adv_sync_new();
    if (per_adv_sync == NULL) {
        LOG_ERR("NoFreePaSync[%u]", sync_handle);
        return -ENOMEM;
    }

    per_adv_sync->handle = sync_handle;
    per_adv_sync->sid = sid;
    per_adv_sync->phy = phy;
    per_adv_sync->interval = interval;
    per_adv_sync->addr.type = addr_type;
    memcpy(per_adv_sync->addr.a.val, addr, BT_ADDR_SIZE);

    LOG_DBG("Addr[%s]", bt_addr_le_str(&per_adv_sync->addr));

    if (out_sync) {
        *out_sync = per_adv_sync;
    }

    return 0;
}

_IDF_ONLY
int bt_le_per_adv_sync_delete(uint16_t sync_handle)
{
    struct bt_le_per_adv_sync *per_adv_sync = NULL;

    LOG_DBG("PaSyncDelete[%u]", sync_handle);

    per_adv_sync = per_adv_sync_find(sync_handle);
    if (per_adv_sync == NULL) {
        LOG_ERR("PaSyncNotFound[%u]", sync_handle);
        return -ENODEV;
    }

    (void)memset(per_adv_sync, 0, sizeof(*per_adv_sync));

    return 0;
}

_IDF_ONLY
int bt_le_per_adv_sync_establish_listener(uint16_t sync_handle)
{
    struct bt_le_per_adv_sync_synced_info info = {0};
    struct bt_le_per_adv_sync *per_adv_sync = NULL;
    struct bt_le_per_adv_sync_cb *listener = NULL;

    LOG_DBG("PaSyncEstabListener[%u]", sync_handle);

    per_adv_sync = per_adv_sync_find(sync_handle);
    if (per_adv_sync == NULL) {
        LOG_ERR("PaSyncNotFound[%u]", sync_handle);
        return -ENODEV;
    }

    info.addr = &per_adv_sync->addr;
    info.sid = per_adv_sync->sid;
    info.interval = per_adv_sync->interval;
    info.phy = per_adv_sync->phy;
    info.conn = NULL;   /* TODO: update for PAST */

    SYS_SLIST_FOR_EACH_CONTAINER(&pa_sync_cbs, listener, node) {
        if (listener->synced) {
            listener->synced(per_adv_sync, &info);
        }
    }

    return 0;
}

_IDF_ONLY
int bt_le_per_adv_sync_lost_listener(uint16_t sync_handle)
{
    struct bt_le_per_adv_sync_term_info info = {0};
    struct bt_le_per_adv_sync *per_adv_sync = NULL;
    struct bt_le_per_adv_sync_cb *listener = NULL;

    LOG_DBG("PaSyncLostListener[%u]", sync_handle);

    per_adv_sync = per_adv_sync_find(sync_handle);
    if (per_adv_sync == NULL) {
        LOG_ERR("PaSyncNotFound[%u]", sync_handle);
        return -ENODEV;
    }

    info.addr = &per_adv_sync->addr;
    info.sid = per_adv_sync->sid;

    SYS_SLIST_FOR_EACH_CONTAINER(&pa_sync_cbs, listener, node) {
        if (listener->term) {
            listener->term(per_adv_sync, &info);
        }
    }

    return 0;
}

_IDF_ONLY
int bt_le_per_adv_sync_report_recv_listener(uint16_t sync_handle,
                                            const uint8_t *data,
                                            size_t data_length)
{
    struct bt_le_per_adv_sync_recv_info info = {0};
    struct bt_le_per_adv_sync *per_adv_sync = NULL;
    struct bt_le_per_adv_sync_cb *listener = NULL;
    struct net_buf_simple_state state = {0};
    struct net_buf_simple buf = {0};

    /* LOG_DBG("PaSyncReportRecvListener[%u][%u]", sync_handle, data_length); */

    if (data == NULL || data_length == 0) {
        LOG_DBG("PaSyncRecvDataNull[%u]", data_length);
        return -EINVAL;
    }

    per_adv_sync = per_adv_sync_find(sync_handle);
    if (per_adv_sync == NULL) {
        LOG_ERR("PaSyncNotFound[%u]", sync_handle);
        return -ENODEV;
    }

    info.addr = &per_adv_sync->addr;
    info.sid = per_adv_sync->sid;

    net_buf_simple_init_with_data(&buf, (void *)data, data_length);

    SYS_SLIST_FOR_EACH_CONTAINER(&pa_sync_cbs, listener, node) {
        if (listener->recv) {
            net_buf_simple_save(&buf, &state);
            listener->recv(per_adv_sync, &info, &buf);
            net_buf_simple_restore(&buf, &state);
        }
    }

    return 0;
}

_IDF_ONLY
void hci_le_biginfo_adv_report(struct net_buf *buf)
{
    struct bt_hci_evt_le_biginfo_adv_report *evt = (void *)buf->data;
    struct bt_le_per_adv_sync *per_adv_sync = NULL;
    struct bt_le_per_adv_sync_cb *listener = NULL;
    struct bt_iso_biginfo biginfo = {0};

    /* LOG_DBG("BigInfoRecvListener[%u]", evt->sync_handle); */

    per_adv_sync = per_adv_sync_find(evt->sync_handle);
    if (per_adv_sync == NULL) {
        LOG_ERR("PaSyncNotFound[%u]", evt->sync_handle);
        return;
    }

    biginfo.addr = &per_adv_sync->addr;
    biginfo.sid = per_adv_sync->sid;
    biginfo.num_bis = evt->num_bis;
    biginfo.sub_evt_count = evt->nse;
    biginfo.iso_interval = evt->iso_interval;
    biginfo.burst_number = evt->bn;
    biginfo.offset = evt->pto;
    biginfo.rep_count = evt->irc;
    biginfo.max_pdu = evt->max_pdu;
    biginfo.sdu_interval = sys_get_le24(evt->sdu_interval);
    biginfo.max_sdu = evt->max_sdu;
    biginfo.phy = evt->phy;
    biginfo.framing = evt->framing;
    biginfo.encryption = (evt->encryption ? true : false);

    SYS_SLIST_FOR_EACH_CONTAINER(&pa_sync_cbs, listener, node) {
        if (listener->biginfo) {
            listener->biginfo(per_adv_sync, &biginfo);
        }
    }
}

_LIB_ONLY
int bt_le_scan_start(const struct bt_le_scan_param *param, void *cb)
{
    int err = 0;

    if (atomic_test_bit(bt_dev.flags, BT_DEV_SCANNING) == false) {
        err = bt_le_nimble_scan_start(param, cb);
        if (err == 0) {
            atomic_set_bit(bt_dev.flags, BT_DEV_SCANNING);
        }
    } else {
        err = -EALREADY;
    }

    return err;
}

_LIB_ONLY
int bt_le_scan_stop(void)
{
    int err = 0;

    if (atomic_test_bit(bt_dev.flags, BT_DEV_SCANNING)) {
        err = bt_le_nimble_scan_stop();
        if (err == 0) {
            atomic_clear_bit(bt_dev.flags, BT_DEV_SCANNING);
        }
    }

    return err;
}
