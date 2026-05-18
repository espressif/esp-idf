/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/buf.h>
#include <zephyr/bluetooth/hci.h>

#include <../host/hci_core.h>
#include <../host/iso_internal.h>

#include "host/ble_gap.h"
#include "host/ble_store.h"
#include "services/gap/ble_svc_gap.h"

#include "common/host.h"
#include "common/app/gap.h"

#include "nimble/hs_error.h"

LOG_MODULE_REGISTER(ISO_NGAP, CONFIG_BT_ISO_LOG_LEVEL);

/* Map a NimBLE connection's security state to Zephyr bt_security_t.
 *
 * To reach L3/L4 the app must configure NimBLE for authenticated pairing:
 *   ble_hs_cfg.sm_mitm   = 1;
 *   ble_hs_cfg.sm_io_cap = <non-NO_IO cap matching the peer>;
 *                          (e.g. DISP_ONLY + KEYBOARD_ONLY -> Passkey Entry,
 *                                DISP_YES_NO on both sides -> Numeric Compare)
 * L4 additionally requires ble_hs_cfg.sm_sc = 1 (LE Secure Connections).
 *
 * ble_gap_sec_state does not expose an SC flag; without it we cannot tell
 * legacy authenticated pairing apart from SC. Look up the peer's LTK record
 * to read the persisted sc bit. If the lookup fails (not bonded, store
 * unavailable) downgrade to L3 conservatively.
 */
static uint8_t nimble_desc_to_sec_level(const struct ble_gap_conn_desc *desc)
{
    const struct ble_gap_sec_state *sec_state = &desc->sec_state;
    struct ble_store_value_sec value = {0};
    struct ble_store_key_sec key = {0};
    int rc;

    /* L1: !encrypted */
    if (!sec_state->encrypted) {
        return BT_SECURITY_L1;
    }

    /* L2: encrypted, unauthenticated (Just Works) */
    if (!sec_state->authenticated) {
        return BT_SECURITY_L2;
    }

    /* L3: encrypted, authenticated, key_size < 16 */
    if (sec_state->key_size < 16) {
        return BT_SECURITY_L3;
    }

    key.peer_addr = desc->peer_id_addr;
    rc = ble_store_read_peer_sec(&key, &value);
    if (rc != 0 || !value.sc) {
        /* Cannot confirm SC -> conservatively L3 */
        return BT_SECURITY_L3;
    }

    /* L4: encrypted, authenticated, 128-bit key, LE Secure Connections */
    return BT_SECURITY_L4;
}

void bt_le_nimble_gap_post_event(void *param)
{
    struct bt_le_gap_app_param *qev = NULL;
    struct ble_gap_conn_desc desc = {0};
    struct ble_gap_event *ev = NULL;
    int err;

    qev = calloc(1, sizeof(*qev));
    assert(qev);

    memset(&desc, 0, sizeof(desc));
    ev = param;

    if (ev->type != BLE_GAP_EVENT_EXT_DISC &&
            ev->type != BLE_GAP_EVENT_PERIODIC_REPORT) {
        LOG_DBG("[N]PostGapEvt[%u]", ev->type);
    }

    /* TODO:
     * - Limit the adv report posted to ISO task;
     * - Limit the extended adv report posted to ISO task;
     * - Limit the periodic adv report posted to ISO task;
     *    - E.g. If the adv report contains any information interested by LE Audio.
     */

    switch (ev->type) {
    case BLE_GAP_EVENT_EXT_DISC:
        qev->type = BT_LE_GAP_APP_PARAM_EXT_SCAN_RECV;

        qev->ext_scan_recv.event_type = (ev->ext_disc.data_status << 5) | ev->ext_disc.props;
        qev->ext_scan_recv.addr.type = ev->ext_disc.addr.type;
        memcpy(qev->ext_scan_recv.addr.val, ev->ext_disc.addr.val, BT_ADDR_SIZE);
        qev->ext_scan_recv.rssi = ev->ext_disc.rssi;
        qev->ext_scan_recv.tx_power = ev->ext_disc.tx_power;
        qev->ext_scan_recv.sid = ev->ext_disc.sid;
        qev->ext_scan_recv.pri_phy = ev->ext_disc.prim_phy;
        qev->ext_scan_recv.sec_phy = ev->ext_disc.sec_phy;
        qev->ext_scan_recv.per_adv_itvl = ev->ext_disc.periodic_adv_itvl;
        qev->ext_scan_recv.data_len = ev->ext_disc.length_data;

        if (qev->ext_scan_recv.data_len) {
            qev->ext_scan_recv.data = calloc(1, qev->ext_scan_recv.data_len);
            assert(qev->ext_scan_recv.data);

            memcpy(qev->ext_scan_recv.data, ev->ext_disc.data, qev->ext_scan_recv.data_len);
        }
        break;

    case BLE_GAP_EVENT_PERIODIC_SYNC:
        qev->type = BT_LE_GAP_APP_PARAM_PA_SYNC;

        qev->pa_sync.status = ev->periodic_sync.status;
        qev->pa_sync.sync_handle = ev->periodic_sync.sync_handle;
        qev->pa_sync.addr.type = ev->periodic_sync.adv_addr.type;
        memcpy(qev->pa_sync.addr.val, ev->periodic_sync.adv_addr.val, BT_ADDR_SIZE);
        qev->pa_sync.sid = ev->periodic_sync.sid;
        qev->pa_sync.adv_phy = ev->periodic_sync.adv_phy;
        qev->pa_sync.per_adv_itvl = ev->periodic_sync.per_adv_ival;
        qev->pa_sync.adv_ca = ev->periodic_sync.adv_clk_accuracy;
        break;

    case BLE_GAP_EVENT_PERIODIC_TRANSFER:
    case BLE_GAP_EVENT_PERIODIC_TRANSFER_V2:
        /* Both events share the same periodic_transfer union field; V2 only
         * adds PAwR-specific tail fields that this handler does not consume.
         */
        qev->type = BT_LE_GAP_APP_PARAM_PA_SYNC_PAST;

        qev->pa_sync_past.status = ev->periodic_transfer.status;
        qev->pa_sync_past.sync_handle = ev->periodic_transfer.sync_handle;
        qev->pa_sync_past.addr.type = ev->periodic_transfer.adv_addr.type;
        memcpy(qev->pa_sync_past.addr.val, ev->periodic_transfer.adv_addr.val, BT_ADDR_SIZE);
        qev->pa_sync_past.sid = ev->periodic_transfer.sid;
        qev->pa_sync_past.adv_phy = ev->periodic_transfer.adv_phy;
        qev->pa_sync_past.per_adv_itvl = ev->periodic_transfer.per_adv_itvl;
        qev->pa_sync_past.adv_ca = ev->periodic_transfer.adv_clk_accuracy;
        qev->pa_sync_past.conn_handle = ev->periodic_transfer.conn_handle;
        break;

    case BLE_GAP_EVENT_PERIODIC_SYNC_LOST:
        qev->type = BT_LE_GAP_APP_PARAM_PA_SYNC_LOST;

        qev->pa_sync_lost.sync_handle = ev->periodic_sync_lost.sync_handle;
        qev->pa_sync_lost.reason = ev->periodic_sync_lost.reason;
        break;

    case BLE_GAP_EVENT_PERIODIC_REPORT:
        qev->type = BT_LE_GAP_APP_PARAM_PA_SYNC_RECV;

        qev->pa_sync_recv.sync_handle = ev->periodic_report.sync_handle;
        qev->pa_sync_recv.tx_power = ev->periodic_report.tx_power;
        qev->pa_sync_recv.rssi = ev->periodic_report.rssi;
        qev->pa_sync_recv.data_status = ev->periodic_report.data_status;
        qev->pa_sync_recv.data_len = ev->periodic_report.data_length;

        if (qev->pa_sync_recv.data_len) {
            qev->pa_sync_recv.data = calloc(1, qev->pa_sync_recv.data_len);
            assert(qev->pa_sync_recv.data);

            memcpy(qev->pa_sync_recv.data, ev->periodic_report.data, qev->pa_sync_recv.data_len);
        }
        break;

    case BLE_GAP_EVENT_CONNECT:
        qev->type = BT_LE_GAP_APP_PARAM_ACL_CONNECT;

        qev->acl_connect.status = ev->connect.status;
        if (qev->acl_connect.status == 0) {
            err = ble_gap_conn_find(ev->connect.conn_handle, &desc);
            assert(err == 0);

            qev->acl_connect.conn_handle = desc.conn_handle;
            qev->acl_connect.role = desc.role;
            qev->acl_connect.dst.type = desc.peer_id_addr.type;
            memcpy(qev->acl_connect.dst.val, desc.peer_id_addr.val, BT_ADDR_SIZE);
        }
        break;

    case BLE_GAP_EVENT_DISCONNECT:
        qev->type = BT_LE_GAP_APP_PARAM_ACL_DISCONNECT;

        qev->acl_disconnect.conn_handle = ev->disconnect.conn.conn_handle;
        qev->acl_disconnect.reason = ev->disconnect.reason;
        break;

    case BLE_GAP_EVENT_ENC_CHANGE:
        qev->type = BT_LE_GAP_APP_PARAM_SECURITY_CHANGE;

        qev->security_change.status = ev->enc_change.status;
        if (qev->security_change.status == 0) {
            err = ble_gap_conn_find(ev->enc_change.conn_handle, &desc);
            assert(err == 0);

            qev->security_change.conn_handle = ev->enc_change.conn_handle;
            qev->security_change.role = desc.role;
            qev->security_change.sec_level = nimble_desc_to_sec_level(&desc);
            qev->security_change.bonded = desc.sec_state.bonded;
            qev->security_change.dst.type = desc.peer_id_addr.type;
            memcpy(qev->security_change.dst.val, desc.peer_id_addr.val, BT_ADDR_SIZE);
        }
        break;

    default:
        LOG_WRN("[N]GapPostEvtUnexp[%u]", ev->type);
        free(qev);
        return;
    }

    err = bt_le_iso_task_post(ISO_QUEUE_ITEM_TYPE_GAP_EVENT, qev, sizeof(*qev));
    if (err) {
        LOG_ERR("[N]GapPostEvtFail[%d][%u]", err, qev->type);
        goto free;
    }

    return;

free:
    switch (qev->type) {
    case BT_LE_GAP_APP_PARAM_EXT_SCAN_RECV:
        if (qev->ext_scan_recv.data) {
            free(qev->ext_scan_recv.data);
            qev->ext_scan_recv.data = NULL;
        }
        break;
    case BT_LE_GAP_APP_PARAM_PA_SYNC_RECV:
        if (qev->pa_sync_recv.data) {
            free(qev->pa_sync_recv.data);
            qev->pa_sync_recv.data = NULL;
        }
        break;
    default:
        break;
    }

    free(qev);
}

int bt_le_nimble_scan_start(const struct bt_le_scan_param *param, ble_gap_event_fn *cb)
{
    struct ble_gap_ext_disc_params uncoded = {0};
    int rc;

    LOG_DBG("[N]ScanStart[%u][%u][%u]", param->type, param->interval, param->window);

    uncoded.itvl = param->interval;
    uncoded.window = param->window;
    uncoded.passive = !param->type;

    /* LE Audio sources broadcast via extended advertising; legacy
     * ble_gap_disc would miss them. Uncoded-only mirrors the Bluedroid
     * side which sets BTM_BLE_GAP_EXT_SCAN_UNCODE_MASK. */
    rc = ble_gap_ext_disc(BLE_OWN_ADDR_PUBLIC, 0, 0, 0, 0, 0,
                          &uncoded, NULL, cb, NULL);
    if (rc) {
        LOG_ERR("[N]ScanStartFail[%d]", rc);
    }

    return nimble_err_to_errno(rc);
}

int bt_le_nimble_scan_stop(void)
{
    int rc;

    LOG_DBG("[N]ScanStop");

    rc = ble_gap_disc_cancel();
    if (rc) {
        LOG_ERR("[N]ScanStopFail[%d]", rc);
    }

    return nimble_err_to_errno(rc);
}

int bt_le_nimble_iso_disconnect(uint16_t conn_handle, uint8_t reason)
{
    return nimble_err_to_errno(ble_gap_iso_disconnect(conn_handle, reason));
}
