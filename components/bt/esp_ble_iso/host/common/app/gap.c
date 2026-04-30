/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/buf.h>
#include <zephyr/bluetooth/hci.h>

#include <../host/hci_core.h>
#include <../host/iso_internal.h>

#include "common/host.h"
#include "common/app/gap.h"

static bt_le_gap_app_cb gap_app_cb;

int bt_le_gap_app_cb_register(bt_le_gap_app_cb cb)
{
    LOG_DBG("GapAppCbReg");

    if (cb == NULL) {
        LOG_ERR("GapAppCbNull");
        return -EINVAL;
    }

    if (gap_app_cb) {
        LOG_WRN("GapAppCbExist");
        return -EALREADY;
    }

    gap_app_cb = cb;

    return 0;
}

void bt_le_gap_app_cb_unregister(void)
{
    if (gap_app_cb) {
        LOG_DBG("GapAppCbUnreg");
        gap_app_cb = NULL;
    }
}

static inline void bt_le_gap_app_cb_evt(struct bt_le_gap_app_event *event)
{
    if (gap_app_cb) {
        gap_app_cb(event);
    }
}

struct biginfo {
    uint16_t sync_handle;
    uint8_t  num_bis;
    uint8_t  nse;
    uint16_t iso_interval;
    uint8_t  bn;
    uint8_t  pto;
    uint8_t  irc;
    uint16_t max_pdu;
    uint8_t  sdu_interval[3];
    uint16_t max_sdu;
    uint8_t  phy;
    uint8_t  framing;
    uint8_t  encryption;
} __packed;

void bt_le_gap_app_biginfo_event(uint8_t *param)
{
    struct bt_le_gap_app_event event = {
        .type = BT_LE_GAP_APP_EVENT_BIGINFO_RECV,
    };
    struct biginfo *biginfo;

    biginfo = (void *)param;

    event.biginfo_recv.sync_handle = biginfo->sync_handle;
    event.biginfo_recv.num_bis = biginfo->num_bis;
    event.biginfo_recv.nse = biginfo->nse;
    event.biginfo_recv.iso_interval = biginfo->iso_interval;
    event.biginfo_recv.bn = biginfo->bn;
    event.biginfo_recv.pto = biginfo->pto;
    event.biginfo_recv.irc = biginfo->irc;
    event.biginfo_recv.max_pdu = biginfo->max_pdu;
    event.biginfo_recv.sdu_interval = sys_get_le24(biginfo->sdu_interval);
    event.biginfo_recv.max_sdu = biginfo->max_sdu;
    event.biginfo_recv.phy = biginfo->phy;
    event.biginfo_recv.framing = biginfo->framing;
    event.biginfo_recv.encryption = biginfo->encryption;

    bt_le_gap_app_cb_evt(&event);
}

static void handle_ext_scan_recv_event_safe(struct bt_le_gap_app_param *param)
{
    struct bt_le_gap_app_event event = {
        .type = BT_LE_GAP_APP_EVENT_EXT_SCAN_RECV,
    };

    event.ext_scan_recv.event_type = param->ext_scan_recv.event_type;
    event.ext_scan_recv.addr.type = param->ext_scan_recv.addr.type;
    memcpy(event.ext_scan_recv.addr.val, param->ext_scan_recv.addr.val, BT_ADDR_SIZE);
    event.ext_scan_recv.rssi = param->ext_scan_recv.rssi;
    event.ext_scan_recv.tx_power = param->ext_scan_recv.tx_power;
    event.ext_scan_recv.sid = param->ext_scan_recv.sid;
    event.ext_scan_recv.pri_phy = param->ext_scan_recv.pri_phy;
    event.ext_scan_recv.sec_phy = param->ext_scan_recv.sec_phy;
    event.ext_scan_recv.per_adv_itvl = param->ext_scan_recv.per_adv_itvl;
    event.ext_scan_recv.data_len = param->ext_scan_recv.data_len;
    event.ext_scan_recv.data = param->ext_scan_recv.data;

    bt_le_host_lock();

    bt_le_scan_recv_listener(event.ext_scan_recv.event_type,
                             event.ext_scan_recv.addr.type,
                             event.ext_scan_recv.addr.val,
                             event.ext_scan_recv.rssi,
                             event.ext_scan_recv.tx_power,
                             event.ext_scan_recv.sid,
                             event.ext_scan_recv.pri_phy,
                             event.ext_scan_recv.sec_phy,
                             event.ext_scan_recv.per_adv_itvl,
                             event.ext_scan_recv.data_len,
                             event.ext_scan_recv.data);

    bt_le_host_unlock();

    bt_le_gap_app_cb_evt(&event);

    if (param->ext_scan_recv.data) {
        free(param->ext_scan_recv.data);
    }
}

static void handle_pa_sync_event_safe(struct bt_le_gap_app_param *param)
{
    struct bt_le_gap_app_event event = {
        .type = BT_LE_GAP_APP_EVENT_PA_SYNC,
    };
    struct bt_le_per_adv_sync *per_adv_sync;
    int err;

    event.pa_sync.status = param->pa_sync.status;
    event.pa_sync.sync_handle = param->pa_sync.sync_handle;
    event.pa_sync.addr.type = param->pa_sync.addr.type;
    memcpy(event.pa_sync.addr.val, param->pa_sync.addr.val, BT_ADDR_SIZE);
    event.pa_sync.sid = param->pa_sync.sid;
    event.pa_sync.adv_phy = param->pa_sync.adv_phy;
    event.pa_sync.per_adv_itvl = param->pa_sync.per_adv_itvl;
    event.pa_sync.adv_ca = param->pa_sync.adv_ca;

    bt_le_host_lock();

    if (event.pa_sync.status) {
        goto end;
    }

    err = bt_le_per_adv_sync_new(event.pa_sync.sync_handle,
                                 event.pa_sync.sid,
                                 event.pa_sync.adv_phy,
                                 event.pa_sync.per_adv_itvl,
                                 event.pa_sync.addr.type,
                                 event.pa_sync.addr.val,
                                 &per_adv_sync);
    if (err) {
        goto end;
    }

    err = bt_le_per_adv_sync_establish_listener(event.pa_sync.sync_handle);
    assert(err == 0);

end:
    bt_le_host_unlock();

    bt_le_gap_app_cb_evt(&event);
}

static void handle_pa_sync_lost_event_safe(struct bt_le_gap_app_param *param)
{
    struct bt_le_gap_app_event event = {
        .type = BT_LE_GAP_APP_EVENT_PA_SYNC_LOST,
    };
    int err;

    event.pa_sync_lost.sync_handle = param->pa_sync_lost.sync_handle;
    event.pa_sync_lost.reason = param->pa_sync_lost.reason;

    bt_le_host_lock();

    err = bt_le_per_adv_sync_lost_listener(event.pa_sync_lost.sync_handle);
    if (err) {
        goto end;
    }

    err = bt_le_per_adv_sync_delete(event.pa_sync_lost.sync_handle);
    assert(err == 0);

end:
    bt_le_host_unlock();

    bt_le_gap_app_cb_evt(&event);
}

static void handle_pa_sync_recv_event_safe(struct bt_le_gap_app_param *param)
{
    struct bt_le_gap_app_event event = {
        .type = BT_LE_GAP_APP_EVENT_PA_SYNC_RECV,
    };

    event.pa_sync_recv.sync_handle = param->pa_sync_recv.sync_handle;
    event.pa_sync_recv.tx_power = param->pa_sync_recv.tx_power;
    event.pa_sync_recv.rssi = param->pa_sync_recv.rssi;
    event.pa_sync_recv.data_status = param->pa_sync_recv.data_status;
    event.pa_sync_recv.data_len = param->pa_sync_recv.data_len;
    event.pa_sync_recv.data = param->pa_sync_recv.data;

    bt_le_host_lock();

    bt_le_per_adv_sync_report_recv_listener(event.pa_sync_recv.sync_handle,
                                            event.pa_sync_recv.data,
                                            event.pa_sync_recv.data_len);

    bt_le_host_unlock();

    bt_le_gap_app_cb_evt(&event);

    if (param->pa_sync_recv.data) {
        free(param->pa_sync_recv.data);
    }
}

static void handle_acl_connect_event_safe(struct bt_le_gap_app_param *param)
{
    struct bt_le_gap_app_event event = {
        .type = BT_LE_GAP_APP_EVENT_ACL_CONNECT,
    };
    struct bt_conn *conn;
    bt_addr_le_t dst;
    int err;

    event.acl_connect.status = param->acl_connect.status;
    event.acl_connect.conn_handle = param->acl_connect.conn_handle;
    event.acl_connect.role = param->acl_connect.role;
    event.acl_connect.dst.type = param->acl_connect.dst.type;
    memcpy(event.acl_connect.dst.val, param->acl_connect.dst.val, BT_ADDR_SIZE);

    bt_le_host_lock();

    if (event.acl_connect.status) {
        goto end;
    }

    conn = bt_le_acl_conn_find(event.acl_connect.conn_handle);
    if (conn) {
        LOG_WRN("AclConnExist[%u]", event.acl_connect.conn_handle);
        goto end;
    }

    dst.type = event.acl_connect.dst.type;
    memcpy(dst.a.val, event.acl_connect.dst.val, BT_ADDR_SIZE);

    /* Not encrypted at this moment */
    err = bt_le_acl_conn_new(event.acl_connect.conn_handle,
                             event.acl_connect.role,
                             &dst, BT_SECURITY_L1);
    if (err) {
        goto end;
    }

    err = bt_le_acl_conn_connected_listener(event.acl_connect.conn_handle);
    assert(err == 0);

end:
    bt_le_host_unlock();

    bt_le_gap_app_cb_evt(&event);
}

static void handle_acl_disconnect_event_safe(struct bt_le_gap_app_param *param)
{
    struct bt_le_gap_app_event event = {
        .type = BT_LE_GAP_APP_EVENT_ACL_DISCONNECT,
    };
    int err;

    event.acl_disconnect.conn_handle = param->acl_disconnect.conn_handle;
    event.acl_disconnect.reason = param->acl_disconnect.reason;

    bt_le_host_lock();

    err = bt_le_acl_conn_disconnected_listener(event.acl_disconnect.conn_handle,
                                               event.acl_disconnect.reason);
    if (err) {
        goto end;
    }

    err = bt_le_acl_conn_delete(event.acl_disconnect.conn_handle);
    assert(err == 0);

end:
    bt_le_host_unlock();

    bt_le_gap_app_cb_evt(&event);
}

static void handle_security_change_event_safe(struct bt_le_gap_app_param *param)
{
    struct bt_le_gap_app_event event = {
        .type = BT_LE_GAP_APP_EVENT_SECURITY_CHANGE,
    };
    struct bt_conn *conn;
    bt_addr_le_t dst;
    bool encrypted;
    int err;

    event.security_change.status = param->security_change.status;
    event.security_change.conn_handle = param->security_change.conn_handle;
    event.security_change.role = param->security_change.role;
    event.security_change.sec_level = param->security_change.sec_level;
    event.security_change.bonded = param->security_change.bonded;
    event.security_change.dst.type = param->security_change.dst.type;
    memcpy(event.security_change.dst.val, param->security_change.dst.val, BT_ADDR_SIZE);

    encrypted = false;

    bt_le_host_lock();

    if (event.security_change.status) {
        goto end;
    }

    conn = bt_le_acl_conn_find(event.security_change.conn_handle);
    if (conn == NULL) {
        dst.type = event.security_change.dst.type;
        memcpy(dst.a.val, event.security_change.dst.val, BT_ADDR_SIZE);

        err = bt_le_acl_conn_new(event.security_change.conn_handle,
                                 event.security_change.role,
                                 &dst, event.security_change.sec_level);
        if (err) {
            goto end;
        }
    } else {
        err = bt_le_acl_conn_update(event.security_change.conn_handle,
                                    event.security_change.sec_level,
                                    &encrypted);
        assert(err == 0);
    }

    err = bt_le_acl_conn_security_changed_listener(event.security_change.conn_handle,
                                                   event.security_change.sec_level);
    assert(err == 0);

    if (encrypted) {
        /* TODO: check if bonded or not */
        err = bt_le_acl_conn_pairing_completed_listener(event.security_change.conn_handle, true);
        assert(err == 0);
    }

end:
    bt_le_host_unlock();

    bt_le_gap_app_cb_evt(&event);
}

static void handle_identity_resolve_event_safe(struct bt_le_gap_app_param *param)
{
    struct bt_le_gap_app_event event = {
        .type = BT_LE_GAP_APP_EVENT_IDENTITY_RESOLVE,
    };
    bt_addr_le_t identity;
    bt_addr_le_t rpa;

    event.identity_resolve.conn_handle = param->identity_resolve.conn_handle;
    event.identity_resolve.rpa.type = param->identity_resolve.rpa.type;
    memcpy(event.identity_resolve.rpa.val, param->identity_resolve.rpa.val, BT_ADDR_SIZE);
    event.identity_resolve.identity.type = param->identity_resolve.identity.type;
    memcpy(event.identity_resolve.identity.val, param->identity_resolve.identity.val, BT_ADDR_SIZE);

    identity.type = event.identity_resolve.identity.type;
    memcpy(identity.a.val, event.identity_resolve.identity.val, BT_ADDR_SIZE);

    rpa.type = event.identity_resolve.rpa.type;
    memcpy(rpa.a.val, event.identity_resolve.rpa.val, BT_ADDR_SIZE);

    bt_le_host_lock();

    bt_le_acl_conn_identity_resolved_listener(event.identity_resolve.conn_handle, &rpa, &identity);

    bt_le_host_unlock();

    bt_le_gap_app_cb_evt(&event);
}

static void handle_bond_delete_event_safe(struct bt_le_gap_app_param *param)
{
    struct bt_le_gap_app_event event = {
        .type = BT_LE_GAP_APP_EVENT_BOND_DELETE,
    };
    bt_addr_le_t peer;

    event.bond_delete.id = param->bond_delete.id;
    event.bond_delete.peer.type = param->bond_delete.peer.type;
    memcpy(event.bond_delete.peer.val, param->bond_delete.peer.val, BT_ADDR_SIZE);

    peer.type = event.bond_delete.peer.type;
    memcpy(peer.a.val, event.bond_delete.peer.val, sizeof(peer.a.val));

    bt_le_host_lock();

    bt_le_acl_conn_bond_deleted_listener(event.bond_delete.id, &peer);

    bt_le_host_unlock();

    bt_le_gap_app_cb_evt(&event);
}

void bt_le_gap_handle_event(uint8_t *data, size_t data_len)
{
    struct bt_le_gap_app_param *param;

    assert(data && data_len);

    param = (struct bt_le_gap_app_param *)data;

    switch (param->type) {
    case BT_LE_GAP_APP_PARAM_EXT_SCAN_RECV:
        handle_ext_scan_recv_event_safe(param);
        break;
    case BT_LE_GAP_APP_PARAM_PA_SYNC:
        handle_pa_sync_event_safe(param);
        break;
    case BT_LE_GAP_APP_PARAM_PA_SYNC_LOST:
        handle_pa_sync_lost_event_safe(param);
        break;
    case BT_LE_GAP_APP_PARAM_PA_SYNC_RECV:
        handle_pa_sync_recv_event_safe(param);
        break;
    case BT_LE_GAP_APP_PARAM_ACL_CONNECT:
        handle_acl_connect_event_safe(param);
        break;
    case BT_LE_GAP_APP_PARAM_ACL_DISCONNECT:
        handle_acl_disconnect_event_safe(param);
        break;
    case BT_LE_GAP_APP_PARAM_SECURITY_CHANGE:
        handle_security_change_event_safe(param);
        break;
    case BT_LE_GAP_APP_PARAM_IDENTITY_RESOLVE:
        handle_identity_resolve_event_safe(param);
        break;
    case BT_LE_GAP_APP_PARAM_BOND_DELETE:
        handle_bond_delete_event_safe(param);
        break;
    default:
        assert(0);
        break;
    }

    free(data);
}

void bt_le_gap_app_post_event(uint8_t type, void *param)
{
    /* Currently type is not used */
    ARG_UNUSED(type);

    bt_le_nimble_gap_post_event(param);
}
