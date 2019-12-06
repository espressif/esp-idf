// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "esp_event.h"
#include "esp_log.h"
#include "esp_event_legacy.h"
#include "esp_wifi_types.h"
#include "esp_netif.h"
#if CONFIG_ETH_ENABLED
#include "esp_eth.h"
#endif

/**
 * The purpose of this file is to provide an "esp_event_send_to_default_loop"
 * function, which is used to forward legacy events (system_event_t) sent using
 * esp_event_send, to the new default event loop (esp_event_post).
 *
 * For each of the events in system_event_id_t, we extract the event data from
 * the corresponding system_event_info_t member, and forward that to
 * esp_event_post function.
 *
 * Some macros are used to reduce the amount of boilerplate.
 *
 * Note that this function only needs to be included into the output file if
 * the new default event loop is used. This function is in a separate file for
 * readability reasons. In order to be linked if the contents of
 * default_event_loop.c is linked, this file is #include-ed into default_event_loop.c.
 */

#if LOG_LOCAL_LEVEL >= 4 /* ESP_LOG_DEBUG */
#define WITH_EVENT_DEBUG
#endif

#ifdef WITH_EVENT_DEBUG
static void esp_system_event_debug(const system_event_t* event);
#endif

#define HANDLE_SYS_EVENT(base_, name_) \
        case SYSTEM_EVENT_ ## name_: \
            return esp_event_post(base_ ## _EVENT, base_ ## _EVENT_ ## name_, \
                    NULL, 0, send_timeout)

#define HANDLE_SYS_EVENT_ARG(base_, name_, member_) \
        case SYSTEM_EVENT_ ## name_: \
            return esp_event_post(base_ ## _EVENT, base_ ## _EVENT_ ## name_, \
                    &event->event_info.member_, sizeof(event->event_info.member_), \
                    send_timeout)

esp_err_t esp_event_send_to_default_loop(system_event_t *event)
{
#ifdef WITH_EVENT_DEBUG
    esp_system_event_debug(event);
#endif // WITH_EVENT_DEBUG

    const TickType_t send_timeout = 0;
    switch (event->event_id) {
        /* Wi-Fi common events */
        HANDLE_SYS_EVENT(WIFI, WIFI_READY);
        HANDLE_SYS_EVENT_ARG(WIFI, SCAN_DONE, scan_done);
        HANDLE_SYS_EVENT(WIFI, STA_START);
        HANDLE_SYS_EVENT(WIFI, STA_STOP);

        /* STA events */
        HANDLE_SYS_EVENT_ARG(WIFI, STA_CONNECTED, connected);
        HANDLE_SYS_EVENT_ARG(WIFI, STA_DISCONNECTED, disconnected);
        HANDLE_SYS_EVENT_ARG(WIFI, STA_AUTHMODE_CHANGE, auth_change);

        /* WPS events */
        HANDLE_SYS_EVENT(WIFI, STA_WPS_ER_SUCCESS);
        HANDLE_SYS_EVENT(WIFI, STA_WPS_ER_TIMEOUT);
        HANDLE_SYS_EVENT_ARG(WIFI, STA_WPS_ER_FAILED, sta_er_fail_reason);
        HANDLE_SYS_EVENT_ARG(WIFI, STA_WPS_ER_PIN, sta_er_pin);
        HANDLE_SYS_EVENT(WIFI, STA_WPS_ER_PBC_OVERLAP);

        /* AP events */
        HANDLE_SYS_EVENT(WIFI, AP_START);
        HANDLE_SYS_EVENT(WIFI, AP_STOP);
        HANDLE_SYS_EVENT_ARG(WIFI, AP_STACONNECTED, sta_connected);
        HANDLE_SYS_EVENT_ARG(WIFI, AP_STADISCONNECTED, sta_disconnected);
        HANDLE_SYS_EVENT_ARG(WIFI, AP_PROBEREQRECVED, ap_probereqrecved);
#if CONFIG_ETH_ENABLED
        /* Ethernet events */
        /* Some extra defines to fit the old naming scheme... */
#define ETH_EVENT_ETH_START ETHERNET_EVENT_START
#define ETH_EVENT_ETH_STOP ETHERNET_EVENT_STOP
#define ETH_EVENT_ETH_CONNECTED ETHERNET_EVENT_CONNECTED
#define ETH_EVENT_ETH_DISCONNECTED ETHERNET_EVENT_DISCONNECTED

        HANDLE_SYS_EVENT(ETH, ETH_START);
        HANDLE_SYS_EVENT(ETH, ETH_STOP);
        HANDLE_SYS_EVENT(ETH, ETH_CONNECTED);
        HANDLE_SYS_EVENT(ETH, ETH_DISCONNECTED);
#endif
        /* IP events */
        HANDLE_SYS_EVENT_ARG(IP, STA_GOT_IP, got_ip);
        HANDLE_SYS_EVENT_ARG(IP, ETH_GOT_IP, got_ip);
        HANDLE_SYS_EVENT(IP, STA_LOST_IP);
        HANDLE_SYS_EVENT_ARG(IP, GOT_IP6, got_ip6);
        HANDLE_SYS_EVENT(IP, AP_STAIPASSIGNED);
        default:
            return ESP_ERR_NOT_SUPPORTED;
    }
}

#ifdef WITH_EVENT_DEBUG

static const char* TAG = "system_event";

typedef struct {
    int err;
    const char *reason;
} wifi_reason_t;

static const wifi_reason_t wifi_reason[] =
{
    {0,                                    "other reason"},
    {WIFI_REASON_UNSPECIFIED,              "unspecified"},
    {WIFI_REASON_AUTH_EXPIRE,              "auth expire"},
    {WIFI_REASON_AUTH_LEAVE,               "auth leave"},
    {WIFI_REASON_ASSOC_EXPIRE,             "assoc expire"},
    {WIFI_REASON_ASSOC_TOOMANY,            "assoc too many"},
    {WIFI_REASON_NOT_AUTHED,               "not authed"},
    {WIFI_REASON_NOT_ASSOCED,              "not assoced"},
    {WIFI_REASON_ASSOC_LEAVE,              "assoc leave"},
    {WIFI_REASON_ASSOC_NOT_AUTHED,         "assoc not authed"},
    {WIFI_REASON_BEACON_TIMEOUT,           "beacon timeout"},
    {WIFI_REASON_NO_AP_FOUND,              "no ap found"},
    {WIFI_REASON_AUTH_FAIL,                "auth fail"},
    {WIFI_REASON_ASSOC_FAIL,               "assoc fail"},
    {WIFI_REASON_HANDSHAKE_TIMEOUT,        "hanshake timeout"},
    {WIFI_REASON_DISASSOC_PWRCAP_BAD,      "bad Power Capability, disassoc"},
    {WIFI_REASON_DISASSOC_SUPCHAN_BAD,     "bad Supported Channels, disassoc"},
    {WIFI_REASON_IE_INVALID,               "invalid IE"},
    {WIFI_REASON_MIC_FAILURE,              "MIC failure"},
    {WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT,   "4-way keying handshake timeout"},
    {WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT, "Group key handshake"},
    {WIFI_REASON_IE_IN_4WAY_DIFFERS,       "IE in 4-way differs"},
    {WIFI_REASON_GROUP_CIPHER_INVALID,     "invalid group cipher"},
    {WIFI_REASON_PAIRWISE_CIPHER_INVALID,  "invalid pairwise cipher"},
    {WIFI_REASON_AKMP_INVALID,             "invalid AKMP"},
    {WIFI_REASON_UNSUPP_RSN_IE_VERSION,    "unsupported RSN IE version"},
    {WIFI_REASON_INVALID_RSN_IE_CAP,       "invalid RSN IE capability"},
    {WIFI_REASON_802_1X_AUTH_FAILED,       "802.1x auth failed"},
    {WIFI_REASON_CIPHER_SUITE_REJECTED,    "cipher suite rejected"}
};

static const char* wifi_disconnect_reason_to_str(int err)
{
    for (int i=0; i< sizeof(wifi_reason)/sizeof(wifi_reason[0]); i++){
        if (err == wifi_reason[i].err){
            return wifi_reason[i].reason;
        }
    }
    return wifi_reason[0].reason;
}

static void esp_system_event_debug(const system_event_t* event)
{
    if (event == NULL) {
        return;
    }

    switch (event->event_id) {
        case SYSTEM_EVENT_WIFI_READY: {
            ESP_LOGD(TAG, "SYSTEM_EVENT_WIFI_READY");
            break;
        }
        case SYSTEM_EVENT_SCAN_DONE: {
            const system_event_sta_scan_done_t *scan_done = &event->event_info.scan_done;
            ESP_LOGD(TAG, "SYSTEM_EVENT_SCAN_DONE, status:%d, number:%d",  scan_done->status, scan_done->number);
            break;
        }
        case SYSTEM_EVENT_STA_START: {
            ESP_LOGD(TAG, "SYSTEM_EVENT_STA_START");
            break;
        }
        case SYSTEM_EVENT_STA_STOP: {
            ESP_LOGD(TAG, "SYSTEM_EVENT_STA_STOP");
            break;
        }
        case SYSTEM_EVENT_STA_CONNECTED: {
            const system_event_sta_connected_t *connected = &event->event_info.connected;
            ESP_LOGD(TAG, "SYSTEM_EVENT_STA_CONNECTED, ssid:%s, ssid_len:%d, bssid:" MACSTR ", channel:%d, authmode:%d", \
                       connected->ssid, connected->ssid_len, MAC2STR(connected->bssid), connected->channel, connected->authmode);
            break;
        }
        case SYSTEM_EVENT_STA_DISCONNECTED: {
            const system_event_sta_disconnected_t *disconnected = &event->event_info.disconnected;
            ESP_LOGD(TAG, "SYSTEM_EVENT_STA_DISCONNECTED, ssid:%s, ssid_len:%d, bssid:" MACSTR ", reason:%d (%s)", \
                       disconnected->ssid, disconnected->ssid_len, MAC2STR(disconnected->bssid), disconnected->reason,
                       wifi_disconnect_reason_to_str(disconnected->reason));
            break;
        }
        case SYSTEM_EVENT_STA_AUTHMODE_CHANGE: {
            const system_event_sta_authmode_change_t *auth_change = &event->event_info.auth_change;
            ESP_LOGD(TAG, "SYSTEM_EVENT_STA_AUTHMODE_CHNAGE, old_mode:%d, new_mode:%d", auth_change->old_mode, auth_change->new_mode);
            break;
        }
        case SYSTEM_EVENT_STA_GOT_IP: {
            const system_event_sta_got_ip_t *got_ip = &event->event_info.got_ip;
            ESP_LOGD(TAG, "SYSTEM_EVENT_STA_GOT_IP, ip:" IPSTR ", mask:" IPSTR ", gw:" IPSTR,
                IP2STR(&got_ip->ip_info.ip),
                IP2STR(&got_ip->ip_info.netmask),
                IP2STR(&got_ip->ip_info.gw));
            break;
        }
        case SYSTEM_EVENT_STA_LOST_IP: {
            ESP_LOGD(TAG, "SYSTEM_EVENT_STA_LOST_IP");
            break;
        }
        case SYSTEM_EVENT_STA_WPS_ER_SUCCESS: {
            ESP_LOGD(TAG, "SYSTEM_EVENT_STA_WPS_ER_SUCCESS");
            break;
        }
        case SYSTEM_EVENT_STA_WPS_ER_FAILED: {
            ESP_LOGD(TAG, "SYSTEM_EVENT_STA_WPS_ER_FAILED");
            break;
        }
        case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT: {
            ESP_LOGD(TAG, "SYSTEM_EVENT_STA_WPS_ER_TIMEOUT");
            break;
        }
        case SYSTEM_EVENT_STA_WPS_ER_PIN: {
            ESP_LOGD(TAG, "SYSTEM_EVENT_STA_WPS_ER_PIN");
            break;
        }
        case SYSTEM_EVENT_STA_WPS_ER_PBC_OVERLAP: {
            ESP_LOGD(TAG, "SYSTEM_EVENT_STA_WPS_ER_PBC_OVERLAP");
            break;
        }
        case SYSTEM_EVENT_AP_START: {
            ESP_LOGD(TAG, "SYSTEM_EVENT_AP_START");
            break;
        }
        case SYSTEM_EVENT_AP_STOP: {
            ESP_LOGD(TAG, "SYSTEM_EVENT_AP_STOP");
            break;
        }
        case SYSTEM_EVENT_AP_STACONNECTED: {
            const system_event_ap_staconnected_t *staconnected = &event->event_info.sta_connected;
            ESP_LOGD(TAG, "SYSTEM_EVENT_AP_STACONNECTED, mac:" MACSTR ", aid:%d", \
                       MAC2STR(staconnected->mac), staconnected->aid);
            break;
        }
        case SYSTEM_EVENT_AP_STADISCONNECTED: {
            const system_event_ap_stadisconnected_t *stadisconnected = &event->event_info.sta_disconnected;
            ESP_LOGD(TAG, "SYSTEM_EVENT_AP_STADISCONNECTED, mac:" MACSTR ", aid:%d", \
                       MAC2STR(stadisconnected->mac), stadisconnected->aid);
            break;
        }
        case SYSTEM_EVENT_AP_STAIPASSIGNED: {
            ESP_LOGD(TAG, "SYSTEM_EVENT_AP_STAIPASSIGNED");
            break;
        }
        case SYSTEM_EVENT_AP_PROBEREQRECVED: {
            const system_event_ap_probe_req_rx_t *ap_probereqrecved = &event->event_info.ap_probereqrecved;
            ESP_LOGD(TAG, "SYSTEM_EVENT_AP_PROBEREQRECVED, rssi:%d, mac:" MACSTR, \
                       ap_probereqrecved->rssi, \
                       MAC2STR(ap_probereqrecved->mac));
            break;
        }
        case SYSTEM_EVENT_GOT_IP6: {
            const esp_ip6_addr_t *addr = &event->event_info.got_ip6.ip6_info.ip;
            ESP_LOGD(TAG, "SYSTEM_EVENT_AP_STA_GOT_IP6 address " IPV6STR, IPV62STR(*addr));
            break;
        }
#if CONFIG_IDF_TARGET_ESP32
        case SYSTEM_EVENT_ETH_START: {
            ESP_LOGD(TAG, "SYSTEM_EVENT_ETH_START");
            break;
        }
        case SYSTEM_EVENT_ETH_STOP: {
            ESP_LOGD(TAG, "SYSTEM_EVENT_ETH_STOP");
            break;
        }
        case SYSTEM_EVENT_ETH_CONNECTED: {
            ESP_LOGD(TAG, "SYSTEM_EVENT_ETH_CONNECETED");
            break;
        }
        case SYSTEM_EVENT_ETH_DISCONNECTED: {
            ESP_LOGD(TAG, "SYSTEM_EVENT_ETH_DISCONNECETED");
            break;
        }
        case SYSTEM_EVENT_ETH_GOT_IP: {
            const system_event_sta_got_ip_t *got_ip = &event->event_info.got_ip;
            ESP_LOGD(TAG, "SYSTEM_EVENT_ETH_GOT_IP, ip:" IPSTR ", mask:" IPSTR ", gw:" IPSTR,
                IP2STR(&got_ip->ip_info.ip),
                IP2STR(&got_ip->ip_info.netmask),
                IP2STR(&got_ip->ip_info.gw));
            break;
        }
#endif
        default: {
            ESP_LOGW(TAG, "unexpected system event %d!", event->event_id);
            break;
        }
    }
}

#endif // WITH_EVENT_DEBUG
