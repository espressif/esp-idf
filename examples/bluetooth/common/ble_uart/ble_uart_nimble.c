/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 *
 * ESP-BLE-UART — NimBLE backend. Implements the lifecycle declared in
 * ble_uart.h on top of the NimBLE host. Active when
 * CONFIG_BT_NIMBLE_ENABLED=y; otherwise ble_uart_bluedroid.c is used.
 */

#include "sdkconfig.h"

#if CONFIG_BT_NIMBLE_ENABLED

#include "ble_uart.h"

#include <assert.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_random.h"

#include "nimble/ble.h"
#include "host/ble_att.h"
#include "host/ble_gap.h"
#include "host/ble_gatt.h"
#include "host/ble_hs.h"
#include "host/ble_hs_mbuf.h"
#include "host/ble_sm.h"
#include "host/ble_uuid.h"
#include "host/util/util.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

/* tx path needs notifications; the disabled-path in
 * ble_gatts_notify_custom() leaks the caller's mbuf. */
#if !MYNEWT_VAL(BLE_GATT_NOTIFY)
#error "ble_uart NimBLE backend requires MYNEWT_VAL(BLE_GATT_NOTIFY)=1"
#endif

static const char *TAG = "ble_uart";

/* Map NimBLE rc → public BLE_UART_E* code; unknown rcs → EFAIL. */
static int xlate_rc(int nimble_rc)
{
    switch (nimble_rc) {
    case 0:                return BLE_UART_OK;
    case BLE_HS_EINVAL:    return BLE_UART_EINVAL;
    case BLE_HS_ENOTCONN:  return BLE_UART_ENOTCONN;
    case BLE_HS_ENOMEM:    return BLE_UART_ENOMEM;
    case BLE_HS_EALREADY:  return BLE_UART_EALREADY;
    default:               return BLE_UART_EFAIL;
    }
}

/* Collapse NimBLE's 4-value peer addr type (0–3) into our public
 * 2-value enum (identity types map onto public/random). */
static uint8_t nimble_peer_type_to_uart(uint8_t nimble_type)
{
    return (nimble_type == BLE_ADDR_PUBLIC ||
            nimble_type == BLE_ADDR_PUBLIC_ID)
           ? BLE_UART_ADDR_TYPE_PUBLIC
           : BLE_UART_ADDR_TYPE_RANDOM;
}

/* Marshal a NimBLE ble_addr_t into our public ble_uart_addr_t. */
static void from_nimble_addr(const ble_addr_t *src, ble_uart_addr_t *dst)
{
    dst->type = nimble_peer_type_to_uart(src->type);
    for (int i = 0; i < 6; i++) {
        dst->bytes[i] = src->val[5 - i];
    }
}

/* Provided by NimBLE's `store/config` lib. */
extern void ble_store_config_init(void);

/* ===== UUIDs =========================================================== */

/* BLE UART profile UUIDs in little-endian byte order. */
#define NUS_SVC_BYTES   0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, \
                        0x93, 0xf3, 0xa3, 0xb5, 0x01, 0x00, 0x40, 0x6e
#define NUS_RX_BYTES    0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, \
                        0x93, 0xf3, 0xa3, 0xb5, 0x02, 0x00, 0x40, 0x6e
#define NUS_TX_BYTES    0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, \
                        0x93, 0xf3, 0xa3, 0xb5, 0x03, 0x00, 0x40, 0x6e

const ble_uart_uuid128_t ble_uart_service_uuid = { .bytes = { NUS_SVC_BYTES } };

static const ble_uuid128_t s_svc_uuid    = BLE_UUID128_INIT(NUS_SVC_BYTES);
static const ble_uuid128_t s_chr_rx_uuid = BLE_UUID128_INIT(NUS_RX_BYTES);
static const ble_uuid128_t s_chr_tx_uuid = BLE_UUID128_INIT(NUS_TX_BYTES);

/* ===== State =========================================================== */

/* RX scratch capacity. Tunable via menuconfig (Component config → ESP-BLE-UART
 * library); fall
 * back to 1024 if CONFIG_BLE_UART_RX_SCRATCH_SIZE is absent. */
#ifndef CONFIG_BLE_UART_RX_SCRATCH_SIZE
#define CONFIG_BLE_UART_RX_SCRATCH_SIZE     1024
#endif
#define RX_SCRATCH      CONFIG_BLE_UART_RX_SCRATCH_SIZE

/* Cached device name. Avoids ble_svc_gap_device_name() which returns
 * NULL when CONFIG_BT_NIMBLE_GAP_SERVICE=n (would NULL-deref). The
 * cap is BLE_UART_DEVICE_NAME_MAX (validated in install) + NUL; we
 * round up for safety margin. */
#define DEV_NAME_MAX     (BLE_UART_DEVICE_NAME_MAX + 2)

static ble_uart_config_t s_cfg;
static char              s_dev_name[DEV_NAME_MAX];
static uint16_t          s_tx_val_handle;
/* Volatile: written from NimBLE host task, polled from caller task. */
static volatile uint16_t s_conn_handle = BLE_HS_CONN_HANDLE_NONE;
static bool              s_subscribed;
static bool              s_installed;
static bool              s_opened;
/* After close(), ble_gatts_stop() drops svc-def pointers; open() must
 * count/add again before the next ble_hs_start().  Cleared on install. */
static bool              s_gatts_needs_readd;
#if MYNEWT_VAL(BLE_HS_AUTO_START)
/* Set in install() when nimble_port_init() queues the one-shot AUTO_START
 * event; cleared on first open() so we don't also ble_hs_sched_start() and
 * trip assert(rc==0) in ble_hs_event_start_stage2 (BLE_HS_EALREADY). */
static bool              s_hs_auto_start_pending;
#endif
static bool              s_shutting_down;   /* gates auto-readvertise during close */
/* Set by ble_uart_close_async() when its worker task is in flight,
 * cleared by the worker just before it exits. uninstall() polls this
 * to drain a pending async close before tearing the port down. */
static volatile bool     s_closing;
static uint8_t           s_own_addr_type;

/* Resolved security policy. Computed once in install() from cfg.encrypted
 * + the per-feature overrides (cfg.sc/bonding/mitm/io_cap), then read
 * in the GAP event handler and the GATT-table builder.
 *   s_link_encrypted = true if any of {sc, bonding, mitm} resolved ON
 *                      → kick pairing on connect, require encryption on chars
 *   s_mitm_required  = resolved mitm bit
 *                      → require AUTHEN flag on chars (Just-Works peer cannot read/write) */
static bool              s_link_encrypted;
static bool              s_mitm_required;

/* Pending Passkey-Entry / Numeric-Comparison request awaiting an
 * application reply via ble_uart_passkey_reply / ble_uart_compare_reply.
 *
 *   s_pending_io_conn   — conn_handle the SM is asking about, or
 *                         BLE_HS_CONN_HANDLE_NONE if no request is in
 *                         flight. Set in PASSKEY_ACTION, cleared on
 *                         reply, on disconnect, and on enc_change.
 *   s_pending_io_action — BLE_SM_IOACT_INPUT or BLE_SM_IOACT_NUMCMP;
 *                         used to reject mismatched reply calls (e.g.
 *                         passkey_reply during NUMCMP).
 *
 * No FreeRTOS lock — both fields are written only from the host task,
 * and the reply API is the only outside reader. The reader takes a
 * local snapshot before injecting. */
static volatile uint16_t s_pending_io_conn   = BLE_HS_CONN_HANDLE_NONE;
static volatile uint8_t  s_pending_io_action;

/* Optional user-supplied advertising payloads. When `s_adv_data_len`
 * is non-zero we feed `s_adv_data` straight to ble_gap_adv_set_data;
 * the buffer always carries the 3-byte Flags AD element we built in
 * install() followed by the user's bytes. Same for the scan response
 * (no Flags element there). Zero length means "use the default
 * field-builder path in start_advertising()". */
static uint8_t  s_adv_data[3 + BLE_UART_ADV_DATA_MAX];
static uint8_t  s_adv_data_len;
static uint8_t  s_scan_rsp_data[BLE_UART_SCAN_RSP_DATA_MAX];
static uint8_t  s_scan_rsp_data_len;

static int  gap_event(struct ble_gap_event *event, void *arg);
static int  start_advertising(void);

/* ===== GATT (BLE UART service) ========================================= */

static int chr_access(uint16_t conn_handle, uint16_t attr_handle,
                      struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    switch (ctxt->op) {
    case BLE_GATT_ACCESS_OP_WRITE_CHR: {
        /* File-scope (BSS) — host task is single-threaded so no reentry. */
        static uint8_t s_rx_buf[RX_SCRATCH];

        uint16_t total = OS_MBUF_PKTLEN(ctxt->om);
        if (total > sizeof(s_rx_buf)) {
            ESP_LOGW(TAG, "rx oversize: %u > %u, rejecting",
                     (unsigned)total, (unsigned)sizeof(s_rx_buf));
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }
        uint16_t copied = 0;
        int rc = ble_hs_mbuf_to_flat(ctxt->om, s_rx_buf, total, &copied);
        if (rc != 0) {
            return BLE_ATT_ERR_UNLIKELY;
        }
        if (s_cfg.ble_uart_on_rx != NULL && copied > 0) {
            s_cfg.ble_uart_on_rx(s_rx_buf, copied);
        }
        return 0;
    }
    case BLE_GATT_ACCESS_OP_READ_CHR:
        return BLE_ATT_ERR_READ_NOT_PERMITTED;
    default:
        return BLE_ATT_ERR_UNLIKELY;
    }
}

/* Encryption-required flag masks. NimBLE derives CCCD permissions from
 * NOTIFY_INDICATE_* (not from READ/WRITE_*), so notify-only chars need
 * the NOTIFY_INDICATE mask, not just the RW mask — otherwise an
 * unpaired central could subscribe and receive notifications over the
 * unencrypted link (see ble_gatts.c:ble_gatts_chr_clt_cfg_flags_from_chr_flags).
 *
 * The _ENC and _AUTHEN halves are split so an encrypted-but-unauthenticated
 * (Just Works) link still passes when mitm=OFF — _AUTHEN gates on the
 * link's authenticated bit which Just Works doesn't set. */
#define CHR_FLAG_RW_ENC          (BLE_GATT_CHR_F_READ_ENC | \
                                  BLE_GATT_CHR_F_WRITE_ENC)
#define CHR_FLAG_RW_AUTHEN       (BLE_GATT_CHR_F_READ_AUTHEN | \
                                  BLE_GATT_CHR_F_WRITE_AUTHEN)
#define CHR_FLAG_NOTIFY_ENC      (BLE_GATT_CHR_F_NOTIFY_INDICATE_ENC)
#define CHR_FLAG_NOTIFY_AUTHEN   (BLE_GATT_CHR_F_NOTIFY_INDICATE_AUTHEN)

static struct ble_gatt_chr_def s_chr_defs[3];
static struct ble_gatt_svc_def s_svc_defs[2];

static void build_gatt_table(bool link_enc, bool mitm)
{
    /* `ble_gatt_chr_flags` is uint32_t — match width here so the
     * 0x10000-and-above NOTIFY_INDICATE flags don't get truncated. */
    ble_gatt_chr_flags rw_enc     = 0;
    ble_gatt_chr_flags notify_enc = 0;
    if (link_enc) {
        rw_enc     |= CHR_FLAG_RW_ENC;
        notify_enc |= CHR_FLAG_NOTIFY_ENC;
        if (mitm) {
            rw_enc     |= CHR_FLAG_RW_AUTHEN;
            notify_enc |= CHR_FLAG_NOTIFY_AUTHEN;
        }
    }

    s_chr_defs[0] = (struct ble_gatt_chr_def){
        .uuid      = &s_chr_rx_uuid.u,
        .access_cb = chr_access,
        .flags     = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_NO_RSP | rw_enc,
    };
    s_chr_defs[1] = (struct ble_gatt_chr_def){
        .uuid       = &s_chr_tx_uuid.u,
        .access_cb  = chr_access,
        .flags      = BLE_GATT_CHR_F_NOTIFY | notify_enc,
        .val_handle = &s_tx_val_handle,
    };
    s_chr_defs[2] = (struct ble_gatt_chr_def){0};

    s_svc_defs[0] = (struct ble_gatt_svc_def){
        .type            = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid            = &s_svc_uuid.u,
        .characteristics = s_chr_defs,
    };
    s_svc_defs[1] = (struct ble_gatt_svc_def){0};
}

static int register_uart_gatt_svc(void)
{
    build_gatt_table(s_link_encrypted, s_mitm_required);

    int rc = ble_gatts_count_cfg(s_svc_defs);
    if (rc != 0) {
        ESP_LOGE(TAG, "ble_gatts_count_cfg rc=%d", rc);
        return xlate_rc(rc);
    }
    rc = ble_gatts_add_svcs(s_svc_defs);
    if (rc != 0) {
        ESP_LOGE(TAG, "ble_gatts_add_svcs rc=%d", rc);
        return xlate_rc(rc);
    }
    return BLE_UART_OK;
}

/* Re-queue std + UART svc defs after close().  Host stop + gatts_reset clear
 * the ATT table and free the svc-def pointer array; only re-adding UART would
 * leave GAP/GATT (and anything else added at install) off-air on the next
 * ble_hs_start().  ble_svc_*_init() is safe to recall on ESP-IDF: SYSINIT_ASSERT
 * is a no-op and gap name storage is not reallocated if already present. */
static int reregister_gatt_svcs_after_close(void)
{
#if NIMBLE_BLE_CONNECT
    ble_svc_gap_init();
#endif
    ble_svc_gatt_init();
    return register_uart_gatt_svc();
}

static void register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg)
{
    char buf[BLE_UUID_STR_LEN];
    switch (ctxt->op) {
    case BLE_GATT_REGISTER_OP_SVC:
        ESP_LOGD(TAG, "registered service %s handle=%d",
                 ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf), ctxt->svc.handle);
        break;
    case BLE_GATT_REGISTER_OP_CHR:
        ESP_LOGD(TAG, "registered chr %s def=%d val=%d",
                 ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
                 ctxt->chr.def_handle, ctxt->chr.val_handle);
        break;
    default:
        break;
    }
}

/* ===== TX ============================================================== */

int ble_uart_tx(const uint8_t *data, size_t len)
{
    /* Snapshot conn_handle once: a peer-A→peer-B disconnect+connect
     * race during a multi-chunk send could otherwise leak later chunks
     * to peer B (notify_custom doesn't gate on the CCCD subscription).
     * Stale handle → BLE_HS_ENOTCONN, we bail cleanly. */
    uint16_t conn_handle = s_conn_handle;
    if (conn_handle == BLE_HS_CONN_HANDLE_NONE) {
        return BLE_UART_ENOTCONN;
    }
    if (data == NULL || len == 0) {
        return BLE_UART_EINVAL;
    }

    uint16_t mtu   = ble_att_mtu(conn_handle);
    size_t   chunk = (mtu > 3) ? (size_t)(mtu - 3) : 20;

    size_t sent = 0;
    while (sent < len) {
        size_t n = len - sent;
        if (n > chunk) {
            n = chunk;
        }
        struct os_mbuf *om = ble_hs_mbuf_from_flat(data + sent, n);
        if (om == NULL) {
            return BLE_UART_ENOMEM;
        }
        int rc = ble_gatts_notify_custom(conn_handle, s_tx_val_handle, om);
        if (rc != 0) {
            ESP_LOGW(TAG, "notify failed: rc=%d", rc);
            /* Callee frees om on every failure path EXCEPT the
             * BLE_GATT_NOTIFY-disabled early-return (BLE_HS_ENOTSUP).
             * Freeing on any other rc would be a double free. */
            if (rc == BLE_HS_ENOTSUP) {
                os_mbuf_free_chain(om);
            }
            return xlate_rc(rc);
        }
        sent += n;
    }
    return BLE_UART_OK;
}

/* Best-effort snapshots; see header for threading caveat. */
bool ble_uart_is_connected(void)  { return s_conn_handle != BLE_HS_CONN_HANDLE_NONE; }
bool ble_uart_is_subscribed(void) { return s_subscribed; }

/* ===== Event dispatch ================================================= */

/* Forward a tagged event to the application callback. NULL-safe so all
 * call sites stay one-liners; runs on the NimBLE host task — caller
 * must keep the local `ble_uart_evt_t` alive across the call (we do
 * via stack/compound literal at each site). */
static void emit_evt(const ble_uart_evt_t *evt)
{
    if (s_cfg.on_event != NULL) {
        s_cfg.on_event(evt);
    }
}

/* ===== Advertising ==================================================== */

static int start_advertising(void)
{
    int rc;

    /* Two paths: raw-bytes (when the app provided its own payload)
     * and field-builder (default). Mixing is allowed — e.g. raw
     * adv_data + default scan_rsp.
     *
     * Default primary payload: Flags AD + Complete Local Name. The
     * 128-bit service UUID lives in the scan response (the 31-byte
     * primary packet can't hold name + 128-bit UUID together). */
    if (s_adv_data_len > 0) {
        rc = ble_gap_adv_set_data(s_adv_data, s_adv_data_len);
        if (rc != 0) {
            ESP_LOGE(TAG, "adv_set_data rc=%d", rc);
            return rc;
        }
    } else {
        const char *name     = s_dev_name;
        size_t      name_len = strlen(name);

        struct ble_hs_adv_fields adv = {
            .flags                  = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP,
            /* If no name was set, advertise without one (NimBLE accepts
             * NULL+0); the service UUID in scan rsp still identifies us. */
            .name                   = name_len > 0 ? (uint8_t *)name : NULL,
            .name_len               = name_len,
            .name_is_complete       = name_len > 0 ? 1 : 0,
        };
        rc = ble_gap_adv_set_fields(&adv);
        if (rc != 0) {
            ESP_LOGE(TAG, "adv_set_fields rc=%d (name too long?)", rc);
            return rc;
        }
    }

    if (s_scan_rsp_data_len > 0) {
        rc = ble_gap_adv_rsp_set_data(s_scan_rsp_data, s_scan_rsp_data_len);
        if (rc != 0) {
            ESP_LOGE(TAG, "adv_rsp_set_data rc=%d", rc);
            return rc;
        }
    } else {
        struct ble_hs_adv_fields rsp = {
            .uuids128             = &s_svc_uuid,
            .num_uuids128         = 1,
            .uuids128_is_complete = 1,
        };
        rc = ble_gap_adv_rsp_set_fields(&rsp);
        if (rc != 0) {
            ESP_LOGE(TAG, "adv_rsp_set_fields rc=%d", rc);
            return rc;
        }
    }

    struct ble_gap_adv_params params = {
        .conn_mode = BLE_GAP_CONN_MODE_UND,
        .disc_mode = BLE_GAP_DISC_MODE_GEN,
    };
    rc = ble_gap_adv_start(s_own_addr_type, NULL, BLE_HS_FOREVER,
                           &params, gap_event, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "adv_start rc=%d", rc);
        return rc;
    }
    /* "advertising as '<name>'" only makes sense when ble_uart owns
     * the primary payload — with a caller-supplied adv_data the name
     * the scanner sees is whatever bytes the caller put in there, not
     * s_dev_name (which is only exposed via the GAP-service Device
     * Name characteristic, post-connect). Pick the wording per path. */
    if (s_adv_data_len > 0 || s_scan_rsp_data_len > 0) {
        ESP_LOGI(TAG, "advertising with custom payload "
                      "(adv=%u B, scan_rsp=%u B; GAP-service name='%s')",
                 (unsigned)s_adv_data_len,
                 (unsigned)s_scan_rsp_data_len,
                 s_dev_name[0] ? s_dev_name : "<unset>");
    } else {
        ESP_LOGI(TAG, "advertising as '%s'",
                 s_dev_name[0] ? s_dev_name : "<no name>");
    }
    return 0;
}

/* ===== GAP event handler ============================================== */

static void show_passkey(uint32_t passkey)
{
    ESP_LOGW(TAG, "");
    ESP_LOGW(TAG, "    +-----------------------------+");
    ESP_LOGW(TAG, "    |  BLE PAIRING PASSKEY:       |");
    ESP_LOGW(TAG, "    |       %06" PRIu32 "                |", passkey);
    ESP_LOGW(TAG, "    +-----------------------------+");
    ESP_LOGW(TAG, "");
}

static int gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;

    switch (event->type) {

    case BLE_GAP_EVENT_CONNECT:
        ESP_LOGI(TAG, "connect %s status=%d handle=%d",
                 event->connect.status == 0 ? "ok" : "failed",
                 event->connect.status,
                 event->connect.conn_handle);
        if (event->connect.status == 0) {
            s_conn_handle = event->connect.conn_handle;
            s_subscribed  = false;
            /* Look up the peer's address; on first pair this equals
             * peer_ota_addr, on bonded reconnect this is the resolved
             * identity address. ble_gap_conn_find should never fail
             * for a just-arrived connect event, but guard anyway —
             * a zero-address payload is preferable to a stale stack
             * read. */
            ble_uart_evt_t e = { .id = BLE_UART_EVT_CONNECTED };
            struct ble_gap_conn_desc d;
            if (ble_gap_conn_find(event->connect.conn_handle, &d) == 0) {
                from_nimble_addr(&d.peer_id_addr, &e.connected.peer);
            }
            emit_evt(&e);
            /* Start pairing immediately (rather than lazily on the
             * first encrypted attribute access). Resolved policy:
             * any of {sc, bonding, mitm} ON → pairing required. */
            if (s_link_encrypted) {
                ble_gap_security_initiate(event->connect.conn_handle);
            }
        } else if (!s_shutting_down) {
            start_advertising();
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI(TAG, "disconnect reason=%d", event->disconnect.reason);
        s_conn_handle = BLE_HS_CONN_HANDLE_NONE;
        s_subscribed  = false;
        /* Drop any pending Passkey-Entry / NC reply; pairing was
         * cancelled along with the link. A stale value here would
         * make the next reply call inject into a closed conn. */
        s_pending_io_conn = BLE_HS_CONN_HANDLE_NONE;
        emit_evt(&(ble_uart_evt_t){
            .id = BLE_UART_EVT_DISCONNECTED,
            .disconnected = { .reason = event->disconnect.reason },
        });
        if (!s_shutting_down) {
            start_advertising();
        }
        return 0;

    case BLE_GAP_EVENT_CONN_UPDATE:
        ESP_LOGI(TAG, "conn_update status=%d", event->conn_update.status);
        return 0;

    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI(TAG, "adv_complete reason=%d", event->adv_complete.reason);
        /* Don't auto-restart while a connection is up. Undirected adv
         * auto-stops at the LL on connect (BT Core spec), and an
         * explicit start while connected would either fail (single-
         * conn build, the default) or accept a second peripheral
         * link we don't want to handle here. ADV_COMPLETE can still
         * arrive in connected state via NimBLE-internal cleanup
         * (e.g. resolving-list updates after bonding); ignore it. */
        if (!s_shutting_down && s_conn_handle == BLE_HS_CONN_HANDLE_NONE) {
            start_advertising();
        }
        return 0;

    case BLE_GAP_EVENT_ENC_CHANGE:
        /* Pairing has resolved one way or the other; clear any pending
         * Passkey-Entry / NC request so the next pairing starts fresh. */
        s_pending_io_conn = BLE_HS_CONN_HANDLE_NONE;
        if (ble_gap_conn_find(event->enc_change.conn_handle, &desc) == 0) {
            ESP_LOGI(TAG, "enc_change status=%d encrypted=%d authenticated=%d bonded=%d",
                     event->enc_change.status,
                     desc.sec_state.encrypted,
                     desc.sec_state.authenticated,
                     desc.sec_state.bonded);
            /* Dispatch on the actual sec_state, not the rc — bonded
             * reconnects can finish with status=BLE_HS_ETIMEOUT (13)
             * while encrypted=1 thanks to a benign race with the
             * peer's auto-encrypt; reporting that as PAIRING_FAILED
             * would be wrong. */
            if (desc.sec_state.encrypted) {
                emit_evt(&(ble_uart_evt_t){
                    .id = BLE_UART_EVT_LINK_SECURE,
                    .link_secure = {
                        .encrypted     = (bool)desc.sec_state.encrypted,
                        .authenticated = (bool)desc.sec_state.authenticated,
                        .bonded        = (bool)desc.sec_state.bonded,
                        .key_size      = (uint8_t)desc.sec_state.key_size,
                    },
                });
            } else {
                emit_evt(&(ble_uart_evt_t){
                    .id = BLE_UART_EVT_PAIRING_FAILED,
                    .pairing_failed = { .reason = event->enc_change.status },
                });
            }
        }
        return 0;

    case BLE_GAP_EVENT_REPEAT_PAIRING:
        /* Drop old keys + retry rather than reject. */
        if (ble_gap_conn_find(event->repeat_pairing.conn_handle, &desc) == 0) {
            ble_store_util_delete_peer(&desc.peer_id_addr);
        }
        return BLE_GAP_REPEAT_PAIRING_RETRY;

    case BLE_GAP_EVENT_PASSKEY_ACTION:
        switch (event->passkey.params.action) {
        case BLE_SM_IOACT_DISP: {
            /* Rejection sampling avoids the modulo bias of
             * `esp_random() % 1000000` (2^32 % 1e6 != 0). */
            const uint32_t passkey_max  = 1000000U;
            const uint32_t reject_above = UINT32_MAX -
                                          (UINT32_MAX % passkey_max);
            uint32_t r;
            do {
                r = esp_random();
            } while (r >= reject_above);
            struct ble_sm_io pkey = {
                .action  = BLE_SM_IOACT_DISP,
                .passkey = r % passkey_max,
            };
            /* Banner stays for backward compat with log-scraping
             * tests; on_event is additive. */
            show_passkey(pkey.passkey);
            emit_evt(&(ble_uart_evt_t){
                .id = BLE_UART_EVT_PASSKEY_DISPLAY,
                .passkey = { .passkey = pkey.passkey },
            });
            int rc = ble_sm_inject_io(event->passkey.conn_handle, &pkey);
            if (rc != 0) {
                ESP_LOGW(TAG, "ble_sm_inject_io(DISP) rc=%d", rc);
            }
            break;
        }

        case BLE_SM_IOACT_INPUT:
            /* Central displays a passkey, user reads it from there
             * and types it into our device. We can't inject anything
             * yet — wait for ble_uart_passkey_reply(). */
            ESP_LOGI(TAG, "passkey entry requested (conn=%d)",
                     event->passkey.conn_handle);
            s_pending_io_conn   = event->passkey.conn_handle;
            s_pending_io_action = BLE_SM_IOACT_INPUT;
            emit_evt(&(ble_uart_evt_t){ .id = BLE_UART_EVT_PASSKEY_REQUEST });
            break;

        case BLE_SM_IOACT_NUMCMP:
            /* Both sides should display the same 6-digit value; user
             * confirms match. The value is in `numcmp` (already a
             * decimal 0..999999, computed by the SM). */
            ESP_LOGI(TAG, "numeric compare %06" PRIu32 " (conn=%d)",
                     event->passkey.params.numcmp,
                     event->passkey.conn_handle);
            s_pending_io_conn   = event->passkey.conn_handle;
            s_pending_io_action = BLE_SM_IOACT_NUMCMP;
            emit_evt(&(ble_uart_evt_t){
                .id = BLE_UART_EVT_NUMERIC_COMPARE,
                .numeric_compare = { .passkey = event->passkey.params.numcmp },
            });
            break;

        case BLE_SM_IOACT_OOB:
            /* OOB plumbing is intentionally not exposed. Letting the
             * SM hang here would surface as a pairing timeout — log
             * loudly and let it. */
            ESP_LOGW(TAG, "OOB pairing requested but not implemented");
            break;

        default:
            ESP_LOGW(TAG, "unexpected passkey action %d",
                     event->passkey.params.action);
            break;
        }
        return 0;

    case BLE_GAP_EVENT_MTU:
        ESP_LOGI(TAG, "mtu=%d (conn=%d)",
                 event->mtu.value, event->mtu.conn_handle);
        return 0;

    case BLE_GAP_EVENT_SUBSCRIBE:
        ESP_LOGI(TAG, "subscribe attr=%d cur_notify=%d",
                 event->subscribe.attr_handle, event->subscribe.cur_notify);
        if (event->subscribe.attr_handle == s_tx_val_handle) {
            bool sub = (event->subscribe.cur_notify != 0);
            /* Edge-trigger so a redundant CCCD write (same value
             * twice) doesn't fire two SUBSCRIBED events. */
            if (sub != s_subscribed) {
                s_subscribed = sub;
                emit_evt(&(ble_uart_evt_t){
                    .id = BLE_UART_EVT_SUBSCRIBED,
                    .subscribed = { .subscribed = sub },
                });
            }
        }
        return 0;

    default:
        return 0;
    }
}

/* ===== Pairing replies ================================================ */

/* Shared body for both reply APIs — looks at the pending state, builds
 * the matching ble_sm_io payload, and injects it. `expected_action` is
 * BLE_SM_IOACT_INPUT for passkey_reply and BLE_SM_IOACT_NUMCMP for
 * compare_reply; calling the wrong API for the in-flight request
 * returns ENOTCONN (treated as "no such request waiting"). */
static int do_pairing_reply(uint8_t expected_action,
                            uint32_t passkey,
                            bool numcmp_accept)
{
    /* Snapshot the volatile fields once. The host task may clear them
     * at any moment (disconnect / enc_change), and we want a coherent
     * decision below. */
    uint16_t conn   = s_pending_io_conn;
    uint8_t  action = s_pending_io_action;
    if (conn == BLE_HS_CONN_HANDLE_NONE || action != expected_action) {
        return BLE_UART_ENOTCONN;
    }

    struct ble_sm_io io = { .action = expected_action };
    if (expected_action == BLE_SM_IOACT_INPUT) {
        io.passkey = passkey;
    } else { /* BLE_SM_IOACT_NUMCMP */
        io.numcmp_accept = numcmp_accept ? 1 : 0;
    }

    /* Clear pending BEFORE inject so a re-entrant on_event triggered
     * by inject_io doesn't see stale state. If inject fails the
     * request is gone anyway (the SM will time out from the central's
     * side), so leaving it cleared is the right move. */
    s_pending_io_conn = BLE_HS_CONN_HANDLE_NONE;

    int rc = ble_sm_inject_io(conn, &io);
    if (rc != 0) {
        ESP_LOGW(TAG, "ble_sm_inject_io(%s) rc=%d",
                 expected_action == BLE_SM_IOACT_INPUT ? "INPUT" : "NUMCMP",
                 rc);
        /* ENOTCONN from NimBLE means the conn vanished between the
         * snapshot and the inject — surface that to the caller as
         * such; everything else is a stack-internal failure. */
        return (rc == BLE_HS_ENOTCONN) ? BLE_UART_ENOTCONN : BLE_UART_EFAIL;
    }
    return BLE_UART_OK;
}

int ble_uart_passkey_reply(uint32_t passkey)
{
    if (passkey > 999999) {
        return BLE_UART_EINVAL;
    }
    return do_pairing_reply(BLE_SM_IOACT_INPUT, passkey, false);
}

int ble_uart_compare_reply(bool match)
{
    return do_pairing_reply(BLE_SM_IOACT_NUMCMP, 0, match);
}

/* ===== Host plumbing =================================================== */

static void on_reset(int reason)
{
    ESP_LOGE(TAG, "Resetting NimBLE state; reason=%d", reason);
}

static void on_sync(void)
{
    int rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);

    rc = ble_hs_id_infer_auto(0, &s_own_addr_type);
    if (rc != 0) {
        ESP_LOGE(TAG, "infer addr type rc=%d", rc);
        return;
    }

    uint8_t addr[6] = {0};
    ble_hs_id_copy_addr(s_own_addr_type, addr, NULL);
    ESP_LOGI(TAG, "addr=%02x:%02x:%02x:%02x:%02x:%02x",
             addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);

    start_advertising();
}

static void nimble_host_task(void *param)
{
    (void)param;
    ESP_LOGI(TAG, "BLE host task started");
    nimble_port_run();
    /* Self-delete instead of nimble_port_freertos_deinit(): do_close()'s
     * nimble_port_stop() returns once port_run() exits in this task, but
     * this task is still running. A quick ble_uart_open() may already have
     * updated the port layer's host_task_h; freertos_deinit() would
     * vTaskDelete(host_task_h) and kill the new host task. */
    vTaskDelete(NULL);
}

/* ===== Public lifecycle ================================================ */

/* Resolved view of cfg.encrypted + the per-feature overrides
 * (cfg.sc / cfg.bonding / cfg.mitm / cfg.io_cap). Used in install()
 * to drive both ble_hs_cfg.sm_* and the GATT-table builder. */
struct sec_policy {
    bool    sc;          /* LE Secure Connections */
    bool    bonding;     /* persist LTK in NVS */
    bool    mitm;        /* require authentication */
    bool    link_enc;    /* derived: sc || bonding || mitm */
    uint8_t sm_io_cap;   /* NimBLE BLE_HS_IO_* */
};

static int resolve_sec_policy(const ble_uart_config_t *cfg,
                              struct sec_policy *out)
{
    const ble_uart_security_t *sec = &cfg->security;

    /* Range-check the public enums up front. Accepting (say) a
     * dangling 99 here would propagate to ble_hs_cfg as garbage and
     * the SM would refuse pairing for non-obvious reasons. */
    if ((unsigned)sec->sc      > BLE_UART_SEC_ON
     || (unsigned)sec->bonding > BLE_UART_SEC_ON
     || (unsigned)sec->mitm    > BLE_UART_SEC_ON
     || (unsigned)sec->io_cap  > BLE_UART_IO_CAP_KEYBOARD_DISPLAY) {
        return BLE_UART_EINVAL;
    }

    /* Input-capable IO caps fire BLE_UART_EVT_PASSKEY_REQUEST or
     * BLE_UART_EVT_NUMERIC_COMPARE and need an application reply via
     * ble_uart_passkey_reply / ble_uart_compare_reply. With on_event
     * NULL the caller would never see the request and pairing would
     * silently stall until the SM times out — fail synchronously.
     *
     * This checks the *configured* io_cap, not the value resolved
     * below. AUTO and DISPLAY_ONLY are excluded on purpose: AUTO with
     * mitm=ON becomes DisplayOnly; Passkey Display (BLE_SM_IOACT_DISP)
     * is satisfied inside gap_event via ble_sm_inject_io with no app
     * reply. BLE_UART_EVT_PASSKEY_DISPLAY is additive when on_event is
     * set; with on_event NULL emit_evt() drops it and pairing still
     * completes. */
    if (cfg->on_event == NULL
        && (sec->io_cap == BLE_UART_IO_CAP_KEYBOARD_ONLY
         || sec->io_cap == BLE_UART_IO_CAP_DISPLAY_YES_NO
         || sec->io_cap == BLE_UART_IO_CAP_KEYBOARD_DISPLAY)) {
        return BLE_UART_EINVAL;
    }

    /* AUTO inherits the bit from cfg.encrypted; OFF / ON override. */
    bool preset = cfg->encrypted;
    out->sc      = (sec->sc      == BLE_UART_SEC_AUTO) ? preset
                                                       : (sec->sc      == BLE_UART_SEC_ON);
    out->bonding = (sec->bonding == BLE_UART_SEC_AUTO) ? preset
                                                       : (sec->bonding == BLE_UART_SEC_ON);
    out->mitm    = (sec->mitm    == BLE_UART_SEC_AUTO) ? preset
                                                       : (sec->mitm    == BLE_UART_SEC_ON);
    /* "Link will be encrypted" iff the SM runs at all — and the SM
     * runs whenever any of these three bits is set. Pairing-without-
     * bonding still encrypts the live link with a session LTK. */
    out->link_enc = out->sc || out->bonding || out->mitm;

    /* IO capability: AUTO picks the minimum that lets the resolved
     * MITM bit succeed; the explicit values map straight to the
     * NimBLE BLE_HS_IO_* constants used by the SM. */
    switch (sec->io_cap) {
    case BLE_UART_IO_CAP_DISPLAY_ONLY:
        out->sm_io_cap = BLE_HS_IO_DISPLAY_ONLY;
        break;
    case BLE_UART_IO_CAP_NO_INPUT_OUTPUT:
        out->sm_io_cap = BLE_HS_IO_NO_INPUT_OUTPUT;
        break;
    case BLE_UART_IO_CAP_KEYBOARD_ONLY:
        out->sm_io_cap = BLE_HS_IO_KEYBOARD_ONLY;
        break;
    case BLE_UART_IO_CAP_DISPLAY_YES_NO:
        out->sm_io_cap = BLE_HS_IO_DISPLAY_YESNO;
        break;
    case BLE_UART_IO_CAP_KEYBOARD_DISPLAY:
        out->sm_io_cap = BLE_HS_IO_KEYBOARD_DISPLAY;
        break;
    case BLE_UART_IO_CAP_AUTO:
    default:
        out->sm_io_cap = out->mitm ? BLE_HS_IO_DISPLAY_ONLY
                                   : BLE_HS_IO_NO_INPUT_OUTPUT;
        break;
    }

    /* Just Works (NoInputNoOutput) cannot satisfy MITM — the SM
     * would reject pairing in flight. Catch it synchronously here. */
    if (out->mitm && out->sm_io_cap == BLE_HS_IO_NO_INPUT_OUTPUT) {
        return BLE_UART_EINVAL;
    }
    return BLE_UART_OK;
}

int ble_uart_install(const ble_uart_config_t *cfg)
{
    if (s_installed) {
        ESP_LOGW(TAG, "ble_uart_install called twice; ignoring");
        return BLE_UART_EALREADY;
    }

    if (cfg != NULL) {
        s_cfg = *cfg;
    } else {
        memset(&s_cfg, 0, sizeof(s_cfg));
    }

    /* Validate device_name length up front. Beyond
     * BLE_UART_DEVICE_NAME_MAX the default-path advertising would
     * silently fail at adv_set_fields time; surfacing the error here
     * is much friendlier. strnlen with cap+1 also stops a missing-NUL
     * caller buffer from running into uninitialised memory. */
    if (s_cfg.device_name != NULL) {
        size_t nlen = strnlen(s_cfg.device_name, BLE_UART_DEVICE_NAME_MAX + 1);
        if (nlen > BLE_UART_DEVICE_NAME_MAX) {
            ESP_LOGE(TAG, "device_name too long: > %u bytes",
                     (unsigned)BLE_UART_DEVICE_NAME_MAX);
            memset(&s_cfg, 0, sizeof(s_cfg));
            return BLE_UART_EINVAL;
        }
    }

    /* Validate caller-supplied advertising payloads up front so an
     * oversized buffer fails the install instead of corrupting the
     * adv packet at start time (where errors only surface in logs).
     * (NULL + len>0 is also rejected — almost always a caller bug.) */
    if (s_cfg.adv_data_len > BLE_UART_ADV_DATA_MAX
        || (s_cfg.adv_data == NULL && s_cfg.adv_data_len > 0)) {
        ESP_LOGE(TAG, "bad adv_data: ptr=%p len=%u (max=%u)",
                 s_cfg.adv_data,
                 (unsigned)s_cfg.adv_data_len,
                 (unsigned)BLE_UART_ADV_DATA_MAX);
        memset(&s_cfg, 0, sizeof(s_cfg));
        return BLE_UART_EINVAL;
    }
    if (s_cfg.scan_rsp_data_len > BLE_UART_SCAN_RSP_DATA_MAX
        || (s_cfg.scan_rsp_data == NULL && s_cfg.scan_rsp_data_len > 0)) {
        ESP_LOGE(TAG, "bad scan_rsp_data: ptr=%p len=%u (max=%u)",
                 s_cfg.scan_rsp_data,
                 (unsigned)s_cfg.scan_rsp_data_len,
                 (unsigned)BLE_UART_SCAN_RSP_DATA_MAX);
        memset(&s_cfg, 0, sizeof(s_cfg));
        return BLE_UART_EINVAL;
    }

    /* Resolve cfg.encrypted + per-feature overrides into a flat
     * policy. Validation (out-of-range enums + impossible MITM/IO
     * combination) happens up front so install() rejects bad cfgs
     * synchronously, before any host-stack resources are allocated. */
    struct sec_policy pol;
    int srv = resolve_sec_policy(&s_cfg, &pol);
    if (srv != BLE_UART_OK) {
        ESP_LOGE(TAG, "bad security cfg: encrypted=%d sc=%d bonding=%d "
                      "mitm=%d io_cap=%d",
                 (int)s_cfg.encrypted,
                 (int)s_cfg.security.sc,      (int)s_cfg.security.bonding,
                 (int)s_cfg.security.mitm,    (int)s_cfg.security.io_cap);
        memset(&s_cfg, 0, sizeof(s_cfg));
        return srv;
    }
    s_link_encrypted = pol.link_enc;
    s_mitm_required  = pol.mitm;

    /* Copy raw payloads now (caller's pointers may not outlive install).
     * For adv_data we also prepend the 3-byte Flags AD ourselves —
     * the controller-visible Flags element is library-controlled and
     * not part of what the application owns. */
    s_adv_data_len      = 0;
    s_scan_rsp_data_len = 0;
    if (s_cfg.adv_data != NULL && s_cfg.adv_data_len > 0) {
        s_adv_data[0] = 0x02;   /* AD length */
        s_adv_data[1] = 0x01;   /* AD type:  Flags */
        s_adv_data[2] = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
        memcpy(s_adv_data + 3, s_cfg.adv_data, s_cfg.adv_data_len);
        s_adv_data_len = (uint8_t)(3 + s_cfg.adv_data_len);
    }
    if (s_cfg.scan_rsp_data != NULL && s_cfg.scan_rsp_data_len > 0) {
        memcpy(s_scan_rsp_data, s_cfg.scan_rsp_data, s_cfg.scan_rsp_data_len);
        s_scan_rsp_data_len = (uint8_t)s_cfg.scan_rsp_data_len;
    }
    /* Drop the pointers — install must not retain caller buffers. */
    s_cfg.adv_data      = NULL;
    s_cfg.scan_rsp_data = NULL;

    esp_err_t err = nimble_port_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "nimble_port_init rc=%d", err);
        return BLE_UART_EFAIL;
    }
#if MYNEWT_VAL(BLE_HS_AUTO_START)
    s_hs_auto_start_pending = true;
#endif
    /* From here every failure must `goto fail` so nimble_port_deinit()
     * runs — leaving the port allocated breaks the next install(). */

    ble_hs_cfg.reset_cb          = on_reset;
    ble_hs_cfg.sync_cb           = on_sync;
    ble_hs_cfg.store_status_cb   = ble_store_util_status_rr;
    ble_hs_cfg.gatts_register_cb = register_cb;

    /* Apply the resolved security policy. NimBLE checks sm_bonding
     * before consulting the key-distribution masks, so it's safe to
     * leave them set unconditionally — they're a no-op when bonding=0. */
    if (pol.link_enc) {
        ble_hs_cfg.sm_io_cap         = pol.sm_io_cap;
        ble_hs_cfg.sm_sc             = pol.sc      ? 1 : 0;
        ble_hs_cfg.sm_bonding        = pol.bonding ? 1 : 0;
        ble_hs_cfg.sm_mitm           = pol.mitm    ? 1 : 0;
        ble_hs_cfg.sm_our_key_dist   = BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID;
        ble_hs_cfg.sm_their_key_dist = BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID;
    } else {
        /* Fully plaintext: SM disabled, no keys exchanged. */
        ble_hs_cfg.sm_io_cap         = BLE_HS_IO_NO_INPUT_OUTPUT;
        ble_hs_cfg.sm_sc             = 0;
        ble_hs_cfg.sm_bonding        = 0;
        ble_hs_cfg.sm_mitm           = 0;
    }

#if NIMBLE_BLE_CONNECT
    ble_svc_gap_init();
#endif
    ble_svc_gatt_init();

    /* Cache the device name into our own buffer (caller's pointer may
     * not outlive this call; also avoids the GAP-service stub path
     * which returns NULL from ble_svc_gap_device_name()). */
    int rc = 0;
    if (s_cfg.device_name != NULL) {
        strncpy(s_dev_name, s_cfg.device_name, sizeof(s_dev_name) - 1);
        s_dev_name[sizeof(s_dev_name) - 1] = '\0';
        s_cfg.device_name = NULL;

        /* Best-effort: also set in the GAP service for peer reads.
         * Returns -1 on the stub path — fine, we already cached locally. */
        rc = ble_svc_gap_device_name_set(s_dev_name);
        if (rc != 0) {
            ESP_LOGI(TAG, "ble_svc_gap_device_name_set rc=%d (GAP service stubbed?)",
                     rc);
            rc = 0;
        }
    } else {
        s_dev_name[0] = '\0';
    }

    rc = register_uart_gatt_svc();
    if (rc != BLE_UART_OK) {
        goto fail;
    }
    s_gatts_needs_readd = false;

    /* Wire up the NVS bond store (requires CONFIG_BT_NIMBLE_NVS_PERSIST=y).
     * Done here — not in open() — so bond-management APIs
     * (ble_uart_get_bond_count / clear_bonds / remove_peer) work
     * between install and open, letting callers wipe stale bonds
     * before the first advertising window opens. */
    ble_store_config_init();

    s_installed = true;
    return BLE_UART_OK;

fail:
#if MYNEWT_VAL(BLE_HS_AUTO_START)
    s_hs_auto_start_pending = false;
#endif
    nimble_port_deinit();
    memset(&s_cfg, 0, sizeof(s_cfg));
    return xlate_rc(rc);
}

int ble_uart_open(void)
{
    if (!s_installed) {
        ESP_LOGE(TAG, "ble_uart_open before ble_uart_install");
        return BLE_UART_EINVAL;
    }
    if (s_opened) {
        ESP_LOGW(TAG, "ble_uart_open called twice; ignoring");
        return BLE_UART_EALREADY;
    }

    if (s_gatts_needs_readd) {
        int grc = reregister_gatt_svcs_after_close();
        if (grc != BLE_UART_OK) {
            return grc;
        }
        s_gatts_needs_readd = false;
    }

    /* Spawn host task, then queue host start. on_sync() starts advertising
     * once the controller sync completes.
     *
     * With BLE_HS_AUTO_START (default), install()'s nimble_port_init()
     * already queued a one-shot start event — open() must not sched_start()
     * again or ble_hs_start() returns BLE_HS_EALREADY and the host task
     * asserts. After close()'s nimble_port_stop() the host is OFF and no
     * AUTO_START event remains, so every later open() must sched_start(). */
    nimble_port_freertos_init(nimble_host_task);
#if MYNEWT_VAL(BLE_HS_AUTO_START)
    if (!s_hs_auto_start_pending) {
        ble_hs_sched_start();
    }
    s_hs_auto_start_pending = false;
#else
    ble_hs_sched_start();
#endif
    s_opened = true;
    return BLE_UART_OK;
}

/* Body of ble_uart_close(); also called directly by the close-async
 * worker, which has already latched s_closing itself. The public
 * wrapper below uses s_closing to reject a sync close that races
 * with an in-flight async close. */
static int do_close(void)
{
    if (!s_opened) {
        return BLE_UART_EALREADY;
    }

    /* Latch first so GAP events stop re-arming advertising. */
    s_shutting_down = true;

    /* Only stop adv if it's still running. Undirected adv auto-stops
     * at the LL on connect, so calling adv_stop while connected just
     * burns one HCI cmd that NimBLE answers with BLE_HS_EALREADY.
     * Mirrors the Bluedroid backend's `if (s_adv_active)` gate. */
    int rc = 0;
    if (ble_gap_adv_active()) {
        rc = ble_gap_adv_stop();
        if (rc != 0 && rc != BLE_HS_EALREADY) {
            ESP_LOGW(TAG, "adv_stop rc=%d", rc);
        }
    }

    /* Graceful disconnect: wait up to 500 ms for the disconnect event
     * so the peer sees a proper LL_TERMINATE_IND, not a controller-yank. */
    if (s_conn_handle != BLE_HS_CONN_HANDLE_NONE) {
        rc = ble_gap_terminate(s_conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        if (rc != 0 && rc != BLE_HS_EALREADY) {
            ESP_LOGW(TAG, "ble_gap_terminate rc=%d", rc);
        }
        for (int i = 0; i < 50 && s_conn_handle != BLE_HS_CONN_HANDLE_NONE; i++) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        if (s_conn_handle != BLE_HS_CONN_HANDLE_NONE) {
            ESP_LOGW(TAG, "disconnect timed out; tearing down anyway");
        }
    }

    /* nimble_port_stop() waits for port_run() to exit in the host task;
     * the host then vTaskDelete(NULL) — no join after stop returns. */
    rc = nimble_port_stop();
    if (rc != 0) {
        ESP_LOGE(TAG, "nimble_port_stop rc=%d", rc);
        s_shutting_down = false;
        return BLE_UART_EFAIL;
    }

    /* Host stop frees the svc-def pointer array; stale ATT rows can remain
     * until cleared.  Use the public ble_gatts_reset() only (no NimBLE
     * source edits) and re-queue svc defs on the next open(). */
    rc = ble_gatts_reset();
    if (rc != 0) {
        ESP_LOGW(TAG, "ble_gatts_reset rc=%d", rc);
    }
    s_tx_val_handle     = 0;
    s_gatts_needs_readd = true;

    s_conn_handle = BLE_HS_CONN_HANDLE_NONE;
    s_subscribed  = false;
    s_opened      = false;
    s_shutting_down = false;
    return BLE_UART_OK;
}

int ble_uart_close(void)
{
    /* If a ble_uart_close_async() worker is in flight, the close
     * sequence is already running on the worker's task — let it
     * finish rather than racing it from here. The worker drives
     * s_opened to false on its own, so the next sync close after
     * the worker drains will get the natural !s_opened EALREADY. */
    if (s_closing) {
        return BLE_UART_EALREADY;
    }
    return do_close();
}

/* ===== Async close ==================================================== */

/* Background worker spawned by ble_uart_close_async(). Lives just
 * long enough to run the synchronous close path (which itself can
 * block on the disconnect timeout and on nimble_port_stop()), then
 * fires the completion event and self-deletes.
 *
 * Spawned as a separate task — not a deferred ble_npl callout — so
 * that nimble_port_stop() can join the host task without us being
 * the host task. */
static void close_async_task(void *arg)
{
    (void)arg;

    /* Bypass the s_closing gate in ble_uart_close(): we ARE the
     * in-flight async close that gate is meant to protect against. */
    int rc = do_close();
    if (rc != BLE_UART_OK && rc != BLE_UART_EALREADY) {
        ESP_LOGW(TAG, "close_async: do_close rc=%d", rc);
    }

    /* Deliver CLOSED on the worker task. Applications must defer
     * ble_uart_uninstall() to another task (PORTING.md §5.3.2).
     * Concurrent uninstall() may clear s_cfg while we read on_event. */
    emit_evt(&(ble_uart_evt_t){
        .id = BLE_UART_EVT_CLOSED,
        .closed = { .status = rc },
    });

    s_closing = false;
    vTaskDelete(NULL);
}

int ble_uart_close_async(void)
{
    /* Same-state checks as the synchronous variant: nothing to close
     * if we never opened, and no point spawning a second worker if
     * the first hasn't drained yet. */
    if (!s_opened || s_closing) {
        return BLE_UART_EALREADY;
    }

    /* Latch BEFORE spawning so a racing caller (different task) sees
     * the in-flight state immediately and gets EALREADY. */
    s_closing = true;

    /* 3 KB is comfortably more than the close path uses (mostly small
     * GAP/HCI helpers + a 50×10ms vTaskDelay loop); bump if you wedge
     * a heavy on_event handler between adv_stop and CLOSED. */
    BaseType_t ok = xTaskCreate(close_async_task, "ble_close",
                                3072, NULL,
                                tskIDLE_PRIORITY + 2, NULL);
    if (ok != pdPASS) {
        s_closing = false;
        return BLE_UART_ENOMEM;
    }
    return BLE_UART_OK;
}

/* ===== Bond management ================================================ */

/* Public API uses big-endian bytes (bytes[0] = MSB) but NimBLE stores
 * addresses little-endian (val[0] = LSB). Caller must supply
 * BLE_UART_ADDR_TYPE_PUBLIC/RANDOM (0/1); the bond store keys on
 * those same identity types. */
static void to_nimble_addr(const ble_uart_addr_t *src, ble_addr_t *dst)
{
    dst->type = src->type;
    for (int i = 0; i < 6; i++) {
        dst->val[i] = src->bytes[5 - i];
    }
}

#if MYNEWT_VAL(BLE_STORE_MAX_BONDS) > 0
/* ble_store_util_bonded_peers enumerates OUR_SEC (unique peer_addr).
 * Heap-allocate the scratch buffer so callers on small-stack tasks are
 * safe regardless of CONFIG_BT_NIMBLE_MAX_BONDS. */
static int bonded_peers_unique_count(size_t *out_count)
{
    const int max_peers = MYNEWT_VAL(BLE_STORE_MAX_BONDS);
    ble_addr_t *peer_addrs = calloc((size_t)max_peers, sizeof(*peer_addrs));
    if (peer_addrs == NULL) {
        return BLE_UART_ENOMEM;
    }
    int num_peers = 0;
    int rc = ble_store_util_bonded_peers(peer_addrs, &num_peers, max_peers);
    free(peer_addrs);
    if (rc != 0) {
        ESP_LOGW(TAG, "ble_store_util_bonded_peers rc=%d", rc);
        return xlate_rc(rc);
    }
    *out_count = (size_t)num_peers;
    return BLE_UART_OK;
}
#endif

int ble_uart_get_bond_count(size_t *out_count)
{
    if (out_count == NULL || !s_installed) {
        return BLE_UART_EINVAL;
    }
#if MYNEWT_VAL(BLE_STORE_MAX_BONDS) > 0
    return bonded_peers_unique_count(out_count);
#else
    *out_count = 0;
    return BLE_UART_OK;
#endif
}

int ble_uart_get_bonded_peers(ble_uart_addr_t *out, size_t cap, size_t *out_count)
{
    if (out_count == NULL || !s_installed
        || (out == NULL && cap > 0)) {
        return BLE_UART_EINVAL;
    }

#if MYNEWT_VAL(BLE_STORE_MAX_BONDS) <= 0
    *out_count = 0;
    return BLE_UART_OK;
#else
    /* ble_store_util_bonded_peers enumerates OUR_SEC (unique peer_addr).
     * Size the buffer to BLE_STORE_MAX_BONDS — the compile-time cap —
     * not PEER_SEC/OUR_SEC raw entry counts (they can disagree). */
    const int max_peers = MYNEWT_VAL(BLE_STORE_MAX_BONDS);

    if (cap == 0) {
        return bonded_peers_unique_count(out_count);
    }

    int n_our = 0;
    int rc = ble_store_util_count(BLE_STORE_OBJ_TYPE_OUR_SEC, &n_our);
    if (rc != 0) {
        ESP_LOGW(TAG, "ble_store_util_count rc=%d", rc);
        return xlate_rc(rc);
    }
    if (n_our <= 0) {
        *out_count = 0;
        return BLE_UART_OK;
    }

    /* Bonded peers in NimBLE's native LE byte order. Heap-allocate to
     * keep the host task's stack untouched even when many peers exist. */
    ble_addr_t *tmp = calloc((size_t)max_peers, sizeof(*tmp));
    if (tmp == NULL) {
        return BLE_UART_ENOMEM;
    }
    int got = 0;
    rc = ble_store_util_bonded_peers(tmp, &got, max_peers);
    if (rc != 0) {
        ESP_LOGW(TAG, "ble_store_util_bonded_peers rc=%d", rc);
        free(tmp);
        return xlate_rc(rc);
    }

    /* Copy at most cap entries into the caller's buffer, flipping
     * NimBLE's LE byte order back to our public big-endian convention
     * and narrowing addr types to BLE_UART_ADDR_TYPE_* . */
    size_t to_copy = ((size_t)got < cap) ? (size_t)got : cap;
    for (size_t i = 0; i < to_copy; i++) {
        from_nimble_addr(&tmp[i], &out[i]);
    }
    free(tmp);
    *out_count = (size_t)got;
    return BLE_UART_OK;
#endif
}

int ble_uart_remove_peer(const ble_uart_addr_t *peer)
{
    if (peer == NULL || !s_installed) {
        return BLE_UART_EINVAL;
    }
    ble_addr_t addr;
    to_nimble_addr(peer, &addr);
    int rc = ble_store_util_delete_peer(&addr);
    if (rc != 0) {
        ESP_LOGW(TAG, "ble_store_util_delete_peer rc=%d", rc);
        return xlate_rc(rc);
    }
    return BLE_UART_OK;
}

int ble_uart_clear_bonds(void)
{
    if (!s_installed) {
        return BLE_UART_EINVAL;
    }
    /* Wipes peer LTK + our LTK + persisted CCCD (and a few NimBLE
     * internal records). Doesn't touch our s_cfg or any other NVS
     * namespace. */
    int rc = ble_store_clear();
    if (rc != 0) {
        ESP_LOGW(TAG, "ble_store_clear rc=%d", rc);
        return xlate_rc(rc);
    }
    return BLE_UART_OK;
}

/* ===== Uninstall ====================================================== */

int ble_uart_uninstall(void)
{
    if (!s_installed) {
        return BLE_UART_EALREADY;
    }

    /* If a ble_uart_close_async() worker is still draining, poll s_closing
     * for up to ~5 s before touching shared state. On timeout, teardown
     * continues anyway — applications must follow PORTING.md §5.3.2 so
     * uninstall runs only after the worker has finished. */
    for (int i = 0; i < 500 && s_closing; i++) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    if (s_closing) {
        ESP_LOGW(TAG, "uninstall: close_async worker still running, "
                      "tearing down anyway");
    }

    /* Best-effort cleanup. Do NOT early-return on a per-step failure:
     * leaving s_installed=true with partially torn-down NimBLE state
     * makes the module unrecoverable (can't re-install, can't retry
     * uninstall cleanly). Mirror the Bluedroid backend: record the
     * first error, keep tearing down, and always wipe our state. */
    int first_rc = BLE_UART_OK;

    if (s_opened) {
        int rc = ble_uart_close();
        if (rc != BLE_UART_OK && rc != BLE_UART_EALREADY) {
            ESP_LOGE(TAG, "ble_uart_close rc=%d", rc);
            if (first_rc == BLE_UART_OK) {
                first_rc = rc;
            }
        }
    }

    /* Best-effort: even if port_deinit fails, wipe our state anyway —
     * otherwise s_installed stays true and the module is unrecoverable
     * (can't re-install, can't retry uninstall cleanly). */
    esp_err_t err = nimble_port_deinit();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "nimble_port_deinit rc=%d", err);
        if (first_rc == BLE_UART_OK) {
            first_rc = BLE_UART_EFAIL;
        }
    }

    memset(&s_cfg, 0, sizeof(s_cfg));
    s_dev_name[0]       = '\0';
    s_tx_val_handle     = 0;
    s_conn_handle       = BLE_HS_CONN_HANDLE_NONE;
    s_subscribed        = false;
    s_own_addr_type     = 0;
    s_shutting_down     = false;
    s_closing           = false;
    s_installed         = false;
    s_opened            = false;
    s_gatts_needs_readd = false;
#if MYNEWT_VAL(BLE_HS_AUTO_START)
    s_hs_auto_start_pending = false;
#endif
    s_adv_data_len      = 0;
    s_scan_rsp_data_len = 0;
    s_link_encrypted    = false;
    s_mitm_required     = false;
    s_pending_io_conn   = BLE_HS_CONN_HANDLE_NONE;
    s_pending_io_action = 0;
    return first_rc;
}

#endif /* CONFIG_BT_NIMBLE_ENABLED */
