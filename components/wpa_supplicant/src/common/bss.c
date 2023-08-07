/*
 * BSS table
 * Copyright (c) 2009-2019, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "common/ieee802_11_defs.h"
#include "common/ieee802_11_common.h"
#include "drivers/driver.h"
#include "eap_peer/eap.h"
#include "wpa_supplicant_i.h"
#include "scan.h"
#include "bss.h"
#ifdef ESP_SUPPLICANT
#include "esp_wifi_driver.h"
#endif

#define MAX_BSS_COUNT 20

void wpa_bss_remove(struct wpa_supplicant *wpa_s, struct wpa_bss *bss,
		    const char *reason)
{
	if (wpa_s->last_scan_res) {
		unsigned int i;
		for (i = 0; i < wpa_s->last_scan_res_used; i++) {
			if (wpa_s->last_scan_res[i] == bss) {
				os_memmove(&wpa_s->last_scan_res[i],
					   &wpa_s->last_scan_res[i + 1],
					   (wpa_s->last_scan_res_used - i - 1)
					   * sizeof(struct wpa_bss *));
				wpa_s->last_scan_res_used--;
				break;
			}
		}
	}
	dl_list_del(&bss->list);
	dl_list_del(&bss->list_id);
	wpa_s->num_bss--;
	wpa_dbg(wpa_s, MSG_DEBUG, "BSS: Remove id %u BSSID " MACSTR
		" SSID '%s' due to %s", bss->id, MAC2STR(bss->bssid),
		wpa_ssid_txt(bss->ssid, bss->ssid_len), reason);
	os_free(bss);
}


/**
 * wpa_bss_get - Fetch a BSS table entry based on BSSID and SSID
 * @wpa_s: Pointer to wpa_supplicant data
 * @bssid: BSSID
 * @ssid: SSID
 * @ssid_len: Length of @ssid
 * Returns: Pointer to the BSS entry or %NULL if not found
 */
struct wpa_bss * wpa_bss_get(struct wpa_supplicant *wpa_s, const u8 *bssid,
			     const u8 *ssid, size_t ssid_len)
{
	struct wpa_bss *bss;
	dl_list_for_each(bss, &wpa_s->bss, struct wpa_bss, list) {
		if (os_memcmp(bss->bssid, bssid, ETH_ALEN) == 0 &&
		    bss->ssid_len == ssid_len &&
		    os_memcmp(bss->ssid, ssid, ssid_len) == 0)
			return bss;
	}
	return NULL;
}


void calculate_update_time(const struct os_reltime *fetch_time,
			   unsigned int age_ms,
			   struct os_reltime *update_time)
{
	os_time_t usec;

	update_time->sec = fetch_time->sec;
	update_time->usec = fetch_time->usec;
	update_time->sec -= age_ms / 1000;
	usec = (age_ms % 1000) * 1000;
	if (update_time->usec < usec) {
		update_time->sec--;
		update_time->usec += 1000000;
	}
	update_time->usec -= usec;
}


static void wpa_bss_copy_res(struct wpa_bss *dst, struct wpa_scan_res *src,
			     struct os_reltime *fetch_time)
{
	dst->flags = src->flags;
	os_memcpy(dst->bssid, src->bssid, ETH_ALEN);
	dst->channel = src->chan;
	dst->beacon_int = src->beacon_int;
	dst->caps = src->caps;
	dst->noise = src->noise;
	dst->level = src->level;
	dst->tsf = src->tsf;
	dst->parent_tsf = src->parent_tsf;

	calculate_update_time(fetch_time, src->age, &dst->last_update);
}

#ifdef ESP_SUPPLICANT
static int wpa_bss_known(struct wpa_supplicant *wpa_s, struct wpa_bss *bss)
{
	struct wifi_ssid *ssid = esp_wifi_sta_get_prof_ssid_internal();

	if (ssid->len == 0)
		return 0;
	if (ssid->len == bss->ssid_len &&
	    os_memcmp(ssid->ssid, bss->ssid, ssid->len) == 0)
		return 1;
	return 0;
}
#endif

static int wpa_bss_in_use(struct wpa_supplicant *wpa_s, struct wpa_bss *bss)
{
	if (bss == wpa_s->current_bss)
		return 1;

#ifndef ESP_SUPPLICANT
	if (wpa_s->current_bss &&
	    (bss->ssid_len != wpa_s->current_bss->ssid_len ||
	     os_memcmp(bss->ssid, wpa_s->current_bss->ssid,
		       bss->ssid_len) != 0))
		return 0; /* SSID has changed */

	return !is_zero_ether_addr(bss->bssid) && wpa_s->current_bss->bssid &&
		(os_memcmp(bss->bssid, wpa_s->current_bss->bssid, ETH_ALEN) == 0);
#else
	return 0;
#endif
}

static int wpa_bss_remove_oldest_unknown(struct wpa_supplicant *wpa_s)
{
	struct wpa_bss *bss;

	dl_list_for_each(bss, &wpa_s->bss, struct wpa_bss, list) {
		if (!wpa_bss_known(wpa_s, bss)) {
			wpa_bss_remove(wpa_s, bss, __func__);
			return 0;
		}
	}

	return -1;
}

static int wpa_bss_remove_oldest(struct wpa_supplicant *wpa_s)
{
	struct wpa_bss *bss;

	/*
	 * Remove the oldest entry that does not match with any configured
	 * network.
	 */
	if (wpa_bss_remove_oldest_unknown(wpa_s) == 0)
		return 0;

	/*
	 * Remove the oldest entry that isn't currently in use.
	 */
	dl_list_for_each(bss, &wpa_s->bss, struct wpa_bss, list) {
		if (!wpa_bss_in_use(wpa_s, bss)) {
			wpa_bss_remove(wpa_s, bss, __func__);
			return 0;
		}
	}

	return -1;
}

static struct wpa_bss * wpa_bss_add(struct wpa_supplicant *wpa_s,
				    const u8 *ssid, size_t ssid_len,
				    struct wpa_scan_res *res,
				    struct os_reltime *fetch_time)
{
	struct wpa_bss *bss;

	if ((wpa_s->num_bss + 1 > MAX_BSS_COUNT) &&
	    (wpa_bss_remove_oldest(wpa_s) < 0)) {
		wpa_printf(MSG_ERROR,
			   "Failed to clean older entries, rejecting scan result");
		return NULL;
	}

	bss = os_zalloc(sizeof(*bss) + res->ie_len + res->beacon_ie_len);
	if (bss == NULL)
		return NULL;
	bss->id = wpa_s->bss_next_id++;
	bss->last_update_idx = wpa_s->bss_update_idx;
	wpa_bss_copy_res(bss, res, fetch_time);
	os_memcpy(bss->ssid, ssid, ssid_len);
	bss->ssid_len = ssid_len;
	bss->ie_len = res->ie_len;
	bss->beacon_ie_len = res->beacon_ie_len;
	os_memcpy(bss->ies, res + 1, res->ie_len + res->beacon_ie_len);

	dl_list_add_tail(&wpa_s->bss, &bss->list);
	dl_list_add_tail(&wpa_s->bss_id, &bss->list_id);
	wpa_s->num_bss++;
	wpa_dbg(wpa_s, MSG_INFO, "BSS: Add new id %u BSSID " MACSTR
		" SSID '%s' chan %d",
		bss->id, MAC2STR(bss->bssid), wpa_ssid_txt(ssid, ssid_len),
		bss->channel);
	return bss;
}

static struct wpa_bss *
wpa_bss_update(struct wpa_supplicant *wpa_s, struct wpa_bss *bss,
	       struct wpa_scan_res *res, struct os_reltime *fetch_time)
{
	if (bss->last_update_idx == wpa_s->bss_update_idx) {
		struct os_reltime update_time;

		/*
		 * Some drivers (e.g., cfg80211) include multiple BSS entries
		 * for the same BSS if that BSS's channel changes. The BSS list
		 * implementation in wpa_supplicant does not do that and we need
		 * to filter out the obsolete results here to make sure only the
		 * most current BSS information remains in the table.
		 */
		wpa_printf(MSG_MSGDUMP, "BSS: " MACSTR
			   " has multiple entries in the scan results - select the most current one",
			   MAC2STR(bss->bssid));
		calculate_update_time(fetch_time, res->age, &update_time);
		wpa_printf(MSG_MSGDUMP,
			   "Accept this BSS entry since it looks more current than the previous update");
	}

	bss->last_update_idx = wpa_s->bss_update_idx;
	wpa_bss_copy_res(bss, res, fetch_time);
	/* Move the entry to the end of the list */
	dl_list_del(&bss->list);
	if (bss->ie_len + bss->beacon_ie_len >=
	    res->ie_len + res->beacon_ie_len) {
		os_memcpy(bss->ies, res + 1, res->ie_len + res->beacon_ie_len);
		bss->ie_len = res->ie_len;
		bss->beacon_ie_len = res->beacon_ie_len;
	} else {
		struct wpa_bss *nbss;
		struct dl_list *prev = bss->list_id.prev;
		dl_list_del(&bss->list_id);
		nbss = os_realloc(bss, sizeof(*bss) + res->ie_len +
				  res->beacon_ie_len);
		if (nbss) {
			unsigned int i;
			for (i = 0; i < wpa_s->last_scan_res_used; i++) {
				if (wpa_s->last_scan_res[i] == bss) {
					wpa_s->last_scan_res[i] = nbss;
					break;
				}
			}
			if (wpa_s->current_bss == bss)
				wpa_s->current_bss = nbss;
			bss = nbss;
			os_memcpy(bss->ies, res + 1,
				  res->ie_len + res->beacon_ie_len);
			bss->ie_len = res->ie_len;
			bss->beacon_ie_len = res->beacon_ie_len;
		}
		dl_list_add(prev, &bss->list_id);
	}
	dl_list_add_tail(&wpa_s->bss, &bss->list);

	return bss;
}


/**
 * wpa_bss_update_start - Start a BSS table update from scan results
 * @wpa_s: Pointer to wpa_supplicant data
 *
 * This function is called at the start of each BSS table update round for new
 * scan results. The actual scan result entries are indicated with calls to
 * wpa_bss_update_scan_res() and the update round is finished with a call to
 * wpa_bss_update_end().
 */
void wpa_bss_update_start(struct wpa_supplicant *wpa_s)
{
	wpa_s->bss_update_idx++;
	wpa_dbg(wpa_s, MSG_DEBUG, "BSS: Start scan result update %u",
		wpa_s->bss_update_idx);
	wpa_s->last_scan_res_used = 0;
}


/**
 * wpa_bss_update_scan_res - Update a BSS table entry based on a scan result
 * @wpa_s: Pointer to wpa_supplicant data
 * @res: Scan result
 * @fetch_time: Time when the result was fetched from the driver
 *
 * This function updates a BSS table entry (or adds one) based on a scan result.
 * This is called separately for each scan result between the calls to
 * wpa_bss_update_start() and wpa_bss_update_end().
 */
void wpa_bss_update_scan_res(struct wpa_supplicant *wpa_s,
			     struct wpa_scan_res *res,
			     struct os_reltime *fetch_time)
{
	const u8 *ssid;
	struct wpa_bss *bss;

	ssid = wpa_scan_get_ie(res, WLAN_EID_SSID);
	if (ssid == NULL) {
		wpa_dbg(wpa_s, MSG_DEBUG, "BSS: No SSID IE included for "
			MACSTR, MAC2STR(res->bssid));
		return;
	}
	if (ssid[1] > SSID_MAX_LEN) {
		wpa_dbg(wpa_s, MSG_DEBUG, "BSS: Too long SSID IE included for "
			MACSTR, MAC2STR(res->bssid));
		return;
	}

	/* TODO: add option for ignoring BSSes we are not interested in
	 * (to save memory) */

	bss = wpa_bss_get(wpa_s, res->bssid, ssid + 2, ssid[1]);
	if (bss == NULL)
		bss = wpa_bss_add(wpa_s, ssid + 2, ssid[1], res, fetch_time);
	else {
		bss = wpa_bss_update(wpa_s, bss, res, fetch_time);
		if (wpa_s->last_scan_res) {
			unsigned int i;
			for (i = 0; i < wpa_s->last_scan_res_used; i++) {
				if (bss == wpa_s->last_scan_res[i]) {
					/* Already in the list */
					return;
				}
			}
		}
	}

	if (bss == NULL)
		return;
	if (wpa_s->last_scan_res_used >= wpa_s->last_scan_res_size) {
		struct wpa_bss **n;
		unsigned int siz;
		if (wpa_s->last_scan_res_size == 0)
			siz = 32;
		else
			siz = wpa_s->last_scan_res_size * 2;
		n = os_realloc_array(wpa_s->last_scan_res, siz,
				     sizeof(struct wpa_bss *));
		if (n == NULL)
			return;
		wpa_s->last_scan_res = n;
		wpa_s->last_scan_res_size = siz;
	}

	if (wpa_s->last_scan_res)
		wpa_s->last_scan_res[wpa_s->last_scan_res_used++] = bss;
}


/**
 * wpa_bss_update_end - End a BSS table update from scan results
 * @wpa_s: Pointer to wpa_supplicant data
 * @info: Information about scan parameters
 * @new_scan: Whether this update round was based on a new scan
 *
 * This function is called at the end of each BSS table update round for new
 * scan results. The start of the update was indicated with a call to
 * wpa_bss_update_start().
 */
void wpa_bss_update_end(struct wpa_supplicant *wpa_s)
{
	os_get_reltime(&wpa_s->last_scan);
}

/**
 * wpa_bss_init - Initialize BSS table
 * @wpa_s: Pointer to wpa_supplicant data
 * Returns: 0 on success, -1 on failure
 *
 * This prepares BSS table lists and timer for periodic updates. The BSS table
 * is deinitialized with wpa_bss_deinit() once not needed anymore.
 */
int wpa_bss_init(struct wpa_supplicant *wpa_s)
{
	dl_list_init(&wpa_s->bss);
	dl_list_init(&wpa_s->bss_id);
	return 0;
}


/**
 * wpa_bss_flush - Flush all unused BSS entries
 * @wpa_s: Pointer to wpa_supplicant data
 */
void wpa_bss_flush(struct wpa_supplicant *wpa_s)
{
	struct wpa_bss *bss, *n;

	if (wpa_s->bss.next == NULL)
		return; /* BSS table not yet initialized */

	dl_list_for_each_safe(bss, n, &wpa_s->bss, struct wpa_bss, list) {
		if (wpa_bss_in_use(wpa_s, bss))
			continue;
		wpa_bss_remove(wpa_s, bss, __func__);
	}
}


/**
 * wpa_bss_deinit - Deinitialize BSS table
 * @wpa_s: Pointer to wpa_supplicant data
 */
void wpa_bss_deinit(struct wpa_supplicant *wpa_s)
{
	wpa_bss_flush(wpa_s);
}


/**
 * wpa_bss_get_bssid - Fetch a BSS table entry based on BSSID
 * @wpa_s: Pointer to wpa_supplicant data
 * @bssid: BSSID
 * Returns: Pointer to the BSS entry or %NULL if not found
 */
struct wpa_bss * wpa_bss_get_bssid(struct wpa_supplicant *wpa_s,
				   const u8 *bssid)
{
	struct wpa_bss *bss;
	dl_list_for_each_reverse(bss, &wpa_s->bss, struct wpa_bss, list) {
		if (os_memcmp(bss->bssid, bssid, ETH_ALEN) == 0)
			return bss;
	}
	return NULL;
}


/**
 * wpa_bss_get_next_bss - Fetch a next BSS table entry from the list
 * @wpa_s: Pointer to wpa_supplicant data
 * @bss: BSS
 * Returns: Pointer to the BSS entry or %NULL if not found
 */
struct wpa_bss * wpa_bss_get_next_bss(struct wpa_supplicant *wpa_s,
				   struct wpa_bss *prev_bss)
{
	struct wpa_bss *bss;

	if (!prev_bss)
		return dl_list_first(&wpa_s->bss, struct wpa_bss, list);
	dl_list_for_each(bss, &wpa_s->bss, struct wpa_bss, list) {
		if (os_memcmp(bss->bssid, prev_bss->bssid, ETH_ALEN) == 0)
			return dl_list_entry(bss->list.next, struct wpa_bss, list);
	}
	return NULL;
}

/**
 * wpa_bss_get_ie - Fetch a specified information element from a BSS entry
 * @bss: BSS table entry
 * @ie: Information element identitifier (WLAN_EID_*)
 * Returns: Pointer to the information element (id field) or %NULL if not found
 *
 * This function returns the first matching information element in the BSS
 * entry.
 */
const u8 * wpa_bss_get_ie(const struct wpa_bss *bss, u8 ie)
{
	return get_ie((const u8 *) (bss + 1), bss->ie_len, ie);
}

/**
 * wpa_bss_get_vendor_ie - Fetch a vendor information element from a BSS entry
 * @bss: BSS table entry
 * @vendor_type: Vendor type (four octets starting the IE payload)
 * Returns: Pointer to the information element (id field) or %NULL if not found
 *
 * This function returns the first matching information element in the BSS
 * entry.
 */
const u8 * wpa_bss_get_vendor_ie(const struct wpa_bss *bss, u32 vendor_type)
{
	const u8 *ies;
	const struct element *elem;

	ies = wpa_bss_ie_ptr(bss);

	for_each_element_id(elem, WLAN_EID_VENDOR_SPECIFIC, ies, bss->ie_len) {
		if (elem->datalen >= 4 &&
				vendor_type == WPA_GET_BE32(elem->data))
			return &elem->id;
	}

	return NULL;
}

int wpa_bss_ext_capab(const struct wpa_bss *bss, unsigned int capab)
{
	return ieee802_11_ext_capab(wpa_bss_get_ie(bss, WLAN_EID_EXT_CAPAB),
				    capab);
}
