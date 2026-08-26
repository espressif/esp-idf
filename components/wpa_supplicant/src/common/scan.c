/*
 * WPA Supplicant - Scanning
 * Copyright (c) 2003-2019, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "common/ieee802_11_defs.h"
#include "wpa_supplicant_i.h"
#include "drivers/driver.h"
#include "common/ieee802_11_common.h"
#include "bss.h"
#include "scan.h"

/**
 * wpa_supplicant_req_scan - Schedule a scan for neighboring access points
 * @wpa_s: Pointer to wpa_supplicant data
 * @sec: Number of seconds after which to scan
 * @usec: Number of microseconds after which to scan
 *
 * This function is used to schedule a scan for neighboring access points after
 * the specified time.
 */
void wpa_supplicant_req_scan(struct wpa_supplicant *wpa_s, int sec, int usec)
{
	int ret;
	struct wpa_driver_scan_params *params;

	os_sleep(sec, usec);

	if (wpa_s->scanning) {
		wpa_dbg(wpa_s, MSG_DEBUG, "Already scanning - Return");
		return;
	}
	if (!wpa_s->current_bss) {
		wpa_dbg(wpa_s, MSG_INFO, "Current BSS is null - Return");
		return;
	}
	params = os_zalloc(sizeof(*params));

	if (!params) {
		wpa_printf(MSG_ERROR, "Memory allocation failed");
		return;
	}
#ifdef CONFIG_WNM
	if (wpa_s->wnm_mode) {
		/* Use the same memory */
		params->ssids[0].ssid = wpa_s->current_bss->ssid;
		params->ssids[0].ssid_len = wpa_s->current_bss->ssid_len;
		params->num_ssids = 1;
	}
#endif
	if (!is_zero_ether_addr(wpa_s->next_scan_bssid)) {
		/* Use the same memory */
		params->bssid = wpa_s->next_scan_bssid;
	}

	if (wpa_s->next_scan_chan)
		params->channel = wpa_s->next_scan_chan;

	wpa_s->scan_reason = REASON_WNM_BSS_TRANS_REQ;
	ret = wpa_supplicant_trigger_scan(wpa_s, params);

	os_free(params);

	if (ret < 0) {
		wpa_printf(MSG_ERROR, "Failed to issue scan - Return");
		return;
	}
}

/**
 * wpa_scan_get_ie - Fetch a specified information element from a scan result
 * @res: Scan result entry
 * @ie: Information element identitifier (WLAN_EID_*)
 * Returns: Pointer to the information element (id field) or %NULL if not found
 *
 * This function returns the first matching information element in the scan
 * result.
 */
const u8 * wpa_scan_get_ie(const struct wpa_scan_res *res, u8 ie)
{
	size_t ie_len = res->ie_len;

	/* Use the Beacon frame IEs if res->ie_len is not available */
	if (!ie_len)
		ie_len = res->beacon_ie_len;

	return get_ie((const u8 *) (res + 1), ie_len, ie);
}

void wpa_scan_free_params(struct wpa_driver_scan_params *params)
{
	if (params == NULL)
		return;

	os_free((u8 *) params->bssid);

	os_free(params);
}
