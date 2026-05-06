/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdarg.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_wnm.h"
#include "esp_rrm.h"
#include "esp_mbo.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "nvs_flash.h"
#include <sys/time.h>
#include "regex.h"
#include <stdio.h>
#include "esp_roaming.h"
#include "esp_roaming_i.h"
#include "utils/common.h"
#include "esp_wifi_driver.h"
#include "utils/eloop.h"
#include "rom/ets_sys.h"
#include "common/ieee802_11_common.h"
#include "common/ieee802_11_defs.h"
#include "common/wpa_common.h"
#include "esp_wpas_glue.h"

static struct roaming_app g_roaming_app;
extern bool current_task_is_wifi_task(void);

typedef void (* scan_done_cb_t)(void *arg, ETS_STATUS status);
extern int esp_wifi_promiscuous_scan_start(wifi_scan_config_t *config, scan_done_cb_t cb);

static int wifi_post_roam_event(struct cand_bss *bss);
static void determine_best_ap(int8_t rssi_threshold);
static bool is_bssid_blacklisted(const uint8_t *bssid);
static void roaming_app_refresh_scan_config_filters(void);
static void roaming_app_apply_runtime_config(void);
static void roaming_app_invalidate_scan_cache(void);
static esp_err_t roaming_app_run_blocking(eloop_blocking_timeout_handler handler, void *user_ctx);
static void roaming_app_connected_event_handler(void *ctx, void *data);
static void roaming_app_disconnected_event_handler(void *ctx, void *data);
static void roaming_app_trigger_roam_internal_handler(void *ctx, void *data);
#if PERIODIC_RRM_MONITORING
static void init_periodic_rrm_event(void);
static void roaming_app_periodic_rrm_internal_handler(void *data, void *ctx);
static void roaming_app_neighbor_report_recv_internal_handler(void *ctx, void *data);
#endif
#if PERIODIC_SCAN_MONITORING
static void init_periodic_scan_roam_event(void);
static void roaming_app_periodic_scan_internal_handler(void *data, void *ctx);
#endif
#if LOW_RSSI_ROAMING_ENABLED
static void roaming_app_rssi_low_internal_handler(void *ctx, void *data);
#endif
#if CONFIG_ESP_WIFI_ROAMING_BSSID_BLACKLIST
static void roaming_app_blacklist_add_handler(void *ctx, void *data);
static void roaming_app_blacklist_remove_handler(void *ctx, void *data);
#endif

static const char *ROAMING_TAG = "ROAM";

#define RSSI_THRESHOLD_DISABLED -100
#define RSSI_THRESHOLD_MAX 10
#define BTM_QUERY_LIST_MAX_LEN (MAX_NEIGHBOR_LEN + 96)
#define ROAMING_PENDING_TIMEOUT_USER_DATA_MAX 16

static void *s_pending_timeout_user_data[ROAMING_PENDING_TIMEOUT_USER_DATA_MAX];

static void roaming_app_track_timeout_user_data(void *user_data)
{
    if (!user_data) {
        return;
    }

    for (size_t i = 0; i < ROAMING_PENDING_TIMEOUT_USER_DATA_MAX; i++) {
        if (s_pending_timeout_user_data[i] == user_data) {
            return;
        }
    }

    for (size_t i = 0; i < ROAMING_PENDING_TIMEOUT_USER_DATA_MAX; i++) {
        if (!s_pending_timeout_user_data[i]) {
            s_pending_timeout_user_data[i] = user_data;
            return;
        }
    }

    ESP_LOGW(ROAMING_TAG, "Timeout user_data tracking overflow; deinit may leak pending event payload");
}

static void roaming_app_untrack_timeout_user_data(void *user_data)
{
    if (!user_data) {
        return;
    }

    for (size_t i = 0; i < ROAMING_PENDING_TIMEOUT_USER_DATA_MAX; i++) {
        if (s_pending_timeout_user_data[i] == user_data) {
            s_pending_timeout_user_data[i] = NULL;
            return;
        }
    }
}

static void roaming_app_free_tracked_timeout_user_data(void)
{
    for (size_t i = 0; i < ROAMING_PENDING_TIMEOUT_USER_DATA_MAX; i++) {
        if (s_pending_timeout_user_data[i]) {
            os_free(s_pending_timeout_user_data[i]);
            s_pending_timeout_user_data[i] = NULL;
        }
    }
}

void esp_wifi_roaming_set_current_bssid(const uint8_t *bssid)
{
    if (bssid) {
        memcpy(g_roaming_app.current_bss.ap.bssid, bssid, ETH_ALEN);
    }
}

static inline long time_diff_sec(const struct timeval *a, const struct timeval *b)
{
    return (a->tv_sec - b->tv_sec);
}

static int32_t roaming_app_clamp_rssi_threshold(int threshold)
{
    if (threshold < RSSI_THRESHOLD_DISABLED) {
        return RSSI_THRESHOLD_DISABLED;
    }

    if (threshold > RSSI_THRESHOLD_MAX) {
        return RSSI_THRESHOLD_MAX;
    }

    return threshold;
}

static bool roaming_app_scan_cache_is_valid(const struct timeval *now)
{
    if (g_roaming_app.scanned_aps.time.tv_sec == 0 && g_roaming_app.scanned_aps.time.tv_usec == 0) {
        return false;
    }

    return time_diff_sec(now, &g_roaming_app.scanned_aps.time) <= SCAN_RESULTS_USABILITY_WINDOW;
}

static const wifi_ap_record_t *roaming_app_find_scan_record(const uint8_t *bssid)
{
    for (uint16_t i = 0; i < g_roaming_app.scanned_aps.current_count; i++) {
        if (memcmp(g_roaming_app.scanned_aps.ap_records[i].bssid, bssid, ETH_ALEN) == 0) {
            return &g_roaming_app.scanned_aps.ap_records[i];
        }
    }

    return NULL;
}

static uint8_t roaming_app_get_phy_type(const wifi_ap_record_t *record)
{
    int sec_chan = 0;

    if (!record) {
        return PHY_TYPE_UNSPECIFIED;
    }

    if (record->second == WIFI_SECOND_CHAN_ABOVE) {
        sec_chan = 1;
    } else if (record->second == WIFI_SECOND_CHAN_BELOW) {
        sec_chan = -1;
    }

    if (record->phy_11ax) {
        return PHY_TYPE_HE;
    }

    if (record->phy_11ac) {
        return PHY_TYPE_VHT;
    }

    if (record->phy_11n || sec_chan != 0 || record->bandwidth > WIFI_BW20) {
        return PHY_TYPE_HT;
    }

    return (record->primary > 14) ? PHY_TYPE_OFDM : PHY_TYPE_ERP;
}

static bool roaming_app_authmode_is_enterprise(wifi_auth_mode_t authmode)
{
    return authmode == WIFI_AUTH_WPA_ENTERPRISE ||
           authmode == WIFI_AUTH_WPA2_ENTERPRISE ||
           authmode == WIFI_AUTH_WPA3_ENTERPRISE ||
           authmode == WIFI_AUTH_WPA2_WPA3_ENTERPRISE ||
           authmode == WIFI_AUTH_WPA3_ENT_192;
}

static bool roaming_app_authmode_is_open_mode(wifi_auth_mode_t authmode)
{
    return authmode == WIFI_AUTH_OPEN || authmode == WIFI_AUTH_OWE;
}

static bool roaming_app_authmode_is_legacy_personal(wifi_auth_mode_t authmode)
{
    return authmode == WIFI_AUTH_WPA_PSK ||
           authmode == WIFI_AUTH_WPA2_PSK ||
           authmode == WIFI_AUTH_WPA_WPA2_PSK;
}

static bool roaming_app_authmode_is_personal(wifi_auth_mode_t authmode)
{
    return roaming_app_authmode_is_legacy_personal(authmode) ||
           authmode == WIFI_AUTH_WPA3_PSK ||
           authmode == WIFI_AUTH_WPA2_WPA3_PSK ||
           authmode == WIFI_AUTH_WAPI_PSK;
}

static bool roaming_app_authmode_is_personal_compatible(wifi_auth_mode_t curr_auth, wifi_auth_mode_t cand_auth)
{
    if (!roaming_app_authmode_is_personal(curr_auth) || !roaming_app_authmode_is_personal(cand_auth)) {
        return false;
    }

    if (curr_auth == WIFI_AUTH_WAPI_PSK || cand_auth == WIFI_AUTH_WAPI_PSK) {
        return (curr_auth == cand_auth) ||
               (curr_auth == WIFI_AUTH_WAPI_PSK && roaming_app_authmode_is_legacy_personal(cand_auth)) ||
               (cand_auth == WIFI_AUTH_WAPI_PSK && roaming_app_authmode_is_legacy_personal(curr_auth));
    }

    return true;
}

static bool roaming_app_allows_wapi_legacy_threshold_transition(wifi_auth_mode_t threshold_auth,
                                                                wifi_auth_mode_t curr_auth,
                                                                wifi_auth_mode_t cand_auth)
{
    return threshold_auth == WIFI_AUTH_WAPI_PSK &&
           curr_auth == WIFI_AUTH_WAPI_PSK &&
           roaming_app_authmode_is_legacy_personal(cand_auth);
}

static int roaming_app_authmode_strength_rank(wifi_auth_mode_t authmode)
{
    switch (authmode) {
    case WIFI_AUTH_OPEN:
        return 0;
    case WIFI_AUTH_WEP:
        return 1;
    case WIFI_AUTH_WPA_PSK:
        return 2;
    case WIFI_AUTH_OWE:
        return 3;
    case WIFI_AUTH_WPA2_PSK:
    case WIFI_AUTH_WPA_WPA2_PSK:
        return 4;
    case WIFI_AUTH_WAPI_PSK:
        return 5;
    case WIFI_AUTH_WPA3_PSK:
    case WIFI_AUTH_WPA2_WPA3_PSK:
    case WIFI_AUTH_DPP:
        return 6;
    case WIFI_AUTH_WPA_ENTERPRISE:
        return 7;
    case WIFI_AUTH_ENTERPRISE:
        return 8;
    case WIFI_AUTH_WPA3_ENTERPRISE:
    case WIFI_AUTH_WPA2_WPA3_ENTERPRISE:
        return 9;
    case WIFI_AUTH_WPA3_ENT_192:
        return 10;
    default:
        return -1;
    }
}

static int roaming_app_candidate_strength_rank(wifi_auth_mode_t cand_auth,
                                               bool candidate_supports_sae,
                                               bool candidate_supports_owe,
                                               bool candidate_supports_wpa3_enterprise,
                                               bool candidate_supports_suite_b_192)
{
    if (candidate_supports_suite_b_192) {
        return roaming_app_authmode_strength_rank(WIFI_AUTH_WPA3_ENT_192);
    }

    if (candidate_supports_wpa3_enterprise) {
        return roaming_app_authmode_strength_rank(WIFI_AUTH_WPA3_ENTERPRISE);
    }

    if (cand_auth == WIFI_AUTH_WPA_ENTERPRISE) {
        return roaming_app_authmode_strength_rank(WIFI_AUTH_WPA_ENTERPRISE);
    }

    if (cand_auth == WIFI_AUTH_ENTERPRISE) {
        return roaming_app_authmode_strength_rank(WIFI_AUTH_ENTERPRISE);
    }

    if (candidate_supports_sae) {
        return roaming_app_authmode_strength_rank(WIFI_AUTH_WPA3_PSK);
    }

    if (cand_auth == WIFI_AUTH_WAPI_PSK) {
        return roaming_app_authmode_strength_rank(WIFI_AUTH_WAPI_PSK);
    }

    if (cand_auth == WIFI_AUTH_WPA2_PSK || cand_auth == WIFI_AUTH_WPA_WPA2_PSK) {
        return roaming_app_authmode_strength_rank(WIFI_AUTH_WPA2_PSK);
    }

    if (candidate_supports_owe) {
        return roaming_app_authmode_strength_rank(WIFI_AUTH_OWE);
    }

    return roaming_app_authmode_strength_rank(cand_auth);
}

static bool roaming_app_candidate_meets_threshold(wifi_auth_mode_t threshold_auth,
                                                  wifi_auth_mode_t curr_auth,
                                                  wifi_auth_mode_t cand_auth,
                                                  bool candidate_supports_sae,
                                                  bool candidate_supports_owe,
                                                  bool candidate_supports_wpa3_enterprise,
                                                  bool candidate_supports_suite_b_192)
{
    int threshold_rank = roaming_app_authmode_strength_rank(threshold_auth);
    int candidate_rank = roaming_app_candidate_strength_rank(cand_auth,
                                                             candidate_supports_sae,
                                                             candidate_supports_owe,
                                                             candidate_supports_wpa3_enterprise,
                                                             candidate_supports_suite_b_192);

    if (threshold_rank < 0 || candidate_rank < 0) {
        return false;
    }

    if (candidate_rank >= threshold_rank) {
        return true;
    }

    return roaming_app_allows_wapi_legacy_threshold_transition(threshold_auth, curr_auth, cand_auth);
}

static const u8 *roaming_app_get_rsnxe_ie(const uint8_t *bssid)
{
    const u8 *rsnxe = NULL;

    if (!bssid) {
        return NULL;
    }

    if (esp_wifi_is_wpa3_compatible_mode_enabled(WIFI_IF_STA)) {
        rsnxe = esp_wifi_sta_get_ie((u8 *) bssid, WFA_RSNXE_OVERRIDE_OUI_TYPE);
    }

    if (!rsnxe) {
        rsnxe = esp_wifi_sta_get_ie((u8 *) bssid, WLAN_EID_RSNX);
    }

    return rsnxe;
}

static int roaming_app_build_btm_candidate(const struct cand_bss *bss, char *buf, size_t buf_len)
{
    const wifi_ap_record_t *record;
    uint8_t channel;
    int sec_chan = 0;
    uint8_t phy_type;
    uint8_t op_class;

    if (!bss || !buf || buf_len == 0) {
        return -1;
    }

    record = roaming_app_find_scan_record(bss->bssid);
    channel = bss->channel;

    if (record) {
        channel = record->primary;
        if (record->second == WIFI_SECOND_CHAN_ABOVE) {
            sec_chan = 1;
        } else if (record->second == WIFI_SECOND_CHAN_BELOW) {
            sec_chan = -1;
        }
        phy_type = roaming_app_get_phy_type(record);
    } else {
        phy_type = (channel > 14) ? PHY_TYPE_OFDM : PHY_TYPE_ERP;
    }

    op_class = get_operating_class(channel, sec_chan);
    if (op_class == 0) {
        ESP_LOGW(ROAMING_TAG, "Could not determine operating class for " MACSTR, MAC2STR(bss->bssid));
        return -1;
    }

    if (snprintf(buf, buf_len, " neighbor=" MACSTR ",0x0000,%u,%u,%u",
                 MAC2STR(bss->bssid), op_class, channel, phy_type) >= (int) buf_len) {
        return -1;
    }

    return 0;
}

static void roaming_app_set_scan_filter_bssid(const uint8_t *bssid, bool enabled)
{
    if (enabled && bssid) {
        memcpy(g_roaming_app.config.scan_filter_bssid, bssid, sizeof(g_roaming_app.config.scan_filter_bssid));
        g_roaming_app.config.scan_filter_bssid_set = true;
        g_roaming_app.config.scan_config.bssid = g_roaming_app.config.scan_filter_bssid;
    } else {
        memset(g_roaming_app.config.scan_filter_bssid, 0, sizeof(g_roaming_app.config.scan_filter_bssid));
        g_roaming_app.config.scan_filter_bssid_set = false;
        g_roaming_app.config.scan_config.bssid = NULL;
    }
}

static void roaming_app_refresh_scan_config_filters(void)
{
    if (g_roaming_app.config.scan_filter_ssid[0] != '\0') {
        g_roaming_app.config.scan_config.ssid = g_roaming_app.config.scan_filter_ssid;
    } else if (g_roaming_app.current_bss.ap.ssid[0] != '\0') {
        g_roaming_app.config.scan_config.ssid = g_roaming_app.current_bss.ap.ssid;
    } else {
        g_roaming_app.config.scan_config.ssid = NULL;
    }

    roaming_app_set_scan_filter_bssid(g_roaming_app.config.scan_filter_bssid_set ?
                                      g_roaming_app.config.scan_filter_bssid : NULL,
                                      g_roaming_app.config.scan_filter_bssid_set);

    if (g_roaming_app.config.scan_config.channel_bitmap.ghz_2_channels ||
            g_roaming_app.config.scan_config.channel_bitmap.ghz_5_channels) {
        g_roaming_app.config.scan_config.channel = 0;
    }
}

static bool roaming_app_clear_connect_hint(void)
{
    wifi_config_t wifi_cfg = {0};
    esp_err_t err = esp_wifi_get_config(WIFI_IF_STA, &wifi_cfg);

    if (err != ESP_OK) {
        ESP_LOGW(ROAMING_TAG, "failed to read sta config while clearing roam hint: %s", esp_err_to_name(err));
        return false;
    }

    if (wifi_cfg.sta.bssid_set || wifi_cfg.sta.channel) {
        wifi_cfg.sta.channel = 0;
        wifi_cfg.sta.bssid_set = 0;
        err = esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg);
        if (err != ESP_OK) {
            ESP_LOGW(ROAMING_TAG, "failed to clear roam hint: %s", esp_err_to_name(err));
            return false;
        }
    }

    return true;
}

static void roaming_app_reset_connect_hint_state(void)
{
    if (!g_roaming_app.connect_hint_active) {
        return;
    }

    if (roaming_app_clear_connect_hint()) {
        g_roaming_app.connect_hint_active = false;
    }
}

static void roaming_app_mark_roam_attempt(void)
{
    gettimeofday(&g_roaming_app.last_roam_attempt_time, NULL);
}

static void roaming_app_mark_roam_success(void)
{
    gettimeofday(&g_roaming_app.last_roam_success_time, NULL);
}

static bool roaming_app_connected_roam_method_available(void)
{
#if NETWORK_ASSISTED_ROAMING_ENABLED
    if (g_roaming_app.config.btm_roaming_enabled && g_roaming_app.current_bss.btm_support) {
#if LEGACY_ROAM_ENABLED && NETWORK_ASSISTED_ROAMING_ENABLED
        if (g_roaming_app.btm_attempt <= g_roaming_app.config.btm_retry_cnt) {
            return true;
        }
#else
        return true;
#endif
    }
#endif /*NETWORK_ASSISTED_ROAMING_ENABLED*/

#if LEGACY_ROAM_ENABLED
    if (g_roaming_app.config.legacy_roam_enabled) {
        return true;
    }
#endif /*LEGACY_ROAM_ENABLED*/

    return false;
}

static const struct timeval *roaming_app_get_backoff_anchor(void)
{
    const struct timeval *anchor = &g_roaming_app.last_roam_attempt_time;

    if (g_roaming_app.last_roam_success_time.tv_sec > anchor->tv_sec ||
            (g_roaming_app.last_roam_success_time.tv_sec == anchor->tv_sec &&
             g_roaming_app.last_roam_success_time.tv_usec > anchor->tv_usec)) {
        anchor = &g_roaming_app.last_roam_success_time;
    }

    return anchor;
}

static bool roaming_app_roam_backoff_active(const struct timeval *now, uint8_t margin_sec)
{
    const struct timeval *anchor;
    long required_gap;

    if (g_roaming_app.config.backoff_time <= margin_sec) {
        return false;
    }

    anchor = roaming_app_get_backoff_anchor();
    required_gap = g_roaming_app.config.backoff_time - margin_sec;
    return time_diff_sec(now, anchor) < required_gap;
}

static bool roaming_app_can_act_on_candidate(const struct timeval *now, bool log_reason)
{
    if (!g_roaming_app.sta_connected) {
        return true;
    }

    if (roaming_app_roam_backoff_active(now, 0)) {
        if (log_reason) {
            ESP_LOGD(ROAMING_TAG, "Skipping candidate processing during roam backoff window");
        }
        return false;
    }

    if (!roaming_app_connected_roam_method_available()) {
        if (log_reason) {
            ESP_LOGD(ROAMING_TAG, "Skipping candidate processing as no roaming method is currently available");
        }
        return false;
    }

    return true;
}

static void roaming_app_stop_periodic_monitors(void)
{
#if PERIODIC_RRM_MONITORING
    g_roaming_app.periodic_rrm_active = false;
    g_roaming_app.rrm_request_active = false;
    eloop_cancel_timeout(roaming_app_periodic_rrm_internal_handler, NULL, NULL);
#endif

#if PERIODIC_SCAN_MONITORING
    g_roaming_app.periodic_scan_active = false;
    eloop_cancel_timeout(roaming_app_periodic_scan_internal_handler, NULL, NULL);
#endif
}

static void roaming_app_set_disconnected_state(const wifi_event_sta_disconnected_t *disconn)
{
    g_roaming_app.sta_connected = false;
    g_roaming_app.current_bss.btm_support = false;
    g_roaming_app.current_bss.rrm_support = false;
    g_roaming_app.current_bss.ap.rssi = -128;
    g_roaming_app.current_bss.ap.authmode = WIFI_AUTH_OPEN;
    memset(g_roaming_app.current_bss.ap.ssid, 0, sizeof(g_roaming_app.current_bss.ap.ssid));

    if (disconn) {
        memcpy(g_roaming_app.current_bss.ap.bssid, disconn->bssid, ETH_ALEN);
    } else {
        memset(g_roaming_app.current_bss.ap.bssid, 0, ETH_ALEN);
    }
}

static bool roaming_app_try_default_reconnect(const char *reason)
{
    roaming_app_reset_connect_hint_state();
    if (esp_wifi_connect() != ESP_OK) {
        ESP_LOGW(ROAMING_TAG, "%s failed", reason);
        return false;
    }

    return true;
}

static void disable_reconnect(void *ctx, void *data)
{
    ESP_LOGD(ROAMING_TAG, "Disable roaming app reconnect");
    g_roaming_app.allow_reconnect = false;
}

static void enable_reconnect(void *ctx, void *data)
{
    ESP_LOGD(ROAMING_TAG, "Enable roaming app reconnect");
    g_roaming_app.allow_reconnect = true;
}

void roam_disable_reconnect(void)
{
    eloop_register_timeout(0, 0, disable_reconnect, NULL, NULL);
}

void roam_enable_reconnect(void)
{
    eloop_register_timeout(0, 0, enable_reconnect, NULL, NULL);
}

static bool roaming_app_get_ap_info(wifi_ap_record_t *ap_info)
{
    esp_err_t err;

    if (!ap_info) {
        return false;
    }

    err = esp_wifi_sta_get_ap_info(ap_info);
    if (err != ESP_OK) {
        ESP_LOGW(ROAMING_TAG, "failed to get current AP info: %s", esp_err_to_name(err));
        return false;
    }
#if LOW_RSSI_ROAMING_ENABLED
    /*
     * If the current rssi is below the configured rssi threshold for
     * low rssi based roaming and the current rssi threshold is below that,
     * we should reset the rssi threshold back to the configured rssi threshold */
    if (g_roaming_app.config.low_rssi_roam_trigger &&
            (ap_info->rssi > g_roaming_app.config.low_rssi_threshold) &&
            (g_roaming_app.current_low_rssi_threshold < g_roaming_app.config.low_rssi_threshold)) {
        if (esp_wifi_set_rssi_threshold(g_roaming_app.config.low_rssi_threshold) == ESP_OK) {
            g_roaming_app.current_low_rssi_threshold = g_roaming_app.config.low_rssi_threshold;
            ESP_LOGD(ROAMING_TAG, "Reset the low rssi threshold back to %d", g_roaming_app.config.low_rssi_threshold);
        } else {
            ESP_LOGW(ROAMING_TAG, "failed to reset low RSSI threshold to %d", g_roaming_app.config.low_rssi_threshold);
        }
    }
#endif /*LOW_RSSI_ROAMING_ENABLED*/
    return true;
}

#if LEGACY_ROAM_ENABLED
static void legacy_roam_clear_bssid_flag(void)
{
    if (g_roaming_app.connect_hint_active && roaming_app_clear_connect_hint()) {
        g_roaming_app.connect_hint_active = false;
        ESP_LOGD(ROAMING_TAG, "cleared bssid/channel hint");
    }
}
#endif /*LEGACY_ROAM_ENABLED*/

#if LOW_RSSI_ROAMING_ENABLED
static void roaming_app_apply_low_rssi_config(void)
{
    int32_t next_threshold;

    if (!g_roaming_app.sta_connected) {
        return;
    }

    if (!g_roaming_app.config.low_rssi_roam_trigger) {
        if (esp_wifi_set_rssi_threshold(RSSI_THRESHOLD_DISABLED) == ESP_OK) {
            g_roaming_app.current_low_rssi_threshold = RSSI_THRESHOLD_DISABLED;
            ESP_LOGD(ROAMING_TAG, "Disabled low RSSI roaming threshold");
        } else {
            ESP_LOGW(ROAMING_TAG, "failed to disable low RSSI roaming threshold");
        }
        return;
    }

    if (!roaming_app_get_ap_info(&g_roaming_app.current_bss.ap)) {
        return;
    }

    if (g_roaming_app.current_bss.ap.rssi < g_roaming_app.config.low_rssi_threshold) {
        /* To ensure that the threshold is set to one offset below the current AP RSSI
         * in case, the AP is already below the RSSI threshold */
        next_threshold = g_roaming_app.current_bss.ap.rssi -
                         g_roaming_app.config.rssi_threshold_reduction_offset;
    } else {
        next_threshold = g_roaming_app.config.low_rssi_threshold;
    }

    next_threshold = roaming_app_clamp_rssi_threshold(next_threshold);
    if (esp_wifi_set_rssi_threshold(next_threshold) == ESP_OK) {
        g_roaming_app.current_low_rssi_threshold = next_threshold;
        ESP_LOGD(ROAMING_TAG, "setting rssi threshold as %d", g_roaming_app.current_low_rssi_threshold);
    } else {
        ESP_LOGW(ROAMING_TAG, "failed to set low RSSI threshold to %d", next_threshold);
    }
}
#endif /*LOW_RSSI_ROAMING_ENABLED*/

static void roaming_app_apply_runtime_config(void)
{
    roaming_app_stop_periodic_monitors();

#if LOW_RSSI_ROAMING_ENABLED
    roaming_app_apply_low_rssi_config();
#endif /*LOW_RSSI_ROAMING_ENABLED*/

#if PERIODIC_RRM_MONITORING
    if (g_roaming_app.sta_connected && g_roaming_app.current_bss.rrm_support && g_roaming_app.config.rrm_monitor) {
        init_periodic_rrm_event();
    }
#endif /*PERIODIC_RRM_MONITORING*/

#if PERIODIC_SCAN_MONITORING
    if (g_roaming_app.sta_connected && g_roaming_app.config.scan_monitor) {
        init_periodic_scan_roam_event();
    }
#endif /*PERIODIC_SCAN_MONITORING*/
}

static void roaming_app_invalidate_scan_cache(void)
{
    g_roaming_app.scanned_aps.current_count = 0;
    g_roaming_app.scanned_aps.time.tv_sec = 0;
    g_roaming_app.scanned_aps.time.tv_usec = 0;
}

static esp_err_t roaming_app_run_blocking(eloop_blocking_timeout_handler handler, void *user_ctx)
{
    int ret;

    if (!handler) {
        return ESP_ERR_INVALID_ARG;
    }

    if (current_task_is_wifi_task()) {
        ret = handler(NULL, user_ctx);
    } else {
        ret = eloop_register_timeout_blocking(handler, NULL, user_ctx);
    }

    return ret < 0 ? ESP_FAIL : ret;
}

#if CONFIG_ESP_WIFI_ROAMING_BSSID_BLACKLIST
static int roaming_app_find_blacklist_entry_index(const uint8_t *bssid)
{
    if (!bssid) {
        return -1;
    }

    for (int i = 0; i < g_roaming_app.bssid_blacklist_count; i++) {
        if (memcmp(g_roaming_app.bssid_blacklist[i].bssid, bssid, ETH_ALEN) == 0) {
            return i;
        }
    }

    return -1;
}

static struct blacklist_entry *roaming_app_find_blacklist_entry(const uint8_t *bssid)
{
    int index = roaming_app_find_blacklist_entry_index(bssid);
    return (index >= 0) ? &g_roaming_app.bssid_blacklist[index] : NULL;
}

static int roaming_app_find_oldest_blacklist_entry_index(void)
{
    int oldest_idx = 0;

    for (int i = 1; i < g_roaming_app.bssid_blacklist_count; i++) {
        if (g_roaming_app.bssid_blacklist[i].timestamp.tv_sec <
                g_roaming_app.bssid_blacklist[oldest_idx].timestamp.tv_sec) {
            oldest_idx = i;
        }
    }

    return oldest_idx;
}

static bool roaming_app_blacklist_entry_blocks_roaming(const struct blacklist_entry *entry)
{
    if (!entry) {
        return false;
    }

#if CONFIG_ESP_WIFI_ROAMING_AUTO_BLACKLISTING
    return entry->manual || (entry->failures >= CONFIG_ESP_WIFI_ROAMING_MAX_CONN_FAILURES);
#else
    return true;
#endif
}

static void roaming_app_touch_blacklist_entry(struct blacklist_entry *entry)
{
    if (entry) {
        gettimeofday(&entry->timestamp, NULL);
    }
}

static void roaming_app_mark_blacklist_entry_manual(struct blacklist_entry *entry, const uint8_t *bssid)
{
    if (!entry || !bssid) {
        return;
    }

    memcpy(entry->bssid, bssid, ETH_ALEN);
    entry->manual = true;
#if CONFIG_ESP_WIFI_ROAMING_AUTO_BLACKLISTING
    entry->failures = CONFIG_ESP_WIFI_ROAMING_MAX_CONN_FAILURES;
#endif
    roaming_app_touch_blacklist_entry(entry);
}

static bool roaming_app_add_manual_blacklist_entry(const uint8_t *bssid)
{
    struct blacklist_entry *entry = roaming_app_find_blacklist_entry(bssid);

    if (entry) {
        roaming_app_mark_blacklist_entry_manual(entry, bssid);
        return true;
    }

    if (g_roaming_app.bssid_blacklist_count >= CONFIG_ESP_WIFI_ROAMING_MAX_CANDIDATES) {
        return false;
    }

    entry = &g_roaming_app.bssid_blacklist[g_roaming_app.bssid_blacklist_count++];
    roaming_app_mark_blacklist_entry_manual(entry, bssid);
    return true;
}

static void roaming_app_record_connection_failure(const uint8_t *bssid)
{
#if CONFIG_ESP_WIFI_ROAMING_AUTO_BLACKLISTING
    struct blacklist_entry *entry;

    if (!bssid) {
        return;
    }

    entry = roaming_app_find_blacklist_entry(bssid);
    if (entry) {
        if (!entry->manual) {
            entry->failures++;
        }
        roaming_app_touch_blacklist_entry(entry);
        ESP_LOGD(ROAMING_TAG, "BSSID " MACSTR " connection failures: %d",
                 MAC2STR(bssid), entry->failures);
        if (!entry->manual && entry->failures >= CONFIG_ESP_WIFI_ROAMING_MAX_CONN_FAILURES) {
            ESP_LOGI(ROAMING_TAG, "BSSID " MACSTR " blacklisted", MAC2STR(bssid));
        }
        return;
    }

    if (g_roaming_app.bssid_blacklist_count < CONFIG_ESP_WIFI_ROAMING_MAX_CANDIDATES) {
        entry = &g_roaming_app.bssid_blacklist[g_roaming_app.bssid_blacklist_count++];
    } else {
        int oldest_idx = roaming_app_find_oldest_blacklist_entry_index();
        ESP_LOGD(ROAMING_TAG, "Blacklist is full. Replacing oldest entry for " MACSTR,
                 MAC2STR(g_roaming_app.bssid_blacklist[oldest_idx].bssid));
        entry = &g_roaming_app.bssid_blacklist[oldest_idx];
    }

    memcpy(entry->bssid, bssid, ETH_ALEN);
    entry->manual = false;
    entry->failures = 1;
    roaming_app_touch_blacklist_entry(entry);
    ESP_LOGD(ROAMING_TAG, "BSSID " MACSTR " added to blacklist tracking", MAC2STR(bssid));
#else
    (void) bssid;
#endif
}

static void roaming_app_reset_auto_blacklist_failures(const uint8_t *bssid)
{
#if CONFIG_ESP_WIFI_ROAMING_AUTO_BLACKLISTING
    struct blacklist_entry *entry = roaming_app_find_blacklist_entry(bssid);

    if (entry && !entry->manual && entry->failures > 0) {
        entry->failures = 0;
        roaming_app_touch_blacklist_entry(entry);
        ESP_LOGD(ROAMING_TAG, "Reset automatic blacklist failure count for " MACSTR, MAC2STR(bssid));
    }
#else
    (void) bssid;
#endif
}

static void roaming_app_blacklist_recovery_internal_handler(void *data, void *ctx)
{
    if (!g_roaming_app.app_active) {
        return;
    }

    if (g_roaming_app.sta_connected || !g_roaming_app.allow_reconnect) {
        return;
    }

    if (!is_bssid_blacklisted(g_roaming_app.current_bss.ap.bssid)) {
        ESP_LOGI(ROAMING_TAG, "Blacklist expired for " MACSTR ", retrying default reconnect",
                 MAC2STR(g_roaming_app.current_bss.ap.bssid));
        roaming_app_try_default_reconnect("Default reconnect after blacklist expiry");
        return;
    }

    roaming_app_invalidate_scan_cache();
    determine_best_ap(0);
}

static void roaming_app_schedule_blacklist_recovery(void)
{
    uint8_t retry_delay = g_roaming_app.config.backoff_time ? g_roaming_app.config.backoff_time : 1;

    eloop_cancel_timeout(roaming_app_blacklist_recovery_internal_handler, NULL, NULL);
    if (eloop_register_timeout(retry_delay, 0, roaming_app_blacklist_recovery_internal_handler, NULL, NULL) != 0) {
        ESP_LOGW(ROAMING_TAG, "Could not schedule blacklist recovery retry");
    }
}

static void roaming_app_retry_blacklist_recovery_if_needed(void)
{
    if (!g_roaming_app.sta_connected && is_bssid_blacklisted(g_roaming_app.current_bss.ap.bssid)) {
        ESP_LOGI(ROAMING_TAG, "No non-blacklisted AP found, scheduling recovery retry");
#if LEGACY_ROAM_ENABLED
        if (g_roaming_app.force_roam_ongoing) {
            legacy_roam_clear_bssid_flag();
        }
#endif /*LEGACY_ROAM_ENABLED*/
        roaming_app_reset_connect_hint_state();
        roaming_app_schedule_blacklist_recovery();
    }
}
#endif /*CONFIG_ESP_WIFI_ROAMING_BSSID_BLACKLIST*/

static int8_t initialize_roaming_event(void)
{
#if LEGACY_ROAM_ENABLED
    /*
     * Resetting the Bssid param as it is possible that a previous force
     * roam has set config to connect to a specific bssid and now further
     * roaming attempts using BTM could lead to a spiral of connecting to
     * the previous AP */
    if (g_roaming_app.force_roam_ongoing) {
        legacy_roam_clear_bssid_flag();
    }
#endif /*LEGACY_ROAM_ENABLED*/
    return ESP_OK;
}

#if PERIODIC_RRM_MONITORING
static void init_periodic_rrm_event(void)
{
    if (!g_roaming_app.config.rrm_monitor) {
        ESP_LOGI(ROAMING_TAG, "RRM monitor is disabled in config");
        return;
    }
    ESP_LOGV(ROAMING_TAG, "Initialised Periodic RRM Monitoring event!");
    if (eloop_register_timeout(g_roaming_app.config.rrm_monitor_time, 0, roaming_app_periodic_rrm_internal_handler, NULL, NULL)) {
        ESP_LOGE(ROAMING_TAG, "Could not register periodic neighbor report event.");
        return;
    }
    g_roaming_app.periodic_rrm_active = true;
}
#endif /*PERIODIC_RRM_MONITORING*/

#if PERIODIC_SCAN_MONITORING
static void init_periodic_scan_roam_event(void)
{
    if (!g_roaming_app.config.scan_monitor) {
        ESP_LOGI(ROAMING_TAG, "%s: Scan monitor is disabled in config", __func__);
        return;
    }
    ESP_LOGV(ROAMING_TAG, "Initialised Periodic Scan Roam event!");
    if (eloop_register_timeout(g_roaming_app.config.scan_interval, 0, roaming_app_periodic_scan_internal_handler, NULL, NULL)) {
        ESP_LOGE(ROAMING_TAG, "Could not register periodic scan monitoring event");
        return;
    }
    g_roaming_app.periodic_scan_active = true;
}
#endif /*PERIODIC_SCAN_MONITORING*/

static void roaming_app_disconnected_event_handler(void *ctx, void *data)
{
    roaming_app_untrack_timeout_user_data(data);

    if (!g_roaming_app.app_active) {
        if (data) {
            os_free(data);
        }
        return;
    }

    wifi_event_sta_disconnected_t *disconn = data;
    roaming_app_stop_periodic_monitors();
    roaming_app_set_disconnected_state(disconn);

    ESP_LOGD(ROAMING_TAG, "station got disconnected reason=%d, rssi =%d", disconn->reason, disconn->rssi);
#if CONFIG_ESP_WIFI_ROAMING_AUTO_BLACKLISTING
    if (disconn->reason == WIFI_REASON_CONNECTION_FAIL || disconn->reason == WIFI_REASON_AUTH_FAIL) {
        roaming_app_record_connection_failure(g_roaming_app.current_bss.ap.bssid);
    }
#endif
    if (disconn->reason == WIFI_REASON_ROAMING) {
        ESP_LOGD(ROAMING_TAG, "station roaming, do nothing");
    } else if (g_roaming_app.allow_reconnect == false) {
        ESP_LOGD(ROAMING_TAG, "station initiated disconnect, do nothing");
    } else {
#if CONFIG_ESP_WIFI_ROAMING_BSSID_BLACKLIST
        if (is_bssid_blacklisted(g_roaming_app.current_bss.ap.bssid)) {
            ESP_LOGD(ROAMING_TAG, "AP " MACSTR " is blacklisted, finding another AP", MAC2STR(g_roaming_app.current_bss.ap.bssid));
            determine_best_ap(0);
        } else {
#endif
#if LEGACY_ROAM_ENABLED
            /*
            * Resetting the Bssid param as it is possible that a previous force
            * roam has set config to connect to a specific bssid and now further
            * roaming attempts using BTM could lead to a spiral of connecting to
            * the previous AP */
            if (g_roaming_app.force_roam_ongoing) {
                legacy_roam_clear_bssid_flag();
            }
#endif /*LEGACY_ROAM_ENABLED*/
            roaming_app_try_default_reconnect("Default reconnect");
#if CONFIG_ESP_WIFI_ROAMING_BSSID_BLACKLIST
        }
#endif
    }
    os_free(disconn);
}

static void roaming_app_connected_event_handler(void *ctx, void *data)
{
    bool ap_info_available;

    if (!g_roaming_app.app_active) {
        return;
    }

    g_roaming_app.sta_connected = true;
#if CONFIG_ESP_WIFI_ROAMING_BSSID_BLACKLIST
    eloop_cancel_timeout(roaming_app_blacklist_recovery_internal_handler, NULL, NULL);
#endif
    roaming_app_reset_connect_hint_state();
    memset(&g_roaming_app.current_bss.ap, 0, sizeof(g_roaming_app.current_bss.ap));
    g_roaming_app.current_bss.ap.rssi = -128;
    ap_info_available = roaming_app_get_ap_info(&g_roaming_app.current_bss.ap);
    if (!ap_info_available) {
        ESP_LOGW(ROAMING_TAG, "Skipping AP info-dependent connected handling");
    }
#if CONFIG_ESP_WIFI_ROAMING_BSSID_BLACKLIST
    if (ap_info_available) {
        roaming_app_reset_auto_blacklist_failures(g_roaming_app.current_bss.ap.bssid);
    }
#endif
    roaming_app_invalidate_scan_cache();
    roaming_app_refresh_scan_config_filters();
    g_roaming_app.current_bss.rrm_support = esp_rrm_is_rrm_supported_connection();
    g_roaming_app.current_bss.btm_support = esp_wnm_is_btm_supported_connection();
    ESP_LOGD(ROAMING_TAG, "Station connected, RRM %ssupported, BTM %ssupported",
             g_roaming_app.current_bss.rrm_support ? " " : "not ",
             g_roaming_app.current_bss.btm_support ? " " : "not ");
    roaming_app_mark_roam_success();
    if (!initialize_roaming_event()) {
        roaming_app_apply_runtime_config();
        ESP_LOGD(ROAMING_TAG, "Initialised initialise roaming events!");
    } else {
        ESP_LOGE(ROAMING_TAG, "Failed to Initialise roaming events");
    }
#if LEGACY_ROAM_ENABLED && NETWORK_ASSISTED_ROAMING_ENABLED
    g_roaming_app.btm_attempt = 0;
#endif /*LEGACY_ROAM_ENABLED && NETWORK_ASSISTED_ROAMING_ENABLED*/
#if LEGACY_ROAM_ENABLED
    g_roaming_app.force_roam_ongoing = false;
#endif /*LEGACY_ROAM_ENABLED*/
    g_roaming_app.allow_reconnect = true;
}

void roam_sta_connected(void)
{
    if (!g_roaming_app.app_active) {
        return;
    }

    if (eloop_register_timeout(0, 0, roaming_app_connected_event_handler, NULL, NULL) != 0) {
        ESP_LOGW(ROAMING_TAG, "Could not schedule connected event handler");
    }
}

void roam_sta_disconnected(void *data)
{
    if (!g_roaming_app.app_active) {
        return;
    }

    wifi_event_sta_disconnected_t *disconn = os_malloc(sizeof(*disconn));

    if (!disconn) {
        return;
    }
    os_memcpy(disconn, data, sizeof(*disconn));
    if (eloop_register_timeout(0, 0, roaming_app_disconnected_event_handler, NULL, disconn) != 0) {
        os_free(disconn);
    } else {
        roaming_app_track_timeout_user_data(disconn);
    }
}

#if PERIODIC_RRM_MONITORING
static int roaming_app_append_string(char *buf, size_t buf_len, size_t *len, const char *fmt, ...)
{
    va_list ap;
    int written;

    if (*len >= buf_len) {
        return -1;
    }

    va_start(ap, fmt);
    written = vsnprintf(buf + *len, buf_len - *len, fmt, ap);
    va_end(ap);

    if (written < 0 || (size_t) written >= buf_len - *len) {
        return -1;
    }

    *len += written;
    return 0;
}

static char * get_btm_neighbor_list(uint8_t *report, size_t report_len)
{
    size_t len = 0;
    const uint8_t *data;
    int ret = 0;

    /*
     * Neighbor Report element (IEEE P802.11-REVmc/D5.0)
     * BSSID[6]
     * BSSID Information[4]
     * Operating Class[1]
     * Channel Number[1]
     * PHY Type[1]
     * Optional Subelements[variable]
     */
#define NR_IE_MIN_LEN (ETH_ALEN + 4 + 1 + 1 + 1)

    if (!report || report_len == 0) {
        ESP_LOGE(ROAMING_TAG, "RRM neighbor report is not valid");
        return NULL;
    }

    char *buf = os_calloc(1, MAX_NEIGHBOR_LEN);
    if (!buf) {
        ESP_LOGE(ROAMING_TAG, "get_btm_list : Memory alloc failed for buf");
        return NULL;
    }
    data = report;
    while (report_len >= 2 + NR_IE_MIN_LEN) {
        const uint8_t *nr;
        uint8_t nr_len = data[1];
        const uint8_t *pos = data, *end;

        if (pos[0] != WLAN_EID_NEIGHBOR_REPORT ||
                nr_len < NR_IE_MIN_LEN) {
            ESP_LOGD(ROAMING_TAG, "CTRL: Invalid Neighbor Report element: id=%u len=%u",
                     data[0], nr_len);
            ret = -1;
            goto cleanup;
        }

        if (2U + nr_len > report_len) {
            ESP_LOGD(ROAMING_TAG, "CTRL: Invalid Neighbor Report element: id=%u len=%zu nr_len=%u",
                     data[0], report_len, nr_len);
            ret = -1;
            goto cleanup;
        }
        pos += 2;
        end = pos + nr_len;

        nr = pos;
        pos += NR_IE_MIN_LEN;

        while (end - pos >= 2) {
            uint8_t s_id;
            uint8_t s_len;
            s_id = *pos++;
            s_len = *pos++;
            (void) s_id;
            if (s_len > end - pos) {
                ret = -1;
                goto cleanup;
            }
            pos += s_len;
        }
        if (pos != end) {
            ret = -1;
            goto cleanup;
        }
        ESP_LOGD(ROAMING_TAG, "RMM neighbor report bssid=" MACSTR
                 " info=0x%" PRIx32 " op_class=%u chan=%u phy_type=%u",
                 MAC2STR(nr), WPA_GET_LE32(nr + ETH_ALEN),
                 nr[ETH_ALEN + 4], nr[ETH_ALEN + 5],
                 nr[ETH_ALEN + 6]);
        /* neighbor start */
        if (roaming_app_append_string(buf, MAX_NEIGHBOR_LEN, &len,
                                      " neighbor=" MACSTR ",0x%04" PRIx32 ",%u,%u,%u",
                                      MAC2STR(nr), WPA_GET_LE32(nr + ETH_ALEN),
                                      nr[ETH_ALEN + 4], nr[ETH_ALEN + 5],
                                      nr[ETH_ALEN + 6]) < 0) {
            ESP_LOGW(ROAMING_TAG, "Neighbor report too large for BTM candidate buffer");
            ret = -1;
            goto cleanup;
        }
        /* optional elements, skip */

        data = end;
        report_len -= 2 + nr_len;
    }

cleanup:
    if (ret < 0) {
        os_free(buf);
        buf = NULL;
    }
    return buf;
}

static void roaming_app_neighbor_report_recv_internal_handler(void *ctx, void *data)
{
    wifi_event_neighbor_report_t *neighbor_report_event = data;
    roaming_app_untrack_timeout_user_data(neighbor_report_event);

    if (!g_roaming_app.app_active) {
        if (neighbor_report_event) {
            os_free(neighbor_report_event);
        }
        return;
    }

    if (!g_roaming_app.rrm_request_active) {
        ESP_LOGV(ROAMING_TAG, "Not the response for our Neighbor Report Request");
        goto cleanup;
    }
    g_roaming_app.rrm_request_active = false;

    if (!neighbor_report_event) {
        ESP_LOGE(ROAMING_TAG, "No data received for neighbor report");
        goto cleanup;
    }

    ESP_LOGD(ROAMING_TAG, "Received cb for Neighbor Report Request");

    uint8_t *pos = (uint8_t *)neighbor_report_event->n_report;
    uint16_t report_len = neighbor_report_event->report_len;
    if (!report_len) {
        ESP_LOGE(ROAMING_TAG, "Neighbor report is empty");
        goto cleanup;
    }

    /* dump report info */
    ESP_LOGD(ROAMING_TAG, "rrm: neighbor report len=%d", report_len);
    ESP_LOG_BUFFER_HEXDUMP(ROAMING_TAG, pos, report_len, ESP_LOG_DEBUG);

    if (g_roaming_app.btm_neighbor_list) {
        os_free(g_roaming_app.btm_neighbor_list);
        g_roaming_app.btm_neighbor_list = NULL;
    }
    /* create neighbor list */
    g_roaming_app.btm_neighbor_list = get_btm_neighbor_list(pos + 1, report_len - 1);

cleanup:
    if (neighbor_report_event) {
        os_free(neighbor_report_event);
    }
}

static void roaming_app_neighbor_report_recv_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    wifi_event_neighbor_report_t *event = (wifi_event_neighbor_report_t*)event_data;
    if (!g_roaming_app.app_active) {
        return;
    }
    if (!event) {
        return;
    }
    wifi_event_neighbor_report_t *event_copy = os_malloc(sizeof(wifi_event_neighbor_report_t) + event->report_len);
    if (!event_copy) {
        ESP_LOGE(ROAMING_TAG, "Failed to allocate memory for neighbor report event");
        return;
    }
    memcpy(event_copy, event, sizeof(wifi_event_neighbor_report_t) + event->report_len);

    if (eloop_register_timeout(0, 0, roaming_app_neighbor_report_recv_internal_handler, NULL, event_copy) != 0) {
        os_free(event_copy);
    } else {
        roaming_app_track_timeout_user_data(event_copy);
    }
}
#endif /*PERIODIC_RRM_MONITORING*/

#if LOW_RSSI_ROAMING_ENABLED
static void roaming_app_rssi_low_internal_handler(void *ctx, void *data)
{
    wifi_event_bss_rssi_low_t *event = data;
    roaming_app_untrack_timeout_user_data(event);

    if (!g_roaming_app.app_active) {
        if (event) {
            os_free(event);
        }
        return;
    }

    ESP_LOGI(ROAMING_TAG, "%s:bss rssi is=%ld", __func__, event->rssi);

    if (!g_roaming_app.sta_connected || !g_roaming_app.config.low_rssi_roam_trigger) {
        os_free(event);
        return;
    }

    if (!roaming_app_get_ap_info(&g_roaming_app.current_bss.ap)) {
        g_roaming_app.current_bss.ap.rssi = event->rssi;
    }
    determine_best_ap(0);
    int32_t next_threshold = g_roaming_app.current_low_rssi_threshold -
                             g_roaming_app.config.rssi_threshold_reduction_offset;
    next_threshold = roaming_app_clamp_rssi_threshold(next_threshold);
    if (esp_wifi_set_rssi_threshold(next_threshold) == ESP_OK) {
        g_roaming_app.current_low_rssi_threshold = next_threshold;
        ESP_LOGD(ROAMING_TAG, "Resetting RSSI Threshold to %d", g_roaming_app.current_low_rssi_threshold);
    } else {
        ESP_LOGW(ROAMING_TAG, "failed to reset RSSI threshold to %d", next_threshold);
    }

    os_free(event);
}

static void roaming_app_rssi_low_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    wifi_event_bss_rssi_low_t *event = event_data;
    if (!g_roaming_app.app_active) {
        return;
    }
    if (!event) {
        return;
    }
    wifi_event_bss_rssi_low_t *event_copy = os_malloc(sizeof(wifi_event_bss_rssi_low_t));
    if (!event_copy) {
        ESP_LOGE(ROAMING_TAG, "Failed to allocate memory for rssi low event");
        return;
    }
    memcpy(event_copy, event, sizeof(wifi_event_bss_rssi_low_t));

    if (eloop_register_timeout(0, 0, roaming_app_rssi_low_internal_handler, NULL, event_copy) != 0) {
        os_free(event_copy);
    } else {
        roaming_app_track_timeout_user_data(event_copy);
    }
}
#endif

#if NETWORK_ASSISTED_ROAMING_ENABLED
static bool trigger_network_assisted_roam(struct cand_bss *bss)
{
    char selected_candidate[96] = {0};
    char *query_list = NULL;
    const char *btm_candidates = g_roaming_app.btm_neighbor_list;

    if (!g_roaming_app.sta_connected) {
        ESP_LOGD(ROAMING_TAG, "Skipping BTM query while disconnected");
        return false;
    }

    if (bss && roaming_app_build_btm_candidate(bss, selected_candidate, sizeof(selected_candidate)) == 0) {
        query_list = os_calloc(1, BTM_QUERY_LIST_MAX_LEN);
        if (!query_list) {
            ESP_LOGW(ROAMING_TAG, "Skipping BTM query due to memory allocation failure");
            return false;
        }
        strlcpy(query_list, selected_candidate, BTM_QUERY_LIST_MAX_LEN);
        if (g_roaming_app.btm_neighbor_list) {
            size_t selected_len = strlen(query_list);
            size_t neighbor_len = strlen(g_roaming_app.btm_neighbor_list);

            if (selected_len + neighbor_len < BTM_QUERY_LIST_MAX_LEN) {
                strlcat(query_list, g_roaming_app.btm_neighbor_list, BTM_QUERY_LIST_MAX_LEN);
            } else {
                ESP_LOGW(ROAMING_TAG, "Skipping cached neighbor list as explicit BTM candidate list is full");
            }
        }
        btm_candidates = query_list;
    }

    if (esp_wnm_send_bss_transition_mgmt_query(REASON_RSSI, btm_candidates, 1) < 0) {
        ESP_LOGD(ROAMING_TAG, "failed to send btm query");
        os_free(query_list);
        return false;
    }
    ESP_LOGD(ROAMING_TAG, "Sent BTM Query");
    roaming_app_mark_roam_attempt();
#if LEGACY_ROAM_ENABLED
    g_roaming_app.btm_attempt++;
#endif
    os_free(query_list);
    return true;
}
#endif /*NETWORK_ASSISTED_ROAMING*/

static bool trigger_targeted_reconnect(struct cand_bss *bss, bool disconnect_first)
{
    wifi_config_t wifi_cfg = {0};
    esp_err_t err;

    if (!bss) {
        return false;
    }

    err = esp_wifi_get_config(WIFI_IF_STA, &wifi_cfg);
    if (err != ESP_OK) {
        ESP_LOGW(ROAMING_TAG, "failed to read sta config for targeted reconnect: %s", esp_err_to_name(err));
        return false;
    }

    if (disconnect_first) {
        err = esp_wifi_internal_issue_disconnect(WIFI_REASON_BSS_TRANSITION_DISASSOC);
        if (err != ESP_OK) {
            ESP_LOGW(ROAMING_TAG, "failed to disconnect for targeted reconnect: %s", esp_err_to_name(err));
            return false;
        }
    }

    wifi_cfg.sta.channel = bss->channel;
    wifi_cfg.sta.bssid_set = true;
    os_memcpy(wifi_cfg.sta.bssid, bss->bssid, ETH_ALEN);

    err = esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg);
    if (err != ESP_OK) {
        ESP_LOGW(ROAMING_TAG, "failed to apply targeted reconnect config: %s", esp_err_to_name(err));
        return false;
    }
    g_roaming_app.connect_hint_active = true;

    err = esp_wifi_connect();
    if (err != ESP_OK) {
        ESP_LOGW(ROAMING_TAG, "failed to start targeted reconnect: %s", esp_err_to_name(err));
        roaming_app_reset_connect_hint_state();
        return false;
    }

    esp_wifi_roaming_set_current_bssid(bss->bssid);
    if (disconnect_first) {
        ESP_LOGI(ROAMING_TAG, "Disconnecting and connecting to " MACSTR " on account of better RSSI", MAC2STR(bss->bssid));
    } else {
        ESP_LOGI(ROAMING_TAG, "Connecting to " MACSTR " as the best recovery candidate", MAC2STR(bss->bssid));
    }
    roaming_app_mark_roam_attempt();
#if LEGACY_ROAM_ENABLED
    g_roaming_app.force_roam_ongoing = true;
#endif /*LEGACY_ROAM_ENABLED*/
    return true;
}

void roaming_app_trigger_roam(struct cand_bss *bss)
{
    struct timeval now;

    if (!g_roaming_app.app_active) {
        goto free_bss;
    }

    gettimeofday(&now, NULL);
    ESP_LOGD(ROAMING_TAG, "Processing trigger roaming request.");
    if (g_roaming_app.sta_connected && roaming_app_roam_backoff_active(&now, 0)) {
        ESP_LOGD(ROAMING_TAG, "Ignoring request as time difference to last roam attempt is %ld",
                 time_diff_sec(&now, &g_roaming_app.last_roam_attempt_time));
        goto free_bss;
    }
#if NETWORK_ASSISTED_ROAMING_ENABLED
    if (g_roaming_app.sta_connected && g_roaming_app.config.btm_roaming_enabled && g_roaming_app.current_bss.btm_support) {
#if LEGACY_ROAM_ENABLED && NETWORK_ASSISTED_ROAMING_ENABLED
        if (g_roaming_app.btm_attempt <= g_roaming_app.config.btm_retry_cnt) {
#endif
            if (trigger_network_assisted_roam(bss)) {
                goto free_bss;
            }
#if LEGACY_ROAM_ENABLED && NETWORK_ASSISTED_ROAMING_ENABLED
        } else {
            ESP_LOGD(ROAMING_TAG, "Not Sending BTM query as this method has failed too many times.");
            g_roaming_app.btm_attempt = 0;
        }
#endif
    }
#endif /*NETWORK_ASSISTED_ROAMING_ENABLED*/
    if (!g_roaming_app.sta_connected) {
        if (trigger_targeted_reconnect(bss, false)) {
            goto free_bss;
        }
    }
#if LEGACY_ROAM_ENABLED
    if (g_roaming_app.sta_connected && g_roaming_app.config.legacy_roam_enabled) {
        if (trigger_targeted_reconnect(bss, true)) {
            goto free_bss;
        }
    }
#endif /*LEGACY_ROAM_ENABLED*/
free_bss :
    os_free(bss);
}

static void roaming_app_trigger_roam_internal_handler(void *ctx, void *data)
{
    roaming_app_untrack_timeout_user_data(data);

    if (!g_roaming_app.app_active) {
        if (data) {
            os_free(data);
        }
        return;
    }

    if (!data) {
        ESP_LOGE(ROAMING_TAG, "No data received for roaming event");
    } else {
        roaming_app_trigger_roam((struct cand_bss *)data);
    }
}

static int wifi_post_roam_event(struct cand_bss *bss)
{
    if (!g_roaming_app.app_active) {
        return -1;
    }

    if (bss) {
        struct cand_bss *cand_bss = (struct cand_bss *)os_zalloc(sizeof(struct cand_bss));
        if (!cand_bss) {
            ESP_LOGE(ROAMING_TAG, "Cannot allocate data for roaming event");
            return -1;
        }
        os_memcpy(cand_bss, bss, sizeof(struct cand_bss));
        /* trigger the roaming event */
        if (eloop_register_timeout(0, 0, roaming_app_trigger_roam_internal_handler, NULL, (void *)cand_bss)) {
            ESP_LOGE(ROAMING_TAG, "Could not register roaming event.");
            os_free(cand_bss);
            return -1;
        }
        roaming_app_track_timeout_user_data(cand_bss);
    } else {
        ESP_LOGE(ROAMING_TAG, "Cannot trigger roaming event without any candidate APs");
        return -1;
    }
    return 0;

}

void print_ap_records(struct scanned_ap_info *ap_info)
{
    ESP_LOGD(ROAMING_TAG, "Scanned AP List");
    for (int i = 0; i < ap_info->current_count ; i++) {
        ESP_LOGD(ROAMING_TAG, "%d. ssid : %s bssid :"MACSTR"  channel : %d rssi : %d authmode : %d", i,
                 ap_info->ap_records[i].ssid, MAC2STR(ap_info->ap_records[i].bssid),
                 ap_info->ap_records[i].primary, ap_info->ap_records[i].rssi, ap_info->ap_records[i].authmode);
    }

}

#if PERIODIC_RRM_MONITORING
static void periodic_rrm_request(struct timeval *now)
{
    if (!roaming_app_get_ap_info(&g_roaming_app.current_bss.ap)) {
        return;
    }
    if (esp_rrm_is_rrm_supported_connection() && (g_roaming_app.current_bss.ap.rssi < g_roaming_app.config.rrm_monitor_rssi_threshold)) {
        if (esp_rrm_send_neighbor_report_request() < 0) {
            ESP_LOGE(ROAMING_TAG, "failed to send neighbor report request");
            return;
        }
        g_roaming_app.rrm_request_active = true;
    }
}
#endif

static bool candidate_security_match(wifi_ap_record_t candidate)
{
    const u8 *rsn_ie = esp_wifi_sta_get_ie((u8 *) candidate.bssid, WLAN_EID_RSN);
    const u8 *rsnxe_ie = roaming_app_get_rsnxe_ie(candidate.bssid);
    struct wpa_ie_data rsn_data = {0};
    struct wpa_ie_data rsnxe_data = {0};
    bool candidate_has_rsn = false;
    bool candidate_has_rsnxe = false;
    u8 transition_disable = wpa_supplicant_get_transition_disable();
    wifi_auth_mode_t curr_auth = g_roaming_app.current_bss.ap.authmode;
    wifi_auth_mode_t cand_auth = candidate.authmode;
    wifi_config_t wifi_cfg = {0};
    bool candidate_supports_pmf;
    bool candidate_supports_sae;
    bool candidate_supports_owe;
    bool candidate_supports_wpa3_enterprise;
    bool candidate_supports_suite_b_192;
    bool candidate_supports_sae_pk;
    bool candidate_supports_sae_h2e;

    if (esp_wifi_get_config(WIFI_IF_STA, &wifi_cfg) != ESP_OK) {
        ESP_LOGW(ROAMING_TAG, "failed to read sta config while validating candidate security");
        return false;
    }

    if (rsn_ie) {
        if (wpa_parse_wpa_ie_rsn(rsn_ie, rsn_ie[1] + 2, &rsn_data) != 0) {
            ESP_LOGW(ROAMING_TAG, "Skipping " MACSTR " due to invalid RSN IE", MAC2STR(candidate.bssid));
            return false;
        }
        candidate_has_rsn = true;
    }

    if (rsnxe_ie) {
        if (wpa_parse_wpa_ie_rsnxe(rsnxe_ie, rsnxe_ie[1] + 2, &rsnxe_data) != 0) {
            ESP_LOGW(ROAMING_TAG, "Skipping " MACSTR " due to invalid/incompatible RSNXE", MAC2STR(candidate.bssid));
            return false;
        }
        candidate_has_rsnxe = true;
    }

    candidate_supports_pmf = candidate_has_rsn && (rsn_data.capabilities & WPA_CAPABILITY_MFPC);
    candidate_supports_sae = candidate_has_rsn ? wpa_key_mgmt_sae(rsn_data.key_mgmt) :
                             (cand_auth == WIFI_AUTH_WPA3_PSK || cand_auth == WIFI_AUTH_WPA2_WPA3_PSK);
    candidate_supports_owe = candidate_has_rsn ? wpa_key_mgmt_owe(rsn_data.key_mgmt) :
                             (cand_auth == WIFI_AUTH_OWE);
    candidate_supports_wpa3_enterprise = candidate_has_rsn ?
                                         (!!(rsn_data.key_mgmt & (WPA_KEY_MGMT_IEEE8021X_SHA256 |
                                                                  WPA_KEY_MGMT_IEEE8021X_SUITE_B |
                                                                  WPA_KEY_MGMT_IEEE8021X_SUITE_B_192)) &&
                                          candidate_supports_pmf) :
                                         ((cand_auth == WIFI_AUTH_WPA3_ENTERPRISE ||
                                           cand_auth == WIFI_AUTH_WPA2_WPA3_ENTERPRISE) &&
                                          candidate_supports_pmf);
    candidate_supports_suite_b_192 = candidate_has_rsn ? wpa_key_mgmt_sha384(rsn_data.key_mgmt) :
                                     (cand_auth == WIFI_AUTH_WPA3_ENT_192);
    candidate_supports_sae_pk = candidate_has_rsnxe &&
                                (rsnxe_data.rsnxe_capa & BIT(WLAN_RSNX_CAPAB_SAE_PK));
    candidate_supports_sae_h2e = candidate_has_rsnxe &&
                                 (rsnxe_data.rsnxe_capa & BIT(WLAN_RSNX_CAPAB_SAE_H2E));

    if (wifi_cfg.sta.pmf_cfg.required && !candidate_supports_pmf) {
        ESP_LOGV(ROAMING_TAG, "Rejecting " MACSTR " as PMF is required", MAC2STR(candidate.bssid));
        return false;
    }

    if (wifi_cfg.sta.sae_pk_mode == WPA3_SAE_PK_MODE_ONLY &&
            candidate_supports_sae && !candidate_supports_sae_pk) {
        ESP_LOGV(ROAMING_TAG, "Rejecting " MACSTR " as SAE-PK is required", MAC2STR(candidate.bssid));
        return false;
    }

    if (wifi_cfg.sta.sae_pwe_h2e == WPA3_SAE_PWE_HASH_TO_ELEMENT &&
            candidate_supports_sae && !candidate_supports_sae_h2e) {
        ESP_LOGV(ROAMING_TAG, "Rejecting " MACSTR " as SAE H2E is required", MAC2STR(candidate.bssid));
        return false;
    }

    if (curr_auth == WIFI_AUTH_WPA3_ENT_192 &&
            (!candidate_supports_suite_b_192 || !candidate_supports_pmf)) {
        return false;
    }

    /* Transition Disable is a learned AP policy and should always be honored
     * during candidate selection to avoid downgrade roams. */
    if (transition_disable & TRANSITION_DISABLE_WPA3_PERSONAL) {
        if (!candidate_supports_sae) {
            return false;
        }
    }
    if ((transition_disable & TRANSITION_DISABLE_SAE_PK) && !candidate_supports_sae_pk) {
        return false;
    }
    if (transition_disable & TRANSITION_DISABLE_ENHANCED_OPEN) {
        if (!candidate_supports_owe) {
            return false;
        }
    }
    if (transition_disable & TRANSITION_DISABLE_WPA3_ENTERPRISE) {
        if (!candidate_supports_wpa3_enterprise) {
            return false;
        }
    }

    if (!roaming_app_candidate_meets_threshold(wifi_cfg.sta.threshold.authmode,
                                               curr_auth,
                                               cand_auth,
                                               candidate_supports_sae,
                                               candidate_supports_owe,
                                               candidate_supports_wpa3_enterprise,
                                               candidate_supports_suite_b_192)) {
        ESP_LOGV(ROAMING_TAG, "Authmode threshold failure %d -> %d",
                 wifi_cfg.sta.threshold.authmode, cand_auth);
        return false;
    }

    if (roaming_app_authmode_is_open_mode(curr_auth) && roaming_app_authmode_is_open_mode(cand_auth)) {
        if ((curr_auth == WIFI_AUTH_OWE || candidate_supports_owe) && !wifi_cfg.sta.owe_enabled) {
            ESP_LOGV(ROAMING_TAG, "Open/OWE transition rejected as OWE is disabled");
            return false;
        }
    }

    ESP_LOGV(ROAMING_TAG, "Cand authmode : %d, Current Authmode : %d", cand_auth, curr_auth);
    if (cand_auth == curr_auth) {
        ESP_LOGV(ROAMING_TAG, "Authmode matched!");
        return true;
    }

    if (roaming_app_authmode_is_open_mode(curr_auth) && roaming_app_authmode_is_open_mode(cand_auth)) {
        ESP_LOGV(ROAMING_TAG, "Roaming between open modes");
        return true;
    }

    if (roaming_app_authmode_is_personal_compatible(curr_auth, cand_auth)) {
        ESP_LOGV(ROAMING_TAG, "Roaming between personal modes");
        return true;
    }

    if (roaming_app_authmode_is_enterprise(curr_auth) &&
            roaming_app_authmode_is_enterprise(cand_auth)) {
        ESP_LOGV(ROAMING_TAG, "Roaming between enterprise modes");
        return true;
    }

    return false;
}

#if CONFIG_ESP_WIFI_ROAMING_BSSID_BLACKLIST
static void remove_expired_blacklist_entries(void)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    int j = 0;
    for (int i = 0; i < g_roaming_app.bssid_blacklist_count; i++) {
        if (time_diff_sec(&now, &g_roaming_app.bssid_blacklist[i].timestamp) <= CONFIG_ESP_WIFI_ROAMING_BLACKLIST_TIMEOUT) {
            if (i != j) {
                g_roaming_app.bssid_blacklist[j] = g_roaming_app.bssid_blacklist[i];
            }
            j++;
        } else {
            ESP_LOGI(ROAMING_TAG, "BSSID " MACSTR " removed from blacklist due to timeout", MAC2STR(g_roaming_app.bssid_blacklist[i].bssid));
        }
    }
    g_roaming_app.bssid_blacklist_count = j;
}
#endif

static bool is_bssid_blacklisted(const uint8_t *bssid)
{
#if CONFIG_ESP_WIFI_ROAMING_BSSID_BLACKLIST
    struct blacklist_entry *entry;

    remove_expired_blacklist_entries();
    entry = roaming_app_find_blacklist_entry(bssid);
    return roaming_app_blacklist_entry_blocks_roaming(entry);
#endif
    (void) bssid;
    return false;
}

static void parse_scan_results_and_roam(void)
{
    struct timeval now;
    const wifi_ap_record_t *current_scan_record = g_roaming_app.sta_connected ?
                                                  roaming_app_find_scan_record(g_roaming_app.current_bss.ap.bssid) :
                                                  NULL;
    int8_t rssi_threshold = g_roaming_app.current_rssi_threshold;
    int16_t current_rssi_baseline = g_roaming_app.current_bss.ap.rssi;
    int16_t best_rssi_diff = rssi_threshold;
    struct cand_bss *best_ap = NULL;
    int16_t rssi_diff = 0;
    uint8_t i;
    int8_t best_ap_index = -1;

    gettimeofday(&now, NULL);

    if (current_scan_record && current_scan_record->rssi > current_rssi_baseline) {
        current_rssi_baseline = current_scan_record->rssi;
    }

    for (i = 0; i < g_roaming_app.scanned_aps.current_count; i++) {
        if (is_bssid_blacklisted(g_roaming_app.scanned_aps.ap_records[i].bssid)) {
            ESP_LOGD(ROAMING_TAG, "BSSID " MACSTR " is blacklisted, skipping", MAC2STR(g_roaming_app.scanned_aps.ap_records[i].bssid));
            continue;
        }
        rssi_diff = g_roaming_app.scanned_aps.ap_records[i].rssi - current_rssi_baseline;
        ESP_LOGD(ROAMING_TAG, "The difference between ("MACSTR", "MACSTR") with rssi (%d,%d) is : %d while the threshold is %d and the best rssi diff yet is %d, thecand_auth is %d",
                              MAC2STR(g_roaming_app.scanned_aps.ap_records[i].bssid),MAC2STR(g_roaming_app.current_bss.ap.bssid),
                              g_roaming_app.scanned_aps.ap_records[i].rssi, current_rssi_baseline,
                              rssi_diff, rssi_threshold, best_rssi_diff, g_roaming_app.scanned_aps.ap_records[i].authmode);
        if ((memcmp(g_roaming_app.scanned_aps.ap_records[i].bssid, g_roaming_app.current_bss.ap.bssid, ETH_ALEN) != 0) &&
             candidate_security_match(g_roaming_app.scanned_aps.ap_records[i]) && rssi_diff > best_rssi_diff ) {
            best_rssi_diff = rssi_diff;
            best_ap_index = i;
        }
    }
    if (best_ap_index >= 0) {
        best_ap = (struct cand_bss*)os_zalloc(sizeof(struct cand_bss));
        if (!best_ap) {
            ESP_LOGE(ROAMING_TAG,"Memory Allocation for candidate bss failed");
            return;
        }
        os_memcpy(best_ap->bssid,g_roaming_app.scanned_aps.ap_records[best_ap_index].bssid , ETH_ALEN);
        best_ap->channel = g_roaming_app.scanned_aps.ap_records[best_ap_index].primary;
    }

    if (best_ap) {
        if (!roaming_app_can_act_on_candidate(&now, true)) {
            os_free(best_ap);
            return;
        }
        ESP_LOGI(ROAMING_TAG,"Found a better AP "MACSTR" at channel %d", MAC2STR(best_ap->bssid), best_ap->channel);
        if (wifi_post_roam_event(best_ap)) {
            ESP_LOGE(ROAMING_TAG, "Posting of roaming event failed");
        }
        os_free(best_ap);
    } else {
        ESP_LOGI(ROAMING_TAG, "Could not find a better AP with the threshold set to %d", g_roaming_app.current_rssi_threshold + 1);
#if CONFIG_ESP_WIFI_ROAMING_BSSID_BLACKLIST
        roaming_app_retry_blacklist_recovery_if_needed();
#endif /*CONFIG_ESP_WIFI_ROAMING_BSSID_BLACKLIST*/
    }
}

static void scan_done_event_handler(void *arg, ETS_STATUS status)
{
    if (status == ETS_OK) {
        esp_err_t err;

        ESP_LOGD(ROAMING_TAG, "Scan Done properly");
        g_roaming_app.scanned_aps.current_count = MAX_CANDIDATE_COUNT;
        err = esp_wifi_scan_get_ap_records(&g_roaming_app.scanned_aps.current_count, g_roaming_app.scanned_aps.ap_records);
        if (err != ESP_OK) {
            g_roaming_app.scanned_aps.current_count = 0;
            ESP_LOGW(ROAMING_TAG, "Failed to fetch scan results: %s", esp_err_to_name(err));
            g_roaming_app.scan_ongoing = false;
#if CONFIG_ESP_WIFI_ROAMING_BSSID_BLACKLIST
            roaming_app_retry_blacklist_recovery_if_needed();
#endif /*CONFIG_ESP_WIFI_ROAMING_BSSID_BLACKLIST*/
            return;
        }
        gettimeofday(&g_roaming_app.scanned_aps.time, NULL);
        print_ap_records(&g_roaming_app.scanned_aps);
        parse_scan_results_and_roam();
    } else {
        ESP_LOGD(ROAMING_TAG, "Scan Done with error %d ", status);
#if CONFIG_ESP_WIFI_ROAMING_BSSID_BLACKLIST
        roaming_app_retry_blacklist_recovery_if_needed();
#endif /*CONFIG_ESP_WIFI_ROAMING_BSSID_BLACKLIST*/
    }
    g_roaming_app.scan_ongoing = false;
}
static bool conduct_scan(void)
{
    /* Issue scan */
    if (esp_wifi_promiscuous_scan_start(&g_roaming_app.config.scan_config, scan_done_event_handler) != ESP_OK) {
        ESP_LOGE(ROAMING_TAG, "failed to issue scan");
        return false;
    }
    ESP_LOGI(ROAMING_TAG, "Issued Scan");
    return true;
}

static void determine_best_ap(int8_t rssi_threshold)
{
    struct timeval now;

    if (!g_roaming_app.app_active) {
        return;
    }

    gettimeofday(&now, NULL);
    if (!roaming_app_can_act_on_candidate(&now, true)) {
        return;
    }

    /* If the scan results are recent enough or a scan is already ongoing we should not trigger a new scan */
    if (!g_roaming_app.scan_ongoing) {
        g_roaming_app.scan_ongoing = true;
        g_roaming_app.current_rssi_threshold = rssi_threshold;
        if (!roaming_app_scan_cache_is_valid(&now)) {
            if (!conduct_scan()) {
                g_roaming_app.scan_ongoing = false;
#if CONFIG_ESP_WIFI_ROAMING_BSSID_BLACKLIST
                roaming_app_retry_blacklist_recovery_if_needed();
#endif /*CONFIG_ESP_WIFI_ROAMING_BSSID_BLACKLIST*/
            }
        } else {
            parse_scan_results_and_roam();
            g_roaming_app.scan_ongoing = false;
        }
    } else if(rssi_threshold < g_roaming_app.current_rssi_threshold) {
        g_roaming_app.current_rssi_threshold = rssi_threshold;
    }
}
#if PERIODIC_SCAN_MONITORING
static void periodic_scan_roam(struct timeval *now)
{
#if NETWORK_ASSISTED_ROAMING_ENABLED && !LEGACY_ROAM_ENABLED
    /*
    * In case we support only network assisted roaming, there is no need to scan
        * if we currently have over 10secs to go for backoff time to expire
        * as the results produced by a scan at this time would not be used by
    * supplicant to build candidate lists.
    * */
    if (roaming_app_roam_backoff_active(now, SUPPLICANT_CANDIDATE_LIST_EXPIRY)) {
        return;
    }
#endif /*NETWORK_ASSISTED_ROAMING_ENABLED && !LEGACY_ROAM_ENABLED*/
    if (!roaming_app_connected_roam_method_available()) {
        return;
    }
        /* If the current RSSI is not worse than the configured threshold
     * for station initiated roam, then do not trigger roam */
    if (!roaming_app_get_ap_info(&g_roaming_app.current_bss.ap)) {
        return;
    }
    ESP_LOGD(ROAMING_TAG, "Connected AP's RSSI=%d", g_roaming_app.current_bss.ap.rssi);
    if (g_roaming_app.current_bss.ap.rssi > g_roaming_app.config.scan_rssi_threshold) {
        ESP_LOGD(ROAMING_TAG, "Not going for scan, Scan RSSI threshold=%d", g_roaming_app.config.scan_rssi_threshold);
        return;
    }

    determine_best_ap(g_roaming_app.config.scan_rssi_diff - 1);
}
#endif /*PERIODIC_SCAN_MONITORING*/

#if PERIODIC_RRM_MONITORING
static void roaming_app_periodic_rrm_internal_handler(void *data, void *ctx)
{
    struct timeval now;

    if (!g_roaming_app.app_active) {
        return;
    }

    if (!g_roaming_app.config.rrm_monitor) {
        ESP_LOGI(ROAMING_TAG, "%s:RRM monitor is disabled in config", __func__);
        return;
    }

    if (g_roaming_app.periodic_rrm_active) {
        ESP_LOGD(ROAMING_TAG,"Triggered periodic rrm event!");
        gettimeofday(&now, NULL);
        /* This will be done every g_roaming_app.config.rrm_monitor_time */
        periodic_rrm_request(&now);

        if (eloop_register_timeout(g_roaming_app.config.rrm_monitor_time, 0, roaming_app_periodic_rrm_internal_handler, NULL, NULL)) {
            ESP_LOGE(ROAMING_TAG,"Could not register periodic neighbor report request event.");
        }
    }
}
#endif /*PERIODIC_RRM_MONITORING*/

#if PERIODIC_SCAN_MONITORING
static void roaming_app_periodic_scan_internal_handler(void *data, void *ctx)
{
    struct timeval now;

    if (!g_roaming_app.app_active) {
        return;
    }

    if (!g_roaming_app.config.scan_monitor) {
        ESP_LOGI(ROAMING_TAG, "%s: Scan monitor is disabled in config", __func__);
        return;
    }

    if (g_roaming_app.periodic_scan_active) {
        ESP_LOGD(ROAMING_TAG,"Started the periodic scan roam event!");
        gettimeofday(&now, NULL);

        /* This will be done every g_roaming_app.config.scan_interval */
        periodic_scan_roam(&now);

        if (eloop_register_timeout(g_roaming_app.config.scan_interval, 0, roaming_app_periodic_scan_internal_handler, NULL, NULL)) {
            ESP_LOGE(ROAMING_TAG,"Could not register periodic scan event.");
        }
    }
}
#endif /*PERIODIC_SCAN_MONITORING*/
static bool validate_scan_chan_list(const char* scan_chan_list)
{
    regex_t regex;
    const char* pattern = "^[0-9]+(,[0-9]+)*$";
    uint8_t ret = regcomp(&regex, pattern, REG_EXTENDED);
    if (ret != 0) {
        ESP_LOGE(ROAMING_TAG, "Regex compilation failed.");
        return false;
    }

    ret = regexec(&regex, scan_chan_list, 0, NULL, 0);
    regfree(&regex);

    if (ret == REG_NOMATCH) {
        return false;
    }

    /* Check for consecutive commas or comma at start/end */
    if (strstr(scan_chan_list, ",,") != NULL || scan_chan_list[0] == ',' || scan_chan_list[strlen(scan_chan_list) - 1] == ',') {
        return false;
    }

    return true;
}

static int8_t parse_scan_chan_list(void)
{
    int8_t ret = 0;
    char *scan_chan_string = NULL;
    char *scan_chan_string_p = NULL;
    if (validate_scan_chan_list(SCAN_PREFERRED_CHAN_LIST) == false) {
        ESP_LOGE(ROAMING_TAG, "scan chan list validation failed.");
        ret = -1;
        goto cleanup;
    }
    scan_chan_string = (char *)os_zalloc(sizeof(char) * strlen(SCAN_PREFERRED_CHAN_LIST) + 1);
    if (scan_chan_string == NULL) {
        ESP_LOGE(ROAMING_TAG, "Memory allocation failed.");
        ret = -1;
        goto cleanup;
    }
    strlcpy(scan_chan_string, SCAN_PREFERRED_CHAN_LIST, strlen(SCAN_PREFERRED_CHAN_LIST) + 1);
    char* token;
    scan_chan_string_p = scan_chan_string;
    token = strsep(&scan_chan_string_p, ",");

    g_roaming_app.config.scan_config.channel_bitmap.ghz_2_channels = 0;
    g_roaming_app.config.scan_config.channel_bitmap.ghz_5_channels = 0;

    while (token != NULL) {
        uint8_t channel = atoi(token);
        /* Check if the number is within the required range */
        if (channel >= 1 && channel <= 14) {
            g_roaming_app.config.scan_config.channel_bitmap.ghz_2_channels |= (1 << channel);
#if CONFIG_SOC_WIFI_SUPPORT_5G
        } else if (channel >= 36 && channel <= 177) {
            g_roaming_app.config.scan_config.channel_bitmap.ghz_5_channels |= CHANNEL_TO_BIT(channel);
#endif
        } else {
            ESP_LOGE(ROAMING_TAG, "Channel out of range: %d", channel);
            ret = -1;
            goto cleanup;
        }
        token = strsep(&scan_chan_string_p, ",");
    }

cleanup:
    if (scan_chan_string) {
        os_free(scan_chan_string);
    }
    return ret;
}

static esp_err_t init_config_params(void)
{
    g_roaming_app.config.backoff_time = ROAMING_BACKOFF_TIME;

    g_roaming_app.config.low_rssi_roam_trigger = LOW_RSSI_ROAMING_ENABLED;
    g_roaming_app.config.low_rssi_threshold = ROAMING_LOW_RSSI_THRESHOLD;
    g_roaming_app.config.rssi_threshold_reduction_offset = RSSI_THRESHOLD_REDUCTION_OFFSET;

    g_roaming_app.config.scan_monitor = PERIODIC_SCAN_MONITORING;
#if PERIODIC_SCAN_MONITORING
    g_roaming_app.config.scan_interval = SCAN_MONITOR_INTERVAL;
    g_roaming_app.config.scan_rssi_threshold = SCAN_MONITOR_RSSI_THRESHOLD;
    g_roaming_app.config.scan_rssi_diff = SCAN_ROAM_RSSI_DIFF;
#endif /* PERIODIC_SCAN_MONITORING */

    g_roaming_app.config.legacy_roam_enabled = LEGACY_ROAM_ENABLED;
    g_roaming_app.config.btm_retry_cnt = BSS_TM_RETRY_COUNT;
    g_roaming_app.config.btm_roaming_enabled = NETWORK_ASSISTED_ROAMING_ENABLED;

    g_roaming_app.config.rrm_monitor = PERIODIC_RRM_MONITORING;
    g_roaming_app.config.rrm_monitor_time = RRM_MONITOR_TIME;
    g_roaming_app.config.rrm_monitor_rssi_threshold = RRM_MONITOR_RSSI_THRESHOLD;

    ESP_LOGD(ROAMING_TAG, "Roaming app config :");

    ESP_LOGD(ROAMING_TAG, "backoff time=%d low_rssi_roam_trigger=%d low_rssi_threshold=%d rssi_threshold_reduction_offset=%d",
                            g_roaming_app.config.backoff_time, g_roaming_app.config.low_rssi_roam_trigger,
                            g_roaming_app.config.low_rssi_threshold, g_roaming_app.config.rssi_threshold_reduction_offset);

#if PERIODIC_SCAN_MONITORING
    ESP_LOGD(ROAMING_TAG, "scan_monitor=%d scan_interval=%d scan_rssi_threshold=%d scan_rssi_diff=%d",
                        g_roaming_app.config.scan_monitor, g_roaming_app.config.scan_interval,
                        g_roaming_app.config.scan_rssi_threshold, g_roaming_app.config.scan_rssi_diff);
#endif /* PERIODIC_SCAN_MONITORING */

    ESP_LOGD(ROAMING_TAG, "legacy_roam_enabled=%d, btm_retry_cnt=%d btm_roaming_enabled=%d",
                        g_roaming_app.config.legacy_roam_enabled,
                        g_roaming_app.config.btm_retry_cnt, g_roaming_app.config.btm_roaming_enabled);

    ESP_LOGD(ROAMING_TAG, "rrm_monitor=%d, rrm_monitor_time=%d rrm_monitor_rssi_threshold=%d",
                        g_roaming_app.config.rrm_monitor,
                        g_roaming_app.config.rrm_monitor_time,
                        g_roaming_app.config.rrm_monitor_rssi_threshold);

    return ESP_OK;
}

static esp_err_t init_scan_config(void)
{
    if (strcmp(DEFAULT_PREFERRED_SCAN_CHAN_LIST, SCAN_PREFERRED_CHAN_LIST)) {
        ESP_ERROR_CHECK(parse_scan_chan_list());
    }

    g_roaming_app.config.scan_config.scan_type = 0;
    g_roaming_app.config.scan_config.scan_time.active.min = SCAN_TIME_MIN_DURATION;
    g_roaming_app.config.scan_config.scan_time.active.max = SCAN_TIME_MAX_DURATION;
    g_roaming_app.config.scan_config.home_chan_dwell_time = HOME_CHANNEL_DWELL_TIME;
    roaming_app_refresh_scan_config_filters();
    return ESP_OK;
}

void roam_init_app(void)
{
#if !LOW_RSSI_ROAMING_ENABLED && !PERIODIC_SCAN_MONITORING
    ESP_LOGE(ROAMING_TAG, "No roaming trigger enabled. Roaming app cannot be initialized");
    return;
#endif

#if !NETWORK_ASSISTED_ROAMING_ENABLED && !LEGACY_ROAM_ENABLED
    ESP_LOGE(ROAMING_TAG, "No roaming method enabled. Roaming app cannot be initialized");
    return;
#endif
    memset(&g_roaming_app, 0, sizeof(g_roaming_app));
    g_roaming_app.app_active = true;
#if LOW_RSSI_ROAMING_ENABLED
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_BSS_RSSI_LOW,
                                               &roaming_app_rssi_low_handler, NULL));
#endif /*LOW_RSSI_ROAMING_ENABLED*/
    ESP_ERROR_CHECK(init_scan_config());
    ESP_ERROR_CHECK(init_config_params());
#if PERIODIC_RRM_MONITORING
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_NEIGHBOR_REP,
                                               &roaming_app_neighbor_report_recv_handler, NULL));
#endif /*PERIODIC_RRM_MONITORING*/
    ESP_LOGI(ROAMING_TAG, "Roaming app initialization done");
}

static void roaming_app_cancel_pending_events(void)
{
    eloop_cancel_timeout(roaming_app_connected_event_handler, ELOOP_ALL_CTX, ELOOP_ALL_CTX);
    eloop_cancel_timeout(roaming_app_disconnected_event_handler, ELOOP_ALL_CTX, ELOOP_ALL_CTX);
    eloop_cancel_timeout(roaming_app_trigger_roam_internal_handler, ELOOP_ALL_CTX, ELOOP_ALL_CTX);
    eloop_cancel_timeout(enable_reconnect, ELOOP_ALL_CTX, ELOOP_ALL_CTX);
    eloop_cancel_timeout(disable_reconnect, ELOOP_ALL_CTX, ELOOP_ALL_CTX);
#if LOW_RSSI_ROAMING_ENABLED
    eloop_cancel_timeout(roaming_app_rssi_low_internal_handler, ELOOP_ALL_CTX, ELOOP_ALL_CTX);
#endif /*LOW_RSSI_ROAMING_ENABLED*/
#if PERIODIC_RRM_MONITORING
    eloop_cancel_timeout(roaming_app_neighbor_report_recv_internal_handler, ELOOP_ALL_CTX, ELOOP_ALL_CTX);
#endif /*PERIODIC_RRM_MONITORING*/
#if CONFIG_ESP_WIFI_ROAMING_BSSID_BLACKLIST
    eloop_cancel_timeout(roaming_app_blacklist_recovery_internal_handler, ELOOP_ALL_CTX, ELOOP_ALL_CTX);
    eloop_cancel_timeout(roaming_app_blacklist_add_handler, ELOOP_ALL_CTX, ELOOP_ALL_CTX);
    eloop_cancel_timeout(roaming_app_blacklist_remove_handler, ELOOP_ALL_CTX, ELOOP_ALL_CTX);
#endif /*CONFIG_ESP_WIFI_ROAMING_BSSID_BLACKLIST*/
    roaming_app_free_tracked_timeout_user_data();
}

static int roaming_app_deinit_internal(void *ctx, void *data)
{
    (void) ctx;
    (void) data;
    g_roaming_app.app_active = false;
    roaming_app_cancel_pending_events();
    roaming_app_stop_periodic_monitors();
    roaming_app_reset_connect_hint_state();
#if PERIODIC_RRM_MONITORING
    if (g_roaming_app.btm_neighbor_list) {
        os_free(g_roaming_app.btm_neighbor_list);
        g_roaming_app.btm_neighbor_list = NULL;
    }
#endif /*PERIODIC_RRM_MONITORING*/
    return ESP_OK;
}

void roam_deinit_app(void)
{
#if !LOW_RSSI_ROAMING_ENABLED && !PERIODIC_SCAN_MONITORING
    ESP_LOGE(ROAMING_TAG, "No roaming trigger enabled. Roaming app cannot be de-initialized");
    return;
#endif

#if !NETWORK_ASSISTED_ROAMING_ENABLED && !LEGACY_ROAM_ENABLED
    ESP_LOGE(ROAMING_TAG, "No roaming trigger enabled. Roaming app cannot be de-initialized");
    return;
#endif

    if (!g_roaming_app.app_active) {
        return;
    }

    if (roaming_app_run_blocking(roaming_app_deinit_internal, NULL) != ESP_OK) {
        ESP_LOGW(ROAMING_TAG, "Failed to deinitialize roaming app on Wi-Fi task");
        return;
    }

#if LOW_RSSI_ROAMING_ENABLED
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_BSS_RSSI_LOW,
                                                 &roaming_app_rssi_low_handler));
#endif /*LOW_RSSI_ROAMING_ENABLED*/
#if PERIODIC_RRM_MONITORING
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_NEIGHBOR_REP,
                                                 &roaming_app_neighbor_report_recv_handler));
#endif /*PERIODIC_RRM_MONITORING*/
}

#if CONFIG_ESP_WIFI_ROAMING_BSSID_BLACKLIST
static void roaming_app_blacklist_add_handler(void *ctx, void *data)
{
    uint8_t *bssid = data;
    roaming_app_untrack_timeout_user_data(bssid);

    if (!g_roaming_app.app_active) {
        if (bssid) {
            os_free(bssid);
        }
        return;
    }

    if (!roaming_app_add_manual_blacklist_entry(bssid)) {
        ESP_LOGE(ROAMING_TAG, "Blacklist is full");
        os_free(bssid);
        return;
    }
    ESP_LOGI(ROAMING_TAG, "BSSID " MACSTR " added to blacklist", MAC2STR(bssid));
    os_free(bssid);
}

esp_err_t esp_wifi_blacklist_add(const uint8_t *bssid)
{
    if (!g_roaming_app.app_active) {
        return ESP_ERR_INVALID_STATE;
    }

    if (!bssid) {
        return ESP_ERR_INVALID_ARG;
    }
    uint8_t *bssid_copy = os_malloc(ETH_ALEN);
    if (!bssid_copy) {
        return ESP_ERR_NO_MEM;
    }
    memcpy(bssid_copy, bssid, ETH_ALEN);
    if (eloop_register_timeout(0, 0, roaming_app_blacklist_add_handler, NULL, bssid_copy) != 0) {
        os_free(bssid_copy);
        return ESP_FAIL;
    }
    roaming_app_track_timeout_user_data(bssid_copy);
    return ESP_OK;
}

static void roaming_app_blacklist_remove_handler(void *ctx, void *data)
{
    uint8_t *bssid = data;
    int found_index;
    roaming_app_untrack_timeout_user_data(bssid);

    if (!g_roaming_app.app_active) {
        if (bssid) {
            os_free(bssid);
        }
        return;
    }

    found_index = roaming_app_find_blacklist_entry_index(bssid);

    if (found_index != -1) {
        // Shift elements to fill the gap
        int remaining_entries = g_roaming_app.bssid_blacklist_count - found_index - 1;
        if (remaining_entries > 0) {
            memmove(&g_roaming_app.bssid_blacklist[found_index], &g_roaming_app.bssid_blacklist[found_index + 1], remaining_entries * sizeof(struct blacklist_entry));
        }
        g_roaming_app.bssid_blacklist_count--;
        ESP_LOGI(ROAMING_TAG, "BSSID " MACSTR " removed from blacklist", MAC2STR(bssid));
    } else {
        ESP_LOGD(ROAMING_TAG, "BSSID " MACSTR " not found in blacklist", MAC2STR(bssid));
    }
    os_free(bssid);
}

esp_err_t esp_wifi_blacklist_remove(const uint8_t *bssid)
{
    if (!g_roaming_app.app_active) {
        return ESP_ERR_INVALID_STATE;
    }

    if (!bssid) {
        return ESP_ERR_INVALID_ARG;
    }
    uint8_t *bssid_copy = os_malloc(ETH_ALEN);
    if (!bssid_copy) {
        return ESP_ERR_NO_MEM;
    }
    memcpy(bssid_copy, bssid, ETH_ALEN);
    if (eloop_register_timeout(0, 0, roaming_app_blacklist_remove_handler, NULL, bssid_copy) != 0) {
        os_free(bssid_copy);
        return ESP_FAIL;
    }
    roaming_app_track_timeout_user_data(bssid_copy);
    return ESP_OK;
}
#endif

static int roaming_app_get_config_params_internal(void *ctx, void *data)
{
    (void) ctx;
    struct roam_config *config = data;

    memcpy(config, &g_roaming_app.config, sizeof(*config));
    config->scan_config.ssid = config->scan_filter_ssid[0] != '\0' ? config->scan_filter_ssid : NULL;
    config->scan_config.bssid = config->scan_filter_bssid_set ? config->scan_filter_bssid : NULL;

    return ESP_OK;
}

esp_err_t roam_get_config_params(struct roam_config *config)
{
    if (!config) {
        return ESP_ERR_INVALID_ARG;
    }

    return roaming_app_run_blocking(roaming_app_get_config_params_internal, config);
}

static int update_config_params(void *data)
{
    struct roam_config *config = data;
    uint8_t next_scan_filter_ssid[ROAM_SCAN_FILTER_SSID_LEN] = {0};
    uint8_t next_scan_filter_bssid[ROAM_SCAN_FILTER_BSSID_LEN] = {0};
    bool next_scan_filter_bssid_set = false;
    bool filters_changed;

    if (config->scan_filter_ssid[0] != '\0') {
        strlcpy((char *) next_scan_filter_ssid,
                (const char *) config->scan_filter_ssid,
                sizeof(next_scan_filter_ssid));
    }

    if (config->scan_filter_bssid_set) {
        memcpy(next_scan_filter_bssid, config->scan_filter_bssid, sizeof(next_scan_filter_bssid));
        next_scan_filter_bssid_set = true;
    }

    filters_changed = strcmp((const char *) g_roaming_app.config.scan_filter_ssid,
                             (const char *) next_scan_filter_ssid) != 0 ||
                      g_roaming_app.config.scan_filter_bssid_set != next_scan_filter_bssid_set ||
                      (next_scan_filter_bssid_set &&
                       memcmp(g_roaming_app.config.scan_filter_bssid,
                              next_scan_filter_bssid,
                              sizeof(next_scan_filter_bssid)) != 0);

    g_roaming_app.config = *config;
    memset(g_roaming_app.config.scan_filter_ssid, 0, sizeof(g_roaming_app.config.scan_filter_ssid));
    memset(g_roaming_app.config.scan_filter_bssid, 0, sizeof(g_roaming_app.config.scan_filter_bssid));
    g_roaming_app.config.scan_filter_bssid_set = false;

    if (next_scan_filter_ssid[0] != '\0') {
        strlcpy((char *) g_roaming_app.config.scan_filter_ssid,
                (const char *) next_scan_filter_ssid,
                sizeof(g_roaming_app.config.scan_filter_ssid));
    }

    if (next_scan_filter_bssid_set) {
        memcpy(g_roaming_app.config.scan_filter_bssid,
               next_scan_filter_bssid,
               sizeof(g_roaming_app.config.scan_filter_bssid));
        g_roaming_app.config.scan_filter_bssid_set = true;
    }

    roaming_app_refresh_scan_config_filters();
    if (filters_changed) {
        roaming_app_invalidate_scan_cache();
    }
    roaming_app_apply_runtime_config();

    ESP_LOGI(ROAMING_TAG, "Updated Roaming app config :");

    ESP_LOGI(ROAMING_TAG, "backoff time=%d low_rssi_roam_trigger=%d low_rssi_threshold=%d rssi_threshold_reduction_offset=%d",
                            g_roaming_app.config.backoff_time, g_roaming_app.config.low_rssi_roam_trigger,
                            g_roaming_app.config.low_rssi_threshold, g_roaming_app.config.rssi_threshold_reduction_offset);

#if PERIODIC_SCAN_MONITORING
    ESP_LOGI(ROAMING_TAG, "scan_monitor=%d scan_interval=%d scan_rssi_threshold=%d scan_rssi_diff=%d",
                        g_roaming_app.config.scan_monitor, g_roaming_app.config.scan_interval,
                        g_roaming_app.config.scan_rssi_threshold, g_roaming_app.config.scan_rssi_diff);
#endif

    ESP_LOGI(ROAMING_TAG, "legacy_roam_enabled=%d, btm_retry_cnt=%d btm_roaming_enabled=%d",
                        g_roaming_app.config.legacy_roam_enabled,
                        g_roaming_app.config.btm_retry_cnt, g_roaming_app.config.btm_roaming_enabled);

    ESP_LOGI(ROAMING_TAG, "rrm_monitor=%d, rrm_monitor_time=%d rrm_monitor_rssi_threshold=%d",
                        g_roaming_app.config.rrm_monitor,
                        g_roaming_app.config.rrm_monitor_time,
                        g_roaming_app.config.rrm_monitor_rssi_threshold);

    return ESP_OK;
}

static void roaming_app_normalize_config_filters(struct roam_config *config)
{
    if (!config) {
        return;
    }

    if (config->scan_filter_ssid[0] == '\0' && config->scan_config.ssid) {
        strlcpy((char *) config->scan_filter_ssid,
                (const char *) config->scan_config.ssid,
                sizeof(config->scan_filter_ssid));
    }

    if (!config->scan_filter_bssid_set && config->scan_config.bssid) {
        memcpy(config->scan_filter_bssid,
               config->scan_config.bssid,
               sizeof(config->scan_filter_bssid));
        config->scan_filter_bssid_set = true;
    }

    config->scan_config.ssid = NULL;
    config->scan_config.bssid = NULL;
}

esp_err_t roam_set_config_params(struct roam_config *config)
{
    wifi_ipc_config_t cfg;
    struct roam_config config_copy;

    if (!config) {
        return ESP_ERR_INVALID_ARG;
    }

    config_copy = *config;
    roaming_app_normalize_config_filters(&config_copy);

    cfg.fn = update_config_params;
    cfg.arg = &config_copy;
    cfg.arg_size = sizeof(config_copy);

    return esp_wifi_ipc_internal(&cfg, false);
}
