/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_wifi_types.h"
#include "utils/common.h"
#include <sys/time.h>
#ifdef __cplusplus
extern "C" {
#endif

#define SUPPLICANT_CANDIDATE_LIST_EXPIRY 10

/* Global Roaming Configuration */
#define ROAMING_BACKOFF_TIME CONFIG_ESP_WIFI_ROAMING_BACKOFF_TIME

/* Low RSSI based roaming configuration */
#define LOW_RSSI_ROAMING_ENABLED CONFIG_ESP_WIFI_ROAMING_LOW_RSSI_ROAMING
#if LOW_RSSI_ROAMING_ENABLED
#define ROAMING_LOW_RSSI_THRESHOLD CONFIG_ESP_WIFI_ROAMING_LOW_RSSI_THRESHOLD
#define RSSI_THRESHOLD_REDUCTION_OFFSET CONFIG_ESP_WIFI_ROAMING_LOW_RSSI_OFFSET
#endif /*LOW_RSSI_ROAMING_ENABLED*/

/* Periodic Scan based Roaming configuration */
#define PERIODIC_SCAN_MONITORING CONFIG_ESP_WIFI_ROAMING_PERIODIC_SCAN_MONITOR
#if PERIODIC_SCAN_MONITORING
#define SCAN_MONITOR_INTERVAL CONFIG_ESP_WIFI_ROAMING_SCAN_MONITOR_INTERVAL
#define SCAN_MONITOR_RSSI_THRESHOLD CONFIG_ESP_WIFI_ROAMING_PERIODIC_SCAN_THRESHOLD
#define SCAN_ROAM_RSSI_DIFF CONFIG_ESP_WIFI_ROAMING_SCAN_ROAM_RSSI_DIFF
#endif /* PERIODIC_SCAN_MONITORING */

/* Scan configuration */
#define SCAN_TIME_MIN_DURATION CONFIG_ESP_WIFI_ROAMING_SCAN_MIN_SCAN_TIME
#define SCAN_TIME_MAX_DURATION CONFIG_ESP_WIFI_ROAMING_SCAN_MAX_SCAN_TIME
#define HOME_CHANNEL_DWELL_TIME CONFIG_ESP_WIFI_ROAMING_HOME_CHANNEL_DWELL_TIME
#define SCAN_PREFERRED_CHAN_LIST CONFIG_ESP_WIFI_ROAMING_SCAN_CHAN_LIST
#define DEFAULT_PREFERRED_SCAN_CHAN_LIST "None"
#define SCAN_RESULTS_USABILITY_WINDOW CONFIG_ESP_WIFI_ROAMING_SCAN_EXPIRY_WINDOW
#define MAX_CANDIDATE_COUNT CONFIG_ESP_WIFI_ROAMING_MAX_CANDIDATES

/* Legacy roaming configuration */
#define LEGACY_ROAM_ENABLED CONFIG_ESP_WIFI_ROAMING_LEGACY_ROAMING

#define BSS_TM_RETRY_COUNT CONFIG_ESP_WIFI_NETWORK_ASSISTED_ROAMING_RETRY_COUNT

/* Network Assisted Roaming */
#define NETWORK_ASSISTED_ROAMING_ENABLED CONFIG_ESP_WIFI_ROAMING_NETWORK_ASSISTED_ROAM

/* Periodic RRM configuration */
#define PERIODIC_RRM_MONITORING CONFIG_ESP_WIFI_ROAMING_PERIODIC_RRM_MONITORING
#if PERIODIC_RRM_MONITORING
#define RRM_MONITOR_TIME CONFIG_ESP_WIFI_ROAMING_RRM_MONITOR_TIME
#define RRM_MONITOR_RSSI_THRESHOLD CONFIG_ESP_WIFI_ROAMING_RRM_MONITOR_THRESHOLD
#endif /*PERIODIC_RRM_MONITORING*/

#define MAX_SCAN_CHAN_LIST_COUNT 14

#define MAX_NEIGHBOR_LEN 512

#define IS_PSK(authmode) \
    (((authmode == WIFI_AUTH_WPA_PSK) || (authmode == WIFI_AUTH_WPA2_PSK) || \
    (authmode == WIFI_AUTH_WPA_WPA2_PSK) || (authmode == WIFI_AUTH_WPA3_PSK) || \
    (authmode == WIFI_AUTH_WPA2_WPA3_PSK) || (authmode == WIFI_AUTH_WAPI_PSK) ? 1 : 0))

#define OWE_COMPATIBLE(curr_auth, cand_auth) \
    ((((curr_auth == WIFI_AUTH_OPEN) || (curr_auth == WIFI_AUTH_OWE)) && ((cand_auth == WIFI_AUTH_OPEN) || (cand_auth == WIFI_AUTH_OWE)))? 1 : 0)

#define PSK_COMPATIBLE(curr_auth, cand_auth) \
    ((IS_PSK(curr_auth) && IS_PSK(cand_auth)) ? 1 : 0)

struct scanned_ap_info {
    uint16_t current_count;
    struct timeval time;
    wifi_ap_record_t ap_records[MAX_CANDIDATE_COUNT];
};
struct cand_bss {
    uint8_t channel;
    uint8_t bssid[ETH_ALEN];
};

struct roaming_app {
    wifi_scan_config_t scan_params;
    bool scan_ongoing;
    int8_t current_rssi_threshold;
    char *btm_neighbor_list;
    struct timeval last_roamed_time;
    wifi_ap_record_t ap_info;
    struct scanned_ap_info scanned_aps;
    bool btm_support;
    bool rrm_support;

#if LOW_RSSI_ROAMING_ENABLED
    int8_t current_low_rssi_threshold;
#endif
#if LEGACY_ROAM_ENABLED && NETWORK_ASSISTED_ROAMING_ENABLED
    uint8_t btm_attempt;
#endif
#if LEGACY_ROAM_ENABLED
    bool force_roam_ongoing;
#endif
#if PERIODIC_RRM_MONITORING
    bool periodic_rrm_active;
    bool rrm_request_active;
#endif
#if PERIODIC_SCAN_MONITORING
    bool periodic_scan_active;
#endif
    bool allow_reconnect;
};

void init_roaming_app(void);
void deinit_roaming_app(void);

#if PERIODIC_RRM_MONITORING
void roaming_app_periodic_rrm_internal_handler(void *data, void *ctx);
#endif /*PERIODIC_RRM_MONITORING*/

#if PERIODIC_SCAN_MONITORING
void roaming_app_periodic_scan_internal_handler(void *data, void *ctx);
#endif /*PERIODIC_SCAN_ROAM_MONITORING*/

void roaming_app_trigger_roam_internal_handler(void *data, void *ctx);

void roaming_app_disable_reconnect(void);
void roaming_app_enable_reconnect(void);
#ifdef __cplusplus
}
#endif
