/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_wifi.h"
#include "esp_check.h"
#include "esp_nan_usd.h"
#include "common/nan_de.h"
#include "esp_wifi_driver.h"
#include "common/ieee802_11_common.h"
#include "utils/eloop.h"
#include "esp_nan.h"

struct nan_de *g_nan_de = NULL;

/* One cached peer per service for get_peer_* (match, reply, or follow-up).
 * Independent of nan_de pauseState (sel_peer_*), which has a ~60s lifetime.
 * Peer cache and g_nan_de are only touched on the wifi/eloop task. */
struct nan_usd_peer {
    bool valid;
    u8 own_svc_id;
    u8 peer_svc_id;
    u8 peer_addr[ETH_ALEN];
    u8 peer_svc_type;
};

static struct nan_usd_peer s_nan_usd_peers[NAN_DE_MAX_SERVICE];

static void nan_usd_clear_peer(int own_svc_id)
{
    if (own_svc_id < 1 || own_svc_id > NAN_DE_MAX_SERVICE) {
        return;
    }
    os_memset(&s_nan_usd_peers[own_svc_id - 1], 0, sizeof(s_nan_usd_peers[0]));
}

static void nan_usd_clear_all_peers(void)
{
    os_memset(s_nan_usd_peers, 0, sizeof(s_nan_usd_peers));
}

static void nan_usd_save_peer(int own_svc_id, int peer_svc_id, const u8 *peer_addr,
                              u8 peer_svc_type)
{
    struct nan_usd_peer *peer;

    if (own_svc_id < 1 || own_svc_id > NAN_DE_MAX_SERVICE || !peer_addr) {
        return;
    }

    /* Keep the latest match, reply, or follow-up peer. */
    peer = &s_nan_usd_peers[own_svc_id - 1];
    peer->valid = true;
    peer->own_svc_id = (u8)own_svc_id;
    peer->peer_svc_id = (u8)peer_svc_id;
    os_memcpy(peer->peer_addr, peer_addr, ETH_ALEN);
    peer->peer_svc_type = peer_svc_type;
}

static const struct nan_usd_peer *nan_usd_get_peer(int own_svc_id)
{
    if (own_svc_id < 1 || own_svc_id > NAN_DE_MAX_SERVICE) {
        return NULL;
    }
    if (!s_nan_usd_peers[own_svc_id - 1].valid) {
        return NULL;
    }
    return &s_nan_usd_peers[own_svc_id - 1];
}

static const char *nan_reason_txt(enum nan_de_reason reason)
{
#ifdef DEBUG_PRINT
    switch (reason) {
    case NAN_DE_REASON_TIMEOUT:
        return "timeout";
    case NAN_DE_REASON_USER_REQUEST:
        return "user-request";
    case NAN_DE_REASON_FAILURE:
        return "failure";
    }

    return "unknown";
#else
    (void)reason;
    return "";
#endif /* DEBUG_PRINT */
}
static void nan_sta_stop_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data)
{
    if (event_id == WIFI_EVENT_STA_STOP) {
        esp_wifi_nan_usd_stop();
    }
}

/* Per section 4.5.3 of the Wi-Fi Aware Specification v4.0 (NaN-USD),
 * only 20 MHz bandwidth channels are permitted for NAN-USD operation
 * in both the 2.4 GHz and 5 GHz frequency bands.
 * */
static int esp_nan_chan_to_freq(uint8_t chan)
{
    // 2.4 GHz band
    if (chan >= 1 && chan <= 13) {
        return 2407 + 5 * chan;
    } else if (chan == 14) {
        return 2414 + 5 * chan;
    }

    // 5 GHz band — standard 20 MHz channel ranges
    if ((chan >= 36 && chan <= 64) ||
            (chan >= 100 && chan <= 144) ||
            (chan >= 149 && chan <= 165)) {
        return 5000 + chan * 5;
    }

    return -1;
}

static int esp_nan_freq_to_chan(int freq)
{
    // 2.4 GHz band
    if (freq >= 2412 && freq <= 2472) {
        return (freq - 2407) / 5;
    } else if (freq == 2484) {
        return 14;
    }

    // 5 GHz band
    if ((freq >= 5180 && freq <= 5240) ||           // Channels 36–64
            (freq >= 5500 && freq <= 5720) ||       // Channels 100–144
            (freq >= 5745 && freq <= 5825)) {       // Channels 149–165
        return (freq - 5000) / 5;
    }
    return -1;
}

enum {
    NAN_USD_EVT_TX_DONE = 1,
    NAN_USD_EVT_TX_WAIT_ENDED,
    NAN_USD_EVT_LISTEN_ENDED,
};

/* Bumped on deinit so late eloop work from a prior session is dropped. */
static uint32_t s_nan_usd_session;

struct nan_usd_wifi_evt_ctx {
    uint32_t session;
    int type;
    unsigned int freq;
};

struct nan_usd_rx_ctx {
    uint32_t session;
    u8 peer_addr[ETH_ALEN];
    u8 a3[ETH_ALEN];
    unsigned int freq;
    size_t len;
    u8 buf[];
};

static void nan_usd_wifi_evt_internal(void *eloop_data, void *user_ctx)
{
    struct nan_usd_wifi_evt_ctx *ctx = user_ctx;

    (void)eloop_data;

    if (g_nan_de && ctx->session == s_nan_usd_session) {
        switch (ctx->type) {
        case NAN_USD_EVT_TX_DONE:
            nan_de_tx_status(g_nan_de, ctx->freq, NULL);
            break;
        case NAN_USD_EVT_TX_WAIT_ENDED:
            nan_de_tx_wait_ended(g_nan_de);
            break;
        case NAN_USD_EVT_LISTEN_ENDED:
            nan_de_listen_ended(g_nan_de, ctx->freq);
            break;
        default:
            break;
        }
    }
    os_free(ctx);
}

static int nan_usd_post_wifi_evt(int type, unsigned int freq)
{
    struct nan_usd_wifi_evt_ctx *ctx = os_zalloc(sizeof(*ctx));

    if (!ctx) {
        return ESP_ERR_NO_MEM;
    }
    ctx->session = s_nan_usd_session;
    ctx->type = type;
    ctx->freq = freq;
    if (eloop_register_timeout(0, 0, nan_usd_wifi_evt_internal, NULL, ctx) != 0) {
        os_free(ctx);
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

static void nan_de_tx_event_handler(void *arg, esp_event_base_t event_base,
                                    int32_t event_id, void *event_data)
{
    (void)arg;
    (void)event_base;

    if (event_id == WIFI_EVENT_ACTION_TX_STATUS) {
        wifi_event_action_tx_status_t *evt = (wifi_event_action_tx_status_t *)event_data;
        if (evt->status == WIFI_ACTION_TX_DONE) {
            int freq = esp_nan_chan_to_freq(evt->channel);
            if (freq == -1) {
                wpa_printf(MSG_ERROR, "Invalid channel received from Action Tx handler");
                return;
            }
            nan_usd_post_wifi_evt(NAN_USD_EVT_TX_DONE, freq);
        } else if (evt->status == WIFI_ACTION_TX_DURATION_COMPLETED) {
            nan_usd_post_wifi_evt(NAN_USD_EVT_TX_WAIT_ENDED, 0);
        }
    } else if (event_id == WIFI_EVENT_ROC_DONE) {
        wifi_event_roc_done_t *evt = (wifi_event_roc_done_t *)event_data;
        int freq = esp_nan_chan_to_freq(evt->channel);
        if (freq == -1) {
            wpa_printf(MSG_ERROR, "Invalid channel received from ROC done handler");
            return;
        }
        nan_usd_post_wifi_evt(NAN_USD_EVT_LISTEN_ENDED, freq);
    }
}

static void nan_usd_rx_sdf_internal(void *eloop_data, void *user_ctx)
{
    struct nan_usd_rx_ctx *ctx = user_ctx;

    (void)eloop_data;

    if (g_nan_de && ctx->session == s_nan_usd_session) {
        nan_de_rx_sdf(g_nan_de, ctx->peer_addr, ctx->a3, ctx->freq, ctx->buf, ctx->len);
    }
    os_free(ctx);
}

int esp_nan_de_rx_action(uint8_t *hdr, uint8_t *payload, size_t len, uint8_t channel)
{
    struct ieee80211_hdr *rx_hdr = (struct ieee80211_hdr *)hdr;
    struct nan_usd_rx_ctx *ctx;
    int freq;
    uint8_t category;
    uint8_t public_action;
    uint32_t oui_value;
    uint8_t oui_type;
    const u8 *sdf;
    size_t sdf_len;

    if (len < 6) {
        /* Frame too short for NAN-SDF frame */
        return ESP_FAIL;
    }
    /* NAN SDF
     * Category code - 1 byte
     * Public Action - 1 byte
     * OUI - 3 bytes
     * WFA subtype - 1 byte
     * NAN PROTOCOL - Variable */

    category = payload[0];
    public_action = payload[1];
    oui_value = WPA_GET_BE24(&payload[2]);
    oui_type = payload[5];

    if (!(category == WLAN_ACTION_PUBLIC &&
            public_action == WLAN_PA_VENDOR_SPECIFIC &&
            oui_value == OUI_WFA &&
            oui_type == NAN_OUI_TYPE)) {
        /* Frame is not a NAN SDF frame */
        return ESP_FAIL;
    }

    freq = esp_nan_chan_to_freq(channel);
    if (freq == -1) {
        wpa_printf(MSG_ERROR, "Invalid channel from Rx action frame");
        return ESP_FAIL;
    }

    sdf = payload + 6;
    sdf_len = len - 6;
    ctx = os_zalloc(sizeof(*ctx) + sdf_len);
    if (!ctx) {
        return ESP_ERR_NO_MEM;
    }
    ctx->session = s_nan_usd_session;
    os_memcpy(ctx->peer_addr, rx_hdr->addr2, ETH_ALEN);
    os_memcpy(ctx->a3, rx_hdr->addr3, ETH_ALEN);
    ctx->freq = freq;
    ctx->len = sdf_len;
    if (sdf_len) {
        os_memcpy(ctx->buf, sdf, sdf_len);
    }

    if (eloop_register_timeout(0, 0, nan_usd_rx_sdf_internal, NULL, ctx) != 0) {
        os_free(ctx);
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

static int esp_nan_de_tx(void *ctx, unsigned int freq, unsigned int wait_time,
                         const u8 *dst, const u8 *src, const  u8 *bssid,
                         const struct wpabuf *buf)
{
    int buf_len = buf->used;
    int channel;

    wifi_action_tx_req_t *req = os_zalloc(sizeof(*req) + buf_len);
    if (!req) {
        wpa_printf(MSG_ERROR, "Allocation for tx request failed");
        return ESP_FAIL;
    }

    req->ifx = WIFI_IF_STA;
    req->type = WIFI_OFFCHAN_TX_REQ;
    req->wait_time_ms = wait_time;
    memcpy(req->dest_mac, dst, ETH_ALEN);
    memcpy(req->bssid, bssid, ETH_ALEN);
    req->no_ack = false;
    req->data_len = buf_len;
    req->rx_cb = esp_nan_de_rx_action;
    memcpy(req->data, buf->buf, buf_len);

    channel = esp_nan_freq_to_chan(freq);
    if (channel == -1) {
        wpa_printf(MSG_ERROR, "Could not determine channel for freq %d", freq);
        os_free(req);
        return ESP_FAIL;
    }
    req->channel = channel;

    if (esp_wifi_action_tx_req(req) != ESP_OK) {
        wpa_printf(MSG_ERROR, "Offchannel tx request failed");
        os_free(req);
        return ESP_FAIL;
    }

    os_free(req);
    return ESP_OK;
}

static int esp_nan_de_listen(void *ctx, unsigned int freq, unsigned int duration)
{
    int channel;

    channel = esp_nan_freq_to_chan(freq);
    if (channel == -1) {
        wpa_printf(MSG_ERROR, "Could not determine channel for freq %d", freq);
        return ESP_FAIL;
    }

    wifi_roc_req_t *req = os_zalloc(sizeof(wifi_roc_req_t));
    if (req == NULL) {
        wpa_printf(MSG_ERROR, "Failed to allocate for ROC request");
        return ESP_FAIL;
    }
    req->ifx = WIFI_IF_STA;
    req->type = WIFI_ROC_REQ;
    req->channel = channel;
    req->wait_time_ms = duration;
    req->rx_cb = esp_nan_de_rx_action;
    req->done_cb = NULL;
    req->allow_broadcast = true;

    if (esp_wifi_remain_on_channel(req) != ESP_OK) {
        wpa_printf(MSG_ERROR, "ROC request failure");
        os_free(req);
        return ESP_FAIL;
    }

    if (g_nan_de) {
        nan_de_listen_started(g_nan_de, freq, duration);
    }
    os_free(req);
    return ESP_OK;
}

static void esp_nan_de_discovery_result(void *ctx, int subscribe_id, enum nan_service_protocol_type srv_proto_type,
                                        const u8 *ssi, size_t ssi_len, int peer_publish_id, const u8 *peer_addr, bool fsd, bool fsd_gas)
{
    wpa_printf(MSG_INFO, "NAN_USD DISCOVERY_RESULT - subscribe_id = %d peer_publish_id = %d peer_address = "MACSTR" service_protocol_type = %d",
               subscribe_id, peer_publish_id, MAC2STR(peer_addr), srv_proto_type);

    /* Cache peer for get_peer_* (subscriber path); do not touch pauseState. */
    nan_usd_save_peer(subscribe_id, peer_publish_id, peer_addr, ESP_NAN_PUBLISH);

    wifi_event_nan_svc_match_t *evt = os_zalloc(sizeof(wifi_event_nan_svc_match_t) + ssi_len);
    if (evt == NULL) {
        return;
    }
    evt->subscribe_id = subscribe_id;
    evt->publish_id = peer_publish_id;
    memcpy(evt->pub_if_mac, peer_addr, ETH_ALEN);
    if (ssi && ssi_len) {
        memcpy(evt->ssi, ssi, ssi_len);
        evt->ssi_len = ssi_len;
    }
    esp_event_post(WIFI_EVENT, WIFI_EVENT_NAN_SVC_MATCH, evt, sizeof(wifi_event_nan_svc_match_t) + ssi_len, portMAX_DELAY);
    os_free(evt);
}

static void esp_nan_de_replied(void *ctx, int publish_id, const u8 *peer_addr,
                               int peer_subscribe_id,
                               enum nan_service_protocol_type srv_proto_type,
                               const u8 *ssi, size_t ssi_len)
{

    wpa_printf(MSG_INFO, "NAN_USD REPLIED - publish_id = %d peer_subscribe_id = %d peer_address = "MACSTR" service_protocol_type = %d",
               publish_id, peer_subscribe_id, MAC2STR(peer_addr), srv_proto_type);

    /* Cache peer for get_peer_* (publisher path); do not touch pauseState. */
    nan_usd_save_peer(publish_id, peer_subscribe_id, peer_addr, ESP_NAN_SUBSCRIBE);

    wifi_event_nan_replied_t *evt = os_zalloc(sizeof(wifi_event_nan_replied_t) + ssi_len);
    if (evt == NULL) {
        return;
    }
    evt->publish_id = publish_id;
    evt->subscribe_id = peer_subscribe_id;
    memcpy(evt->sub_if_mac, peer_addr, ETH_ALEN);
    if (ssi && ssi_len) {
        memcpy(evt->ssi, ssi, ssi_len);
        evt->ssi_len = ssi_len;
    }
    esp_event_post(WIFI_EVENT, WIFI_EVENT_NAN_REPLIED, evt, sizeof(wifi_event_nan_replied_t) + ssi_len, portMAX_DELAY);
    os_free(evt);
}

static void esp_nan_de_publish_terminated(void *ctx, int publish_id,
                                          enum nan_de_reason reason)
{
    wpa_printf(MSG_INFO, "NAN_USD PUBLISH_TERMINATED - publish_id = %d reason = %s", publish_id, nan_reason_txt(reason));
    nan_usd_clear_peer(publish_id);
}

static void esp_nan_de_subscribe_terminated(void *ctx, int subscribe_id,
                                            enum nan_de_reason reason)
{
    wpa_printf(MSG_INFO, "NAN_USD SUBSCRIBE_TERMINATED - subscribe_id = %d reason = %s", subscribe_id, nan_reason_txt(reason));
    nan_usd_clear_peer(subscribe_id);
}

static void esp_nan_de_receive(void *ctx, int id, int peer_instance_id,
                               const u8 *ssi, size_t ssi_len,
                               const u8 *peer_addr)
{
    int own_type;

    wpa_hexdump(MSG_INFO, "NAN_RECEIVE", ssi, ssi_len);

    /* Passive subscribe replies with a Follow-up, so cache the opposite role here. */
    own_type = g_nan_de ? nan_de_get_service_type(g_nan_de, id) : -1;
    if (own_type >= 0) {
        u8 peer_svc_type = (own_type == 0) ? ESP_NAN_SUBSCRIBE : ESP_NAN_PUBLISH;

        nan_usd_save_peer(id, peer_instance_id, peer_addr, peer_svc_type);
    }

    wifi_event_nan_receive_t *evt = os_zalloc(sizeof(wifi_event_nan_receive_t) + ssi_len);
    if (evt == NULL) {
        return;
    }
    evt->inst_id = id;
    evt->peer_inst_id = peer_instance_id;
    memcpy(evt->peer_if_mac, peer_addr, ETH_ALEN);
    if (ssi && ssi_len) {
        memcpy(evt->ssi, ssi, ssi_len);
        evt->ssi_len = ssi_len;
    }
    esp_event_post(WIFI_EVENT, WIFI_EVENT_NAN_RECEIVE, evt, sizeof(wifi_event_nan_receive_t) + ssi_len, portMAX_DELAY);
    os_free(evt);
}

static void nan_usd_register_wifi_events(void)
{
    esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_ACTION_TX_STATUS,
                               &nan_de_tx_event_handler, NULL);
    esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_ROC_DONE,
                               &nan_de_tx_event_handler, NULL);
    esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_STOP,
                               &nan_sta_stop_handler, NULL);
}

static void nan_usd_unregister_wifi_events(void)
{
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_ACTION_TX_STATUS, &nan_de_tx_event_handler);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_ROC_DONE, &nan_de_tx_event_handler);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_STOP, &nan_sta_stop_handler);
}

static int esp_nan_usd_deinit_internal(void *eloop_data, void *user_ctx)
{
    (void)eloop_data;
    (void)user_ctx;

    s_nan_usd_session++;

    if (!g_nan_de) {
        return ESP_OK;
    }

    nan_de_deinit(g_nan_de);
    g_nan_de = NULL;
    nan_usd_clear_all_peers();
    return ESP_OK;
}

esp_err_t esp_nan_usd_deinit(void)
{
    int ret;

    /*
     * Unregister on the caller task before blocking on eloop. WIFI_EVENT
     * handlers (e.g. STA_STOP) run on the default event task while that task
     * holds the event-loop mutex; unregistering from eloop while that handler
     * waits on eloop_register_timeout_blocking deadlocks.
     */
    nan_usd_unregister_wifi_events();
    ret = eloop_register_timeout_blocking(esp_nan_usd_deinit_internal, NULL, NULL);
    if (ret != ESP_OK) {
        nan_usd_register_wifi_events();
    }
    return ret;
}

static int esp_nan_usd_init_internal(void *eloop_data, void *user_ctx)
{
    struct nan_callbacks cb;
    uint8_t mac[ETH_ALEN];
#ifndef ESP_SUPPLICANT
    bool offload = wpa_s->drv_flags2 & WPA_DRIVER_FLAGS2_NAN_OFFLOAD;
#else
    bool offload = false;
#endif
    int max_listen = 1000; // default supplicant value; hardcoded for now

    (void)eloop_data;
    (void)user_ctx;

    if (g_nan_de) {
        return ESP_OK;
    }

    os_memset(&cb, 0, sizeof(cb));
    cb.ctx = NULL;
    cb.tx = esp_nan_de_tx;
    cb.listen = esp_nan_de_listen;
    cb.discovery_result = esp_nan_de_discovery_result;
    cb.replied = esp_nan_de_replied;
    cb.publish_terminated = esp_nan_de_publish_terminated;
    cb.subscribe_terminated = esp_nan_de_subscribe_terminated;
    cb.receive = esp_nan_de_receive;

    if (esp_wifi_get_mac(WIFI_IF_STA, mac) != ESP_OK) {
        wpa_printf(MSG_ERROR, "NAN-USD: Fetching MAC of STA ifx failed");
        return ESP_FAIL;
    }
    g_nan_de = nan_de_init(mac, offload, false, max_listen, &cb);
    if (!g_nan_de) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_nan_usd_init(void)
{
    int ret;

    ret = eloop_register_timeout_blocking(esp_nan_usd_init_internal, NULL, NULL);
    if (ret == ESP_OK) {
        nan_usd_register_wifi_events();
    } else {
        nan_usd_unregister_wifi_events();
    }
    return ret;
}

int esp_nan_get_freq_list(int *freq_list, const uint8_t *chan_list, uint8_t chan_list_len)
{
    int freq, freq_ind = 0;
    for (int i = 0; i < chan_list_len; i++) {
        freq = esp_nan_chan_to_freq(chan_list[i]);
        if (freq != -1) {
            freq_list[freq_ind++] = freq;
        }
    }
    freq_list[freq_ind++] = 0;
    return freq_ind;
}

static int esp_nan_usd_publish_internal(const char *service_name, enum nan_service_protocol_type srv_proto_type,
                                        unsigned int ttl, uint8_t *ssi, uint16_t ssi_len, uint8_t default_channel,
                                        const wifi_scan_channel_bitmap_t channel_bitmap)
{
    int publish_id;
    struct wpabuf *buf = NULL;
    int freq, *freq_list = NULL;
    struct nan_publish_params pub_params = ESP_USD_PUBLISH_DEFAULT_PARAMS();
    bool p2p = false;
    uint8_t i = 0, chan_list_len;
    uint8_t bitmap_idx_2g = 1; // BIT-0 is not used in channel bitmap
    uint16_t channel_2ghz_bitmap;
#if CONFIG_SOC_WIFI_SUPPORT_5G
    uint8_t bitmap_idx_5g = 1; // BIT-0 is not used in channel bitmap
    uint32_t channel_5ghz_bitmap;
#endif

    if (!g_nan_de) {
        return -1;
    }

    pub_params.ttl = ttl;
    if (ssi && ssi_len) {
        buf = wpabuf_alloc(ssi_len);
        if (!buf) {
            wpa_printf(MSG_ERROR, "Allocating memory failed for NaN-USD Publish");
            return -1;
        }
        wpabuf_put_data(buf, ssi, ssi_len);
    }

    freq = esp_nan_chan_to_freq(default_channel);
    if (freq != -1) {
        pub_params.freq = freq;
    } else {
        pub_params.freq = NAN_USD_DEFAULT_FREQ;
    }

    channel_2ghz_bitmap = channel_bitmap.ghz_2_channels;
    channel_2ghz_bitmap &= ~BIT(0); // BIT-0 is not used for channel
    chan_list_len = __builtin_popcount(channel_2ghz_bitmap);

#if CONFIG_SOC_WIFI_SUPPORT_5G
    channel_5ghz_bitmap = channel_bitmap.ghz_5_channels;
    channel_5ghz_bitmap &= ~BIT(0); // BIT-0 is not used for channel
    chan_list_len +=  __builtin_popcount(channel_5ghz_bitmap);
#endif

    if (chan_list_len) {
        uint8_t chan_list[chan_list_len];
        while (channel_2ghz_bitmap) {
            bitmap_idx_2g = __builtin_ctz(channel_2ghz_bitmap);
            uint8_t chan_num = BIT_NUMBER_TO_CHANNEL(bitmap_idx_2g, WIFI_BAND_2G);
            if (chan_num != 0) {
                chan_list[i++] = chan_num;
            }
            channel_2ghz_bitmap &= ~BIT(bitmap_idx_2g);
        }
#if CONFIG_SOC_WIFI_SUPPORT_5G
        while (channel_5ghz_bitmap) {
            bitmap_idx_5g = __builtin_ctz(channel_5ghz_bitmap);
            uint8_t chan_num = BIT_NUMBER_TO_CHANNEL(bitmap_idx_5g, WIFI_BAND_5G);
            if (chan_num != 0) {
                chan_list[i++] = chan_num;
            }
            channel_5ghz_bitmap &= ~BIT(bitmap_idx_5g);
        }
#endif

        chan_list_len = i;
        freq_list = (int *)os_malloc(sizeof(int) * (chan_list_len + 1));
        if (freq_list) {
            esp_nan_get_freq_list(freq_list, chan_list, chan_list_len);
            pub_params.freq_list = freq_list;
        } else {
            wpa_printf(MSG_ERROR, "Allocating memory failed for frequency list");
            wpabuf_free(buf);
            return -1;
        }
    }

    publish_id = nan_de_publish(g_nan_de, service_name, srv_proto_type,
                                buf, NULL, &pub_params, p2p);
    if (publish_id > 0) {
        nan_usd_clear_peer(publish_id);
    }

    wpabuf_free(buf);
    if (freq_list) {
        os_free(freq_list);
    }
    return publish_id;
}

static int nan_usd_publish_on_eloop(void *eloop_data, void *user_ctx)
{
    const wifi_nan_publish_cfg_t *publish_cfg = user_ctx;

    (void)eloop_data;
    return esp_nan_usd_publish_internal(publish_cfg->service_name, WIFI_SVC_PROTO_RESERVED,
                                        publish_cfg->ttl, publish_cfg->ssi,
                                        publish_cfg->ssi_len, publish_cfg->usd_publish_config.usd_default_channel,
                                        publish_cfg->usd_publish_config.usd_chan_bitmap);
}

int esp_nan_usd_publish(const wifi_nan_publish_cfg_t *publish_cfg)
{
    if (!publish_cfg) {
        return -1;
    }
    return eloop_register_timeout_blocking(nan_usd_publish_on_eloop, NULL, (void *)publish_cfg);
}

struct nan_usd_update_publish_ctx {
    int publish_id;
    uint8_t *ssi;
    uint16_t ssi_len;
};

static int nan_usd_update_publish_internal(void *eloop_data, void *user_ctx)
{
    struct nan_usd_update_publish_ctx *ctx = user_ctx;
    struct wpabuf *buf = NULL;
    esp_err_t ret;

    (void)eloop_data;

    if (!g_nan_de) {
        return ESP_FAIL;
    }

    if (ctx->ssi && ctx->ssi_len) {
        buf = wpabuf_alloc(ctx->ssi_len);
        if (!buf) {
            wpa_printf(MSG_ERROR, "Allocating memory failed for NaN-USD Update Publish");
            return ESP_FAIL;
        }
        wpabuf_put_data(buf, ctx->ssi, ctx->ssi_len);
    }

    ret = nan_de_update_publish(g_nan_de, ctx->publish_id, buf);
    wpabuf_free(buf);
    return ret;
}

esp_err_t esp_nan_usd_update_publish(int publish_id, uint8_t *ssi, uint16_t ssi_len)
{
    struct nan_usd_update_publish_ctx ctx = {
        .publish_id = publish_id,
        .ssi = ssi,
        .ssi_len = ssi_len,
    };

    return eloop_register_timeout_blocking(nan_usd_update_publish_internal, NULL, &ctx);
}

static int nan_usd_cancel_publish_internal(void *eloop_data, void *user_ctx)
{
    int publish_id = *(int *)user_ctx;

    (void)eloop_data;

    if (!g_nan_de) {
        return ESP_FAIL;
    }

    nan_de_cancel_publish(g_nan_de, publish_id);
    nan_usd_clear_peer(publish_id);
    return ESP_OK;
}

esp_err_t esp_nan_usd_cancel_publish(int publish_id)
{
    return eloop_register_timeout_blocking(nan_usd_cancel_publish_internal, NULL, &publish_id);
}

/* Note: Our USD implementation uses the Service Protocol Type provided in SSI.
 * The 'srv_proto_type' parameter will be ignored.
 */
static int esp_nan_usd_subscribe_internal(const char *service_name, enum nan_service_protocol_type srv_proto_type,
                                          unsigned int ttl, uint8_t default_channel,  uint8_t *ssi, uint16_t ssi_len,
                                          const wifi_scan_channel_bitmap_t channel_bitmap)
{
    int subscribe_id;
    struct wpabuf *buf = NULL;
    int freq, *freq_list = NULL;
    /* USD Specification allows either active or passive mode for subscriber.
     * By default USD-Subscriber will be in passive mode */
    struct nan_subscribe_params sub_params = ESP_USD_SUBSCRIBE_DEFAULT_PARAMS();
    bool p2p = false;
    uint8_t i = 0, chan_list_len;
    uint8_t bitmap_idx_2g = 1; // BIT-0 is not used in channel bitmap
    uint16_t channel_2ghz_bitmap;

    if (!g_nan_de) {
        return -1;
    }

    sub_params.ttl = ttl;
    freq = esp_nan_chan_to_freq(default_channel);
    if (freq != -1) {
        sub_params.freq = freq;
    } else {
        sub_params.freq = NAN_USD_DEFAULT_FREQ;
    }

    if (ssi && ssi_len) {
        buf = wpabuf_alloc(ssi_len);
        if (!buf) {
            wpa_printf(MSG_ERROR, "Allocating memory failed for NaN-USD Subscribe");
            return -1;
        }
        wpabuf_put_data(buf, ssi, ssi_len);
    }

    channel_2ghz_bitmap = channel_bitmap.ghz_2_channels;
    channel_2ghz_bitmap &= ~BIT(0); // BIT-0 is not used for channel
    chan_list_len = __builtin_popcount(channel_2ghz_bitmap);
    if (chan_list_len) {
        uint8_t chan_list[chan_list_len];
        while (channel_2ghz_bitmap) {
            bitmap_idx_2g = __builtin_ctz(channel_2ghz_bitmap);
            uint8_t chan_num = BIT_NUMBER_TO_CHANNEL(bitmap_idx_2g, WIFI_BAND_2G);
            if (chan_num != 0) {
                chan_list[i++] = chan_num;
            }
            channel_2ghz_bitmap &= ~BIT(bitmap_idx_2g);
        }
        freq_list = (int *)os_malloc(sizeof(int) * (chan_list_len + 1));
        if (freq_list) {
            esp_nan_get_freq_list(freq_list, chan_list, chan_list_len);
            sub_params.freq_list = freq_list;
        } else {
            wpa_printf(MSG_ERROR, "Allocating memory failed for frequency list");
            wpabuf_free(buf);
            return -1;
        }
    }

    subscribe_id = nan_de_subscribe(g_nan_de, service_name, srv_proto_type, buf, NULL, &sub_params, p2p);
    if (subscribe_id > 0) {
        nan_usd_clear_peer(subscribe_id);
    }

    wpabuf_free(buf);
    if (freq_list) {
        os_free(freq_list);
    }
    return subscribe_id;
}

static int nan_usd_subscribe_on_eloop(void *eloop_data, void *user_ctx)
{
    const wifi_nan_subscribe_cfg_t *subscribe_cfg = user_ctx;

    (void)eloop_data;
    return esp_nan_usd_subscribe_internal(subscribe_cfg->service_name, WIFI_SVC_PROTO_RESERVED, subscribe_cfg->ttl,
                                          subscribe_cfg->usd_subscribe_config.usd_default_channel, subscribe_cfg->ssi, subscribe_cfg->ssi_len,
                                          subscribe_cfg->usd_subscribe_config.usd_chan_bitmap);
}

int esp_nan_usd_subscribe(const wifi_nan_subscribe_cfg_t *subscribe_cfg)
{
    if (!subscribe_cfg) {
        return -1;
    }
    return eloop_register_timeout_blocking(nan_usd_subscribe_on_eloop, NULL, (void *)subscribe_cfg);
}

static int nan_usd_cancel_subscribe_internal(void *eloop_data, void *user_ctx)
{
    int subscribe_id = *(int *)user_ctx;

    (void)eloop_data;

    if (!g_nan_de) {
        return ESP_FAIL;
    }

    nan_de_cancel_subscribe(g_nan_de, subscribe_id);
    nan_usd_clear_peer(subscribe_id);
    return ESP_OK;
}

esp_err_t esp_nan_usd_cancel_subscribe(int subscribe_id)
{
    return eloop_register_timeout_blocking(nan_usd_cancel_subscribe_internal, NULL, &subscribe_id);
}

static int nan_usd_cancel_service_internal(void *eloop_data, void *user_ctx)
{
    int service_id = *(int *)user_ctx;

    (void)eloop_data;

    if (!g_nan_de) {
        return ESP_FAIL;
    }

    nan_de_cancel_service(g_nan_de, service_id);
    nan_usd_clear_peer(service_id);
    return ESP_OK;
}

esp_err_t esp_nan_usd_cancel_service(int service_id)
{
    return eloop_register_timeout_blocking(nan_usd_cancel_service_internal, NULL, &service_id);
}

struct nan_usd_transmit_ctx {
    int handle;
    const uint8_t *ssi;
    uint16_t ssi_len;
    const u8 *peer_addr;
    u8 req_instance_id;
};

static int nan_usd_transmit_internal(void *eloop_data, void *user_ctx)
{
    struct nan_usd_transmit_ctx *ctx = user_ctx;
    struct wpabuf *buf = NULL;
    esp_err_t ret;

    (void)eloop_data;

    if (!g_nan_de || !ctx->peer_addr) {
        return ESP_FAIL;
    }

    if (ctx->ssi && ctx->ssi_len) {
        buf = wpabuf_alloc(ctx->ssi_len);
        if (!buf) {
            wpa_printf(MSG_ERROR, "Allocating memory failed for NaN-USD transmit");
            return ESP_FAIL;
        }
        wpabuf_put_data(buf, ctx->ssi, ctx->ssi_len);
    }

    ret = nan_de_transmit(g_nan_de, ctx->handle, buf, NULL, ctx->peer_addr, ctx->req_instance_id);
    wpabuf_free(buf);
    return ret;
}

esp_err_t esp_nan_usd_transmit(int handle, const uint8_t *ssi, uint16_t ssi_len, const u8 *peer_addr, u8 req_instance_id)
{
    struct nan_usd_transmit_ctx ctx = {
        .handle = handle,
        .ssi = ssi,
        .ssi_len = ssi_len,
        .peer_addr = peer_addr,
        .req_instance_id = req_instance_id,
    };

    return eloop_register_timeout_blocking(nan_usd_transmit_internal, NULL, &ctx);
}

struct nan_usd_get_own_svc_ctx {
    uint8_t *own_svc_id;
    char *svc_name;
    int *num_peer_records;
};

struct nan_usd_get_peer_records_ctx {
    int *num_peer_records;
    uint8_t own_svc_id;
    struct nan_peer_record *peer_record;
};

struct nan_usd_get_peer_info_ctx {
    char *svc_name;
    uint8_t *peer_mac;
    struct nan_peer_record *peer_info;
};

static int nan_usd_get_own_svc_info_internal(void *eloop_data, void *user_ctx)
{
    struct nan_usd_get_own_svc_ctx *ctx = user_ctx;

    (void)eloop_data;

    if (!g_nan_de) {
        return ESP_FAIL;
    }

    if (*ctx->own_svc_id == 0) {
        for (int i = 1; i <= NAN_DE_MAX_SERVICE; i++) {
            const char *name = nan_de_get_service_name(g_nan_de, i);
            if (name && strcmp(name, ctx->svc_name) == 0) {
                *ctx->own_svc_id = i;
                break;
            }
        }
        if (*ctx->own_svc_id == 0) {
            wpa_printf(MSG_ERROR, "NAN-USD: No record found for service name %s", ctx->svc_name);
            return ESP_FAIL;
        }
    } else {
        const char *name = nan_de_get_service_name(g_nan_de, *ctx->own_svc_id);
        if (!name) {
            wpa_printf(MSG_ERROR, "NAN-USD: No record found for service ID %d", *ctx->own_svc_id);
            return ESP_FAIL;
        }
        strlcpy(ctx->svc_name, name, ESP_WIFI_MAX_SVC_NAME_LEN);
    }

    *ctx->num_peer_records = nan_usd_get_peer(*ctx->own_svc_id) ? 1 : 0;
    return ESP_OK;
}

static int nan_usd_get_peer_records_internal(void *eloop_data, void *user_ctx)
{
    struct nan_usd_get_peer_records_ctx *ctx = user_ctx;
    const struct nan_usd_peer *peer;

    (void)eloop_data;

    if (!g_nan_de) {
        return ESP_FAIL;
    }

    if (nan_de_get_service_type(g_nan_de, ctx->own_svc_id) < 0) {
        *ctx->num_peer_records = 0;
        wpa_printf(MSG_DEBUG, "NAN-USD: No service found with id %d", ctx->own_svc_id);
        return ESP_FAIL;
    }

    peer = nan_usd_get_peer(ctx->own_svc_id);
    if (peer) {
        /* USD caches one peer per service. */
        ctx->peer_record[0].peer_svc_id = peer->peer_svc_id;
        ctx->peer_record[0].own_svc_id = ctx->own_svc_id;
        ctx->peer_record[0].peer_svc_type = peer->peer_svc_type;
        os_memcpy(ctx->peer_record[0].peer_nmi, peer->peer_addr, ETH_ALEN);
        ctx->peer_record[0].ndp_id = 0;
        os_memset(ctx->peer_record[0].peer_ndi, 0, ETH_ALEN);
        *ctx->num_peer_records = 1;
    } else {
        *ctx->num_peer_records = 0;
    }

    return ESP_OK;
}

static int nan_usd_get_peer_info_internal(void *eloop_data, void *user_ctx)
{
    struct nan_usd_get_peer_info_ctx *ctx = user_ctx;
    int start = 1, end = NAN_DE_MAX_SERVICE;

    (void)eloop_data;

    if (!g_nan_de) {
        return ESP_FAIL;
    }

    if (ctx->svc_name) {
        bool found = false;
        for (int i = 1; i <= NAN_DE_MAX_SERVICE; i++) {
            const char *name = nan_de_get_service_name(g_nan_de, i);
            if (name && strcmp(name, ctx->svc_name) == 0) {
                start = i;
                end = i;
                found = true;
                break;
            }
        }
        if (!found) {
            wpa_printf(MSG_ERROR, "NAN-USD: No record found for service name %s", ctx->svc_name);
            return ESP_FAIL;
        }
    }

    for (int i = start; i <= end; i++) {
        const struct nan_usd_peer *peer;

        if (nan_de_get_service_type(g_nan_de, i) < 0) {
            continue;
        }

        peer = nan_usd_get_peer(i);
        if (peer && os_memcmp(peer->peer_addr, ctx->peer_mac, ETH_ALEN) == 0) {
            ctx->peer_info->peer_svc_id = peer->peer_svc_id;
            ctx->peer_info->own_svc_id = i;
            ctx->peer_info->peer_svc_type = peer->peer_svc_type;
            os_memcpy(ctx->peer_info->peer_nmi, peer->peer_addr, ETH_ALEN);
            ctx->peer_info->ndp_id = 0;
            os_memset(ctx->peer_info->peer_ndi, 0, ETH_ALEN);
            return ESP_OK;
        }
    }

    wpa_printf(MSG_DEBUG, "NAN-USD: No record found for Peer "MACSTR, MAC2STR(ctx->peer_mac));
    return ESP_FAIL;
}

esp_err_t esp_nan_usd_get_own_svc_info(uint8_t *own_svc_id, char *svc_name, int *num_peer_records)
{
    struct nan_usd_get_own_svc_ctx ctx;

    if (!own_svc_id || !num_peer_records || !svc_name) {
        wpa_printf(MSG_ERROR, "NAN-USD: NULL memory address for input parameters");
        return ESP_FAIL;
    }

    ctx.own_svc_id = own_svc_id;
    ctx.svc_name = svc_name;
    ctx.num_peer_records = num_peer_records;

    return eloop_register_timeout_blocking(nan_usd_get_own_svc_info_internal, NULL, &ctx);
}

esp_err_t esp_nan_usd_get_peer_records(int *num_peer_records, uint8_t own_svc_id, struct nan_peer_record *peer_record)
{
    struct nan_usd_get_peer_records_ctx ctx;

    if (!peer_record || !num_peer_records) {
        wpa_printf(MSG_ERROR, "NAN-USD: NULL memory address for input parameters");
        return ESP_FAIL;
    }
    if (own_svc_id < 1 || own_svc_id > NAN_DE_MAX_SERVICE) {
        wpa_printf(MSG_ERROR, "NAN-USD: Invalid service ID");
        return ESP_FAIL;
    }
    if (*num_peer_records == 0) {
        wpa_printf(MSG_ERROR, "NAN-USD: Number of peer records provided is 0");
        return ESP_FAIL;
    }

    ctx.num_peer_records = num_peer_records;
    ctx.own_svc_id = own_svc_id;
    ctx.peer_record = peer_record;

    return eloop_register_timeout_blocking(nan_usd_get_peer_records_internal, NULL, &ctx);
}

esp_err_t esp_nan_usd_get_peer_info(char *svc_name, uint8_t *peer_mac, struct nan_peer_record *peer_info)
{
    struct nan_usd_get_peer_info_ctx ctx;

    if (!peer_mac || !peer_info) {
        wpa_printf(MSG_ERROR, "NAN-USD: Invalid memory address for input parameters");
        return ESP_FAIL;
    }

    ctx.svc_name = svc_name;
    ctx.peer_mac = peer_mac;
    ctx.peer_info = peer_info;

    return eloop_register_timeout_blocking(nan_usd_get_peer_info_internal, NULL, &ctx);
}
