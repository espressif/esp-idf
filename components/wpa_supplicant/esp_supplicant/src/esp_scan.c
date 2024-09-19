/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "common/ieee802_11_defs.h"
#include "common/wpa_supplicant_i.h"
#include "utils/wpa_debug.h"
#include "esp_wifi_driver.h"
#include "esp_wifi_types.h"
#include "drivers/driver.h"
#include "common/scan.h"
#include "common/bss.h"
#include "common/rrm.h"
#include "common/ieee802_11_common.h"
#include "esp_common_i.h"
#include "esp_scan_i.h"
#include "common/wnm_sta.h"
#include "esp_scan_i.h"
#include "esp_common_i.h"

extern struct wpa_supplicant g_wpa_supp;

static void scan_done_event_handler(void *arg, ETS_STATUS status)
{
    struct wpa_supplicant *wpa_s = &g_wpa_supp;

    /* update last scan time */
    wpa_s->scan_start_tsf = esp_wifi_get_tsf_time(WIFI_IF_STA);
    if (wpa_s->scanning) {
        wpa_s->type &= ~(1 << WLAN_FC_STYPE_BEACON) & ~(1 << WLAN_FC_STYPE_PROBE_RESP);
        esp_wifi_register_mgmt_frame_internal(wpa_s->type, wpa_s->subtype);
    }
#ifdef CONFIG_SUPPLICANT_TASK
    if (esp_supplicant_post_evt(SIG_SUPPLICANT_SCAN_DONE, 0) != 0) {
        wpa_printf(MSG_ERROR, "Posting of scan done failed!");
    }
#else
    esp_supplicant_handle_scan_done_evt();
#endif /*CONFIG_SUPPLICANT_TASK*/
}

#if defined(CONFIG_IEEE80211KV)
static void handle_wnm_scan_done(struct wpa_supplicant *wpa_s)
{
    struct wpa_bss *bss = wpa_bss_get_next_bss(wpa_s, wpa_s->current_bss);

    if (wpa_s->wnm_neighbor_report_elements) {
        wnm_scan_process(wpa_s, 1);
    } else if (wpa_s->wnm_dissoc_timer) {
        if (wpa_s->num_bss == 1) {
            wpa_printf(MSG_INFO, "not able to find another candidate, do nothing");
            return;
        }
        /* this is a already matched bss */
        if (bss) {
            wnm_bss_tm_connect(wpa_s, bss, NULL, 1);
        }
    }
}
#endif

static void scan_done_cleanup(struct wpa_supplicant *wpa_s)
{
    wpa_s->scanning = 0;
    wpa_s->scan_reason = 0;
    /* clean scan list from net80211 */
    esp_wifi_clear_ap_list();
}

void esp_supplicant_handle_scan_done_evt(void)
{
    struct wpa_supplicant *wpa_s = &g_wpa_supp;

    wpa_printf(MSG_INFO, "scan done received");
#if defined(CONFIG_IEEE80211KV)
    /* Check which module started this, call the respective function */
    if (wpa_s->scan_reason == REASON_RRM_BEACON_REPORT) {
        wpas_beacon_rep_scan_process(wpa_s, wpa_s->scan_start_tsf);
    } else if (wpa_s->scan_reason == REASON_WNM_BSS_TRANS_REQ) {
        handle_wnm_scan_done(wpa_s);
    }
#endif
    if (wpa_s->scanning) {
        scan_done_cleanup(wpa_s);
    }
    wpa_bss_update_end(wpa_s);
#ifndef SCAN_CACHE_SUPPORTED
    wpa_bss_flush(wpa_s);
#endif
}

void esp_scan_init(struct wpa_supplicant *wpa_s)
{
    wpa_s->scanning = 0;
    wpa_bss_init(wpa_s);
    wpa_s->last_scan_res = NULL;
    wpa_s->last_scan_res_used = 0;
}

void esp_scan_deinit(struct wpa_supplicant *wpa_s)
{
    wpa_bss_deinit(wpa_s);
    os_free(wpa_s->last_scan_res);
    wpa_s->last_scan_res = NULL;
    wpa_s->last_scan_res_used = 0;
}

int esp_handle_beacon_probe(u8 type, u8 *frame, size_t len, u8 *sender,
                            int8_t rssi, u8 channel, u64 current_tsf)
{
    struct wpa_supplicant *wpa_s = &g_wpa_supp;
    struct os_reltime now;
    struct wpa_scan_res *res;
    u8 *ptr;

    if (len < 12) {
        wpa_printf(MSG_ERROR, "beacon/probe is having short len=%d", len);
        return -1;
    }

    res = os_zalloc(sizeof(struct wpa_scan_res) + len - 12);
    if (!res) {
        wpa_printf(MSG_ERROR, "failed to allocate memory");
        return -1;
    }

    ptr = (u8 *)res;
    os_get_time(&now);
    os_memcpy(res->bssid, sender, ETH_ALEN);
    res->tsf = WPA_GET_LE64(frame);
    frame += 8;
    len -= 8;

    if ((wpa_s->scan_start_tsf == 0) &&
            wpa_s->current_bss &&
            (os_memcmp(wpa_s->current_bss, sender, ETH_ALEN) == 0)) {
        wpa_s->scan_start_tsf = res->tsf;
        os_memcpy(wpa_s->tsf_bssid, sender, ETH_ALEN);
    }
    res->beacon_int = WPA_GET_LE16(frame);

    frame += 2;
    len -= 2;
    res->caps = WPA_GET_LE16(frame);
    frame += 2;
    len -= 2;

    res->chan = channel;
    res->noise = 0;
    res->level = rssi;
    os_memcpy(res->tsf_bssid, wpa_s->tsf_bssid, ETH_ALEN);
    res->parent_tsf = current_tsf - wpa_s->scan_start_tsf;
    res->ie_len = len;

    ptr += sizeof(struct wpa_scan_res);

    /* update rest of the frame */
    os_memcpy(ptr, frame, len);
    wpa_bss_update_scan_res(wpa_s, res, &now);
    os_get_reltime(&wpa_s->last_scan);
    os_free(res);

    return 0;
}

#ifdef CONFIG_WNM
static void get_scan_channel_bitmap(struct wpa_supplicant *wpa_s, wifi_scan_config_t *params)
{
    const int MAX_2GHZ_CHANNEL = 14;

    if (!wpa_s->wnm_num_neighbor_report) {
        wpa_printf(MSG_DEBUG, "No Neighbor Report to gather scan channel list");
        return;
    }
    params->channel_bitmap.ghz_2_channels = 0;
    params->channel_bitmap.ghz_5_channels = 0;

    for (int i = 0; i < wpa_s->wnm_num_neighbor_report; i++) {
        struct neighbor_report *nei = &wpa_s->wnm_neighbor_report_elements[i];

        if (nei->channel_number <= MAX_2GHZ_CHANNEL) {
            params->channel_bitmap.ghz_2_channels |= CHANNEL_TO_BIT(nei->channel_number);
        } else {
            params->channel_bitmap.ghz_5_channels |= CHANNEL_TO_BIT(nei->channel_number);
        }
    }
}
#endif /*CONFIG_WNM*/

static int issue_scan(struct wpa_supplicant *wpa_s,
                      struct wpa_driver_scan_params *scan_params)
{
    wifi_scan_config_t *params = NULL;
    int ret = 0;
    u64 scan_start_tsf = esp_wifi_get_tsf_time(WIFI_IF_STA);

    /* TODO: Directly try to connect if scan results are recent */
    if ((scan_start_tsf -  wpa_s->scan_start_tsf) > 100000) {
        wpa_printf(MSG_DEBUG, "flushing old scan cache %llu",
                   (scan_start_tsf -  wpa_s->scan_start_tsf));
        wpa_bss_flush(wpa_s);
    }

    esp_wifi_get_macaddr_internal(WIFI_IF_STA, wpa_s->tsf_bssid);

    if (scan_params) {
        params = os_zalloc(sizeof(wifi_scan_config_t));
        if (!params) {
            wpa_printf(MSG_ERROR, "failed to allocate memory");
            return -1;
        }
        if (scan_params->num_ssids) {
            params->ssid = os_zalloc(scan_params->ssids[0].ssid_len + 1);
            if (!params->ssid) {
                wpa_printf(MSG_ERROR, "failed to allocate memory");
                ret = -1;
                goto cleanup;
            }
            os_memcpy(params->ssid, scan_params->ssids[0].ssid, scan_params->ssids[0].ssid_len);
        } else

            if (scan_params->mode == BEACON_REPORT_MODE_PASSIVE) {
                params->scan_type = WIFI_SCAN_TYPE_PASSIVE;
            } else {
                params->scan_type = WIFI_SCAN_TYPE_ACTIVE;
            }

        if (scan_params->bssid) {
            params->bssid = os_zalloc(ETH_ALEN);
            if (!params->bssid) {
                wpa_printf(MSG_ERROR, "failed to allocate memory");
                ret = -1;
                goto cleanup;
            }
            os_memcpy(params->bssid, scan_params->bssid, ETH_ALEN);
        }
        if (scan_params->channel) {
            params->channel = scan_params->channel;
        }
#ifdef CONFIG_WNM
        else {
            get_scan_channel_bitmap(wpa_s, params);
        }
#endif /*CONFIG_WNM*/
        if (scan_params->duration) {
            params->scan_time.passive = scan_params->duration;
            params->scan_time.active.min = scan_params->duration;
            params->scan_time.active.max = scan_params->duration;
        } else {
            params->scan_time.active.min = SUPPLICANT_SCAN_ACTIVE_SCAN_MIN_DURATION;
            params->scan_time.active.max = SUPPLICANT_SCAN_ACTIVE_SCAN_MAX_DURATION;
        }
    }

    wpa_s->scan_start_tsf = scan_start_tsf;
    /* Register frames to come to supplicant when we park on channel */
    wpa_s->type |= (1 << WLAN_FC_STYPE_BEACON) | (1 << WLAN_FC_STYPE_PROBE_RESP);
    esp_wifi_register_mgmt_frame_internal(wpa_s->type, wpa_s->subtype);

    typedef void (* scan_done_cb_t)(void *arg, ETS_STATUS status);
    extern int esp_wifi_promiscuous_scan_start(wifi_scan_config_t *config, scan_done_cb_t cb);
    /* issue scan */
    if (esp_wifi_promiscuous_scan_start(params, scan_done_event_handler) < 0) {
        ret = -1;
        goto cleanup;
    }
    wpa_s->scanning = 1;
    wpa_bss_update_start(wpa_s);
    wpa_printf(MSG_INFO, "scan issued at time=%llu", wpa_s->scan_start_tsf);

cleanup:
    if (params) {
        if (params->ssid) {
            os_free(params->ssid);
        }
        if (params->bssid) {
            os_free(params->bssid);
        }
        os_free(params);
    }

    return ret;
}

/**
 * wpa_supplicant_trigger_scan - Request driver to start a scan
 * @wpa_s: Pointer to wpa_supplicant data
 * @params: Scan parameters
 * Returns: 0 on success, -1 on failure
 */
int wpa_supplicant_trigger_scan(struct wpa_supplicant *wpa_s,
                                struct wpa_driver_scan_params *params)
{
    return issue_scan(wpa_s, params);
}

void wpa_scan_results_free(struct wpa_scan_results *res)
{
    size_t i;

    if (res == NULL) {
        return;
    }

    for (i = 0; i < res->num; i++) {
        os_free(res->res[i]);
    }
    os_free(res->res);
    os_free(res);
}
