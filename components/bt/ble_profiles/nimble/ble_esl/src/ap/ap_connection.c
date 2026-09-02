/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file ble_esl_ap_connection.c
 * @brief ESL AP — Connection management, service/characteristic discovery,
 *        and internal GATT read/write/ECP helpers.
 */

#include <string.h>
#include "esp_log.h"
#include "esp_err.h"

#include "nimble/ble.h"
#include "host/ble_hs.h"
#include "host/ble_gap.h"

#include "ble_esl_ap.h"
#include "ble_esl_ap_int.h"

static const char *TAG = "esl_ap_conn";

/** PAwR advertising instance (shared with ap_pawr.c / ap_lifecycle.c) */
#define PAWR_ADV_INSTANCE   0

/** Connection-establishment timeout for the PAwR connection procedure (ms) */
#define PAWR_CONNECT_TIMEOUT_MS  30000

/* ========================== Global State ========================== */

ble_esl_ap_state_t *g_esl_ap = NULL;

/* ========================== Forward Declarations ========================== */

static int ble_esl_ap_gap_event(struct ble_gap_event *event, void *arg);
static void handle_scan_result(struct ble_gap_event *event);
static void handle_connect_event(struct ble_gap_event *event, void *arg);
static void handle_disconnect_event(struct ble_gap_event *event);
static void handle_enc_change(struct ble_gap_event *event);
static void handle_pairing_complete(struct ble_gap_event *event);
static void handle_notify_rx(struct ble_gap_event *event);


/* ========================== Helper Functions ========================== */

ble_esl_ap_conn_t *ble_esl_ap_find_conn(uint16_t conn_handle)
{
    if (!g_esl_ap) {
        return NULL;
    }
    for (int i = 0; i < CONFIG_BLE_ESL_AP_MAX_CONNECTIONS; i++) {
        if (g_esl_ap->conns[i].in_use &&
            g_esl_ap->conns[i].conn_handle == conn_handle) {
            return &g_esl_ap->conns[i];
        }
    }
    return NULL;
}

ble_esl_ap_conn_t *ble_esl_ap_alloc_conn(void)
{
    if (!g_esl_ap) {
        return NULL;
    }
    for (int i = 0; i < CONFIG_BLE_ESL_AP_MAX_CONNECTIONS; i++) {
        if (!g_esl_ap->conns[i].in_use) {
            memset(&g_esl_ap->conns[i], 0, sizeof(ble_esl_ap_conn_t));
            g_esl_ap->conns[i].in_use = true;
            g_esl_ap->conns[i].conn_handle = BLE_ESL_AP_CONN_HANDLE_INVALID;
            g_esl_ap->conns[i].esl_addr = BLE_ESL_AP_CONN_HANDLE_INVALID;
            return &g_esl_ap->conns[i];
        }
    }
    return NULL;
}

void ble_esl_ap_free_conn(ble_esl_ap_conn_t *conn)
{
    if (conn) {
        memset(conn, 0, sizeof(ble_esl_ap_conn_t));
        conn->in_use = false;
        conn->conn_handle = BLE_ESL_AP_CONN_HANDLE_INVALID;
    }
}

ble_esl_ap_esl_entry_t *ble_esl_ap_find_esl(uint16_t esl_addr)
{
    if (!g_esl_ap) {
        return NULL;
    }
    for (int i = 0; i < CONFIG_BLE_ESL_AP_MAX_ESLS; i++) {
        if (g_esl_ap->esls[i].in_use &&
            g_esl_ap->esls[i].esl_addr == esl_addr) {
            return &g_esl_ap->esls[i];
        }
    }
    return NULL;
}

ble_esl_ap_esl_entry_t *ble_esl_ap_find_esl_by_ble_addr(const uint8_t *addr,
                                                         uint8_t addr_type)
{
    if (!g_esl_ap || !addr) {
        return NULL;
    }
    for (int i = 0; i < CONFIG_BLE_ESL_AP_MAX_ESLS; i++) {
        if (g_esl_ap->esls[i].in_use &&
            g_esl_ap->esls[i].ble_addr_type == addr_type &&
            memcmp(g_esl_ap->esls[i].ble_addr, addr, 6) == 0) {
            return &g_esl_ap->esls[i];
        }
    }
    return NULL;
}

ble_esl_ap_esl_entry_t *ble_esl_ap_alloc_esl(void)
{
    if (!g_esl_ap) {
        return NULL;
    }
    for (int i = 0; i < CONFIG_BLE_ESL_AP_MAX_ESLS; i++) {
        if (!g_esl_ap->esls[i].in_use) {
            memset(&g_esl_ap->esls[i], 0, sizeof(ble_esl_ap_esl_entry_t));
            g_esl_ap->esls[i].in_use = true;
            g_esl_ap->esls[i].conn_handle = BLE_ESL_AP_CONN_HANDLE_INVALID;
            return &g_esl_ap->esls[i];
        }
    }
    return NULL;
}

bool ble_esl_ap_is_associated(const uint8_t *addr, uint8_t addr_type)
{
    return (ble_esl_ap_find_esl_by_ble_addr(addr, addr_type) != NULL);
}

/* ========================== Public APIs ========================== */

esp_err_t ble_esl_ap_init(const ble_esl_ap_config_t *config)
{
    if (!config || !config->callback) {
        return ESP_ERR_INVALID_ARG;
    }
    /* Zero subevents would leave the PAwR train with nothing to transmit in and
     * is used as a divisor when servicing subevent data requests. */
    if (config->pawr_config.num_subevents == 0 ||
        config->pawr_config.num_subevents > BLE_ESL_AP_PAWR_MAX_SUBEVENTS) {
        ESP_LOGE(TAG, "num_subevents %u out of range (1..%d)",
                 config->pawr_config.num_subevents, BLE_ESL_AP_PAWR_MAX_SUBEVENTS);
        return ESP_ERR_INVALID_ARG;
    }
    if (g_esl_ap) {
        ESP_LOGE(TAG, "Already initialized");
        return ESP_ERR_INVALID_STATE;
    }

    g_esl_ap = calloc(1, sizeof(ble_esl_ap_state_t));
    if (!g_esl_ap) {
        ESP_LOGE(TAG, "Failed to allocate AP state");
        return ESP_ERR_NO_MEM;
    }

    g_esl_ap->app_cb = config->callback;
    g_esl_ap->pawr_config = config->pawr_config;
    g_esl_ap->initialized = true;
    g_esl_ap->started = false;
    g_esl_ap->pawr_active = false;

    /* Initialize connection table */
    for (int i = 0; i < CONFIG_BLE_ESL_AP_MAX_CONNECTIONS; i++) {
        g_esl_ap->conns[i].in_use = false;
        g_esl_ap->conns[i].conn_handle = BLE_ESL_AP_CONN_HANDLE_INVALID;
    }

    /* Initialize ESL tracking table */
    for (int i = 0; i < CONFIG_BLE_ESL_AP_MAX_ESLS; i++) {
        g_esl_ap->esls[i].in_use = false;
        g_esl_ap->esls[i].conn_handle = BLE_ESL_AP_CONN_HANDLE_INVALID;
    }

    /* Initialize sub-modules */
    esp_err_t ret = ble_esl_ap_lifecycle_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init lifecycle sub-module: %s", esp_err_to_name(ret));
        free(g_esl_ap);
        g_esl_ap = NULL;
        return ret;
    }

    ret = ble_esl_ap_pawr_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init PAwR sub-module: %s", esp_err_to_name(ret));
        ble_esl_ap_lifecycle_deinit();
        free(g_esl_ap);
        g_esl_ap = NULL;
        return ret;
    }

    ret = ble_esl_ap_command_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init command sub-module: %s", esp_err_to_name(ret));
        ble_esl_ap_pawr_deinit();
        ble_esl_ap_lifecycle_deinit();
        free(g_esl_ap);
        g_esl_ap = NULL;
        return ret;
    }

    ret = ble_esl_ap_gattc_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init GATT client: %s", esp_err_to_name(ret));
        ble_esl_ap_command_deinit();
        ble_esl_ap_pawr_deinit();
        ble_esl_ap_lifecycle_deinit();
        free(g_esl_ap);
        g_esl_ap = NULL;
        return ret;
    }

    ESP_LOGI(TAG, "ESL AP initialized");
    return ESP_OK;
}

/**
 * @brief Run the disconnect cleanup for one established link during deinit
 *
 * ble_gap_terminate() is asynchronous, so the BLE_GAP_EVENT_DISCONNECT for a
 * link torn down by ble_esl_ap_deinit() is delivered after the module state has
 * been freed and is therefore dropped by the GAP event handler. The per-link
 * cleanup normally performed by handle_disconnect_event() must consequently be
 * done synchronously here.
 *
 * @param conn Connection context with a valid conn_handle
 */
static void deinit_teardown_conn(ble_esl_ap_conn_t *conn)
{
    uint16_t conn_handle = conn->conn_handle;

    /* Fire disconnected event */
    if (g_esl_ap->app_cb) {
        ble_esl_ap_disconn_info_t info;
        info.conn_handle = conn_handle;
        memcpy(info.addr, conn->addr, 6);
        info.reason = BLE_ERR_REM_USER_CONN_TERM;
        g_esl_ap->app_cb(BLE_ESL_AP_EVT_DISCONNECTED, &info);
    }

    /* Invoke any pending ECP callback with error before clearing the slot */
    if (conn->ecp_pending && conn->ecp_cb) {
        ble_esl_ap_gatt_cb_t cb = conn->ecp_cb;
        void *user_data = conn->ecp_user_data;
        conn->ecp_pending = false;
        conn->ecp_cb = NULL;
        conn->ecp_user_data = NULL;
        cb(conn_handle, ESP_FAIL, NULL, 0, user_data);
    }

    /* Clean up any pending ECP command context and timer in the command module */
    ble_esl_ap_command_cleanup_conn(conn_handle);

    /* Let lifecycle module complete any pending synchronize procedure
     * and handle link-loss state transitions */
    ble_esl_ap_lifecycle_handle_disconnect(conn_handle);
}

esp_err_t ble_esl_ap_deinit(void)
{
    if (!g_esl_ap || !g_esl_ap->initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    /* Stop scanning and PAwR if active */
    if (g_esl_ap->started) {
        ble_esl_ap_stop();
    }

    /* Mark the module as no longer operational before touching connections:
     * from here on the GAP event handler drops every asynchronously delivered
     * event, so no callback can dereference the state (nor the conns[] slot
     * passed as GAP cb_arg) while it is being torn down and freed below. */
    g_esl_ap->initialized = false;

    /* Cancel any pending (not yet established) connection attempts */
    ble_gap_conn_cancel();

    /* Disconnect all active connections and free every slot synchronously */
    esp_err_t ret = ESP_OK;
    for (int i = 0; i < CONFIG_BLE_ESL_AP_MAX_CONNECTIONS; i++) {
        ble_esl_ap_conn_t *conn = &g_esl_ap->conns[i];
        if (!conn->in_use) {
            continue;
        }
        if (conn->conn_handle != BLE_ESL_AP_CONN_HANDLE_INVALID) {
            int rc = ble_gap_terminate(conn->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
            /* ENOTCONN (link already gone), EALREADY (termination already in
             * progress) and EDISABLED (host stopped, so no link survives) all
             * mean the link is on its way out: cleaning the slot up is safe.
             * Any other error means the controller link is still up — keep the
             * slot so the link stays tracked and abort the deinit below. */
            if (rc != 0 && rc != BLE_HS_ENOTCONN && rc != BLE_HS_EALREADY &&
                rc != BLE_HS_EDISABLED) {
                ESP_LOGE(TAG, "Failed to terminate conn_handle=%u; rc=%d",
                         conn->conn_handle, rc);
                ret = ESP_FAIL;
                continue;
            }
            deinit_teardown_conn(conn);
        }
        ble_esl_ap_free_conn(conn);
    }

    if (ret != ESP_OK) {
        /* At least one link could not be terminated. Tearing the module down
         * now would leak it: the application would be told the link is gone
         * while the controller keeps it up, and the later GAP events would be
         * dropped. Return to the initialized (stopped) state instead so the
         * remaining links stay tracked and the caller can retry deinit. */
        g_esl_ap->initialized = true;
        ESP_LOGE(TAG, "Deinit aborted: connection termination failed");
        return ret;
    }

    /* Deinitialize sub-modules */
    ble_esl_ap_gattc_deinit();
    ble_esl_ap_command_deinit();
    ble_esl_ap_pawr_deinit();
    ble_esl_ap_lifecycle_deinit();

    /* Free global state — set pointer to NULL first to prevent
     * asynchronous callbacks from dereferencing freed memory */
    ble_esl_ap_state_t *ap = g_esl_ap;
    g_esl_ap = NULL;
    free(ap);

    ESP_LOGI(TAG, "ESL AP deinitialized");
    return ESP_OK;
}

esp_err_t ble_esl_ap_start(void)
{
    if (!g_esl_ap || !g_esl_ap->initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    if (g_esl_ap->started) {
        ESP_LOGW(TAG, "Already started");
        return ESP_ERR_INVALID_STATE;
    }

    /* Start GAP General Discovery (scanning) */
    uint8_t own_addr_type;
    int rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to determine address type; rc=%d", rc);
        return ESP_FAIL;
    }

    struct ble_gap_disc_params disc_params = {0};
    disc_params.filter_duplicates = 1;
    disc_params.passive = 0;  /* Active scan for General Discovery */
    disc_params.itvl = 0;     /* Use defaults */
    disc_params.window = 0;
    disc_params.filter_policy = 0;
    disc_params.limited = 0;

    rc = ble_gap_disc(own_addr_type, BLE_HS_FOREVER, &disc_params,
                      ble_esl_ap_gap_event, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to start GAP discovery; rc=%d", rc);
        return ESP_FAIL;
    }

    /* Start PAwR broadcasting */
    esp_err_t ret = ble_esl_ap_pawr_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start PAwR: %s", esp_err_to_name(ret));
        ble_gap_disc_cancel();
        return ret;
    }

    g_esl_ap->started = true;
    ESP_LOGI(TAG, "ESL AP started (scanning + PAwR)");
    return ESP_OK;
}

esp_err_t ble_esl_ap_stop(void)
{
    if (!g_esl_ap || !g_esl_ap->initialized || !g_esl_ap->started) {
        return ESP_ERR_INVALID_STATE;
    }

    /* Cancel any pending (not yet established) connection attempts */
    ble_gap_conn_cancel();

    /* Stop scanning */
    if (ble_gap_disc_active()) {
        int rc = ble_gap_disc_cancel();
        if (rc != 0 && rc != BLE_HS_EALREADY) {
            ESP_LOGW(TAG, "Failed to cancel discovery; rc=%d", rc);
        }
    }

    /* Stop PAwR broadcasting */
    esp_err_t ret = ble_esl_ap_pawr_stop();
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Failed to stop PAwR: %s", esp_err_to_name(ret));
    }

    g_esl_ap->started = false;
    ESP_LOGI(TAG, "ESL AP stopped");
    return ESP_OK;
}

esp_err_t ble_esl_ap_connect(const uint8_t *addr, uint8_t addr_type)
{
    if (!addr) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!g_esl_ap || !g_esl_ap->initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    /* Check if we have a free connection slot */
    ble_esl_ap_conn_t *conn = ble_esl_ap_alloc_conn();
    if (!conn) {
        ESP_LOGE(TAG, "Max connections reached");
        return ESP_ERR_NO_MEM;
    }

    /* Store address in the pre-allocated slot */
    memcpy(conn->addr, addr, 6);
    conn->addr_type = addr_type;

    /* Build peer address */
    ble_addr_t peer_addr;
    peer_addr.type = addr_type;
    memcpy(peer_addr.val, addr, 6);

    uint8_t own_addr_type;
    int rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to determine address type; rc=%d", rc);
        ble_esl_ap_free_conn(conn);
        return ESP_FAIL;
    }

    /* Stop scanning before connecting (NimBLE requirement unless
     * BLE_HOST_ALLOW_CONNECT_WITH_SCAN is enabled) */
    if (ble_gap_disc_active()) {
        rc = ble_gap_disc_cancel();
        if (rc != 0 && rc != BLE_HS_EALREADY) {
            ESP_LOGW(TAG, "Failed to cancel scan before connect; rc=%d", rc);
        }
    }

    /* Initiate connection with default parameters, 30s timeout.
     * Pass conn as cb_arg so the GAP event handler can directly
     * identify the connection slot without guessing. */
    rc = ble_gap_connect(own_addr_type, &peer_addr, 30000, NULL,
                         ble_esl_ap_gap_event, conn);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to initiate connection; rc=%d", rc);
        ble_esl_ap_free_conn(conn);

        /* Resume scanning if we were started */
        if (g_esl_ap->started) {
            struct ble_gap_disc_params disc_params = {0};
            disc_params.filter_duplicates = 1;
            disc_params.passive = 0;
            ble_gap_disc(own_addr_type, BLE_HS_FOREVER, &disc_params,
                         ble_esl_ap_gap_event, NULL);
        }
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Connection initiated to %02x:%02x:%02x:%02x:%02x:%02x",
             addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
    return ESP_OK;
}

esp_err_t ble_esl_ap_connect_synced(ble_esl_address_t esl_addr)
{
    if (!g_esl_ap || !g_esl_ap->initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    uint16_t addr_key = BLE_ESL_AP_ADDR_PACK(esl_addr);

    /* The ESL must be tracked and currently Synchronized to our PAwR train.
     * Only a Synchronized ESL is reachable via the Periodic Advertising
     * Connection procedure; in other states use ble_esl_ap_connect(). */
    ble_esl_ap_esl_entry_t *esl = ble_esl_ap_find_esl(addr_key);
    if (!esl) {
        ESP_LOGE(TAG, "connect_synced: ESL 0x%04X not tracked", addr_key);
        return ESP_ERR_NOT_FOUND;
    }
    if (esl->state != BLE_ESL_STATE_SYNCHRONIZED) {
        ESP_LOGE(TAG, "connect_synced: ESL 0x%04X not Synchronized (state=%d)",
                 addr_key, esl->state);
        return ESP_ERR_INVALID_STATE;
    }

    /* Reserve a connection slot; the GAP event handler identifies it via cb_arg. */
    ble_esl_ap_conn_t *conn = ble_esl_ap_alloc_conn();
    if (!conn) {
        ESP_LOGE(TAG, "connect_synced: max connections reached");
        return ESP_ERR_NO_MEM;
    }
    memcpy(conn->addr, esl->ble_addr, 6);
    conn->addr_type = esl->ble_addr_type;
    conn->esl_addr = addr_key;

    ble_addr_t peer_addr;
    peer_addr.type = esl->ble_addr_type;
    memcpy(peer_addr.val, esl->ble_addr, 6);

    uint8_t own_addr_type;
    int rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        ESP_LOGE(TAG, "connect_synced: failed to determine address type; rc=%d", rc);
        ble_esl_ap_free_conn(conn);
        return ESP_FAIL;
    }

    /* Initiating a connection is not possible while scanning is in progress. */
    if (ble_gap_disc_active()) {
        rc = ble_gap_disc_cancel();
        if (rc != 0 && rc != BLE_HS_EALREADY) {
            ESP_LOGW(TAG, "connect_synced: failed to cancel scan; rc=%d", rc);
        }
    }

    /* Periodic Advertising Connection procedure: send the AUX_CONNECT_REQ from
     * the subevent this ESL responds in — its Group_ID — on our own PAwR train
     * (PAWR_ADV_INSTANCE). NULL phy params request controller defaults. */
    uint8_t subevent = BLE_ESL_ADDR_GROUP_ID(esl_addr);
    rc = ble_gap_connect_with_synced(own_addr_type, PAWR_ADV_INSTANCE, subevent,
                                     &peer_addr, PAWR_CONNECT_TIMEOUT_MS,
                                     BLE_GAP_LE_PHY_1M_MASK, NULL, NULL, NULL,
                                     ble_esl_ap_gap_event, conn);
    if (rc != 0) {
        ESP_LOGE(TAG, "connect_synced: ble_gap_connect_with_synced failed; rc=%d", rc);
        ble_esl_ap_free_conn(conn);

        /* Resume scanning if we were started */
        if (g_esl_ap->started && !ble_gap_disc_active()) {
            struct ble_gap_disc_params disc_params = {0};
            disc_params.filter_duplicates = 1;
            disc_params.passive = 0;
            ble_gap_disc(own_addr_type, BLE_HS_FOREVER, &disc_params,
                         ble_esl_ap_gap_event, NULL);
        }
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "connect_synced: PAwR connection initiated to ESL 0x%04X (subevent=%u)",
             addr_key, subevent);
    return ESP_OK;
}

esp_err_t ble_esl_ap_disconnect(uint16_t conn_handle)
{
    if (!g_esl_ap || !g_esl_ap->initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(conn_handle);
    if (!conn) {
        return ESP_ERR_NOT_FOUND;
    }

    int rc = ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to terminate connection; rc=%d", rc);
        return ESP_FAIL;
    }

    return ESP_OK;
}

/* ========================== GAP Event Handler ========================== */

static int ble_esl_ap_gap_event(struct ble_gap_event *event, void *arg)
{
    /* ble_esl_ap_deinit() clears `initialized` before tearing the module down,
     * so any event delivered from that point on must be dropped: the state —
     * including the conns[] slot passed as `arg` — is being freed. */
    if (!g_esl_ap || !g_esl_ap->initialized) {
        if (event->type == BLE_GAP_EVENT_CONNECT && event->connect.status == 0) {
            /* A link came up while shutting down; drop it so it is not leaked */
            ble_gap_terminate(event->connect.conn_handle,
                              BLE_ERR_REM_USER_CONN_TERM);
        }
        return 0;
    }

    switch (event->type) {
    case BLE_GAP_EVENT_DISC:
    case BLE_GAP_EVENT_EXT_DISC:
        handle_scan_result(event);
        return 0;

    case BLE_GAP_EVENT_CONNECT:
        handle_connect_event(event, arg);
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        handle_disconnect_event(event);
        return 0;

    case BLE_GAP_EVENT_ENC_CHANGE:
        handle_enc_change(event);
        return 0;

    case BLE_GAP_EVENT_PARING_COMPLETE:
        handle_pairing_complete(event);
        return 0;

    case BLE_GAP_EVENT_NOTIFY_RX:
        handle_notify_rx(event);
        return 0;

    case BLE_GAP_EVENT_DISC_COMPLETE:
        ESP_LOGI(TAG, "Discovery complete; reason=%d", event->disc_complete.reason);
        return 0;

    case BLE_GAP_EVENT_MTU:
        ESP_LOGD(TAG, "MTU update; conn_handle=%d mtu=%d",
                 event->mtu.conn_handle, event->mtu.value);
        return 0;

    default:
        return 0;
    }
}

/* ========================== Scan Result Handling ========================== */

static void handle_scan_result(struct ble_gap_event *event)
{
    if (!g_esl_ap || !g_esl_ap->app_cb) {
        return;
    }

    ble_addr_t addr;
    int8_t rssi;
    const uint8_t *data;
    uint8_t length_data;

    if (event->type == BLE_GAP_EVENT_EXT_DISC) {
        const struct ble_gap_ext_disc_desc *ext = &event->ext_disc;
        addr        = ext->addr;
        rssi        = ext->rssi;
        data        = ext->data;
        length_data = ext->length_data;
    } else {
        const struct ble_gap_disc_desc *disc = &event->disc;
        addr        = disc->addr;
        rssi        = disc->rssi;
        data        = disc->data;
        length_data = disc->length_data;
    }

    bool associated = ble_esl_ap_is_associated(addr.val, addr.type);

    ble_esl_ap_scan_result_t result;
    memcpy(result.addr, addr.val, 6);
    result.addr_type    = addr.type;
    result.rssi         = rssi;
    result.adv_data     = data;
    result.adv_data_len = length_data;
    result.is_associated = associated;

    g_esl_ap->app_cb(BLE_ESL_AP_EVT_SCAN_RESULT, &result);
}

/* ========================== Connect Event Handling ========================== */

static void handle_connect_event(struct ble_gap_event *event, void *arg)
{
    if (!g_esl_ap) {
        return;
    }

    uint16_t conn_handle = event->connect.conn_handle;
    ble_esl_ap_conn_t *conn = (ble_esl_ap_conn_t *)arg;

    if (event->connect.status != 0) {
        /* Connection failed */
        ESP_LOGE(TAG, "Connection failed; status=%d", event->connect.status);

        /* Use the conn pointer passed as cb_arg to directly identify the slot */
        if (conn && conn->in_use &&
            conn->conn_handle == BLE_ESL_AP_CONN_HANDLE_INVALID) {
            /* Fire disconnected event */
            if (g_esl_ap->app_cb) {
                ble_esl_ap_disconn_info_t info;
                info.conn_handle = BLE_ESL_AP_CONN_HANDLE_INVALID;
                memcpy(info.addr, conn->addr, 6);
                info.reason = (uint8_t)event->connect.status;
                g_esl_ap->app_cb(BLE_ESL_AP_EVT_DISCONNECTED, &info);
            }
            ble_esl_ap_free_conn(conn);
        }

        /* Resume scanning if started */
        if (g_esl_ap->started && !ble_gap_disc_active()) {
            uint8_t own_addr_type;
            if (ble_hs_id_infer_auto(0, &own_addr_type) == 0) {
                struct ble_gap_disc_params disc_params = {0};
                disc_params.filter_duplicates = 1;
                disc_params.passive = 0;
                ble_gap_disc(own_addr_type, BLE_HS_FOREVER, &disc_params,
                             ble_esl_ap_gap_event, NULL);
            }
        }
        return;
    }

    /* Connection established successfully */
    ESP_LOGI(TAG, "Connection established; conn_handle=%d", conn_handle);

    /* Get connection descriptor to retrieve peer identity address */
    struct ble_gap_conn_desc desc;
    int rc = ble_gap_conn_find(conn_handle, &desc);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to find connection desc; rc=%d", rc);

        /* Free the pre-allocated connection slot to prevent a leak */
        if (conn && conn->in_use &&
            conn->conn_handle == BLE_ESL_AP_CONN_HANDLE_INVALID) {
            if (g_esl_ap->app_cb) {
                ble_esl_ap_disconn_info_t info;
                info.conn_handle = BLE_ESL_AP_CONN_HANDLE_INVALID;
                memcpy(info.addr, conn->addr, 6);
                info.reason = BLE_ERR_UNSPECIFIED;
                g_esl_ap->app_cb(BLE_ESL_AP_EVT_DISCONNECTED, &info);
            }
            ble_esl_ap_free_conn(conn);
        }

        ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);

        /* Resume scanning if started */
        if (g_esl_ap->started && !ble_gap_disc_active()) {
            uint8_t own_addr_type;
            if (ble_hs_id_infer_auto(0, &own_addr_type) == 0) {
                struct ble_gap_disc_params disc_params = {0};
                disc_params.filter_duplicates = 1;
                disc_params.passive = 0;
                ble_gap_disc(own_addr_type, BLE_HS_FOREVER, &disc_params,
                             ble_esl_ap_gap_event, NULL);
            }
        }
        return;
    }

    if (!conn || !conn->in_use ||
        conn->conn_handle != BLE_ESL_AP_CONN_HANDLE_INVALID) {
        ESP_LOGE(TAG, "No valid pre-allocated conn slot for handle=%d", conn_handle);
        ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);

        /* Resume scanning if started */
        if (g_esl_ap->started && !ble_gap_disc_active()) {
            uint8_t own_addr_type;
            if (ble_hs_id_infer_auto(0, &own_addr_type) == 0) {
                struct ble_gap_disc_params disc_params = {0};
                disc_params.filter_duplicates = 1;
                disc_params.passive = 0;
                ble_gap_disc(own_addr_type, BLE_HS_FOREVER, &disc_params,
                             ble_esl_ap_gap_event, NULL);
            }
        }
        return;
    }

    /* Populate connection context using the stable identity address
     * (peer_id_addr) instead of the temporary OTA address (peer_ota_addr)
     * to ensure correct ESL tracking across reconnections with RPAs. */
    conn->conn_handle = conn_handle;
    memcpy(conn->addr, desc.peer_id_addr.val, 6);
    conn->addr_type = desc.peer_id_addr.type;
    memset(&conn->handles, 0, sizeof(ble_esl_ap_char_handles_t));
    conn->ecp_pending = false;

    /* Start security */
    rc = ble_gap_security_initiate(conn_handle);
    if (rc != 0 && rc != BLE_HS_EALREADY) {
        ESP_LOGW(TAG, "Failed to initiate encryption; conn_handle=%u rc=%d",
                 conn_handle, rc);
    }
    ESP_LOGI(TAG, "Initiate encryption; conn_handle=%u", conn_handle);

    /* Resume scanning if started */
    if (g_esl_ap->started && !ble_gap_disc_active()) {
        uint8_t own_addr_type;
        if (ble_hs_id_infer_auto(0, &own_addr_type) == 0) {
            struct ble_gap_disc_params disc_params = {0};
            disc_params.filter_duplicates = 1;
            disc_params.passive = 0;
            ble_gap_disc(own_addr_type, BLE_HS_FOREVER, &disc_params,
                         ble_esl_ap_gap_event, NULL);
        }
    }
}

/* ========================== Disconnect Event Handling ========================== */

static void handle_disconnect_event(struct ble_gap_event *event)
{
    if (!g_esl_ap) {
        return;
    }

    uint16_t conn_handle = event->disconnect.conn.conn_handle;
    int reason = event->disconnect.reason;

    ESP_LOGI(TAG, "Disconnected; conn_handle=%d reason=%d", conn_handle, reason);

    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(conn_handle);
    if (!conn) {
        ESP_LOGW(TAG, "Disconnect for unknown conn_handle=%d", conn_handle);
        return;
    }

    /* Fire disconnected event */
    if (g_esl_ap->app_cb) {
        ble_esl_ap_disconn_info_t info;
        info.conn_handle = conn_handle;
        memcpy(info.addr, conn->addr, 6);
        info.reason = (uint8_t)reason;
        g_esl_ap->app_cb(BLE_ESL_AP_EVT_DISCONNECTED, &info);
    }

    /* Invoke any pending ECP callback with error before clearing the slot */
    if (conn->ecp_pending && conn->ecp_cb) {
        ble_esl_ap_gatt_cb_t cb = conn->ecp_cb;
        void *user_data = conn->ecp_user_data;
        conn->ecp_pending = false;
        conn->ecp_cb = NULL;
        conn->ecp_user_data = NULL;
        cb(conn_handle, ESP_FAIL, NULL, 0, user_data);
    }

    /* Clean up any pending ECP command context and timer in the command module */
    ble_esl_ap_command_cleanup_conn(conn_handle);

    /* Let lifecycle module complete any pending synchronize procedure
     * and handle link-loss state transitions */
    ble_esl_ap_lifecycle_handle_disconnect(conn_handle);

    /* Free connection slot */
    ble_esl_ap_free_conn(conn);

    /* Resume scanning if started */
    if (g_esl_ap->started && !ble_gap_disc_active()) {
        uint8_t own_addr_type;
        if (ble_hs_id_infer_auto(0, &own_addr_type) == 0) {
            struct ble_gap_disc_params disc_params = {0};
            disc_params.filter_duplicates = 1;
            disc_params.passive = 0;
            ble_gap_disc(own_addr_type, BLE_HS_FOREVER, &disc_params,
                         ble_esl_ap_gap_event, NULL);
        }
    }
}

/* ======================================================================== */
/* Handle encryption change                                                 */
/* ======================================================================== */
static void handle_enc_change(struct ble_gap_event *event)
{
    uint16_t conn_handle = event->enc_change.conn_handle;
    uint8_t status = event->enc_change.status;
    ESP_LOGI(TAG, "Encryption change; conn_handle=%u status=%d",
             conn_handle, status);

    if (status != 0) {
        ESP_LOGW(TAG, "Encryption failed for conn_handle=%u; terminating connection", conn_handle);
        ble_gap_terminate(conn_handle, BLE_ERR_AUTH_FAIL);
        return;
    }

    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(conn_handle);
    if (!conn) {
        return;
    }

    /* If the encrypted link is to an already-associated ESL — reconnected from
     * Unsynchronized (connectable advertising) or from Synchronized (Periodic
     * Advertising Connection procedure) — the ESL has entered the Updating
     * state. Track it so ble_esl_ap_configure()/synchronize() see a valid state
     * and link-loss recovery routes it back to Unsynchronized. */
    ble_esl_ap_esl_entry_t *esl = ble_esl_ap_find_esl_by_ble_addr(conn->addr,
                                                                  conn->addr_type);
    if (esl != NULL &&
        (esl->state == BLE_ESL_STATE_SYNCHRONIZED ||
         esl->state == BLE_ESL_STATE_UNSYNCHRONIZED)) {
        esl->conn_handle = conn_handle;
        conn->esl_addr = esl->esl_addr;
        ble_esl_ap_update_esl_state(esl->esl_addr, BLE_ESL_STATE_UPDATING);
    }

    if (conn->disc_done) {
        ESP_LOGD(TAG, "Encryption change on already-discovered conn_handle=%u; skipping discovery",
                 conn_handle);
        return;
    }

    /* Start discovery */
    ble_esl_ap_start_svc_discovery(conn_handle);
}

/* ======================================================================== */
/* Handle pairing complete                                                  */
/* ======================================================================== */
static void handle_pairing_complete(struct ble_gap_event *event)
{
    uint16_t conn_handle = event->pairing_complete.conn_handle;
    uint8_t status = event->pairing_complete.status;
    ESP_LOGI(TAG, "Pairing complete; conn_handle=%u status=%d",
             conn_handle, status);
}

/* ========================== Notification Handling ========================== */

static void handle_notify_rx(struct ble_gap_event *event)
{
    if (!g_esl_ap) {
        return;
    }

    uint16_t conn_handle = event->notify_rx.conn_handle;
    uint16_t attr_handle = event->notify_rx.attr_handle;

    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(conn_handle);
    if (!conn) {
        return;
    }

    /* Check if this is an ECP notification */
    if (attr_handle == conn->handles.ecp_handle && conn->ecp_pending) {
        /* Extract notification data from mbuf */
        uint16_t data_len = OS_MBUF_PKTLEN(event->notify_rx.om);
        uint8_t *data = NULL;
        uint8_t buf[BLE_ESL_AP_READ_BUF_MAX];
        esp_err_t status = ESP_OK;

        if (data_len > 0 && data_len <= sizeof(buf)) {
            int rc = os_mbuf_copydata(event->notify_rx.om, 0, data_len, buf);
            if (rc == 0) {
                data = buf;
            } else {
                data_len = 0;
            }
        } else if (data_len > sizeof(buf)) {
            ESP_LOGW(TAG, "ECP notification too large: %u > %zu", data_len, sizeof(buf));
            data_len = 0;
            status = ESP_FAIL;
        }

        /* Clear pending state and invoke callback */
        conn->ecp_pending = false;
        ble_esl_ap_gatt_cb_t cb = conn->ecp_cb;
        void *user_data = conn->ecp_user_data;
        conn->ecp_cb = NULL;
        conn->ecp_user_data = NULL;

        if (cb) {
            cb(conn_handle, status, data, data_len, user_data);
        } else if (data && data_len > 0) {
            /* Route to command module only when no registered callback handled it,
             * to avoid double-processing and spurious 'no pending command' warnings */
            ble_esl_ap_command_handle_ecp_notification(conn_handle, data, data_len);
        }
    }
}
