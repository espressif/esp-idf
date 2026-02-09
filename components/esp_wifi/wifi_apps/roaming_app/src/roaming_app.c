/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
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
#include "common/ieee802_11_defs.h"
#include "common/wpa_common.h"
#include "esp_wpas_glue.h"

struct roaming_app g_roaming_app;

typedef void (* scan_done_cb_t)(void *arg, ETS_STATUS status);
extern int esp_wifi_promiscuous_scan_start(wifi_scan_config_t *config, scan_done_cb_t cb);

static int wifi_post_roam_event(struct cand_bss *bss);
static void determine_best_ap(int8_t rssi_threshold);
static bool is_bssid_blacklisted(const uint8_t *bssid);
#if PERIODIC_RRM_MONITORING
static void roaming_app_periodic_rrm_internal_handler(void *data, void *ctx);
#endif
#if PERIODIC_SCAN_MONITORING
static void roaming_app_periodic_scan_internal_handler(void *data, void *ctx);
#endif

static const char *ROAMING_TAG = "ROAM";

void esp_wifi_roaming_set_current_bssid(const uint8_t *bssid)
{
    if (bssid) {
        memcpy(g_roaming_app.current_bss.ap.bssid, bssid, ETH_ALEN);
    }
}

static inline long time_diff_sec(struct timeval *a, struct timeval *b)
{
    return (a->tv_sec - b->tv_sec);
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

static void roaming_app_get_ap_info(wifi_ap_record_t *ap_info)
{
    esp_wifi_sta_get_ap_info(ap_info);
#if LOW_RSSI_ROAMING_ENABLED
    /*
     * If the current rssi is below the configured rssi threshold for
     * low rssi based roaming and the current rssi threshold is below that,
     * we should reset the rssi threshold back to the configured rssi threshold */
    if ((ap_info->rssi > g_roaming_app.config.low_rssi_threshold) && (g_roaming_app.current_low_rssi_threshold < g_roaming_app.config.low_rssi_threshold)) {
        g_roaming_app.current_low_rssi_threshold = g_roaming_app.config.low_rssi_threshold;
        esp_wifi_set_rssi_threshold(g_roaming_app.config.low_rssi_threshold);
        ESP_LOGD(ROAMING_TAG, "Reset the low rssi threshold back to %d", g_roaming_app.config.low_rssi_threshold);
    }
#endif /*LOW_RSSI_ROAMING_ENABLED*/
}

#if LEGACY_ROAM_ENABLED
static void legacy_roam_clear_bssid_flag(void)
{
    wifi_config_t *config = {0};

    config = os_zalloc(sizeof(wifi_config_t));
    if (!config) {
        ESP_LOGE(ROAMING_TAG, "failed to allocate memory");
        return;
    }

    esp_wifi_get_config(WIFI_IF_STA, config);
    if (config->sta.bssid_set) {
        config->sta.bssid_set = 0;
        esp_wifi_set_config(WIFI_IF_STA, config);
    }
    os_free(config);
    ESP_LOGD(ROAMING_TAG, "cleared bssid flag");
}
#endif /*LEGACY_ROAM_ENABLED*/

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
    g_roaming_app.periodic_rrm_active = true;
    if (eloop_register_timeout(g_roaming_app.config.rrm_monitor_time, 0, roaming_app_periodic_rrm_internal_handler, NULL, NULL)) {
        ESP_LOGE(ROAMING_TAG, "Could not register periodic neighbor report event.");
    }
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
    g_roaming_app.periodic_scan_active = true;
    if (eloop_register_timeout(g_roaming_app.config.scan_interval, 0, roaming_app_periodic_scan_internal_handler, NULL, NULL)) {
        ESP_LOGE(ROAMING_TAG, "Could not register periodic scan monitoring event");
    }
}
#endif /*PERIODIC_SCAN_ROAM_MONITORING*/

static void roaming_app_disconnected_event_handler(void *ctx, void *data)
{
#if PERIODIC_RRM_MONITORING
    g_roaming_app.periodic_rrm_active = false;
#endif /*PERIODIC_RRM_MONITORING*/

#if PERIODIC_SCAN_MONITORING
    g_roaming_app.periodic_scan_active = false;
#endif /*PERIODIC_SCAN_MONITORING*/

    wifi_event_sta_disconnected_t *disconn = data;
#define RSSI_INVALID -128
    g_roaming_app.current_bss.ap.rssi = RSSI_INVALID;
#undef RSSI_INVALID

    ESP_LOGD(ROAMING_TAG, "station got disconnected reason=%d, rssi =%d", disconn->reason, disconn->rssi);
#if CONFIG_ESP_WIFI_ROAMING_AUTO_BLACKLISTING
    if (disconn->reason == WIFI_REASON_CONNECTION_FAIL || disconn->reason == WIFI_REASON_AUTH_FAIL) {
        bool found = false;
        for (int i = 0; i < g_roaming_app.bssid_blacklist_count; i++) {
            if (memcmp(g_roaming_app.bssid_blacklist[i].bssid, g_roaming_app.current_bss.ap.bssid, ETH_ALEN) == 0) {
                g_roaming_app.bssid_blacklist[i].failures++;
                gettimeofday(&g_roaming_app.bssid_blacklist[i].timestamp, NULL);
                ESP_LOGD(ROAMING_TAG, "BSSID " MACSTR " connection failures: %d", MAC2STR(g_roaming_app.current_bss.ap.bssid), g_roaming_app.bssid_blacklist[i].failures);
                if (g_roaming_app.bssid_blacklist[i].failures >= CONFIG_ESP_WIFI_ROAMING_MAX_CONN_FAILURES) {
                    ESP_LOGI(ROAMING_TAG, "BSSID " MACSTR " blacklisted", MAC2STR(g_roaming_app.current_bss.ap.bssid));
                    /* The BSSID is now blacklisted. The logic later in this function
                     * will check the blacklist and trigger a scan for a new AP
                     * instead of trying to reconnect to this one. */
                }
                found = true;
                break;
            }
        }
        if (!found) {
            if (g_roaming_app.bssid_blacklist_count < CONFIG_ESP_WIFI_ROAMING_MAX_CANDIDATES) {
                memcpy(g_roaming_app.bssid_blacklist[g_roaming_app.bssid_blacklist_count].bssid, g_roaming_app.current_bss.ap.bssid, ETH_ALEN);
                g_roaming_app.bssid_blacklist[g_roaming_app.bssid_blacklist_count].failures = 1;
                gettimeofday(&g_roaming_app.bssid_blacklist[g_roaming_app.bssid_blacklist_count].timestamp, NULL);
                g_roaming_app.bssid_blacklist_count++;
                ESP_LOGD(ROAMING_TAG, "BSSID " MACSTR " added to blacklist tracking", MAC2STR(g_roaming_app.current_bss.ap.bssid));
            } else {
                int oldest_idx = 0;
                for (int i = 1; i < g_roaming_app.bssid_blacklist_count; i++) {
                    if (g_roaming_app.bssid_blacklist[i].timestamp.tv_sec < g_roaming_app.bssid_blacklist[oldest_idx].timestamp.tv_sec) {
                        oldest_idx = i;
                    }
                }
                ESP_LOGD(ROAMING_TAG, "Blacklist is full. Replacing oldest entry for " MACSTR, MAC2STR(g_roaming_app.bssid_blacklist[oldest_idx].bssid));
                memcpy(g_roaming_app.bssid_blacklist[oldest_idx].bssid, g_roaming_app.current_bss.ap.bssid, ETH_ALEN);
                g_roaming_app.bssid_blacklist[oldest_idx].failures = 1;
                gettimeofday(&g_roaming_app.bssid_blacklist[oldest_idx].timestamp, NULL);
                ESP_LOGD(ROAMING_TAG, "BSSID " MACSTR " added to blacklist tracking", MAC2STR(g_roaming_app.current_bss.ap.bssid));
            }
        }
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
            esp_wifi_connect();
#if CONFIG_ESP_WIFI_ROAMING_BSSID_BLACKLIST
        }
#endif
    }
    os_free(disconn);
}

static void roaming_app_connected_event_handler(void *ctx, void *data)
{
    roaming_app_get_ap_info(&g_roaming_app.current_bss.ap);
    g_roaming_app.config.scan_config.ssid = g_roaming_app.current_bss.ap.ssid;
#if LOW_RSSI_ROAMING_ENABLED
    if (g_roaming_app.current_bss.ap.rssi < g_roaming_app.config.low_rssi_threshold) {
        /* To ensure that the threshold is set to one offset below the current AP RSSI
         * in case, the AP is already below the RSSI threshold */
        g_roaming_app.current_low_rssi_threshold =  g_roaming_app.current_bss.ap.rssi - g_roaming_app.config.rssi_threshold_reduction_offset;
    } else {
        g_roaming_app.current_low_rssi_threshold = g_roaming_app.config.low_rssi_threshold;
    }
    ESP_LOGD(ROAMING_TAG, "setting rssi threshold as %d", g_roaming_app.current_low_rssi_threshold);
    esp_wifi_set_rssi_threshold(g_roaming_app.current_low_rssi_threshold);
#endif /*LOW_RSSI_ROAMING_ENABLED*/
    g_roaming_app.current_bss.rrm_support = esp_rrm_is_rrm_supported_connection();
    g_roaming_app.current_bss.btm_support = esp_wnm_is_btm_supported_connection();
    ESP_LOGD(ROAMING_TAG, "Station connected, RRM %ssupported, BTM %ssupported",
             g_roaming_app.current_bss.rrm_support ? " " : "not ",
             g_roaming_app.current_bss.btm_support ? " " : "not ");
    gettimeofday(&g_roaming_app.last_roamed_time, NULL);
    if (!initialize_roaming_event()) {
#if PERIODIC_RRM_MONITORING
        if (g_roaming_app.current_bss.rrm_support) {
            init_periodic_rrm_event();
        }
#endif /*PERIODIC_RRM_MONITORING*/
#if PERIODIC_SCAN_MONITORING
        init_periodic_scan_roam_event();
#endif /*PERIODIC_SCAN_ROAM_MONITORING*/
        ESP_LOGD(ROAMING_TAG, "Initialised initialise roaming events!");
    } else {
        ESP_LOGE(ROAMING_TAG, "Failed to Initialise roaming events");
    }
#if LEGACY_ROAM_ENABLED
    g_roaming_app.force_roam_ongoing = false;
#endif /*LEGACY_ROAM_ENABLED*/
    g_roaming_app.allow_reconnect = true;
}

void roam_sta_connected(void)
{
    eloop_register_timeout(0, 0, roaming_app_connected_event_handler, NULL, NULL);
}

void roam_sta_disconnected(void *data)
{
    wifi_event_sta_disconnected_t *disconn = os_malloc(sizeof(*disconn));

    if (!disconn) {
        return;
    }
    os_memcpy(disconn, data, sizeof(*disconn));
    if (eloop_register_timeout(0, 0, roaming_app_disconnected_event_handler, NULL, disconn) != 0) {
        os_free(disconn);
    }
}

#define MAX_NEIGHBOR_LEN 512
#if PERIODIC_RRM_MONITORING
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

        while (end - pos > 2) {
            uint8_t s_len;
            s_len = *pos++;
            if (s_len > end - pos) {
                ret = -1;
                goto cleanup;
            }
            pos += s_len;
        }
        ESP_LOGD(ROAMING_TAG, "RMM neighbor report bssid=" MACSTR
                 " info=0x%" PRIx32 " op_class=%u chan=%u phy_type=%u",
                 MAC2STR(nr), WPA_GET_LE32(nr + ETH_ALEN),
                 nr[ETH_ALEN + 4], nr[ETH_ALEN + 5],
                 nr[ETH_ALEN + 6]);
        /* neighbor start */
        len += snprintf(buf + len, MAX_NEIGHBOR_LEN - len, " neighbor=");
        /* bssid */
        len += snprintf(buf + len, MAX_NEIGHBOR_LEN - len, MACSTR, MAC2STR(nr));
        /* , */
        len += snprintf(buf + len, MAX_NEIGHBOR_LEN - len, ",");
        /* bssid info */
        len += snprintf(buf + len, MAX_NEIGHBOR_LEN - len, "0x%04" PRIx32 "", WPA_GET_LE32(nr + ETH_ALEN));
        len += snprintf(buf + len, MAX_NEIGHBOR_LEN - len, ",");
        /* operating class */
        len += snprintf(buf + len, MAX_NEIGHBOR_LEN - len, "%u", nr[ETH_ALEN + 4]);
        len += snprintf(buf + len, MAX_NEIGHBOR_LEN - len, ",");
        /* channel number */
        len += snprintf(buf + len, MAX_NEIGHBOR_LEN - len, "%u", nr[ETH_ALEN + 5]);
        len += snprintf(buf + len, MAX_NEIGHBOR_LEN - len, ",");
        /* phy type */
        len += snprintf(buf + len, MAX_NEIGHBOR_LEN - len, "%u", nr[ETH_ALEN + 6]);
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
    uint8_t report_len = neighbor_report_event->report_len;
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
    }
}
#endif /*PERIODIC_RRM_MONITORING*/

#if LOW_RSSI_ROAMING_ENABLED
static void roaming_app_rssi_low_internal_handler(void *ctx, void *data)
{
    wifi_event_bss_rssi_low_t *event = data;
    ESP_LOGI(ROAMING_TAG, "%s:bss rssi is=%ld", __func__, event->rssi);

    roaming_app_get_ap_info(&g_roaming_app.current_bss.ap);
    determine_best_ap(0);
    g_roaming_app.current_low_rssi_threshold -=  g_roaming_app.config.rssi_threshold_reduction_offset;
    ESP_LOGD(ROAMING_TAG, "Resetting RSSI Threshold to %d", g_roaming_app.current_low_rssi_threshold);
    esp_wifi_set_rssi_threshold(g_roaming_app.current_low_rssi_threshold);

    os_free(event);
}

static void roaming_app_rssi_low_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    wifi_event_bss_rssi_low_t *event = event_data;
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
    }
}
#endif

#if NETWORK_ASSISTED_ROAMING_ENABLED
static void trigger_network_assisted_roam(void)
{
    if (esp_wnm_send_bss_transition_mgmt_query(REASON_RSSI, g_roaming_app.btm_neighbor_list, 1) < 0) {
        ESP_LOGD(ROAMING_TAG, "failed to send btm query");
    }
    ESP_LOGD(ROAMING_TAG, "Sent BTM Query");
    gettimeofday(&g_roaming_app.last_roamed_time, NULL);
#if LEGACY_ROAM_ENABLED
    g_roaming_app.btm_attempt++;
#endif
}
#endif /*NETWORK_ASSISTED_ROAMING*/

#if LEGACY_ROAM_ENABLED
static void trigger_legacy_roam(struct cand_bss *bss)
{
    wifi_config_t wifi_cfg = {0};
    esp_wifi_get_config(WIFI_IF_STA, &wifi_cfg);
    wifi_cfg.sta.channel = bss->channel;
    wifi_cfg.sta.bssid_set = true;
    os_memcpy(wifi_cfg.sta.bssid, bss->bssid, ETH_ALEN);
    esp_wifi_internal_issue_disconnect(WIFI_REASON_BSS_TRANSITION_DISASSOC);
    esp_wifi_roaming_set_current_bssid(bss->bssid);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg);
    esp_wifi_connect();
    ESP_LOGI(ROAMING_TAG, "Disconnecting and connecting to "MACSTR" on account of better rssi",MAC2STR(bss->bssid));
    gettimeofday(&g_roaming_app.last_roamed_time, NULL);
    g_roaming_app.force_roam_ongoing = true;
}
#endif /*LEGACY_ROAM_ENABLED*/

void roaming_app_trigger_roam(struct cand_bss *bss)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    ESP_LOGD(ROAMING_TAG,"Processing trigger roaming request.");
    if (time_diff_sec(&now, &g_roaming_app.last_roamed_time) < g_roaming_app.config.backoff_time ) {
        ESP_LOGD(ROAMING_TAG,"Ignoring request as time difference to last request is %ld",time_diff_sec(&now, &g_roaming_app.last_roamed_time));
        goto free_bss;
    }
#if NETWORK_ASSISTED_ROAMING_ENABLED
    if (g_roaming_app.config.btm_roaming_enabled && g_roaming_app.current_bss.btm_support) {
#if LEGACY_ROAM_ENABLED && NETWORK_ASSISTED_ROAMING_ENABLED
        if (g_roaming_app.btm_attempt <= g_roaming_app.config.btm_retry_cnt) {
#endif
            trigger_network_assisted_roam();
            goto free_bss;
#if LEGACY_ROAM_ENABLED && NETWORK_ASSISTED_ROAMING_ENABLED
        } else {
            ESP_LOGD(ROAMING_TAG, "Not Sending BTM query as this method has failed too many times.");
                        g_roaming_app.btm_attempt = 0;
        }
#endif
    }
#endif /*NETWORK_ASSISTED_ROAMING_ENABLED*/
#if LEGACY_ROAM_ENABLED
    if (g_roaming_app.config.legacy_roam_enabled) {
        trigger_legacy_roam(bss);
    }
#endif /*LEGACY_ROAM_ENABLED*/
free_bss :
    os_free(bss);
}

void roaming_app_trigger_roam_internal_handler(void *ctx, void *data)
{
    if (!data) {
        ESP_LOGE(ROAMING_TAG, "No data received for roaming event");
    } else {
        roaming_app_trigger_roam((struct cand_bss *)data);
    }
}

static int wifi_post_roam_event(struct cand_bss *bss)
{
    if (bss) {
        struct cand_bss *cand_bss = (struct cand_bss *)os_zalloc(sizeof(struct cand_bss));
        if (!cand_bss) {
            ESP_LOGE(ROAMING_TAG, "Cannot allocate data for roaming event");
            return -1;
        }
        os_memcpy(cand_bss, bss, sizeof(struct cand_bss));
        /* trigger the roaming event */
        if (eloop_register_timeout(0, 0, roaming_app_trigger_roam_internal_handler, NULL, (void*)cand_bss)) {
            ESP_LOGE(ROAMING_TAG, "Could not register roaming event.");
            os_free(cand_bss);
            return -1;
        }
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
                 ap_info->ap_records[i].ssid,MAC2STR(ap_info->ap_records[i].bssid),
                 ap_info->ap_records[i].primary,ap_info->ap_records[i].rssi, ap_info->ap_records[i].authmode);
    }

}

#if PERIODIC_RRM_MONITORING
static void periodic_rrm_request(struct timeval *now)
{
    roaming_app_get_ap_info(&g_roaming_app.current_bss.ap);
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
#if CONFIG_ESP_WIFI_ROAMING_PREVENT_DOWNGRADE
    u8 transition_disable = wpa_supplicant_get_transition_disable();
    if (transition_disable & TRANSITION_DISABLE_WPA3_PERSONAL) {
        if (candidate.authmode == WIFI_AUTH_WPA2_PSK) {
            return false;
        }
    }
    if (transition_disable & TRANSITION_DISABLE_ENHANCED_OPEN) {
        if (candidate.authmode == WIFI_AUTH_OPEN) {
            return false;
        }
    }
    if (transition_disable & TRANSITION_DISABLE_WPA3_ENTERPRISE) {
        if (candidate.authmode == WIFI_AUTH_WPA2_ENTERPRISE) {
            return false;
        }
    }
#if TODO // application doesn't have a way to know SAE-PK enabled AP atm
    if (transition_disable & TRANSITION_DISABLE_SAE_PK) {
        if (candidate.authmode == WIFI_AUTH_WPA3_PSK) {
            return false;
        }
    }
#endif
#endif
    wifi_auth_mode_t curr_auth = g_roaming_app.current_bss.ap.authmode;
    wifi_auth_mode_t cand_auth = candidate.authmode;
    ESP_LOGV(ROAMING_TAG, "Cand authmode : %d, Current Authmode : %d", cand_auth, curr_auth);
    if (cand_auth == curr_auth) {
        ESP_LOGV(ROAMING_TAG, "Authmode matched!");
        return true;
    }
    wifi_config_t wifi_cfg = {0};
    esp_wifi_get_config(WIFI_IF_STA, &wifi_cfg);
    if (wifi_cfg.sta.owe_enabled && OWE_COMPATIBLE(curr_auth, cand_auth)) {
        if (wifi_cfg.sta.threshold.authmode == WIFI_AUTH_OPEN) {
            ESP_LOGV(ROAMING_TAG, "transition between OWE and open permitted");
            return true;
        } else {
            ESP_LOGV(ROAMING_TAG, "transition between OWE and open not permitted");
            return false;
        }
    } else if (wifi_cfg.sta.threshold.authmode > cand_auth) {
        /* If the authmode of the candidate AP is less than our threshold, it
         * will fail during connection */
        ESP_LOGV(ROAMING_TAG, "Authmode threshold failure %d -> %d", wifi_cfg.sta.threshold.authmode, cand_auth);
        return false;
    } else if (PSK_COMPATIBLE(curr_auth, cand_auth)) {
        /*
         * PSK based authmodes are compatible with each other for roaming
         */
        ESP_LOGV(ROAMING_TAG, "Roaming between a PSK APs");
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
    remove_expired_blacklist_entries();
    for (int i = 0; i < g_roaming_app.bssid_blacklist_count; i++) {
        if (memcmp(g_roaming_app.bssid_blacklist[i].bssid, bssid, ETH_ALEN) == 0) {
#if CONFIG_ESP_WIFI_ROAMING_AUTO_BLACKLISTING
            if (g_roaming_app.bssid_blacklist[i].failures >= CONFIG_ESP_WIFI_ROAMING_MAX_CONN_FAILURES) {
                return true;
            }
#else
            return true;
#endif
        }
    }
#endif
    return false;
}

static void parse_scan_results_and_roam(void)
{
    int8_t rssi_threshold = g_roaming_app.current_rssi_threshold;
    uint8_t best_rssi_diff = rssi_threshold;
    struct cand_bss *best_ap = NULL;
    int8_t rssi_diff = 0;
    uint8_t i;
    int8_t best_ap_index = -1;
    for (i = 0; i < g_roaming_app.scanned_aps.current_count; i++) {
        if (is_bssid_blacklisted(g_roaming_app.scanned_aps.ap_records[i].bssid)) {
            ESP_LOGD(ROAMING_TAG, "BSSID " MACSTR " is blacklisted, skipping", MAC2STR(g_roaming_app.scanned_aps.ap_records[i].bssid));
            continue;
        }
        rssi_diff = g_roaming_app.scanned_aps.ap_records[i].rssi - g_roaming_app.current_bss.ap.rssi;
        ESP_LOGD(ROAMING_TAG, "The difference between ("MACSTR", "MACSTR") with rssi (%d,%d) is : %d while the threshold is %d and the best rssi diff yet is %d, thecand_auth is %d",
                              MAC2STR(g_roaming_app.scanned_aps.ap_records[i].bssid),MAC2STR(g_roaming_app.current_bss.ap.bssid),
                              g_roaming_app.scanned_aps.ap_records[i].rssi, g_roaming_app.current_bss.ap.rssi,
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
        ESP_LOGI(ROAMING_TAG,"Found a better AP "MACSTR" at channel %d", MAC2STR(best_ap->bssid), best_ap->channel);
        if (wifi_post_roam_event(best_ap)) {
            ESP_LOGE(ROAMING_TAG, "Posting of roaming event failed");
        }
        os_free(best_ap);
    } else {
        ESP_LOGI(ROAMING_TAG, "Could not find a better AP with the threshold set to %d", g_roaming_app.current_rssi_threshold + 1);
    }
}

static void scan_done_event_handler(void *arg, ETS_STATUS status)
{
    if (status == ETS_OK) {
        ESP_LOGD(ROAMING_TAG, "Scan Done properly");
        g_roaming_app.scanned_aps.current_count = MAX_CANDIDATE_COUNT;
        esp_wifi_scan_get_ap_records(&g_roaming_app.scanned_aps.current_count, g_roaming_app.scanned_aps.ap_records);
        print_ap_records(&g_roaming_app.scanned_aps);
        parse_scan_results_and_roam();
    } else {
        ESP_LOGD(ROAMING_TAG, "Scan Done with error %d ", status);
    }
    g_roaming_app.scan_ongoing = false;
}
static bool conduct_scan(void)
{
    gettimeofday(&g_roaming_app.scanned_aps.time, NULL);
    os_memset(&g_roaming_app.scanned_aps, 0, sizeof(struct scanned_ap_info));
    /* Issue scan */
    if (esp_wifi_promiscuous_scan_start(&g_roaming_app.config.scan_config, scan_done_event_handler) < 0) {
        ESP_LOGE(ROAMING_TAG, "failed to issue scan");
        return false;
    }
    ESP_LOGI(ROAMING_TAG, "Issued Scan");
    return true;
}

static void determine_best_ap(int8_t rssi_threshold)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    /* If the scan results are recent enough or a scan is already ongoing we should not trigger a new scan */
    if (!g_roaming_app.scan_ongoing) {
        g_roaming_app.scan_ongoing = true;
        g_roaming_app.current_rssi_threshold = rssi_threshold;
        if (time_diff_sec(&now,&g_roaming_app.scanned_aps.time) > SCAN_RESULTS_USABILITY_WINDOW) {
            if (!conduct_scan()) {
                g_roaming_app.scan_ongoing = false;
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
    if (time_diff_sec(now, &g_roaming_app.last_roamed_time) < g_roaming_app.config.backoff_time - SUPPLICANT_CANDIDATE_LIST_EXPIRY) {
        return;
    }
#endif /*NETWORK_ASSISTED_ROAMING_ENABLED && !LEGACY_ROAM_ENABLED*/
        /* If the current RSSI is not worse than the configured threshold
     * for station initiated roam, then do not trigger roam */
    roaming_app_get_ap_info(&g_roaming_app.current_bss.ap);
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
#endif /*PERIODIC_SCAN_ROAM_MONITORING*/
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
    gettimeofday(&g_roaming_app.scanned_aps.time, NULL);
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
#if LOW_RSSI_ROAMING_ENABLED
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_BSS_RSSI_LOW,
                                                 &roaming_app_rssi_low_handler));
#endif /*LOW_RSSI_ROAMING_ENABLED*/

#if PERIODIC_SCAN_MONITORING
    g_roaming_app.periodic_scan_active = false;
    eloop_cancel_timeout(roaming_app_periodic_scan_internal_handler, NULL, NULL);
#endif /*PERIODIC_SCAN_MONITORING*/
#if PERIODIC_RRM_MONITORING
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_NEIGHBOR_REP,
                                                 &roaming_app_neighbor_report_recv_handler));
    /* Disabling the periodic scan and RRM events */
    g_roaming_app.periodic_rrm_active = false;
    eloop_cancel_timeout(roaming_app_periodic_rrm_internal_handler, NULL, NULL);
    if (g_roaming_app.btm_neighbor_list) {
        os_free(g_roaming_app.btm_neighbor_list);
        g_roaming_app.btm_neighbor_list = NULL;
    }
#endif /*PERIODIC_RRM_MONITORING*/
}

#if CONFIG_ESP_WIFI_ROAMING_BSSID_BLACKLIST
static void roaming_app_blacklist_add_handler(void *ctx, void *data)
{
    uint8_t *bssid = data;
    if (g_roaming_app.bssid_blacklist_count >= CONFIG_ESP_WIFI_ROAMING_MAX_CANDIDATES) {
        ESP_LOGE(ROAMING_TAG, "Blacklist is full");
        os_free(bssid);
        return;
    }
    for (int i = 0; i < g_roaming_app.bssid_blacklist_count; i++) {
        if (memcmp(g_roaming_app.bssid_blacklist[i].bssid, bssid, ETH_ALEN) == 0) {
            ESP_LOGD(ROAMING_TAG, "BSSID " MACSTR " already in blacklist", MAC2STR(bssid));
            os_free(bssid);
            return; // Already blacklisted
        }
    }
    memcpy(g_roaming_app.bssid_blacklist[g_roaming_app.bssid_blacklist_count].bssid, bssid, ETH_ALEN);
#if CONFIG_ESP_WIFI_ROAMING_AUTO_BLACKLISTING
    g_roaming_app.bssid_blacklist[g_roaming_app.bssid_blacklist_count].failures = CONFIG_ESP_WIFI_ROAMING_MAX_CONN_FAILURES;
#endif
    gettimeofday(&g_roaming_app.bssid_blacklist[g_roaming_app.bssid_blacklist_count].timestamp, NULL);
    g_roaming_app.bssid_blacklist_count++;
    ESP_LOGI(ROAMING_TAG, "BSSID " MACSTR " added to blacklist", MAC2STR(bssid));
    os_free(bssid);
}

esp_err_t esp_wifi_blacklist_add(const uint8_t *bssid)
{
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
    return ESP_OK;
}

static void roaming_app_blacklist_remove_handler(void *ctx, void *data)
{
    uint8_t *bssid = data;
    int found_index = -1;
    for (int i = 0; i < g_roaming_app.bssid_blacklist_count; i++) {
        if (memcmp(g_roaming_app.bssid_blacklist[i].bssid, bssid, ETH_ALEN) == 0) {
            found_index = i;
            break;
        }
    }

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
    return ESP_OK;
}
#endif

/* No need for this to be done in pptask ctx */
esp_err_t roam_get_config_params(struct roam_config *config)
{
    memcpy(config, &g_roaming_app.config, sizeof(*config));

    return ESP_OK;
}

static int update_config_params(void *data)
{
    struct roam_config *config = data;
    g_roaming_app.config = *config;

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

esp_err_t roam_set_config_params(struct roam_config *config)
{
    wifi_ipc_config_t cfg;

    cfg.fn = update_config_params;
    cfg.arg = config;
    cfg.arg_size = sizeof(*config);
    esp_wifi_ipc_internal(&cfg, false);

    return ESP_OK;
}
