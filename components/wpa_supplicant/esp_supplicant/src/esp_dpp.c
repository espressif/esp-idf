/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
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
#include "esp_wps_i.h"

#ifdef CONFIG_DPP
static void *s_dpp_task_hdl = NULL;
static void *s_dpp_evt_queue = NULL;
static void *s_dpp_api_lock = NULL;

static bool s_dpp_listen_in_progress;
static struct esp_dpp_context_t s_dpp_ctx;
static wifi_action_rx_cb_t s_action_rx_cb = esp_supp_rx_action;

#define DPP_API_LOCK() os_mutex_lock(s_dpp_api_lock)
#define DPP_API_UNLOCK() os_mutex_unlock(s_dpp_api_lock)

struct action_rx_param {
    u8 sa[ETH_ALEN];
    u32 channel;
    u32 frm_len;
    u32 vendor_data_len;
    struct ieee80211_action *action_frm;
};


static int esp_dpp_post_evt(uint32_t evt_id, uint32_t data)
{
    dpp_event_t *evt = os_zalloc(sizeof(dpp_event_t));
    int ret = ESP_OK;

    if (evt == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto end;
    }
    evt->id = evt_id;
    evt->data = data;
    if (s_dpp_api_lock) {
        DPP_API_LOCK();
    } else {
        ret = ESP_ERR_DPP_FAILURE;
        goto end;
    }
    if (os_queue_send(s_dpp_evt_queue, &evt, os_task_ms_to_tick(10)) != TRUE) {
        DPP_API_UNLOCK();
        ret = ESP_ERR_DPP_FAILURE;
        goto end;
    }
    if (evt_id != SIG_DPP_DEL_TASK) {
        DPP_API_UNLOCK();
    }

    return ret;
end:
    if (evt) {
        os_free(evt);
    }
    return ret;
}

static void esp_dpp_call_cb(esp_supp_dpp_event_t evt, void *data)
{
    if ( evt == ESP_SUPP_DPP_FAIL && s_dpp_ctx.dpp_auth) {
        dpp_auth_deinit(s_dpp_ctx.dpp_auth);
        s_dpp_ctx.dpp_auth = NULL;
    }
    s_dpp_ctx.dpp_event_cb(evt, data);
}

static void esp_dpp_auth_conf_wait_timeout(void *eloop_ctx, void *timeout_ctx)
{
    if (!s_dpp_ctx.dpp_auth || !s_dpp_ctx.dpp_auth->waiting_auth_conf)
        return;

    wpa_printf(MSG_DEBUG,
           "DPP: Terminate authentication exchange due to Auth Confirm timeout");
    if (s_dpp_ctx.dpp_auth) {
        dpp_auth_deinit(s_dpp_ctx.dpp_auth);
        s_dpp_ctx.dpp_auth = NULL;
    }
    esp_dpp_call_cb(ESP_SUPP_DPP_FAIL, (void *)ESP_ERR_DPP_AUTH_TIMEOUT);
}

void esp_send_action_frame(uint8_t *dest_mac, const uint8_t *buf, uint32_t len,
                           uint8_t channel, uint32_t wait_time_ms)
{
    wifi_action_tx_req_t *req = os_zalloc(sizeof(*req) + len);;
    if (!req) {
        return;
    }

    req->ifx = WIFI_IF_STA;
    memcpy(req->dest_mac, dest_mac, ETH_ALEN);
    req->no_ack = false;
    req->data_len = len;
    req->rx_cb = s_action_rx_cb;
    memcpy(req->data, buf, req->data_len);

    wpa_printf(MSG_DEBUG, "DPP: Mgmt Tx - MAC:" MACSTR ", Channel-%d, WaitT-%d",
               MAC2STR(dest_mac), channel, wait_time_ms);

    if (ESP_OK != esp_wifi_action_tx_req(WIFI_OFFCHAN_TX_REQ, channel,
                                         wait_time_ms, req)) {
        wpa_printf(MSG_ERROR, "DPP: Failed to perfrm offchannel operation");
        esp_dpp_call_cb(ESP_SUPP_DPP_FAIL, (void *)ESP_ERR_DPP_TX_FAILURE);
        os_free(req);
        return;
    }

    os_free(req);
}

static void esp_dpp_rx_auth_req(struct action_rx_param *rx_param, uint8_t *dpp_data)
{
    size_t len = rx_param->vendor_data_len - 2;
    const u8 *r_bootstrap, *i_bootstrap;
    u16 r_bootstrap_len, i_bootstrap_len;
    struct dpp_bootstrap_info *own_bi;
    int rc;

    wpa_printf(MSG_INFO, "DPP: Authentication Request from " MACSTR, MAC2STR(rx_param->sa));

    r_bootstrap = dpp_get_attr(dpp_data, len, DPP_ATTR_R_BOOTSTRAP_KEY_HASH,
                               &r_bootstrap_len);
    if (!r_bootstrap || r_bootstrap_len != SHA256_MAC_LEN) {
        wpa_printf(MSG_INFO, "DPP: Missing or invalid Responder Bootstrapping Key Hash attribute");
        rc = ESP_ERR_DPP_INVALID_ATTR;
        goto fail;
    }
    wpa_hexdump(MSG_MSGDUMP, "DPP: Responder Bootstrapping Key Hash", r_bootstrap, r_bootstrap_len);

    i_bootstrap = dpp_get_attr(dpp_data, len, DPP_ATTR_I_BOOTSTRAP_KEY_HASH,
                               &i_bootstrap_len);
    if (!i_bootstrap || i_bootstrap_len != SHA256_MAC_LEN) {
        wpa_printf(MSG_INFO, "DPP: Missing or invalid Initiator Bootstrapping Key Hash attribute");
        rc = ESP_ERR_DPP_INVALID_ATTR;
        goto fail;
    }
    wpa_hexdump(MSG_MSGDUMP, "DPP: Initiator Bootstrapping Key Hash", i_bootstrap, i_bootstrap_len);

    own_bi = dpp_bootstrap_get_id(s_dpp_ctx.dpp_global, s_dpp_ctx.id);
    /* Try to find own and peer bootstrapping key matches based on the
     * received hash values */
    if (os_memcmp(own_bi->pubkey_hash, r_bootstrap, SHA256_MAC_LEN)) {
        wpa_printf(MSG_INFO, "DPP: No matching own bootstrapping key found as responder - ignore message");
        rc = ESP_ERR_DPP_INVALID_ATTR;
        goto fail;
    }
    if (s_dpp_ctx.dpp_auth) {
        wpa_printf(MSG_DEBUG, "DPP: Already in DPP authentication exchange - ignore new one");
        return;
    }
    s_dpp_ctx.dpp_auth = dpp_auth_req_rx(NULL, DPP_CAPAB_ENROLLEE, 0, NULL,
                                         own_bi, rx_param->channel,
                                         (const u8 *)&rx_param->action_frm->u.public_action.v, dpp_data, len);
    os_memcpy(s_dpp_ctx.dpp_auth->peer_mac_addr, rx_param->sa, ETH_ALEN);
    esp_send_action_frame(rx_param->sa, wpabuf_head(s_dpp_ctx.dpp_auth->resp_msg),
                          wpabuf_len(s_dpp_ctx.dpp_auth->resp_msg),
                          rx_param->channel, OFFCHAN_TX_WAIT_TIME);
    eloop_cancel_timeout(esp_dpp_auth_conf_wait_timeout, NULL,NULL);
    eloop_register_timeout(ESP_DPP_AUTH_TIMEOUT_SECS, 0, esp_dpp_auth_conf_wait_timeout,NULL, NULL);

    return;
fail:
    esp_dpp_call_cb(ESP_SUPP_DPP_FAIL, (void *)rc);
}

static void gas_query_req_tx(struct dpp_authentication *auth)
{
    struct wpabuf *buf;
    int supp_op_classes[] = {81, 0};

    buf = dpp_build_conf_req_helper(auth, NULL, 0, NULL,
                                    supp_op_classes);
    if (!buf) {
        wpa_printf(MSG_DEBUG, "DPP: No configuration request data available");
        esp_dpp_call_cb(ESP_SUPP_DPP_FAIL, (void *)ESP_ERR_DPP_FAILURE);
        return;
    }

    wpa_printf(MSG_DEBUG, "DPP: GAS request to " MACSTR " (chan %u)",
               MAC2STR(auth->peer_mac_addr), auth->curr_chan);

    esp_send_action_frame(auth->peer_mac_addr, wpabuf_head(buf), wpabuf_len(buf),
                          auth->curr_chan, OFFCHAN_TX_WAIT_TIME);
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
    s_dpp_listen_in_progress = true;
    esp_wifi_action_tx_req(WIFI_OFFCHAN_TX_CANCEL, 0, 0, NULL);
    esp_dpp_call_cb(ESP_SUPP_DPP_CFG_RECVD, wifi_cfg);

    return 0;
}

static void esp_dpp_rx_auth_conf(struct action_rx_param *rx_param, uint8_t *dpp_data)
{
    struct dpp_authentication *auth = s_dpp_ctx.dpp_auth;
    struct ieee80211_public_action *public_action =
            &rx_param->action_frm->u.public_action;
    size_t len = rx_param->vendor_data_len - 2;
    int rc;

    wpa_printf(MSG_DEBUG, "DPP: Authentication Confirmation from " MACSTR,
               MAC2STR(rx_param->sa));

    if (!auth) {
        wpa_printf(MSG_DEBUG, "DPP: No DPP Authentication in progress - drop");
        rc = ESP_ERR_DPP_FAILURE;
        goto fail;
    }

    if (os_memcmp(rx_param->sa, auth->peer_mac_addr, ETH_ALEN) != 0) {
        wpa_printf(MSG_DEBUG, "DPP: MAC address mismatch (expected "
                   MACSTR ") - drop", MAC2STR(auth->peer_mac_addr));
        rc = ESP_ERR_DPP_FAILURE;
        goto fail;
    }

    eloop_cancel_timeout(esp_dpp_auth_conf_wait_timeout, NULL, NULL);

    if (dpp_auth_conf_rx(auth, (const u8 *)&public_action->v,
                         dpp_data, len) < 0) {
        wpa_printf(MSG_DEBUG, "DPP: Authentication failed");
        rc = ESP_ERR_DPP_FAILURE;
        goto fail;
    }

    /* Send GAS Query Req */
    gas_query_req_tx(auth);

    return;

fail:
    esp_dpp_call_cb(ESP_SUPP_DPP_FAIL, (void *)rc);
}

static void esp_dpp_rx_auth(struct action_rx_param *rx_param)
{
    uint8_t crypto_suit, type;
    uint8_t *tmp;

    tmp = rx_param->action_frm->u.public_action.v.pa_vendor_spec.vendor_data;
    crypto_suit = tmp[0];
    type = tmp[1];

    if (crypto_suit != 1) {
        wpa_printf(MSG_ERROR, "DPP: Unsupported crypto suit");
        esp_dpp_call_cb(ESP_SUPP_DPP_FAIL, (void *)ESP_ERR_NOT_SUPPORTED);
        return;
    }

    switch (type) {
    case DPP_PA_AUTHENTICATION_REQ:
        esp_dpp_rx_auth_req(rx_param, &tmp[2]);
        break;
    case DPP_PA_AUTHENTICATION_CONF:
        esp_dpp_rx_auth_conf(rx_param, &tmp[2]);
        break;
    }
}

static void gas_query_resp_rx(struct action_rx_param *rx_param)
{
    struct dpp_authentication *auth = s_dpp_ctx.dpp_auth;
    uint8_t *pos = rx_param->action_frm->u.public_action.v.pa_gas_resp.data;
    uint8_t *resp = &pos[10];
    int i, res;

    if (pos[1] == WLAN_EID_VENDOR_SPECIFIC && pos[2] == 5 &&
            WPA_GET_BE24(&pos[3]) == OUI_WFA && pos[6] == 0x1a && pos[7] == 1 && auth) {
        if (dpp_conf_resp_rx(auth, resp, rx_param->vendor_data_len - 2) < 0) {
            wpa_printf(MSG_DEBUG, "DPP: Configuration attempt failed");
            goto fail;
        }

        for (i = 0; i < auth->num_conf_obj; i++) {
            res = esp_dpp_handle_config_obj(auth, &auth->conf_obj[i]);
            if (res < 0) {
                goto fail;
            }
        }
    }

    return;
fail:
    esp_dpp_call_cb(ESP_SUPP_DPP_FAIL, (void *)ESP_ERR_DPP_FAILURE);
}

static void esp_dpp_rx_action(struct action_rx_param *rx_param)
{
    if (rx_param->action_frm->category == WLAN_ACTION_PUBLIC) {
        struct ieee80211_public_action *public_action =
                &rx_param->action_frm->u.public_action;

        wpa_printf(MSG_DEBUG, "DPP: Rx Public Action frame: action - %d",
                   public_action->action);

        if (public_action->action == WLAN_PA_VENDOR_SPECIFIC &&
                WPA_GET_BE24(public_action->v.pa_vendor_spec.oui) == OUI_WFA &&
                public_action->v.pa_vendor_spec.wfa_stype == DPP_OUI_TYPE) {

            rx_param->vendor_data_len = rx_param->frm_len -
                                        (size_t)(public_action->v.pa_vendor_spec.vendor_data -
                                                 (u8 *)rx_param->action_frm);

            if (s_dpp_listen_in_progress) {
                esp_supp_dpp_stop_listen();
            }

            esp_dpp_rx_auth(rx_param);
        } else if (public_action->action == WLAN_PA_GAS_INITIAL_RESP &&
                   public_action->v.pa_gas_resp.type == WLAN_EID_ADV_PROTO &&
                   public_action->v.pa_gas_resp.length == 8 &&
                   public_action->v.pa_gas_resp.status_code == 0) {

            rx_param->vendor_data_len = rx_param->frm_len -
                                        (size_t)(public_action->v.pa_gas_resp.data +
                                                 public_action->v.pa_gas_resp.length -
                                                 (u8 *)rx_param->action_frm);

            gas_query_resp_rx(rx_param);
        }
    }

    os_free(rx_param->action_frm);
    os_free(rx_param);
}

static void esp_dpp_task(void *pvParameters )
{
    dpp_event_t *evt;
    bool task_del = false;

    for (;;) {
        if (os_queue_recv(s_dpp_evt_queue, &evt, OS_BLOCK) == TRUE) {
            if (evt->id >= SIG_DPP_MAX) {
                os_free(evt);
                continue;
            }

            switch (evt->id) {
            case SIG_DPP_DEL_TASK:
                struct dpp_bootstrap_params_t *params = &s_dpp_ctx.bootstrap_params;
                eloop_cancel_timeout(esp_dpp_auth_conf_wait_timeout, NULL, NULL);
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
                task_del = true;
                break;

            case SIG_DPP_BOOTSTRAP_GEN: {
                char *command = (char *)evt->data;
                const char *uri;

                s_dpp_ctx.id = dpp_bootstrap_gen(s_dpp_ctx.dpp_global, command);
                uri = dpp_bootstrap_get_uri(s_dpp_ctx.dpp_global, s_dpp_ctx.id);

                esp_dpp_call_cb(ESP_SUPP_DPP_URI_READY, (void *)uri);
                os_free(command);
            }
            break;

            case SIG_DPP_RX_ACTION: {
                esp_dpp_rx_action((struct action_rx_param *)evt->data);
            }
            break;

            case SIG_DPP_LISTEN_NEXT_CHANNEL: {
                struct dpp_bootstrap_params_t *p = &s_dpp_ctx.bootstrap_params;
                static int counter;
                int channel;
                esp_err_t ret = 0;

                if (p->num_chan <= 0) {
                    wpa_printf(MSG_ERROR, "Listen channel not set");
                    break;
                }
                channel = p->chan_list[counter++ % p->num_chan];
                ret = esp_wifi_remain_on_channel(WIFI_IF_STA, WIFI_ROC_REQ, channel,
                                           BOOTSTRAP_ROC_WAIT_TIME, s_action_rx_cb);
                if (ret != ESP_OK) {
                    wpa_printf(MSG_ERROR, "Failed ROC. error : 0x%x", ret);
                    break;
                }
                s_dpp_listen_in_progress = true;
            }
            break;

            default:
                break;
            }

            os_free(evt);

            if (task_del) {
                break;
            }
        }
    }

    os_queue_delete(s_dpp_evt_queue);
    s_dpp_evt_queue = NULL;

    if (s_dpp_api_lock) {
        os_semphr_delete(s_dpp_api_lock);
        s_dpp_api_lock = NULL;
    }

    /* At this point, we completed */
    os_task_delete(NULL);
}

int esp_supp_rx_action(uint8_t *hdr, uint8_t *payload, size_t len, uint8_t channel)
{
    struct ieee80211_hdr *rx_hdr = (struct ieee80211_hdr *)hdr;
    struct action_rx_param *rx_param;
    int ret = ESP_ERR_NOT_SUPPORTED;

    if (WLAN_FC_GET_STYPE(rx_hdr->frame_control) == WLAN_FC_STYPE_ACTION) {
        rx_param = os_zalloc(sizeof(struct action_rx_param));
        if (!rx_param) {
            wpa_printf(MSG_ERROR, "Failed to allocate memory for Rx Action");
            return ESP_ERR_NO_MEM;
        }
        os_memcpy(rx_param->sa, rx_hdr->addr2, ETH_ALEN);
        rx_param->channel = channel;
        rx_param->action_frm = os_zalloc(len);
        if (!rx_param->action_frm) {
            wpa_printf(MSG_ERROR, "Failed to allocate memory for Rx Action");
            os_free(rx_param);
            return ESP_ERR_NO_MEM;
        }
        rx_param->frm_len = len;
        os_memcpy(rx_param->action_frm, payload, len);

        ret = esp_dpp_post_evt(SIG_DPP_RX_ACTION, (u32)rx_param);
        if (ESP_OK != ret) {
            wpa_printf(MSG_ERROR, "Failed to post event to DPP Task(status=%d)", ret);
            os_free(rx_param->action_frm);
            os_free(rx_param);
            return ret;
        }
    }

    return ret;
}

static void offchan_event_handler(void *arg, esp_event_base_t event_base,
                                  int32_t event_id, void *event_data)
{
    if (event_id == WIFI_EVENT_ACTION_TX_STATUS) {
        wifi_event_action_tx_status_t *evt =
            (wifi_event_action_tx_status_t *)event_data;
        wpa_printf(MSG_DEBUG, "Mgmt Tx Status - %d, Cookie - 0x%x",
                   evt->status, (uint32_t)evt->context);

        if (evt->status) {
            eloop_cancel_timeout(esp_dpp_auth_conf_wait_timeout, NULL, NULL);
            esp_dpp_call_cb(ESP_SUPP_DPP_FAIL, (void *)ESP_ERR_DPP_TX_FAILURE);
        }

    } else if (event_id == WIFI_EVENT_ROC_DONE) {
        wifi_event_roc_done_t *evt = (wifi_event_roc_done_t *)event_data;

        if (s_dpp_listen_in_progress && evt->context == (uint32_t)s_action_rx_cb) {
            esp_dpp_post_evt(SIG_DPP_LISTEN_NEXT_CHANNEL, 0);
        }
    }
}

static char *esp_dpp_parse_chan_list(const char *chan_list)
{
    struct dpp_bootstrap_params_t *params = &s_dpp_ctx.bootstrap_params;
    char *uri_channels = os_zalloc(14 * 6 + 1);
    const char *pos = chan_list;
    const char *pos2;
    char *pos3 = uri_channels;
    params->num_chan = 0;

    os_memcpy(pos3, " chan=", strlen(" chan="));
    pos3 += strlen(" chan=");

    while (pos && *pos) {
        int channel;
        int len = strlen(chan_list);

        pos2 = pos;
        while (*pos2 >= '0' && *pos2 <= '9') {
            pos2++;
        }
        if (*pos2 == ',' || *pos2 == ' ' || *pos2 == '\0') {
            channel = atoi(pos);
            if (channel < 1 || channel > 14) {
                os_free(uri_channels);
                return NULL;
            }
            params->chan_list[params->num_chan++] = channel;
            os_memcpy(pos3, "81/", strlen("81/"));
            pos3 += strlen("81/");
            os_memcpy(pos3, pos, (pos2 - pos));
            pos3 += (pos2 - pos);
            *pos3++ = ',';

            pos = pos2 + 1;
        }
        while (*pos == ',' || *pos == ' ' || *pos == '\0') {
            pos++;
        }

        if (((int)(pos - chan_list) >= len)) {
            break;
        }
    }
    *(pos3 - 1) = ' ';

    return uri_channels;
}

esp_err_t
esp_supp_dpp_bootstrap_gen(const char *chan_list, enum dpp_bootstrap_type type,
                           const char *key, const char *uri_info)
{
    if (!s_dpp_ctx.dpp_global) {
        wpa_printf(MSG_ERROR, "DPP: failed to bootstrap as dpp not initialized.");
        return ESP_FAIL;
    }
    struct dpp_bootstrap_params_t *params = &s_dpp_ctx.bootstrap_params;
    char *uri_chan_list = esp_dpp_parse_chan_list(chan_list);
    char *command = os_zalloc(1200);
    int ret;

    if (!uri_chan_list || !command || params->num_chan >= 14 || params->num_chan == 0) {
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

    ret = esp_dpp_post_evt(SIG_DPP_BOOTSTRAP_GEN, (u32)command);
    if (ret != ESP_OK) {
        os_free(command);
        if (params->info) {
            os_free(params->info);
            params->info = NULL;
        }
        goto fail;
    }

    ret = ESP_OK;
fail:
    if (uri_chan_list) {
        os_free(uri_chan_list);
    }

    return ret;
}

esp_err_t esp_supp_dpp_start_listen(void)
{
    if (s_dpp_listen_in_progress) {
        wpa_printf(MSG_ERROR, "DPP: Failed to start listen as listen is already in progress.");
        return ESP_FAIL;
    }

    if (!s_dpp_ctx.dpp_global || s_dpp_ctx.id < 1) {
        wpa_printf(MSG_ERROR, "DPP: failed to start listen as dpp not initialized or bootstrapped.");
        return ESP_FAIL;
    }

    if (esp_wifi_get_user_init_flag_internal() == 0) {
        wpa_printf(MSG_ERROR, "DPP: ROC not possible before wifi is started");
        return ESP_ERR_INVALID_STATE;
    }

    return esp_dpp_post_evt(SIG_DPP_LISTEN_NEXT_CHANNEL, 0);
}

void esp_supp_dpp_stop_listen(void)
{
    s_dpp_listen_in_progress = false;
    esp_wifi_remain_on_channel(WIFI_IF_STA, WIFI_ROC_CANCEL, 0, 0, NULL);
}

bool is_dpp_enabled(void)
{
    return (s_dpp_ctx.dpp_global ? true : false);
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
    if (s_dpp_ctx.dpp_global) {
        wpa_printf(MSG_ERROR, "DPP: failed to init as init already done.");
        return ESP_FAIL;
    }

    os_bzero(&s_dpp_ctx, sizeof(s_dpp_ctx));
    struct dpp_global_config cfg = {0};
    cfg.cb_ctx = &s_dpp_ctx;
    cfg.msg_ctx = &s_dpp_ctx;
    s_dpp_ctx.dpp_global = dpp_global_init(&cfg);
    if (!s_dpp_ctx.dpp_global) {
        wpa_printf(MSG_ERROR, "DPP: failed to allocate memory for dpp_global");
        ret = ESP_ERR_NO_MEM;
        goto init_fail;
    }

    s_dpp_api_lock = os_recursive_mutex_create();
    if (!s_dpp_api_lock) {
        wpa_printf(MSG_ERROR, "DPP: dpp_init: failed to create DPP API lock");
        ret = ESP_ERR_NO_MEM;
        goto init_fail;
    }

    s_dpp_evt_queue = os_queue_create(3, sizeof(dpp_event_t));
    if (!s_dpp_evt_queue) {
        wpa_printf(MSG_ERROR, "DPP: dpp_init: failed to create DPP API queue");
        ret = ESP_ERR_NO_MEM;
        goto init_fail;
    }

    ret = os_task_create(esp_dpp_task, "dppT", DPP_TASK_STACK_SIZE, NULL, 2, &s_dpp_task_hdl);
    if (ret != TRUE) {
        wpa_printf(MSG_ERROR, "DPP: failed to create task");
        ret = ESP_ERR_NO_MEM;
        goto init_fail;
    }

    s_dpp_listen_in_progress = false;
    s_dpp_ctx.dpp_event_cb = cb;

    esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_ACTION_TX_STATUS,
                               &offchan_event_handler, NULL);
    esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_ROC_DONE,
                               &offchan_event_handler, NULL);

    wpa_printf(MSG_INFO, "esp_dpp_task prio:%d, stack:%d", 2, DPP_TASK_STACK_SIZE);

    return ESP_OK;
init_fail:
    if (s_dpp_ctx.dpp_global) {
        dpp_global_deinit(s_dpp_ctx.dpp_global);
        s_dpp_ctx.dpp_global = NULL;
    }
    if (s_dpp_api_lock) {
        os_mutex_delete(s_dpp_api_lock);
        s_dpp_api_lock = NULL;
    }
    if (s_dpp_evt_queue) {
        os_queue_delete(s_dpp_evt_queue);
        s_dpp_evt_queue = NULL;
    }
    return ret;
}
void esp_supp_dpp_deinit(void)
{

    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_ACTION_TX_STATUS,
                               &offchan_event_handler);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_ROC_DONE,
                               &offchan_event_handler);
    if (s_dpp_ctx.dpp_global) {
        if (esp_dpp_post_evt(SIG_DPP_DEL_TASK, 0)) {
            wpa_printf(MSG_ERROR, "DPP Deinit Failed");
        }
    }
}
#endif
