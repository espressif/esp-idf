/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 *
 * BLE UART — NimBLE backend. Implements the lifecycle declared in
 * ble_uart.h on top of the NimBLE host. Active when
 * CONFIG_BT_NIMBLE_ENABLED=y; otherwise ble_uart_bluedroid.c is used.
 */

#include "sdkconfig.h"

#if CONFIG_BT_NIMBLE_ENABLED

#include "ble_uart.h"

#include <assert.h>
#include <inttypes.h>
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

/* Provided by NimBLE's `store/config` lib. */
extern void ble_store_config_init(void);

/* ===== UUIDs =========================================================== */

/* NUS UUIDs in little-endian byte order. */
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

/* RX scratch capacity. Tunable via menuconfig; fall back to 1024 if
 * Kconfig.projbuild isn't carried along when reusing this file. */
#ifndef CONFIG_BLE_UART_RX_SCRATCH_SIZE
#define CONFIG_BLE_UART_RX_SCRATCH_SIZE     1024
#endif
#define RX_SCRATCH      CONFIG_BLE_UART_RX_SCRATCH_SIZE

/* Cached device name. Avoids ble_svc_gap_device_name() which returns
 * NULL when CONFIG_BT_NIMBLE_GAP_SERVICE=n (would NULL-deref). 32B
 * covers the BLE 31-byte adv-payload limit + NUL. */
#define DEV_NAME_MAX     32

static ble_uart_config_t s_cfg;
static char              s_dev_name[DEV_NAME_MAX];
static uint16_t          s_tx_val_handle;
/* Volatile: written from NimBLE host task, polled from caller task. */
static volatile uint16_t s_conn_handle = BLE_HS_CONN_HANDLE_NONE;
static bool              s_subscribed;
static bool              s_installed;
static bool              s_opened;
static bool              s_shutting_down;   /* gates auto-readvertise during close */
static uint8_t           s_own_addr_type;

static int  gap_event(struct ble_gap_event *event, void *arg);
static int  start_advertising(void);

/* ===== GATT (NUS) ====================================================== */

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
 * unencrypted link (see ble_gatts.c:ble_gatts_chr_clt_cfg_flags_from_chr_flags). */
#define CHR_FLAG_RW_ENC      (BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_READ_AUTHEN | \
                              BLE_GATT_CHR_F_WRITE_ENC | BLE_GATT_CHR_F_WRITE_AUTHEN)
#define CHR_FLAG_NOTIFY_ENC  (BLE_GATT_CHR_F_NOTIFY_INDICATE_ENC | \
                              BLE_GATT_CHR_F_NOTIFY_INDICATE_AUTHEN)

static struct ble_gatt_chr_def s_chr_defs[3];
static struct ble_gatt_svc_def s_svc_defs[2];

static void build_gatt_table(bool encrypted)
{
    /* `ble_gatt_chr_flags` is uint32_t — match width here so the
     * 0x10000-and-above NOTIFY_INDICATE flags don't get truncated. */
    ble_gatt_chr_flags rw_enc     = encrypted ? CHR_FLAG_RW_ENC     : 0;
    ble_gatt_chr_flags notify_enc = encrypted ? CHR_FLAG_NOTIFY_ENC : 0;

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

static void register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg)
{
    char buf[BLE_UUID_STR_LEN];
    switch (ctxt->op) {
    case BLE_GATT_REGISTER_OP_SVC:
        ESP_LOGI(TAG, "registered service %s handle=%d",
                 ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf), ctxt->svc.handle);
        break;
    case BLE_GATT_REGISTER_OP_CHR:
        ESP_LOGI(TAG, "registered chr %s def=%d val=%d",
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

/* ===== Advertising ==================================================== */

static int start_advertising(void)
{
    /* 31-byte primary adv can't hold flags + tx_pwr + name + 128-bit
     * UUID together, so split: primary = flags+tx_pwr+name,
     * scan rsp = NUS UUID. */
    const char *name     = s_dev_name;
    size_t      name_len = strlen(name);

    struct ble_hs_adv_fields adv = {
        .flags                  = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP,
        .tx_pwr_lvl_is_present  = 1,
        .tx_pwr_lvl             = BLE_HS_ADV_TX_PWR_LVL_AUTO,
        /* If no name was set, advertise without one (NimBLE accepts
         * NULL+0); the NUS UUID in scan rsp still identifies us. */
        .name                   = name_len > 0 ? (uint8_t *)name : NULL,
        .name_len               = name_len,
        .name_is_complete       = name_len > 0 ? 1 : 0,
    };
    int rc = ble_gap_adv_set_fields(&adv);
    if (rc != 0) {
        ESP_LOGE(TAG, "adv_set_fields rc=%d (name too long?)", rc);
        return rc;
    }

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
    ESP_LOGI(TAG, "advertising as '%s'", name_len > 0 ? name : "<no name>");
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
            /* Start pairing immediately (rather than lazily on the
             * first encrypted attribute access). */
            if (s_cfg.encrypted) {
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
        if (!s_shutting_down) {
            start_advertising();
        }
        return 0;

    case BLE_GAP_EVENT_CONN_UPDATE:
        ESP_LOGI(TAG, "conn_update status=%d", event->conn_update.status);
        return 0;

    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI(TAG, "adv_complete reason=%d", event->adv_complete.reason);
        if (!s_shutting_down) {
            start_advertising();
        }
        return 0;

    case BLE_GAP_EVENT_ENC_CHANGE:
        if (ble_gap_conn_find(event->enc_change.conn_handle, &desc) == 0) {
            ESP_LOGI(TAG, "enc_change status=%d encrypted=%d authenticated=%d bonded=%d",
                     event->enc_change.status,
                     desc.sec_state.encrypted,
                     desc.sec_state.authenticated,
                     desc.sec_state.bonded);
        }
        return 0;

    case BLE_GAP_EVENT_REPEAT_PAIRING:
        /* Drop old keys + retry rather than reject. */
        if (ble_gap_conn_find(event->repeat_pairing.conn_handle, &desc) == 0) {
            ble_store_util_delete_peer(&desc.peer_id_addr);
        }
        return BLE_GAP_REPEAT_PAIRING_RETRY;

    case BLE_GAP_EVENT_PASSKEY_ACTION:
        if (event->passkey.params.action == BLE_SM_IOACT_DISP) {
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
            show_passkey(pkey.passkey);
            int rc = ble_sm_inject_io(event->passkey.conn_handle, &pkey);
            if (rc != 0) {
                ESP_LOGW(TAG, "ble_sm_inject_io rc=%d", rc);
            }
        } else {
            ESP_LOGW(TAG, "passkey action %d not handled (DisplayOnly only)",
                     event->passkey.params.action);
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
            s_subscribed = (event->subscribe.cur_notify != 0);
        }
        return 0;

    default:
        return 0;
    }
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
    ESP_LOGI(TAG, "BLE host task started");
    nimble_port_run();
    nimble_port_freertos_deinit();
}

/* ===== Public lifecycle ================================================ */

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

    esp_err_t err = nimble_port_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "nimble_port_init rc=%d", err);
        return BLE_UART_EFAIL;
    }
    /* From here every failure must `goto fail` so nimble_port_deinit()
     * runs — leaving the port allocated breaks the next install(). */

    ble_hs_cfg.reset_cb          = on_reset;
    ble_hs_cfg.sync_cb           = on_sync;
    ble_hs_cfg.store_status_cb   = ble_store_util_status_rr;
    ble_hs_cfg.gatts_register_cb = register_cb;

    /* Encrypted = LE Secure Connections + Bonding + MITM, DisplayOnly.
     * Plaintext = SM disabled. */
    if (s_cfg.encrypted) {
        ble_hs_cfg.sm_io_cap         = BLE_HS_IO_DISPLAY_ONLY;
        ble_hs_cfg.sm_sc             = 1;
        ble_hs_cfg.sm_bonding        = 1;
        ble_hs_cfg.sm_mitm           = 1;
        ble_hs_cfg.sm_our_key_dist   = BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID;
        ble_hs_cfg.sm_their_key_dist = BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID;
    } else {
        ble_hs_cfg.sm_io_cap         = BLE_HS_IO_NO_INPUT_OUTPUT;
        ble_hs_cfg.sm_sc             = 0;
        ble_hs_cfg.sm_bonding        = 0;
        ble_hs_cfg.sm_mitm           = 0;
    }

    ble_svc_gap_init();
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

    build_gatt_table(s_cfg.encrypted);

    rc = ble_gatts_count_cfg(s_svc_defs);
    if (rc != 0) {
        ESP_LOGE(TAG, "ble_gatts_count_cfg rc=%d", rc);
        goto fail;
    }
    rc = ble_gatts_add_svcs(s_svc_defs);
    if (rc != 0) {
        ESP_LOGE(TAG, "ble_gatts_add_svcs rc=%d", rc);
        goto fail;
    }

    s_installed = true;
    return BLE_UART_OK;

fail:
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

    /* NVS bond store (requires CONFIG_BT_NIMBLE_NVS_PERSIST=y). */
    ble_store_config_init();

    /* Spawn host task; on_sync starts advertising once controller is ready. */
    nimble_port_freertos_init(nimble_host_task);
    s_opened = true;
    return BLE_UART_OK;
}

int ble_uart_close(void)
{
    if (!s_opened) {
        return BLE_UART_EALREADY;
    }

    /* Latch first so GAP events stop re-arming advertising. */
    s_shutting_down = true;

    int rc = ble_gap_adv_stop();
    if (rc != 0 && rc != BLE_HS_EALREADY) {
        ESP_LOGW(TAG, "adv_stop rc=%d", rc);
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

    /* nimble_host_task self-cleans (port_freertos_deinit + delete) when
     * port_run returns, so no explicit join. */
    rc = nimble_port_stop();
    if (rc != 0) {
        ESP_LOGE(TAG, "nimble_port_stop rc=%d", rc);
        s_shutting_down = false;
        return BLE_UART_EFAIL;
    }

    s_conn_handle = BLE_HS_CONN_HANDLE_NONE;
    s_subscribed  = false;
    s_opened      = false;
    s_shutting_down = false;
    return BLE_UART_OK;
}

int ble_uart_uninstall(void)
{
    if (!s_installed) {
        return BLE_UART_EALREADY;
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
    s_dev_name[0]   = '\0';
    s_tx_val_handle = 0;
    s_conn_handle   = BLE_HS_CONN_HANDLE_NONE;
    s_subscribed    = false;
    s_own_addr_type = 0;
    s_shutting_down = false;
    s_installed     = false;
    s_opened        = false;
    return first_rc;
}

#endif /* CONFIG_BT_NIMBLE_ENABLED */
