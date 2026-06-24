/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/buf.h>
#include <zephyr/bluetooth/hci.h>

#include <../host/hci_core.h>
#include <../host/iso_internal.h>

#include "bta/bta_api.h"
#include "stack/btm_ble_api.h"
#include "stack/hcidefs.h"

#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatts_api.h"

#include "bluedroid/btm_error.h"
#include "bluedroid/hci.h"

#include "common/host.h"
#include "common/app/gap.h"

LOG_MODULE_REGISTER(ISO_BGAP, CONFIG_BT_ISO_LOG_LEVEL);

extern void btc_ble_5_gap_callback(tBTA_DM_BLE_5_GAP_EVENT event,
                                   tBTA_DM_BLE_5_GAP_CB_PARAMS *params);

#if (BLE_50_EXTEND_SYNC_EN == TRUE)
/* Active PA sync handle tracker. Used to synthesize a PA_SYNC_LOST event
 * to the iso task when the application calls
 * esp_ble_gap_periodic_adv_sync_terminate(): Bluedroid's
 * BTM_BLE_5_GAP_PERIODIC_ADV_SYNC_TERMINATE_COMPLETE_EVT only carries a
 * status byte and lacks sync_handle, so we recover it from here. This
 * mirrors NimBLE host behavior, where ble_gap_periodic_adv_sync_terminate
 * internally enqueues a sync_lost event.
 *
 * The tracker is populated by both PA_SYNC_ESTAB (direct scan-and-sync)
 * and PA_SYNC_TRANS_RECV (PAST-delivered sync) on success, and cleared
 * by real PA_SYNC_LOST or by the synthesized terminate path below.
 *
 * Single-slot — assumes one active PA sync (and at most one in-flight
 * terminate) at a time. The audio examples never hold multiple PA syncs
 * concurrently. Multi-sync acceptors / Broadcast Assistants will need an
 * N-slot tracker plus per-terminate intent capture.
 *
 * TODO(bluedroid): drop this once
 * BTM_BLE_5_GAP_PERIODIC_ADV_SYNC_TERMINATE_COMPLETE_EVT also surfaces
 * sync_handle (BTM_BlePeriodicAdvSyncTerm in btm_ble_5_gap.c knows the
 * handle, it just isn't propagated into cb_params). Ideally Bluedroid
 * would directly synthesize a SYNC_LOST event on successful terminate,
 * matching the NimBLE host contract; then both this tracker and the
 * synthesis below become dead code. */
#define ISO_PA_SYNC_HANDLE_NONE     0xFFFF
static uint16_t active_pa_sync_handle = ISO_PA_SYNC_HANDLE_NONE;
#endif /* BLE_50_EXTEND_SYNC_EN == TRUE */

/* Fast-path BTA → iso-queue post.
 *
 * BTU calls gap_app_cb directly; this function bypasses the BTC dispatch
 * hop for the audio-critical events so they reach the iso task in the same
 * order BTU received them. BIGINFO_ADV_REPORT already posts directly from
 * iso_evt_handler; routing PA_SYNC_ESTAB / PA_SYNC_LOST / EXT_ADV_REPORT /
 * PAST_RECV the same way keeps both event streams ordered. */
static void bt_le_bluedroid_gap_post_event_bta(tBTA_DM_BLE_5_GAP_EVENT event,
                                               tBTA_DM_BLE_5_GAP_CB_PARAMS *params);

static void gap_app_cb(tBTA_DM_BLE_5_GAP_EVENT event, tBTA_DM_BLE_5_GAP_CB_PARAMS *params)
{
    /* Audio-critical data events: post directly to iso task here to keep
     * ordering with BIGINFO_ADV_REPORT (which iso_evt_handler also posts
     * directly). The BTC dispatch below still runs for non-audio recipients
     * of esp_ble_gap_register_callback. */
    switch (event) {
#if (BLE_50_EXTEND_SCAN_EN == TRUE)
    case BTM_BLE_5_GAP_EXT_ADV_REPORT_EVT:
#endif
#if (BLE_50_EXTEND_SYNC_EN == TRUE)
    case BTM_BLE_5_GAP_PERIODIC_ADV_SYNC_ESTAB_EVT:
    case BTM_BLE_5_GAP_PERIODIC_ADV_SYNC_LOST_EVT:
    case BTM_BLE_5_GAP_PERIODIC_ADV_SYNC_TERMINATE_COMPLETE_EVT:
    case BTM_BLE_5_GAP_PERIODIC_ADV_REPORT_EVT:
#endif
#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
    case BTM_BLE_GAP_PERIODIC_ADV_SYNC_TRANS_RECV_EVT:
#endif
        bt_le_bluedroid_gap_post_event_bta(event, params);
        break;
    default:
        break;
    }

    /* Forward to BTC so esp_ble_gap_register_callback() recipients see it. */
    btc_ble_5_gap_callback(event, params);
}

void bt_le_bluedroid_gap_post_event(uint16_t event, void *param)
{
    const esp_ble_gap_cb_param_t *p = param;
    struct bt_le_gap_app_param *qev = NULL;
    int err;

    qev = calloc(1, sizeof(*qev));
    assert(qev);

    /* Only AUTH_CMPL reaches here from the application's
     * esp_ble_gap_register_callback path. EXT_ADV_REPORT / PA_SYNC_ESTAB /
     * PA_SYNC_LOST / PAST_RECV are posted directly by gap_app_cb on the BTM
     * BLE 5 channel; SMP events have no such channel so AUTH_CMPL is the
     * only one the application must forward. */
    switch (event) {
    case ESP_GAP_BLE_AUTH_CMPL_EVT: {
        const esp_ble_auth_cmpl_t *a = &p->ble_security.auth_cmpl;
        struct gatt_conn *gatt_conn;
        uint8_t sec_level;

        gatt_conn = bt_le_bluedroid_find_gatt_conn_with_addr(a->addr_type, a->bd_addr, false);
        if (gatt_conn == NULL) {
            LOG_ERR("[B]UnknownDevForEnc");
            free(qev);
            return;
        }

        qev->type = BT_LE_GAP_APP_PARAM_SECURITY_CHANGE;

        qev->security_change.status = (a->success ? 0x00 : 0xFF);

        /* Populate connection identity unconditionally so failure events
         * carry valid conn_handle / role / dst to the application. */
        qev->security_change.conn_handle = gatt_conn->conn_handle;
        qev->security_change.role = gatt_conn->role;
        qev->security_change.dst.type = gatt_conn->peer.type;
        memcpy(qev->security_change.dst.val, gatt_conn->peer.val, BT_ADDR_SIZE);

        if (qev->security_change.status == 0) {
            /* Derive level from auth_mode bits (mirrors NimBLE's sec_state
             * mapping). esp_ble_auth_cmpl_t exposes neither encrypted/
             * authenticated flags nor a key_size, so MITM presence is the
             * only authenticated-vs-Just-Works signal we have. */
            if (!(a->auth_mode & ESP_LE_AUTH_REQ_MITM)) {
                sec_level = BT_SECURITY_L2;
            } else if (!(a->auth_mode & ESP_LE_AUTH_REQ_SC_ONLY)) {
                sec_level = BT_SECURITY_L3;
            } else {
                sec_level = BT_SECURITY_L4;
            }

            qev->security_change.sec_level = sec_level;
            qev->security_change.bonded = (a->auth_mode & ESP_LE_AUTH_BOND) ? 1 : 0;
        }
        break;
    }

    default:
        LOG_WRN("[B]GapPostEvtUnexp[%u]", event);
        free(qev);
        return;
    }

    err = bt_le_iso_task_post(ISO_QUEUE_ITEM_TYPE_GAP_EVENT, qev, sizeof(*qev));
    if (err) {
        LOG_ERR("[B]GapPostEvtFail[%d][%u]", err, qev->type);
        free(qev);
    }
}

static void bt_le_bluedroid_gap_post_event_bta(tBTA_DM_BLE_5_GAP_EVENT event,
                                               tBTA_DM_BLE_5_GAP_CB_PARAMS *params)
{
    struct bt_le_gap_app_param *qev = NULL;
    enum iso_queue_item_type q_type;
    int err;

    qev = calloc(1, sizeof(*qev));
    assert(qev);

    switch (event) {
#if (BLE_50_EXTEND_SCAN_EN == TRUE)
    case BTM_BLE_5_GAP_EXT_ADV_REPORT_EVT: {
        const tBTM_BLE_EXT_ADV_REPORT *r = &params->ext_adv_report;

        qev->type = BT_LE_GAP_APP_PARAM_EXT_SCAN_RECV;

        qev->ext_scan_recv.event_type = (r->data_status << 5) | r->event_type;
        qev->ext_scan_recv.addr.type = r->addr_type;
        memcpy(qev->ext_scan_recv.addr.val, r->addr, BT_ADDR_SIZE);
        /* BTM declares rssi/tx_power as UINT8; the bytes are spec-defined as
         * signed (RSSI typically negative). Cast preserves the bit pattern. */
        qev->ext_scan_recv.rssi = (int8_t)r->rssi;
        qev->ext_scan_recv.tx_power = (int8_t)r->tx_power;
        qev->ext_scan_recv.sid = r->sid;
        qev->ext_scan_recv.pri_phy = r->primary_phy;
        qev->ext_scan_recv.sec_phy = r->secondry_phy;
        qev->ext_scan_recv.per_adv_itvl = r->per_adv_interval;
        qev->ext_scan_recv.data_len = r->adv_data_len;

        if (qev->ext_scan_recv.data_len) {
            qev->ext_scan_recv.data = calloc(1, qev->ext_scan_recv.data_len);
            assert(qev->ext_scan_recv.data);
            memcpy(qev->ext_scan_recv.data, r->adv_data, qev->ext_scan_recv.data_len);
        }
        break;
    }
#endif /* BLE_50_EXTEND_SCAN_EN == TRUE */

#if (BLE_50_EXTEND_SYNC_EN == TRUE)
    case BTM_BLE_5_GAP_PERIODIC_ADV_SYNC_ESTAB_EVT: {
        const tBTM_BLE_PERIOD_ADV_SYNC_ESTAB *e = &params->sync_estab;

        qev->type = BT_LE_GAP_APP_PARAM_PA_SYNC;

        qev->pa_sync.status = e->status;
        qev->pa_sync.sync_handle = e->sync_handle;
        qev->pa_sync.addr.type = e->adv_addr_type;
        memcpy(qev->pa_sync.addr.val, e->adv_addr, BT_ADDR_SIZE);
        qev->pa_sync.sid = e->sid;
        qev->pa_sync.adv_phy = e->adv_phy;
        qev->pa_sync.per_adv_itvl = e->period_adv_interval;
        qev->pa_sync.adv_ca = e->adv_clk_accuracy;

        if (e->status == 0) {
            if (active_pa_sync_handle != ISO_PA_SYNC_HANDLE_NONE) {
                /* Concurrent PA sync: single-slot tracker overwrites the
                 * existing handle, so a later terminate on the first sync
                 * would synthesize SYNC_LOST against the wrong one.
                 * See ISO_PA_SYNC_HANDLE_NONE doc for the multi-sync TODO. */
                LOG_WRN("[B]PaSyncEstabOverwrite[%04x->%04x]",
                        active_pa_sync_handle, e->sync_handle);
            } else {
                LOG_INF("[B]PaSyncEstab[%04x]", e->sync_handle);
            }
            active_pa_sync_handle = e->sync_handle;
        }
        break;
    }

    case BTM_BLE_5_GAP_PERIODIC_ADV_SYNC_LOST_EVT:
        qev->type = BT_LE_GAP_APP_PARAM_PA_SYNC_LOST;

        /* Public API exposes only sync_handle; reason isn't surfaced. */
        qev->pa_sync_lost.sync_handle = params->sync_lost.sync_handle;
        qev->pa_sync_lost.reason = 0;

        if (active_pa_sync_handle == qev->pa_sync_lost.sync_handle) {
            LOG_INF("[B]PaSyncLost[%04x]", qev->pa_sync_lost.sync_handle);
            active_pa_sync_handle = ISO_PA_SYNC_HANDLE_NONE;
        }
        break;

    case BTM_BLE_5_GAP_PERIODIC_ADV_SYNC_TERMINATE_COMPLETE_EVT:
        /* Host-commanded terminate: synthesize PA_SYNC_LOST so the iso
         * task sees a unified sync-end event regardless of whether the
         * sync was lost on-air or torn down via
         * esp_ble_gap_periodic_adv_sync_terminate(). The complete event
         * only carries status; recover sync_handle from the tracker.
         * See ISO_PA_SYNC_HANDLE_NONE definition for limitations and
         * the Bluedroid TODO. */
        if (params->per_adv_sync_term.status != BTM_SUCCESS ||
                active_pa_sync_handle == ISO_PA_SYNC_HANDLE_NONE) {
            LOG_WRN("[B]PaSyncTermNoSynth[%02x][%04x]",
                    params->per_adv_sync_term.status, active_pa_sync_handle);
            free(qev);
            return;
        }

        qev->type = BT_LE_GAP_APP_PARAM_PA_SYNC_LOST;
        qev->pa_sync_lost.sync_handle = active_pa_sync_handle;
        qev->pa_sync_lost.reason = 0;

        LOG_INF("[B]PaSyncTermSynth[%04x]", active_pa_sync_handle);
        active_pa_sync_handle = ISO_PA_SYNC_HANDLE_NONE;
        break;

    case BTM_BLE_5_GAP_PERIODIC_ADV_REPORT_EVT: {
        const tBTM_PERIOD_ADV_REPORT *r = &params->period_adv_report;

        qev->type = BT_LE_GAP_APP_PARAM_PA_SYNC_RECV;

        qev->pa_sync_recv.sync_handle = r->sync_handle;
        qev->pa_sync_recv.tx_power = r->tx_power;
        qev->pa_sync_recv.rssi = r->rssi;
        qev->pa_sync_recv.data_status = r->data_status;
        qev->pa_sync_recv.data_len = r->data_length;

        if (qev->pa_sync_recv.data_len) {
            qev->pa_sync_recv.data = calloc(1, qev->pa_sync_recv.data_len);
            assert(qev->pa_sync_recv.data);
            memcpy(qev->pa_sync_recv.data, r->data, qev->pa_sync_recv.data_len);
        }
        break;
    }
#endif /* BLE_50_EXTEND_SYNC_EN == TRUE */

#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
    case BTM_BLE_GAP_PERIODIC_ADV_SYNC_TRANS_RECV_EVT: {
        const tBTM_BLE_PERIOD_ADV_SYNC_TRANS_RECV *r = &params->past_recv;
        struct gatt_conn *gatt_conn;

        /* Look up the PAST-delivering ACL handle by peer BDA — past_recv
         * doesn't carry addr type. */
        gatt_conn = bt_le_bluedroid_find_gatt_conn_with_addr(0, r->addr, true);
        if (gatt_conn == NULL) {
            LOG_ERR("[B]UnknownPastSrc");
            free(qev);
            return;
        }

        qev->type = BT_LE_GAP_APP_PARAM_PA_SYNC_PAST;

        qev->pa_sync_past.status = r->status;
        qev->pa_sync_past.sync_handle = r->sync_handle;
        qev->pa_sync_past.addr.type = r->adv_addr_type;
        memcpy(qev->pa_sync_past.addr.val, r->adv_addr, BT_ADDR_SIZE);
        qev->pa_sync_past.sid = r->adv_sid;
        qev->pa_sync_past.adv_phy = r->adv_phy;
        qev->pa_sync_past.per_adv_itvl = r->adv_interval;
        qev->pa_sync_past.adv_ca = r->adv_clk_accuracy;
        qev->pa_sync_past.conn_handle = gatt_conn->conn_handle;

#if (BLE_50_EXTEND_SYNC_EN == TRUE)
        /* PAST-established syncs need tracker too: a later app-initiated
         * terminate hits TERMINATE_COMPLETE_EVT regardless of how the sync
         * was originally created. Gated by BLE_50_EXTEND_SYNC_EN since
         * that's where the tracker decl + LOST/TERMINATE consumers live. */
        if (r->status == 0) {
            if (active_pa_sync_handle != ISO_PA_SYNC_HANDLE_NONE) {
                LOG_WRN("[B]PaSyncTransOverwrite[%04x->%04x]",
                        active_pa_sync_handle, r->sync_handle);
            } else {
                LOG_INF("[B]PaSyncTrans[%04x]", r->sync_handle);
            }
            active_pa_sync_handle = r->sync_handle;
        }
#endif /* BLE_50_EXTEND_SYNC_EN == TRUE */
        break;
    }
#endif /* BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE */

    default:
        free(qev);
        return;
    }

    /* High-volume reports go to the droppable floodable queue (named 1:1 after
     * the HCI report event); connection / PA-sync lifecycle events stay
     * reliable on the normal queue. Keep this mapping in sync with
     * nimble/gap.c. */
    switch (qev->type) {
    case BT_LE_GAP_APP_PARAM_EXT_SCAN_RECV:
        q_type = ISO_QUEUE_ITEM_TYPE_EXT_ADV_REPORT;
        break;
    case BT_LE_GAP_APP_PARAM_PA_SYNC_RECV:
        q_type = ISO_QUEUE_ITEM_TYPE_PER_ADV_REPORT;
        break;
    default:
        q_type = ISO_QUEUE_ITEM_TYPE_GAP_EVENT;
        break;
    }

    err = bt_le_iso_task_post(q_type, qev, sizeof(*qev));
    if (err) {
        /* Floodable reports drop by design when the queue is full; only a
         * failure on the reliable (normal-queue) path is a real error. */
        if (q_type == ISO_QUEUE_ITEM_TYPE_GAP_EVENT) {
            LOG_ERR("[B]GapPostEvtBtaFail[%d][%u]", err, qev->type);
        } else {
            LOG_DBG("[B]GapRptDrop[%u]", qev->type);
        }
        goto free;
    }

    return;

free:
    /* Mirror nimble/gap.c cleanup: free nested data buffers carried by
     * specific event types before freeing the qev container itself. */
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

int bt_le_bluedroid_scan_start(const struct bt_le_scan_param *param)
{
    tBTM_STATUS status;

    LOG_DBG("[B]ScanStart[%u][%u][%u]", param->type, param->interval, param->window);

#if USE_DIRECT_HCI
    {
        /* HCI LE Set Extended Scan Parameters (uncoded only):
         *   own_addr_type(1) | scan_filter_policy(1) | scanning_phys(1)
         *   | per-phy { scan_type(1) | scan_interval(2) | scan_window(2) } */
        uint8_t cmd_params[8];

        cmd_params[0] = BLE_ADDR_PUBLIC;
        cmd_params[1] = 0;            /* filter_policy: accept all */
        cmd_params[2] = 0x01;         /* scanning_phys: LE 1M only */
        cmd_params[3] = param->type;
        sys_put_le16(param->interval, cmd_params + 4);
        sys_put_le16(param->window,   cmd_params + 6);

        status = bt_le_bluedroid_hci_send_sync(HCI_BLE_SET_EXT_SCAN_PARAMS,
                                               cmd_params, sizeof(cmd_params),
                                               NULL, 0);
    }
#else /* USE_DIRECT_HCI */
    {
        tBTM_BLE_EXT_SCAN_PARAMS scan_params = {0};

        scan_params.own_addr_type = BLE_ADDR_PUBLIC;
        scan_params.filter_policy = 0;
        scan_params.scan_duplicate = 0;
        scan_params.cfg_mask = BTM_BLE_GAP_EXT_SCAN_UNCODE_MASK;
        scan_params.uncoded_cfg.scan_type = param->type;
        scan_params.uncoded_cfg.scan_interval = param->interval;
        scan_params.uncoded_cfg.scan_window = param->window;

        bt_le_host_lock();
        status = BTM_BleSetExtendedScanParams(&scan_params);
        bt_le_host_unlock();
    }
#endif /* USE_DIRECT_HCI */

    if (status != BTM_SUCCESS) {
        LOG_ERR("[B]SetScanParamsFail[%02x]", status);
        return bluedroid_err_to_errno(status);
    }

#if USE_DIRECT_HCI
    {
        /* HCI LE Set Extended Scan Enable:
         *   enable(1) | filter_duplicates(1) | duration(2) | period(2)
         * duration=period=0 → continuous scan. */
        uint8_t cmd_params[6] = { 1, 0, 0, 0, 0, 0 };

        status = bt_le_bluedroid_hci_send_sync(HCI_BLE_SET_EXT_SCAN_ENABLE,
                                               cmd_params, sizeof(cmd_params),
                                               NULL, 0);
    }
#else /* USE_DIRECT_HCI */
    bt_le_host_lock();
    status = BTM_BleExtendedScan(true, 0, 0);
    bt_le_host_unlock();
#endif /* USE_DIRECT_HCI */

    if (status != BTM_SUCCESS) {
        LOG_ERR("[B]ScanStartFail[%02x]", status);
    }

    return bluedroid_err_to_errno(status);
}

int bt_le_bluedroid_scan_stop(void)
{
    tBTM_STATUS status;

    LOG_DBG("[B]ScanStop");

#if USE_DIRECT_HCI
    {
        /* HCI LE Set Extended Scan Enable with enable=0; other fields
         * are ignored by the controller per spec but must be present. */
        uint8_t cmd_params[6] = { 0, 0, 0, 0, 0, 0 };

        status = bt_le_bluedroid_hci_send_sync(HCI_BLE_SET_EXT_SCAN_ENABLE,
                                               cmd_params, sizeof(cmd_params),
                                               NULL, 0);
    }
#else /* USE_DIRECT_HCI */
    bt_le_host_lock();
    status = BTM_BleExtendedScan(false, 0, 0);
    bt_le_host_unlock();
#endif /* USE_DIRECT_HCI */

    if (status != BTM_SUCCESS) {
        LOG_ERR("[B]ScanStopFail[%02x]", status);
    }

    return bluedroid_err_to_errno(status);
}

int bt_le_bluedroid_gap_init(void)
{
    BTM_BleGapRegisterCallback(gap_app_cb);

    return 0;
}
