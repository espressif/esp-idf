/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
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

#ifdef CONFIG_DPP

struct action_rx_param {
    u8 sa[ETH_ALEN];
    u32 channel;
    u32 frm_len;
    u32 vendor_data_len;
    struct ieee80211_action action_frm[];
};

static void *s_dpp_api_lock = NULL;
static void *s_dpp_event_group = NULL;

#define DPP_ROC_EVENT_HANDLED  BIT0

static atomic_bool roc_in_progress;
static struct esp_dpp_context_t s_dpp_ctx;
static int esp_supp_rx_action(uint8_t *hdr, uint8_t *payload, size_t len, uint8_t channel);
static wifi_action_rx_cb_t s_action_rx_cb = esp_supp_rx_action;
static uint8_t s_current_tx_op_id;
static void tx_status_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data);
static void roc_status_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data);
static void dpp_listen_next_channel(void *data, void *user_ctx);
static esp_err_t dpp_api_lock(void)
{
    if (!s_dpp_api_lock) {
        s_dpp_api_lock = os_recursive_mutex_create();
    }
    if (!s_dpp_api_lock) {
        wpa_printf(MSG_ERROR, "DPP: dpp_init: failed to create DPP API lock");
        return ESP_ERR_NO_MEM;
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

static void dpp_event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data)
{
    if (!s_dpp_ctx.dpp_event_cb) {
        return;
    }
    switch (event_id) {
    case WIFI_EVENT_DPP_URI_READY:
        wifi_event_dpp_uri_ready_t *event = (wifi_event_dpp_uri_ready_t *) event_data;
        s_dpp_ctx.dpp_event_cb(ESP_SUPP_DPP_URI_READY, (void *)(event->uri));
        break;
    case WIFI_EVENT_DPP_CFG_RECVD:
        s_dpp_ctx.dpp_event_cb(ESP_SUPP_DPP_CFG_RECVD, (wifi_config_t *)event_data);
        break;
    case WIFI_EVENT_DPP_FAILED:
        s_dpp_ctx.dpp_event_cb(ESP_SUPP_DPP_FAIL, (void *)event_data);
        break;
    default:
        break;
    }
    return;
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

    wpa_printf(MSG_DEBUG, "DPP: Stoping ROC");
    req.ifx = WIFI_IF_STA;
    req.type = WIFI_ROC_CANCEL;
    eloop_cancel_timeout(dpp_listen_next_channel, NULL, NULL);
    s_dpp_ctx.dpp_listen_ongoing = false;

    esp_wifi_remain_on_channel(&req);

    return 0;
}

static void dpp_stop(void)
{
    if (s_dpp_ctx.dpp_auth) {
        dpp_deinit_auth();
        listen_stop_handler(NULL, NULL);
    }
}

static void dpp_abort_with_failure(uint32_t failure_reason)
{
    /* Stop DPP*/
    dpp_stop();

    /* Send event to APP */
    wifi_event_dpp_failed_t event = {0};
    event.failure_reason = failure_reason;
    esp_event_post(WIFI_EVENT, WIFI_EVENT_DPP_FAILED, &event, sizeof(event), OS_BLOCK);
}

static void esp_dpp_auth_conf_wait_timeout(void *eloop_ctx, void *timeout_ctx)
{
    if (!s_dpp_ctx.dpp_auth || !s_dpp_ctx.dpp_auth->waiting_auth_conf) {
        return;
    }

    wpa_printf(MSG_INFO,
               "DPP: Terminate authentication exchange due to Auth Confirm timeout");
    dpp_abort_with_failure(ESP_ERR_DPP_AUTH_TIMEOUT);
}

esp_err_t esp_dpp_send_action_frame(uint8_t *dest_mac, const uint8_t *buf, uint32_t len,
                                    uint8_t channel, uint32_t wait_time_ms)
{
    wifi_action_tx_req_t *req = os_zalloc(sizeof(*req) + len);
    if (!req) {
        return ESP_FAIL;
    }

    req->ifx = WIFI_IF_STA;
    os_memcpy(req->dest_mac, dest_mac, ETH_ALEN);
    req->no_ack = false;
    req->data_len = len;
    req->rx_cb = s_action_rx_cb;
    req->channel = channel;
    req->wait_time_ms = wait_time_ms;
    req->type = WIFI_OFFCHAN_TX_REQ;
    os_memcpy(req->data, buf, req->data_len);

    wpa_printf(MSG_DEBUG, "DPP: Mgmt Tx - MAC:" MACSTR ", Channel-%d, WaitT-%d",
               MAC2STR(dest_mac), channel, wait_time_ms);

    if (ESP_OK != esp_wifi_action_tx_req(req)) {
        wpa_printf(MSG_ERROR, "DPP: Failed to perform offchannel operation");
        dpp_abort_with_failure(ESP_ERR_DPP_TX_FAILURE);
        os_free(req);
        return ESP_FAIL;
    }

    wpa_printf(MSG_DEBUG, "Sent DPP action frame %d", req->op_id);
    s_current_tx_op_id = req->op_id;
    os_free(req);
    return ESP_OK;
}

static void esp_dpp_auth_resp_retry_timeout(void *eloop_ctx, void *timeout_ctx)
{
    struct dpp_authentication *auth = s_dpp_ctx.dpp_auth;

    if (!auth || !auth->resp_msg) {
        return;
    }

    wpa_printf(MSG_DEBUG,
               "DPP: Retry Authentication Response after timeout");
    wpa_msg(wpa_s, MSG_INFO, DPP_EVENT_TX "dst=" MACSTR
            " chan=%u type=%d",
            MAC2STR(auth->peer_mac_addr), auth->curr_chan,
            DPP_PA_AUTHENTICATION_RESP);

    esp_dpp_send_action_frame(s_dpp_ctx.dpp_auth->peer_mac_addr,
                              wpabuf_head(s_dpp_ctx.dpp_auth->resp_msg),
                              wpabuf_len(s_dpp_ctx.dpp_auth->resp_msg),
                              auth->curr_chan, 1000 + OFFCHAN_TX_WAIT_TIME);
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
    os_memcpy(s_dpp_ctx.dpp_auth->peer_mac_addr, rx_param->sa, ETH_ALEN);

    wpa_printf(MSG_INFO, "DPP: Sending authentication response chan(%d)", rx_param->channel);
    eloop_cancel_timeout(esp_dpp_auth_resp_retry_timeout, NULL, NULL);
    eloop_register_timeout(0, 200000, esp_dpp_auth_resp_retry_timeout, NULL, NULL);
    esp_dpp_send_action_frame(rx_param->sa, wpabuf_head(s_dpp_ctx.dpp_auth->resp_msg),
                              wpabuf_len(s_dpp_ctx.dpp_auth->resp_msg),
                              rx_param->channel, 1000 + OFFCHAN_TX_WAIT_TIME);

    return ESP_OK;
}

static void gas_query_timeout(void *eloop_data, void *user_ctx)
{
    struct dpp_authentication *auth = user_ctx;

    if (!auth || !auth->auth_success) {
        wpa_printf(MSG_INFO, "DPP-GAS: Auth %p state not correct", auth);
        return;
    }

    wpa_printf(MSG_DEBUG, "GAS: No response received for GAS query");
    if (auth->conf_req) {
        wpabuf_free(auth->conf_req);
        auth->conf_req = NULL;
    }
    dpp_abort_with_failure(ESP_ERR_DPP_CONF_TIMEOUT);
}

static int gas_query_req_tx(struct dpp_authentication *auth)
{
    struct wpabuf *buf;
    int supp_op_classes[] = {81, 0};
    int ret;

    buf = dpp_build_conf_req_helper(auth, NULL, 0, NULL,
                                    supp_op_classes);
    if (!buf) {
        wpa_printf(MSG_ERROR, "DPP: No configuration request data available");
        dpp_abort_with_failure(ESP_ERR_DPP_FAILURE);
        return ESP_FAIL;
    }

    wpa_printf(MSG_INFO, "DPP: GAS request to " MACSTR " (chan %u)",
               MAC2STR(auth->peer_mac_addr), auth->curr_chan);

    ret = esp_dpp_send_action_frame(auth->peer_mac_addr, wpabuf_head(buf), wpabuf_len(buf),
                                    auth->curr_chan, 1000 + OFFCHAN_TX_WAIT_TIME);
    if (ret != ESP_OK) {
        wpabuf_free(buf);
        return ret;
    }

    auth->conf_req = buf;
    eloop_register_timeout(2, 0, gas_query_timeout, NULL, auth);

    return ret;
}

static int esp_dpp_handle_config_obj(struct dpp_authentication *auth,
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
         * most clients are ready to receive as an event
         * message. */
        wpa_printf(MSG_INFO, DPP_EVENT_CONNECTOR "%s",
                   conf->connector);
    }
    if (atomic_load(&roc_in_progress)) {
        listen_stop_handler(NULL, NULL);
    }
    /* deinit AUTH since authentication is done */
    dpp_deinit_auth();

    wifi_event_dpp_config_received_t event = {0};
    event.wifi_cfg = s_dpp_ctx.wifi_cfg;
    esp_event_post(WIFI_EVENT, WIFI_EVENT_DPP_CFG_RECVD, &event, sizeof(event), OS_BLOCK);

    return 0;
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

    /* Send GAS Query Req */
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
    enum dpp_status_error res = 0;
    struct dpp_introduction intro;
    os_time_t expiry;
    struct os_time now;
    struct wpa_sm *sm = get_wpa_sm();
    struct rsn_pmksa_cache_entry *entry = NULL;
    int i = 0;

    if (!rx_param) {
        return ESP_ERR_INVALID_ARG;
    }

    size_t len = rx_param->vendor_data_len - 2;

    buf = rx_param->action_frm->u.public_action.v.pa_vendor_spec.vendor_data;

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
            return ESP_FAIL;
        }
        if (trans_id[0] != TRANSACTION_ID) {
            wpa_printf(MSG_ERROR, "DPP: Ignore frame with unexpected Transaction ID %u", trans_id[0]);
            return ESP_FAIL;
        }

        status = dpp_get_attr(&buf[2], len, DPP_ATTR_STATUS, &status_len);
        if (!status || status_len != 1) {
            wpa_printf(MSG_ERROR, "DPP: Peer did not include Status");
            return ESP_FAIL;
        }
        if (status[0] != DPP_STATUS_OK) {
            wpa_printf(MSG_ERROR, "DPP: Peer rejected network introduction: Status %u", status[0]);
            return ESP_FAIL;
        }

        connector = dpp_get_attr(&buf[2], len, DPP_ATTR_CONNECTOR, &connector_len);
        if (!connector) {
            wpa_printf(MSG_ERROR, "DPP: Peer did not include its Connector");
            return ESP_FAIL;
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
                seconds = expiry - now.sec;
            } else {
                seconds = ESP_DPP_PMK_CACHE_DEFAULT_TIMEOUT;
            }
            os_get_reltime(&rnow);
            entry->expiration = rnow.sec + seconds;
            entry->reauth_time = rnow.sec + seconds;
            entry->network_ctx = auth;

            pmksa_cache_add_entry(sm->pmksa, entry);

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
    dpp_connect(rx_param->sa, true);
    return ESP_OK;
fail:
    os_memset(&intro, 0, sizeof(intro));
    if (entry != NULL) {
        os_free(entry);
    }
    return ESP_FAIL;
}

static esp_err_t esp_dpp_rx_frm(struct action_rx_param *rx_param)
{
    uint8_t crypto_suit, type;
    uint8_t *tmp;
    int ret = ESP_OK;

    tmp = rx_param->action_frm->u.public_action.v.pa_vendor_spec.vendor_data;
    crypto_suit = tmp[0];
    type = tmp[1];

    if (crypto_suit != 1) {
        wpa_printf(MSG_ERROR, "DPP: Unsupported crypto suit");
        return ESP_FAIL;
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
    uint8_t *resp = &pos[10];
    int i, res;

    if (pos[1] == WLAN_EID_VENDOR_SPECIFIC && pos[2] == 5 &&
            WPA_GET_BE24(&pos[3]) == OUI_WFA && pos[6] == 0x1a && pos[7] == 1 && auth) {

        eloop_cancel_timeout(gas_query_timeout, NULL, auth);
        if (dpp_conf_resp_rx(auth, resp, rx_param->vendor_data_len - 2) < 0) {
            wpa_printf(MSG_INFO, "DPP: Configuration attempt failed");
            goto fail;
        }

        for (i = 0; i < auth->num_conf_obj; i++) {
            res = esp_dpp_handle_config_obj(auth, &auth->conf_obj[i]);
            if (res < 0) {
                wpa_printf(MSG_INFO, "DPP: Configuration parsing failed");
                goto fail;
            }
        }
    }

    return ESP_OK;
fail:
    return ESP_FAIL;
}

static void esp_dpp_rx_action(void *data, void *user_ctx)
{
    struct action_rx_param *rx_param = data;
    struct ieee80211_public_action *public_action;
    int ret = ESP_OK;

    if (!rx_param) {
        return;
    }

    /* we don't cater other action frames except public here */
    if (rx_param->action_frm->category != WLAN_ACTION_PUBLIC) {
        return;
    }

    public_action = &rx_param->action_frm->u.public_action;

    wpa_printf(MSG_DEBUG, "DPP: Rx Public Action frame: action - %d",
               public_action->action);

    if (public_action->action == WLAN_PA_VENDOR_SPECIFIC &&
            WPA_GET_BE24(public_action->v.pa_vendor_spec.oui) == OUI_WFA &&
            public_action->v.pa_vendor_spec.wfa_stype == DPP_OUI_TYPE) {

        rx_param->vendor_data_len = rx_param->frm_len -
                                    (size_t)(public_action->v.pa_vendor_spec.vendor_data -
                                             (u8 *)rx_param->action_frm);

        if (atomic_load(&roc_in_progress)) {
            listen_stop_handler(NULL, NULL);
        }

        ret = esp_dpp_rx_frm(rx_param);
    } else if (public_action->action == WLAN_PA_GAS_INITIAL_RESP &&
               public_action->v.pa_gas_resp.type == WLAN_EID_ADV_PROTO &&
               public_action->v.pa_gas_resp.length == 8 &&
               public_action->v.pa_gas_resp.status_code == 0) {

        rx_param->vendor_data_len = rx_param->frm_len -
                                    (size_t)(public_action->v.pa_gas_resp.data +
                                             public_action->v.pa_gas_resp.length -
                                             (u8 *)rx_param->action_frm);

        wpa_printf(MSG_DEBUG, "DPP: Gas response received");
        ret = gas_query_resp_rx(rx_param);
    }

    os_free(rx_param);

    if (ret != ESP_OK) {
        dpp_abort_with_failure(ESP_ERR_DPP_FAILURE);
    }
}

static void dpp_listen_next_channel(void *data, void *user_ctx)
{
    struct dpp_bootstrap_params_t *p = &s_dpp_ctx.bootstrap_params;
    static int counter;
    int channel;
    esp_err_t ret = 0;
    wifi_roc_req_t req = {0};

    if (!s_dpp_ctx.dpp_listen_ongoing) {
        return;
    }
    if (p->num_chan <= 0) {
        wpa_printf(MSG_ERROR, "Listen channel not set");
        return;
    }

    channel = p->chan_list[counter++ % p->num_chan];

    wpa_printf(MSG_DEBUG, "DPP: Starting ROC on channel %d", channel);

    req.ifx = WIFI_IF_STA;
    req.type = WIFI_ROC_REQ;
    req.channel = channel;
    req.wait_time_ms = BOOTSTRAP_ROC_WAIT_TIME;
    req.rx_cb = s_action_rx_cb;
    ret = esp_wifi_remain_on_channel(&req);
    if (ret != ESP_OK) {
        wpa_printf(MSG_ERROR, "Failed ROC. error : 0x%x", ret);
        return;
    }
    os_event_group_clear_bits(s_dpp_event_group, DPP_ROC_EVENT_HANDLED);
}

static void esp_dpp_bootstrap_gen(void *data, void *user_ctx)
{
    char *command = data;
    const char *uri;
    uint32_t len;

    s_dpp_ctx.id = dpp_bootstrap_gen(s_dpp_ctx.dpp_global, command);

    if (s_dpp_ctx.id < 0) {
        struct dpp_bootstrap_params_t *params = &s_dpp_ctx.bootstrap_params;
        if (params->info) {
            os_free(params->info);
            params->info = NULL;
        }
    }
    uri = dpp_bootstrap_get_uri(s_dpp_ctx.dpp_global, s_dpp_ctx.id);

    wifi_event_dpp_uri_ready_t *event;
    len = sizeof(*event) + os_strlen(uri) + 1;
    event = os_malloc(len);
    if (!event) {
        return;
    }
    event->uri_data_len = os_strlen(uri);
    os_memcpy(event->uri, uri, event->uri_data_len);
    event->uri[event->uri_data_len++] = '\0';
    esp_event_post(WIFI_EVENT, WIFI_EVENT_DPP_URI_READY, event, len, OS_BLOCK);
    os_free(event);
    os_free(command);
    dpp_api_lock();
    s_dpp_ctx.bootstrap_done = true;
    dpp_api_unlock();
}

static int esp_dpp_deinit(void *data, void *user_ctx)
{
    struct dpp_bootstrap_params_t *params = &s_dpp_ctx.bootstrap_params;

    wpa_printf(MSG_DEBUG, "DPP: Deinitializing DPP");
    /* Cancel all registered timeouts */
    eloop_cancel_timeout(esp_dpp_auth_conf_wait_timeout, NULL, NULL);

    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_ACTION_TX_STATUS,
                                 &tx_status_handler);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_ROC_DONE,
                                 &roc_status_handler);

    if (s_dpp_ctx.dpp_event_cb) {
        esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_DPP_URI_READY,
                                     &dpp_event_handler);
        esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_DPP_CFG_RECVD,
                                     &dpp_event_handler);
        esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_DPP_FAILED,
                                     &dpp_event_handler);
    }
    if (params->info) {
        os_free(params->info);
        params->info = NULL;
    }

    if (s_dpp_ctx.dpp_global) {
        dpp_global_deinit(s_dpp_ctx.dpp_global);
        s_dpp_ctx.dpp_global = NULL;
    }
    if (s_dpp_ctx.dpp_auth) {
        dpp_auth_deinit(s_dpp_ctx.dpp_auth);
        s_dpp_ctx.dpp_auth = NULL;
    }
    s_dpp_ctx.dpp_init_done = false;
    s_dpp_ctx.bootstrap_done = false;
    s_dpp_ctx.dpp_event_cb = NULL;
    if (s_dpp_event_group) {
        os_event_group_delete(s_dpp_event_group);
        s_dpp_event_group = NULL;
    }

    return 0;
}

static int esp_supp_rx_action(uint8_t *hdr, uint8_t *payload, size_t len, uint8_t channel)
{
    struct ieee80211_hdr *rx_hdr = (struct ieee80211_hdr *)hdr;
    struct action_rx_param *rx_param;
    int ret = ESP_ERR_NOT_SUPPORTED;
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

        eloop_register_timeout(0, 0, esp_dpp_rx_action, rx_param, NULL);
    }

    return ret;
}

static void esp_dpp_auth_resp_retry(void *eloop_ctx, void *timeout_ctx)
{
    struct dpp_authentication *auth = s_dpp_ctx.dpp_auth;
    unsigned int max_tries = 5;

    if (!auth || !auth->resp_msg) {
        return;
    }

    auth->auth_resp_tries++;
    if (auth->auth_resp_tries >= max_tries) {
        wpa_printf(MSG_INFO, "DPP: No confirm received from initiator - stopping exchange");
        dpp_abort_with_failure(ESP_ERR_DPP_TX_FAILURE);
        return;
    }

    wpa_printf(MSG_INFO,
               "DPP: Retransmiting Authentication Response frame");
    eloop_cancel_timeout(esp_dpp_auth_resp_retry_timeout, NULL, NULL);
    esp_dpp_auth_resp_retry_timeout(NULL, NULL);
}

static void tx_status_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data)
{
    struct dpp_authentication *auth = s_dpp_ctx.dpp_auth;

    wifi_event_action_tx_status_t *evt = event_data;

    wpa_printf(MSG_DEBUG, "Mgmt Tx Status - %d, Cookie - 0x%x",
               evt->status, (uint32_t)evt->context);

    if (evt->op_id != s_current_tx_op_id) {
        wpa_printf(MSG_DEBUG, "DPP: status not for recent frame op_id=%u, s_current_tx_op_id=%u",
                   evt->op_id, s_current_tx_op_id);
        return;
    }
    if (!auth) {
        wpa_printf(MSG_DEBUG, "Auth already deinitialized, return");
        return;
    }
    if (auth->waiting_auth_conf) {
        eloop_cancel_timeout(esp_dpp_auth_resp_retry_timeout, NULL, NULL);
        if (evt->status == WIFI_ACTION_TX_FAILED) {
            /* failed to send auth response frame */
            eloop_cancel_timeout(esp_dpp_auth_conf_wait_timeout, NULL, NULL);
            eloop_register_timeout(1, 0, esp_dpp_auth_resp_retry, NULL, NULL);
        } else if (evt->status == WIFI_ACTION_TX_DONE) {
            eloop_cancel_timeout(esp_dpp_auth_conf_wait_timeout, NULL, NULL);
            eloop_register_timeout(ESP_DPP_AUTH_TIMEOUT_SECS, 0, esp_dpp_auth_conf_wait_timeout, NULL, NULL);
        }
    } else if (auth->auth_success) {
        if (evt->status == WIFI_ACTION_TX_FAILED) {
            /* failed to send gas query frame, retry logic needed? */
            wpa_printf(MSG_WARNING, "DPP: failed to send GAS query frame");
            dpp_abort_with_failure(ESP_ERR_DPP_TX_FAILURE);
        } else if (evt->status == WIFI_ACTION_TX_DONE) {
            eloop_cancel_timeout(gas_query_timeout, NULL, auth);
            eloop_register_timeout(ESP_GAS_TIMEOUT_SECS, 0, gas_query_timeout, NULL, auth);
        }
    }
    atomic_store(&roc_in_progress, true);
}

static void roc_status_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    wifi_event_roc_done_t *evt = (wifi_event_roc_done_t *)event_data;

    if (evt->context == (uint32_t)s_action_rx_cb) {
        eloop_cancel_timeout(dpp_listen_next_channel, NULL, NULL);
        eloop_register_timeout(0, 0, dpp_listen_next_channel, NULL, NULL);
    }

    atomic_store(&roc_in_progress, false);
    os_event_group_set_bits(s_dpp_event_group, DPP_ROC_EVENT_HANDLED);
}

static char *esp_dpp_parse_chan_list(const char *chan_list)
{
    struct dpp_bootstrap_params_t *params = &s_dpp_ctx.bootstrap_params;
    size_t max_uri_len = ESP_DPP_MAX_CHAN_COUNT * 8 + strlen(" chan=") + 1;

    char *uri_channels = os_zalloc(max_uri_len);
    if (!uri_channels) {
        wpa_printf(MSG_WARNING, "DPP: URI allocation failed");
        return NULL;
    }

    char *uri_ptr = uri_channels;
    params->num_chan = 0;

    /* Append " chan=" at the beginning of the URI */
    strcpy(uri_ptr, " chan=");
    uri_ptr += strlen(" chan=");

    while (*chan_list && params->num_chan < ESP_DPP_MAX_CHAN_COUNT) {
        int channel = 0;

        /* Parse the channel number */
        while (*chan_list >= '0' && *chan_list <= '9') {
            channel = channel * 10 + (*chan_list - '0');
            chan_list++;
        }

        /* Validate the channel number */
        if (CHANNEL_TO_BIT_NUMBER(channel) == 0) {
            wpa_printf(MSG_WARNING, "DPP: Skipping invalid channel %d", channel);
            /* Skip to the next valid entry */
            while (*chan_list == ',' || *chan_list == ' ') {
                chan_list++;
            }
            continue;  // Skip the bad channel and move to the next one
        }

        /* Get the operating class for the channel */
        u8 oper_class = get_operating_class(channel, 0);
        if (oper_class == 0) {
            wpa_printf(MSG_WARNING, "DPP: Skipping channel %d due to missing operating class", channel);
            /* Skip to the next valid entry */
            while (*chan_list == ',' || *chan_list == ' ') {
                chan_list++;
            }
            continue;  /* Skip to the next channel if no operating class found */
        }

        /* Add the valid channel to the list */
        params->chan_list[params->num_chan++] = channel;

        /* Check if there's space left in uri_channels buffer */
        size_t remaining_space = max_uri_len - (uri_ptr - uri_channels);
        if (remaining_space <= 8) {  // Oper class + "/" + channel + "," + null terminator
            wpa_printf(MSG_ERROR, "DPP: Not enough space in URI buffer");
            os_free(uri_channels);
            return NULL;
        }

        /* Append the operating class and channel to the URI */
        uri_ptr += sprintf(uri_ptr, "%d/%d,", oper_class, channel);

        /* Skip any delimiters (comma or space) */
        while (*chan_list == ',' || *chan_list == ' ') {
            chan_list++;
        }
    }

    if (!params->num_chan) {
        wpa_printf(MSG_ERROR, "DPP: No valid channel in the list");
        os_free(uri_channels);
        return NULL;
    }

    /* Replace the last comma with a space if there was content added */
    if (uri_ptr > uri_channels && *(uri_ptr - 1) == ',') {
        *(uri_ptr - 1) = ' ';
    }

    return uri_channels;
}

esp_err_t
esp_supp_dpp_bootstrap_gen(const char *chan_list, enum dpp_bootstrap_type type,
                           const char *key, const char *uri_info)
{
    int ret = dpp_api_lock();
    if (ret != ESP_OK) {
        return ret;
    }
    if (!s_dpp_ctx.dpp_init_done) {
        dpp_api_unlock();
        wpa_printf(MSG_ERROR, "DPP: failed to bootstrap as dpp not initialized.");
        return ESP_FAIL;
    }
    dpp_api_unlock();

    struct dpp_bootstrap_params_t *params = &s_dpp_ctx.bootstrap_params;
    char *uri_chan_list = esp_dpp_parse_chan_list(chan_list);

    wpa_printf(MSG_DEBUG, "DPP: Starting bootstrap genration");
    if (params->num_chan > ESP_DPP_MAX_CHAN_COUNT) {
        os_free(uri_chan_list);
        wpa_printf(MSG_ERROR, "DPP: Invalid channel list");
        return ESP_ERR_DPP_INVALID_LIST;
    }

    char *command = os_zalloc(1200);

    if (!uri_chan_list || !command || params->num_chan > ESP_DPP_MAX_CHAN_COUNT || params->num_chan == 0) {
        wpa_printf(MSG_ERROR, "Invalid Channel list - %s", chan_list);
        if (command) {
            os_free(command);
        }
        ret = ESP_ERR_DPP_FAILURE;
        goto fail;
    }

    if (type != DPP_BOOTSTRAP_QR_CODE) {
        wpa_printf(MSG_INFO, "Bootstrap type %d not supported", type);
        os_free(command);
        ret = ESP_ERR_NOT_SUPPORTED;
        goto fail;
    }
    params->type = type;
    esp_wifi_get_mac(WIFI_IF_STA, params->mac);

    if (uri_info) {
        params->info_len = strlen(uri_info);
        if (params->info_len) {
            params->info = os_zalloc(params->info_len + 1);
            if (!params->info) {
                os_free(command);
                ret = ESP_ERR_NO_MEM;
                goto fail;
            }
            os_memcpy(params->info, uri_info, params->info_len);
        }
    }

    os_snprintf(command, 1200, "type=qrcode mac=" MACSTR "%s%s%s%s%s",
                MAC2STR(params->mac), uri_chan_list,
                key ? "key=" : "", key ? key : "",
                params->info_len ? " info=" : "",
                params->info_len ? params->info : "");

    eloop_register_timeout(0, 0, esp_dpp_bootstrap_gen, command, NULL);

    ret = ESP_OK;
fail:
    if (uri_chan_list) {
        os_free(uri_chan_list);
    }

    return ret;
}

static void dpp_listen_start(void *ctx, void *data)
{
    s_dpp_ctx.dpp_listen_ongoing = true;
    dpp_listen_next_channel(NULL, NULL);
}

esp_err_t esp_supp_dpp_start_listen(void)
{
    int ret = dpp_api_lock();
    if (ret != ESP_OK) {
        return ret;
    }
    if (!s_dpp_ctx.dpp_init_done || !s_dpp_ctx.bootstrap_done) {
        wpa_printf(MSG_ERROR, "DPP: failed to start listen as dpp not initialized or bootstrapped.");
        dpp_api_unlock();
        return ESP_FAIL;
    }
    dpp_api_unlock();

    if (esp_wifi_get_user_init_flag_internal() == 0) {
        wpa_printf(MSG_ERROR, "DPP: ROC not possible before wifi is started");
        return ESP_ERR_INVALID_STATE;
    }

    /* cancel previous ROC if ongoing */
    esp_supp_dpp_stop_listen();

    /* Give ample time to set the bit, timeout is necessary when ROC is not running previously */
    os_event_group_wait_bits(s_dpp_event_group, DPP_ROC_EVENT_HANDLED, 0, 0, os_task_ms_to_tick(100));
    wpa_printf(MSG_DEBUG, "DPP: Starting ROC");
    eloop_register_timeout(0, 0, dpp_listen_start, NULL, NULL);
    return 0;
}

esp_err_t esp_supp_dpp_stop_listen(void)
{
    int ret = eloop_register_timeout_blocking(listen_stop_handler, NULL, NULL);

    if (ret) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

/* this is called from public API, need protection */
bool is_dpp_enabled(void)
{
    int ret;
    ret = dpp_api_lock();
    if (ret != ESP_OK) {
        return false;
    }
    ret = s_dpp_ctx.dpp_init_done;
    dpp_api_unlock();

    return ret;
}

/* Caller supposed to have dpp_api_lock() before calling this in blocking context */
static int esp_dpp_init(void *eloop_data, void *user_ctx)
{
    struct dpp_global_config cfg = {0};
    int ret;
    esp_supp_dpp_event_cb_t cb = user_ctx;

    cfg.cb_ctx = &s_dpp_ctx;
    cfg.msg_ctx = &s_dpp_ctx;
    os_bzero(&s_dpp_ctx, sizeof(s_dpp_ctx));

    s_dpp_ctx.dpp_global = dpp_global_init(&cfg);
    if (!s_dpp_ctx.dpp_global) {
        wpa_printf(MSG_ERROR, "DPP: failed to allocate memory for dpp_global");
        ret = ESP_ERR_NO_MEM;
        goto init_fail;
    }

    s_dpp_ctx.dpp_event_cb = cb;

    if (cb) {
        esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_DPP_URI_READY,
                                   &dpp_event_handler, NULL);
        esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_DPP_CFG_RECVD,
                                   &dpp_event_handler, NULL);
        esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_DPP_FAILED,
                                   &dpp_event_handler, NULL);
    }
    esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_ACTION_TX_STATUS,
                               &tx_status_handler, NULL);
    esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_ROC_DONE,
                               &roc_status_handler, NULL);

    s_dpp_event_group = os_event_group_create();
    wpa_printf(MSG_INFO, "DPP: dpp init done");
    s_dpp_ctx.dpp_init_done = true;

    return ESP_OK;
init_fail:
    if (s_dpp_ctx.dpp_global) {
        dpp_global_deinit(s_dpp_ctx.dpp_global);
        s_dpp_ctx.dpp_global = NULL;
    }
    return ret;

}

esp_err_t esp_supp_dpp_init(esp_supp_dpp_event_cb_t cb)
{
    esp_err_t ret = ESP_OK;

    wifi_mode_t mode = 0;
    if (esp_wifi_get_mode(&mode) || ((mode != WIFI_MODE_STA) && (mode != WIFI_MODE_APSTA))) {
        wpa_printf(MSG_ERROR, "DPP: failed to init as not in station mode.");
        return ESP_FAIL;
    }

    if (is_wps_enabled()) {
        wpa_printf(MSG_ERROR, "DPP: failed to init since WPS is enabled");
        return ESP_FAIL;
    }

    ret = dpp_api_lock();
    if (ret != ESP_OK) {
        return ret;
    }
    if (s_dpp_ctx.dpp_init_done) {
        wpa_printf(MSG_ERROR, "DPP: failed to init as init already done. Please deinit first and retry.");
        dpp_api_unlock();
        return ESP_FAIL;
    }
    ret = eloop_register_timeout_blocking(esp_dpp_init, NULL, cb);
    dpp_api_unlock();
    return ret;
}

esp_err_t esp_dpp_start_net_intro_protocol(uint8_t *bssid)
{
    struct dpp_authentication *auth = s_dpp_ctx.dpp_auth;
    struct wpabuf *buf = NULL;
    int ret = ESP_OK;

    for (int i = 0; i < auth->num_conf_obj; i++) {
        os_memcpy(auth->peer_mac_addr, bssid, ETH_ALEN);
        buf = dpp_build_peer_disc_req(auth, &auth->conf_obj[i]);

        if (!buf) {
            return ESP_ERR_NO_MEM;
        }

        if (esp_dpp_send_action_frame(bssid, wpabuf_head(buf), wpabuf_len(buf),
                                      auth->curr_chan, OFFCHAN_TX_WAIT_TIME) != ESP_OK) {
            ret = ESP_FAIL;
        }

        wpabuf_free(buf);
    }

    return ret;
}

esp_err_t esp_supp_dpp_deinit(void)
{
    int ret = dpp_api_lock();
    if (ret != ESP_OK) {
        return ret;
    }
    if (!s_dpp_ctx.dpp_init_done) {
        wpa_printf(MSG_DEBUG, "DPP: Already disabled");
        dpp_api_unlock();
        return ESP_OK;
    }
    dpp_api_unlock();
    eloop_register_timeout_blocking(esp_dpp_deinit, NULL, NULL);
    return ESP_OK;
}
#endif
