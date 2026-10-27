/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_wifi.h"
#include "esp_wifi_roaming.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ROAM_SCAN_FILTER_SSID_LEN (MAX_SSID_LEN + 1)
#define ROAM_SCAN_FILTER_BSSID_LEN 6

struct roam_config {
    uint8_t backoff_time;
    bool low_rssi_roam_trigger;
    int8_t low_rssi_threshold;
    uint8_t rssi_threshold_reduction_offset;
    bool scan_monitor;
#if CONFIG_ESP_WIFI_ROAMING_PERIODIC_SCAN_MONITOR
    uint8_t scan_interval;
    int8_t scan_rssi_threshold;
    uint8_t scan_rssi_diff;
#endif
    bool legacy_roam_enabled;
    uint8_t btm_retry_cnt;
    bool btm_roaming_enabled;
    bool rrm_monitor;
    uint8_t rrm_monitor_time;
    int8_t rrm_monitor_rssi_threshold;
    wifi_scan_config_t scan_config;
    uint8_t scan_filter_ssid[ROAM_SCAN_FILTER_SSID_LEN];
    uint8_t scan_filter_bssid[ROAM_SCAN_FILTER_BSSID_LEN];
    bool scan_filter_bssid_set;
    uint8_t low_rssi_roam_diff;
    /* Appended so the members above keep their offsets. Each takes effect only when its option is built in. */
    bool fast_connect;            /**< remember the AP and connect to it directly (ESP_WIFI_ROAMING_FAST_CONNECT) */
    bool join_check;              /**< scan once after each connection (ESP_WIFI_ROAMING_JOIN_CHECK) */
    uint8_t join_check_rssi_diff; /**< dB a candidate must beat the current AP by, for that check (1 to 99) */
};

void roam_init_app(void);
void roam_deinit_app(void);
void roam_disable_reconnect(void);
void roam_enable_reconnect(void);
void roam_sta_connected(void);
void roam_sta_disconnected(void *disconn);
esp_err_t roam_get_config_params(struct roam_config *config);
esp_err_t roam_set_config_params(struct roam_config *config);
#if CONFIG_ESP_WIFI_ROAMING_BSSID_BLACKLIST
esp_err_t esp_wifi_blacklist_add(const uint8_t *bssid);
esp_err_t esp_wifi_blacklist_remove(const uint8_t *bssid);
#endif

#if CONFIG_ESP_WIFI_ENABLE_ROAMING_APP
void esp_wifi_roaming_set_current_bssid(const uint8_t *bssid);
/* For the supplicant: true while fast connect keeps its record of the AP as the station configuration's
 * connect hint, which the supplicant then leaves in place when BTM or MBO is enabled. */
bool roam_fast_connect_owns_connect_hint(void);
/* For the supplicant's connection and disconnection callbacks, on the Wi-Fi task: fast connect clears its
 * record after a failed attempt there, before the driver posts WIFI_EVENT_STA_DISCONNECTED. */
void roam_fast_connect_sta_connected(void);
void roam_fast_connect_sta_disconnected(uint8_t reason);
#endif

#ifdef __cplusplus
}
#endif
