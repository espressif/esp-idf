/*
 * SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "utils/includes.h"
#include "utils/common.h"
#include "utils/eloop.h"
#include "common/defs.h"

#include "esp_dpp_i.h"
#include "esp_dpp.h"
#include "esp_wpa.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "common/ieee802_11_defs.h"
#include "common/ieee802_11_common.h"
#include "esp_wps_i.h"
#include "rsn_supp/wpa.h"
#include "rsn_supp/wpa_i.h"
#include "rsn_supp/pmksa_cache.h"
#include <stdatomic.h>
#include <limits.h>

#ifdef CONFIG_DPP

struct action_rx_param {
    u8 sa[ETH_ALEN];
    u32 channel;
    u32 frm_len;
    u32 vendor_data_len;
    struct ieee80211_action action_frm[];
};

static void *s_dpp_api_lock = NULL;
/* Event group for DPP operations (e.g., ROC/Listen).
 * This object is persistent (created in common_init, never deleted)
 */
static void *s_dpp_event_group = NULL;

#define DPP_ROC_EVENT_HANDLED  BIT0
#define DPP_MAX_COMEBACK_DELAY_TU 5000 /* ~5 seconds */

static atomic_bool roc_in_progress;
static atomic_bool dpp_shutting_down;
static atomic_bool s_dpp_init_done;
static bool s_dpp_init_pending;
static struct esp_dpp_context_t s_dpp_ctx;
static int esp_supp_rx_action(uint8_t *hdr, uint8_t *payload, size_t len, uint8_t channel);
static wifi_action_rx_cb_t s_action_rx_cb = esp_supp_rx_action;
static void tx_status_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data);
static void roc_status_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data);
static void dpp_listen_next_channel(void *data, void *user_ctx);
static void esp_dpp_cancel_timeouts(void);
static void dpp_cancel_auth_gas_eloop_timeouts(void);

static void esp_dpp_auth_resp_retry(void *eloop_ctx, void *timeout_ctx);
static void esp_dpp_peer_disc_retry(void *eloop_ctx, void *timeout_ctx);
static void esp_dpp_gas_query_req_retry(void *eloop_ctx, void *timeout_ctx);
static esp_err_t esp_dpp_start_net_intro_protocol_internal(uint8_t *bssid);
static void dpp_stop_internal(void);
static esp_err_t gas_query_req_tx(struct dpp_authentication *auth);
static void gas_query_timeout(void *eloop_data, void *user_ctx);
static void peer_disc_timeout(void *eloop_data, void *user_ctx);

static bool esp_dpp_stored_conf_matches_row(struct dpp_config_store *dc,
                                            const esp_dpp_config_data_t *row);
static esp_err_t esp_dpp_conf_alloc_from_config_data(const esp_dpp_config_data_t *config,
                                                     struct dpp_conf **out_conf);
static struct dpp_conf *esp_dpp_config_store_get_selected_entry(void);

static void dpp_gas_cleanup_locked(void)
{
    wpabuf_clear_free(s_dpp_ctx.gas_resp_buf);
    s_dpp_ctx.gas_resp_buf = NULL;
    s_dpp_ctx.gas_wait_comeback = false;
    s_dpp_ctx.gas_frag_id = 0;
}

static esp_err_t dpp_api_lock(void)
{
    if (!s_dpp_api_lock) {
        return ESP_ERR_INVALID_STATE;
    }

    os_mutex_lock(s_dpp_api_lock);

    return ESP_OK;
}

static esp_err_t dpp_api_unlock(void)
{
    if (s_dpp_api_lock) {
        os_mutex_unlock(s_dpp_api_lock);
    }

    return ESP_OK;
}

static uint8_t dpp_deinit_auth(void)
{
    if (s_dpp_ctx.dpp_auth) {
        dpp_auth_deinit(s_dpp_ctx.dpp_auth);
        s_dpp_ctx.dpp_auth = NULL;
    }

    return ESP_OK;
}

static int listen_stop_handler(void *data, void *user_ctx)
{
    wifi_roc_req_t req = {0};

    if (!atomic_load(&s_dpp_init_done)) {
        return 0;
    }

    wpa_printf(MSG_DEBUG, "DPP: Stoping ROC");
    req.ifx = WIFI_IF_STA;
    req.type = WIFI_ROC_CANCEL;
    eloop_cancel_timeout(dpp_listen_next_channel, NULL, NULL);
    s_dpp_ctx.dpp_listen_ongoing = false;

    esp_wifi_remain_on_channel(&req);

    return 0;
}

/* Teardown only. Call under dpp_api_lock or from dpp_stop() after it acquired the lock. */
static void dpp_stop_internal(void)
{
    dpp_cancel_auth_gas_eloop_timeouts();
    dpp_gas_cleanup_locked();

    if (s_dpp_ctx.dpp_auth) {
        dpp_deinit_auth();
    }

    if (s_dpp_ctx.dpp_listen_ongoing || atomic_load(&roc_in_progress)) {
        listen_stop_handler(NULL, NULL);
    }
}

/* Caller must not hold dpp_api_lock (posts WIFI_EVENT_DPP_FAILED may block). */
static void dpp_post_dpp_failed_event(uint32_t failure_reason)
{
    wifi_event_dpp_failed_t event = {0};
    event.failure_reason = failure_reason;

    esp_err_t ret = esp_event_post(WIFI_EVENT, WIFI_EVENT_DPP_FAILED, &event, sizeof(event), os_task_ms_to_tick(200));
    if (ret != ESP_OK) {
        wpa_printf(MSG_ERROR, "DPP: Failed to post DPP_FAILED event, error 0x%x", ret);
    }
}

/* Helper to stop and post failure while holding dpp_api_lock. */
static void dpp_abort_failure_locked(uint32_t reason)
{
    dpp_stop_internal();
    dpp_api_unlock();
    dpp_post_dpp_failed_event(reason);
}

static void esp_dpp_auth_conf_wait_timeout(void *eloop_ctx, void *timeout_ctx)
{
    /* Prerequisite confirmed via lifecycle (timers cancelled on deinit), lock acquisition will not fail. */
    dpp_api_lock();

    if (atomic_load(&dpp_shutting_down)) {
        dpp_api_unlock();
        return;
    }

    if (!s_dpp_ctx.dpp_auth || !s_dpp_ctx.dpp_auth->waiting_auth_conf) {
        dpp_api_unlock();
        return;
    }

    wpa_printf(MSG_INFO,
               "DPP: Terminate authentication exchange due to Auth Confirm timeout");
    dpp_abort_failure_locked(ESP_ERR_DPP_AUTH_TIMEOUT);
}

esp_err_t esp_dpp_send_action_frame(uint8_t *dest_mac, const uint8_t *buf, uint32_t len,
                                    uint8_t channel, uint32_t wait_time_ms, enum dpp_tx_frame_type type)
{
    wifi_action_tx_req_t *req;

    if (len > SIZE_MAX - sizeof(*req)) {
        return ESP_FAIL;
    }

    req = os_zalloc(sizeof(*req) + len);
    if (!req) {
        return ESP_FAIL;
    }

    req->ifx = WIFI_IF_STA;
    os_memcpy(req->dest_mac, dest_mac, ETH_ALEN);
    req->no_ack = false;
    req->data_len = len;
    req->rx_cb = s_action_rx_cb;
    req->channel = channel;
    req->sec_channel = WIFI_SECOND_CHAN_NONE;
    req->wait_time_ms = wait_time_ms;
    req->type = (type == DPP_TX_PEER_DISCOVERY_REQ) ? WIFI_OFFCHAN_TX_CONNECTING_REQ
                : WIFI_OFFCHAN_TX_REQ;
    os_memcpy(req->data, buf, req->data_len);

    wpa_printf(MSG_DEBUG, "DPP: Mgmt Tx - MAC:" MACSTR ", Channel-%d, WaitT-%d, Type-%d",
               MAC2STR(dest_mac), channel, wait_time_ms, type);

    if (ESP_OK != esp_wifi_action_tx_req(req)) {
        wpa_printf(MSG_ERROR, "DPP: Failed to perform offchannel operation");
        os_free(req);
        return ESP_FAIL;
    }

    wpa_printf(MSG_DEBUG, "Sent DPP action frame %d", req->op_id);
    s_dpp_ctx.pending_tx_op.op_id = req->op_id;
    s_dpp_ctx.pending_tx_op.type = type;
    s_dpp_ctx.pending_tx_op_in_progress = true;
    os_free(req);
    return ESP_OK;
}

static void esp_dpp_auth_resp_retry_timeout(void *eloop_ctx, void *timeout_ctx)
{
    struct dpp_authentication *auth;
    esp_err_t err;

    /* Prerequisite confirmed via lifecycle (timers cancelled on deinit), lock acquisition will not fail. */
    dpp_api_lock();

    if (atomic_load(&dpp_shutting_down)) {
        dpp_api_unlock();
        return;
    }

    auth = s_dpp_ctx.dpp_auth;

    if (!auth || !auth->resp_msg) {
        dpp_api_unlock();
        return;
    }

    wpa_printf(MSG_DEBUG,
               "DPP: Retry Authentication Response after timeout");
    wpa_msg(wpa_s, MSG_INFO, DPP_EVENT_TX "dst=" MACSTR
            " chan=%u type=%d",
            MAC2STR(auth->peer_mac_addr), auth->curr_chan,
            DPP_TX_AUTHENTICATION_RESP);

    err = esp_dpp_send_action_frame(auth->peer_mac_addr,
                                    wpabuf_head(auth->resp_msg),
                                    wpabuf_len(auth->resp_msg),
                                    auth->curr_chan, 1000 + OFFCHAN_TX_WAIT_TIME,
                                    DPP_TX_AUTHENTICATION_RESP);
    if (err != ESP_OK) {
        dpp_abort_failure_locked(ESP_ERR_DPP_TX_FAILURE);
        return;
    }

    dpp_api_unlock();
}
static esp_err_t esp_dpp_rx_auth_req(struct action_rx_param *rx_param, uint8_t *dpp_data)
{
    size_t len = rx_param->vendor_data_len - 2;
    const u8 *r_bootstrap, *i_bootstrap;
    u16 r_bootstrap_len, i_bootstrap_len;
    struct dpp_bootstrap_info *own_bi;
    int rc;

    wpa_printf(MSG_INFO, "DPP: Authentication Request from " MACSTR " chan=%d",
               MAC2STR(rx_param->sa), rx_param->channel);

    r_bootstrap = dpp_get_attr(dpp_data, len, DPP_ATTR_R_BOOTSTRAP_KEY_HASH,
                               &r_bootstrap_len);
    if (!r_bootstrap || r_bootstrap_len != SHA256_MAC_LEN) {
        wpa_printf(MSG_INFO, "DPP: Missing or invalid Responder Bootstrapping Key Hash attribute");
        rc = ESP_ERR_DPP_INVALID_ATTR;
        return rc;
    }
    wpa_hexdump(MSG_MSGDUMP, "DPP: Responder Bootstrapping Key Hash", r_bootstrap, r_bootstrap_len);

    i_bootstrap = dpp_get_attr(dpp_data, len, DPP_ATTR_I_BOOTSTRAP_KEY_HASH,
                               &i_bootstrap_len);
    if (!i_bootstrap || i_bootstrap_len != SHA256_MAC_LEN) {
        wpa_printf(MSG_INFO, "DPP: Missing or invalid Initiator Bootstrapping Key Hash attribute");
        rc = ESP_ERR_DPP_INVALID_ATTR;
        return rc;
    }
    wpa_hexdump(MSG_MSGDUMP, "DPP: Initiator Bootstrapping Key Hash", i_bootstrap, i_bootstrap_len);

    own_bi = dpp_bootstrap_get_id(s_dpp_ctx.dpp_global, s_dpp_ctx.id);
    if (!own_bi) {
        wpa_printf(MSG_ERROR, "DPP: Failed to find responder bootstrap information");
        rc = ESP_ERR_DPP_FAILURE;
        return rc;
    }
    /* Try to find own and peer bootstrapping key matches based on the
     * received hash values */
    if (os_memcmp(own_bi->pubkey_hash, r_bootstrap, SHA256_MAC_LEN)) {
        wpa_printf(MSG_INFO, "DPP: No matching own bootstrapping key found as responder - ignore message");
        rc = ESP_ERR_DPP_INVALID_ATTR;
        return ESP_ERR_DPP_INVALID_ATTR;
    }
    if (s_dpp_ctx.dpp_auth) {
        wpa_printf(MSG_DEBUG, "DPP: Already in DPP authentication exchange - ignore new one");
        return ESP_OK;
    }
    s_dpp_ctx.dpp_auth = dpp_auth_req_rx(NULL, DPP_CAPAB_ENROLLEE, 0, NULL,
                                         own_bi, rx_param->channel,
                                         (const u8 *)&rx_param->action_frm->u.public_action.v, dpp_data, len);
    if (!s_dpp_ctx.dpp_auth || !s_dpp_ctx.dpp_auth->resp_msg) {
        wpa_printf(MSG_ERROR, "DPP: Failed to allocate authentication response");
        if (s_dpp_ctx.dpp_auth) {
            dpp_auth_deinit(s_dpp_ctx.dpp_auth);
            s_dpp_ctx.dpp_auth = NULL;
        }
        rc = ESP_ERR_DPP_FAILURE;
        return rc;
    }
    os_memcpy(s_dpp_ctx.dpp_auth->peer_mac_addr, rx_param->sa, ETH_ALEN);

    wpa_printf(MSG_INFO, "DPP: Sending authentication response chan(%d)", rx_param->channel);
    eloop_cancel_timeout(esp_dpp_auth_resp_retry_timeout, NULL, NULL);
    if (eloop_register_timeout(0, 200000, esp_dpp_auth_resp_retry_timeout, NULL, NULL) < 0) {
        wpa_printf(MSG_ERROR, "DPP: Failed to register auth_resp_retry_timeout");
        return ESP_ERR_NO_MEM;
    }
    esp_err_t err = esp_dpp_send_action_frame(rx_param->sa, wpabuf_head(s_dpp_ctx.dpp_auth->resp_msg),
                                              wpabuf_len(s_dpp_ctx.dpp_auth->resp_msg),
                                              rx_param->channel, 1000 + OFFCHAN_TX_WAIT_TIME,
                                              DPP_TX_AUTHENTICATION_RESP);
    if (err != ESP_OK) {
        return ESP_ERR_DPP_TX_FAILURE;
    }

    return ESP_OK;
}
static int esp_dpp_rx_auth_conf(struct action_rx_param *rx_param, uint8_t *dpp_data)
{
    struct dpp_authentication *auth = s_dpp_ctx.dpp_auth;
    struct ieee80211_public_action *public_action =
            &rx_param->action_frm->u.public_action;
    size_t len = rx_param->vendor_data_len - 2;
    int rc;

    wpa_printf(MSG_INFO, "DPP: Authentication Confirmation from " MACSTR,
               MAC2STR(rx_param->sa));

    if (!auth) {
        wpa_printf(MSG_DEBUG, "DPP: No DPP Authentication in progress - drop");
        return ESP_ERR_DPP_FAILURE;
    }

    if (os_memcmp(rx_param->sa, auth->peer_mac_addr, ETH_ALEN) != 0) {
        wpa_printf(MSG_DEBUG, "DPP: MAC address mismatch (expected "
                   MACSTR ") - drop", MAC2STR(auth->peer_mac_addr));
        return ESP_ERR_DPP_FAILURE;
    }

    eloop_cancel_timeout(esp_dpp_auth_conf_wait_timeout, NULL, NULL);

    if (dpp_auth_conf_rx(auth, (const u8 *)&public_action->v,
                         dpp_data, len) < 0) {
        wpa_printf(MSG_ERROR, "DPP: Authentication failed");
        return ESP_ERR_DPP_FAILURE;
    }

    /* Fresh GAS query for this session: reset retry budget so a previous
     * partial run cannot eat into this session's retries. The retry path
     * (esp_dpp_gas_query_req_retry) bumps the counter itself. */
    s_dpp_ctx.gas_query_tries = 0;

    rc = gas_query_req_tx(auth);
    if (rc != ESP_OK) {
        wpa_printf(MSG_ERROR, "DPP: GAS query Tx failed");
        return rc;
    }

    return rc;
}

static esp_err_t esp_dpp_rx_peer_disc_resp(struct action_rx_param *rx_param)
{
    struct dpp_config_store *dc = s_dpp_ctx.dpp_config_store;
    uint8_t *buf;

    if (!dc) {
        return ESP_ERR_DPP_FAILURE;
    }
    const uint8_t *trans_id;
    uint16_t trans_id_len;
    size_t len = rx_param->vendor_data_len - 2;

    buf = rx_param->action_frm->u.public_action.v.pa_vendor_spec.vendor_data;

    if (os_memcmp(dc->peer_mac_addr, rx_param->sa, ETH_ALEN) != 0) {
        wpa_printf(MSG_DEBUG, "DPP: Not expecting Peer Discovery response from " MACSTR, MAC2STR(rx_param->sa));
        return ESP_OK;
    }

    wpa_printf(MSG_DEBUG, "DPP: Peer Discovery from " MACSTR, MAC2STR(rx_param->sa));

    trans_id = dpp_get_attr(&buf[2], len, DPP_ATTR_TRANSACTION_ID, &trans_id_len);
    if (!trans_id || trans_id_len != 1) {
        wpa_printf(MSG_ERROR, "DPP: Peer did not include Transaction ID");
        return ESP_ERR_DPP_INVALID_ATTR;
    }
    if (trans_id[0] != TRANSACTION_ID) {
        wpa_printf(MSG_DEBUG, "DPP: Ignore frame with unexpected Transaction ID %u", trans_id[0]);
        return ESP_OK;
    }

    const uint8_t *status_val;
    uint16_t status_len;
    const uint8_t *peer_connector;
    uint16_t peer_connector_len;
    struct dpp_introduction *intro = NULL;
    os_time_t expiry = 0;
    struct dpp_conf *conf = dc->conf;

    if (!conf) {
        wpa_printf(MSG_ERROR, "DPP: No active configuration to derive PMK");
        return ESP_ERR_DPP_FAILURE;
    }

    intro = os_zalloc(sizeof(*intro));
    if (!intro) {
        wpa_printf(MSG_ERROR, "DPP: Failed to allocate memory for intro");
        return ESP_ERR_NO_MEM;
    }

    esp_err_t ret = ESP_OK;

    status_val = dpp_get_attr(&buf[2], len, DPP_ATTR_STATUS, &status_len);
    if (!status_val || status_len != 1 || status_val[0] != DPP_STATUS_OK) {
        wpa_printf(MSG_ERROR, "DPP: Peer Discovery failed or bad status (status: %d)", status_val ? status_val[0] : -1);
        ret = ESP_ERR_DPP_FAILURE;
        goto out;
    }

    peer_connector = dpp_get_attr(&buf[2], len, DPP_ATTR_CONNECTOR, &peer_connector_len);
    if (!peer_connector) {
        wpa_printf(MSG_ERROR, "DPP: Peer did not include Connector");
        ret = ESP_ERR_DPP_INVALID_ATTR;
        goto out;
    }

    /* peer_disc_timeout handles timeout in Enrollee role */
    eloop_cancel_timeout(peer_disc_timeout, NULL, s_dpp_ctx.dpp_auth);

    if (!conf->connector || !conf->net_access_key || !conf->c_sign_key) {
        wpa_printf(MSG_ERROR, "DPP: Incomplete config for network introduction");
        ret = ESP_ERR_DPP_FAILURE;
        goto out;
    }

    if (dpp_peer_intro(intro, conf->connector,
                       wpabuf_head(conf->net_access_key), wpabuf_len(conf->net_access_key),
                       wpabuf_head(conf->c_sign_key), wpabuf_len(conf->c_sign_key),
                       peer_connector, peer_connector_len, &expiry) != DPP_STATUS_OK) {
        wpa_printf(MSG_ERROR, "DPP: Failed to derive PMK from Peer Discovery Response");
        ret = ESP_ERR_DPP_FAILURE;
        goto out;
    }

    struct rsn_pmksa_cache_entry *entry;
    struct os_reltime now_rel;
    os_time_t seconds;

    entry = os_zalloc(sizeof(*entry));
    if (!entry) {
        wpa_printf(MSG_ERROR, "DPP: Failed to allocate PMKSA entry");
        ret = ESP_ERR_NO_MEM;
        goto out;
    }

    os_memcpy(entry->aa, rx_param->sa, ETH_ALEN);
    os_memcpy(entry->pmkid, intro->pmkid, PMKID_LEN);
    os_memcpy(entry->pmk, intro->pmk, intro->pmk_len);
    entry->pmk_len = intro->pmk_len;
    entry->akmp = WPA_KEY_MGMT_DPP;
    entry->network_ctx = NULL;

    if (expiry > 0) {
        struct os_time now;
        os_get_time(&now);
        if (expiry > now.sec) {
            seconds = expiry - now.sec;
        } else {
            wpa_printf(MSG_WARNING, "DPP: Connector expired during processing");
            bin_clear_free(entry, sizeof(*entry));
            ret = ESP_ERR_DPP_FAILURE;
            goto out;
        }
    } else {
        seconds = ESP_DPP_PMK_CACHE_DEFAULT_TIMEOUT;
    }

    os_get_reltime(&now_rel);
    entry->expiration = now_rel.sec + seconds;
    entry->reauth_time = entry->expiration;

    pmksa_cache_add_entry(gWpaSm.pmksa, entry);

    if (dpp_connect(rx_param->sa, true) != ESP_OK) {
        wpa_printf(MSG_ERROR, "DPP: Failed to trigger connection after Peer Discovery");
        ret = ESP_ERR_DPP_FAILURE;
    }

out:
    bin_clear_free(intro, sizeof(*intro));
    return ret;
}

static esp_err_t esp_dpp_rx_frm(struct action_rx_param *rx_param)
{
    uint8_t crypto_suit, type;
    uint8_t *tmp;
    int ret = ESP_OK;

    tmp = rx_param->action_frm->u.public_action.v.pa_vendor_spec.vendor_data;
    if (rx_param->vendor_data_len < 2) { /* vendor_data too short for 2-octet DPP header before attr buffer */
        wpa_printf(MSG_DEBUG,
                   "DPP: vendor-specific data too short (len=%u), dropping",
                   (unsigned)rx_param->vendor_data_len);
        return ESP_ERR_DPP_INVALID_ATTR;
    }
    crypto_suit = tmp[0];
    type = tmp[1];

    if (crypto_suit != 1) {
        wpa_printf(MSG_ERROR, "DPP: Unsupported crypto suit");
        return ESP_ERR_DPP_INVALID_ATTR;
    }

    switch (type) {
    case DPP_PA_AUTHENTICATION_REQ:
        ret = esp_dpp_rx_auth_req(rx_param, &tmp[2]);
        break;
    case DPP_PA_AUTHENTICATION_CONF:
        ret = esp_dpp_rx_auth_conf(rx_param, &tmp[2]);
        break;
    case DPP_PA_PEER_DISCOVERY_RESP:
        ret = esp_dpp_rx_peer_disc_resp(rx_param);
        break;
    default:
        wpa_printf(MSG_DEBUG, "DPP: ignore unhandled DPP public action type %u", type);
        break;
    }

    return ret;
}

static void peer_disc_timeout(void *eloop_data, void *user_ctx)
{
    /* Note: user_ctx (auth) is guaranteed valid and not a use-after-free risk.
     * The eloop is single-threaded, and pending timeouts are synchronously cancelled
     * via dpp_cancel_auth_gas_eloop_timeouts() during teardown before auth is freed. */
    struct dpp_authentication *auth = user_ctx;

    /* Prerequisite confirmed via lifecycle (timers cancelled on deinit), lock acquisition will not fail. */
    dpp_api_lock();

    if (atomic_load(&dpp_shutting_down)) {
        dpp_api_unlock();
        return;
    }

    if (!s_dpp_ctx.dpp_auth || (s_dpp_ctx.dpp_auth != auth)) {
        wpa_printf(MSG_INFO, "DPP: Auth %p state not correct for peer discovery", auth);
        dpp_api_unlock();
        return;
    }

    wpa_printf(MSG_DEBUG, "DPP: No response received for Peer Discovery Request");
    dpp_abort_failure_locked(ESP_ERR_DPP_CONF_TIMEOUT);
}

static void gas_query_timeout(void *eloop_data, void *user_ctx)
{
    /* Note: user_ctx (auth) is guaranteed valid and not a use-after-free risk.
     * The eloop is single-threaded, and pending timeouts are synchronously cancelled
     * via dpp_cancel_auth_gas_eloop_timeouts() during teardown before auth is freed. */
    struct dpp_authentication *auth = user_ctx;

    /* Prerequisite confirmed via lifecycle (timers cancelled on deinit), lock acquisition will not fail. */
    dpp_api_lock();

    if (atomic_load(&dpp_shutting_down)) {
        dpp_api_unlock();
        return;
    }

    if (!s_dpp_ctx.dpp_auth || !s_dpp_ctx.dpp_auth->auth_success || (s_dpp_ctx.dpp_auth != auth)) {
        wpa_printf(MSG_INFO, "DPP-GAS: Auth %p state not correct", auth);
        dpp_api_unlock();
        return;
    }

    wpa_printf(MSG_DEBUG, "GAS: No response received for GAS query");
    dpp_abort_failure_locked(ESP_ERR_DPP_CONF_TIMEOUT);
}

static esp_err_t gas_query_req_tx(struct dpp_authentication *auth)
{
    struct wpabuf *buf;
    int supp_op_classes[] = {81, 0};
    esp_err_t ret;

    wpabuf_free(auth->conf_req);
    auth->conf_req = NULL;

    buf = dpp_build_conf_req_helper(auth, NULL, 0, NULL,
                                    supp_op_classes);
    if (!buf) {
        wpa_printf(MSG_ERROR, "DPP: No configuration request data available");
        return ESP_ERR_DPP_FAILURE;
    }

    wpa_printf(MSG_INFO, "DPP: GAS request to " MACSTR " (chan %u)",
               MAC2STR(auth->peer_mac_addr), auth->curr_chan);

    ret = esp_dpp_send_action_frame(auth->peer_mac_addr, wpabuf_head(buf), wpabuf_len(buf),
                                    auth->curr_chan, 1000 + OFFCHAN_TX_WAIT_TIME,
                                    DPP_TX_GAS_CONFIG_REQ);
    if (ret != ESP_OK) {
        wpabuf_free(buf);
        return ret;
    }

    auth->conf_req = buf;
    if (eloop_register_timeout(ESP_GAS_TIMEOUT_SECS, 0, gas_query_timeout, NULL, auth) < 0) {
        wpa_printf(MSG_ERROR, "DPP: Failed to register gas_query_timeout");
        wpabuf_free(auth->conf_req);
        auth->conf_req = NULL;
        return ESP_ERR_NO_MEM;
    }

    return ret;
}

static void esp_dpp_process_config_obj(esp_dpp_config_data_t *config_data, struct dpp_config_obj *conf)
{
    if (conf->ssid_len) {
        size_t n = conf->ssid_len;

        if (n > MAX_SSID_LEN) {
            n = MAX_SSID_LEN;
            wpa_printf(MSG_WARNING, "DPP: SSID truncated to %u octets", MAX_SSID_LEN);
        }
        config_data->ssid_len = (uint8_t)n;
        os_memcpy(config_data->ssid, conf->ssid, n);
    }

    if (dpp_akm_legacy(conf->akm) || dpp_akm_ver2(conf->akm)) {
        if (conf->passphrase[0]) {
            size_t pass_len = os_strnlen(conf->passphrase, sizeof(conf->passphrase));

            if (pass_len > MAX_PASSPHRASE_LEN) {
                wpa_printf(MSG_WARNING,
                           "DPP: passphrase length %zu exceeds MAX_PASSPHRASE_LEN, truncating",
                           pass_len);
                pass_len = MAX_PASSPHRASE_LEN;
            }
            os_memcpy(config_data->password, conf->passphrase, pass_len);
            config_data->password_len = (uint8_t) pass_len;
        } else if (conf->psk_set) {
            char hex_tmp[MAX_PASSPHRASE_LEN + 1];
            int hex_chars;

            hex_chars = wpa_snprintf_hex(hex_tmp, sizeof(hex_tmp), conf->psk, PMK_LEN);
            if (hex_chars != (int)(PMK_LEN * 2)) {
                wpa_hexdump_key(MSG_ERROR,
                                "DPP: PSK hex conversion failed (unexpected length)",
                                conf->psk, PMK_LEN);
                forced_memzero(hex_tmp, sizeof(hex_tmp));
            } else {
                os_memcpy(config_data->password, hex_tmp, PMK_LEN * 2);
                config_data->password_len = PMK_LEN * 2;
                forced_memzero(hex_tmp, sizeof(hex_tmp));
            }
        }
    }
}

static int esp_dpp_handle_config_obj(struct dpp_authentication *auth,
                                     struct dpp_config_obj *conf, esp_dpp_config_data_t *config_data)
{
    forced_memzero(config_data, sizeof(*config_data));

    if (conf->connector) {
        size_t n;

        wpa_printf(MSG_INFO, DPP_EVENT_CONNECTOR "%s",
                   conf->connector);
        n = os_strlcpy(config_data->connector, conf->connector,
                       sizeof(config_data->connector));
        if (n >= sizeof(config_data->connector)) {
            wpa_printf(MSG_WARNING,
                       "DPP: Connector length %zu exceeds storage (%zu); "
                       "truncated JWS cannot be used, rejecting configuration object",
                       n, sizeof(config_data->connector));
            forced_memzero(config_data, sizeof(*config_data));
            return -1;
        }
        config_data->connector_len = (uint16_t)n;
    } else {
        config_data->connector_len = 0;
    }

    if (auth->net_access_key && dpp_akm_dpp(conf->akm)) {
        size_t key_len = wpabuf_len(auth->net_access_key);
        if (key_len > ESP_DPP_MAX_KEY_LEN) {
            key_len = ESP_DPP_MAX_KEY_LEN;
        }
        os_memcpy(config_data->net_access_key, wpabuf_head(auth->net_access_key), key_len);
        config_data->net_access_key_len = (uint16_t)key_len;
        config_data->net_access_key_expiry = auth->net_access_key_expiry;
    }

    if (conf->c_sign_key) {
        size_t key_len = wpabuf_len(conf->c_sign_key);
        if (key_len > ESP_DPP_MAX_KEY_LEN) {
            key_len = ESP_DPP_MAX_KEY_LEN;
        }
        os_memcpy(config_data->c_sign_key, wpabuf_head(conf->c_sign_key), key_len);
        config_data->c_sign_key_len = (uint16_t)key_len;
    }

    config_data->curr_chan = (uint8_t)auth->curr_chan;
    config_data->akm = (uint8_t)conf->akm;

    esp_dpp_process_config_obj(config_data, conf);

    return 0;
}

static void esp_dpp_fill_wifi_cfg_from_config(const esp_dpp_config_data_t *dpp, wifi_config_t *wifi_cfg)
{
    size_t ssid_len;
    size_t pass_len;

    forced_memzero(wifi_cfg, sizeof(*wifi_cfg));
    if (!dpp) {
        return;
    }

    if (dpp->ssid_len) {
        ssid_len = dpp->ssid_len;
        if (ssid_len > sizeof(wifi_cfg->sta.ssid)) {
            ssid_len = sizeof(wifi_cfg->sta.ssid);
        }
        os_memcpy(wifi_cfg->sta.ssid, dpp->ssid, ssid_len);
    } else if (dpp->ssid[0]) {
        /* Fallback for legacy configs that don't set ssid_len */
        ssid_len = os_strnlen((const char *) dpp->ssid, sizeof(dpp->ssid));
        os_memcpy(wifi_cfg->sta.ssid, dpp->ssid, ssid_len);
    }

    if (dpp_akm_legacy((enum dpp_akm) dpp->akm) || dpp_akm_ver2((enum dpp_akm) dpp->akm)) {
        pass_len = dpp->password_len;
        if (pass_len == 0 && dpp->password[0]) {
            /* Fallback for legacy configs */
            pass_len = os_strnlen((const char *) dpp->password, sizeof(dpp->password));
        }
        if (pass_len > MAX_PASSPHRASE_LEN) {
            wpa_printf(MSG_WARNING,
                       "DPP: password_len %zu exceeds MAX_PASSPHRASE_LEN, clamping",
                       pass_len);
            pass_len = MAX_PASSPHRASE_LEN;
        }
        if (pass_len > sizeof(wifi_cfg->sta.password)) {
            pass_len = sizeof(wifi_cfg->sta.password);
        }
        if (pass_len) {
            os_memcpy(wifi_cfg->sta.password, dpp->password, pass_len);
        }
    }

    if (dpp_akm_sae((enum dpp_akm) dpp->akm)) {
        wifi_cfg->sta.pmf_cfg.capable = true;
        wifi_cfg->sta.pmf_cfg.required = true;
    }

    if (dpp->curr_chan) {
        wifi_cfg->sta.channel = dpp->curr_chan;
    }
}

static esp_err_t gas_process_complete_resp(struct dpp_authentication *auth,
                                           const uint8_t *resp, size_t dpp_data_len)
{
    int i, res;
    uint8_t conf_capacity = 0;
    size_t recv_size = 0;
    size_t post_size = 0;
    struct dpp_conf *new_conf_pending = NULL;
    wifi_event_dpp_config_received_t *recv = NULL;
    esp_err_t ret = ESP_ERR_DPP_FAILURE;

    if (dpp_conf_resp_rx(auth, resp, dpp_data_len) < 0) {
        wpa_printf(MSG_INFO, "DPP: Configuration attempt failed");
        return ESP_ERR_DPP_FAILURE;
    }

    /* Configuration parsed successfully — cancel the GAS response timeout.
     * Without this, gas_query_timeout would fire a few seconds later and
     * post a spurious WIFI_EVENT_DPP_FAILED after the app already received
     * WIFI_EVENT_DPP_CFG_RECVD. */
    eloop_cancel_timeout(gas_query_timeout, NULL, auth);

    if (auth->num_conf_obj <= 0) {
        conf_capacity = 0;
    } else if (auth->num_conf_obj > ESP_DPP_MAX_CONFIG_COUNT) {
        conf_capacity = ESP_DPP_MAX_CONFIG_COUNT;
    } else {
        conf_capacity = (uint8_t)auth->num_conf_obj;
    }
    recv_size = sizeof(*recv) + conf_capacity * sizeof(recv->configs[0]);
    recv = os_zalloc(recv_size);
    if (!recv) {
        wpa_printf(MSG_ERROR, "DPP: Failed to allocate memory for DPP event");
        return ESP_ERR_NO_MEM;
    }

    /*
     * Runs on eloop only.
     * A candidate configuration is built into new_conf_pending and committed to dc->conf only
     * after esp_event_post succeeds (commit-at-end). We track old_conf across the unlock
     * window to prevent overwriting a configuration set concurrently by the app.
     */
    if (auth->num_conf_obj > ESP_DPP_MAX_CONFIG_COUNT) {
        wpa_printf(MSG_WARNING,
                   "DPP: %d config objects in response, forwarding first %d and dropping %d",
                   auth->num_conf_obj, ESP_DPP_MAX_CONFIG_COUNT,
                   auth->num_conf_obj - ESP_DPP_MAX_CONFIG_COUNT);
    }

    for (i = 0; i < auth->num_conf_obj && recv->total_conf < conf_capacity; i++) {
        uint8_t idx = recv->total_conf;

        res = esp_dpp_handle_config_obj(auth, &auth->conf_obj[i], &recv->configs[idx]);
        if (res < 0) {
            wpa_printf(MSG_WARNING, "DPP: Configuration storage failed for object %d; not counting in total_conf", i);
            continue;
        }

        recv->total_conf++;
    }

    if (recv->total_conf > 0) {
        esp_dpp_config_data_t *first_conf = &recv->configs[0];

        esp_dpp_fill_wifi_cfg_from_config(first_conf, &recv->wifi_cfg);

        if (dpp_akm_dpp((enum dpp_akm) first_conf->akm) &&
                (first_conf->connector_len > 0)) {
            if (s_dpp_ctx.dpp_config_store) {
                struct dpp_config_store *dc = s_dpp_ctx.dpp_config_store;

                if (esp_dpp_stored_conf_matches_row(dc, first_conf)) {
                    /* Same as installed config; nothing new to store. */
                } else {
                    esp_err_t autostore = esp_dpp_conf_alloc_from_config_data(first_conf,
                                                                              &new_conf_pending);

                    if (autostore != ESP_OK) {
                        wpa_printf(MSG_WARNING,
                                   "DPP: auto-load of first configuration object for supplicant failed: %d",
                                   (int) autostore);
                        goto out;
                    }
                }
            }
        } else if (!dpp_akm_dpp((enum dpp_akm) first_conf->akm) &&
                   s_dpp_ctx.dpp_config_store && s_dpp_ctx.dpp_config_store->conf) {
            /* Replaced by a non-DPP AKM (legacy PSK/SAE): drop stored DPP
             * config so it is not reused. A malformed DPP-AKM config with
             * no Connector is rejected without touching the stored entry. */
            dpp_clear_confs(s_dpp_ctx.dpp_config_store->conf);
            s_dpp_ctx.dpp_config_store->conf = NULL;
        }
    }

    if (recv->total_conf == 0) {
        wpa_printf(MSG_ERROR, "DPP: No valid configuration object was processed");
        goto out;
    }

    post_size = sizeof(*recv) + recv->total_conf * sizeof(recv->configs[0]);
    struct dpp_conf *old_conf = s_dpp_ctx.dpp_config_store ? s_dpp_ctx.dpp_config_store->conf : NULL;
    dpp_api_unlock();

    if (esp_event_post(WIFI_EVENT, WIFI_EVENT_DPP_CFG_RECVD, recv, post_size, os_task_ms_to_tick(200)) != ESP_OK) {
        wpa_printf(MSG_ERROR, "DPP: Failed to post WIFI_EVENT_DPP_CFG_RECVD");
        /* Prerequisite confirmed via atomic check at entry, lock re-acquisition will not fail. */
        dpp_api_lock();
        goto out;
    }

    /* Prerequisite confirmed via atomic check at entry, lock re-acquisition will not fail. */
    dpp_api_lock();

    /* Re-verify state: another task (e.g. deinit) could have run while the lock
     * was released. If DPP was shut down, skip the config store commit. */
    if (atomic_load(&dpp_shutting_down) || !atomic_load(&s_dpp_init_done)) {
        wpa_printf(MSG_WARNING, "DPP: State changed during event post, skipping config store commit");
        ret = ESP_ERR_INVALID_STATE;
        goto out;
    }

    if (new_conf_pending) {
        if (s_dpp_ctx.dpp_config_store) {
            struct dpp_config_store *dc = s_dpp_ctx.dpp_config_store;

            if (dc->conf != old_conf) {
                wpa_printf(MSG_INFO, "DPP: Configuration updated by another task during event post, dropping new config");
                dpp_clear_confs(new_conf_pending);
            } else {
                dpp_clear_confs(dc->conf);
                dc->conf = new_conf_pending;
            }
        } else {
            dpp_clear_confs(new_conf_pending);
        }
        new_conf_pending = NULL;
    }

    ret = ESP_OK;

out:
    bin_clear_free(recv, recv_size);
    dpp_clear_confs(new_conf_pending);
    return ret;
}

static esp_err_t gas_comeback_req_tx(struct dpp_authentication *auth)
{
    struct wpabuf *buf;
    esp_err_t ret;

    buf = gas_build_comeback_req(s_dpp_ctx.gas_dialog_token);
    if (!buf) {
        wpa_printf(MSG_ERROR, "DPP: Failed to build GAS Comeback Request");
        return ESP_ERR_DPP_FAILURE;
    }

    wpa_printf(MSG_INFO, "DPP: GAS Comeback Request to " MACSTR " (chan %u)",
               MAC2STR(auth->peer_mac_addr), auth->curr_chan);

    ret = esp_dpp_send_action_frame(auth->peer_mac_addr,
                                    wpabuf_head(buf), wpabuf_len(buf),
                                    auth->curr_chan,
                                    1000 + OFFCHAN_TX_WAIT_TIME,
                                    DPP_TX_GAS_COMEBACK);
    wpabuf_free(buf);

    if (ret == ESP_OK) {
        eloop_cancel_timeout(gas_query_timeout, NULL, auth);
        if (eloop_register_timeout(ESP_GAS_TIMEOUT_SECS, 0,
                                   gas_query_timeout, NULL, auth) != 0) {
            return ESP_ERR_DPP_FAILURE;
        }
    }

    return ret;
}

static void gas_comeback_delay_timeout(void *eloop_data, void *user_ctx)
{
    /* Note: user_ctx (auth) is guaranteed valid and not a use-after-free risk.
     * The eloop is single-threaded, and pending timeouts are synchronously cancelled
     * via dpp_cancel_auth_gas_eloop_timeouts() during teardown before auth is freed. */
    struct dpp_authentication *auth = user_ctx;

    if (dpp_api_lock() != ESP_OK) {
        return;
    }

    if (atomic_load(&dpp_shutting_down)) {
        dpp_api_unlock();
        return;
    }

    if (!s_dpp_ctx.dpp_auth || s_dpp_ctx.dpp_auth != auth ||
            !s_dpp_ctx.dpp_auth->auth_success) {
        dpp_api_unlock();
        return;
    }
    if (gas_comeback_req_tx(auth) != ESP_OK) {
        dpp_abort_failure_locked(ESP_ERR_DPP_TX_FAILURE);
        return;
    }
    dpp_api_unlock();
}

static esp_err_t gas_query_resp_rx(struct action_rx_param *rx_param)
{
    struct dpp_authentication *auth = s_dpp_ctx.dpp_auth;
    u16 comeback_delay;
    u16 status_code;
    uint8_t *pos;
    size_t vendor_len = rx_param->vendor_data_len;

    if (!auth) {
        return ESP_OK;
    }

    /* Verify source MAC: Management frames like GAS can be easily spoofed. */
    if (os_memcmp(rx_param->sa, auth->peer_mac_addr, ETH_ALEN) != 0) {
        wpa_printf(MSG_DEBUG, "DPP: MAC mismatch - ignore potentially spoofed frame");
        return ESP_OK;
    }

    dpp_gas_cleanup_locked();

    if (!auth->auth_success) {
        wpa_printf(MSG_DEBUG, "DPP: GAS Initial Response while DPP auth not complete; ignore");
        return ESP_OK;
    }

    if (auth->num_conf_obj > 0) {
        wpa_printf(MSG_DEBUG, "DPP: Already received configuration, ignoring retransmitted GAS Initial Response");
        return ESP_OK;
    }

    pos = rx_param->action_frm->u.public_action.v.pa_gas_resp.data;
    status_code = WPA_GET_LE16((const u8 *)&rx_param->action_frm->u.public_action.v.pa_gas_resp.status_code);
    comeback_delay = WPA_GET_LE16((const u8 *)&rx_param->action_frm->u.public_action.v.pa_gas_resp.comeback_delay);

    if (status_code != 0) {
        wpa_printf(MSG_ERROR, "DPP: GAS Initial Response failed status=%u", status_code);
        eloop_cancel_timeout(gas_query_timeout, NULL, auth);
        return ESP_ERR_DPP_FAILURE;
    }

    if (comeback_delay) {
        if (comeback_delay > DPP_MAX_COMEBACK_DELAY_TU) {
            wpa_printf(MSG_WARNING, "DPP: GAS comeback delay %u TU too large, capping to %u",
                       comeback_delay, DPP_MAX_COMEBACK_DELAY_TU);
            comeback_delay = DPP_MAX_COMEBACK_DELAY_TU;
        }
        unsigned int secs = (comeback_delay * 1024) / 1000000;
        unsigned int usecs = comeback_delay * 1024 - secs * 1000000;

        wpa_printf(MSG_DEBUG, "DPP: GAS comeback delay %u TUs (%u.%06u s)",
                   comeback_delay, secs, usecs);

        s_dpp_ctx.gas_wait_comeback = true;
        s_dpp_ctx.gas_frag_id = 0;
        s_dpp_ctx.gas_dialog_token =
            rx_param->action_frm->u.public_action.v.pa_gas_resp.diag_token;

        eloop_cancel_timeout(gas_query_timeout, NULL, auth);

        if (eloop_register_timeout(secs, usecs, gas_comeback_delay_timeout, NULL, auth) != 0) {
            return ESP_ERR_DPP_FAILURE;
        }
        return ESP_OK;
    }

    if (vendor_len < 8) {
        wpa_printf(MSG_INFO, "DPP: Too short GAS response data (adv protocol)");
        eloop_cancel_timeout(gas_query_timeout, NULL, auth);
        return ESP_ERR_DPP_FAILURE;
    }

    if (!(pos[1] == WLAN_EID_VENDOR_SPECIFIC && pos[2] == 5 &&
            WPA_GET_BE24(&pos[3]) == OUI_WFA && pos[6] == DPP_OUI_TYPE && pos[7] == 1)) {
        wpa_hexdump(MSG_INFO, "DPP: Failed, Configuration Response adv_proto", pos, 8);
        eloop_cancel_timeout(gas_query_timeout, NULL, auth);
        return ESP_ERR_DPP_FAILURE;
    }

    if (vendor_len < 10) {
        wpa_printf(MSG_INFO, "DPP: Too short GAS response data");
        eloop_cancel_timeout(gas_query_timeout, NULL, auth);
        return ESP_ERR_DPP_FAILURE;
    }

    size_t dpp_data_len = vendor_len - 10;
    uint8_t *resp = &pos[10];

    return gas_process_complete_resp(auth, resp, dpp_data_len);
}

static esp_err_t gas_comeback_resp_rx(struct action_rx_param *rx_param)
{
    struct dpp_authentication *auth = s_dpp_ctx.dpp_auth;
    uint8_t *frame_start;
    size_t frame_len;
    uint8_t *pos;
    u16 status_code, comeback_delay, resp_len;
    u8 frag_id, more_frags;
    unsigned int left;
    esp_err_t ret = ESP_ERR_DPP_FAILURE;

    if (!auth || !auth->auth_success) {
        return ESP_OK;
    }

    if (auth->num_conf_obj > 0) {
        wpa_printf(MSG_DEBUG, "DPP: Already received configuration, ignoring retransmitted GAS Comeback Response");
        return ESP_OK;
    }

    /* Verify source MAC: Management frames like GAS can be easily spoofed. */
    if (os_memcmp(rx_param->sa, auth->peer_mac_addr, ETH_ALEN) != 0) {
        wpa_printf(MSG_DEBUG, "DPP: MAC mismatch on GAS Comeback Response - ignore potentially spoofed frame");
        return ESP_OK;
    }

    if (!s_dpp_ctx.gas_wait_comeback) {
        wpa_printf(MSG_DEBUG, "DPP: Unexpected GAS Comeback Response - ignore");
        return ESP_OK;
    }

    frame_start = (uint8_t *)&rx_param->action_frm->u.public_action;
    frame_len = rx_param->frm_len -
                (size_t)(frame_start - (uint8_t *)rx_param->action_frm);
    pos = frame_start;

    if (frame_len < 8) { /* need at least 7 for fixed fields plus 1 for adv proto element start */
        wpa_printf(MSG_DEBUG, "DPP: Too short GAS Comeback Response");
        goto gas_fail;
    }

    pos++; /* action */

    if (*pos != s_dpp_ctx.gas_dialog_token) {
        wpa_printf(MSG_DEBUG, "DPP: GAS dialog token mismatch (expected %u, got %u)",
                   s_dpp_ctx.gas_dialog_token, *pos);
        return ESP_OK;
    }
    pos++; /* dialog token */

    status_code = WPA_GET_LE16(pos);
    pos += 2;

    frag_id = *pos & 0x7f;
    more_frags = (*pos & 0x80) >> 7;
    pos++; /* fragment id; bit7 = more fragments */

    comeback_delay = WPA_GET_LE16(pos);
    pos += 2;

    wpa_printf(MSG_DEBUG, "DPP: GAS Comeback Response: status=%u frag_id=%u "
               "more_frags=%u comeback_delay=%u",
               status_code, frag_id, more_frags, comeback_delay);

    if (status_code != 0 && status_code != WLAN_STATUS_QUERY_RESP_OUTSTANDING) {
        wpa_printf(MSG_ERROR, "DPP: GAS Comeback Response failed status=%u", status_code);
        goto gas_fail;
    }

    if (comeback_delay) {
        if (frag_id) {
            wpa_printf(MSG_ERROR, "DPP: Invalid comeback response with "
                       "non-zero frag_id and comeback_delay");
            goto gas_fail;
        }
        if (comeback_delay > DPP_MAX_COMEBACK_DELAY_TU) {
            wpa_printf(MSG_WARNING, "DPP: GAS comeback delay %u TU too large, capping to %u",
                       comeback_delay, DPP_MAX_COMEBACK_DELAY_TU);
            comeback_delay = DPP_MAX_COMEBACK_DELAY_TU;
        }
        unsigned int secs = (comeback_delay * 1024) / 1000000;
        unsigned int usecs = comeback_delay * 1024 - secs * 1000000;
        eloop_cancel_timeout(gas_query_timeout, NULL, auth);
        if (eloop_register_timeout(secs, usecs, gas_comeback_delay_timeout, NULL, auth) != 0) {
            goto gas_fail;
        }
        return ESP_OK;
    }

    {
        u8 expected_frag = s_dpp_ctx.gas_frag_id & 0x7f;
        u8 previous_frag = (expected_frag - 1) & 0x7f;

        if (frag_id != expected_frag) {
            if (frag_id == previous_frag) {
                wpa_printf(MSG_DEBUG, "DPP: Drop frame as possible "
                           "retry of previous fragment");
                return ESP_OK;
            }
            wpa_printf(MSG_ERROR, "DPP: Unexpected frag_id %u (expected %u)",
                       frag_id, expected_frag);
            goto gas_fail;
        }
    }
    s_dpp_ctx.gas_frag_id = (s_dpp_ctx.gas_frag_id + 1) & 0x7f;

    left = frame_len - (size_t)(pos - frame_start);
    if (left < 2 || pos[0] != WLAN_EID_ADV_PROTO || left < (size_t)(2 + pos[1])) {
        wpa_printf(MSG_DEBUG, "DPP: No room for Advertisement Protocol element");
        goto gas_fail;
    }
    pos += 2 + pos[1];

    left = frame_len - (size_t)(pos - frame_start);
    if (left < 2) {
        wpa_printf(MSG_DEBUG, "DPP: No room for GAS Response Length");
        goto gas_fail;
    }
    resp_len = WPA_GET_LE16(pos);
    pos += 2;

    left = frame_len - (size_t)(pos - frame_start);
    if (resp_len > left) {
        wpa_printf(MSG_DEBUG, "DPP: Truncated GAS Comeback Response");
        goto gas_fail;
    }

    if (s_dpp_ctx.gas_resp_buf &&
            wpabuf_len(s_dpp_ctx.gas_resp_buf) + resp_len > ESP_DPP_GAS_REASSEMBLY_MAX_LEN) {
        wpa_printf(MSG_ERROR, "DPP: GAS response too long, rejecting fragments");
        ret = ESP_ERR_NO_MEM;
        goto gas_fail;
    }

    if (!s_dpp_ctx.gas_resp_buf) {
        s_dpp_ctx.gas_resp_buf = wpabuf_alloc(resp_len + 256);
        if (!s_dpp_ctx.gas_resp_buf) {
            ret = ESP_ERR_NO_MEM;
            goto gas_fail;
        }
    }
    if (wpabuf_resize(&s_dpp_ctx.gas_resp_buf, resp_len) < 0) {
        wpa_printf(MSG_ERROR, "DPP: No memory to store GAS fragment");
        ret = ESP_ERR_NO_MEM;
        goto gas_fail;
    }
    wpabuf_put_data(s_dpp_ctx.gas_resp_buf, pos, resp_len);

    if (more_frags) {
        if (gas_comeback_req_tx(auth) != ESP_OK) {
            goto gas_fail;
        }
        return ESP_OK;
    }

    ret = gas_process_complete_resp(auth,
                                    wpabuf_head(s_dpp_ctx.gas_resp_buf),
                                    wpabuf_len(s_dpp_ctx.gas_resp_buf));
    if (ret != ESP_OK) {
        goto gas_fail;
    }
    dpp_gas_cleanup_locked();
    return ret;

gas_fail:
    dpp_gas_cleanup_locked();
    eloop_cancel_timeout(gas_query_timeout, ELOOP_ALL_CTX, ELOOP_ALL_CTX);
    eloop_cancel_timeout(gas_comeback_delay_timeout, ELOOP_ALL_CTX, ELOOP_ALL_CTX);
    return ret;
}

static void esp_dpp_rx_action(void *data, void *user_ctx)
{
    struct action_rx_param *rx_param = data;
    struct ieee80211_public_action *public_action;
    int ret = ESP_OK;

    if (!rx_param) {
        return;
    }

    if (!atomic_load(&s_dpp_init_done)) {
        os_free(rx_param);
        return;
    }

    /* Prerequisite confirmed via atomic check, lock acquisition will not fail. */
    dpp_api_lock();

    if (atomic_load(&dpp_shutting_down)) {
        goto fail_unlock;
    }

    /* we don't cater other action frames except public here */
    if (rx_param->action_frm->category != WLAN_ACTION_PUBLIC) {
        goto fail_unlock;
    }

    public_action = &rx_param->action_frm->u.public_action;

    wpa_printf(MSG_DEBUG, "DPP: Rx Public Action frame: action - %d",
               public_action->action);

    if (public_action->action == WLAN_PA_VENDOR_SPECIFIC) {
        if (rx_param->frm_len < 6) {
            wpa_printf(MSG_DEBUG, "DPP: vendor-specific public action too short (%u), dropping",
                       (unsigned)rx_param->frm_len);
            goto fail_unlock;
        }
        if (WPA_GET_BE24(public_action->v.pa_vendor_spec.oui) == OUI_WFA &&
                public_action->v.pa_vendor_spec.wfa_stype == DPP_OUI_TYPE) {

            rx_param->vendor_data_len = rx_param->frm_len -
                                        (size_t)(public_action->v.pa_vendor_spec.vendor_data -
                                                 (u8 *)rx_param->action_frm);

            if (atomic_load(&roc_in_progress)) {
                listen_stop_handler(NULL, NULL);
            }

            ret = esp_dpp_rx_frm(rx_param);
        }
    } else if (public_action->action == WLAN_PA_GAS_INITIAL_RESP && rx_param->frm_len >= 7) {
        if (!s_dpp_ctx.dpp_auth ||
                s_dpp_ctx.dpp_auth->gas_dialog_token < 0 ||
                public_action->v.pa_gas_resp.diag_token !=
                s_dpp_ctx.dpp_auth->gas_dialog_token) {
            wpa_printf(MSG_DEBUG,
                       "DPP: GAS dialog token mismatch (rx=%u exp=%d) - drop",
                       public_action->v.pa_gas_resp.diag_token,
                       s_dpp_ctx.dpp_auth ?
                       s_dpp_ctx.dpp_auth->gas_dialog_token : -1);
            goto fail_unlock;
        }

        uint16_t status_code = WPA_GET_LE16((u8 *)&public_action->v.pa_gas_resp.status_code);

        if (status_code != 0) {
            /* Non-zero status: error frame (typically 7 bytes, no Adv Proto element).
             * gas_query_resp_rx handles status_code != 0 → ESP_ERR_DPP_FAILURE,
             * enabling fast failure instead of waiting for a timeout. */
            if (atomic_load(&roc_in_progress)) {
                listen_stop_handler(NULL, NULL);
            }
            wpa_printf(MSG_DEBUG, "DPP: GAS Initial Response with error status=%u", status_code);
            ret = gas_query_resp_rx(rx_param);
        } else if (rx_param->frm_len >= 9 &&
                   public_action->v.pa_gas_resp.type == WLAN_EID_ADV_PROTO &&
                   public_action->v.pa_gas_resp.length == 8) {

            size_t offset = (size_t)(public_action->v.pa_gas_resp.data -
                                     (u8 *)rx_param->action_frm);

            if (rx_param->frm_len < offset) {
                wpa_printf(MSG_DEBUG,
                           "DPP: GAS Initial Resp truncated (len=%u < adv_proto data offset %zu), dropping",
                           (unsigned)rx_param->frm_len, offset);
                goto fail_unlock;
            }

            if (atomic_load(&roc_in_progress)) {
                listen_stop_handler(NULL, NULL);
            }

            rx_param->vendor_data_len = rx_param->frm_len - offset;
            wpa_printf(MSG_DEBUG, "DPP: Gas response received");
            ret = gas_query_resp_rx(rx_param);
        }
    } else if (public_action->action == WLAN_PA_GAS_COMEBACK_RESP &&
               rx_param->frm_len >= 9) {
        ret = gas_comeback_resp_rx(rx_param);
    }

fail_unlock:
    os_free(rx_param);

    if (ret != ESP_OK) {
        if (ret != ESP_ERR_INVALID_STATE) {
            dpp_abort_failure_locked(ret);
        } else {
            dpp_api_unlock();
        }
        return;
    }

    /* On successful config receipt, clean up based on AKM type:
     * - Legacy AKM (PSK/SAE): DPP's job is done. The app has the SSID+password
     *   and will call esp_wifi_connect() directly. Stop DPP to free auth state
     *   and prevent stale timers from firing.
     * - DPP AKM (Connector): The app still needs to call
     *   esp_dpp_start_net_intro_protocol(), which requires auth->conf_obj and
     *   auth->net_access_key. Keep auth alive but cancel GAS timers. */
    if (s_dpp_ctx.dpp_auth && s_dpp_ctx.dpp_auth->num_conf_obj > 0) {
        bool has_dpp_akm = false;
        for (size_t i = 0; i < s_dpp_ctx.dpp_auth->num_conf_obj; i++) {
            if (dpp_akm_dpp(s_dpp_ctx.dpp_auth->conf_obj[i].akm)) {
                has_dpp_akm = true;
                break;
            }
        }

        if (!has_dpp_akm) {
            dpp_stop_internal();
        } else {
            /* Cancel stale GAS timers; auth is still needed for net intro */
            dpp_cancel_auth_gas_eloop_timeouts();
        }
    }
    dpp_api_unlock();
}

static void dpp_listen_next_channel(void *data, void *user_ctx)
{
    struct dpp_bootstrap_params_t *p = &s_dpp_ctx.bootstrap_params;
    int channel;
    esp_err_t ret = 0;
    wifi_roc_req_t req = {0};
    unsigned int nc;

    /* Prerequisite confirmed via lifecycle (timers cancelled on deinit), lock acquisition will not fail. */
    dpp_api_lock();

    if (atomic_load(&dpp_shutting_down) || !s_dpp_ctx.dpp_listen_ongoing) {
        dpp_api_unlock();
        return;
    }
    if (p->num_chan <= 0) {
        wpa_printf(MSG_ERROR, "Listen channel not set");
        dpp_api_unlock();
        return;
    }

    nc = (unsigned int)p->num_chan;
    channel = p->chan_list[s_dpp_ctx.listen_chan_idx % nc];
    s_dpp_ctx.listen_chan_idx = (s_dpp_ctx.listen_chan_idx + 1U) % nc;

    wpa_printf(MSG_DEBUG, "DPP: Starting ROC on channel %d", channel);

    req.ifx = WIFI_IF_STA;
    req.type = WIFI_ROC_REQ;
    req.channel = channel;
    req.wait_time_ms = BOOTSTRAP_ROC_WAIT_TIME;
    req.rx_cb = s_action_rx_cb;
    ret = esp_wifi_remain_on_channel(&req);
    if (ret != ESP_OK) {
        wpa_printf(MSG_ERROR, "Failed ROC. error : 0x%x", ret);
        dpp_abort_failure_locked(ESP_ERR_DPP_FAILURE);
        return;
    }
    if (s_dpp_event_group) {
        os_event_group_clear_bits(s_dpp_event_group, DPP_ROC_EVENT_HANDLED);
    }
    atomic_store(&roc_in_progress, true);
    dpp_api_unlock();
}

static void esp_dpp_bootstrap_gen(void *data, void *user_ctx)
{
    char *command = data;
    const char *uri;
    uint32_t len;
    bool success = false;
    wifi_event_dpp_uri_ready_t *event = NULL;
    int id;

    if (!atomic_load(&s_dpp_init_done)) {
        goto out;
    }

    /* Prerequisite confirmed via atomic check, lock acquisition will not fail. */
    dpp_api_lock();

    s_dpp_ctx.bootstrap_done = false;

    if (atomic_load(&dpp_shutting_down) || !command || !s_dpp_ctx.dpp_global) {
        dpp_api_unlock();
        goto out;
    }

    if (s_dpp_ctx.id >= 0 && s_dpp_ctx.dpp_global) {
        char id_str[16];
        os_snprintf(id_str, sizeof(id_str), "%u", (unsigned int)s_dpp_ctx.id);
        dpp_bootstrap_remove(s_dpp_ctx.dpp_global, id_str);
        s_dpp_ctx.id = -1;
    }

    s_dpp_ctx.id = dpp_bootstrap_gen(s_dpp_ctx.dpp_global, command);
    id = s_dpp_ctx.id;

    if (id < 0) {
        dpp_api_unlock();
        goto fail;
    }

    uri = dpp_bootstrap_get_uri(s_dpp_ctx.dpp_global, id);
    if (!uri) {
        dpp_api_unlock();
        goto fail;
    }

    len = sizeof(*event) + os_strlen(uri) + 1;
    event = os_malloc(len);
    if (!event) {
        dpp_api_unlock();
        goto fail;
    }
    event->uri_data_len = os_strlen(uri);
    os_memcpy(event->uri, uri, event->uri_data_len);
    event->uri[event->uri_data_len++] = '\0';

    dpp_api_unlock();

    esp_err_t ret = esp_event_post(WIFI_EVENT, WIFI_EVENT_DPP_URI_READY, event, len, os_task_ms_to_tick(200));
    if (ret != ESP_OK) {
        wpa_printf(MSG_ERROR, "DPP: Failed to post DPP_URI_READY event, error 0x%x", ret);
        success = false;
        goto fail;
    } else {
        dpp_api_lock();
        s_dpp_ctx.bootstrap_done = true;
        dpp_api_unlock();
        success = true;
    }

    goto out;

fail:
    if (!success && !atomic_load(&dpp_shutting_down)) {
        /* Prerequisite confirmed at function entry, lock re-acquisition will not fail. */
        dpp_api_lock();

        if (s_dpp_ctx.id >= 0 && s_dpp_ctx.dpp_global) {
            char id_str[16];
            os_snprintf(id_str, sizeof(id_str), "%u", (unsigned int)s_dpp_ctx.id);
            dpp_bootstrap_remove(s_dpp_ctx.dpp_global, id_str);
        }
        s_dpp_ctx.id = -1;
        s_dpp_ctx.bootstrap_done = false;
        dpp_stop_internal();
        dpp_api_unlock();

        dpp_post_dpp_failed_event(ESP_ERR_DPP_FAILURE);
    }

out:
    os_free(event);
    os_free(command);
}

static int esp_dpp_deinit(void *data, void *user_ctx)
{
    esp_err_t ret;

    ret = dpp_api_lock();
    if (ret != ESP_OK) {
        return ret;
    }

    dpp_stop_internal();

    atomic_store(&s_dpp_init_done, false);
    wpa_printf(MSG_DEBUG, "DPP: Deinitializing DPP");
    atomic_store(&dpp_shutting_down, true);
    esp_dpp_cancel_timeouts();

    /* No lock should be held while unregistering events,
     * it might cause deadlock with wifi task */
    dpp_api_unlock();
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_ACTION_TX_STATUS,
                                 &tx_status_handler);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_ROC_DONE,
                                 &roc_status_handler);

    /* Prerequisite confirmed at function entry, lock re-acquisition will not fail. */
    dpp_api_lock();

    if (s_dpp_ctx.dpp_global) {
        dpp_global_deinit(s_dpp_ctx.dpp_global);
        s_dpp_ctx.dpp_global = NULL;
    }
    if (s_dpp_ctx.dpp_config_store) {
        dpp_config_store_deinit(s_dpp_ctx.dpp_config_store);
        s_dpp_ctx.dpp_config_store = NULL;
    }
    if (s_dpp_ctx.dpp_auth) {
        dpp_auth_deinit(s_dpp_ctx.dpp_auth);
        s_dpp_ctx.dpp_auth = NULL;
    }

    os_bzero(&s_dpp_ctx, sizeof(s_dpp_ctx));

    dpp_api_unlock();
    return 0;
}

static int esp_supp_rx_action(uint8_t *hdr, uint8_t *payload, size_t len, uint8_t channel)
{
    struct ieee80211_hdr *rx_hdr = (struct ieee80211_hdr *)hdr;
    struct action_rx_param *rx_param;
    int ret = ESP_ERR_NOT_SUPPORTED;

    if (!hdr || !payload || len < 2) {
        return ESP_ERR_INVALID_ARG;
    }

    struct ieee80211_action *frm = (struct ieee80211_action *)payload;

    if (WLAN_FC_GET_STYPE(rx_hdr->frame_control) == WLAN_FC_STYPE_ACTION) {
        if (frm->category != WLAN_ACTION_PUBLIC) {
            wpa_printf(MSG_DEBUG, "Not public action frame");
            return -1;
        }

        rx_param = os_zalloc(sizeof(struct action_rx_param) + len);
        if (!rx_param) {
            wpa_printf(MSG_ERROR, "Failed to allocate memory for Rx Action");
            return ESP_ERR_NO_MEM;
        }
        os_memcpy(rx_param->sa, rx_hdr->addr2, ETH_ALEN);
        rx_param->channel = channel;
        rx_param->frm_len = len;
        os_memcpy(rx_param->action_frm, payload, len);

        if (eloop_register_timeout(0, 0, esp_dpp_rx_action, rx_param, NULL) != 0) {
            os_free(rx_param);
            return ESP_ERR_NO_MEM;
        }
        ret = ESP_OK;
    }

    return ret;
}

static void esp_dpp_auth_resp_retry(void *eloop_ctx, void *timeout_ctx)
{
    struct dpp_authentication *auth;
    unsigned int max_tries = 5;

    /* Prerequisite confirmed via lifecycle (timers cancelled on deinit), lock acquisition will not fail. */
    dpp_api_lock();

    if (atomic_load(&dpp_shutting_down)) {
        dpp_api_unlock();
        return;
    }

    auth = s_dpp_ctx.dpp_auth;
    if (!auth || !auth->resp_msg) {
        dpp_api_unlock();
        return;
    }

    auth->auth_resp_tries++;
    if (auth->auth_resp_tries >= max_tries) {
        wpa_printf(MSG_INFO, "DPP: No confirm received from initiator - stopping exchange");
        dpp_abort_failure_locked(ESP_ERR_DPP_TX_FAILURE);
        return;
    }

    wpa_printf(MSG_INFO,
               "DPP: Retransmiting Authentication Response frame");
    eloop_cancel_timeout(esp_dpp_auth_resp_retry_timeout, NULL, NULL);
    dpp_api_unlock();
    esp_dpp_auth_resp_retry_timeout(NULL, NULL);
}

static void esp_dpp_peer_disc_retry(void *eloop_ctx, void *timeout_ctx)
{
    struct dpp_authentication *auth;
    struct dpp_config_store *config_store;
    uint8_t ap_bssid[ETH_ALEN];
    unsigned int max_tries = 5;

    /* Prerequisite confirmed via lifecycle (timers cancelled on deinit), lock acquisition will not fail. */
    dpp_api_lock();

    if (atomic_load(&dpp_shutting_down)) {
        dpp_api_unlock();
        return;
    }

    auth = s_dpp_ctx.dpp_auth;
    config_store = s_dpp_ctx.dpp_config_store;
    if (!auth || !config_store) {
        dpp_api_unlock();
        return;
    }

    s_dpp_ctx.gas_query_tries++;
    if (s_dpp_ctx.gas_query_tries >= max_tries) {
        wpa_printf(MSG_INFO, "DPP: No Peer Discovery response received - stopping exchange");
        dpp_abort_failure_locked(ESP_ERR_DPP_TX_FAILURE);
        return;
    }

    /* auth->peer_mac_addr is the DPP Configurator's source MAC from the Auth
     * Request and may differ from the AP BSSID. Use the BSSID saved on the
     * original call to esp_dpp_start_net_intro_protocol(). Copy it locally
     * because esp_dpp_start_net_intro_protocol_internal() memcpys its
     * argument back into config_store->peer_mac_addr. */
    os_memcpy(ap_bssid, config_store->peer_mac_addr, ETH_ALEN);

    wpa_printf(MSG_INFO, "DPP: Retransmitting Peer Discovery Request frame");
    if (esp_dpp_start_net_intro_protocol_internal(ap_bssid) != ESP_OK) {
        dpp_abort_failure_locked(ESP_ERR_DPP_TX_FAILURE);
        return;
    }
    /* Safety net: if TX status is never delivered, this timeout ensures we don't
     * hang forever. tx_status_eloop_handler will cancel and re-register with the
     * correct deadline when the TX status arrives. */
    if (eloop_register_timeout(ESP_GAS_TIMEOUT_SECS + 2, 0, peer_disc_timeout, NULL, auth) < 0) {
        wpa_printf(MSG_ERROR, "DPP: Failed to register peer_disc_timeout for intro");
        dpp_abort_failure_locked(ESP_ERR_DPP_FAILURE);
        return;
    }
    dpp_api_unlock();
}

static void esp_dpp_gas_query_req_retry(void *eloop_ctx, void *timeout_ctx)
{
    struct dpp_authentication *auth;
    unsigned int max_tries = 5;

    /* Prerequisite confirmed via lifecycle (timers cancelled on deinit), lock acquisition will not fail. */
    dpp_api_lock();

    if (atomic_load(&dpp_shutting_down)) {
        dpp_api_unlock();
        return;
    }

    auth = s_dpp_ctx.dpp_auth;
    if (!auth) {
        dpp_api_unlock();
        return;
    }

    s_dpp_ctx.gas_query_tries++;
    if (s_dpp_ctx.gas_query_tries >= max_tries) {
        wpa_printf(MSG_INFO, "DPP: No GAS response received - stopping exchange");
        dpp_abort_failure_locked(ESP_ERR_DPP_TX_FAILURE);
        return;
    }

    wpa_printf(MSG_INFO, "DPP: Retransmitting GAS Query Request frame");
    if (gas_query_req_tx(auth) != ESP_OK) {
        dpp_abort_failure_locked(ESP_ERR_DPP_TX_FAILURE);
        return;
    }
    dpp_api_unlock();
}

static void tx_status_eloop_handler(void *eloop_ctx, void *event_data)
{
    struct dpp_authentication *auth;
    wifi_event_action_tx_status_t *evt = event_data;
    enum dpp_tx_frame_type type = DPP_TX_INVALID;

    if (!evt) {
        return;
    }

    /* Prerequisite confirmed via lifecycle (timers cancelled on deinit), lock acquisition will not fail. */
    dpp_api_lock();

    if (atomic_load(&dpp_shutting_down)) {
        os_free(evt);
        dpp_api_unlock();
        return;
    }

    auth = s_dpp_ctx.dpp_auth;

    wpa_printf(MSG_DEBUG, "Mgmt Tx Status - %d, Cookie - 0x%x",
               evt->status, (uint32_t)evt->context);

    if (s_dpp_ctx.pending_tx_op_in_progress && s_dpp_ctx.pending_tx_op.op_id == evt->op_id) {
        type = s_dpp_ctx.pending_tx_op.type;
        s_dpp_ctx.pending_tx_op_in_progress = false;
    }

    if (type == DPP_TX_INVALID) {
        wpa_printf(MSG_DEBUG, "DPP: status not for recent frame op_id=%u", evt->op_id);
    } else if (!auth) {
        wpa_printf(MSG_DEBUG, "Auth already deinitialized, return");
    } else if (type == DPP_TX_AUTHENTICATION_RESP) {
        eloop_cancel_timeout(esp_dpp_auth_resp_retry_timeout, NULL, NULL);
        if (evt->status == WIFI_ACTION_TX_FAILED) {
            /* failed to send auth response frame */
            eloop_cancel_timeout(esp_dpp_auth_conf_wait_timeout, NULL, NULL);
            if (eloop_register_timeout(1, 0, esp_dpp_auth_resp_retry, NULL, NULL) < 0) {
                wpa_printf(MSG_ERROR, "DPP: Failed to register auth_resp_retry");
                dpp_abort_failure_locked(ESP_ERR_DPP_FAILURE);
                os_free(evt);
                return;
            }
        } else if (evt->status == WIFI_ACTION_TX_DONE) {
            eloop_cancel_timeout(esp_dpp_auth_conf_wait_timeout, NULL, NULL);
            if (eloop_register_timeout(ESP_DPP_AUTH_TIMEOUT_SECS, 0, esp_dpp_auth_conf_wait_timeout, NULL, NULL) < 0) {
                wpa_printf(MSG_ERROR, "DPP: Failed to register auth_conf_wait_timeout");
                dpp_abort_failure_locked(ESP_ERR_DPP_FAILURE);
                os_free(evt);
                return;
            }
        }
    } else if (type == DPP_TX_PEER_DISCOVERY_REQ) {
        if (evt->status == WIFI_ACTION_TX_FAILED) {
            /* failed to send Peer Discovery frame, retry */
            wpa_printf(MSG_WARNING, "DPP: failed to send Peer Discovery frame, retrying");
            eloop_cancel_timeout(peer_disc_timeout, NULL, auth);
            if (eloop_register_timeout(1, 0, esp_dpp_peer_disc_retry, NULL, NULL) < 0) {
                wpa_printf(MSG_ERROR, "DPP: Failed to register esp_dpp_peer_disc_retry");
                dpp_abort_failure_locked(ESP_ERR_DPP_FAILURE);
                os_free(evt);
                return;
            }
        } else if (evt->status == WIFI_ACTION_TX_DONE) {
            /* Peer discovery sent, wait for response. */
            eloop_cancel_timeout(peer_disc_timeout, NULL, auth);
            if (eloop_register_timeout(ESP_GAS_TIMEOUT_SECS, 0, peer_disc_timeout, NULL, auth) < 0) {
                wpa_printf(MSG_ERROR, "DPP: Failed to register peer_disc_timeout after disc TX");
                dpp_abort_failure_locked(ESP_ERR_DPP_FAILURE);
                os_free(evt);
                return;
            }
        }
    } else if (type == DPP_TX_AUTHENTICATION_CONF) {
        /* Note: ESP-IDF currently only acts as Enrollee, so it never transmits
         * AUTH_CONF. This branch is for future Configurator role support. */
        if (evt->status == WIFI_ACTION_TX_FAILED) {
            wpa_printf(MSG_WARNING, "DPP: failed to send Auth Confirm frame");
        } else if (evt->status == WIFI_ACTION_TX_DONE) {
            /* Auth confirm sent, peer should start configuration */
            wpa_printf(MSG_DEBUG, "DPP: Auth Confirm frame sent");
        }
    } else if (type == DPP_TX_GAS_CONFIG_REQ) {
        if (evt->status == WIFI_ACTION_TX_FAILED) {
            /* failed to send gas query frame, retry */
            wpa_printf(MSG_WARNING, "DPP: failed to send GAS query frame");
            eloop_cancel_timeout(gas_query_timeout, NULL, auth);
            if (eloop_register_timeout(1, 0, esp_dpp_gas_query_req_retry, NULL, NULL) < 0) {
                wpa_printf(MSG_ERROR, "DPP: Failed to register gas_query_req_retry");
                dpp_abort_failure_locked(ESP_ERR_DPP_FAILURE);
                os_free(evt);
                return;
            }
        } else if (evt->status == WIFI_ACTION_TX_DONE) {
            eloop_cancel_timeout(gas_query_timeout, NULL, auth);
            if (eloop_register_timeout(ESP_GAS_TIMEOUT_SECS, 0, gas_query_timeout, NULL, auth) < 0) {
                wpa_printf(MSG_ERROR, "DPP: Failed to register gas_query_timeout after GAS TX");
                dpp_abort_failure_locked(ESP_ERR_DPP_FAILURE);
                os_free(evt);
                return;
            }
        }
    } else if (type == DPP_TX_GAS_COMEBACK) {
        if (auth->auth_success) {
            if (evt->status == WIFI_ACTION_TX_FAILED) {
                wpa_printf(MSG_WARNING, "DPP: Failed to send GAS Comeback Request");
                dpp_abort_failure_locked(ESP_ERR_DPP_TX_FAILURE);
                os_free(evt);
                return;
            } else if (evt->status == WIFI_ACTION_TX_DONE) {
                /* Re-arm response timeout from actual send time (more precise
                 * than the timeout armed at queue time in gas_comeback_req_tx). */
                eloop_cancel_timeout(gas_query_timeout, NULL, auth);
                if (eloop_register_timeout(ESP_GAS_TIMEOUT_SECS, 0,
                                           gas_query_timeout, NULL, auth) < 0) {
                    wpa_printf(MSG_ERROR, "DPP: Failed to register gas_query_timeout after comeback TX");
                    dpp_abort_failure_locked(ESP_ERR_DPP_FAILURE);
                    os_free(evt);
                    return;
                }
            }
        }
    }
    os_free(evt);
    dpp_api_unlock();
}

static void tx_status_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data)
{
    if (atomic_load(&dpp_shutting_down)) {
        return;
    }

    wifi_event_action_tx_status_t *evt_c = os_malloc(sizeof(*evt_c));
    if (evt_c) {
        os_memcpy(evt_c, event_data, sizeof(*evt_c));
        if (eloop_register_timeout(0, 0, tx_status_eloop_handler, NULL, evt_c) < 0) {
            wpa_printf(MSG_ERROR, "DPP: Failed to register TX status handler");
            os_free(evt_c);
        }
    } else {
        wpa_printf(MSG_ERROR, "DPP: Failed to allocate memory for TX status");
    }
}

static void roc_status_eloop_handler(void *eloop_ctx, void *event_data)
{
    wifi_event_roc_done_t *evt = (wifi_event_roc_done_t *)event_data;
    void *eg;

    /* Prerequisite confirmed via lifecycle (timers cancelled on deinit), lock acquisition will not fail. */
    dpp_api_lock();

    if (atomic_load(&dpp_shutting_down)) {
        if (evt) {
            os_free(evt);
        }
        atomic_store(&roc_in_progress, false);
        dpp_api_unlock();
        return;
    }

    eg = s_dpp_event_group;

    if (evt) {
        if (evt->context == (uint32_t)s_action_rx_cb) {
            eloop_cancel_timeout(dpp_listen_next_channel, NULL, NULL);
            if (eloop_register_timeout(0, 0, dpp_listen_next_channel, NULL, NULL) < 0) {
                wpa_printf(MSG_ERROR,
                           "DPP: Failed to register listen_next_channel after ROC; listen may stall");
                atomic_store(&roc_in_progress, false);
                if (eg) {
                    os_event_group_set_bits(eg, DPP_ROC_EVENT_HANDLED);
                }
                os_free(evt);
                dpp_abort_failure_locked(ESP_ERR_NO_MEM);
                return;
            }
        }
        os_free(evt);
    }

    atomic_store(&roc_in_progress, false);
    if (eg) {
        os_event_group_set_bits(eg, DPP_ROC_EVENT_HANDLED);
    }
    dpp_api_unlock();
}

static void roc_status_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (atomic_load(&dpp_shutting_down)) {
        return;
    }

    wifi_event_roc_done_t *evt_c = os_malloc(sizeof(*evt_c));
    if (evt_c) {
        os_memcpy(evt_c, event_data, sizeof(*evt_c));
        if (eloop_register_timeout(0, 0, roc_status_eloop_handler, NULL, evt_c) < 0) {
            os_free(evt_c);
        }
    } else {
        wpa_printf(MSG_ERROR, "DPP: Failed to allocate memory for ROC status");
    }
}

static char *esp_dpp_parse_chan_list(const char *chan_list)
{
    struct dpp_bootstrap_params_t *params = &s_dpp_ctx.bootstrap_params;
    size_t max_uri_len = ESP_DPP_MAX_CHAN_COUNT * 8 + os_strlen(" chan=") + 1;
    uint8_t local_chan_list[ESP_DPP_MAX_CHAN_COUNT];
    uint8_t local_num_chan = 0;

    if (!chan_list) {
        return NULL;
    }

    char *uri_channels = os_zalloc(max_uri_len);
    if (!uri_channels) {
        wpa_printf(MSG_WARNING, "DPP: URI allocation failed");
        return NULL;
    }

    char *uri_ptr = uri_channels;
    size_t current_offset = 0;

    int written = os_snprintf(uri_ptr + current_offset, max_uri_len - current_offset, " chan=");
    if (written < 0 || written >= max_uri_len - current_offset) {
        wpa_printf(MSG_ERROR, "DPP: URI buffer too small for initial string");
        os_free(uri_channels);
        return NULL;
    }
    current_offset += written;

    while (*chan_list && local_num_chan < ESP_DPP_MAX_CHAN_COUNT) {
        const char *token_start = chan_list;
        int channel = 0;
        bool channel_digits_overflow = false;

        while (*chan_list >= '0' && *chan_list <= '9') {
            int digit = *chan_list - '0';

            if (channel > (INT_MAX - digit) / 10) {
                channel_digits_overflow = true;
                while (*chan_list >= '0' && *chan_list <= '9') {
                    chan_list++;
                }
                break;
            }
            channel = channel * 10 + digit;
            chan_list++;
        }
        if (channel_digits_overflow) {
            wpa_printf(MSG_WARNING, "DPP: channel number too large in chan_list");
            while (*chan_list == ',' || *chan_list == ' ') {
                chan_list++;
            }
            continue;
        }
        if (token_start == chan_list) {
            wpa_printf(MSG_WARNING, "DPP: Skipping unexpected channel list character '%c'", *chan_list);
            chan_list++;
            continue;
        }

        if (channel < 1 || channel > 255) {
            wpa_printf(MSG_WARNING, "DPP: Skipping channel out of range %d", channel);
            while (*chan_list == ',' || *chan_list == ' ') {
                chan_list++;
            }
            continue;
        }

        if (CHANNEL_TO_BIT_NUMBER(channel) == 0) {
            wpa_printf(MSG_WARNING, "DPP: Skipping invalid channel %d", channel);
            while (*chan_list == ',' || *chan_list == ' ') {
                chan_list++;
            }
            continue;
        }

        u8 oper_class = get_operating_class(channel, 0);
        if (oper_class == 0) {
            wpa_printf(MSG_WARNING, "DPP: Skipping channel %d due to missing operating class", channel);
            while (*chan_list == ',' || *chan_list == ' ') {
                chan_list++;
            }
            continue;
        }

        local_chan_list[local_num_chan] = channel;
        local_num_chan++;

        written = os_snprintf(uri_ptr + current_offset, max_uri_len - current_offset, "%d/%d,", oper_class, channel);
        if (written < 0 || written >= max_uri_len - current_offset) {
            wpa_printf(MSG_ERROR, "DPP: URI buffer too small while adding channel");
            os_free(uri_channels);
            return NULL;
        }
        current_offset += written;

        while (*chan_list == ',' || *chan_list == ' ') {
            chan_list++;
        }
    }

    if (local_num_chan == 0) {
        wpa_printf(MSG_WARNING, "DPP: No valid channels found in list");
        os_free(uri_channels);
        return NULL;
    }

    if (current_offset > os_strlen(" chan=") && uri_ptr[current_offset - 1] == ',') {
        uri_ptr[current_offset - 1] = ' ';
    }

    /* Success: commit local values to global context */
    params->num_chan = local_num_chan;
    os_memcpy(params->chan_list, local_chan_list, local_num_chan);

    return uri_channels;
}

esp_err_t
esp_supp_dpp_bootstrap_gen(const char *chan_list, esp_supp_dpp_bootstrap_t type,
                           const char *key, const char *info)
{
    if (!atomic_load(&s_dpp_init_done)) {
        wpa_printf(MSG_ERROR, "DPP: failed to bootstrap as dpp not initialized.");
        return ESP_ERR_INVALID_STATE;
    }

    /* Prerequisite confirmed via atomic check, lock acquisition will not fail. */
    dpp_api_lock();

    if (!atomic_load(&s_dpp_init_done)) {
        wpa_printf(MSG_ERROR, "DPP: failed to bootstrap as dpp not initialized.");
        dpp_api_unlock();
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret = ESP_OK;
    char *uri_chan_list = NULL;
    char *command = NULL;
    struct dpp_bootstrap_params_t *params = &s_dpp_ctx.bootstrap_params;
    uri_chan_list = esp_dpp_parse_chan_list(chan_list);

    if (!uri_chan_list || params->num_chan > ESP_DPP_MAX_CHAN_COUNT || params->num_chan == 0) {
        wpa_printf(MSG_ERROR, "DPP: Invalid channel list - %s", chan_list ? chan_list : "(null)");
        s_dpp_ctx.bootstrap_done = false;
        ret = ESP_ERR_DPP_INVALID_LIST;
        goto fail;
    }

    if (type != DPP_BOOTSTRAP_QR_CODE) {
        wpa_printf(MSG_INFO, "Bootstrap type %d not supported", type);
        ret = ESP_ERR_NOT_SUPPORTED;
        goto fail;
    }

    size_t info_len = info ? os_strlen(info) : 0;
    size_t command_len = os_strlen("type=qrcode mac=") + 17 + os_strlen(uri_chan_list) +
                         (key ? os_strlen(" key=") + os_strlen(key) : 0) +
                         (info_len ? os_strlen(" info=") + info_len : 0) + 1;

    command = os_zalloc(command_len);
    if (!command) {
        wpa_printf(MSG_ERROR, "DPP: Failed to allocate memory for bootstrap command");
        ret = ESP_ERR_NO_MEM;
        goto fail;
    }

    params->type = type;
    esp_wifi_get_mac(WIFI_IF_STA, params->mac);

    os_snprintf(command, command_len, "type=qrcode mac=" MACSTR "%s%s%s%s%s",
                MAC2STR(params->mac), uri_chan_list,
                key ? " key=" : "", key ? key : "",
                info_len ? " info=" : "",
                info_len ? info : "");

    os_free(uri_chan_list);
    uri_chan_list = NULL;

    if (eloop_register_timeout(0, 0, esp_dpp_bootstrap_gen, command, NULL) < 0) {
        wpa_printf(MSG_ERROR, "DPP: Failed to register bootstrap_gen timeout");
        os_free(command);
        ret = ESP_ERR_NO_MEM;
        goto fail;
    }

    ret = ESP_OK;
fail:
    if (ret != ESP_OK) {
        // No info to free
    }
    if (uri_chan_list) {
        os_free(uri_chan_list);
    }

    dpp_api_unlock();
    return ret;
}

static void dpp_listen_start(void *ctx, void *data)
{
    /* Prerequisite confirmed by caller, lock acquisition will not fail. */
    dpp_api_lock();
    s_dpp_ctx.dpp_listen_ongoing = true;
    dpp_api_unlock();

    dpp_listen_next_channel(NULL, NULL);
}

static void dpp_cancel_auth_gas_eloop_timeouts(void)
{
    eloop_cancel_timeout(esp_dpp_auth_resp_retry_timeout, NULL, NULL);
    eloop_cancel_timeout(esp_dpp_auth_conf_wait_timeout, NULL, NULL);
    eloop_cancel_timeout(esp_dpp_auth_resp_retry, NULL, NULL);
    eloop_cancel_timeout(gas_query_timeout, ELOOP_ALL_CTX, ELOOP_ALL_CTX);
    eloop_cancel_timeout(gas_comeback_delay_timeout, ELOOP_ALL_CTX, ELOOP_ALL_CTX);
    eloop_cancel_timeout(esp_dpp_peer_disc_retry, NULL, NULL);
    eloop_cancel_timeout(peer_disc_timeout, ELOOP_ALL_CTX, ELOOP_ALL_CTX);
    eloop_cancel_timeout(esp_dpp_gas_query_req_retry, NULL, NULL);
}

static void esp_dpp_cancel_timeouts(void)
{
    eloop_cancel_timeout(dpp_listen_next_channel, NULL, NULL);
    dpp_cancel_auth_gas_eloop_timeouts();
    eloop_cancel_timeout(dpp_listen_start, NULL, NULL);
}

esp_err_t esp_supp_dpp_start_listen(void)
{
    if (!atomic_load(&s_dpp_init_done)) {
        wpa_printf(MSG_ERROR, "DPP: failed to start listen as dpp not initialized.");
        return ESP_ERR_INVALID_STATE;
    }

    /* Prerequisite confirmed via atomic check, lock acquisition will not fail. */
    dpp_api_lock();

    if (!atomic_load(&s_dpp_init_done)) {
        wpa_printf(MSG_ERROR, "DPP: failed to start listen as dpp not initialized.");
        dpp_api_unlock();
        return ESP_ERR_INVALID_STATE;
    }

    if (!s_dpp_ctx.bootstrap_done) {
        wpa_printf(MSG_ERROR, "DPP: failed to start listen as dpp not bootstrapped.");
        dpp_api_unlock();
        return ESP_ERR_INVALID_STATE;
    }

    if (esp_wifi_get_user_init_flag_internal() == 0) {
        wpa_printf(MSG_ERROR, "DPP: ROC not possible before wifi is started");
        dpp_api_unlock();
        return ESP_ERR_INVALID_STATE;
    }
    dpp_api_unlock();

    /* cancel previous ROC if ongoing */
    esp_supp_dpp_stop_listen();

    /* Prerequisite confirmed via atomic check, lock acquisition will not fail. */
    dpp_api_lock();

    /* Re-verify lifecycle: stop_listen released the lock and blocked, so state may have changed.
     * Note: bootstrap_done is not re-checked here as it is not needed to access the event group;
     * it will be re-validated in the final locked section after the bit wait. */
    if (atomic_load(&dpp_shutting_down) || !atomic_load(&s_dpp_init_done)) {
        dpp_api_unlock();
        return ESP_ERR_INVALID_STATE;
    }

    /* Give ample time to set the bit, timeout is necessary when ROC is not running previously.
     * Capture the event group handle locally and release the lock BEFORE blocking, so the
     * eloop task is never starved waiting on the mutex (avoids priority inversion with
     * dpp_stop/dpp_abort_with_failure which also acquire this lock). */
    if (!s_dpp_event_group) {
        dpp_api_unlock();
        return ESP_ERR_INVALID_STATE;
    }
    void *eg = s_dpp_event_group;
    dpp_api_unlock();

    os_event_group_wait_bits(eg, DPP_ROC_EVENT_HANDLED, 0, 0,
                             os_task_ms_to_tick(100));

    /* Re-acquire and re-validate: deinit may have run while we were waiting */
    /* Prerequisite confirmed via atomic check, lock acquisition will not fail. */
    dpp_api_lock();

    if (atomic_load(&dpp_shutting_down) || !atomic_load(&s_dpp_init_done) ||
            !s_dpp_ctx.bootstrap_done) {
        dpp_api_unlock();
        return ESP_ERR_INVALID_STATE;
    }

    wpa_printf(MSG_DEBUG, "DPP: Starting ROC");
    if (eloop_register_timeout(0, 0, dpp_listen_start, NULL, NULL) < 0) {
        wpa_printf(MSG_ERROR, "DPP: Failed to register listen_start timeout");
        dpp_api_unlock();
        return ESP_ERR_NO_MEM;
    }

    dpp_api_unlock();
    return ESP_OK;
}

esp_err_t esp_supp_dpp_stop_listen(void)
{
    if (!atomic_load(&s_dpp_init_done)) {
        return ESP_OK;
    }

    int ret = eloop_register_timeout_blocking(listen_stop_handler, NULL, NULL);

    if (ret) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

/* this is called from public API, need protection */
bool is_dpp_enabled(void)
{
    bool ret;

    if (!atomic_load(&s_dpp_init_done)) {
        return false;
    }

    /* Prerequisite confirmed via atomic check, lock acquisition will not fail. */
    dpp_api_lock();

    if (!atomic_load(&s_dpp_init_done)) {
        dpp_api_unlock();
        return false;
    }

    ret = (s_dpp_ctx.dpp_global != NULL);
    dpp_api_unlock();
    return ret;
}

static int esp_dpp_init(void *eloop_data, void *user_ctx)
{
    struct dpp_global_config cfg = {0};
    int ret;

    cfg.msg_ctx = &s_dpp_ctx;

    /* Lock handle is created in esp_supp_dpp_common_init and never freed.
     * Lock acquisition here will not fail. */
    dpp_api_lock();

    os_bzero(&s_dpp_ctx, sizeof(s_dpp_ctx));
    atomic_store(&dpp_shutting_down, false);
    atomic_store(&roc_in_progress, false);

    s_dpp_ctx.dpp_global = dpp_global_init(&cfg);
    if (!s_dpp_ctx.dpp_global) {
        wpa_printf(MSG_ERROR, "DPP: failed to allocate memory for dpp_global");
        ret = ESP_ERR_NO_MEM;
        goto init_fail;
    }

    s_dpp_ctx.dpp_config_store = dpp_config_store_init();
    if (!s_dpp_ctx.dpp_config_store) {
        wpa_printf(MSG_ERROR, "DPP: failed to allocate memory for config store");
        ret = ESP_ERR_NO_MEM;
        goto init_fail;
    }

    ret = esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_ACTION_TX_STATUS,
                                     &tx_status_handler, NULL);
    if (ret != ESP_OK) {
        wpa_printf(MSG_ERROR, "DPP: failed to register TX status handler");
        ret = ESP_FAIL;
        goto init_fail;
    }
    ret = esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_ROC_DONE,
                                     &roc_status_handler, NULL);
    if (ret != ESP_OK) {
        wpa_printf(MSG_ERROR, "DPP: failed to register ROC done handler");
        ret = ESP_FAIL;
        goto init_fail;
    }

    wpa_printf(MSG_INFO, "DPP: dpp init done");
    atomic_store(&s_dpp_init_done, true);
    dpp_api_unlock();

    return ESP_OK;
init_fail:
    atomic_store(&dpp_shutting_down, true);
    dpp_api_unlock();
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_ACTION_TX_STATUS,
                                 &tx_status_handler);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_ROC_DONE,
                                 &roc_status_handler);
    dpp_api_lock();
    if (s_dpp_ctx.dpp_global) {
        dpp_global_deinit(s_dpp_ctx.dpp_global);
        s_dpp_ctx.dpp_global = NULL;
    }
    if (s_dpp_ctx.dpp_config_store) {
        dpp_config_store_deinit(s_dpp_ctx.dpp_config_store);
        s_dpp_ctx.dpp_config_store = NULL;
    }
    dpp_api_unlock();
    return ret;

}

esp_err_t esp_supp_dpp_init(void)
{
    int eloop_ret;

    wifi_mode_t mode = 0;
    if (esp_wifi_get_mode(&mode) || ((mode != WIFI_MODE_STA) && (mode != WIFI_MODE_APSTA))) {
        wpa_printf(MSG_ERROR, "DPP: failed to init as not in station mode.");
        return ESP_ERR_DPP_FAILURE;
    }

    if (wps_get_owner() != WPS_OWNER_NONE) {
        wpa_printf(MSG_ERROR, "DPP: failed to init since WPS is enabled");
        return ESP_ERR_DPP_FAILURE;
    }

    if (atomic_load(&s_dpp_init_done)) {
        return ESP_ERR_DPP_FAILURE;
    }

    /* Prerequisite confirmed via check above, lock acquisition will not fail. */
    dpp_api_lock();

    if (atomic_load(&s_dpp_init_done)) {
        wpa_printf(MSG_DEBUG, "DPP: init already completed by another thread");
        dpp_api_unlock();
        return ESP_ERR_DPP_FAILURE;
    }

    if (s_dpp_init_pending) {
        wpa_printf(MSG_ERROR, "DPP: init already in progress");
        dpp_api_unlock();
        return ESP_ERR_DPP_FAILURE;
    }

    s_dpp_init_pending = true;
    dpp_api_unlock();

    eloop_ret = eloop_register_timeout_blocking(esp_dpp_init, NULL, NULL);

    /* Prerequisite confirmed at start of function, lock re-acquisition will not fail. */
    dpp_api_lock();
    s_dpp_init_pending = false;
    dpp_api_unlock();

    if (eloop_ret != 0) {
        return ESP_ERR_DPP_FAILURE;
    }
    return ESP_OK;
}

static esp_err_t esp_dpp_start_net_intro_protocol_internal(uint8_t *bssid)
{
    struct dpp_conf *config;
    struct dpp_config_store *config_store;
    struct wpabuf *buf = NULL;
    esp_err_t ret = ESP_OK;
    uint8_t curr_chan;

    config = esp_dpp_config_store_get_selected_entry();
    if (!config) {
        wpa_printf(MSG_ERROR, "DPP: No connection configuration set");
        return ESP_ERR_INVALID_STATE;
    }

    config_store = s_dpp_ctx.dpp_config_store;
    os_memcpy(config_store->peer_mac_addr, bssid, ETH_ALEN);
    curr_chan = config->curr_chan;
    buf = dpp_build_peer_disc_req(config_store, config);

    if (!buf) {
        return ESP_ERR_DPP_FAILURE;
    }

    ret = esp_dpp_send_action_frame(bssid, wpabuf_head(buf), wpabuf_len(buf),
                                    curr_chan, 1000 + OFFCHAN_TX_WAIT_TIME,
                                    DPP_TX_PEER_DISCOVERY_REQ);

    wpabuf_free(buf);

    return ret;
}

esp_err_t esp_dpp_start_net_intro_protocol(uint8_t *bssid)
{
    esp_err_t ret;

    if (!bssid) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!atomic_load(&s_dpp_init_done)) {
        return ESP_ERR_INVALID_STATE;
    }

    /* Prerequisite confirmed via atomic check, lock acquisition will not fail. */
    dpp_api_lock();

    if (s_dpp_ctx.dpp_deinit_pending) {
        dpp_api_unlock();
        return ESP_ERR_INVALID_STATE;
    }

    /* Ensure no stale Auth, GAS, or Discovery timers from a previous attempt
     * are running. If they fire, they would abort the new attempt prematurely.
     * Use the narrower auth/GAS/disc cancel; full esp_dpp_cancel_timeouts()
     * is reserved for deinit. */
    dpp_cancel_auth_gas_eloop_timeouts();
    s_dpp_ctx.gas_query_tries = 0;

    wpa_printf(MSG_INFO, "DPP: Starting Network Introduction to " MACSTR, MAC2STR(bssid));

    ret = esp_dpp_start_net_intro_protocol_internal(bssid);
    if (ret != ESP_OK) {
        /* Normalize generic ESP_FAIL or ESP_ERR_INVALID_STATE to
         * a DPP-specific failure so the application-facing event reason
         * is meaningful and consistent. */
        esp_err_t reason = (ret == ESP_FAIL) ? ESP_ERR_DPP_TX_FAILURE :
                           (ret == ESP_ERR_INVALID_STATE) ? ESP_ERR_DPP_FAILURE : ret;
        dpp_abort_failure_locked(reason);
        return ret;
    }

    /* If esp_dpp_start_net_intro_protocol_internal returns ESP_OK, the frame was
     * successfully queued to the Wi-Fi driver. The driver is guaranteed to generate
     * a TX status event (WIFI_EVENT_ACTION_TX_STATUS). The timeout for the peer's
     * response will be registered in tx_status_eloop_handler once the frame is sent. */
    dpp_api_unlock();
    return ret;
}

esp_err_t esp_supp_dpp_deinit(void)
{
    int eloop_ret;
    esp_err_t ret;

    ret = dpp_api_lock();
    if (ret != ESP_OK) {
        /* Lock not initialized means common_init was never called; nothing to deinit. */
        return ESP_OK;
    }

    if (!atomic_load(&s_dpp_init_done)) {
        if (s_dpp_init_pending) {
            wpa_printf(MSG_ERROR, "DPP: deinit failed as init is in progress");
            dpp_api_unlock();
            return ESP_ERR_INVALID_STATE;
        }
        wpa_printf(MSG_DEBUG, "DPP: deinit already completed or not initialized");
        dpp_api_unlock();
        return ESP_OK;
    }

    if (s_dpp_ctx.dpp_deinit_pending) {
        wpa_printf(MSG_DEBUG, "DPP: deinit already scheduled");
        dpp_api_unlock();
        return ESP_OK;
    }

    s_dpp_ctx.dpp_deinit_pending = true;
    dpp_api_unlock();
    eloop_ret = eloop_register_timeout_blocking(esp_dpp_deinit, NULL, NULL);
    if (eloop_ret != 0) {
        /* Prerequisite confirmed at function entry, lock re-acquisition will not fail. */
        dpp_api_lock();
        s_dpp_ctx.dpp_deinit_pending = false;
        dpp_api_unlock();
        return ESP_ERR_DPP_FAILURE;
    }
    return ESP_OK;
}

static bool esp_dpp_stored_conf_matches_row(struct dpp_config_store *dc,
                                            const esp_dpp_config_data_t *row)
{
    struct dpp_conf *c;
    enum dpp_akm want;

    if (!dc || !row) {
        return false;
    }

    c = dc->conf;
    want = (enum dpp_akm) row->akm;
    if (!c || !dpp_akm_dpp(c->akm) || c->akm != want || c->curr_chan != row->curr_chan) {
        return false;
    }
    if (row->connector_len > 0) {
        size_t cfg_clen;
        size_t c_clen;

        cfg_clen = row->connector_len;
        if (cfg_clen >= ESP_DPP_MAX_CONNECTOR_LEN) {
            return false;
        }
        if (!c->connector) {
            return false;
        }
        c_clen = os_strlen(c->connector);
        if (c_clen != cfg_clen ||
                os_memcmp(c->connector, row->connector, cfg_clen) != 0) {
            return false;
        }
    } else if (c->connector) {
        return false;
    }

    return true;
}

static esp_err_t esp_dpp_conf_alloc_from_config_data(const esp_dpp_config_data_t *config,
                                                     struct dpp_conf **out_conf)
{
    struct dpp_conf *new_conf = NULL;

    *out_conf = NULL;

    if (!config) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!dpp_akm_dpp((enum dpp_akm) config->akm)) {
        return ESP_ERR_INVALID_ARG;
    }

    if (config->connector_len == 0 || config->net_access_key_len == 0 ||
            config->c_sign_key_len == 0) {
        wpa_printf(MSG_ERROR, "DPP: Incomplete DPP configuration row");
        return ESP_ERR_INVALID_ARG;
    }

    if (config->net_access_key_len > ESP_DPP_MAX_KEY_LEN ||
            config->c_sign_key_len > ESP_DPP_MAX_KEY_LEN) {
        wpa_printf(MSG_ERROR, "DPP: Invalid key length in config");
        return ESP_ERR_INVALID_ARG;
    }

    new_conf = os_zalloc(sizeof(struct dpp_conf));
    if (!new_conf) {
        return ESP_ERR_NO_MEM;
    }

    if (config->connector_len > 0) {
        size_t clen;

        clen = config->connector_len;
        if (clen >= ESP_DPP_MAX_CONNECTOR_LEN) {
            wpa_printf(MSG_ERROR, "DPP: connector_len out of range");
            dpp_clear_confs(new_conf);
            return ESP_ERR_INVALID_ARG;
        }

        new_conf->connector = os_malloc(clen + 1);
        if (!new_conf->connector) {
            goto fail;
        }
        os_memcpy(new_conf->connector, config->connector, clen);
        new_conf->connector[clen] = '\0';
    }
    if (config->net_access_key_len) {
        new_conf->net_access_key = wpabuf_alloc_copy(config->net_access_key, config->net_access_key_len);
        if (!new_conf->net_access_key) {
            goto fail;
        }
        new_conf->net_access_key_expiry = config->net_access_key_expiry;
    }
    if (config->c_sign_key_len) {
        new_conf->c_sign_key = wpabuf_alloc_copy(config->c_sign_key, config->c_sign_key_len);
        if (!new_conf->c_sign_key) {
            goto fail;
        }
        new_conf->dpp_csign_len = config->c_sign_key_len;
    }
    new_conf->curr_chan = config->curr_chan;
    new_conf->akm = (enum dpp_akm) config->akm;

    *out_conf = new_conf;
    return ESP_OK;

fail:
    dpp_clear_confs(new_conf);
    return ESP_ERR_NO_MEM;
}

static struct dpp_conf *esp_dpp_config_store_get_selected_entry(void)
{
    struct dpp_config_store *dc = s_dpp_ctx.dpp_config_store;

    if (!dc) {
        return NULL;
    }
    return dc->conf;
}

esp_err_t esp_supp_dpp_set_config(const esp_dpp_config_data_t *config)
{
    struct dpp_conf *new_conf = NULL;
    struct dpp_config_store *dc;
    esp_err_t err;

    err = dpp_api_lock();
    if (err != ESP_OK) {
        return err;
    }

    if (!atomic_load(&s_dpp_init_done)) {
        dpp_api_unlock();
        return ESP_ERR_INVALID_STATE;
    }

    dc = s_dpp_ctx.dpp_config_store;

    if (!config) {
        if (dc && dc->conf) {
            dpp_clear_confs(dc->conf);
            dc->conf = NULL;
        }
        dpp_api_unlock();
        return ESP_OK;
    }

    if (!dc) {
        dpp_api_unlock();
        return ESP_ERR_INVALID_STATE;
    }

    if (esp_dpp_stored_conf_matches_row(dc, config)) {
        dpp_api_unlock();
        return ESP_OK;
    }

    err = esp_dpp_conf_alloc_from_config_data(config, &new_conf);
    if (err != ESP_OK) {
        dpp_api_unlock();
        return err;
    }

    dpp_clear_confs(dc->conf);
    dc->conf = new_conf;

    dpp_api_unlock();

    return ESP_OK;
}

esp_err_t esp_supp_dpp_common_init(void)
{
    if (!s_dpp_api_lock) {
        s_dpp_api_lock = os_recursive_mutex_create();
    }
    /* Note: s_dpp_api_lock is a lifetime resource for the DPP module.
     * It is not deleted even if subsequent initialization steps (like event
     * group creation) fail, as it remains valid for retry attempts. */
    if (!s_dpp_api_lock) {
        wpa_printf(MSG_ERROR, "DPP: failed to create DPP API lock");
        return ESP_ERR_NO_MEM;
    }

    if (!s_dpp_event_group) {
        s_dpp_event_group = os_event_group_create();
    }
    if (!s_dpp_event_group) {
        wpa_printf(MSG_ERROR, "DPP: failed to create DPP event group");
        return ESP_ERR_NO_MEM;
    }

    return ESP_OK;
}
#endif
