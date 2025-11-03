/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
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

static void *s_nan_usd_data_lock = NULL;
#define NAN_USD_DATA_LOCK() os_mutex_lock(s_nan_usd_data_lock)
#define NAN_USD_DATA_UNLOCK() os_mutex_unlock(s_nan_usd_data_lock)

#ifdef DEBUG_PRINT
static const char *nan_reason_txt(enum nan_de_reason reason)
{
    switch (reason) {
    case NAN_DE_REASON_TIMEOUT:
        return "timeout";
    case NAN_DE_REASON_USER_REQUEST:
        return "user-request";
    case NAN_DE_REASON_FAILURE:
        return "failure";
    }

    return "unknown";
}
#endif /* DEBUG_PRINT */
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

static void nan_de_tx_event_handler(void *arg, esp_event_base_t event_base,
                                    int32_t event_id, void *event_data)
{
    if (event_id == WIFI_EVENT_ACTION_TX_STATUS) {
        wifi_event_action_tx_status_t *evt = (wifi_event_action_tx_status_t *)event_data;
        if (evt->status == WIFI_ACTION_TX_DONE) {
            int freq = esp_nan_chan_to_freq(evt->channel);
            if (freq == -1) {
                wpa_printf(MSG_ERROR, "Invalid channel received from Action Tx handler");
                return;
            }
            NAN_USD_DATA_LOCK();
            nan_de_tx_status(g_nan_de, freq, NULL);
            NAN_USD_DATA_UNLOCK();
        } else if (evt->status == WIFI_ACTION_TX_DURATION_COMPLETED) {
            NAN_USD_DATA_LOCK();
            nan_de_tx_wait_ended(g_nan_de);
            NAN_USD_DATA_UNLOCK();
        }
    } else if (event_id == WIFI_EVENT_ROC_DONE) {
        wifi_event_roc_done_t *evt = (wifi_event_roc_done_t *)event_data;
        int freq = esp_nan_chan_to_freq(evt->channel);
        if (freq == -1) {
            wpa_printf(MSG_ERROR, "Invalid channel received from ROC done handler");
        }
        NAN_USD_DATA_LOCK();
        nan_de_listen_ended(g_nan_de, freq);
        NAN_USD_DATA_UNLOCK();
    }
}

int esp_nan_de_rx_action(uint8_t *hdr, uint8_t *payload, size_t len, uint8_t channel)
{
    struct ieee80211_hdr *rx_hdr = (struct ieee80211_hdr *)hdr;
    int freq;

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

    uint8_t category = *payload++;
    uint8_t public_action = *payload++;
    uint32_t oui_value = WPA_GET_BE24(payload);
    payload += 3;
    uint8_t oui_type = *payload++;

    if (category == WLAN_ACTION_PUBLIC &&
            public_action == WLAN_PA_VENDOR_SPECIFIC &&
            oui_value == OUI_WFA &&
            oui_type == NAN_OUI_TYPE) {
        /* Received a valid NAN Service Discovery Frame */
    } else {
        /* Frame is not a NAN SDF frame */
        return ESP_FAIL;
    }

    freq = esp_nan_chan_to_freq(channel);
    if (freq == -1) {
        wpa_printf(MSG_ERROR, "Invalid channel from Rx action frame");
        return ESP_FAIL;
    }

    NAN_USD_DATA_LOCK();
    nan_de_rx_sdf(g_nan_de, rx_hdr->addr2, rx_hdr->addr3, freq, payload, len - 6);
    NAN_USD_DATA_UNLOCK();
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

    NAN_USD_DATA_LOCK();
    nan_de_listen_started(g_nan_de, freq, duration);
    NAN_USD_DATA_UNLOCK();
    os_free(req);
    return ESP_OK;
}

static void esp_nan_de_discovery_result(void *ctx, int subscribe_id, enum nan_service_protocol_type srv_proto_type,
                                        const u8 *ssi, size_t ssi_len, int peer_publish_id, const u8 *peer_addr, bool fsd, bool fsd_gas)
{
    wpa_printf(MSG_INFO, "NAN_USD DISCOVERY_RESULT - subscribe_id = %d peer_publish_id = %d peer_address = "MACSTR" service_protocol_type = %d",
               subscribe_id, peer_publish_id, MAC2STR(peer_addr), srv_proto_type);

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
}

static void esp_nan_de_subscribe_terminated(void *ctx, int subscribe_id,
                                            enum nan_de_reason reason)
{
    wpa_printf(MSG_INFO, "NAN_USD SUBSCRIBE_TERMINATED - subscribe_id = %d reason = %s", subscribe_id, nan_reason_txt(reason));
}

static void esp_nan_de_receive(void *ctx, int id, int peer_instance_id,
                               const u8 *ssi, size_t ssi_len,
                               const u8 *peer_addr)
{
    wpa_hexdump(MSG_INFO, "NAN_RECEIVE", ssi, ssi_len);

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

esp_err_t esp_nan_usd_deinit()
{

    NAN_USD_DATA_LOCK();
    if (!g_nan_de) {
        NAN_USD_DATA_UNLOCK();
        return ESP_FAIL;
    }

    nan_de_deinit(g_nan_de);
    g_nan_de = NULL;
    NAN_USD_DATA_UNLOCK();

    if (s_nan_usd_data_lock) {
        os_mutex_delete(s_nan_usd_data_lock);
        s_nan_usd_data_lock = NULL;
    }

    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_ACTION_TX_STATUS, &nan_de_tx_event_handler);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_ROC_DONE, &nan_de_tx_event_handler);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_STOP,
                                 &nan_sta_stop_handler);

    return ESP_OK;
}

esp_err_t esp_nan_usd_init(void)
{
    struct nan_callbacks cb;
    uint8_t mac[ETH_ALEN];
#ifndef ESP_SUPPLICANT
    bool offload = wpa_s->drv_flags2 & WPA_DRIVER_FLAGS2_NAN_OFFLOAD;
#else
    bool offload = false;
#endif
    int max_listen = 1000; // default supplicant value; hardcoded for now

    os_memset(&cb, 0, sizeof(cb));
    cb.ctx = NULL;
    cb.tx = esp_nan_de_tx;
    cb.listen = esp_nan_de_listen;
    cb.discovery_result = esp_nan_de_discovery_result;
    cb.replied = esp_nan_de_replied;
    cb.publish_terminated = esp_nan_de_publish_terminated;
    cb.subscribe_terminated = esp_nan_de_subscribe_terminated;
    cb.receive = esp_nan_de_receive;

    s_nan_usd_data_lock = os_recursive_mutex_create();
    if (!s_nan_usd_data_lock) {
        ESP_LOGE("NAN-USD", "Failed to create NAN-USD data lock");
        return  ESP_FAIL;
    }

    ESP_RETURN_ON_ERROR(esp_wifi_get_mac(WIFI_IF_STA, mac), "NAN-USD", "Fetching MAC of STA ifx failed");
    g_nan_de = nan_de_init(mac, offload, false, max_listen, &cb);
    if (!g_nan_de) {
        return ESP_FAIL;
    }
    esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_ACTION_TX_STATUS,
                               &nan_de_tx_event_handler, NULL);
    esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_ROC_DONE,
                               &nan_de_tx_event_handler, NULL);
    esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_STOP,
                               &nan_sta_stop_handler, NULL);

    return ESP_OK;
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

    NAN_USD_DATA_LOCK();
    if (!g_nan_de) {
        goto fail;
    }

    pub_params.ttl = ttl;
    if (ssi && ssi_len) {
        buf = wpabuf_alloc(ssi_len);
        if (!buf) {
            wpa_printf(MSG_ERROR, "Allocating memory failed for NaN-USD Publish");
            goto fail;
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
            goto fail;
        }
    }

    publish_id = nan_de_publish(g_nan_de, service_name, srv_proto_type,
                                buf, NULL, &pub_params, p2p);

    wpabuf_free(buf);
    if (freq_list) {
        os_free(freq_list);
    }
    NAN_USD_DATA_UNLOCK();
    return publish_id;
fail:
    NAN_USD_DATA_UNLOCK();
    return -1;
}

int esp_nan_usd_publish(const wifi_nan_publish_cfg_t *publish_cfg)
{
    return esp_nan_usd_publish_internal(publish_cfg->service_name, WIFI_SVC_PROTO_RESERVED,
                                        publish_cfg->ttl, publish_cfg->ssi,
                                        publish_cfg->ssi_len, publish_cfg->usd_publish_config.usd_default_channel,
                                        publish_cfg->usd_publish_config.usd_chan_bitmap);
}

esp_err_t esp_nan_usd_update_publish(int publish_id, uint8_t *ssi, uint16_t ssi_len)
{
    struct wpabuf *buf = NULL;
    esp_err_t ret;

    NAN_USD_DATA_LOCK();
    if (!g_nan_de) {
        goto fail;
    }

    if (ssi && ssi_len) {
        buf = wpabuf_alloc(ssi_len);
        if (!buf) {
            wpa_printf(MSG_ERROR, "Allocating memory failed for NaN-USD Update Publish");
            goto fail;
        }
        wpabuf_put_data(buf, ssi, ssi_len);
    }

    ret = nan_de_update_publish(g_nan_de, publish_id, buf);
    wpabuf_free(buf);
    NAN_USD_DATA_UNLOCK();
    return ret;
fail:
    NAN_USD_DATA_UNLOCK();
    return ESP_FAIL;
}

esp_err_t esp_nan_usd_cancel_publish(int publish_id)
{
    NAN_USD_DATA_LOCK();
    if (!g_nan_de) {
        NAN_USD_DATA_UNLOCK();
        return ESP_FAIL;
    }

    nan_de_cancel_publish(g_nan_de, publish_id);
    NAN_USD_DATA_UNLOCK();
    return ESP_OK;
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

    NAN_USD_DATA_LOCK();
    if (!g_nan_de) {
        goto fail;
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
            goto fail;
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
            goto fail;
        }
    }

    subscribe_id = nan_de_subscribe(g_nan_de, service_name, srv_proto_type, buf, NULL, &sub_params, p2p);

    wpabuf_free(buf);
    if (freq_list) {
        os_free(freq_list);
    }
    NAN_USD_DATA_UNLOCK();
    return subscribe_id;
fail:
    NAN_USD_DATA_UNLOCK();
    return -1;
}

int esp_nan_usd_subscribe(const wifi_nan_subscribe_cfg_t *subscribe_cfg)
{
    return esp_nan_usd_subscribe_internal(subscribe_cfg->service_name, WIFI_SVC_PROTO_RESERVED, subscribe_cfg->ttl,
                                          subscribe_cfg->usd_subscribe_config.usd_default_channel, subscribe_cfg->ssi, subscribe_cfg->ssi_len,
                                          subscribe_cfg->usd_subscribe_config.usd_chan_bitmap);
}

esp_err_t esp_nan_usd_cancel_subscribe(int subscribe_id)
{
    NAN_USD_DATA_LOCK();
    if (!g_nan_de) {
        NAN_USD_DATA_UNLOCK();
        return ESP_FAIL;
    }

    nan_de_cancel_subscribe(g_nan_de, subscribe_id);
    NAN_USD_DATA_UNLOCK();
    return ESP_OK;
}

esp_err_t esp_nan_usd_cancel_service(int service_id)
{
    NAN_USD_DATA_LOCK();
    if (!g_nan_de) {
        NAN_USD_DATA_UNLOCK();
        return ESP_FAIL;
    }

    nan_de_cancel_service(g_nan_de, service_id);
    NAN_USD_DATA_UNLOCK();
    return ESP_OK;
}

esp_err_t esp_nan_usd_transmit(int handle, const uint8_t *ssi, uint16_t ssi_len, const u8 *peer_addr, u8 req_instance_id)
{
    struct wpabuf *buf = NULL;
    esp_err_t ret;

    NAN_USD_DATA_LOCK();
    if (!g_nan_de || !peer_addr) {
        goto fail;
    }

    if (ssi && ssi_len) {
        buf = wpabuf_alloc(ssi_len);
        if (!buf) {
            wpa_printf(MSG_ERROR, "Allocating memory failed for NaN-USD transmit");
            goto fail;
        }
        wpabuf_put_data(buf, ssi, ssi_len);
    }

    ret = nan_de_transmit(g_nan_de, handle, buf, NULL, peer_addr, req_instance_id);

    wpabuf_free(buf);
    NAN_USD_DATA_UNLOCK();
    return ret;
fail:
    NAN_USD_DATA_UNLOCK();
    return ESP_FAIL;
}
