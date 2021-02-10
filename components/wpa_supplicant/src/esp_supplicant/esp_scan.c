/**
 * Copyright 2020 Espressif Systems (Shanghai) PTE LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
#include "esp_supplicant/esp_common_i.h"
#include "common/wnm_sta.h"

extern struct wpa_supplicant g_wpa_supp;

static void esp_scan_done_event_handler(void* arg, esp_event_base_t event_base,
					int32_t event_id, void* event_data)
{
	struct wpa_supplicant *wpa_s = &g_wpa_supp;
	if (!wpa_s->scanning) {
		/* update last scan time */
		wpa_s->scan_start_tsf = esp_wifi_get_tsf_time(WIFI_IF_STA);
		wpa_printf(MSG_DEBUG, "scan not triggered by supplicant, ignore");
		return;
	}
	wpa_s->type &= ~(1 << WLAN_FC_STYPE_BEACON) & ~(1 << WLAN_FC_STYPE_PROBE_RESP);
	esp_wifi_register_mgmt_frame_internal(wpa_s->type, wpa_s->subtype);
	esp_supplicant_post_evt(SIG_SUPPLICANT_SCAN_DONE, 0);

}

static void esp_supp_handle_wnm_scan_done(struct wpa_supplicant *wpa_s)
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

static void esp_supp_scan_done_cleanup(struct wpa_supplicant *wpa_s)
{
	uint16_t number = 1;
	wifi_ap_record_t ap_records;

	wpa_s->scanning = 0;
	wpa_s->scan_reason = 0;
	/* clean scan list from net80211 */
	esp_wifi_scan_get_ap_records(&number, &ap_records);
}

void esp_supplicant_handle_scan_done_evt(void)
{
	struct wpa_supplicant *wpa_s = &g_wpa_supp;

	wpa_printf(MSG_INFO, "scan done received");
	/* Check which module started this, call the respective function */
	if (wpa_s->scan_reason == REASON_RRM_BEACON_REPORT) {
		wpas_beacon_rep_scan_process(wpa_s, wpa_s->scan_start_tsf);
	} else if (wpa_s->scan_reason == REASON_WNM_BSS_TRANS_REQ) {
		esp_supp_handle_wnm_scan_done(wpa_s);
	}
	esp_supp_scan_done_cleanup(wpa_s);
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
	esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_SCAN_DONE,
			&esp_scan_done_event_handler, NULL);
}

void esp_scan_deinit(struct wpa_supplicant *wpa_s)
{
	wpa_bss_deinit(wpa_s);
}

int esp_handle_beacon_probe(u8 type, u8 *frame, size_t len, u8 *sender,
			    u32 rssi, u8 channel, u64 current_tsf)
{
	struct wpa_supplicant *wpa_s = &g_wpa_supp;
	struct os_reltime now;
	struct wpa_scan_res *res;
	u8 *ptr;

	if (len < 12) {
		wpa_printf(MSG_ERROR, "beacon/probe is having short len=%d\n", len);
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
	if (type == WLAN_FC_STYPE_PROBE_RESP)
		res->ie_len = len;
	else if (type == WLAN_FC_STYPE_BEACON)
		res->beacon_ie_len = len;

	ptr += sizeof(struct wpa_scan_res);

	/* update rest of the frame */
	os_memcpy(ptr, frame, len);
	wpa_bss_update_scan_res(wpa_s, res, &now);
	os_free(res);

	return 0;
}

static int esp_issue_scan(struct wpa_supplicant *wpa_s,
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
				return -1;
			}
			os_memcpy(params->ssid, scan_params->ssids[0].ssid, scan_params->ssids[0].ssid_len);
			params->scan_type = WIFI_SCAN_TYPE_ACTIVE;
		} else
			params->scan_type = WIFI_SCAN_TYPE_PASSIVE;

		if (scan_params->bssid) {
			params->bssid = os_zalloc(ETH_ALEN);
			if (!params->bssid) {
				wpa_printf(MSG_ERROR, "failed to allocate memory");
				return -1;
			}
			os_memcpy(params->bssid, scan_params->bssid, ETH_ALEN);
		}
		if (scan_params->channel) {
			params->channel = scan_params->channel;
		}

		if (scan_params->duration) {
			params->scan_time.passive = scan_params->duration;
			params->scan_time.active.min = scan_params->duration;
			params->scan_time.active.max = scan_params->duration;
		}
	}

	wpa_s->scan_start_tsf = scan_start_tsf;
	/* Register frames to come to supplicant when we park on channel */
	wpa_s->type |= (1 << WLAN_FC_STYPE_BEACON) | (1 << WLAN_FC_STYPE_PROBE_RESP);
	esp_wifi_register_mgmt_frame_internal(wpa_s->type, wpa_s->subtype);

	/* issue scan */
	if (esp_wifi_scan_start(params, false) < 0) {
		ret = -1;
		goto cleanup;
	}
	wpa_s->scanning = 1;
	wpa_bss_update_start(wpa_s);
	wpa_printf(MSG_INFO, "scan issued at time=%llu", wpa_s->scan_start_tsf);

cleanup:
	if (params->ssid)
		os_free(params->ssid);
	if (params->bssid)
		os_free(params->bssid);
	os_free(params);

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
	return esp_issue_scan(wpa_s, params);
}

void wpa_scan_results_free(struct wpa_scan_results *res)
{
	size_t i;

	if (res == NULL)
		return;

	for (i = 0; i < res->num; i++)
		os_free(res->res[i]);
	os_free(res->res);
	os_free(res);
}
