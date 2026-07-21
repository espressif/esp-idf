/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file esl_state.c
 * @brief ESL state machine, lifecycle APIs, security, and timeout handling.
 */

#include <string.h>
#include <assert.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "nimble/ble.h"
#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "host/ble_gatt.h"
#include "host/ble_uuid.h"
#include "host/ble_store.h"

#include "ble_esl.h"
#include "ble_esl_int.h"
#include "ble_esl_state_int.h"

static const char *TAG = "esl_state";

#define ESL_EXT_ADV_INSTANCE    0

static esl_state_ctx_t *s_ctx = NULL;

/* ========================== Forward Declarations ========================== */

static int esl_gap_event_handler(struct ble_gap_event *event, void *arg);
static void sync_timeout_cb(void *arg);
static void unsync_timeout_cb(void *arg);
static void ecp_timeout_cb(void *arg);
static esp_err_t esl_start_advertising(void);
static esp_err_t esl_stop_advertising(void);
static bool is_transition_valid(ble_esl_state_t from, ble_esl_state_t to);

/* ========================== State Machine ========================== */

/**
 * @brief Check if a state transition is valid
 */
static bool is_transition_valid(ble_esl_state_t from, ble_esl_state_t to)
{
    switch (from) {
    case BLE_ESL_STATE_UNASSOCIATED:
        return (to == BLE_ESL_STATE_CONFIGURING);
    case BLE_ESL_STATE_CONFIGURING:
        return (to == BLE_ESL_STATE_UNASSOCIATED ||
                to == BLE_ESL_STATE_SYNCHRONIZED ||
                to == BLE_ESL_STATE_UNSYNCHRONIZED);
    case BLE_ESL_STATE_SYNCHRONIZED:
        return (to == BLE_ESL_STATE_UPDATING ||
                to == BLE_ESL_STATE_UNSYNCHRONIZED);
    case BLE_ESL_STATE_UPDATING:
        return (to == BLE_ESL_STATE_UNASSOCIATED ||
                to == BLE_ESL_STATE_SYNCHRONIZED ||
                to == BLE_ESL_STATE_UNSYNCHRONIZED);
    case BLE_ESL_STATE_UNSYNCHRONIZED:
        return (to == BLE_ESL_STATE_UNASSOCIATED ||
                to == BLE_ESL_STATE_UPDATING);
    default:
        return false;
    }
}

esl_state_ctx_t *esl_state_ctx_get(void)
{
    return s_ctx;
}

uint16_t esl_state_get_conn_handle(void)
{
    if (s_ctx == NULL) {
        return BLE_HS_CONN_HANDLE_NONE;
    }
    return s_ctx->conn_handle;
}

bool esl_is_pawr_synced(void)
{
    if (s_ctx == NULL) {
        return false;
    }
    return s_ctx->pawr_synced;
}

/**
 * @brief Terminate the tracked ACL link, if any
 *
 * On success the handle is deliberately kept so the disconnect event handler can
 * match it and run its cleanup. BLE_HS_ENOTCONN means the host has no such
 * connection: the handle is stale and no disconnect event will ever arrive, so it
 * is cleared here. Any other failure may leave the link up (e.g. BLE_HS_EALREADY,
 * termination already in progress), so the handle is kept to match the disconnect
 * event that still follows.
 */
static void esl_terminate_active_conn(void)
{
    if (s_ctx->conn_handle == BLE_HS_CONN_HANDLE_NONE) {
        return;
    }

    int rc = ble_gap_terminate(s_ctx->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
    if (rc == BLE_HS_ENOTCONN) {
        ESP_LOGW(TAG, "Connection %u already gone — clearing stale handle",
                 s_ctx->conn_handle);
        s_ctx->conn_handle = BLE_HS_CONN_HANDLE_NONE;
    } else if (rc != 0) {
        ESP_LOGW(TAG, "Failed to terminate connection %u: rc=%d",
                 s_ctx->conn_handle, rc);
    }
}

esp_err_t esl_state_transition(ble_esl_state_t new_state)
{
    if (s_ctx == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    ble_esl_state_t old_state = s_ctx->state;
    if (old_state == new_state) {
        return ESP_OK;
    }

    if (!is_transition_valid(old_state, new_state)) {
        ESP_LOGW(TAG, "Invalid state transition: %d -> %d", old_state, new_state);
        return ESP_ERR_INVALID_STATE;
    }

    ESP_LOGI(TAG, "State transition: %d -> %d", old_state, new_state);

    /* Disarm timers from old state */
    if (old_state == BLE_ESL_STATE_SYNCHRONIZED) {
        esp_timer_stop(s_ctx->sync_timer);
    }
    if (old_state == BLE_ESL_STATE_UNSYNCHRONIZED) {
        esp_timer_stop(s_ctx->unsync_timer);
    }

    /* Execute side effects for the new state */
    switch (new_state) {
    case BLE_ESL_STATE_UNASSOCIATED:
        /* Stop active advertising first to allow unpair to succeed
         * and to ensure advertising restarts with correct parameters */
        esl_stop_advertising();
        /* Clear stored data (keys, address, timed commands) */
        esl_clear_stored_data(false);
        s_ctx->config_complete = 0;
        s_ctx->pawr_synced = false;
        /* Delete bonding info (requires advertising to be stopped) */
        if (s_ctx->has_bonded_peer) {
            int unpair_rc = ble_gap_unpair(&s_ctx->bonded_peer_addr);
            if (unpair_rc != 0) {
                ESP_LOGW(TAG, "ble_gap_unpair failed: rc=%d", unpair_rc);
            }
            s_ctx->has_bonded_peer = false;
        }
        /* Start advertising with Unassociated parameters if started */
        s_ctx->state = new_state;
        if (s_ctx->started) {
            esl_start_advertising();
        }
        break;

    case BLE_ESL_STATE_CONFIGURING:
        s_ctx->config_complete = 0;
        /* Reset PAST/Update Complete tracking so a stale flag from a previous
         * provisioning cycle cannot prematurely advance to Synchronized. */
        s_ctx->past_received = false;
        s_ctx->update_complete_received = false;
        s_ctx->pawr_synced = false;
        s_ctx->state = new_state;
        /* No advertising — ACL link is active */
        break;

    case BLE_ESL_STATE_SYNCHRONIZED:
        s_ctx->state = new_state;
        s_ctx->pawr_synced = true;
        /* Stop legacy advertising if any */
        esl_stop_advertising();
        /* Disconnect ACL link if one is active */
        esl_terminate_active_conn();
        /* Arm 60-minute sync timeout only if service is running */
        if (s_ctx->started) {
            esp_timer_start_once(s_ctx->sync_timer, ESL_STATE_TIMEOUT_US);
        }
        break;

    case BLE_ESL_STATE_UPDATING:
        s_ctx->state = new_state;
        /* Reset PAST/Update Complete tracking flags. pawr_synced is deliberately
         * left untouched: while Updating, the ESL only ignores PAwR data — the
         * sync to the AP's train is not lost, so the Basic State Synchronized
         * bit must keep reporting it. */
        s_ctx->past_received = false;
        s_ctx->update_complete_received = false;
        /* Disarm sync timeout (already done above if from Synchronized) */
        /* No advertising — ACL link is active */
        break;

    case BLE_ESL_STATE_UNSYNCHRONIZED:
        s_ctx->state = new_state;
        s_ctx->pawr_synced = false;
        /* Arm 60-minute unsync timeout only if service is running */
        if (s_ctx->started) {
            esp_timer_start_once(s_ctx->unsync_timer, ESL_STATE_TIMEOUT_US);
            /* Start connectable advertising */
            esl_start_advertising();
        }
        break;

    default:
        return ESP_ERR_INVALID_ARG;
    }

    /* Fire state changed callback */
    esl_notify_app(BLE_ESL_EVT_STATE_CHANGED, &(ble_esl_cb_param_t){
        .state_changed = {
            .prev_state = old_state,
            .new_state = new_state,
        }
    });

    return ESP_OK;
}

/* ========================== Absolute Time ========================== */

uint32_t esl_get_abs_time(void)
{
    if (s_ctx == NULL) {
        return 0;
    }
    int64_t now_us = esp_timer_get_time();
    int64_t elapsed_ms = (now_us - s_ctx->abs_time_offset_us) / 1000;
    return (uint32_t)(s_ctx->abs_time_base + (uint32_t)elapsed_ms);
}

/* ========================== App Callback ========================== */

void esl_notify_app(ble_esl_event_t event, ble_esl_cb_param_t *param)
{
    if (s_ctx != NULL && s_ctx->app_cb != NULL) {
        s_ctx->app_cb(event, param);
    }
}

/* ========================== Getters ========================== */

esp_err_t esl_get_address(uint8_t *esl_id, uint8_t *group_id)
{
    if (s_ctx == NULL || !s_ctx->address_valid) {
        return ESP_ERR_INVALID_STATE;
    }
    if (esl_id != NULL) {
        *esl_id = s_ctx->esl_address.esl_id;
    }
    if (group_id != NULL) {
        *group_id = BLE_ESL_ADDR_GROUP_ID(s_ctx->esl_address);
    }
    return ESP_OK;
}

const ble_esl_config_t *esl_get_config(void)
{
    if (s_ctx == NULL) {
        return NULL;
    }
    return &s_ctx->config;
}

const ble_esl_key_material_t *esl_get_ap_sync_key(void)
{
    if (s_ctx == NULL || !s_ctx->ap_sync_key_valid) {
        return NULL;
    }
    return &s_ctx->ap_sync_key;
}

const ble_esl_key_material_t *esl_get_resp_key(void)
{
    if (s_ctx == NULL || !s_ctx->resp_key_valid) {
        return NULL;
    }
    return &s_ctx->resp_key;
}

/* ========================== Update Complete Notification ========================== */

void esl_notify_update_complete(void)
{
    if (s_ctx == NULL) {
        return;
    }

    s_ctx->update_complete_received = true;

    /* Advance to Synchronized only once PAST (PAwR sync transfer) has also
     * completed. Entering Synchronized disconnects the ACL link that carries
     * PAST, so transitioning on Update Complete alone would leave the ESL
     * un-synced to the PAwR train and unable to answer commands (e.g. Ping).
     * Whichever of {Update Complete, PAST} arrives last triggers the
     * transition — the PAST path performs the symmetric check in
     * handle_gap_periodic_transfer(). */
    switch (s_ctx->state) {
    case BLE_ESL_STATE_CONFIGURING:
        /* First provisioning also requires all mandatory configuration writes. */
        if (s_ctx->past_received &&
            (s_ctx->config_complete & CONFIG_COMPLETE_MASK) == CONFIG_COMPLETE_MASK) {
            esl_state_transition(BLE_ESL_STATE_SYNCHRONIZED);
        } else {
            ESP_LOGI(TAG, "Update Complete received in Configuring — waiting for PAST / config");
        }
        break;

    case BLE_ESL_STATE_UPDATING:
        if (s_ctx->past_received) {
            esl_state_transition(BLE_ESL_STATE_SYNCHRONIZED);
        } else {
            ESP_LOGI(TAG, "Update Complete received — waiting for PAST");
        }
        break;

    default:
        ESP_LOGW(TAG, "Update Complete received in unexpected state %d", s_ctx->state);
        break;
    }
}

/* ========================== PAwR Sync Teardown ========================== */

/**
 * @brief Terminate the active PAwR periodic sync (if any) and clear its handle
 *
 * @return ESP_OK if no sync is active or the sync is gone (terminated here or
 *         already unknown to the host), ESP_FAIL if the host still owns the sync
 */
static esp_err_t esl_terminate_pawr_sync(void)
{
    if (s_ctx == NULL || s_ctx->pawr_sync_handle == BLE_HS_CONN_HANDLE_NONE) {
        return ESP_OK;
    }

    int rc = ble_gap_periodic_adv_sync_terminate(s_ctx->pawr_sync_handle);
    if (rc != 0 && rc != BLE_HS_ENOTCONN) {
        /* Anything other than ENOTCONN (host no longer knows this sync, e.g. it
         * was lost concurrently) means the host still holds the periodic sync.
         * Keep the handle so the caller can retry, and keep pawr_synced so the
         * Basic State Synchronized bit stays truthful. */
        ESP_LOGW(TAG, "Failed to terminate PAwR sync %u: rc=%d",
                 s_ctx->pawr_sync_handle, rc);
        return ESP_FAIL;
    }

    s_ctx->pawr_sync_handle = BLE_HS_CONN_HANDLE_NONE;
    s_ctx->pawr_synced = false;
    return ESP_OK;
}

/* ========================== Clear Stored Data ========================== */

void esl_clear_stored_data(bool clear_images)
{
    if (s_ctx == NULL) {
        return;
    }

    /* Zero keys */
    memset(&s_ctx->ap_sync_key, 0, sizeof(s_ctx->ap_sync_key));
    memset(&s_ctx->resp_key, 0, sizeof(s_ctx->resp_key));
    s_ctx->ap_sync_key_valid = false;
    s_ctx->resp_key_valid = false;

    /* Zero address */
    memset(&s_ctx->esl_address, 0, sizeof(s_ctx->esl_address));
    s_ctx->address_valid = false;

    /* Drop the PAwR synchronization along with the key material that feeds it.
     * A failure is logged by the helper, which then keeps the handle so a later
     * teardown (state transition, stop, deinit) can retry. */
    (void)esl_terminate_pawr_sync();

    /* Cancel all timed commands */
    esl_cmd_display_cancel_all();
    esl_cmd_led_cancel_all();

    (void)clear_images;
    /* Image clearing would be handled by the application via factory reset event */
}

/* ========================== GAP Advertising ========================== */

static esp_err_t esl_start_advertising(void)
{
    if (ble_gap_ext_adv_active(ESL_EXT_ADV_INSTANCE)) {
        return ESP_OK;
    }

    struct ble_gap_ext_adv_params params;
    struct os_mbuf *data;
    int rc;

    ble_esl_state_t state = s_ctx->state;

    if (state == BLE_ESL_STATE_UNASSOCIATED) {
        /* Undirected Connectable, General Discoverable with ESL Service UUID */
        memset(&params, 0, sizeof(params));
        params.connectable = 1;
        params.own_addr_type = BLE_OWN_ADDR_PUBLIC;
        params.primary_phy = BLE_HCI_LE_PHY_1M;
        params.secondary_phy = BLE_HCI_LE_PHY_1M;
        params.tx_power = 127;
        params.sid = 1;

        rc = ble_gap_ext_adv_configure(ESL_EXT_ADV_INSTANCE, &params, NULL,
                                       esl_gap_event_handler, NULL);
        if (rc != 0) {
            ESP_LOGE(TAG, "Failed to configure ext adv: rc=%d", rc);
            return ESP_FAIL;
        }

        /* Build advertising data: Flags + ESL Service UUID */
        uint8_t adv_data[] = {
            0x02, BLE_HS_ADV_TYPE_FLAGS,
            BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP,
            0x03, BLE_HS_ADV_TYPE_COMP_UUIDS16,
            (uint8_t)(BLE_ESL_SVC_UUID & 0xFF),
            (uint8_t)(BLE_ESL_SVC_UUID >> 8),
        };

        data = os_msys_get_pkthdr(sizeof(adv_data), 0);
        if (data == NULL) {
            ESP_LOGE(TAG, "Failed to allocate mbuf for adv data");
            return ESP_ERR_NO_MEM;
        }

        rc = os_mbuf_append(data, adv_data, sizeof(adv_data));
        if (rc != 0) {
            os_mbuf_free_chain(data);
            ESP_LOGE(TAG, "Failed to append adv data to mbuf: rc=%d", rc);
            return ESP_FAIL;
        }

        rc = ble_gap_ext_adv_set_data(ESL_EXT_ADV_INSTANCE, data);
        if (rc != 0) {
            ESP_LOGE(TAG, "Failed to set ext adv data: rc=%d", rc);
            return ESP_FAIL;
        }

        /* Start advertising indefinitely (duration=0, max_events=0) */
        rc = ble_gap_ext_adv_start(ESL_EXT_ADV_INSTANCE, 0, 0);
        if (rc != 0) {
            ESP_LOGE(TAG, "Failed to start ext adv: rc=%d", rc);
            return ESP_FAIL;
        }
        ESP_LOGI(TAG, "Extended advertising started (Unassociated, bondable)");

    } else if (state == BLE_ESL_STATE_UNSYNCHRONIZED) {
        /* GAP Connectable mode — no specific AD type mandated */
        memset(&params, 0, sizeof(params));
        params.connectable = 1;
        params.own_addr_type = BLE_OWN_ADDR_PUBLIC;
        params.primary_phy = BLE_HCI_LE_PHY_1M;
        params.secondary_phy = BLE_HCI_LE_PHY_1M;
        params.tx_power = 127;
        params.sid = 1;

        rc = ble_gap_ext_adv_configure(ESL_EXT_ADV_INSTANCE, &params, NULL,
                                       esl_gap_event_handler, NULL);
        if (rc != 0) {
            ESP_LOGE(TAG, "Failed to configure ext adv: rc=%d", rc);
            return ESP_FAIL;
        }

        /* Build advertising data: Flags only */
        uint8_t adv_data[] = {
            0x02, BLE_HS_ADV_TYPE_FLAGS, BLE_HS_ADV_F_BREDR_UNSUP,
        };

        data = os_msys_get_pkthdr(sizeof(adv_data), 0);
        if (data == NULL) {
            ESP_LOGE(TAG, "Failed to allocate mbuf for adv data");
            return ESP_ERR_NO_MEM;
        }

        rc = os_mbuf_append(data, adv_data, sizeof(adv_data));
        if (rc != 0) {
            os_mbuf_free_chain(data);
            ESP_LOGE(TAG, "Failed to append adv data to mbuf: rc=%d", rc);
            return ESP_FAIL;
        }

        rc = ble_gap_ext_adv_set_data(ESL_EXT_ADV_INSTANCE, data);
        if (rc != 0) {
            ESP_LOGE(TAG, "Failed to set ext adv data: rc=%d", rc);
            return ESP_FAIL;
        }

        /* Start advertising indefinitely (duration=0, max_events=0) */
        rc = ble_gap_ext_adv_start(ESL_EXT_ADV_INSTANCE, 0, 0);
        if (rc != 0) {
            ESP_LOGE(TAG, "Failed to start ext adv: rc=%d", rc);
            return ESP_FAIL;
        }
        ESP_LOGI(TAG, "Extended advertising started (Unsynchronized, connectable)");
    }

    return ESP_OK;
}

static esp_err_t esl_stop_advertising(void)
{
    if (ble_gap_ext_adv_active(ESL_EXT_ADV_INSTANCE)) {
        int rc = ble_gap_ext_adv_stop(ESL_EXT_ADV_INSTANCE);
        if (rc != 0 && rc != BLE_HS_EALREADY) {
            ESP_LOGW(TAG, "Failed to stop ext adv: rc=%d", rc);
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}

/* ========================== GAP Event Handler ========================== */

/**
 * @brief Handle a new connection event
 */
static void handle_gap_connect(struct ble_gap_event *event)
{
    if (event->connect.status != 0) {
        ESP_LOGW(TAG, "Connection failed: status=%d", event->connect.status);
        /* Restart advertising if needed */
        if (s_ctx->started &&
            (s_ctx->state == BLE_ESL_STATE_UNASSOCIATED ||
             s_ctx->state == BLE_ESL_STATE_UNSYNCHRONIZED)) {
            esl_start_advertising();
        }
        return;
    }

    /* Reject secondary connections if one is already active */
    if (s_ctx->conn_handle != BLE_HS_CONN_HANDLE_NONE) {
        ESP_LOGW(TAG, "Already connected (handle=%d) — terminating new connection %d",
                 s_ctx->conn_handle, event->connect.conn_handle);
        ble_gap_terminate(event->connect.conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        return;
    }

    s_ctx->conn_handle = event->connect.conn_handle;
    ESP_LOGI(TAG, "Connected: handle=%d, state=%d",
             s_ctx->conn_handle, s_ctx->state);

    /* When reconnected by the trusted AP — from Unsynchronized via connectable
     * advertising, or from Synchronized via the Periodic Advertising Connection
     * procedure (AUX_CONNECT_REQ in our PAwR subevent) — request security so the
     * stored LTK is used to encrypt the link. */
    if (s_ctx->state == BLE_ESL_STATE_UNSYNCHRONIZED ||
        s_ctx->state == BLE_ESL_STATE_SYNCHRONIZED) {
        int rc = ble_gap_security_initiate(s_ctx->conn_handle);
        if (rc != 0) {
            ESP_LOGW(TAG, "Security initiation failed: rc=%d", rc);
        }
    }
}

/**
 * @brief Handle a disconnect event
 */
static void handle_gap_disconnect(struct ble_gap_event *event)
{
    uint16_t handle = event->disconnect.conn.conn_handle;
    ESP_LOGI(TAG, "Disconnected: handle=%d reason=%d state=%d",
             handle, event->disconnect.reason, s_ctx->state);

    /* Ignore disconnect for a connection we are not tracking */
    if (handle != s_ctx->conn_handle) {
        ESP_LOGW(TAG, "Disconnect for unknown handle %d (active=%d) — ignoring",
                 handle, s_ctx->conn_handle);
        return;
    }

    s_ctx->conn_handle = BLE_HS_CONN_HANDLE_NONE;

    /* If deinit is pending, signal the semaphore and skip state transitions */
    if (s_ctx->deinit_pending) {
        if (s_ctx->deinit_sem != NULL) {
            xSemaphoreGive(s_ctx->deinit_sem);
        }
        return;
    }

    /* If factory reset is pending, clear all data and transition to Unassociated */
    if (esl_is_factory_reset_pending()) {
        esl_clear_stored_data(true);
        esl_state_transition(BLE_ESL_STATE_UNASSOCIATED);
        esl_clear_factory_reset_pending();
        return;
    }

    switch (s_ctx->state) {
    case BLE_ESL_STATE_CONFIGURING:
        if ((s_ctx->config_complete & CONFIG_COMPLETE_MASK) == CONFIG_COMPLETE_MASK) {
            /* Configuration complete — go to Unsynchronized */
            esl_state_transition(BLE_ESL_STATE_UNSYNCHRONIZED);
        } else {
            /* Configuration incomplete — discard and go to Unassociated */
            esl_state_transition(BLE_ESL_STATE_UNASSOCIATED);
        }
        break;

    case BLE_ESL_STATE_UPDATING:
        /* Link loss during Updating — go to Unsynchronized */
        esl_state_transition(BLE_ESL_STATE_UNSYNCHRONIZED);
        break;

    default:
        /* For other states, restart advertising if needed */
        if (s_ctx->started &&
            (s_ctx->state == BLE_ESL_STATE_UNASSOCIATED ||
             s_ctx->state == BLE_ESL_STATE_UNSYNCHRONIZED)) {
            esl_start_advertising();
        }
        break;
    }
}

static void enable_past_reception(uint16_t conn_handle)
{
    struct ble_gap_periodic_sync_params sync_params = {0};
    sync_params.skip = 0;
    sync_params.sync_timeout = 1000; /* 10 s in 10 ms units */
    sync_params.reports_disabled = 0;

    int rc = ble_gap_periodic_adv_sync_receive(conn_handle, &sync_params,
                                               esl_gap_event_handler, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "ble_gap_periodic_adv_sync_receive failed: rc=%d", rc);
    }

    ESP_LOGI(TAG, "Enable PAST reception: conn_handle=%u", conn_handle);
}

/**
 * @brief Handle encryption change event
 */
static void handle_gap_enc_change(struct ble_gap_event *event)
{
    if (event->enc_change.status != 0) {
        ESP_LOGW(TAG, "Encryption change failed: status=%d state=%d",
                 event->enc_change.status, s_ctx->state);
        return;
    }

    /* Ignore encryption events for connections we are not tracking */
    if (event->enc_change.conn_handle != s_ctx->conn_handle) {
        ESP_LOGW(TAG, "Enc change for unknown handle %d (active=%d) — ignoring",
                 event->enc_change.conn_handle, s_ctx->conn_handle);
        return;
    }

    struct ble_gap_conn_desc desc;
    int rc = ble_gap_conn_find(event->enc_change.conn_handle, &desc);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to find connection for enc change");
        return;
    }

    ESP_LOGI(TAG, "Encryption enabled: bonded=%d, authenticated=%d, state=%d",
             desc.sec_state.bonded, desc.sec_state.authenticated, s_ctx->state);

    if ((s_ctx->state == BLE_ESL_STATE_UNSYNCHRONIZED ||
         s_ctx->state == BLE_ESL_STATE_SYNCHRONIZED) && desc.sec_state.bonded) {
        /* Verify this is the trusted bonded peer */
        if (s_ctx->has_bonded_peer &&
            ble_addr_cmp(&desc.peer_id_addr, &s_ctx->bonded_peer_addr) == 0) {
            /* Trusted AP (re)connected:
             *  - from Unsynchronized via connectable advertising, or
             *  - from Synchronized via the Periodic Advertising Connection
             *    procedure.
             * Move to Updating and re-arm PAST reception so the AP can
             * re-synchronize us afterwards. */
            esl_state_transition(BLE_ESL_STATE_UPDATING);
            enable_past_reception(event->enc_change.conn_handle);
        } else {
            ESP_LOGW(TAG, "Enc change: bonded peer mismatch — disconnecting untrusted device");
            ble_gap_terminate(event->enc_change.conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        }
    }
}

/**
 * @brief Handle pairing complete event
 */
static void handle_gap_pairing_complete(struct ble_gap_event *event)
{
    if (event->pairing_complete.status != 0) {
        ESP_LOGW(TAG, "Pairing failed: status=%d state=%d",
                 event->pairing_complete.status, s_ctx->state);
        return;
    }

    ESP_LOGI(TAG, "Pairing complete: handle=%d, state=%d",
             event->pairing_complete.conn_handle, s_ctx->state);

    struct ble_gap_conn_desc desc;
    int rc = ble_gap_conn_find(event->pairing_complete.conn_handle, &desc);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to find connection after pairing");
        return;
    }

    if (desc.sec_state.bonded) {
        if (s_ctx->state == BLE_ESL_STATE_UNASSOCIATED) {
            /* Store bonded peer address only in Unassociated state */
            s_ctx->bonded_peer_addr = desc.peer_id_addr;
            s_ctx->has_bonded_peer = true;
            /* Bond formed from Unassociated — transition to Configuring */
            esl_state_transition(BLE_ESL_STATE_CONFIGURING);
            enable_past_reception(event->pairing_complete.conn_handle);
        } else if ((s_ctx->state == BLE_ESL_STATE_UPDATING ||
                    s_ctx->state == BLE_ESL_STATE_UNSYNCHRONIZED ||
                    s_ctx->state == BLE_ESL_STATE_SYNCHRONIZED) &&
                   s_ctx->has_bonded_peer &&
                   ble_addr_cmp(&desc.peer_id_addr, &s_ctx->bonded_peer_addr) == 0) {
            /* Trusted AP reconnected and re-paired. Just refresh the stored
             * address; the encryption-change handler drives the state change
             * (UNSYNCHRONIZED -> UPDATING). Rejecting here would tear down a
             * legitimate reconnection and delete the bond, leaving the ESL
             * permanently unreachable for updates. */
            s_ctx->bonded_peer_addr = desc.peer_id_addr;
        } else {
            /* Reject untrusted pairing in other states — delete peer and disconnect */
            ESP_LOGW(TAG, "Pairing rejected: not in Unassociated state (%d)", s_ctx->state);
            ble_store_util_delete_peer(&desc.peer_id_addr);
            ble_gap_terminate(event->pairing_complete.conn_handle,
                              BLE_ERR_REM_USER_CONN_TERM);
        }
    }
}

/**
 * @brief Handle repeat pairing event
 */
static int handle_gap_repeat_pairing(struct ble_gap_event *event)
{
    /* In Updating, Unsynchronized, or Synchronized state (the latter reachable
     * via the Periodic Advertising Connection procedure), allow re-pairing only
     * from the trusted bonded peer and reject all others. */
    if (s_ctx->state == BLE_ESL_STATE_UPDATING ||
        s_ctx->state == BLE_ESL_STATE_UNSYNCHRONIZED ||
        s_ctx->state == BLE_ESL_STATE_SYNCHRONIZED) {
        struct ble_gap_conn_desc desc;
        int rc = ble_gap_conn_find(event->repeat_pairing.conn_handle, &desc);
        if (rc == 0 && s_ctx->has_bonded_peer) {
            /* Check if this is the trusted peer */
            if (ble_addr_cmp(&desc.peer_id_addr, &s_ctx->bonded_peer_addr) == 0) {
                /* Trusted peer — allow re-pairing */
                ble_store_util_delete_peer(&desc.peer_id_addr);
                return BLE_GAP_REPEAT_PAIRING_RETRY;
            }
        }
        /* Reject pairing from untrusted device */
        return BLE_GAP_REPEAT_PAIRING_IGNORE;
    }

    /* For other states, allow re-pairing */
    struct ble_gap_conn_desc desc;
    int rc = ble_gap_conn_find(event->repeat_pairing.conn_handle, &desc);
    if (rc != 0) {
        ESP_LOGW(TAG, "Repeat pairing: conn_find failed rc=%d — ignoring", rc);
        return BLE_GAP_REPEAT_PAIRING_IGNORE;
    }
    ble_store_util_delete_peer(&desc.peer_id_addr);
    return BLE_GAP_REPEAT_PAIRING_RETRY;
}

/**
 * @brief Handle PAST (Periodic Advertising Sync Transfer) received event
 */
static void handle_gap_periodic_transfer(struct ble_gap_event *event)
{
    if (event->periodic_transfer.status != 0) {
        ESP_LOGW(TAG, "PAST failed: status=%d — retrying PAST reception", event->periodic_transfer.status);
        /* The controller failed to sync to the PAwR train (e.g. HCI 0x3E).
         * NimBLE already freed the psync entry, so re-arm PAST reception. */
        if (s_ctx->conn_handle != BLE_HS_CONN_HANDLE_NONE) {
            enable_past_reception(s_ctx->conn_handle);
        }
        return;
    }

    uint16_t sync_handle = event->periodic_transfer.sync_handle;
    ESP_LOGI(TAG, "PAST received: sync_handle=%d, state=%d", sync_handle, s_ctx->state);

    /* Terminate the previous PAwR sync if one was still alive (kept during
     * SYNCHRONIZED → UPDATING because ble_gap_periodic_adv_sync_terminate
     * defers pool-entry freeing and would cause enable_past_reception to fail
     * with ENOMEM if freed synchronously). */
    if (s_ctx->pawr_sync_handle != BLE_HS_CONN_HANDLE_NONE &&
        s_ctx->pawr_sync_handle != sync_handle) {
        int term_rc = ble_gap_periodic_adv_sync_terminate(s_ctx->pawr_sync_handle);
        if (term_rc != 0 && term_rc != BLE_HS_ENOTCONN) {
            /* Not fatal: the new sync from PAST supersedes the old one, which is
             * dropped when the host tears down its stale entry. */
            ESP_LOGW(TAG, "Failed to terminate stale PAwR sync %u: rc=%d",
                     s_ctx->pawr_sync_handle, term_rc);
        }
    }
    s_ctx->pawr_sync_handle = sync_handle;

    /* Enable PAwR subevent reception and response slots for our group/subevent. */
    uint8_t group_id = BLE_ESL_ADDR_GROUP_ID(s_ctx->esl_address);
    int rc = ble_gap_periodic_adv_sync_subev(sync_handle, 0, 1, &group_id);
    if (rc != 0) {
        ESP_LOGW(TAG, "PAST: sync_subev failed: rc=%d", rc);
    } else {
        ESP_LOGI(TAG, "PAwR response slots enabled for subevent %d", group_id);
    }

    /* Eagerly initialise the PAwR module with the correct sync handle so that
     * esl_pawr_encrypt_and_send() can use it before the first report arrives. */
    esl_pawr_set_sync_context(sync_handle, group_id);

    if (s_ctx->state == BLE_ESL_STATE_CONFIGURING) {
        /* In Configuring, all four mandatory writes AND Update Complete are
         * required before SYNCHRONIZED so premature/out-of-order PAST cannot
         * advance the state machine before provisioning finishes. */
        s_ctx->past_received = true;
        if (s_ctx->update_complete_received &&
            (s_ctx->config_complete & CONFIG_COMPLETE_MASK) == CONFIG_COMPLETE_MASK) {
            esl_state_transition(BLE_ESL_STATE_SYNCHRONIZED);
        } else {
            ESP_LOGI(TAG, "PAST received in Configuring — waiting for Update Complete / config");
        }
    } else if (s_ctx->state == BLE_ESL_STATE_SYNCHRONIZED) {
        /* PAST received while already Synchronized; record that PAwR sync is
         * established for any subsequent update cycle. */
        ESP_LOGI(TAG, "PAST received in Synchronized — PAwR sync established");
        s_ctx->past_received = true;
    } else if (s_ctx->state == BLE_ESL_STATE_UPDATING) {
        /* In Updating, both PAST and Update Complete are required */
        s_ctx->past_received = true;
        if (s_ctx->update_complete_received) {
            esl_state_transition(BLE_ESL_STATE_SYNCHRONIZED);
        } else {
            ESP_LOGI(TAG, "PAST received in Updating — waiting for Update Complete");
        }
    }
}

/**
 * @brief Handle PAwR sync data received
 */
static void handle_gap_periodic_report(struct ble_gap_event *event)
{
    /* Only process in Synchronized state */
    if (s_ctx->state != BLE_ESL_STATE_SYNCHRONIZED) {
        return;
    }

    uint16_t sync_handle = event->periodic_report.sync_handle;
    uint16_t event_counter = event->periodic_report.event_counter;
    uint8_t subevent = event->periodic_report.subevent;
    uint8_t data_length = event->periodic_report.data_length;
    const uint8_t *data = event->periodic_report.data;

    if (event->periodic_report.data_status != BLE_GAP_PER_ADV_DATA_STATUS_COMPLETE) {
        return;
    }

    /* PAwR subevent data is a raw AD structure:
     *   [AD Len(1)] [AD Type 0x31(1)] [Randomizer(5)] [Ciphertext(n)] [MIC(4)]
     * ble_esl_ead_decrypt() expects only the AD value (Randomizer..MIC), so the
     * length and type octets must be parsed off first. */
    /* Empty subevent data is normal: the AP transmits an empty subevent for any
     * group with no queued command, so ignore it silently instead of warning. */
    if (data == NULL || data_length == 0) {
        return;
    }
    if (data_length < 2 + BLE_ESL_RANDOMIZER_SIZE + BLE_ESL_MIC_SIZE + 1) {
        ESP_LOGW(TAG, "PAwR report: data too short (%u bytes)", data_length);
        return;
    }

    uint8_t ad_len  = data[0];   /* counts the AD type octet + encrypted payload */
    uint8_t ad_type = data[1];

    if (ad_type != BLE_ESL_AD_TYPE_ENCRYPTED_DATA ||
        ad_len < 1 + BLE_ESL_RANDOMIZER_SIZE + BLE_ESL_MIC_SIZE ||
        (uint16_t)(ad_len + 1) > data_length) {
        ESP_LOGW(TAG, "PAwR report: not a valid Encrypted Data AD (type=0x%02x len=%u)",
                 ad_type, ad_len);
        return;
    }

    /* Strip the AD length + type octets to get the EAD payload. */
    const uint8_t *enc_payload = &data[2];
    uint8_t enc_payload_len = ad_len - 1;

    /* Get AP Sync Key Material for decryption */
    const ble_esl_key_material_t *key_mat = esl_get_ap_sync_key();
    if (key_mat == NULL) {
        ESP_LOGW(TAG, "PAwR report: AP Sync Key not available");
        return;
    }

    /* enc_payload layout: [Randomizer(5)] [Ciphertext(n)] [MIC(4)] */
    uint8_t decrypted[BLE_ESL_PAYLOAD_MAX_SIZE];
    size_t payload_len = 0;
    esp_err_t err = ble_esl_ead_decrypt(key_mat->session_key, key_mat->iv,
                                        enc_payload, enc_payload_len,
                                        decrypted, sizeof(decrypted),
                                        &payload_len);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "PAwR report: decryption failed: %d", (int)err);
        return;
    }

    /* Decryption and MIC verification succeeded — process the packet */
    esl_pawr_set_sync_context(sync_handle, event_counter);
    err = esl_pawr_process_sync_packet(subevent, decrypted, (uint8_t)payload_len);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "PAwR report: process sync packet failed: %d", (int)err);
    }

    /* Reset sync timeout on valid data */
    esp_timer_stop(s_ctx->sync_timer);
    esp_timer_start_once(s_ctx->sync_timer, ESL_STATE_TIMEOUT_US);
}

static int esl_gap_event_handler(struct ble_gap_event *event, void *arg)
{
    if (s_ctx == NULL) {
        return 0;
    }

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        handle_gap_connect(event);
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        handle_gap_disconnect(event);
        return 0;

    case BLE_GAP_EVENT_ENC_CHANGE:
        handle_gap_enc_change(event);
        return 0;

    case BLE_GAP_EVENT_PARING_COMPLETE:
        handle_gap_pairing_complete(event);
        return 0;

    case BLE_GAP_EVENT_REPEAT_PAIRING:
        return handle_gap_repeat_pairing(event);

    case BLE_GAP_EVENT_PERIODIC_TRANSFER_V2:
        /* fall through — same struct layout as V1; dispatched when PAwR is enabled */
    case BLE_GAP_EVENT_PERIODIC_TRANSFER:
        handle_gap_periodic_transfer(event);
        return 0;

    case BLE_GAP_EVENT_PERIODIC_REPORT:
        handle_gap_periodic_report(event);
        return 0;

    case BLE_GAP_EVENT_PERIODIC_SYNC_LOST:
        /* Fired asynchronously after ble_gap_periodic_adv_sync_terminate frees
         * the periodic sync pool slot. If PAST was deferred, arm it now. */
        if (s_ctx->past_pending &&
            s_ctx->state == BLE_ESL_STATE_UPDATING &&
            s_ctx->conn_handle != BLE_HS_CONN_HANDLE_NONE) {
            s_ctx->past_pending = false;
            ESP_LOGI(TAG, "SYNC_LOST: re-arming PAST reception (conn=%d)",
                     s_ctx->conn_handle);
            enable_past_reception(s_ctx->conn_handle);
        }
        return 0;

    case BLE_GAP_EVENT_NOTIFY_TX:
        esl_handle_ecp_notify_tx(event->notify_tx.conn_handle,
                                 event->notify_tx.attr_handle,
                                 event->notify_tx.status,
                                 event->notify_tx.indication);
        return 0;

    case BLE_GAP_EVENT_SUBSCRIBE:
        ESP_LOGI(TAG, "Subscribe: handle=%d attr=%d notify=%d indicate=%d",
                 event->subscribe.conn_handle, event->subscribe.attr_handle,
                 event->subscribe.cur_notify, event->subscribe.cur_indicate);
        return 0;

    case BLE_GAP_EVENT_MTU:
        ESP_LOGI(TAG, "MTU update: handle=%d mtu=%d",
                 event->mtu.conn_handle, event->mtu.value);
        return 0;

    default:
        return 0;
    }
}

/* ========================== Timer Callbacks ========================== */

static void sync_timeout_cb(void *arg)
{
    if (s_ctx == NULL || s_ctx->state != BLE_ESL_STATE_SYNCHRONIZED) {
        ESP_LOGW(TAG, "Sync timeout fired in unexpected state — ignoring");
        return;
    }
    ESP_LOGW(TAG, "Sync timeout (60 min) — transitioning to Unsynchronized");
    esl_state_transition(BLE_ESL_STATE_UNSYNCHRONIZED);
}

static void unsync_timeout_cb(void *arg)
{
    if (s_ctx == NULL || s_ctx->state != BLE_ESL_STATE_UNSYNCHRONIZED) {
        ESP_LOGW(TAG, "Unsync timeout fired in unexpected state — ignoring");
        return;
    }
    ESP_LOGW(TAG, "Unsync timeout (60 min) — clearing data, transitioning to Unassociated");
    esl_state_transition(BLE_ESL_STATE_UNASSOCIATED);
}

static void ecp_timeout_cb(void *arg)
{
    ESP_LOGW(TAG, "ECP procedure timeout (30s) — sending Retry error response");

    /* Build Error response TLV: opcode=0x00 (Tag=0, Length=0), params=[error code] */
    uint8_t resp_params[1] = { BLE_ESL_ERR_RETRY };
    uint8_t resp_buf[BLE_ESL_TLV_MAX_SIZE];
    uint8_t resp_len;
    esp_err_t err = ble_esl_tlv_encode(BLE_ESL_RESP_ERROR, resp_params,
                                       sizeof(resp_params), resp_buf, &resp_len);
    if (err == ESP_OK) {
        esl_send_ecp_response(resp_buf, resp_len);
    }
}

/* ========================== Config Validation ========================== */

static esp_err_t esl_validate_config(const ble_esl_config_t *config)
{
    if (config == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (config->num_displays > CONFIG_BLE_ESL_MAX_DISPLAYS) {
        ESP_LOGE(TAG, "num_displays=%u exceeds CONFIG_BLE_ESL_MAX_DISPLAYS=%u",
                 config->num_displays, CONFIG_BLE_ESL_MAX_DISPLAYS);
        return ESP_ERR_INVALID_ARG;
    }

    if (config->num_displays > 0 && config->display_info == NULL) {
        ESP_LOGE(TAG, "display_info NULL with num_displays=%u", config->num_displays);
        return ESP_ERR_INVALID_ARG;
    }

    /* Image slots are tracked by a fixed-size OTS object id map in esl_gatts.c */
    if (config->num_images > CONFIG_BLE_ESL_MAX_IMAGES) {
        ESP_LOGE(TAG, "num_images=%u exceeds CONFIG_BLE_ESL_MAX_IMAGES=%u",
                 config->num_images, CONFIG_BLE_ESL_MAX_IMAGES);
        return ESP_ERR_INVALID_ARG;
    }

    /* A NULL image_writable_mask is a valid configuration even with num_images > 0:
     * it declares every image slot read-only (static/pre-loaded content), so no
     * OTS write property is granted. The mask length itself cannot be checked here
     * because ble_esl_config_t carries no length field for it — the caller contract
     * (num_images entries) is documented in ble_esl.h. */
    if (config->num_images > 0 && config->image_writable_mask == NULL) {
        ESP_LOGI(TAG, "image_writable_mask NULL with num_images=%u — all images are read-only",
                 config->num_images);
    }

    if (config->num_leds > CONFIG_BLE_ESL_MAX_LEDS) {
        ESP_LOGE(TAG, "num_leds=%u exceeds CONFIG_BLE_ESL_MAX_LEDS=%u",
                 config->num_leds, CONFIG_BLE_ESL_MAX_LEDS);
        return ESP_ERR_INVALID_ARG;
    }

    if (config->num_leds > 0 && config->led_info == NULL) {
        ESP_LOGE(TAG, "led_info NULL with num_leds=%u", config->num_leds);
        return ESP_ERR_INVALID_ARG;
    }

    if (config->num_sensors > CONFIG_BLE_ESL_MAX_SENSORS) {
        ESP_LOGE(TAG, "num_sensors=%u exceeds CONFIG_BLE_ESL_MAX_SENSORS=%u",
                 config->num_sensors, CONFIG_BLE_ESL_MAX_SENSORS);
        return ESP_ERR_INVALID_ARG;
    }

    if (config->num_sensors > 0 &&
        (config->sensor_info == NULL || config->sensor_info_len == 0)) {
        ESP_LOGE(TAG, "sensor_info invalid with num_sensors=%u", config->num_sensors);
        return ESP_ERR_INVALID_ARG;
    }

    if (config->vendor_specific && config->pnp_id == NULL) {
        ESP_LOGE(TAG, "pnp_id NULL with vendor_specific=true");
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

/* ========================== Public APIs ========================== */

esp_err_t ble_esl_init(const ble_esl_config_t *config)
{
    if (s_ctx != NULL) {
        ESP_LOGE(TAG, "Already initialized");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = esl_validate_config(config);
    if (err != ESP_OK) {
        return err;
    }

    /* Allocate context */
    s_ctx = calloc(1, sizeof(esl_state_ctx_t));
    if (s_ctx == NULL) {
        ESP_LOGE(TAG, "Failed to allocate state context");
        return ESP_ERR_NO_MEM;
    }

    /* Copy config */
    memcpy(&s_ctx->config, config, sizeof(ble_esl_config_t));
    s_ctx->state = BLE_ESL_STATE_UNASSOCIATED;
    s_ctx->conn_handle = BLE_HS_CONN_HANDLE_NONE;
    s_ctx->pawr_sync_handle = BLE_HS_CONN_HANDLE_NONE;
    s_ctx->initialized = true;

    /* Configure security as mandated by the ESL Profile: LE Secure Connections
     * with bonding, No Input No Output.
     *
     * WARNING: ble_hs_cfg is GLOBAL host state, not per-profile. These writes
     * override whatever the application or another profile configured, and
     * ble_esl_deinit() does not restore the previous values. The caller-facing
     * contract is documented on ble_esl_init() in ble_esl.h. */
    ble_hs_cfg.sm_io_cap = BLE_HS_IO_NO_INPUT_OUTPUT;
    ble_hs_cfg.sm_bonding = 1;
    ble_hs_cfg.sm_mitm = 0;
    ble_hs_cfg.sm_sc = 1; /* LE Secure Connections */
    ble_hs_cfg.sm_our_key_dist |= BLE_SM_PAIR_KEY_DIST_ENC;
    ble_hs_cfg.sm_their_key_dist |= BLE_SM_PAIR_KEY_DIST_ENC;

    /* Create timers */
    esp_timer_create_args_t sync_timer_args = {
        .callback = sync_timeout_cb,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "esl_sync_tmr",
    };
    err = esp_timer_create(&sync_timer_args, &s_ctx->sync_timer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create sync timer: %s", esp_err_to_name(err));
        free(s_ctx);
        s_ctx = NULL;
        return ESP_ERR_NO_MEM;
    }

    esp_timer_create_args_t unsync_timer_args = {
        .callback = unsync_timeout_cb,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "esl_unsync_tmr",
    };
    err = esp_timer_create(&unsync_timer_args, &s_ctx->unsync_timer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create unsync timer: %s", esp_err_to_name(err));
        esp_timer_delete(s_ctx->sync_timer);
        free(s_ctx);
        s_ctx = NULL;
        return ESP_ERR_NO_MEM;
    }

    esp_timer_create_args_t ecp_timer_args = {
        .callback = ecp_timeout_cb,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "esl_ecp_tmr",
    };
    err = esp_timer_create(&ecp_timer_args, &s_ctx->ecp_timer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create ECP timer: %s", esp_err_to_name(err));
        esp_timer_delete(s_ctx->sync_timer);
        esp_timer_delete(s_ctx->unsync_timer);
        free(s_ctx);
        s_ctx = NULL;
        return ESP_ERR_NO_MEM;
    }

    /* Build and register GATT service */
    err = esl_gatts_init(config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "GATT service init failed: %s", esp_err_to_name(err));
        esl_gatts_deinit();
        esp_timer_delete(s_ctx->sync_timer);
        esp_timer_delete(s_ctx->unsync_timer);
        esp_timer_delete(s_ctx->ecp_timer);
        free(s_ctx);
        s_ctx = NULL;
        return err;
    }

    /* Initialize sub-modules */
    err = esl_cmd_lifecycle_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Lifecycle init failed");
        esl_gatts_deinit();
        esp_timer_delete(s_ctx->sync_timer);
        esp_timer_delete(s_ctx->unsync_timer);
        esp_timer_delete(s_ctx->ecp_timer);
        free(s_ctx);
        s_ctx = NULL;
        return err;
    }

    err = esl_cmd_display_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Display init failed");
        esl_cmd_lifecycle_deinit();
        esl_gatts_deinit();
        esp_timer_delete(s_ctx->sync_timer);
        esp_timer_delete(s_ctx->unsync_timer);
        esp_timer_delete(s_ctx->ecp_timer);
        free(s_ctx);
        s_ctx = NULL;
        return err;
    }

    err = esl_cmd_led_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "LED init failed");
        esl_cmd_display_deinit();
        esl_cmd_lifecycle_deinit();
        esl_gatts_deinit();
        esp_timer_delete(s_ctx->sync_timer);
        esp_timer_delete(s_ctx->unsync_timer);
        esp_timer_delete(s_ctx->ecp_timer);
        free(s_ctx);
        s_ctx = NULL;
        return err;
    }

    err = esl_pawr_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "PAwR init failed");
        esl_cmd_led_deinit();
        esl_cmd_display_deinit();
        esl_cmd_lifecycle_deinit();
        esl_gatts_deinit();
        esp_timer_delete(s_ctx->sync_timer);
        esp_timer_delete(s_ctx->unsync_timer);
        esp_timer_delete(s_ctx->ecp_timer);
        free(s_ctx);
        s_ctx = NULL;
        return err;
    }

    ESP_LOGI(TAG, "ESL initialized: displays=%u images=%u leds=%u sensors=%u",
             config->num_displays, config->num_images,
             config->num_leds, config->num_sensors);

    return ESP_OK;
}

esp_err_t ble_esl_deinit(void)
{
    if (s_ctx == NULL) {
        ESP_LOGE(TAG, "Not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    /* If there is an active connection, wait for the async disconnect to complete */
    if (s_ctx->started && s_ctx->conn_handle != BLE_HS_CONN_HANDLE_NONE) {
        s_ctx->deinit_sem = xSemaphoreCreateBinary();
        if (s_ctx->deinit_sem != NULL) {
            s_ctx->deinit_pending = true;
            /* ble_esl_stop will call ble_gap_terminate. A failed PAwR teardown
             * is logged there and retried below, so deinit continues either way. */
            (void)ble_esl_stop();
            /* Wait for disconnect event (up to 5 seconds). If the handle was
             * already stale, ble_esl_stop() cleared it and no event is coming,
             * so there is nothing to wait for. */
            if (s_ctx->conn_handle != BLE_HS_CONN_HANDLE_NONE &&
                    xSemaphoreTake(s_ctx->deinit_sem, pdMS_TO_TICKS(5000)) != pdTRUE) {
                ESP_LOGW(TAG, "Timed out waiting for disconnect during deinit");
            }
            vSemaphoreDelete(s_ctx->deinit_sem);
            s_ctx->deinit_sem = NULL;
            s_ctx->deinit_pending = false;
        } else {
            ESP_LOGW(TAG, "Failed to create deinit semaphore — proceeding without wait");
            (void)ble_esl_stop();
        }
    } else if (s_ctx->started) {
        (void)ble_esl_stop();
    }

    /* Stop timers */
    esp_timer_stop(s_ctx->sync_timer);
    esp_timer_stop(s_ctx->unsync_timer);
    esp_timer_stop(s_ctx->ecp_timer);
    esp_timer_delete(s_ctx->sync_timer);
    esp_timer_delete(s_ctx->unsync_timer);
    esp_timer_delete(s_ctx->ecp_timer);

    /* Tear down the controller-level PAwR sync before dropping the context so no
     * periodic report/sync-lost event can reference freed state. This also acts
     * as the retry for a teardown that ble_esl_stop() could not complete. */
    if (esl_terminate_pawr_sync() != ESP_OK) {
        ESP_LOGW(TAG, "PAwR sync %u still active while deinitializing",
                 s_ctx->pawr_sync_handle);
    }

    /* Deinit sub-modules */
    esl_pawr_deinit();
    esl_cmd_led_deinit();
    esl_cmd_display_deinit();
    esl_cmd_lifecycle_deinit();
    esl_gatts_deinit();

    /* Free context */
    free(s_ctx);
    s_ctx = NULL;

    ESP_LOGI(TAG, "ESL deinitialized");
    return ESP_OK;
}

esp_err_t ble_esl_start(void)
{
    if (s_ctx == NULL || !s_ctx->initialized) {
        ESP_LOGE(TAG, "Not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (s_ctx->started) {
        ESP_LOGW(TAG, "Already started");
        return ESP_ERR_INVALID_STATE;
    }

    s_ctx->started = true;

    /* Resume behavior based on current state */
    switch (s_ctx->state) {
    case BLE_ESL_STATE_UNASSOCIATED:
        {
            esp_err_t err = esl_start_advertising();
            if (err != ESP_OK) {
                s_ctx->started = false;
                return err;
            }
        }
        break;

    case BLE_ESL_STATE_UNSYNCHRONIZED:
        {
            esp_err_t err = esl_start_advertising();
            if (err != ESP_OK) {
                s_ctx->started = false;
                return err;
            }
            /* Re-arm unsync timeout that was stopped by ble_esl_stop() */
            esp_timer_start_once(s_ctx->unsync_timer, ESL_STATE_TIMEOUT_US);
        }
        break;

    case BLE_ESL_STATE_SYNCHRONIZED:
        /* Re-arm sync timeout that was stopped by ble_esl_stop() */
        esp_timer_start_once(s_ctx->sync_timer, ESL_STATE_TIMEOUT_US);
        break;

    default:
        /* For other states (CONFIGURING, UPDATING), no-op */
        break;
    }

    ESP_LOGI(TAG, "ESL started");
    return ESP_OK;
}

esp_err_t ble_esl_stop(void)
{
    if (s_ctx == NULL || !s_ctx->started) {
        ESP_LOGE(TAG, "Not started");
        return ESP_ERR_INVALID_STATE;
    }

    s_ctx->started = false;

    /* Stop advertising */
    esl_stop_advertising();

    /* Stop PAwR synchronization if active. On failure the handle and the synced
     * flag are retained (the host still owns the sync), so the stop is reported
     * as incomplete instead of pretending we are unsynced. */
    esp_err_t sync_err = esl_terminate_pawr_sync();

    /* Disconnect if connected. The handle is normally cleared by the disconnect
     * event handler so that it can still match the event and run its cleanup. */
    esl_terminate_active_conn();

    /* Stop timers but preserve state */
    esp_timer_stop(s_ctx->sync_timer);
    esp_timer_stop(s_ctx->unsync_timer);

    if (sync_err != ESP_OK) {
        ESP_LOGW(TAG, "ESL stopped, but PAwR sync %u could not be terminated",
                 s_ctx->pawr_sync_handle);
        return sync_err;
    }

    ESP_LOGI(TAG, "ESL stopped");
    return ESP_OK;
}

ble_esl_state_t ble_esl_get_state(void)
{
    if (s_ctx == NULL) {
        return BLE_ESL_STATE_UNASSOCIATED;
    }
    return s_ctx->state;
}

esp_err_t ble_esl_register_cb(ble_esl_cb_t callback)
{
    if (callback == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (s_ctx == NULL) {
        ESP_LOGE(TAG, "Not initialized — call ble_esl_init() first");
        return ESP_ERR_INVALID_STATE;
    }

    s_ctx->app_cb = callback;
    ESP_LOGI(TAG, "Application callback registered");
    return ESP_OK;
}
