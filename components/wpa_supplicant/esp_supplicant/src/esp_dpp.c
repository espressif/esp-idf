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

    if (!atomic_load(&s_dpp_init_done) || atomic_load(&dpp_shutting_down)) {
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
    req->type = WIFI_OFFCHAN_TX_REQ;
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

static void gas_query_timeout(void *eloop_data, void *user_ctx)
{
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

static int gas_query_req_tx(struct dpp_authentication *auth)
{
    struct wpabuf *buf;
    int supp_op_classes[] = {81, 0};
    int ret;

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
    if (eloop_register_timeout(2, 0, gas_query_timeout, NULL, auth) < 0) {
        wpa_printf(MSG_ERROR, "DPP: Failed to register gas_query_timeout");
        wpabuf_free(auth->conf_req);
        auth->conf_req = NULL;
        return ESP_ERR_NO_MEM;
    }

    return ret;
}

static esp_err_t esp_dpp_handle_config_obj(struct dpp_authentication *auth,
                                           struct dpp_config_obj *conf)
{
    wifi_config_t *wifi_cfg = &s_dpp_ctx.wifi_cfg;
    os_memset(wifi_cfg, 0, sizeof(wifi_config_t));

    if (conf->ssid_len) {
        os_memcpy(wifi_cfg->sta.ssid, conf->ssid, conf->ssid_len);
    }

    if (dpp_akm_legacy(conf->akm)) {
        if (conf->passphrase[0])
            os_memcpy(wifi_cfg->sta.password, conf->passphrase,
                      sizeof(wifi_cfg->sta.password));
        if (conf->akm == DPP_AKM_PSK_SAE) {
            wifi_cfg->sta.pmf_cfg.required = true;
        }
    }

    if (conf->connector) {
        /* TODO: Save the Connector and consider using a command
         * to fetch the value instead of sending an event with
         * it. The Connector could end up being larger than what
         * most clients are receive as an event
         * message. */
        wpa_printf(MSG_INFO, DPP_EVENT_CONNECTOR "%s",
                   conf->connector);
    }
    if (atomic_load(&roc_in_progress)) {
        listen_stop_handler(NULL, NULL);
    }

    wifi_event_dpp_config_received_t event = {0};
    event.wifi_cfg = *wifi_cfg;

    dpp_api_unlock();
    esp_err_t ret = esp_event_post(WIFI_EVENT, WIFI_EVENT_DPP_CFG_RECVD, &event, sizeof(event),
                                   os_task_ms_to_tick(200));

    /* Prerequisite confirmed via atomic check at entry point, lock re-acquisition will not fail. */
    dpp_api_lock();

    /* Re-verify auth context: another task (e.g. deinit) could have freed s_dpp_ctx.dpp_auth
     * while the lock was released. Note: ABA (pointer reuse) is not possible here because
     * dpp_auth allocation is strictly serialized on the eloop task, which is currently
     * occupied by this function. A pointer mismatch is sufficient to detect invalidation. */
    if (!atomic_load(&s_dpp_init_done) || atomic_load(&dpp_shutting_down) || s_dpp_ctx.dpp_auth != auth) {
        wpa_printf(MSG_ERROR, "DPP: Invalid state after relock in config handling - aborting");
        return ESP_ERR_INVALID_STATE;
    }

    if (ret != ESP_OK) {
        wpa_printf(MSG_ERROR, "DPP: Failed to post DPP_CFG_RECVD event, error 0x%x", ret);
        return ret;
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

    /* Send GAS Query Req. Reset retry counter: GAS Config is a new phase
     * with its own budget across retransmits. */
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
    struct dpp_authentication *auth = s_dpp_ctx.dpp_auth;
    uint8_t *buf;
    unsigned int seconds;
    struct os_reltime rnow;
    const uint8_t *connector, *trans_id, *status = NULL;
    uint16_t connector_len, trans_id_len, status_len;
    enum dpp_status_error res = DPP_STATUS_NOT_COMPATIBLE;
    struct dpp_introduction intro;
    os_time_t expiry;
    struct os_time now;
    struct wpa_sm *sm = get_wpa_sm();
    struct rsn_pmksa_cache_entry *entry = NULL;
    int i = 0;

    if (!rx_param) {
        return ESP_ERR_INVALID_ARG;
    }

    if (rx_param->vendor_data_len < 2) {
        /* esp_dpp_rx_frm already guards length; keep ESP_OK so spoofed frames
         * cannot abort the session (same rationale as esp_dpp_rx_frm / GAS). */
        wpa_printf(MSG_DEBUG, "DPP: Too short vendor specific data in peer discovery response");
        return ESP_OK;
    }
    size_t len = rx_param->vendor_data_len - 2;

    buf = rx_param->action_frm->u.public_action.v.pa_vendor_spec.vendor_data;

    if (!auth) {
        wpa_printf(MSG_DEBUG, "DPP: Auth context not found for Peer Discovery response");
        return ESP_ERR_INVALID_STATE;
    }

    if (os_memcmp(auth->peer_mac_addr, rx_param->sa, ETH_ALEN) != 0) {
        wpa_printf(MSG_DEBUG, "DPP: Not expecting Peer Discovery response from " MACSTR, MAC2STR(rx_param->sa));
        return ESP_OK;
    }

    wpa_printf(MSG_DEBUG, "DPP: Peer Discovery from " MACSTR, MAC2STR(rx_param->sa));

    for (i = 0; i < auth->num_conf_obj; i++) {

        if (!auth->conf_obj[i].connector
                || !auth->net_access_key
                || !auth->conf_obj[i].c_sign_key
                || dpp_akm_legacy(auth->conf_obj[i].akm)) {
            wpa_printf(MSG_DEBUG, "DPP: Profile not found for network introduction or akm mismatch");
            continue;
        }

        trans_id = dpp_get_attr(&buf[2], len, DPP_ATTR_TRANSACTION_ID, &trans_id_len);
        if (!trans_id || trans_id_len != 1) {
            wpa_printf(MSG_ERROR, "DPP: Peer did not include Transaction ID");
            return ESP_ERR_DPP_FAILURE;
        }
        if (trans_id[0] != TRANSACTION_ID) {
            wpa_printf(MSG_ERROR, "DPP: Ignore frame with unexpected Transaction ID %u", trans_id[0]);
            return ESP_ERR_DPP_FAILURE;
        }

        status = dpp_get_attr(&buf[2], len, DPP_ATTR_STATUS, &status_len);
        if (!status || status_len != 1) {
            wpa_printf(MSG_ERROR, "DPP: Peer did not include Status");
            return ESP_ERR_DPP_FAILURE;
        }
        if (status[0] != DPP_STATUS_OK) {
            wpa_printf(MSG_ERROR, "DPP: Peer rejected network introduction: Status %u", status[0]);
            return ESP_ERR_DPP_FAILURE;
        }

        connector = dpp_get_attr(&buf[2], len, DPP_ATTR_CONNECTOR, &connector_len);
        if (!connector) {
            wpa_printf(MSG_ERROR, "DPP: Peer did not include its Connector");
            return ESP_ERR_DPP_FAILURE;
        }

        res = dpp_peer_intro(&intro, auth->conf_obj[i].connector,
                             wpabuf_head(auth->net_access_key),
                             wpabuf_len(auth->net_access_key),
                             wpabuf_head(auth->conf_obj[i].c_sign_key),
                             wpabuf_len(auth->conf_obj[i].c_sign_key),
                             connector, connector_len, &expiry);

        if (res == DPP_STATUS_OK) {
            entry = os_zalloc(sizeof(*entry));
            if (!entry) {
                goto fail;
            }
            os_memcpy(entry->aa, rx_param->sa, ETH_ALEN);
            os_memcpy(entry->pmkid, intro.pmkid, PMKID_LEN);
            os_memcpy(entry->pmk, intro.pmk, intro.pmk_len);
            entry->pmk_len = intro.pmk_len;
            entry->akmp = WPA_KEY_MGMT_DPP;

            if (expiry) {
                os_get_time(&now);
                if (expiry > now.sec) {
                    seconds = expiry - now.sec;
                } else {
                    wpa_printf(MSG_WARNING, "DPP: Connector expired during processing");
                    goto fail;
                }
            } else {
                seconds = ESP_DPP_PMK_CACHE_DEFAULT_TIMEOUT;
            }
            os_get_reltime(&rnow);
            entry->expiration = rnow.sec + seconds;
            entry->reauth_time = rnow.sec + seconds;
            entry->network_ctx = NULL;

            wpa_printf(MSG_INFO, "peer=" MACSTR " status=%u", MAC2STR(rx_param->sa), status[0]);
            break;
        }
    }

    if (res != DPP_STATUS_OK) {
        wpa_printf(MSG_ERROR, "DPP: Network Introduction protocol resulted in failure");
        goto fail;
    }

    wpa_printf(MSG_DEBUG,
               "DPP: Try connection after successful network introduction");
    int rc = dpp_connect(rx_param->sa, true);
    if (rc != ESP_OK) {
        goto fail;
    }

    /* Connection initiated! Now we commit the PMK to the global cache. */
    pmksa_cache_add_entry(sm->pmksa, entry);
    entry = NULL;

    /* Final cleanup: connection initiated, auth context and timers no longer needed. */
    dpp_stop_internal();
    forced_memzero(&intro, sizeof(intro));

    return ESP_OK;
fail:
    forced_memzero(&intro, sizeof(intro));
    if (entry != NULL) {
        bin_clear_free(entry, sizeof(*entry));
    }
    return ESP_ERR_DPP_FAILURE;
}

static esp_err_t esp_dpp_rx_frm(struct action_rx_param *rx_param)
{
    uint8_t crypto_suit, type;
    uint8_t *tmp;
    int ret = ESP_OK;

    if (rx_param->vendor_data_len < 2) {
        /* Note: We do not return an error or abort the session on data validation
         * failures to avoid interruption from spoofed frames. */
        wpa_printf(MSG_ERROR, "DPP: Vendor data too short (%u)", (unsigned)rx_param->vendor_data_len);
        return ESP_OK;
    }

    tmp = rx_param->action_frm->u.public_action.v.pa_vendor_spec.vendor_data;
    crypto_suit = tmp[0];
    type = tmp[1];

    if (crypto_suit != 1) {
        wpa_printf(MSG_ERROR, "DPP: Unsupported crypto suit");
        return ESP_OK;
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
    }

    return ret;
}

static esp_err_t gas_query_resp_rx(struct action_rx_param *rx_param)
{
    struct dpp_authentication *auth = s_dpp_ctx.dpp_auth;
    uint8_t *pos = rx_param->action_frm->u.public_action.v.pa_gas_resp.data;
    uint8_t *resp = &pos[10];  /* first byte of DPP attributes */
    size_t vendor_len = rx_param->vendor_data_len;
    esp_err_t ret = ESP_OK;

    if (!auth) {
        return ESP_OK;
    }

    /* Verify source MAC: Management frames like GAS can be easily spoofed. */
    if (os_memcmp(rx_param->sa, auth->peer_mac_addr, ETH_ALEN) != 0) {
        wpa_printf(MSG_DEBUG, "DPP: MAC mismatch - ignore potentially spoofed frame");
        return ESP_OK;
    }

    if (vendor_len < 2) {
        /* Note: We do not return an error or abort the session on data validation
         * failures (like length or structural checks) to avoid interruption from
         * spoofed frames. The session will timeout naturally if a legitimate
         * response is not received. */
        wpa_printf(MSG_DEBUG, "DPP: GAS response vendor data too short (%u)",
                   (unsigned)vendor_len);
        return ESP_OK;
    }

    /* Basic structural checks on the Advertisement Protocol payload */
    if (!(pos[1] == WLAN_EID_VENDOR_SPECIFIC && pos[2] == 5 &&
            WPA_GET_BE24(&pos[3]) == OUI_WFA && pos[6] == 0x1a && pos[7] == 1)) {
        wpa_hexdump(MSG_INFO, "DPP: Failed, Configuration Response adv_proto", pos, 8);
        return ESP_OK;
    }

    /* DPP attribute length = vendor_data_len - 2, now safe after the >= 2 check above */
    size_t dpp_data_len = vendor_len - 2;

    if (dpp_conf_resp_rx(auth, resp, dpp_data_len) < 0) {
        wpa_printf(MSG_INFO, "DPP: Configuration attempt failed");
        return ESP_OK;
    }

    eloop_cancel_timeout(gas_query_timeout, NULL, auth);

    /* At the moment the architecture only supports one DPP configuration.
     * We parse and handle the first configuration object only. */
    if (auth->num_conf_obj > 0) {
        ret = esp_dpp_handle_config_obj(auth, &auth->conf_obj[0]);
        if (ret != ESP_OK) {
            wpa_printf(MSG_INFO, "DPP: Configuration handling failed");
            goto fail;
        }

        /* If legacy AKM, we are done with DPP. Stop everything. */
        if (dpp_akm_legacy(auth->conf_obj[0].akm)) {
            dpp_stop_internal();
            return ESP_OK;
        }
    }

    return ESP_OK;

fail:
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
    } else if (public_action->action == WLAN_PA_GAS_INITIAL_RESP &&
               rx_param->frm_len >= 9 &&
               public_action->v.pa_gas_resp.type == WLAN_EID_ADV_PROTO &&
               public_action->v.pa_gas_resp.length == 8 &&
               public_action->v.pa_gas_resp.status_code == 0) {

        if (!s_dpp_ctx.dpp_auth ||
                s_dpp_ctx.dpp_auth->gas_dialog_token < 0 ||
                public_action->v.pa_gas_resp.diag_token !=
                s_dpp_ctx.dpp_auth->gas_dialog_token) {
            wpa_printf(MSG_DEBUG,
                       "DPP: GAS dialog token mismatch (rx=%u exp=%d) - drop",
                       public_action->v.pa_gas_resp.diag_token,
                       s_dpp_ctx.dpp_auth ?
                       s_dpp_ctx.dpp_auth->gas_dialog_token : -1);
            os_free(rx_param);
            return;
        }

        size_t offset = (size_t)(public_action->v.pa_gas_resp.data +
                                 public_action->v.pa_gas_resp.length -
                                 (u8 *)rx_param->action_frm);

        if (rx_param->frm_len < offset) {
            wpa_printf(MSG_DEBUG, "DPP: Ignored too short GAS Initial Response");
            goto fail_unlock;
        }

        rx_param->vendor_data_len = rx_param->frm_len - offset;

        wpa_printf(MSG_DEBUG, "DPP: Gas response received");
        ret = gas_query_resp_rx(rx_param);
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
        dpp_abort_failure_locked(ret);
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
        wpa_printf(MSG_INFO, "DPP: No Peer Discovery response received - stopping exchange");
        dpp_abort_failure_locked(ESP_ERR_DPP_TX_FAILURE);
        return;
    }

    wpa_printf(MSG_INFO, "DPP: Retransmitting Peer Discovery Request frame");
    if (esp_dpp_start_net_intro_protocol_internal(auth->peer_mac_addr) != ESP_OK) {
        dpp_abort_failure_locked(ESP_ERR_DPP_TX_FAILURE);
        return;
    }
    /* Safety net: if TX status is never delivered, this timeout ensures we don't
     * hang forever. tx_status_eloop_handler will cancel and re-register with the
     * correct deadline when the TX status arrives.
     * Note: Reuse gas_query_timeout for Peer Discovery phase to maintain
     * compatibility with older applications that expect ESP_ERR_DPP_CONF_TIMEOUT
     * on all discovery/config failures. */
    if (eloop_register_timeout(ESP_GAS_TIMEOUT_SECS + 2, 0, gas_query_timeout, NULL, auth) < 0) {
        wpa_printf(MSG_ERROR, "DPP: Failed to register gas_query_timeout for intro");
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
            wpa_printf(MSG_WARNING, "DPP: failed to send Peer Discovery frame");
            eloop_cancel_timeout(gas_query_timeout, NULL, auth);
            if (eloop_register_timeout(1, 0, esp_dpp_peer_disc_retry, NULL, NULL) < 0) {
                wpa_printf(MSG_ERROR, "DPP: Failed to register peer_disc_retry");
                dpp_abort_failure_locked(ESP_ERR_DPP_FAILURE);
                os_free(evt);
                return;
            }
        } else if (evt->status == WIFI_ACTION_TX_DONE) {
            /* Peer discovery sent, wait for response.
             * Note: Reuse gas_query_timeout for Peer Discovery phase to maintain
             * compatibility with older applications that expect ESP_ERR_DPP_CONF_TIMEOUT
             * on all discovery/config failures. */
            eloop_cancel_timeout(gas_query_timeout, NULL, auth);
            if (eloop_register_timeout(ESP_GAS_TIMEOUT_SECS, 0, gas_query_timeout, NULL, auth) < 0) {
                wpa_printf(MSG_ERROR, "DPP: Failed to register gas_query_timeout after disc TX");
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
                dpp_stop_internal();
                atomic_store(&roc_in_progress, false);
                if (eg) {
                    os_event_group_set_bits(eg, DPP_ROC_EVENT_HANDLED);
                }
                os_free(evt);
                dpp_api_unlock();
                dpp_post_dpp_failed_event(ESP_ERR_NO_MEM);
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
    struct dpp_bootstrap_params_t *params = &s_dpp_ctx.bootstrap_params;
    char *uri_chan_list = esp_dpp_parse_chan_list(chan_list);

    wpa_printf(MSG_DEBUG, "DPP: Starting bootstrap genration");
    if (!uri_chan_list) {
        wpa_printf(MSG_ERROR, "Invalid Channel list - %s", chan_list ? chan_list : "(null)");
        s_dpp_ctx.bootstrap_done = false;
        ret = ESP_ERR_DPP_INVALID_LIST;
        goto fail;
    }

    if (type != DPP_BOOTSTRAP_QR_CODE) {
        wpa_printf(MSG_INFO, "Bootstrap type %d not supported", type);
        ret = ESP_ERR_NOT_SUPPORTED;
        goto fail;
    }

    params->type = type;
    esp_wifi_get_mac(WIFI_IF_STA, params->mac);

    size_t info_len = info ? os_strlen(info) : 0;

    size_t command_len = os_strlen("type=qrcode mac=") + 17 + os_strlen(uri_chan_list) +
                         (key ? os_strlen("key=") + os_strlen(key) : 0) +
                         (info_len ? os_strlen(" info=") + info_len : 0) + 1;

    char *command = os_zalloc(command_len);
    if (!command) {
        wpa_printf(MSG_ERROR, "DPP: Failed to allocate memory for bootstrap command");
        ret = ESP_ERR_NO_MEM;
        goto fail;
    }

    os_snprintf(command, command_len, "type=qrcode mac=" MACSTR "%s%s%s%s%s",
                MAC2STR(params->mac), uri_chan_list,
                key ? "key=" : "", key ? key : "",
                info_len ? " info=" : "",
                info_len ? info : "");

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
    dpp_listen_next_channel(NULL, NULL);
    dpp_api_unlock();
}

static void dpp_cancel_auth_gas_eloop_timeouts(void)
{
    eloop_cancel_timeout(esp_dpp_auth_resp_retry_timeout, NULL, NULL);
    eloop_cancel_timeout(esp_dpp_auth_conf_wait_timeout, NULL, NULL);
    eloop_cancel_timeout(esp_dpp_auth_resp_retry, NULL, NULL);
    eloop_cancel_timeout(gas_query_timeout, ELOOP_ALL_CTX, ELOOP_ALL_CTX);
    eloop_cancel_timeout(esp_dpp_peer_disc_retry, NULL, NULL);
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
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_ACTION_TX_STATUS,
                                 &tx_status_handler);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_ROC_DONE,
                                 &roc_status_handler);
    if (s_dpp_ctx.dpp_global) {
        dpp_global_deinit(s_dpp_ctx.dpp_global);
        s_dpp_ctx.dpp_global = NULL;
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
    struct dpp_authentication *auth = s_dpp_ctx.dpp_auth;
    struct wpabuf *buf = NULL;
    int ret = ESP_OK;

    if (!auth) {
        return ESP_ERR_INVALID_STATE;
    }

    if (auth->num_conf_obj == 0) {
        return ESP_ERR_INVALID_STATE;
    }

    /* At the moment the architecture only supports one DPP configuration.
     * We attempt network introduction with the first configuration object only. */
    os_memcpy(auth->peer_mac_addr, bssid, ETH_ALEN);
    buf = dpp_build_peer_disc_req(auth, &auth->conf_obj[0]);

    if (!buf) {
        return ESP_ERR_NO_MEM;
    }

    if (esp_dpp_send_action_frame(bssid, wpabuf_head(buf), wpabuf_len(buf),
                                  auth->curr_chan, 1000 + OFFCHAN_TX_WAIT_TIME,
                                  DPP_TX_PEER_DISCOVERY_REQ) != ESP_OK) {
        ret = ESP_FAIL;
    }

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

    s_dpp_ctx.gas_query_tries = 0;

    /* Ensure no stale Auth, GAS, or Discovery timers from a previous attempt
     * are running. If they fire, they would abort the new attempt prematurely. */
    esp_dpp_cancel_timeouts();

    wpa_printf(MSG_INFO, "DPP: Starting Network Introduction to " MACSTR, MAC2STR(bssid));

    ret = esp_dpp_start_net_intro_protocol_internal(bssid);
    if (ret != ESP_OK) {
        if (ret == ESP_ERR_INVALID_STATE) {
            dpp_api_unlock();
        } else {
            dpp_abort_failure_locked(ret);
        }
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
