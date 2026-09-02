/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file ble_esl_ap_lifecycle.c
 * @brief ESL AP Lifecycle & State Management
 *
 * Implements per-ESL state tracking, configuration writes, info reads,
 * OTS image transfer, PAST-based synchronization, and 60-minute timeout
 * monitoring for Synchronized → Unsynchronized and Unsynchronized → Unassociated
 * transitions.
 */

#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "host/ble_gap.h"

#include "ble_esl_ap.h"
#include "ble_esl_ap_int.h"
#include "ble_esl_common.h"
#include "ble_ots_client.h"

static const char *TAG = "esl_ap_lifecycle";

/* ========================== Constants ========================== */

/** Timeout check period: 60 seconds in microseconds */
#define LIFECYCLE_TIMER_PERIOD_US       (60ULL * 1000000ULL)

/** 60-minute timeout in microseconds */
#define LIFECYCLE_TIMEOUT_US            (60ULL * 60ULL * 1000000ULL)

/** OTS write mode: Truncate bit (bit 1) */
#define OTS_WRITE_MODE_TRUNCATE         0x02

/** PAwR advertising instance (assumed instance 0 for PAST set info) */
#define PAWR_ADV_INSTANCE               0

/* The key material struct is written to the peer as-is, so its packed layout
 * must match the 24-octet on-air Key Material characteristic value. */
_Static_assert(sizeof(ble_esl_key_material_t) == BLE_ESL_KEY_MATERIAL_SIZE,
               "ble_esl_key_material_t must match the on-air Key Material layout");

/* ========================== Internal Context Structures ========================== */

/**
 * @brief Context for chained configure writes
 */
typedef struct {
    uint16_t conn_handle;
    ble_esl_ap_esl_config_t config;
    uint16_t esl_addr;
} configure_ctx_t;

/**
 * @brief Context for chained info reads
 */
typedef struct {
    uint16_t conn_handle;
    ble_esl_ap_esl_info_t info;
    /* Dynamic buffers for read data */
    uint8_t *display_buf;
    uint16_t display_buf_len;
    uint8_t *image_buf;
    uint16_t image_buf_len;
    uint8_t *sensor_buf;
    uint16_t sensor_buf_len;
    uint8_t *led_buf;
    uint16_t led_buf_len;
} read_info_ctx_t;

/**
 * @brief Context for image transfer
 */
typedef struct {
    uint16_t conn_handle;
    uint8_t image_index;
    const uint8_t *data;
    uint32_t data_len;
    uint32_t sent_offset;   /*!< Number of bytes already sent */
    uint16_t otc_mtu;       /*!< Negotiated L2CAP OTC MTU (max SDU size) */
    bool truncate;
    bool otc_open;          /*!< true once the L2CAP OTC channel is open */
} image_transfer_ctx_t;

/**
 * @brief Context for synchronize procedure
 */
typedef struct {
    uint16_t conn_handle;
    uint16_t esl_addr;
    bool past_initiated;    /*!< True after PAST has been successfully initiated */
} synchronize_ctx_t;

/* ========================== Forward Declarations ========================== */

static void lifecycle_timeout_cb(void *arg);

/* Configure chain callbacks */
static void configure_write_addr_cb(uint16_t conn_handle, esp_err_t status,
                                    const uint8_t *data, uint16_t data_len,
                                    void *user_data);
static void configure_write_sync_key_cb(uint16_t conn_handle, esp_err_t status,
                                        const uint8_t *data, uint16_t data_len,
                                        void *user_data);
static void configure_write_resp_key_cb(uint16_t conn_handle, esp_err_t status,
                                        const uint8_t *data, uint16_t data_len,
                                        void *user_data);
static void configure_write_abs_time_cb(uint16_t conn_handle, esp_err_t status,
                                        const uint8_t *data, uint16_t data_len,
                                        void *user_data);

/* Read info chain callbacks */
static void read_info_display_cb(uint16_t conn_handle, esp_err_t status,
                                 const uint8_t *data, uint16_t data_len,
                                 void *user_data);
static void read_info_image_cb(uint16_t conn_handle, esp_err_t status,
                               const uint8_t *data, uint16_t data_len,
                               void *user_data);
static void read_info_sensor_cb(uint16_t conn_handle, esp_err_t status,
                                const uint8_t *data, uint16_t data_len,
                                void *user_data);
static void read_info_led_cb(uint16_t conn_handle, esp_err_t status,
                             const uint8_t *data, uint16_t data_len,
                             void *user_data);
static void read_info_pnp_id_cb(uint16_t conn_handle, esp_err_t status,
                                const uint8_t *data, uint16_t data_len,
                                void *user_data);
static void read_info_complete(read_info_ctx_t *ctx, esp_err_t status);

/* Read info chain: advance to next characteristic */
static void read_info_try_image(read_info_ctx_t *ctx);
static void read_info_try_sensor(read_info_ctx_t *ctx);
static void read_info_try_led(read_info_ctx_t *ctx);
static void read_info_try_pnp_id(read_info_ctx_t *ctx);

/* Synchronize callbacks */
static void synchronize_ecp_cb(uint16_t conn_handle, esp_err_t status,
                               const uint8_t *data, uint16_t data_len,
                               void *user_data);

/* OTS client event handler for image transfer (called from OTS callback dispatcher) */
void ble_esl_ap_lifecycle_handle_ots_event(uint16_t conn_id,
                                           int event,
                                           const void *param);

/* ========================== Static State ========================== */

/** Active image transfer context (one at a time) */
static image_transfer_ctx_t *s_image_ctx = NULL;

/** Active synchronize context (only one at a time; new requests are rejected while active) */
static synchronize_ctx_t *s_sync_ctx = NULL;

/* ========================== Lifecycle Init / Deinit ========================== */

esp_err_t ble_esl_ap_lifecycle_init(void)
{
    assert(g_esl_ap != NULL);

    esp_timer_create_args_t timer_args = {
        .callback = lifecycle_timeout_cb,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "esl_ap_timeout",
        .skip_unhandled_events = true,
    };

    esp_err_t ret = esp_timer_create(&timer_args, &g_esl_ap->timeout_timer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create lifecycle timer: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = esp_timer_start_periodic(g_esl_ap->timeout_timer, LIFECYCLE_TIMER_PERIOD_US);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start lifecycle timer: %s", esp_err_to_name(ret));
        esp_timer_delete(g_esl_ap->timeout_timer);
        g_esl_ap->timeout_timer = NULL;
        return ret;
    }

    ESP_LOGI(TAG, "Lifecycle sub-module initialized (60s check period)");
    return ESP_OK;
}

void ble_esl_ap_lifecycle_deinit(void)
{
    if (g_esl_ap == NULL) {
        return;
    }

    if (g_esl_ap->timeout_timer != NULL) {
        esp_err_t ret = esp_timer_stop_blocking(g_esl_ap->timeout_timer, portMAX_DELAY);
        if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
            ESP_LOGW(TAG, "Failed to stop lifecycle timer: %s", esp_err_to_name(ret));
        }
        esp_timer_delete(g_esl_ap->timeout_timer);
        g_esl_ap->timeout_timer = NULL;
    }

    /* Free any outstanding transfer context */
    if (s_image_ctx != NULL) {
        free(s_image_ctx);
        s_image_ctx = NULL;
    }
    if (s_sync_ctx != NULL) {
        free(s_sync_ctx);
        s_sync_ctx = NULL;
    }

    ESP_LOGI(TAG, "Lifecycle sub-module deinitialized");
}

/* ========================== Timeout Callback ========================== */

/**
 * @brief Periodic timer callback — checks for 60-minute timeouts
 *
 * Iterates the ESL tracking table:
 * - Synchronized ESLs: if last_sync_time_us is older than 60 min → Unsynchronized
 * - Unsynchronized ESLs: if unsync_entry_time_us is older than 60 min → Unassociated
 */
static void lifecycle_timeout_cb(void *arg)
{
    (void)arg;

    if (g_esl_ap == NULL) {
        return;
    }

    int64_t now_us = esp_timer_get_time();

    for (int i = 0; i < CONFIG_BLE_ESL_AP_MAX_ESLS; i++) {
        ble_esl_ap_esl_entry_t *esl = &g_esl_ap->esls[i];
        if (!esl->in_use) {
            continue;
        }

        if (esl->state == BLE_ESL_STATE_SYNCHRONIZED) {
            /* Check 60-minute sync timeout */
            if (esl->last_sync_time_us != 0 &&
                (now_us - esl->last_sync_time_us) >= (int64_t)LIFECYCLE_TIMEOUT_US) {
                ESP_LOGW(TAG, "ESL 0x%04X: sync timeout (60 min), transitioning to Unsynchronized",
                         esl->esl_addr);
                ble_esl_ap_update_esl_state(esl->esl_addr,
                                            BLE_ESL_STATE_UNSYNCHRONIZED);
            }
        } else if (esl->state == BLE_ESL_STATE_UNSYNCHRONIZED) {
            /* Check 60-minute reconnect timeout */
            if (esl->unsync_entry_time_us != 0 &&
                (now_us - esl->unsync_entry_time_us) >= (int64_t)LIFECYCLE_TIMEOUT_US) {
                ESP_LOGW(TAG, "ESL 0x%04X: unsync timeout (60 min), transitioning to Unassociated",
                         esl->esl_addr);
                ble_esl_ap_update_esl_state(esl->esl_addr,
                                            BLE_ESL_STATE_UNASSOCIATED);
            }
        }
    }
}

/* ========================== Update ESL State ========================== */

esp_err_t ble_esl_ap_update_esl_state(uint16_t esl_addr,
                                      ble_esl_state_t new_state)
{
    if (g_esl_ap == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    ble_esl_ap_esl_entry_t *esl = ble_esl_ap_find_esl(esl_addr);
    if (esl == NULL) {
        ESP_LOGW(TAG, "update_esl_state: ESL 0x%04X not found", esl_addr);
        return ESP_ERR_NOT_FOUND;
    }

    ble_esl_state_t old_state = esl->state;
    esl->state = new_state;

    ESP_LOGI(TAG, "ESL 0x%04X state: %d -> %d", esl_addr, old_state, new_state);

    /* Record timestamps for timeout tracking */
    if (new_state == BLE_ESL_STATE_UNSYNCHRONIZED) {
        esl->unsync_entry_time_us = esp_timer_get_time();
    }

    if (new_state == BLE_ESL_STATE_SYNCHRONIZED) {
        esl->last_sync_time_us = esp_timer_get_time();
        esl->unsync_entry_time_us = 0;
    }

    /* Fire state changed event to application */
    ble_esl_ap_state_changed_t evt = {
        .conn_handle = esl->conn_handle,
        .esl_addr = ble_esl_ap_addr_unpack(esl_addr),
        .old_state = old_state,
        .new_state = new_state,
    };

    if (g_esl_ap->app_cb != NULL) {
        g_esl_ap->app_cb(BLE_ESL_AP_EVT_STATE_CHANGED, &evt);
    }

    /* Handle Unassociated cleanup */
    if (new_state == BLE_ESL_STATE_UNASSOCIATED) {
        ESP_LOGI(TAG, "ESL 0x%04X: cleaning up tracking entry (Unassociated)", esl_addr);
        memset(esl, 0, sizeof(*esl));
        esl->in_use = false;
        esl->conn_handle = BLE_ESL_AP_CONN_HANDLE_INVALID;
    }

    return ESP_OK;
}

/* ========================== Get ESL State ========================== */

ble_esl_state_t ble_esl_ap_get_esl_state(ble_esl_address_t esl_addr)
{
    if (g_esl_ap == NULL) {
        return BLE_ESL_STATE_UNASSOCIATED;
    }

    ble_esl_ap_esl_entry_t *esl = ble_esl_ap_find_esl(BLE_ESL_AP_ADDR_PACK(esl_addr));
    if (esl == NULL) {
        return BLE_ESL_STATE_UNASSOCIATED;
    }

    return esl->state;
}

/* ========================== Configure ========================== */

esp_err_t ble_esl_ap_configure(uint16_t conn_handle,
                               const ble_esl_ap_esl_config_t *config)
{
    if (g_esl_ap == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    if (config == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (config->group_id > BLE_ESL_GROUP_ID_MAX) {
        ESP_LOGE(TAG, "configure: invalid group_id 0x%02X", config->group_id);
        return ESP_ERR_INVALID_ARG;
    }

    /* Validate connection handle */
    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(conn_handle);
    if (conn == NULL) {
        ESP_LOGE(TAG, "configure: conn_handle 0x%04X not found", conn_handle);
        return ESP_ERR_NOT_FOUND;
    }

    /* Allocate context for the chained write sequence */
    configure_ctx_t *ctx = calloc(1, sizeof(configure_ctx_t));
    if (ctx == NULL) {
        return ESP_ERR_NO_MEM;
    }

    ctx->conn_handle = conn_handle;
    memcpy(&ctx->config, config, sizeof(ble_esl_ap_esl_config_t));
    ctx->esl_addr = BLE_ESL_AP_MAKE_ADDR(config->esl_id, config->group_id);

    /* Store ESL address in connection context for cross-reference */
    conn->esl_addr = ctx->esl_addr;

    /* Find or create ESL tracking entry */
    ble_esl_ap_esl_entry_t *esl = ble_esl_ap_find_esl(ctx->esl_addr);
    if (esl == NULL) {
        /* Try to find by BLE address (may already exist from a prior association) */
        esl = ble_esl_ap_find_esl_by_ble_addr(conn->addr, conn->addr_type);
        if (esl == NULL) {
            esl = ble_esl_ap_alloc_esl();
            if (esl == NULL) {
                ESP_LOGE(TAG, "configure: ESL tracking table full");
                free(ctx);
                return ESP_ERR_NO_MEM;
            }
        }
    }

    /* Initialize/update the tracking entry */
    esl->in_use = true;
    esl->esl_addr = ctx->esl_addr;
    memcpy(esl->ble_addr, conn->addr, 6);
    esl->ble_addr_type = conn->addr_type;
    esl->conn_handle = conn_handle;
    esl->config_complete = false;
    esl->state = BLE_ESL_STATE_CONFIGURING;

    /* Store key material in the ESL tracking entry */
    esl->resp_key = config->resp_key;

    /* Set PAwR sync key (shared across all ESLs) */
    ble_esl_ap_pawr_set_sync_key(&config->ap_sync_key);

    /* Set per-ESL response key */
    esp_err_t ret = ble_esl_ap_pawr_set_response_key(ctx->esl_addr,
                                                     &config->resp_key);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "configure: failed to set response key for 0x%04X: %s",
                 ctx->esl_addr, esp_err_to_name(ret));
        /* Non-fatal — continue with configuration */
    }

    /* Step 1: Write ESL Address (2 bytes: esl_id + group_id) */
    uint8_t addr_data[2];
    addr_data[0] = config->esl_id;
    addr_data[1] = config->group_id & 0x7F;  /* RFU bit 7 = 0 */

    ret = ble_esl_ap_gatt_write(conn_handle, BLE_ESL_CHR_UUID_ESL_ADDRESS,
                                addr_data, sizeof(addr_data),
                                configure_write_addr_cb, ctx);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "configure: failed to initiate ESL Address write: %s",
                 esp_err_to_name(ret));
        ble_esl_ap_update_esl_state(ctx->esl_addr, BLE_ESL_STATE_UNASSOCIATED);
        free(ctx);
        return ret;
    }

    return ESP_OK;
}

/* ---- Configure chain callbacks ---- */

static void configure_write_addr_cb(uint16_t conn_handle, esp_err_t status,
                                    const uint8_t *data, uint16_t data_len,
                                    void *user_data)
{
    configure_ctx_t *ctx = (configure_ctx_t *)user_data;
    assert(ctx != NULL);

    if (status != ESP_OK) {
        ESP_LOGE(TAG, "configure: ESL Address write failed: %s", esp_err_to_name(status));
        goto fail;
    }

    ESP_LOGD(TAG, "configure: ESL Address written, writing AP Sync Key Material");

    /* Step 2: Write AP Sync Key Material (24 bytes: 16-byte key + 8-byte IV) */
    esp_err_t ret = ble_esl_ap_gatt_write(conn_handle, BLE_ESL_CHR_UUID_AP_SYNC_KEY,
                                           (const uint8_t *)&ctx->config.ap_sync_key,
                                           sizeof(ctx->config.ap_sync_key),
                                           configure_write_sync_key_cb, ctx);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "configure: failed to initiate AP Sync Key write: %s",
                 esp_err_to_name(ret));
        goto fail;
    }
    return;

fail:
    {
        ble_esl_ap_update_esl_state(ctx->esl_addr, BLE_ESL_STATE_UNASSOCIATED);
        ble_esl_ap_configured_t evt = {
            .conn_handle = conn_handle,
            .status = (status != ESP_OK) ? status : ESP_FAIL,
        };
        if (g_esl_ap != NULL && g_esl_ap->app_cb != NULL) {
            g_esl_ap->app_cb(BLE_ESL_AP_EVT_CONFIGURED, &evt);
        }
        free(ctx);
    }
}

static void configure_write_sync_key_cb(uint16_t conn_handle, esp_err_t status,
                                        const uint8_t *data, uint16_t data_len,
                                        void *user_data)
{
    configure_ctx_t *ctx = (configure_ctx_t *)user_data;
    assert(ctx != NULL);

    if (status != ESP_OK) {
        ESP_LOGE(TAG, "configure: AP Sync Key write failed: %s", esp_err_to_name(status));
        goto fail;
    }

    ESP_LOGD(TAG, "configure: AP Sync Key written, writing Response Key Material");

    /* Step 3: Write ESL Response Key Material (24 bytes) */
    esp_err_t ret = ble_esl_ap_gatt_write(conn_handle, BLE_ESL_CHR_UUID_RESP_KEY,
                                           (const uint8_t *)&ctx->config.resp_key,
                                           sizeof(ctx->config.resp_key),
                                           configure_write_resp_key_cb, ctx);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "configure: failed to initiate Response Key write: %s",
                 esp_err_to_name(ret));
        goto fail;
    }
    return;

fail:
    {
        ble_esl_ap_update_esl_state(ctx->esl_addr, BLE_ESL_STATE_UNASSOCIATED);
        ble_esl_ap_configured_t evt = {
            .conn_handle = conn_handle,
            .status = (status != ESP_OK) ? status : ESP_FAIL,
        };
        if (g_esl_ap != NULL && g_esl_ap->app_cb != NULL) {
            g_esl_ap->app_cb(BLE_ESL_AP_EVT_CONFIGURED, &evt);
        }
        free(ctx);
    }
}

static void configure_write_resp_key_cb(uint16_t conn_handle, esp_err_t status,
                                        const uint8_t *data, uint16_t data_len,
                                        void *user_data)
{
    configure_ctx_t *ctx = (configure_ctx_t *)user_data;
    assert(ctx != NULL);

    if (status != ESP_OK) {
        ESP_LOGE(TAG, "configure: Response Key write failed: %s", esp_err_to_name(status));
        goto fail;
    }

    ESP_LOGD(TAG, "configure: Response Key written, writing Absolute Time");

    /* Step 4: Write ESL Current Absolute Time (4 bytes from esp_timer_get_time) */
    int64_t now_us = esp_timer_get_time();
    /* Convert microseconds to milliseconds for ESL Absolute Time */
    uint32_t abs_time_ms = (uint32_t)(now_us / 1000ULL);
    uint8_t abs_time_data[4];
    abs_time_data[0] = (uint8_t)(abs_time_ms & 0xFF);
    abs_time_data[1] = (uint8_t)((abs_time_ms >> 8) & 0xFF);
    abs_time_data[2] = (uint8_t)((abs_time_ms >> 16) & 0xFF);
    abs_time_data[3] = (uint8_t)((abs_time_ms >> 24) & 0xFF);

    esp_err_t ret = ble_esl_ap_gatt_write(conn_handle, BLE_ESL_CHR_UUID_CURRENT_ABS_TIME,
                                           abs_time_data, sizeof(abs_time_data),
                                           configure_write_abs_time_cb, ctx);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "configure: failed to initiate Absolute Time write: %s",
                 esp_err_to_name(ret));
        goto fail;
    }
    return;

fail:
    {
        ble_esl_ap_update_esl_state(ctx->esl_addr, BLE_ESL_STATE_UNASSOCIATED);
        ble_esl_ap_configured_t evt = {
            .conn_handle = conn_handle,
            .status = (status != ESP_OK) ? status : ESP_FAIL,
        };
        if (g_esl_ap != NULL && g_esl_ap->app_cb != NULL) {
            g_esl_ap->app_cb(BLE_ESL_AP_EVT_CONFIGURED, &evt);
        }
        free(ctx);
    }
}

static void configure_write_abs_time_cb(uint16_t conn_handle, esp_err_t status,
                                        const uint8_t *data, uint16_t data_len,
                                        void *user_data)
{
    configure_ctx_t *ctx = (configure_ctx_t *)user_data;
    assert(ctx != NULL);

    if (status != ESP_OK) {
        ESP_LOGE(TAG, "configure: Absolute Time write failed: %s", esp_err_to_name(status));
        ble_esl_ap_update_esl_state(ctx->esl_addr, BLE_ESL_STATE_UNASSOCIATED);
    } else {
        ESP_LOGI(TAG, "configure: all 4 mandatory characteristics written for ESL 0x%04X",
                 ctx->esl_addr);

        /* Mark configuration as complete in the tracking entry */
        ble_esl_ap_esl_entry_t *esl = ble_esl_ap_find_esl(ctx->esl_addr);
        if (esl != NULL) {
            esl->config_complete = true;
        }
    }

    /* Fire BLE_ESL_AP_EVT_CONFIGURED */
    ble_esl_ap_configured_t evt = {
        .conn_handle = conn_handle,
        .status = status,
    };

    if (g_esl_ap != NULL && g_esl_ap->app_cb != NULL) {
        g_esl_ap->app_cb(BLE_ESL_AP_EVT_CONFIGURED, &evt);
    }

    free(ctx);
}

/* ========================== Read Info ========================== */

esp_err_t ble_esl_ap_read_info(uint16_t conn_handle)
{
    if (g_esl_ap == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(conn_handle);
    if (conn == NULL) {
        ESP_LOGE(TAG, "read_info: conn_handle 0x%04X not found", conn_handle);
        return ESP_ERR_NOT_FOUND;
    }

    /* Allocate context for the chained read sequence */
    read_info_ctx_t *ctx = calloc(1, sizeof(read_info_ctx_t));
    if (ctx == NULL) {
        return ESP_ERR_NO_MEM;
    }

    ctx->conn_handle = conn_handle;
    ctx->info.conn_handle = conn_handle;
    ctx->info.status = ESP_OK;
    ctx->info.max_image_index = 0xFF;  /* Default if char absent */

    /* Start with Display Information if present */
    if (conn->handles.display_info_handle != 0) {
        esp_err_t ret = ble_esl_ap_gatt_read(conn_handle, BLE_ESL_CHR_UUID_DISPLAY_INFO,
                                              read_info_display_cb, ctx);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "read_info: failed to read Display Info: %s",
                     esp_err_to_name(ret));
            free(ctx);
            return ret;
        }
    } else {
        /* Skip to Image Info */
        read_info_try_image(ctx);
    }

    return ESP_OK;
}

static void read_info_try_image(read_info_ctx_t *ctx)
{
    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(ctx->conn_handle);
    if (conn == NULL) {
        read_info_complete(ctx, ESP_ERR_NOT_FOUND);
        return;
    }

    if (conn->handles.image_info_handle != 0) {
        esp_err_t ret = ble_esl_ap_gatt_read(ctx->conn_handle, BLE_ESL_CHR_UUID_IMAGE_INFO,
                                              read_info_image_cb, ctx);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "read_info: failed to read Image Info: %s", esp_err_to_name(ret));
            read_info_try_sensor(ctx);
        }
    } else {
        read_info_try_sensor(ctx);
    }
}

static void read_info_try_sensor(read_info_ctx_t *ctx)
{
    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(ctx->conn_handle);
    if (conn == NULL) {
        read_info_complete(ctx, ESP_ERR_NOT_FOUND);
        return;
    }

    if (conn->handles.sensor_info_handle != 0) {
        esp_err_t ret = ble_esl_ap_gatt_read(ctx->conn_handle, BLE_ESL_CHR_UUID_SENSOR_INFO,
                                              read_info_sensor_cb, ctx);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "read_info: failed to read Sensor Info: %s", esp_err_to_name(ret));
            read_info_try_led(ctx);
        }
    } else {
        read_info_try_led(ctx);
    }
}

static void read_info_try_led(read_info_ctx_t *ctx)
{
    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(ctx->conn_handle);
    if (conn == NULL) {
        read_info_complete(ctx, ESP_ERR_NOT_FOUND);
        return;
    }

    if (conn->handles.led_info_handle != 0) {
        esp_err_t ret = ble_esl_ap_gatt_read(ctx->conn_handle, BLE_ESL_CHR_UUID_LED_INFO,
                                              read_info_led_cb, ctx);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "read_info: failed to read LED Info: %s", esp_err_to_name(ret));
            read_info_try_pnp_id(ctx);
        }
    } else {
        read_info_try_pnp_id(ctx);
    }
}

static void read_info_try_pnp_id(read_info_ctx_t *ctx)
{
    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(ctx->conn_handle);
    if (conn == NULL) {
        read_info_complete(ctx, ESP_ERR_NOT_FOUND);
        return;
    }

    if (conn->handles.pnp_id_handle != 0) {
        esp_err_t ret = ble_esl_ap_gatt_read(ctx->conn_handle, BLE_ESL_AP_PNP_ID_UUID,
                                              read_info_pnp_id_cb, ctx);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "read_info: failed to read PnP ID: %s", esp_err_to_name(ret));
            read_info_complete(ctx, ESP_OK);
        }
    } else {
        read_info_complete(ctx, ESP_OK);
    }
}

/* ---- Read info chain callbacks ---- */

static void read_info_display_cb(uint16_t conn_handle, esp_err_t status,
                                 const uint8_t *data, uint16_t data_len,
                                 void *user_data)
{
    read_info_ctx_t *ctx = (read_info_ctx_t *)user_data;
    assert(ctx != NULL);

    if (status == ESP_OK && data != NULL && data_len > 0) {
        /* Each display info is 5 bytes (ble_esl_display_info_t) */
        uint8_t num_displays = data_len / sizeof(ble_esl_display_info_t);
        if (num_displays > 0) {
            ctx->display_buf = malloc(data_len);
            if (ctx->display_buf != NULL) {
                memcpy(ctx->display_buf, data, data_len);
                ctx->display_buf_len = data_len;
                ctx->info.num_displays = num_displays;
                ctx->info.displays = (const ble_esl_display_info_t *)ctx->display_buf;
            }
        }
    } else if (status != ESP_OK) {
        ESP_LOGW(TAG, "read_info: Display Info read failed: %s", esp_err_to_name(status));
    }

    read_info_try_image(ctx);
}

static void read_info_image_cb(uint16_t conn_handle, esp_err_t status,
                               const uint8_t *data, uint16_t data_len,
                               void *user_data)
{
    read_info_ctx_t *ctx = (read_info_ctx_t *)user_data;
    assert(ctx != NULL);

    if (status == ESP_OK && data != NULL && data_len >= 1) {
        ctx->info.max_image_index = data[0];
    } else if (status != ESP_OK) {
        ESP_LOGW(TAG, "read_info: Image Info read failed: %s", esp_err_to_name(status));
    }

    read_info_try_sensor(ctx);
}

static void read_info_sensor_cb(uint16_t conn_handle, esp_err_t status,
                                const uint8_t *data, uint16_t data_len,
                                void *user_data)
{
    read_info_ctx_t *ctx = (read_info_ctx_t *)user_data;
    assert(ctx != NULL);

    if (status == ESP_OK && data != NULL && data_len > 0) {
        ctx->sensor_buf = malloc(data_len);
        if (ctx->sensor_buf != NULL) {
            memcpy(ctx->sensor_buf, data, data_len);
            ctx->sensor_buf_len = data_len;
            ctx->info.sensors = ctx->sensor_buf;
            ctx->info.sensors_len = data_len;

            /* Count sensors: iterate variable-length sensor info structures */
            uint8_t count = 0;
            uint16_t offset = 0;
            while (offset < data_len) {
                if (data[offset] == BLE_ESL_SENSOR_INFO_SIZE_SHORT) {
                    if (offset + BLE_ESL_SENSOR_INFO_SHORT_LEN > data_len) {
                        /* Truncated short sensor structure, stop parsing */
                        break;
                    }
                    offset += BLE_ESL_SENSOR_INFO_SHORT_LEN;
                } else if (data[offset] == BLE_ESL_SENSOR_INFO_SIZE_LONG) {
                    if (offset + BLE_ESL_SENSOR_INFO_LONG_LEN > data_len) {
                        /* Truncated long sensor structure, stop parsing */
                        break;
                    }
                    offset += BLE_ESL_SENSOR_INFO_LONG_LEN;
                } else {
                    /* Unknown format, skip remaining */
                    break;
                }
                count++;
            }
            ctx->info.num_sensors = count;
        }
    } else if (status != ESP_OK) {
        ESP_LOGW(TAG, "read_info: Sensor Info read failed: %s", esp_err_to_name(status));
    }

    read_info_try_led(ctx);
}

static void read_info_led_cb(uint16_t conn_handle, esp_err_t status,
                             const uint8_t *data, uint16_t data_len,
                             void *user_data)
{
    read_info_ctx_t *ctx = (read_info_ctx_t *)user_data;
    assert(ctx != NULL);

    if (status == ESP_OK && data != NULL && data_len > 0) {
        if (data_len > UINT8_MAX) {
            ESP_LOGW(TAG, "read_info: LED Info data_len %u exceeds max (255), clamping",
                     data_len);
            data_len = UINT8_MAX;
        }
        ctx->led_buf = malloc(data_len);
        if (ctx->led_buf != NULL) {
            memcpy(ctx->led_buf, data, data_len);
            ctx->led_buf_len = data_len;
            ctx->info.num_leds = (uint8_t)data_len;  /* 1 octet per LED */
            ctx->info.leds = ctx->led_buf;
        }
    } else if (status != ESP_OK) {
        ESP_LOGW(TAG, "read_info: LED Info read failed: %s", esp_err_to_name(status));
    }

    read_info_try_pnp_id(ctx);
}

static void read_info_pnp_id_cb(uint16_t conn_handle, esp_err_t status,
                                const uint8_t *data, uint16_t data_len,
                                void *user_data)
{
    read_info_ctx_t *ctx = (read_info_ctx_t *)user_data;
    assert(ctx != NULL);

    if (status == ESP_OK && data != NULL && data_len >= BLE_ESL_AP_PNP_ID_LEN) {
        ctx->info.has_pnp_id = true;
        memcpy(ctx->info.pnp_id, data, BLE_ESL_AP_PNP_ID_LEN);
    } else if (status != ESP_OK) {
        ESP_LOGW(TAG, "read_info: PnP ID read failed: %s", esp_err_to_name(status));
        /* Check if connection was lost */
        ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(conn_handle);
        if (conn == NULL) {
            read_info_complete(ctx, ESP_ERR_NOT_FOUND);
            return;
        }
    }

    read_info_complete(ctx, ESP_OK);
}

static void read_info_complete(read_info_ctx_t *ctx, esp_err_t status)
{
    ctx->info.status = status;

    /* Fire BLE_ESL_AP_EVT_ESL_INFO */
    if (g_esl_ap != NULL && g_esl_ap->app_cb != NULL) {
        g_esl_ap->app_cb(BLE_ESL_AP_EVT_ESL_INFO, &ctx->info);
    }

    /* Free dynamic buffers */
    free(ctx->display_buf);
    free(ctx->image_buf);
    free(ctx->sensor_buf);
    free(ctx->led_buf);
    free(ctx);
}

/* ========================== Transfer Image ========================== */

esp_err_t ble_esl_ap_transfer_image(const ble_esl_ap_image_transfer_params_t *params)
{
    if (g_esl_ap == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    if (params == NULL || params->data == NULL || params->data_len == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(params->conn_handle);
    if (conn == NULL) {
        ESP_LOGE(TAG, "transfer_image: conn_handle 0x%04X not found", params->conn_handle);
        return ESP_ERR_NOT_FOUND;
    }

    /* Only one image transfer at a time */
    if (s_image_ctx != NULL) {
        ESP_LOGE(TAG, "transfer_image: another transfer is in progress");
        return ESP_ERR_INVALID_STATE;
    }

    /* Allocate transfer context */
    s_image_ctx = calloc(1, sizeof(image_transfer_ctx_t));
    if (s_image_ctx == NULL) {
        return ESP_ERR_NO_MEM;
    }

    s_image_ctx->conn_handle = params->conn_handle;
    s_image_ctx->image_index = params->image_index;
    s_image_ctx->data = params->data;
    s_image_ctx->data_len = params->data_len;
    s_image_ctx->truncate = params->truncate;
    s_image_ctx->otc_mtu = 256;  /* Default L2CAP OTC MTU; updated on CHANNEL_OPEN */

    /* Step 1: Select the target object via OLCP Go To (over GATT).
     * Per the OTS spec, the Current Object must be selected *before* the
     * Object Transfer Channel is opened and OACP Write is issued. Opening the
     * OTC first breaks the procedure ordering — e.g. PTS rejects the premature
     * LE Credit Based Connection with "LE_PSM not supported" (BLE_HS_ENOTSUP).
     * The OLCP response handler opens the OTC; CHANNEL_OPEN then issues OACP
     * Write. */
    uint64_t object_id = BLE_ESL_OTS_OBJECT_ID_BASE + s_image_ctx->image_index;
    int rc = ble_ots_client_goto_object(params->conn_handle, object_id);
    if (rc != 0) {
        ESP_LOGE(TAG, "transfer_image: ble_ots_client_goto_object failed; rc=%d", rc);
        free(s_image_ctx);
        s_image_ctx = NULL;
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "transfer_image: OLCP Go To issued (image_index=%u)",
             params->image_index);
    return ESP_OK;
}

/**
 * @brief Terminate the current image transfer and release its resources.
 *
 * Closes the L2CAP Object Transfer Channel (if one was opened), reports the
 * final status to the application, and frees the transfer context. Safe to
 * call from any terminal point of the transfer state machine.
 *
 * The BLE_OTS_CLIENT_EVT_CHANNEL_CLOSED that results from closing the channel
 * is a no-op, because s_image_ctx is cleared here before it can be delivered.
 *
 * @param conn_id Connection the transfer ran on
 * @param status  Final transfer status reported to the application
 */
static void image_transfer_finish(uint16_t conn_id, esp_err_t status)
{
    if (s_image_ctx == NULL) {
        return;
    }

    ble_esl_ap_image_transferred_t evt = {
        .conn_handle = conn_id,
        .image_index = s_image_ctx->image_index,
        .status = status,
    };
    if (g_esl_ap != NULL && g_esl_ap->app_cb != NULL) {
        g_esl_ap->app_cb(BLE_ESL_AP_EVT_IMAGE_TRANSFERRED, &evt);
    }

    /* Release the Object Transfer Channel we opened so it is not leaked across
     * transfers. Skipped when no channel was opened (e.g. a failure before
     * CHANNEL_OPEN) or when it was already torn down (CHANNEL_CLOSED). */
    /* should not closed (Cause PTS case fail)
    if (s_image_ctx->otc_open) {
        int rc = ble_ots_client_close_channel(conn_id);
        if (rc != 0) {
            ESP_LOGW(TAG, "transfer_image: close_channel failed; rc=%d", rc);
        }
    } */

    free(s_image_ctx);
    s_image_ctx = NULL;
}

/**
 * @brief OTS client event handler for image transfer tracking
 *
 * Called from the OTS client callback dispatcher (registered during AP init).
 * Drives the transfer sequence: OLCP Go To (select object, in transfer_image)
 * → OLCP response (success → open OTC channel) → channel open (→ OACP Write)
 * → OACP response (Write success → send data) → data sent (track progress),
 * plus timeout/close/error handling. Every terminal path funnels through
 * image_transfer_finish(), which also closes the OTC channel.
 */
void ble_esl_ap_lifecycle_handle_ots_event(uint16_t conn_id,
                                           int event,
                                           const void *param)
{
    if (s_image_ctx == NULL || conn_id != s_image_ctx->conn_handle) {
        return;
    }

    switch (event) {

    case BLE_OTS_CLIENT_EVT_CHANNEL_OPEN: {
        /* L2CAP OTC channel is open — store negotiated MTU for chunking */
        const ble_ots_client_channel_info_t *ch_info = (const ble_ots_client_channel_info_t *)param;
        if (ch_info && ch_info->mtu > 0) {
            s_image_ctx->otc_mtu = ch_info->mtu;
        }
        s_image_ctx->otc_open = true;
        /* Object already selected via OLCP — now issue OACP Write Object Contents */
        ESP_LOGD(TAG, "transfer_image: OTC channel open (mtu=%u), issuing OACP Write (image_index=%u)",
                 s_image_ctx->otc_mtu, s_image_ctx->image_index);
        uint8_t mode = s_image_ctx->truncate ? OTS_WRITE_MODE_TRUNCATE : 0;
        int rc = ble_ots_client_write_object_content(conn_id, 0,
                                                     s_image_ctx->data_len, mode);
        if (rc != 0) {
            ESP_LOGE(TAG, "transfer_image: write_object_content failed; rc=%d", rc);
            image_transfer_finish(conn_id, ESP_FAIL);
        }
        break;
    }

    case BLE_OTS_CLIENT_EVT_OLCP_RESPONSE: {
        const ble_ots_client_olcp_response_t *olcp = (const ble_ots_client_olcp_response_t *)param;
        bool failed = false;

        if (olcp->result_code != 0x01) {  /* not Success */
            ESP_LOGE(TAG, "transfer_image: Go To failed, result_code=%d", olcp->result_code);
            failed = true;
        } else {
            /* Step 2: Object selected — open the L2CAP Object Transfer Channel.
             * If it was already opened by a previous transfer on this
             * connection (BLE_HS_EALREADY), skip straight to OACP Write;
             * otherwise wait for BLE_OTS_CLIENT_EVT_CHANNEL_OPEN. */
            ESP_LOGD(TAG, "transfer_image: Go To succeeded, opening OTC channel");
            int rc = ble_ots_client_open_channel(conn_id);
            if (rc == BLE_HS_EALREADY) {
                ESP_LOGD(TAG, "transfer_image: OTC already open, issuing OACP Write");
                s_image_ctx->otc_open = true;
                uint8_t mode = s_image_ctx->truncate ? OTS_WRITE_MODE_TRUNCATE : 0;
                rc = ble_ots_client_write_object_content(conn_id, 0,
                                                         s_image_ctx->data_len, mode);
                if (rc != 0) {
                    ESP_LOGE(TAG, "transfer_image: write_object_content failed; rc=%d", rc);
                    failed = true;
                }
            } else if (rc != 0) {
                ESP_LOGE(TAG, "transfer_image: open_channel failed; rc=%d", rc);
                failed = true;
            }
            /* else: wait for CHANNEL_OPEN to issue OACP Write */
        }

        if (failed) {
            image_transfer_finish(conn_id, ESP_FAIL);
        }
        break;
    }

    case BLE_OTS_CLIENT_EVT_OACP_RESPONSE: {
        const ble_ots_client_oacp_response_t *oacp = (const ble_ots_client_oacp_response_t *)param;
        if (oacp->request_opcode == 0x06 && oacp->result_code == 0x01) {
            /* OACP Write success — send the first chunk of data.
             * One send_data() call becomes one L2CAP SDU, so it must not
             * exceed the negotiated OTC MTU (max SDU size) or ble_l2cap_send
             * rejects it with BLE_HS_EBADDATA. Chunk by otc_mtu. */
            ESP_LOGD(TAG, "transfer_image: OACP Write accepted, sending data");
            uint16_t chunk_len = (s_image_ctx->data_len > s_image_ctx->otc_mtu) ?
                                  s_image_ctx->otc_mtu : (uint16_t)s_image_ctx->data_len;
            s_image_ctx->sent_offset = chunk_len;
            int rc = ble_ots_client_send_data(conn_id, s_image_ctx->data, chunk_len);
            if (rc != 0) {
                ESP_LOGE(TAG, "transfer_image: send_data failed; rc=%d", rc);
                image_transfer_finish(conn_id, ESP_FAIL);
            }
        } else if (oacp->result_code != 0x01) {
            ESP_LOGE(TAG, "transfer_image: OACP failed, opcode=%d result=%d",
                     oacp->request_opcode, oacp->result_code);
            image_transfer_finish(conn_id, ESP_FAIL);
        }
        break;
    }

    case BLE_OTS_CLIENT_EVT_DATA_SENT: {
        const ble_ots_client_data_sent_t *sent = (const ble_ots_client_data_sent_t *)param;
        if (sent->remaining == 0) {
            /* Transfer complete */
            ESP_LOGI(TAG, "transfer_image: image transfer complete for index %d",
                     s_image_ctx->image_index);
            image_transfer_finish(conn_id, ESP_OK);
        } else {
            /* Send the next chunk, capped at the negotiated OTC MTU (one SDU). */
            uint32_t offset = s_image_ctx->data_len - sent->remaining;
            uint16_t chunk_len = (sent->remaining > s_image_ctx->otc_mtu) ?
                                  s_image_ctx->otc_mtu : (uint16_t)sent->remaining;
            s_image_ctx->sent_offset = offset + chunk_len;
            ESP_LOGD(TAG, "transfer_image: sending next chunk at offset %lu, %u bytes "
                     "(%lu remaining)", (unsigned long)offset, chunk_len,
                     (unsigned long)sent->remaining);
            int rc = ble_ots_client_send_data(conn_id,
                                               s_image_ctx->data + offset,
                                               chunk_len);
            if (rc != 0) {
                ESP_LOGE(TAG, "transfer_image: send_data failed for chunk; rc=%d", rc);
                image_transfer_finish(conn_id, ESP_FAIL);
            }
        }
        break;
    }

    case BLE_OTS_CLIENT_EVT_TRANSFER_TIMEOUT:
    case BLE_OTS_CLIENT_EVT_CP_TIMEOUT: {
        ESP_LOGE(TAG, "transfer_image: timeout during transfer");
        image_transfer_finish(conn_id, ESP_ERR_TIMEOUT);
        break;
    }

    case BLE_OTS_CLIENT_EVT_CHANNEL_CLOSED: {
        /* If transfer was in progress, report failure. The channel is already
         * torn down, so clear otc_open to avoid a redundant close. */
        if (s_image_ctx != NULL && conn_id == s_image_ctx->conn_handle) {
            ESP_LOGW(TAG, "transfer_image: channel closed during transfer");
            s_image_ctx->otc_open = false;
            image_transfer_finish(conn_id, ESP_FAIL);
        }
        break;
    }

    default:
        break;
    }
}

/* ========================== Synchronize ========================== */

esp_err_t ble_esl_ap_synchronize(uint16_t conn_handle)
{
    if (g_esl_ap == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(conn_handle);
    if (conn == NULL) {
        ESP_LOGE(TAG, "synchronize: conn_handle 0x%04X not found", conn_handle);
        return ESP_ERR_NOT_FOUND;
    }

    /* Need a valid ESL address to proceed */
    ble_esl_ap_esl_entry_t *esl = ble_esl_ap_find_esl(conn->esl_addr);
    if (esl == NULL) {
        ESP_LOGE(TAG, "synchronize: ESL address 0x%04X not tracked for conn 0x%04X",
                 conn->esl_addr, conn_handle);
        return ESP_ERR_INVALID_STATE;
    }

    /* Only one synchronize procedure at a time */
    if (s_sync_ctx != NULL) {
        ESP_LOGE(TAG, "synchronize: another synchronization is in progress");
        return ESP_ERR_INVALID_STATE;
    }

    /* Allocate synchronize context */
    synchronize_ctx_t *ctx = calloc(1, sizeof(synchronize_ctx_t));
    if (ctx == NULL) {
        return ESP_ERR_NO_MEM;
    }

    ctx->conn_handle = conn_handle;
    ctx->esl_addr = conn->esl_addr;
    s_sync_ctx = ctx;

    /* Build Update Complete TLV: opcode 0x04, parameter = ESL_ID (1 byte) */
    uint8_t esl_id = BLE_ESL_AP_ADDR_ESL_ID(conn->esl_addr);
    uint8_t tlv_buf[BLE_ESL_TLV_MAX_SIZE];
    uint8_t tlv_len = 0;

    /* Update Complete opcode = 0x04, Length nibble = 0 (1 param byte: ESL_ID) */
    uint8_t opcode = BLE_ESL_TLV_OPCODE(BLE_ESL_CMD_UPDATE_COMPLETE, 0);
    esp_err_t ret = ble_esl_tlv_encode(opcode, &esl_id, 1, tlv_buf, &tlv_len);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "synchronize: TLV encode failed: %s", esp_err_to_name(ret));
        free(ctx);
        s_sync_ctx = NULL;
        return ret;
    }

    /*
     * Write Update Complete directly to the ECP characteristic.
     * Per ESL spec Section 3.9.2.5.2, the Update Complete command has no
     * response, so we use ble_esl_ap_gatt_write (which fires the callback
     * on write completion) instead of ble_esl_ap_ecp_write (which waits
     * for a notification that will never arrive).
     */
    ret = ble_esl_ap_gatt_write(conn_handle, BLE_ESL_CHR_UUID_ECP,
                                tlv_buf, tlv_len,
                                synchronize_ecp_cb, ctx);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "synchronize: ECP write failed: %s", esp_err_to_name(ret));
        free(ctx);
        s_sync_ctx = NULL;
        return ret;
    }

    ESP_LOGI(TAG, "synchronize: Update Complete sent for ESL 0x%04X", conn->esl_addr);
    return ESP_OK;
}

/**
 * @brief GATT write completion callback for Update Complete command
 *
 * On successful write, initiates PAST procedure immediately (no notification
 * expected per ESL spec). The ESL will synchronize to the AP's periodic
 * advertising and then disconnect.
 */
static void synchronize_ecp_cb(uint16_t conn_handle, esp_err_t status,
                               const uint8_t *data, uint16_t data_len,
                               void *user_data)
{
    synchronize_ctx_t *ctx = (synchronize_ctx_t *)user_data;
    assert(ctx != NULL);

    if (status != ESP_OK) {
        ESP_LOGE(TAG, "synchronize: Update Complete write failed: %s", esp_err_to_name(status));
        ble_esl_ap_synchronized_t evt = {
            .esl_addr = ble_esl_ap_addr_unpack(ctx->esl_addr),
            .status = status,
        };
        if (g_esl_ap != NULL && g_esl_ap->app_cb != NULL) {
            g_esl_ap->app_cb(BLE_ESL_AP_EVT_SYNCHRONIZED, &evt);
        }
        if (s_sync_ctx == ctx) {
            s_sync_ctx = NULL;
        }
        free(ctx);
        return;
    }

    ESP_LOGI(TAG, "synchronize: Update Complete written, initiating PAST");

    /*
     * Initiate PAST: transfer our own periodic advertising set info
     * to the connected ESL so it can synchronize to our PAwR train.
     * service_data = esl_addr for application-level context.
     */
    int rc = ble_gap_periodic_adv_sync_set_info(PAWR_ADV_INSTANCE,
                                                 conn_handle,
                                                 ctx->esl_addr);
    if (rc != 0) {
        ESP_LOGE(TAG, "synchronize: PAST failed; rc=%d", rc);
        ble_esl_ap_synchronized_t evt = {
            .esl_addr = ble_esl_ap_addr_unpack(ctx->esl_addr),
            .status = ESP_FAIL,
        };
        if (g_esl_ap != NULL && g_esl_ap->app_cb != NULL) {
            g_esl_ap->app_cb(BLE_ESL_AP_EVT_SYNCHRONIZED, &evt);
        }
        if (s_sync_ctx == ctx) {
            s_sync_ctx = NULL;
        }
        free(ctx);
        return;
    }

    /*
     * PAST initiated successfully. Mark the context so the disconnect handler
     * knows PAST was completed before the link dropped.
     */
    ctx->past_initiated = true;

    /*
     * The ESL will synchronize to our PAwR train
     * and then disconnect. The disconnect event handler (in ble_esl_ap_connection.c)
     * should detect that s_sync_ctx is active for this connection and:
     * 1. Call ble_esl_ap_update_esl_state(addr, SYNCHRONIZED)
     * 2. Fire BLE_ESL_AP_EVT_SYNCHRONIZED
     *
     * We keep s_sync_ctx alive until the disconnect event arrives.
     */
    ESP_LOGI(TAG, "synchronize: PAST initiated for ESL 0x%04X, awaiting disconnect",
             ctx->esl_addr);
}

/**
 * @brief Called by the connection module when a disconnect occurs for a
 *        connection that has an active synchronize procedure.
 *
 * This function checks if the disconnected connection matches the active
 * synchronize context and completes the synchronization procedure.
 *
 * Note: This is intended to be called from the disconnect handler in
 * ble_esl_ap_connection.c. If the connection module does not call this
 * directly, the lifecycle module can also detect it via the state tracking.
 */
void ble_esl_ap_lifecycle_handle_disconnect(uint16_t conn_handle)
{
    /* Step 1: Look up the ESL entry connected on this conn_handle and handle
     * link-loss state transitions for Configuring / Updating states.
     * Skip link-loss transitions if this disconnect belongs to an active
     * synchronize procedure — Step 2 will handle the state change cleanly. */
    bool is_sync_disconnect = (s_sync_ctx != NULL &&
                               s_sync_ctx->conn_handle == conn_handle);

    if (g_esl_ap != NULL) {
        for (int i = 0; i < CONFIG_BLE_ESL_AP_MAX_ESLS; i++) {
            ble_esl_ap_esl_entry_t *esl = &g_esl_ap->esls[i];
            if (!esl->in_use || esl->conn_handle != conn_handle) {
                continue;
            }

            /* Clear the connection handle */
            esl->conn_handle = BLE_ESL_AP_CONN_HANDLE_INVALID;

            if (!is_sync_disconnect) {
                if (esl->state == BLE_ESL_STATE_CONFIGURING) {
                    if (esl->config_complete) {
                        ESP_LOGI(TAG, "link-loss in Configuring (config complete) for ESL 0x%04X, "
                                 "transitioning to Unsynchronized", esl->esl_addr);
                        ble_esl_ap_update_esl_state(esl->esl_addr,
                                                    BLE_ESL_STATE_UNSYNCHRONIZED);
                    } else {
                        ESP_LOGI(TAG, "link-loss in Configuring (config incomplete) for ESL 0x%04X, "
                                 "transitioning to Unassociated", esl->esl_addr);
                        ble_esl_ap_update_esl_state(esl->esl_addr,
                                                    BLE_ESL_STATE_UNASSOCIATED);
                    }
                } else if (esl->state == BLE_ESL_STATE_UPDATING) {
                    ESP_LOGI(TAG, "link-loss in Updating for ESL 0x%04X, "
                             "transitioning to Unsynchronized", esl->esl_addr);
                    ble_esl_ap_update_esl_state(esl->esl_addr,
                                                BLE_ESL_STATE_UNSYNCHRONIZED);
                }
            }
            break;
        }
    }

    /* Step 2: Handle synchronize procedure completion if this disconnect
     * belongs to an active synchronize context. */
    if (s_sync_ctx != NULL && s_sync_ctx->conn_handle == conn_handle) {
        synchronize_ctx_t *ctx = s_sync_ctx;
        s_sync_ctx = NULL;

        if (ctx->past_initiated) {
            /* PAST was successfully initiated — complete synchronization */
            ESP_LOGI(TAG, "synchronize: disconnect received for ESL 0x%04X, completing sync",
                     ctx->esl_addr);

            esp_err_t ret = ble_esl_ap_update_esl_state(ctx->esl_addr,
                                                         BLE_ESL_STATE_SYNCHRONIZED);

            ble_esl_ap_synchronized_t evt = {
                .esl_addr = ble_esl_ap_addr_unpack(ctx->esl_addr),
                .status = (ret == ESP_OK) ? ESP_OK : ret,
            };
            if (g_esl_ap != NULL && g_esl_ap->app_cb != NULL) {
                g_esl_ap->app_cb(BLE_ESL_AP_EVT_SYNCHRONIZED, &evt);
            }
        } else {
            /* Premature disconnect before PAST was initiated — report failure */
            ESP_LOGW(TAG, "synchronize: premature disconnect for ESL 0x%04X before PAST",
                     ctx->esl_addr);

            ble_esl_ap_update_esl_state(ctx->esl_addr,
                                        BLE_ESL_STATE_UNSYNCHRONIZED);

            ble_esl_ap_synchronized_t evt = {
                .esl_addr = ble_esl_ap_addr_unpack(ctx->esl_addr),
                .status = ESP_FAIL,
            };
            if (g_esl_ap != NULL && g_esl_ap->app_cb != NULL) {
                g_esl_ap->app_cb(BLE_ESL_AP_EVT_SYNCHRONIZED, &evt);
            }
        }

        free(ctx);
    }

    /* Step 3: Clean up image transfer context if this disconnect
     * belongs to an active image transfer. */
    if (s_image_ctx != NULL && s_image_ctx->conn_handle == conn_handle) {
        ESP_LOGW(TAG, "transfer_image: disconnect during image transfer");
        ble_esl_ap_image_transferred_t img_evt = {
            .conn_handle = conn_handle,
            .image_index = s_image_ctx->image_index,
            .status = ESP_FAIL,
        };
        if (g_esl_ap != NULL && g_esl_ap->app_cb != NULL) {
            g_esl_ap->app_cb(BLE_ESL_AP_EVT_IMAGE_TRANSFERRED, &img_evt);
        }
        free(s_image_ctx);
        s_image_ctx = NULL;
    }
}
