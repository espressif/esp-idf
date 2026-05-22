/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 *
 * BLE UART — Bluedroid backend. Implements the lifecycle declared in
 * ble_uart.h on top of the Bluedroid host using the service-table API
 * (esp_ble_gatts_create_attr_tab). Active when
 * CONFIG_BT_BLUEDROID_ENABLED=y; otherwise ble_uart_nimble.c is used.
 *
 * Differences vs the NimBLE backend:
 *   - PREP/EXEC long-write reassembly is open-coded (NimBLE does it
 *     for us; Bluedroid hands raw fragments to the application).
 *   - MTU is cached locally in s_local_mtu — no Bluedroid query API.
 */

#include "sdkconfig.h"

#if CONFIG_BT_BLUEDROID_ENABLED

#include "ble_uart.h"

#include <inttypes.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_defs.h"
#include "esp_bt_device.h"
#include "esp_gap_ble_api.h"
#include "esp_gatt_common_api.h"
#include "esp_gatts_api.h"

static const char *TAG = "ble_uart";

/* ===== Backend constants ============================================== */

#define UART_APP_ID         0x55  /* arbitrary, must be unique per profile */
#define UART_SVC_INST_ID    0     /* single service instance */

/* RX scratch capacity. See ble_uart_nimble.c for rationale. */
#ifndef CONFIG_BLE_UART_RX_SCRATCH_SIZE
#define CONFIG_BLE_UART_RX_SCRATCH_SIZE     1024
#endif
#define RX_SCRATCH      CONFIG_BLE_UART_RX_SCRATCH_SIZE

/* ===== UUIDs =========================================================== */

const ble_uart_uuid128_t ble_uart_service_uuid = {
    .bytes = { 0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0,
               0x93, 0xf3, 0xa3, 0xb5, 0x01, 0x00, 0x40, 0x6e },
};

static const uint8_t s_svc_uuid_bytes[16] = {
    0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0,
    0x93, 0xf3, 0xa3, 0xb5, 0x01, 0x00, 0x40, 0x6e,
};
static const uint8_t s_chr_rx_uuid[16] = {
    0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0,
    0x93, 0xf3, 0xa3, 0xb5, 0x02, 0x00, 0x40, 0x6e,
};
static const uint8_t s_chr_tx_uuid[16] = {
    0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0,
    0x93, 0xf3, 0xa3, 0xb5, 0x03, 0x00, 0x40, 0x6e,
};

/* 16-bit UUIDs used in the attr table (declarations + CCCD). */
static const uint16_t s_pri_svc_uuid     = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t s_char_decl_uuid   = ESP_GATT_UUID_CHAR_DECLARE;
static const uint16_t s_cccd_uuid        = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;

static const uint8_t  s_char_prop_write_nr =
    ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_WRITE_NR;
static const uint8_t  s_char_prop_notify   = ESP_GATT_CHAR_PROP_BIT_NOTIFY;

static const uint8_t  s_cccd_default[2]    = {0x00, 0x00};

/* Placeholders for the value attributes — RX is RSP_BY_APP (we own
 * the response), TX is set on demand by send_indicate(). */
static uint8_t        s_rx_val_placeholder[1];
static uint8_t        s_tx_val_placeholder[1];

/* ===== Service-table indices ========================================== */

enum {
    NUS_IDX_SVC,
    NUS_IDX_RX_DECL,
    NUS_IDX_RX_VAL,
    NUS_IDX_TX_DECL,
    NUS_IDX_TX_VAL,
    NUS_IDX_TX_CCCD,
    NUS_IDX_NB,
};

/* ===== State =========================================================== */

static ble_uart_config_t s_cfg;
static esp_gatt_if_t     s_gatts_if          = ESP_GATT_IF_NONE;
static uint16_t          s_handles[NUS_IDX_NB];
/* Volatile: written from BTC task, polled from caller task. */
static volatile uint16_t s_conn_id           = 0xFFFF;     /* invalid sentinel */
static esp_bd_addr_t     s_remote_bda;
static uint16_t          s_local_mtu         = 23;         /* spec default */
static bool              s_subscribed;
static bool              s_installed;
static bool              s_opened;
static bool              s_shutting_down;
static volatile bool     s_attr_tab_ready;
static bool              s_adv_active;

/* Two-bit latch driving the "configure adv data + scan rsp before
 * start_advertising" sequence. start_advertising fires only when both
 * SET_COMPLETE_EVT events have cleared their bit. */
#define ADV_CONFIG_FLAG       (1 << 0)
#define SCAN_RSP_CONFIG_FLAG  (1 << 1)
static uint8_t           s_adv_config_done;

/* Long-write accumulator (Bluedroid doesn't reassemble for us). */
static uint8_t   s_rx_buf[RX_SCRATCH];
static uint16_t  s_prep_len;
static bool      s_prep_bad;

/* ===== Backend rc → public rc ========================================= */

static int xlate_rc(esp_err_t rc)
{
    switch (rc) {
    case ESP_OK:                return BLE_UART_OK;
    case ESP_ERR_INVALID_ARG:
    case ESP_ERR_INVALID_STATE: return BLE_UART_EINVAL;
    case ESP_ERR_NO_MEM:        return BLE_UART_ENOMEM;
    default:                    return BLE_UART_EFAIL;
    }
}

/* ===== GATT attribute table =========================================== */

/* Permissions are patched at install time depending on cfg.encrypted. */
static esp_gatts_attr_db_t s_nus_db[NUS_IDX_NB];

static void build_attr_table(bool encrypted)
{
    const esp_gatt_perm_t r_perm = encrypted
        ? (ESP_GATT_PERM_READ_ENC_MITM)
        : (ESP_GATT_PERM_READ);
    const esp_gatt_perm_t w_perm = encrypted
        ? (ESP_GATT_PERM_WRITE_ENC_MITM)
        : (ESP_GATT_PERM_WRITE);

    /* [SVC] primary service declaration */
    s_nus_db[NUS_IDX_SVC] = (esp_gatts_attr_db_t){
        .attr_control = {ESP_GATT_AUTO_RSP},
        .att_desc = {
            .uuid_length = ESP_UUID_LEN_16,
            .uuid_p      = (uint8_t *)&s_pri_svc_uuid,
            .perm        = ESP_GATT_PERM_READ,
            .max_length  = sizeof(s_svc_uuid_bytes),
            .length      = sizeof(s_svc_uuid_bytes),
            .value       = (uint8_t *)s_svc_uuid_bytes,
        },
    };

    /* [RX] characteristic declaration (Write + Write-no-rsp) */
    s_nus_db[NUS_IDX_RX_DECL] = (esp_gatts_attr_db_t){
        .attr_control = {ESP_GATT_AUTO_RSP},
        .att_desc = {
            .uuid_length = ESP_UUID_LEN_16,
            .uuid_p      = (uint8_t *)&s_char_decl_uuid,
            .perm        = ESP_GATT_PERM_READ,
            .max_length  = sizeof(s_char_prop_write_nr),
            .length      = sizeof(s_char_prop_write_nr),
            .value       = (uint8_t *)&s_char_prop_write_nr,
        },
    };

    /* [RX value] — RSP_BY_APP because Bluedroid can't auto-respond
     * to PREP_WRITE_REQ; we own the response path. */
    s_nus_db[NUS_IDX_RX_VAL] = (esp_gatts_attr_db_t){
        .attr_control = {ESP_GATT_RSP_BY_APP},
        .att_desc = {
            .uuid_length = ESP_UUID_LEN_128,
            .uuid_p      = (uint8_t *)s_chr_rx_uuid,
            .perm        = w_perm,
            .max_length  = RX_SCRATCH,
            .length      = sizeof(s_rx_val_placeholder),
            .value       = s_rx_val_placeholder,
        },
    };

    /* [TX] characteristic declaration (Notify only) */
    s_nus_db[NUS_IDX_TX_DECL] = (esp_gatts_attr_db_t){
        .attr_control = {ESP_GATT_AUTO_RSP},
        .att_desc = {
            .uuid_length = ESP_UUID_LEN_16,
            .uuid_p      = (uint8_t *)&s_char_decl_uuid,
            .perm        = ESP_GATT_PERM_READ,
            .max_length  = sizeof(s_char_prop_notify),
            .length      = sizeof(s_char_prop_notify),
            .value       = (uint8_t *)&s_char_prop_notify,
        },
    };

    /* [TX value] — NUS spec is notify-only, so the prop above doesn't
     * advertise READ; perm only matters if a client tries READ anyway. */
    s_nus_db[NUS_IDX_TX_VAL] = (esp_gatts_attr_db_t){
        .attr_control = {ESP_GATT_AUTO_RSP},
        .att_desc = {
            .uuid_length = ESP_UUID_LEN_128,
            .uuid_p      = (uint8_t *)s_chr_tx_uuid,
            .perm        = r_perm,
            .max_length  = RX_SCRATCH,
            .length      = sizeof(s_tx_val_placeholder),
            .value       = s_tx_val_placeholder,
        },
    };

    /* [TX CCCD] — central writes 0x0001 to subscribe, 0x0000 to stop.
     * w_perm enforces the encryption requirement. */
    s_nus_db[NUS_IDX_TX_CCCD] = (esp_gatts_attr_db_t){
        .attr_control = {ESP_GATT_AUTO_RSP},
        .att_desc = {
            .uuid_length = ESP_UUID_LEN_16,
            .uuid_p      = (uint8_t *)&s_cccd_uuid,
            .perm        = ESP_GATT_PERM_READ | w_perm,
            .max_length  = sizeof(uint16_t),
            .length      = sizeof(s_cccd_default),
            .value       = (uint8_t *)s_cccd_default,
        },
    };
}

/* ===== Advertising ==================================================== */

static esp_ble_adv_data_t s_adv_data = {
    .set_scan_rsp        = false,
    .include_name        = true,
    .include_txpower     = true,
    .min_interval        = 0,
    .max_interval        = 0,
    .appearance          = 0x00,
    .manufacturer_len    = 0,
    .p_manufacturer_data = NULL,
    .service_data_len    = 0,
    .p_service_data      = NULL,
    .service_uuid_len    = 0,
    .p_service_uuid      = NULL,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

/* Scan response = NUS UUID. Splitting it off the primary payload
 * leaves room for name + tx_pwr in the 31-byte primary. */
static esp_ble_adv_data_t s_scan_rsp_data = {
    .set_scan_rsp        = true,
    .include_name        = false,
    .include_txpower     = false,
    .service_uuid_len    = sizeof(s_svc_uuid_bytes),
    .p_service_uuid      = (uint8_t *)s_svc_uuid_bytes,
};

static esp_ble_adv_params_t s_adv_params = {
    .adv_int_min       = 0xa0,             /* 100 ms */
    .adv_int_max       = 0xa0,
    .adv_type          = ADV_TYPE_IND,
    .own_addr_type     = BLE_ADDR_TYPE_PUBLIC,
    .channel_map       = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static int start_advertising(void)
{
    esp_err_t rc = esp_ble_gap_start_advertising(&s_adv_params);
    if (rc != ESP_OK) {
        ESP_LOGE(TAG, "adv_start rc=%s", esp_err_to_name(rc));
        return xlate_rc(rc);
    }
    s_adv_active = true;
    return BLE_UART_OK;
}

/* Push adv data + scan response. start_advertising is triggered from
 * the matching SET_COMPLETE_EVT once both halves are realised.
 *
 * On a sync failure of either config call, the matching SET_COMPLETE_EVT
 * will NEVER fire — so we must wipe the latch entirely (not just clear
 * one bit) to avoid (a) advertising silently lost, or (b) the other
 * half's event firing start_advertising with a half-configured payload. */
static int configure_advertising(void)
{
    s_adv_config_done = ADV_CONFIG_FLAG | SCAN_RSP_CONFIG_FLAG;

    esp_err_t rc = esp_ble_gap_config_adv_data(&s_adv_data);
    if (rc != ESP_OK) {
        ESP_LOGE(TAG, "config_adv_data rc=%s", esp_err_to_name(rc));
        s_adv_config_done = 0;
        return xlate_rc(rc);
    }
    rc = esp_ble_gap_config_adv_data(&s_scan_rsp_data);
    if (rc != ESP_OK) {
        ESP_LOGE(TAG, "config_scan_rsp rc=%s", esp_err_to_name(rc));
        /* adv_data is in flight; its SET_COMPLETE_EVT will hit the
         * "stale event" branch in the GAP handler and bail. */
        s_adv_config_done = 0;
        return xlate_rc(rc);
    }
    return BLE_UART_OK;
}

/* ===== Passkey banner ================================================= */

static void show_passkey(uint32_t passkey)
{
    ESP_LOGW(TAG, "");
    ESP_LOGW(TAG, "    +-----------------------------+");
    ESP_LOGW(TAG, "    |  BLE PAIRING PASSKEY:       |");
    ESP_LOGW(TAG, "    |       %06" PRIu32 "                |", passkey);
    ESP_LOGW(TAG, "    +-----------------------------+");
    ESP_LOGW(TAG, "");
}

/* ===== Long-write accumulator ========================================= */

static void prep_reset(void)
{
    s_prep_len = 0;
    s_prep_bad = false;
}

/* Append one PREP_WRITE fragment. Bad fragments latch s_prep_bad; we
 * still answer OK to the client so its prepare-queue advances, then
 * reject the whole batch at EXEC time (BT Core Spec §3.4.6.3 model).
 *
 * Two checks, both required:
 *   1. Bounds — would otherwise memcpy past s_rx_buf.
 *   2. Strict ascending contiguity — the spec lets a hostile client
 *      send arbitrary offsets; if we accepted gaps, the bytes between
 *      the writes would leak from the previous transaction (s_rx_buf
 *      is BSS and prep_reset() doesn't zero it). All real-world BLE
 *      centrals send PREP fragments in strict order anyway. */
static void prep_append(uint16_t offset, const uint8_t *data, uint16_t len)
{
    if (s_prep_bad) {
        return;
    }
    if ((uint32_t)offset + len > sizeof(s_rx_buf)) {
        ESP_LOGW(TAG, "prep_write overflow: offset=%u len=%u cap=%u",
                 offset, len, (unsigned)sizeof(s_rx_buf));
        s_prep_bad = true;
        return;
    }
    if (offset != s_prep_len) {
        ESP_LOGW(TAG, "prep_write non-contiguous: offset=%u expected=%u",
                 offset, s_prep_len);
        s_prep_bad = true;
        return;
    }
    memcpy(s_rx_buf + offset, data, len);
    s_prep_len = offset + len;
}

/* ===== GATT event handler ============================================= */

/* Reply to a PREP_WRITE_REQ by echoing the value (BT spec mandates). */
static void send_prep_write_response(uint16_t conn_id, uint32_t trans_id,
                                     esp_gatt_status_t status,
                                     uint16_t handle, uint16_t offset,
                                     const uint8_t *value, uint16_t len)
{
    esp_gatt_rsp_t rsp = {0};
    rsp.attr_value.handle = handle;
    rsp.attr_value.offset = offset;
    rsp.attr_value.len    = (len > sizeof(rsp.attr_value.value))
                            ? sizeof(rsp.attr_value.value)
                            : len;
    if (rsp.attr_value.len) {
        memcpy(rsp.attr_value.value, value, rsp.attr_value.len);
    }
    esp_ble_gatts_send_response(s_gatts_if, conn_id, trans_id, status, &rsp);
}

static void handle_write(esp_ble_gatts_cb_param_t *p)
{
    uint16_t handle = p->write.handle;

    if (p->write.is_prep) {
        /* Two-tier validation per BT Core Spec §3.4.6.1 / §3.4.6.3:
         *   - Attribute-level (wrong handle): error in the PREP_RSP.
         *   - Fragment-level (offset/oversize/non-contiguous): latched
         *     in s_prep_bad; PREP_RSP stays OK and the error is
         *     surfaced once at EXEC time. */
        esp_gatt_status_t status = ESP_GATT_OK;
        if (handle != s_handles[NUS_IDX_RX_VAL]) {
            status = ESP_GATT_INVALID_HANDLE;
        } else {
            prep_append(p->write.offset, p->write.value, p->write.len);
            /* status stays OK even if s_prep_bad got latched. */
        }
        if (p->write.need_rsp) {
            send_prep_write_response(p->write.conn_id, p->write.trans_id,
                                     status, handle, p->write.offset,
                                     p->write.value, p->write.len);
        }
        return;
    }

    /* Plain (non-prep) write. */
    esp_gatt_status_t status = ESP_GATT_OK;
    if (handle == s_handles[NUS_IDX_RX_VAL]) {
        if (s_cfg.ble_uart_on_rx != NULL && p->write.len > 0) {
            s_cfg.ble_uart_on_rx(p->write.value, p->write.len);
        }
    } else if (handle == s_handles[NUS_IDX_TX_CCCD]) {
        /* CCCD must be exactly 2 bytes (BT Core §3.3.3.3). */
        if (p->write.len != 2) {
            ESP_LOGW(TAG, "malformed CCCD write len=%u", p->write.len);
            status = ESP_GATT_INVALID_ATTR_LEN;
        } else {
            uint16_t cccd = (uint16_t)p->write.value[0]
                          | ((uint16_t)p->write.value[1] << 8);
            s_subscribed = (cccd & 0x0001) != 0;
            ESP_LOGI(TAG, "subscribe cccd=0x%04x sub=%d", cccd, s_subscribed);
        }
    }

    if (p->write.need_rsp) {
        esp_ble_gatts_send_response(s_gatts_if, p->write.conn_id,
                                    p->write.trans_id, status, NULL);
    }
}

static void handle_exec_write(esp_ble_gatts_cb_param_t *p)
{
    /* CANCEL (flag=0x00) must always succeed; EXEC (flag=0x01) returns
     * INVALID_ATTR_LEN if PREP latched an error. Either way, drop
     * the queue. */
    esp_gatt_status_t status = ESP_GATT_OK;

    if (p->exec_write.exec_write_flag == ESP_GATT_PREP_WRITE_EXEC) {
        if (s_prep_bad) {
            ESP_LOGW(TAG, "exec_write: rejecting bad prep buffer (len=%u)",
                     s_prep_len);
            status = ESP_GATT_INVALID_ATTR_LEN;
        } else if (s_cfg.ble_uart_on_rx != NULL && s_prep_len > 0) {
            s_cfg.ble_uart_on_rx(s_rx_buf, s_prep_len);
        }
    }
    prep_reset();
    esp_ble_gatts_send_response(s_gatts_if, p->exec_write.conn_id,
                                p->exec_write.trans_id, status, NULL);
}

static void gatts_profile_event_handler(esp_gatts_cb_event_t event,
                                        esp_gatt_if_t gatts_if,
                                        esp_ble_gatts_cb_param_t *param)
{
    switch (event) {

    case ESP_GATTS_REG_EVT:
        ESP_LOGI(TAG, "gatts reg status=%d app_id=%u gatts_if=%u",
                 param->reg.status, param->reg.app_id, gatts_if);
        if (param->reg.status != ESP_GATT_OK) {
            return;
        }
        s_gatts_if = gatts_if;
        if (s_cfg.device_name != NULL) {
            esp_ble_gap_set_device_name(s_cfg.device_name);
            /* Bluedroid copies internally; drop our pointer. */
            s_cfg.device_name = NULL;
        }
        esp_ble_gatts_create_attr_tab(s_nus_db, gatts_if,
                                      NUS_IDX_NB, UART_SVC_INST_ID);
        break;

    case ESP_GATTS_CREAT_ATTR_TAB_EVT:
        if (param->add_attr_tab.status != ESP_GATT_OK
            || param->add_attr_tab.num_handle != NUS_IDX_NB) {
            ESP_LOGE(TAG, "create_attr_tab failed status=%d num=%u (expected %u)",
                     param->add_attr_tab.status,
                     param->add_attr_tab.num_handle, NUS_IDX_NB);
            return;
        }
        memcpy(s_handles, param->add_attr_tab.handles, sizeof(s_handles));
        ESP_LOGI(TAG, "registered service svc_handle=%u rx=%u tx=%u cccd=%u",
                 s_handles[NUS_IDX_SVC], s_handles[NUS_IDX_RX_VAL],
                 s_handles[NUS_IDX_TX_VAL], s_handles[NUS_IDX_TX_CCCD]);
        esp_ble_gatts_start_service(s_handles[NUS_IDX_SVC]);
        s_attr_tab_ready = true;
        /* If open() was waiting for the table, kick adv config now.
         * On the BTC task — no caller to return errors to. */
        if (s_opened && !s_adv_active) {
            (void)configure_advertising();
        }
        break;

    case ESP_GATTS_WRITE_EVT:
        handle_write(param);
        break;

    case ESP_GATTS_EXEC_WRITE_EVT:
        handle_exec_write(param);
        break;

    case ESP_GATTS_MTU_EVT:
        s_local_mtu = param->mtu.mtu;
        ESP_LOGI(TAG, "mtu=%u (conn=%u)", s_local_mtu, param->mtu.conn_id);
        break;

    case ESP_GATTS_CONNECT_EVT:
        /* Bluedroid only fires this on a successful physical link;
         * the param struct has no status field. */
        s_conn_id    = param->connect.conn_id;
        s_subscribed = false;
        /* Link-layer auto-stops undirected adv on connect (per BT spec)
         * but Bluedroid doesn't deliver ADV_STOP_COMPLETE_EVT for that
         * case — only for explicit stop_advertising(). Clear here so
         * close() doesn't try to stop an already-stopped advertiser. */
        s_adv_active = false;
        memcpy(s_remote_bda, param->connect.remote_bda, sizeof(s_remote_bda));
        ESP_LOGI(TAG, "connect conn_id=%u remote " ESP_BD_ADDR_STR,
                 s_conn_id, ESP_BD_ADDR_HEX(s_remote_bda));
        if (s_cfg.encrypted) {
            /* Kick pairing immediately rather than lazily on the
             * first encrypted attribute access. */
            esp_ble_set_encryption(param->connect.remote_bda,
                                   ESP_BLE_SEC_ENCRYPT_MITM);
        }
        break;

    case ESP_GATTS_DISCONNECT_EVT:
        ESP_LOGI(TAG, "disconnect conn_id=%u reason=0x%x",
                 param->disconnect.conn_id, param->disconnect.reason);
        s_conn_id    = 0xFFFF;
        s_subscribed = false;
        /* MTU is per-connection: reset to the spec default 23 so the
         * next peer (if it skips the MTU exchange) doesn't inherit
         * the previous link's negotiated value and overflow tx chunks. */
        s_local_mtu  = 23;
        prep_reset();
        if (!s_shutting_down) {
            start_advertising();
        }
        break;

    case ESP_GATTS_CONF_EVT:
        if (param->conf.status != ESP_GATT_OK) {
            ESP_LOGW(TAG, "notify confirm status=%d handle=%u",
                     param->conf.status, param->conf.handle);
        }
        break;

    case ESP_GATTS_READ_EVT:
        ESP_LOGD(TAG, "read on handle=%u (ignored)", param->read.handle);
        break;

    case ESP_GATTS_UNREG_EVT:
        s_gatts_if       = ESP_GATT_IF_NONE;
        s_attr_tab_ready = false;
        break;

    default:
        break;
    }
}

/* ===== GAP event handler ============================================== */

static void gap_event_handler(esp_gap_ble_cb_event_t event,
                              esp_ble_gap_cb_param_t *param)
{
    switch (event) {

    /* Both SET_COMPLETE handlers below need to:
     *   1) drop stale events (latch already zeroed by a sync failure);
     *   2) drop async failures (status != SUCCESS) and wipe the latch
     *      so the other half can't satisfy the "==0 → start_adv" check
     *      and launch advertising with a malformed payload. */
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        if (!(s_adv_config_done & ADV_CONFIG_FLAG)) {
            ESP_LOGD(TAG, "stale ADV_DATA_SET_COMPLETE_EVT ignored");
            break;
        }
        if (param->adv_data_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "adv_data set failed status=0x%x",
                     param->adv_data_cmpl.status);
            s_adv_config_done = 0;
            break;
        }
        s_adv_config_done &= ~ADV_CONFIG_FLAG;
        if (s_adv_config_done == 0 && s_opened && !s_shutting_down) {
            start_advertising();
        }
        break;

    case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
        if (!(s_adv_config_done & SCAN_RSP_CONFIG_FLAG)) {
            ESP_LOGD(TAG, "stale SCAN_RSP_DATA_SET_COMPLETE_EVT ignored");
            break;
        }
        if (param->scan_rsp_data_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "scan_rsp set failed status=0x%x",
                     param->scan_rsp_data_cmpl.status);
            s_adv_config_done = 0;
            break;
        }
        s_adv_config_done &= ~SCAN_RSP_CONFIG_FLAG;
        if (s_adv_config_done == 0 && s_opened && !s_shutting_down) {
            start_advertising();
        }
        break;

    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "adv_start failed status=0x%x",
                     param->adv_start_cmpl.status);
            s_adv_active = false;
            break;
        }
        ESP_LOGI(TAG, "advertising started");
        break;

    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        s_adv_active = false;
        ESP_LOGI(TAG, "advertising stopped");
        break;

    case ESP_GAP_BLE_PASSKEY_NOTIF_EVT:
        show_passkey(param->ble_security.key_notif.passkey);
        break;

    case ESP_GAP_BLE_AUTH_CMPL_EVT: {
        esp_ble_auth_cmpl_t *a = &param->ble_security.auth_cmpl;
        if (a->success) {
            ESP_LOGI(TAG, "pairing ok auth_mode=0x%x", a->auth_mode);
        } else {
            ESP_LOGW(TAG, "pairing failed reason=0x%x", a->fail_reason);
        }
        break;
    }

    case ESP_GAP_BLE_SEC_REQ_EVT:
        esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);
        break;

    case ESP_GAP_BLE_KEY_EVT:
        ESP_LOGD(TAG, "key event type=0x%x",
                 param->ble_security.ble_key.key_type);
        break;

    case ESP_GAP_BLE_NC_REQ_EVT:
        /* Numeric Comparison shouldn't fire with our DisplayOnly IO
         * (BT Core §2.3.5.1). Reject — accepting would flag the LTK
         * as MITM-authenticated without any user actually comparing
         * the digits, silently downgrading the security we asked for. */
        esp_ble_confirm_reply(param->ble_security.ble_req.bd_addr, false);
        break;

    default:
        break;
    }
}

/* ===== Public TX ====================================================== */

int ble_uart_tx(const uint8_t *data, size_t len)
{
    /* Snapshot conn_id and MTU once: a peer-A→peer-B disconnect+connect
     * race during a multi-chunk send could otherwise leak later chunks
     * to peer B (send_indicate doesn't gate on the CCCD subscription).
     * Stale conn_id → ESP_ERR_INVALID_ARG, we bail cleanly. Stale MTU
     * is harmless (chunks would only be smaller, never too big). */
    uint16_t conn_id  = s_conn_id;
    uint16_t mtu_snap = s_local_mtu;
    if (conn_id == 0xFFFF) {
        return BLE_UART_ENOTCONN;
    }
    if (data == NULL || len == 0) {
        return BLE_UART_EINVAL;
    }

    size_t chunk = (mtu_snap > 3) ? (size_t)(mtu_snap - 3) : 20;
    size_t sent  = 0;
    while (sent < len) {
        size_t n = len - sent;
        if (n > chunk) {
            n = chunk;
        }
        esp_err_t rc = esp_ble_gatts_send_indicate(s_gatts_if, conn_id,
                                                   s_handles[NUS_IDX_TX_VAL],
                                                   (uint16_t)n,
                                                   (uint8_t *)(data + sent),
                                                   false);  /* notify, not indicate */
        if (rc != ESP_OK) {
            ESP_LOGW(TAG, "send_indicate rc=%s", esp_err_to_name(rc));
            return xlate_rc(rc);
        }
        sent += n;
    }
    return BLE_UART_OK;
}

/* Best-effort snapshots; see header for threading caveat. */
bool ble_uart_is_connected(void)  { return s_conn_id != 0xFFFF; }
bool ble_uart_is_subscribed(void) { return s_subscribed; }

/* ===== Lifecycle ====================================================== */

static int configure_security(bool encrypted)
{
    esp_ble_auth_req_t auth_req = encrypted ? ESP_LE_AUTH_REQ_SC_MITM_BOND
                                            : ESP_LE_AUTH_NO_BOND;
    esp_ble_io_cap_t   iocap    = encrypted ? ESP_IO_CAP_OUT
                                            : ESP_IO_CAP_NONE;
    uint8_t key_size = 16;
    uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    uint8_t rsp_key  = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;

    /* Apply each SM param individually and bail on the first failure.
     * Do NOT collapse into `rc |= ...` — esp_err_t values aren't bit
     * flags (e.g. NO_MEM|INVALID_ARG = INVALID_STATE), and a single
     * ESP_FAIL=-1 sticks the accumulator. */
    struct { esp_ble_sm_param_t id; const void *val; uint8_t len; const char *tag; } params[] = {
        { ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(auth_req), "AUTHEN_REQ_MODE" },
        { ESP_BLE_SM_IOCAP_MODE,      &iocap,    sizeof(iocap),    "IOCAP_MODE"      },
        { ESP_BLE_SM_MAX_KEY_SIZE,    &key_size, sizeof(key_size), "MAX_KEY_SIZE"    },
        { ESP_BLE_SM_SET_INIT_KEY,    &init_key, sizeof(init_key), "SET_INIT_KEY"    },
        { ESP_BLE_SM_SET_RSP_KEY,     &rsp_key,  sizeof(rsp_key),  "SET_RSP_KEY"     },
    };
    for (size_t i = 0; i < sizeof(params) / sizeof(params[0]); i++) {
        esp_err_t rc = esp_ble_gap_set_security_param(params[i].id,
                                                      (void *)params[i].val,
                                                      params[i].len);
        if (rc != ESP_OK) {
            ESP_LOGE(TAG, "set_security_param[%s] rc=%s",
                     params[i].tag, esp_err_to_name(rc));
            return xlate_rc(rc);
        }
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

    /* Free BR/EDR controller RAM we won't use (no-op on BLE-only chips). */
    esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);

    /* Bring up controller + Bluedroid step by step. Track `stage` so
     * the fail label can unwind exactly the layers we allocated —
     * otherwise a partial install would leave the controller enabled
     * with no module state pointing at it, and the next install()
     * would fail with INVALID_STATE. */
    enum {
        STAGE_NONE = 0,
        STAGE_CTRL_INIT,
        STAGE_CTRL_ENABLE,
        STAGE_BLUEDROID_INIT,
        STAGE_BLUEDROID_ENABLE,
        STAGE_APP_REGISTER,
    } stage = STAGE_NONE;

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_err_t rc = esp_bt_controller_init(&bt_cfg);
    if (rc != ESP_OK) {
        ESP_LOGE(TAG, "controller_init rc=%s", esp_err_to_name(rc));
        goto fail;
    }
    stage = STAGE_CTRL_INIT;

    rc = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (rc != ESP_OK) {
        ESP_LOGE(TAG, "controller_enable rc=%s", esp_err_to_name(rc));
        goto fail;
    }
    stage = STAGE_CTRL_ENABLE;

    rc = esp_bluedroid_init();
    if (rc != ESP_OK) {
        ESP_LOGE(TAG, "bluedroid_init rc=%s", esp_err_to_name(rc));
        goto fail;
    }
    stage = STAGE_BLUEDROID_INIT;

    rc = esp_bluedroid_enable();
    if (rc != ESP_OK) {
        ESP_LOGE(TAG, "bluedroid_enable rc=%s", esp_err_to_name(rc));
        goto fail;
    }
    stage = STAGE_BLUEDROID_ENABLE;

    rc = esp_ble_gatts_register_callback(gatts_profile_event_handler);
    if (rc != ESP_OK) {
        ESP_LOGE(TAG, "gatts_register rc=%s", esp_err_to_name(rc));
        goto fail;
    }
    rc = esp_ble_gap_register_callback(gap_event_handler);
    if (rc != ESP_OK) {
        ESP_LOGE(TAG, "gap_register rc=%s", esp_err_to_name(rc));
        goto fail;
    }

    /* SM must be configured before app_register so any incoming
     * pairing request finds the right policy. */
    int srv = configure_security(s_cfg.encrypted);
    if (srv != BLE_UART_OK) {
        ESP_LOGE(TAG, "security config failed rc=%d", srv);
        rc = ESP_FAIL;
        goto fail;
    }

    /* Bluedroid has no compile-time MTU Kconfig (NimBLE has
     * CONFIG_BT_NIMBLE_ATT_PREFERRED_MTU); set it at runtime here so
     * both backends behave the same. Failure isn't fatal — we'd just
     * fall back to the spec default 23. */
    rc = esp_ble_gatt_set_local_mtu(512);
    if (rc != ESP_OK) {
        ESP_LOGW(TAG, "set_local_mtu(512) rc=%s; staying at default",
                 esp_err_to_name(rc));
    }

    build_attr_table(s_cfg.encrypted);

    rc = esp_ble_gatts_app_register(UART_APP_ID);
    if (rc != ESP_OK) {
        ESP_LOGE(TAG, "app_register rc=%s", esp_err_to_name(rc));
        goto fail;
    }
    stage = STAGE_APP_REGISTER;

    /* Wait briefly for CREAT_ATTR_TAB_EVT so the GATT table is in
     * place before open() asks the stack to advertise (otherwise the
     * peer would scan us with an empty database). 500 ms is well over
     * the typical bring-up time (~150 ms on ESP32-S3). */
    for (int i = 0; i < 50 && !s_attr_tab_ready; i++) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    if (!s_attr_tab_ready) {
        ESP_LOGW(TAG, "attr table not ready after 500 ms; open() will retry");
    }

    s_installed = true;
    return BLE_UART_OK;

fail:
    /* Unwind exactly the layers we brought up; ignore secondary errors. */
    switch (stage) {
    case STAGE_APP_REGISTER:
        if (s_gatts_if != ESP_GATT_IF_NONE) {
            esp_ble_gatts_app_unregister(s_gatts_if);
            s_gatts_if = ESP_GATT_IF_NONE;
        }
        /* fallthrough */
    case STAGE_BLUEDROID_ENABLE:
        esp_bluedroid_disable();
        /* fallthrough */
    case STAGE_BLUEDROID_INIT:
        esp_bluedroid_deinit();
        /* fallthrough */
    case STAGE_CTRL_ENABLE:
        esp_bt_controller_disable();
        /* fallthrough */
    case STAGE_CTRL_INIT:
        esp_bt_controller_deinit();
        /* fallthrough */
    case STAGE_NONE:
        break;
    }
    s_attr_tab_ready = false;
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
    s_opened = true;

    /* If the GATT table is up, kick adv-config now; otherwise
     * CREAT_ATTR_TAB_EVT will do it when it arrives. */
    if (s_attr_tab_ready) {
        int rc = configure_advertising();
        if (rc != BLE_UART_OK) {
            /* Roll back so the caller can retry open() without
             * going through close + uninstall. */
            s_opened = false;
            return rc;
        }
    }
    return BLE_UART_OK;
}

int ble_uart_close(void)
{
    if (!s_opened) {
        return BLE_UART_EALREADY;
    }

    /* Latch first so DISCONNECT_EVT skips the auto-readvertise. */
    s_shutting_down = true;

    if (s_adv_active) {
        esp_ble_gap_stop_advertising();
    }

    /* Graceful disconnect: ask the controller to send LL_TERMINATE,
     * then wait up to 500 ms for DISCONNECT_EVT to clear s_conn_id. */
    if (s_conn_id != 0xFFFF) {
        esp_err_t rc = esp_ble_gap_disconnect(s_remote_bda);
        if (rc != ESP_OK) {
            ESP_LOGW(TAG, "gap_disconnect rc=%s", esp_err_to_name(rc));
        }
        for (int i = 0; i < 50 && s_conn_id != 0xFFFF; i++) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        if (s_conn_id != 0xFFFF) {
            ESP_LOGW(TAG, "disconnect timed out; tearing down anyway");
        }
    }

    s_conn_id    = 0xFFFF;
    s_subscribed = false;
    /* Reset MTU here too in case DISCONNECT_EVT timed out above. */
    s_local_mtu  = 23;
    /* Defensively clear the long-write accumulator: if DISCONNECT_EVT
     * didn't fire within our wait window, prep_reset() in the event
     * handler never ran, and stale s_prep_len/s_prep_bad would poison
     * the first PREP_WRITE of the next session (offset 0 != stale len
     * latches s_prep_bad and rejects the whole batch at EXEC time). */
    prep_reset();
    s_opened     = false;
    s_shutting_down = false;
    return BLE_UART_OK;
}

int ble_uart_uninstall(void)
{
    if (!s_installed) {
        return BLE_UART_EALREADY;
    }

    /* Best-effort cleanup. We MUST NOT early-return on a per-step
     * failure: that would leave s_installed=true with the SDK in
     * some half-torn-down state, blocking both re-install and retry.
     * Mirror the install() goto-fail philosophy: record the first
     * error, keep tearing down, and always wipe our state. */
    esp_err_t first_err = ESP_OK;

    if (s_opened) {
        int rc = ble_uart_close();
        if (rc != BLE_UART_OK && rc != BLE_UART_EALREADY) {
            ESP_LOGE(TAG, "ble_uart_close rc=%d", rc);
            if (first_err == ESP_OK) first_err = rc;
        }
    }

    if (s_gatts_if != ESP_GATT_IF_NONE) {
        esp_err_t rc = esp_ble_gatts_app_unregister(s_gatts_if);
        if (rc != ESP_OK && first_err == ESP_OK) {
            ESP_LOGE(TAG, "gatts_app_unregister rc=%s", esp_err_to_name(rc));
            first_err = rc;
        }
        s_gatts_if = ESP_GATT_IF_NONE;
    }

    esp_err_t rc = esp_bluedroid_disable();
    if (rc != ESP_OK) {
        ESP_LOGE(TAG, "bluedroid_disable rc=%s", esp_err_to_name(rc));
        if (first_err == ESP_OK) first_err = rc;
    }
    rc = esp_bluedroid_deinit();
    if (rc != ESP_OK) {
        ESP_LOGE(TAG, "bluedroid_deinit rc=%s", esp_err_to_name(rc));
        if (first_err == ESP_OK) first_err = rc;
    }
    rc = esp_bt_controller_disable();
    if (rc != ESP_OK) {
        ESP_LOGE(TAG, "controller_disable rc=%s", esp_err_to_name(rc));
        if (first_err == ESP_OK) first_err = rc;
    }
    rc = esp_bt_controller_deinit();
    if (rc != ESP_OK) {
        ESP_LOGE(TAG, "controller_deinit rc=%s", esp_err_to_name(rc));
        if (first_err == ESP_OK) first_err = rc;
    }

    /* Wipe state unconditionally, even on partial failure. */
    memset(&s_cfg, 0, sizeof(s_cfg));
    memset(s_handles, 0, sizeof(s_handles));
    memset(s_remote_bda, 0, sizeof(s_remote_bda));
    s_conn_id         = 0xFFFF;
    s_local_mtu       = 23;
    s_subscribed      = false;
    s_installed       = false;
    s_opened          = false;
    s_shutting_down   = false;
    s_attr_tab_ready  = false;
    s_adv_active      = false;
    s_adv_config_done = 0;
    prep_reset();
    return first_err == ESP_OK ? BLE_UART_OK : xlate_rc(first_err);
}

#endif /* CONFIG_BT_BLUEDROID_ENABLED */
