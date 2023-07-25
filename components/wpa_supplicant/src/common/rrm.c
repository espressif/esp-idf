/*
 * wpa_supplicant - Radio Measurements
 * Copyright (c) 2003-2016, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "utils/bitfield.h"
#include "utils/eloop.h"
#include "common/ieee802_11_defs.h"
#include "common/ieee802_11_common.h"
#include "wpa_supplicant_i.h"
#include "bss.h"
#include "rrm.h"
#include "scan.h"
#include <sys/errno.h>
#ifdef ESP_SUPPLICANT
#include "esp_common_i.h"
#endif

static void wpas_rrm_neighbor_rep_timeout_handler(void *data, void *user_ctx)
{
	struct rrm_data *rrm = data;

	if (!rrm->notify_neighbor_rep) {
		wpa_printf(MSG_ERROR,
			   "RRM: Unexpected neighbor report timeout");
		return;
	}

	wpa_printf(MSG_DEBUG, "RRM: Notifying neighbor report - NONE");
	rrm->notify_neighbor_rep(rrm->neighbor_rep_cb_ctx, NULL, 0);

	rrm->notify_neighbor_rep = NULL;
	rrm->neighbor_rep_cb_ctx = NULL;
}


/*
 * wpas_rrm_reset - Clear and reset all RRM data in wpa_supplicant
 * @wpa_s: Pointer to wpa_supplicant
 */
void wpas_rrm_reset(struct wpa_supplicant *wpa_s)
{
	wpa_s->rrm.rrm_used = 0;

	eloop_cancel_timeout(wpas_rrm_neighbor_rep_timeout_handler, &wpa_s->rrm,
			     NULL);
	if (wpa_s->rrm.notify_neighbor_rep)
		wpas_rrm_neighbor_rep_timeout_handler(&wpa_s->rrm, NULL);
	wpa_s->rrm.next_neighbor_rep_token = 1;
	wpas_clear_beacon_rep_data(wpa_s);
}


/*
 * wpas_rrm_process_neighbor_rep - Handle incoming neighbor report
 * @wpa_s: Pointer to wpa_supplicant
 * @report: Neighbor report buffer, prefixed by a 1-byte dialog token
 * @report_len: Length of neighbor report buffer
 */
void wpas_rrm_process_neighbor_rep(struct wpa_supplicant *wpa_s,
				   const u8 *report, size_t report_len)
{

	wpa_hexdump(MSG_DEBUG, "RRM: New Neighbor Report", report, report_len);
	if (report_len < 1)
		return;

	if (report[0] != wpa_s->rrm.next_neighbor_rep_token - 1) {
		wpa_printf(MSG_DEBUG,
			   "RRM: Discarding neighbor report with token %d (expected %d)",
			   report[0], wpa_s->rrm.next_neighbor_rep_token - 1);
		return;
	}

	eloop_cancel_timeout(wpas_rrm_neighbor_rep_timeout_handler, &wpa_s->rrm,
			     NULL);

	if (!wpa_s->rrm.notify_neighbor_rep) {
		wpa_msg(wpa_s, MSG_INFO, "RRM: Unexpected neighbor report");
		return;
	}

	wpa_printf(MSG_DEBUG, "RRM: Notifying neighbor report (token = %d)",
		report[0]);
	wpa_s->rrm.notify_neighbor_rep(wpa_s->rrm.neighbor_rep_cb_ctx,
				       report, report_len);
	wpa_s->rrm.notify_neighbor_rep = NULL;
	wpa_s->rrm.neighbor_rep_cb_ctx = NULL;
}


#if defined(__CYGWIN__) || defined(CONFIG_NATIVE_WINDOWS)
/* Workaround different, undefined for Windows, error codes used here */
#ifndef ENOTCONN
#define ENOTCONN -1
#endif
#ifndef EOPNOTSUPP
#define EOPNOTSUPP -1
#endif
#ifndef ECANCELED
#define ECANCELED -1
#endif
#endif

/* Measurement Request element + Location Subject + Maximum Age subelement */
#define MEASURE_REQUEST_LCI_LEN (3 + 1 + 4)
/* Measurement Request element + Location Civic Request */
#define MEASURE_REQUEST_CIVIC_LEN (3 + 5)


/**
 * wpas_rrm_send_neighbor_rep_request - Request a neighbor report from our AP
 * @wpa_s: Pointer to wpa_supplicant
 * @ssid: if not null, this is sent in the request. Otherwise, no SSID IE
 *	  is sent in the request.
 * @lci: if set, neighbor request will include LCI request
 * @civic: if set, neighbor request will include civic location request
 * @cb: Callback function to be called once the requested report arrives, or
 *	timed out after RRM_NEIGHBOR_REPORT_TIMEOUT seconds.
 *	In the former case, 'neighbor_rep' is a newly allocated wpabuf, and it's
 *	the requester's responsibility to free it.
 *	In the latter case NULL will be sent in 'neighbor_rep'.
 * @cb_ctx: Context value to send the callback function
 * Returns: 0 in case of success, negative error code otherwise
 *
 * In case there is a previous request which has not been answered yet, the
 * new request fails. The caller may retry after RRM_NEIGHBOR_REPORT_TIMEOUT.
 * Request must contain a callback function.
 */
int wpas_rrm_send_neighbor_rep_request(struct wpa_supplicant *wpa_s,
				       const struct wpa_ssid_value *ssid,
				       int lci, int civic,
				       void (*cb)(void *ctx,
						  const u8 *neighbor_rep, size_t len),
				       void *cb_ctx)
{
	struct wpabuf *buf;

	if (!(wpa_s->rrm_ie[0] & WLAN_RRM_CAPS_NEIGHBOR_REPORT)) {
		wpa_printf(MSG_ERROR,
			"RRM: No network support for Neighbor Report.");
		return -EOPNOTSUPP;
	}

	/* Refuse if there's a live request */
	if (wpa_s->rrm.notify_neighbor_rep) {
		wpa_dbg(wpa_s, MSG_DEBUG,
			"RRM: Currently handling previous Neighbor Report.");
		return -EBUSY;
	}

	/* 3 = action category + action code + dialog token */
	buf = wpabuf_alloc(3 + (ssid ? 2 + ssid->ssid_len : 0) +
			   (lci ? 2 + MEASURE_REQUEST_LCI_LEN : 0) +
			   (civic ? 2 + MEASURE_REQUEST_CIVIC_LEN : 0));
	if (buf == NULL) {
		wpa_dbg(wpa_s, MSG_DEBUG,
			"RRM: Failed to allocate Neighbor Report Request");
		return -ENOMEM;
	}

	wpa_dbg(wpa_s, MSG_DEBUG,
		"RRM: Neighbor report request (for %s), token=%d",
		(ssid ? wpa_ssid_txt(ssid->ssid, ssid->ssid_len) : ""),
		wpa_s->rrm.next_neighbor_rep_token);

	wpabuf_put_u8(buf, WLAN_ACTION_RADIO_MEASUREMENT);
	wpabuf_put_u8(buf, WLAN_RRM_NEIGHBOR_REPORT_REQUEST);
	wpabuf_put_u8(buf, wpa_s->rrm.next_neighbor_rep_token);
	if (ssid) {
		wpabuf_put_u8(buf, WLAN_EID_SSID);
		wpabuf_put_u8(buf, ssid->ssid_len);
		wpabuf_put_data(buf, ssid->ssid, ssid->ssid_len);
	}

	if (lci) {
		/* IEEE P802.11-REVmc/D5.0 9.4.2.21 */
		wpabuf_put_u8(buf, WLAN_EID_MEASURE_REQUEST);
		wpabuf_put_u8(buf, MEASURE_REQUEST_LCI_LEN);

		/*
		 * Measurement token; nonzero number that is unique among the
		 * Measurement Request elements in a particular frame.
		 */
		wpabuf_put_u8(buf, 1); /* Measurement Token */

		/*
		 * Parallel, Enable, Request, and Report bits are 0, Duration is
		 * reserved.
		 */
		wpabuf_put_u8(buf, 0); /* Measurement Request Mode */
		wpabuf_put_u8(buf, MEASURE_TYPE_LCI); /* Measurement Type */

		/* IEEE P802.11-REVmc/D5.0 9.4.2.21.10 - LCI request */
		/* Location Subject */
		wpabuf_put_u8(buf, LOCATION_SUBJECT_REMOTE);

		/* Optional Subelements */
		/*
		 * IEEE P802.11-REVmc/D5.0 Figure 9-170
		 * The Maximum Age subelement is required, otherwise the AP can
		 * send only data that was determined after receiving the
		 * request. Setting it here to unlimited age.
		 */
		wpabuf_put_u8(buf, LCI_REQ_SUBELEM_MAX_AGE);
		wpabuf_put_u8(buf, 2);
		wpabuf_put_le16(buf, 0xffff);
	}

	if (civic) {
		/* IEEE P802.11-REVmc/D5.0 9.4.2.21 */
		wpabuf_put_u8(buf, WLAN_EID_MEASURE_REQUEST);
		wpabuf_put_u8(buf, MEASURE_REQUEST_CIVIC_LEN);

		/*
		 * Measurement token; nonzero number that is unique among the
		 * Measurement Request elements in a particular frame.
		 */
		wpabuf_put_u8(buf, 2); /* Measurement Token */

		/*
		 * Parallel, Enable, Request, and Report bits are 0, Duration is
		 * reserved.
		 */
		wpabuf_put_u8(buf, 0); /* Measurement Request Mode */
		/* Measurement Type */
		wpabuf_put_u8(buf, MEASURE_TYPE_LOCATION_CIVIC);

		/* IEEE P802.11-REVmc/D5.0 9.4.2.21.14:
		 * Location Civic request */
		/* Location Subject */
		wpabuf_put_u8(buf, LOCATION_SUBJECT_REMOTE);
		wpabuf_put_u8(buf, 0); /* Civic Location Type: IETF RFC 4776 */
		/* Location Service Interval Units: Seconds */
		wpabuf_put_u8(buf, 0);
		/* Location Service Interval: 0 - Only one report is requested
		 */
		wpabuf_put_le16(buf, 0);
		/* No optional subelements */
	}

	wpa_s->rrm.next_neighbor_rep_token++;

	if (wpa_drv_send_action(wpa_s, 0, 0,
				wpabuf_head(buf), wpabuf_len(buf), 0) < 0) {
		wpa_dbg(wpa_s, MSG_DEBUG,
			"RRM: Failed to send Neighbor Report Request");
		wpabuf_free(buf);
		return -ECANCELED;
	}

	wpa_s->rrm.neighbor_rep_cb_ctx = cb_ctx;
	wpa_s->rrm.notify_neighbor_rep = cb;
	eloop_register_timeout(RRM_NEIGHBOR_REPORT_TIMEOUT, 0,
			       wpas_rrm_neighbor_rep_timeout_handler,
			       &wpa_s->rrm, NULL);

	wpabuf_free(buf);
	return 0;
}


static int wpas_rrm_report_elem(struct wpabuf **buf, u8 token, u8 mode, u8 type,
				const u8 *data, size_t data_len)
{
	if (wpabuf_resize(buf, 5 + data_len))
		return -1;

	wpabuf_put_u8(*buf, WLAN_EID_MEASURE_REPORT);
	wpabuf_put_u8(*buf, 3 + data_len);
	wpabuf_put_u8(*buf, token);
	wpabuf_put_u8(*buf, mode);
	wpabuf_put_u8(*buf, type);

	if (data_len)
		wpabuf_put_data(*buf, data, data_len);

	return 0;
}

static void wpas_rrm_send_msr_report_mpdu(struct wpa_supplicant *wpa_s,
					  const u8 *data, size_t len)
{
	struct wpabuf *report = wpabuf_alloc(len + 3);

	if (!report)
		return;

	wpabuf_put_u8(report, WLAN_ACTION_RADIO_MEASUREMENT);
	wpabuf_put_u8(report, WLAN_RRM_RADIO_MEASUREMENT_REPORT);
	wpabuf_put_u8(report, wpa_s->rrm.token);

	wpabuf_put_data(report, data, len);

	if (wpa_drv_send_action(wpa_s, 0, 0,
				wpabuf_head(report), wpabuf_len(report), 0)) {
		wpa_printf(MSG_ERROR,
			   "RRM: Radio measurement report failed: Sending Action frame failed");
	}

	wpabuf_free(report);
}


static int wpas_rrm_beacon_rep_update_last_frame(u8 *pos, size_t len)
{
	struct rrm_measurement_report_element *msr_rep;
	u8 *end = pos + len;
	u8 *msr_rep_end;
	struct rrm_measurement_beacon_report *rep = NULL;
	u8 *subelem;

	/* Find the last beacon report element */
	while (end - pos >= (int) sizeof(*msr_rep)) {
		msr_rep = (struct rrm_measurement_report_element *) pos;
		msr_rep_end = pos + msr_rep->len + 2;

		if (msr_rep->eid != WLAN_EID_MEASURE_REPORT ||
		    msr_rep_end > end) {
			/* Should not happen. This indicates a bug. */
			wpa_printf(MSG_ERROR,
				   "RRM: non-measurement report element in measurement report frame");
			return -1;
		}

		if (msr_rep->type == MEASURE_TYPE_BEACON)
			rep = (struct rrm_measurement_beacon_report *)
				msr_rep->variable;

		pos += pos[1] + 2;
	}

	if (!rep)
		return 0;

	subelem = rep->variable;
	while (subelem + 2 < msr_rep_end &&
	       subelem[0] != WLAN_BEACON_REPORT_SUBELEM_LAST_INDICATION)
		subelem += 2 + subelem[1];

	if (subelem + 2 < msr_rep_end &&
	    subelem[0] == WLAN_BEACON_REPORT_SUBELEM_LAST_INDICATION &&
	    subelem[1] == 1 &&
	    subelem + BEACON_REPORT_LAST_INDICATION_SUBELEM_LEN <= end)
		subelem[2] = 1;

	return 0;
}


static void wpas_rrm_send_msr_report(struct wpa_supplicant *wpa_s,
				     struct wpabuf *buf)
{
	int len = wpabuf_len(buf);
	u8 *pos = wpabuf_mhead_u8(buf), *next = pos;

#define MPDU_REPORT_LEN (int) (IEEE80211_MAX_MMPDU_SIZE - IEEE80211_HDRLEN - 3)

	while (len) {
		int send_len = (len > MPDU_REPORT_LEN) ? next - pos : len;

		if (send_len == len)
			wpas_rrm_beacon_rep_update_last_frame(pos, len);

		if (send_len == len ||
		    (send_len + next[1] + 2) > MPDU_REPORT_LEN) {
			wpas_rrm_send_msr_report_mpdu(wpa_s, pos, send_len);
			len -= send_len;
			pos = next;
		}

		if (len)
			next += next[1] + 2;
	}
#undef MPDU_REPORT_LEN
}

static int wpas_get_op_chan_phy(int channel, const u8 *ies, size_t ies_len,
				u8 *op_class, u8 *chan, u8 *phy_type)
{
	const u8 *ie;
	int sec_chan = 0;
	struct ieee80211_ht_operation *ht_oper = NULL;

	ie = get_ie(ies, ies_len, WLAN_EID_HT_OPERATION);
	if (ie && ie[1] >= 2) {
		ht_oper = (struct ieee80211_ht_operation *) (ie + 2);

		if (ht_oper->ht_param & HT_INFO_HT_PARAM_SECONDARY_CHNL_ABOVE)
			sec_chan = 1;
		else if (ht_oper->ht_param &
			 HT_INFO_HT_PARAM_SECONDARY_CHNL_BELOW)
			sec_chan = -1;
	}

	*op_class = get_operating_class(channel, sec_chan);

	*phy_type = (sec_chan != 0) ? PHY_TYPE_HT : PHY_TYPE_ERP;

	return 0;
}


static int wpas_beacon_rep_add_frame_body(struct bitfield *eids,
					  enum beacon_report_detail detail,
					  struct wpa_bss *bss, u8 *buf,
					  size_t buf_len, u8 **ies_buf,
					  size_t *ie_len, int add_fixed)
{
	u8 *ies = *ies_buf;
	size_t ies_len = *ie_len;
	u8 *pos = buf;
	int rem_len;

	rem_len = 255 - sizeof(struct rrm_measurement_beacon_report) -
		sizeof(struct rrm_measurement_report_element) - 2 -
		REPORTED_FRAME_BODY_SUBELEM_LEN;

	if (detail > BEACON_REPORT_DETAIL_ALL_FIELDS_AND_ELEMENTS) {
		wpa_printf(MSG_DEBUG,
			   "Beacon Request: Invalid reporting detail: %d",
			   detail);
		return -1;
	}

	if (detail == BEACON_REPORT_DETAIL_NONE)
		return 0;

	/*
	 * Minimal frame body subelement size: EID(1) + length(1) + TSF(8) +
	 * beacon interval(2) + capabilities(2) = 14 bytes
	 */
	if (add_fixed && buf_len < 14)
		return -1;

	*pos++ = WLAN_BEACON_REPORT_SUBELEM_FRAME_BODY;
	/* The length will be filled later */
	pos++;

	if (add_fixed) {
		WPA_PUT_LE64(pos, bss->tsf);
		pos += sizeof(bss->tsf);
		WPA_PUT_LE16(pos, bss->beacon_int);
		pos += 2;
		WPA_PUT_LE16(pos, bss->caps);
		pos += 2;
	}

	rem_len -= pos - buf;

	/*
	 * According to IEEE Std 802.11-2016, 9.4.2.22.7, if the reported frame
	 * body subelement causes the element to exceed the maximum element
	 * size, the subelement is truncated so that the last IE is a complete
	 * IE. So even when required to report all IEs, add elements one after
	 * the other and stop once there is no more room in the measurement
	 * element.
	 */
	while (ies_len > 2 && 2U + ies[1] <= ies_len && rem_len > 0) {
		if (detail == BEACON_REPORT_DETAIL_ALL_FIELDS_AND_ELEMENTS ||
		    (eids && bitfield_is_set(eids, ies[0]))) {
			u8 elen = ies[1];

			if (2 + elen > buf + buf_len - pos ||
			    2 + elen > rem_len)
				break;

			*pos++ = ies[0];
			*pos++ = elen;
			os_memcpy(pos, ies + 2, elen);
			pos += elen;
			rem_len -= 2 + elen;
		}

		ies_len -= 2 + ies[1];
		ies += 2 + ies[1];
	}

	*ie_len = ies_len;
	*ies_buf = ies;

	/* Now the length is known */
	buf[1] = pos - buf - 2;
	return pos - buf;
}


static int wpas_add_beacon_rep_elem(struct beacon_rep_data *data,
				    struct wpa_bss *bss,
				    struct wpabuf **wpa_buf,
				    struct rrm_measurement_beacon_report *rep,
				    u8 **ie, size_t *ie_len, u8 idx)
{
	int ret;
	u8 *buf, *pos;
	u32 subelems_len = REPORTED_FRAME_BODY_SUBELEM_LEN +
		(data->last_indication ?
		 BEACON_REPORT_LAST_INDICATION_SUBELEM_LEN : 0);

	/* Maximum element length: Beacon Report element + Reported Frame Body
	 * subelement + all IEs of the reported Beacon frame + Reported Frame
	 * Body Fragment ID subelement */
	buf = os_malloc(sizeof(*rep) + 14 + *ie_len + subelems_len);
	if (!buf)
		return -1;

	os_memcpy(buf, rep, sizeof(*rep));

	ret = wpas_beacon_rep_add_frame_body(data->eids, data->report_detail,
					     bss, buf + sizeof(*rep),
					     14 + *ie_len, ie, ie_len,
					     idx == 0);
	if (ret < 0)
		goto out;

	pos = buf + ret + sizeof(*rep);
	pos[0] = WLAN_BEACON_REPORT_SUBELEM_FRAME_BODY_FRAGMENT_ID;
	pos[1] = 2;

	/*
	 * Only one Beacon Report Measurement is supported at a time, so
	 * the Beacon Report ID can always be set to 1.
	 */
	pos[2] = 1;

	/* Fragment ID Number (bits 0..6) and More Frame Body Fragments (bit 7)
 */
	pos[3] = idx;
	if (data->report_detail != BEACON_REPORT_DETAIL_NONE && *ie_len)
		pos[3] |= REPORTED_FRAME_BODY_MORE_FRAGMENTS;
	else
		pos[3] &= ~REPORTED_FRAME_BODY_MORE_FRAGMENTS;

	pos += REPORTED_FRAME_BODY_SUBELEM_LEN;

	if (data->last_indication) {
		pos[0] = WLAN_BEACON_REPORT_SUBELEM_LAST_INDICATION;
		pos[1] = 1;

		/* This field will be updated later if this is the last frame */
		pos[2] = 0;
	}

	ret = wpas_rrm_report_elem(wpa_buf, data->token,
				   MEASUREMENT_REPORT_MODE_ACCEPT,
				   MEASURE_TYPE_BEACON, buf,
				   ret + sizeof(*rep) + subelems_len);
out:
	os_free(buf);
	return ret;
}


static int wpas_add_beacon_rep(struct wpa_supplicant *wpa_s,
			       struct wpabuf **wpa_buf, struct wpa_bss *bss,
			       u64 start, u64 parent_tsf)
{
	struct beacon_rep_data *data = &wpa_s->beacon_rep_data;
	u8 *ies = (u8 *) (bss + 1);
	u8 *pos = ies;
	size_t ies_len = bss->ie_len ? bss->ie_len : bss->beacon_ie_len;
	struct rrm_measurement_beacon_report rep;
	u8 idx = 0;

	if (os_memcmp(data->bssid, broadcast_ether_addr, ETH_ALEN) != 0 &&
	    os_memcmp(data->bssid, bss->bssid, ETH_ALEN) != 0)
		return 0;

	if (data->ssid_len &&
	    (data->ssid_len != bss->ssid_len ||
	     os_memcmp(data->ssid, bss->ssid, bss->ssid_len) != 0))
		return 0;

	if (wpas_get_op_chan_phy(bss->channel, ies, ies_len, &rep.op_class,
				 &rep.channel, &rep.report_info) < 0)
		return 0;

	rep.channel = bss->channel;
	rep.start_time = host_to_le64(start);
	rep.duration = host_to_le16(data->scan_params.duration);
	rep.rcpi = rssi_to_rcpi(bss->level);
	rep.rsni = 255; /* 255 indicates that RSNI is not available */
	os_memcpy(rep.bssid, bss->bssid, ETH_ALEN);
	rep.antenna_id = 0; /* unknown */
	rep.parent_tsf = host_to_le32(parent_tsf);

	do {
		int ret;

		ret = wpas_add_beacon_rep_elem(data, bss, wpa_buf, &rep,
					       &pos, &ies_len, idx++);
		if (ret)
			return ret;
	} while (data->report_detail != BEACON_REPORT_DETAIL_NONE &&
		 ies_len >= 2);

	return 0;
}


static int wpas_beacon_rep_no_results(struct wpa_supplicant *wpa_s,
				      struct wpabuf **buf)
{
	return wpas_rrm_report_elem(buf, wpa_s->beacon_rep_data.token,
				    MEASUREMENT_REPORT_MODE_ACCEPT,
				    MEASURE_TYPE_BEACON, NULL, 0);
}


static void wpas_beacon_rep_table(struct wpa_supplicant *wpa_s,
				  struct wpabuf **buf)
{
	size_t i;

	for (i = 0; i < wpa_s->last_scan_res_used; i++) {
		if (wpas_add_beacon_rep(wpa_s, buf, wpa_s->last_scan_res[i],
					0, 0) < 0)
			break;
	}

	if (!(*buf))
		wpas_beacon_rep_no_results(wpa_s, buf);

	wpa_hexdump_buf(MSG_DEBUG, "RRM: Radio Measurement report", *buf);
}


static void wpas_rrm_refuse_request(struct wpa_supplicant *wpa_s)
{
	if (!is_multicast_ether_addr(wpa_s->rrm.dst_addr)) {
		struct wpabuf *buf = NULL;

		if (wpas_rrm_report_elem(&buf, wpa_s->beacon_rep_data.token,
					 MEASUREMENT_REPORT_MODE_REJECT_REFUSED,
					 MEASURE_TYPE_BEACON, NULL, 0)) {
			wpa_printf(MSG_ERROR, "RRM: Memory allocation failed");
			wpabuf_free(buf);
			return;
		}

		wpas_rrm_send_msr_report(wpa_s, buf);
		wpabuf_free(buf);
	}

	wpas_clear_beacon_rep_data(wpa_s);
}


static void wpas_rrm_scan_timeout(void *eloop_ctx, void *timeout_ctx)
{
	struct wpa_supplicant *wpa_s = eloop_ctx;
	struct wpa_driver_scan_params *params =
		&wpa_s->beacon_rep_data.scan_params;
	u16 prev_duration = params->duration;

	if (!wpa_s->current_bss)
		return;

	os_get_reltime(&wpa_s->beacon_rep_scan);
	wpa_s->scan_reason = REASON_RRM_BEACON_REPORT;
	if (wpa_supplicant_trigger_scan(wpa_s, params) < 0)
		wpas_rrm_refuse_request(wpa_s);
	params->duration = prev_duration;
}


static int wpas_rm_handle_beacon_req_subelem(struct wpa_supplicant *wpa_s,
					     struct beacon_rep_data *data,
					     u8 sid, u8 slen, const u8 *subelem)
{
	u8 report_info, i;

	switch (sid) {
	case WLAN_BEACON_REQUEST_SUBELEM_SSID:
		if (!slen) {
			wpa_printf(MSG_DEBUG,
				   "SSID subelement with zero length - wildcard SSID");
			break;
		}

		if (slen > SSID_MAX_LEN) {
			wpa_printf(MSG_DEBUG,
				   "Invalid SSID subelement length: %u", slen);
			return -1;
		}

		data->ssid_len = slen;
		os_memcpy(data->ssid, subelem, data->ssid_len);
		break;
	case WLAN_BEACON_REQUEST_SUBELEM_INFO:
		if (slen != 2) {
			wpa_printf(MSG_DEBUG,
				   "Invalid reporting information subelement length: %u",
				   slen);
			return -1;
		}

		report_info = subelem[0];
		if (report_info != 0) {
			wpa_printf(MSG_DEBUG,
				   "reporting information=%u is not supported",
				   report_info);
			return 0;
		}
		break;
	case WLAN_BEACON_REQUEST_SUBELEM_DETAIL:
		if (slen != 1) {
			wpa_printf(MSG_DEBUG,
				   "Invalid reporting detail subelement length: %u",
				   slen);
			return -1;
		}

		data->report_detail = subelem[0];
		if (data->report_detail >
		    BEACON_REPORT_DETAIL_ALL_FIELDS_AND_ELEMENTS) {
			wpa_printf(MSG_DEBUG, "Invalid reporting detail: %u",
				   subelem[0]);
			return -1;
		}

		break;
	case WLAN_BEACON_REQUEST_SUBELEM_REQUEST:
		if (data->report_detail !=
		    BEACON_REPORT_DETAIL_REQUESTED_ONLY) {
			wpa_printf(MSG_DEBUG,
				   "Beacon request: request subelement is present but report detail is %u",
				   data->report_detail);
			return -1;
		}

		if (!slen) {
			wpa_printf(MSG_DEBUG,
				   "Invalid request subelement length: %u",
				   slen);
			return -1;
		}

		if (data->eids) {
			wpa_printf(MSG_DEBUG,
				   "Beacon Request: Request subelement appears more than once");
			return -1;
		}

		data->eids = bitfield_alloc(255);
		if (!data->eids) {
			wpa_printf(MSG_DEBUG, "Failed to allocate EIDs bitmap");
			return -1;
		}

		for (i = 0; i < slen; i++)
			bitfield_set(data->eids, subelem[i]);
		break;
	case WLAN_BEACON_REQUEST_SUBELEM_AP_CHANNEL:
		/* Skip - it will be processed when freqs are added */
		break;
	case WLAN_BEACON_REQUEST_SUBELEM_LAST_INDICATION:
		if (slen != 1) {
			wpa_printf(MSG_DEBUG,
				   "Beacon request: Invalid last indication request subelement length: %u",
				   slen);
			return -1;
		}

		data->last_indication = subelem[0];
		break;
	default:
		wpa_printf(MSG_DEBUG,
			   "Beacon request: Unknown subelement id %u", sid);
		break;
	}

	return 1;
}


/**
 * Returns 0 if the next element can be processed, 1 if some operation was
 * triggered, and -1 if processing failed (i.e., the element is in invalid
 * format or an internal error occurred).
 */
static int
wpas_rm_handle_beacon_req(struct wpa_supplicant *wpa_s,
			  u8 elem_token, int duration_mandatory,
			  const struct rrm_measurement_beacon_request *req,
			  size_t len, struct wpabuf **buf)
{
	struct beacon_rep_data *data = &wpa_s->beacon_rep_data;
	struct wpa_driver_scan_params *params = &data->scan_params;
	const u8 *subelems;
	size_t elems_len;
	u16 rand_interval;
	u32 interval_usec;
	u32 _rand;
	int ret = 0, res;
	u8 reject_mode;

	if (len < sizeof(*req))
		return -1;

	if (req->mode != BEACON_REPORT_MODE_PASSIVE &&
	    req->mode != BEACON_REPORT_MODE_ACTIVE &&
	    req->mode != BEACON_REPORT_MODE_TABLE)
		return 0;

	subelems = req->variable;
	elems_len = len - sizeof(*req);
	rand_interval = le_to_host16(req->rand_interval);

	os_memset(data, 0, sizeof(*data));

	data->token = elem_token;

	/* default reporting detail is all fixed length fields and all
	 * elements */
	data->report_detail = BEACON_REPORT_DETAIL_ALL_FIELDS_AND_ELEMENTS;
	os_memcpy(data->bssid, req->bssid, ETH_ALEN);

	while (elems_len >= 2) {
		if (subelems[1] > elems_len - 2) {
			wpa_printf(MSG_DEBUG,
				   "Beacon Request: Truncated subelement");
			ret = -1;
			goto out;
		}

		res = wpas_rm_handle_beacon_req_subelem(
			wpa_s, data, subelems[0], subelems[1], &subelems[2]);
		if (res < 0) {
			ret = res;
			goto out;
		} else if (!res) {
			reject_mode = MEASUREMENT_REPORT_MODE_REJECT_INCAPABLE;
			goto out_reject;
		}

		elems_len -= 2 + subelems[1];
		subelems += 2 + subelems[1];
	}

	if (req->mode == BEACON_REPORT_MODE_TABLE) {
		wpas_beacon_rep_table(wpa_s, buf);
		goto out;
	}
	params->channel = req->channel;
#ifdef ESP_SUPPLICANT
	if (params->channel == 0xff) {
		/* set it to zero */
		params->channel = 0;
	}
#endif
	params->duration = le_to_host16(req->duration);
	params->duration_mandatory = duration_mandatory;
	params->mode = req->mode;
	if (!params->duration) {
		wpa_printf(MSG_DEBUG, "Beacon request: Duration is 0");
		ret = -1;
		goto out;
	}

	if (data->ssid_len) {
		params->ssids[params->num_ssids].ssid = data->ssid;
		params->ssids[params->num_ssids++].ssid_len = data->ssid_len;
	}

	if (os_get_random((u8 *) &_rand, sizeof(_rand)) < 0)
		_rand = os_random();
	interval_usec = (_rand % (rand_interval + 1)) * 1024;
	os_sleep(0, interval_usec);
	wpas_rrm_scan_timeout(wpa_s, NULL);
	return 1;
out_reject:
	if (!is_multicast_ether_addr(wpa_s->rrm.dst_addr) &&
	    wpas_rrm_report_elem(buf, elem_token, reject_mode,
				 MEASURE_TYPE_BEACON, NULL, 0) < 0) {
		wpa_printf(MSG_DEBUG, "RRM: Failed to add report element");
		ret = -1;
	}
out:
	wpas_clear_beacon_rep_data(wpa_s);
	return ret;
}


static int
wpas_rrm_handle_msr_req_element(
	struct wpa_supplicant *wpa_s,
	const struct rrm_measurement_request_element *req,
	struct wpabuf **buf)
{
	int duration_mandatory;

	wpa_printf(MSG_DEBUG, "Measurement request type %d token %d",
		   req->type, req->token);

	if (req->mode & MEASUREMENT_REQUEST_MODE_ENABLE) {
		/* Enable bit is not supported for now */
		wpa_printf(MSG_DEBUG, "RRM: Enable bit not supported, ignore");
		return 0;
	}

	if ((req->mode & MEASUREMENT_REQUEST_MODE_PARALLEL) &&
	    req->type > MEASURE_TYPE_RPI_HIST) {
		/* Parallel measurements are not supported for now */
		wpa_printf(MSG_DEBUG,
			   "RRM: Parallel measurements are not supported, reject");
		goto reject;
	}

	duration_mandatory =
		!!(req->mode & MEASUREMENT_REQUEST_MODE_DURATION_MANDATORY);

	switch (req->type) {
	case MEASURE_TYPE_BEACON:
		return wpas_rm_handle_beacon_req(wpa_s, req->token,
						 duration_mandatory,
						 (const void *) req->variable,
						 req->len - 3, buf);
	default:
		wpa_printf(MSG_INFO,
			   "RRM: Unsupported radio measurement type %u",
			   req->type);
		break;
	}

reject:
	if (!is_multicast_ether_addr(wpa_s->rrm.dst_addr) &&
	    wpas_rrm_report_elem(buf, req->token,
				 MEASUREMENT_REPORT_MODE_REJECT_INCAPABLE,
				 req->type, NULL, 0) < 0) {
		wpa_printf(MSG_DEBUG, "RRM: Failed to add report element");
		return -1;
	}

	return 0;
}


static struct wpabuf *
wpas_rrm_process_msr_req_elems(struct wpa_supplicant *wpa_s, const u8 *pos,
			       size_t len)
{
	struct wpabuf *buf = NULL;

	while (len) {
		const struct rrm_measurement_request_element *req;
		int res;

		if (len < 2) {
			wpa_printf(MSG_DEBUG, "RRM: Truncated element");
			goto out;
		}

		req = (const struct rrm_measurement_request_element *) pos;
		if (req->eid != WLAN_EID_MEASURE_REQUEST) {
			wpa_printf(MSG_DEBUG,
				   "RRM: Expected Measurement Request element, but EID is %u",
				   req->eid);
			printf("len is %d", len);
			goto out;
		}

		if (req->len < 3) {
			wpa_printf(MSG_DEBUG, "RRM: Element length too short");
			goto out;
		}

		if (req->len > len - 2) {
			wpa_printf(MSG_DEBUG, "RRM: Element length too long");
			goto out;
		}

		res = wpas_rrm_handle_msr_req_element(wpa_s, req, &buf);
		if (res < 0)
			goto out;

		pos += req->len + 2;
		len -= req->len + 2;
	}

	return buf;

out:
	wpabuf_free(buf);
	return NULL;
}


void wpas_rrm_handle_radio_measurement_request(struct wpa_supplicant *wpa_s,
					       const u8 *src, const u8 *dst,
					       const u8 *frame, size_t len)
{
	struct wpabuf *report;

	if (!wpa_s->rrm.rrm_used) {
		wpa_printf(MSG_INFO,
			   "RRM: Ignoring radio measurement request: Not RRM network");
		return;
	}

	if (len < 3) {
		wpa_printf(MSG_INFO,
			   "RRM: Ignoring too short radio measurement request");
		return;
	}

	wpa_s->rrm.token = *frame;
	os_memcpy(wpa_s->rrm.dst_addr, dst, ETH_ALEN);

	/* Number of repetitions is not supported */

	report = wpas_rrm_process_msr_req_elems(wpa_s, frame + 3, len - 3);
	if (!report)
		return;

	wpas_rrm_send_msr_report(wpa_s, report);
	wpabuf_free(report);
}


void wpas_rrm_handle_link_measurement_request(struct wpa_supplicant *wpa_s,
					      const u8 *src,
					      const u8 *frame, size_t len,
					      int rssi)
{
	struct wpabuf *buf;
	const struct rrm_link_measurement_request *req;
	struct rrm_link_measurement_report report;

	req = (const struct rrm_link_measurement_request *) frame;
	if (len < sizeof(*req)) {
		wpa_printf(MSG_INFO,
			   "RRM: Link measurement report failed. Request too short");
		return;
	}

	os_memset(&report, 0, sizeof(report));
	report.dialog_token = req->dialog_token;
	report.tpc.eid = WLAN_EID_TPC_REPORT;
	report.tpc.len = 2;
	/* Note: The driver is expected to update report.tpc.tx_power and
	 * report.tpc.link_margin subfields when sending out this frame.
	 * Similarly, the driver would need to update report.rx_ant_id and
	 * report.tx_ant_id subfields. */

#ifdef ESP_SUPPLICANT
	esp_get_tx_power(&report.tpc.tx_power);
	/* Minimum RSSI = -96 dbm */
	report.tpc.link_margin = rssi + 96;
#endif
	report.rsni = 255; /* 255 indicates that RSNI is not available */
	report.rcpi = rssi_to_rcpi(rssi);

	/* action_category + action_code */
	buf = wpabuf_alloc(2 + sizeof(report));
	if (buf == NULL) {
		wpa_printf(MSG_ERROR,
			   "RRM: Link measurement report failed. Buffer allocation failed");
		return;
	}

	wpabuf_put_u8(buf, WLAN_ACTION_RADIO_MEASUREMENT);
	wpabuf_put_u8(buf, WLAN_RRM_LINK_MEASUREMENT_REPORT);
	wpabuf_put_data(buf, &report, sizeof(report));
	wpa_hexdump_buf(MSG_DEBUG, "RRM: Link measurement report", buf);

	if (wpa_drv_send_action(wpa_s, 0, 0,
				wpabuf_head(buf), wpabuf_len(buf), 0)) {
		wpa_printf(MSG_ERROR,
			   "RRM: Link measurement report failed. Send action failed");
	}
	wpabuf_free(buf);
}


int wpas_beacon_rep_scan_process(struct wpa_supplicant *wpa_s,
				 u64 scan_start_tsf)
{
	size_t i = 0;
	struct wpabuf *buf = NULL;
	struct wpa_bss *bss = NULL;

	if (!wpa_s->beacon_rep_data.token)
		return 0;

	if (!wpa_s->current_bss)
		goto out;

	wpa_printf(MSG_DEBUG, "RRM: TSF  current BSS: " MACSTR,
		   MAC2STR(wpa_s->current_bss->bssid));

	for (i = 0; i < wpa_s->num_bss; i++) {
		bss = wpa_bss_get_next_bss(wpa_s, bss);

		if (!bss)
			continue;

		if (wpa_s->beacon_rep_data.scan_params.channel &&
				bss->channel != wpa_s->beacon_rep_data.scan_params.channel)
			continue;
		/* We flush scan results before issuing scan again */
#ifndef ESP_SUPPLICANT
		/*
		 * Don't report results that were not received during the
		 * current measurement.
		 */
		if (scan_start_tsf <
			   scan_res->res[i]->parent_tsf) {
			struct os_reltime update_time, diff;

			/* For now, allow 7 ms older results due to some
			 * unknown issue with cfg80211 BSS table updates during
			 * a scan with the current BSS.
			 * TODO: Fix this more properly to avoid having to have
			 * this type of hacks in place. */
			calculate_update_time(&scan_res->fetch_time,
					      scan_res->res[i]->age,
					      &update_time);
			os_reltime_sub(&wpa_s->beacon_rep_scan,
				       &update_time, &diff);
			if (os_reltime_before(&update_time,
					      &wpa_s->beacon_rep_scan) &&
			    (diff.sec || diff.usec >= 8000)) {
				wpa_printf(MSG_ERROR,
					   "RRM: Ignore scan result for " MACSTR
					   " due to old update (age(ms) %u, calculated age %u.%06u seconds)",
					   MAC2STR(scan_res->res[i]->bssid),
					   scan_res->res[i]->age,
					   (unsigned int) diff.sec,
					   (unsigned int) diff.usec);
				continue;
			}
		} else {
			continue;
		}
#endif
		if (wpas_add_beacon_rep(wpa_s, &buf, bss, scan_start_tsf,
					bss->parent_tsf) < 0)
			break;
	}

	if (!buf && wpas_beacon_rep_no_results(wpa_s, &buf))
		goto out;

	wpa_hexdump_buf(MSG_DEBUG, "RRM: Radio Measurement report", buf);

	wpas_rrm_send_msr_report(wpa_s, buf);
	wpabuf_free(buf);

out:
	wpas_clear_beacon_rep_data(wpa_s);
	return 1;
}


void wpas_clear_beacon_rep_data(struct wpa_supplicant *wpa_s)
{
	struct beacon_rep_data *data = &wpa_s->beacon_rep_data;

	eloop_cancel_timeout(wpas_rrm_scan_timeout, wpa_s, NULL);
	bitfield_free(data->eids);
	os_memset(data, 0, sizeof(*data));
}
