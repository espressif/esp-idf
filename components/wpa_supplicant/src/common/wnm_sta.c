/*
 * wpa_supplicant - WNM
 * Copyright (c) 2011-2013, Qualcomm Atheros, Inc.
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "common/ieee802_11_defs.h"
#include "common/ieee802_11_common.h"
#include "rsn_supp/wpa.h"
#include "wpa_supplicant_i.h"
#include "drivers/driver.h"
#include "scan.h"
#include "bss.h"
#include "wnm_sta.h"

#define MAX_TFS_IE_LEN  1024
#define WNM_MAX_NEIGHBOR_REPORT 10

#define WNM_SCAN_RESULT_AGE 2 /* 2 seconds */

void wnm_deallocate_memory(struct wpa_supplicant *wpa_s)
{
	int i;

	for (i = 0; i < wpa_s->wnm_num_neighbor_report; i++) {
		os_free(wpa_s->wnm_neighbor_report_elements[i].meas_pilot);
		os_free(wpa_s->wnm_neighbor_report_elements[i].mul_bssid);
	}

	wpa_s->wnm_num_neighbor_report = 0;
	os_free(wpa_s->wnm_neighbor_report_elements);
	wpa_s->wnm_neighbor_report_elements = NULL;
}


static void wnm_parse_neighbor_report_elem(struct neighbor_report *rep,
					   u8 id, u8 elen, const u8 *pos)
{
	switch (id) {
	case WNM_NEIGHBOR_TSF:
		if (elen < 2 + 2) {
			wpa_printf(MSG_DEBUG, "WNM: Too short TSF");
			break;
		}
		rep->tsf_offset = WPA_GET_LE16(pos);
		rep->beacon_int = WPA_GET_LE16(pos + 2);
		rep->tsf_present = 1;
		break;
	case WNM_NEIGHBOR_CONDENSED_COUNTRY_STRING:
		if (elen < 2) {
			wpa_printf(MSG_DEBUG, "WNM: Too short condensed "
				   "country string");
			break;
		}
		os_memcpy(rep->country, pos, 2);
		rep->country_present = 1;
		break;
	case WNM_NEIGHBOR_BSS_TRANSITION_CANDIDATE:
		if (elen < 1) {
			wpa_printf(MSG_DEBUG, "WNM: Too short BSS transition "
				   "candidate");
			break;
		}
		rep->preference = pos[0];
		rep->preference_present = 1;
		break;
	case WNM_NEIGHBOR_BSS_TERMINATION_DURATION:
		if (elen < 10) {
			wpa_printf(MSG_DEBUG,
				   "WNM: Too short BSS termination duration");
			break;
		}
		rep->bss_term_tsf = WPA_GET_LE64(pos);
		rep->bss_term_dur = WPA_GET_LE16(pos + 8);
		rep->bss_term_present = 1;
		break;
	case WNM_NEIGHBOR_BEARING:
		if (elen < 8) {
			wpa_printf(MSG_DEBUG, "WNM: Too short neighbor "
				   "bearing");
			break;
		}
		rep->bearing = WPA_GET_LE16(pos);
		rep->distance = WPA_GET_LE32(pos + 2);
		rep->rel_height = WPA_GET_LE16(pos + 2 + 4);
		rep->bearing_present = 1;
		break;
	case WNM_NEIGHBOR_MEASUREMENT_PILOT:
		if (elen < 1) {
			wpa_printf(MSG_DEBUG, "WNM: Too short measurement "
				   "pilot");
			break;
		}
		os_free(rep->meas_pilot);
		rep->meas_pilot = os_zalloc(sizeof(struct measurement_pilot));
		if (rep->meas_pilot == NULL)
			break;
		rep->meas_pilot->measurement_pilot = pos[0];
		rep->meas_pilot->subelem_len = elen - 1;
		os_memcpy(rep->meas_pilot->subelems, pos + 1, elen - 1);
		break;
	case WNM_NEIGHBOR_RRM_ENABLED_CAPABILITIES:
		if (elen < 5) {
			wpa_printf(MSG_DEBUG, "WNM: Too short RRM enabled "
				   "capabilities");
			break;
		}
		os_memcpy(rep->rm_capab, pos, 5);
		rep->rm_capab_present = 1;
		break;
	case WNM_NEIGHBOR_MULTIPLE_BSSID:
		if (elen < 1) {
			wpa_printf(MSG_DEBUG, "WNM: Too short multiple BSSID");
			break;
		}
		os_free(rep->mul_bssid);
		rep->mul_bssid = os_zalloc(sizeof(struct multiple_bssid));
		if (rep->mul_bssid == NULL)
			break;
		rep->mul_bssid->max_bssid_indicator = pos[0];
		rep->mul_bssid->subelem_len = elen - 1;
		os_memcpy(rep->mul_bssid->subelems, pos + 1, elen - 1);
		break;
	}
}

static void wnm_parse_neighbor_report(struct wpa_supplicant *wpa_s,
				      const u8 *pos, u8 len,
				      struct neighbor_report *rep)
{
	u8 left = len;

	if (left < 13) {
		wpa_printf(MSG_DEBUG, "WNM: Too short neighbor report");
		return;
	}

	os_memcpy(rep->bssid, pos, ETH_ALEN);
	rep->bssid_info = WPA_GET_LE32(pos + ETH_ALEN);
	rep->regulatory_class = *(pos + 10);
	rep->channel_number = *(pos + 11);
	rep->phy_type = *(pos + 12);

	pos += 13;
	left -= 13;

	while (left >= 2) {
		u8 id, elen;

		id = *pos++;
		elen = *pos++;
		wpa_printf(MSG_DEBUG, "WNM: Subelement id=%u len=%u", id, elen);
		left -= 2;
		if (elen > left) {
			wpa_printf(MSG_DEBUG,
				   "WNM: Truncated neighbor report subelement");
			break;
		}
		wnm_parse_neighbor_report_elem(rep, id, elen, pos);
		left -= elen;
		pos += elen;
	}
}


static void wnm_clear_acceptable(struct wpa_supplicant *wpa_s)
{
	unsigned int i;

	for (i = 0; i < wpa_s->wnm_num_neighbor_report; i++)
		wpa_s->wnm_neighbor_report_elements[i].acceptable = 0;
}


static struct wpa_bss * get_first_acceptable(struct wpa_supplicant *wpa_s)
{
	unsigned int i;
	struct neighbor_report *nei;

	for (i = 0; i < wpa_s->wnm_num_neighbor_report; i++) {
		nei = &wpa_s->wnm_neighbor_report_elements[i];
		if (nei->acceptable)
			return wpa_bss_get_bssid(wpa_s, nei->bssid);
	}

	return NULL;
}


#ifdef CONFIG_MBO
static struct wpa_bss *
get_mbo_transition_candidate(struct wpa_supplicant *wpa_s,
			     enum mbo_transition_reject_reason *reason)
{
	struct wpa_bss *target = NULL;
	struct wpa_bss_trans_info params;
	struct wpa_bss_candidate_info *info = NULL;
	struct neighbor_report *nei = wpa_s->wnm_neighbor_report_elements;
	u8 *first_candidate_bssid = NULL, *pos;
	unsigned int i;

	params.mbo_transition_reason = wpa_s->wnm_mbo_transition_reason;
	params.n_candidates = 0;
	params.bssid = os_calloc(wpa_s->wnm_num_neighbor_report, ETH_ALEN);
	if (!params.bssid)
		return NULL;

	pos = params.bssid;
	for (i = 0; i < wpa_s->wnm_num_neighbor_report; nei++, i++) {
		if (nei->is_first)
			first_candidate_bssid = nei->bssid;
		if (!nei->acceptable)
			continue;
		os_memcpy(pos, nei->bssid, ETH_ALEN);
		pos += ETH_ALEN;
		params.n_candidates++;
	}

	if (!params.n_candidates)
		goto end;

#ifndef ESP_SUPPLICANT
	info = wpa_drv_get_bss_trans_status(wpa_s, &params);
#endif
	if (!info) {
		/* If failed to get candidate BSS transition status from driver,
		 * get the first acceptable candidate from wpa_supplicant.
		 */
		target = wpa_bss_get_bssid(wpa_s, params.bssid);
		goto end;
	}

	/* Get the first acceptable candidate from driver */
	for (i = 0; i < info->num; i++) {
		if (info->candidates[i].is_accept) {
			target = wpa_bss_get_bssid(wpa_s,
						   info->candidates[i].bssid);
			goto end;
		}
	}

	/* If Disassociation Imminent is set and driver rejects all the
	 * candidate select first acceptable candidate which has
	 * rssi > disassoc_imminent_rssi_threshold
	 */
	if (wpa_s->wnm_mode & WNM_BSS_TM_REQ_DISASSOC_IMMINENT) {
		for (i = 0; i < info->num; i++) {
			target = wpa_bss_get_bssid(wpa_s,
						   info->candidates[i].bssid);
#ifndef ESP_SUPPLICANT
			if (target &&
			    (target->level <
			     wpa_s->conf->disassoc_imminent_rssi_threshold))
				continue;
#else
			if (target)
				continue;
#endif
			goto end;
		}
	}

	/* While sending BTM reject use reason code of the first candidate
	 * received in BTM request frame
	 */
	if (reason) {
		for (i = 0; i < info->num; i++) {
			if (first_candidate_bssid &&
			    os_memcmp(first_candidate_bssid,
				      info->candidates[i].bssid, ETH_ALEN) == 0)
			{
				*reason = info->candidates[i].reject_reason;
				break;
			}
		}
	}

	target = NULL;

end:
	os_free(params.bssid);
	if (info) {
		os_free(info->candidates);
		os_free(info);
	}
	return target;
}
#endif /* CONFIG_MBO */



/* basic function to match candidate profile with current bss */
bool wpa_scan_res_match(struct wpa_supplicant *wpa_s,
			struct wpa_bss *current_bss,
			struct wpa_bss *target_bss)
{
	if (current_bss->ssid_len != target_bss->ssid_len) {
		wpa_printf(MSG_DEBUG, "WNM: ssid didn't match");
		return false;
	}
	if (os_memcmp(current_bss->ssid, target_bss->ssid, current_bss->ssid_len) != 0) {
		wpa_printf(MSG_DEBUG, "WNM: ssid didn't match");
		return false;
	}

	/* Just check for Open/secure mode */
	if ((current_bss->caps & WLAN_CAPABILITY_PRIVACY) != (target_bss->caps & WLAN_CAPABILITY_PRIVACY)) {
		wpa_printf(MSG_DEBUG, "WNM: Security didn't match");
		return false;
	}

	return true;
}

static struct wpa_bss *
compare_scan_neighbor_results(struct wpa_supplicant *wpa_s, os_time_t age_secs,
			      enum mbo_transition_reject_reason *reason)
{
	u8 i;
	struct wpa_bss *bss = wpa_s->current_bss;
	struct wpa_bss *target;

	if (!bss)
		return NULL;

	wnm_clear_acceptable(wpa_s);

	for (i = 0; i < wpa_s->wnm_num_neighbor_report; i++) {
		struct neighbor_report *nei;

		nei = &wpa_s->wnm_neighbor_report_elements[i];
		if (nei->preference_present && nei->preference == 0) {
			wpa_printf(MSG_DEBUG, "Skip excluded BSS " MACSTR,
				   MAC2STR(nei->bssid));
			continue;
		}

		target = wpa_bss_get_bssid(wpa_s, nei->bssid);
		if (!target) {
			wpa_printf(MSG_DEBUG, "Candidate BSS " MACSTR
				   " (pref %d) not found in scan results",
				   MAC2STR(nei->bssid),
				   nei->preference_present ? nei->preference :
				   -1);
			continue;
		}

		if (age_secs) {
			struct os_reltime now;

			if (os_get_reltime(&now) == 0 &&
			    os_reltime_expired(&now, &target->last_update,
					       age_secs)) {
				wpa_printf(MSG_DEBUG,
					   "Candidate BSS is more than %jd seconds old",
					   (intmax_t)age_secs);
				continue;
			}
		}

		if (bss->ssid_len != target->ssid_len ||
		    os_memcmp(bss->ssid, target->ssid, bss->ssid_len) != 0) {
			/*
			 * TODO: Could consider allowing transition to another
			 * ESS if PMF was enabled for the association.
			 */
			wpa_printf(MSG_DEBUG, "Candidate BSS " MACSTR
				   " (pref %d) in different ESS",
				   MAC2STR(nei->bssid),
				   nei->preference_present ? nei->preference :
				   -1);
			continue;
		}

		if (wpa_s->current_bss &&
		    !wpa_scan_res_match(wpa_s, wpa_s->current_bss, target)) {
			wpa_printf(MSG_DEBUG, "Candidate BSS " MACSTR
				   " (pref %d) does not match the current network profile",
				   MAC2STR(nei->bssid),
				   nei->preference_present ? nei->preference :
				   -1);
			continue;
		}

		if (target->level < bss->level && target->level < -80) {
			wpa_printf(MSG_DEBUG, "Candidate BSS " MACSTR
				   " (pref %d) does not have sufficient signal level (%d)",
				   MAC2STR(nei->bssid),
				   nei->preference_present ? nei->preference :
				   -1,
				   target->level);
			continue;
		}

		nei->acceptable = 1;
	}

#ifdef CONFIG_MBO
	if (wpa_s->wnm_mbo_trans_reason_present)
		target = get_mbo_transition_candidate(wpa_s, reason);
	else
		target = get_first_acceptable(wpa_s);
#else /* CONFIG_MBO */
	target = get_first_acceptable(wpa_s);
#endif /* CONFIG_MBO */

	if (target) {
		wpa_printf(MSG_DEBUG,
			   "WNM: Found an acceptable preferred transition candidate BSS "
			   MACSTR " (RSSI %d)",
			   MAC2STR(target->bssid), target->level);
	}

	return target;
}


static int wpa_bss_ies_eq(struct wpa_bss *a, struct wpa_bss *b, u8 eid)
{
	const u8 *ie_a, *ie_b;

	if (!a || !b)
		return 0;

	ie_a = wpa_bss_get_ie(a, eid);
	ie_b = wpa_bss_get_ie(b, eid);

	if (!ie_a || !ie_b || ie_a[1] != ie_b[1])
		return 0;

	return os_memcmp(ie_a, ie_b, ie_a[1]) == 0;
}


static u32 wnm_get_bss_info(struct wpa_supplicant *wpa_s, struct wpa_bss *bss)
{
	u32 info = 0;

	info |= NEI_REP_BSSID_INFO_AP_UNKNOWN_REACH;

	/*
	 * Leave the security and key scope bits unset to indicate that the
	 * security information is not available.
	 */

	if (bss->caps & WLAN_CAPABILITY_SPECTRUM_MGMT)
		info |= NEI_REP_BSSID_INFO_SPECTRUM_MGMT;
	if (bss->caps & WLAN_CAPABILITY_QOS)
		info |= NEI_REP_BSSID_INFO_QOS;
	if (bss->caps & WLAN_CAPABILITY_APSD)
		info |= NEI_REP_BSSID_INFO_APSD;
	if (bss->caps & WLAN_CAPABILITY_RADIO_MEASUREMENT)
		info |= NEI_REP_BSSID_INFO_RM;
	if (bss->caps & WLAN_CAPABILITY_DELAYED_BLOCK_ACK)
		info |= NEI_REP_BSSID_INFO_DELAYED_BA;
	if (bss->caps & WLAN_CAPABILITY_IMM_BLOCK_ACK)
		info |= NEI_REP_BSSID_INFO_IMM_BA;
	if (wpa_bss_ies_eq(bss, wpa_s->current_bss, WLAN_EID_MOBILITY_DOMAIN))
		info |= NEI_REP_BSSID_INFO_MOBILITY_DOMAIN;
	if (wpa_bss_ies_eq(bss, wpa_s->current_bss, WLAN_EID_HT_CAP))
		info |= NEI_REP_BSSID_INFO_HT;

	return info;
}


static int wnm_add_nei_rep(struct wpabuf **buf, const u8 *bssid,
			   u32 bss_info, u8 op_class, u8 chan, u8 phy_type,
			   u8 pref)
{
	if (wpabuf_len(*buf) + 18 >
	    IEEE80211_MAX_MMPDU_SIZE - IEEE80211_HDRLEN) {
		wpa_printf(MSG_DEBUG,
			   "WNM: No room in frame for Neighbor Report element");
		return -1;
	}

	if (wpabuf_resize(buf, 18) < 0) {
		wpa_printf(MSG_DEBUG,
			   "WNM: Failed to allocate memory for Neighbor Report element");
		return -1;
	}

	wpabuf_put_u8(*buf, WLAN_EID_NEIGHBOR_REPORT);
	/* length: 13 for basic neighbor report + 3 for preference subelement */
	wpabuf_put_u8(*buf, 16);
	wpabuf_put_data(*buf, bssid, ETH_ALEN);
	wpabuf_put_le32(*buf, bss_info);
	wpabuf_put_u8(*buf, op_class);
	wpabuf_put_u8(*buf, chan);
	wpabuf_put_u8(*buf, phy_type);
	wpabuf_put_u8(*buf, WNM_NEIGHBOR_BSS_TRANSITION_CANDIDATE);
	wpabuf_put_u8(*buf, 1);
	wpabuf_put_u8(*buf, pref);
	return 0;
}

static int wnm_nei_rep_add_bss(struct wpa_supplicant *wpa_s,
			       struct wpa_bss *bss, struct wpabuf **buf,
			       u8 pref)
{
	const u8 *ie;
	u8 op_class;
	int sec_chan = 0;
	enum phy_type phy_type;
	u32 info;
	struct ieee80211_ht_operation *ht_oper = NULL;

	ie = wpa_bss_get_ie(bss, WLAN_EID_HT_OPERATION);
	if (ie && ie[1] >= 2) {
		ht_oper = (struct ieee80211_ht_operation *) (ie + 2);

		if (ht_oper->ht_param & HT_INFO_HT_PARAM_SECONDARY_CHNL_ABOVE)
			sec_chan = 1;
		else if (ht_oper->ht_param &
			 HT_INFO_HT_PARAM_SECONDARY_CHNL_BELOW)
			sec_chan = -1;
	}

	op_class = get_operating_class(bss->channel, sec_chan);

	phy_type = (sec_chan != 0) ? PHY_TYPE_HT : PHY_TYPE_ERP;

	info = wnm_get_bss_info(wpa_s, bss);

	return wnm_add_nei_rep(buf, bss->bssid, info, op_class, bss->channel, phy_type,
			       pref);
}


static void wnm_add_cand_list(struct wpa_supplicant *wpa_s, struct wpabuf **buf)
{
	unsigned int i, pref = 255;
	struct os_reltime now;

	if (!wpa_s->current_bss)
		return;

	/*
	 * TODO: Define when scan results are no longer valid for the candidate
	 * list.
	 */
	os_get_reltime(&now);
	if (os_reltime_expired(&now, &wpa_s->last_scan, 10))
		return;

	wpa_printf(MSG_DEBUG,
		   "WNM: Add candidate list to BSS Transition Management Response frame");
	for (i = 0; i < wpa_s->last_scan_res_used && pref; i++) {
		struct wpa_bss *bss = wpa_s->last_scan_res[i];
		int res;

		if (wpa_scan_res_match(wpa_s, wpa_s->current_bss, bss)) {
			res = wnm_nei_rep_add_bss(wpa_s, bss, buf, pref--);
			if (res == -2)
				continue; /* could not build entry for BSS */
			if (res < 0)
				break; /* no more room for candidates */
			if (pref == 1)
				break;
		}
	}

	wpa_hexdump_buf(MSG_DEBUG,
			"WNM: BSS Transition Management Response candidate list",
			*buf);
}


#define BTM_RESP_MIN_SIZE	5 + ETH_ALEN

static void wnm_send_bss_transition_mgmt_resp(
	struct wpa_supplicant *wpa_s, u8 dialog_token,
	enum bss_trans_mgmt_status_code status,
	enum mbo_transition_reject_reason reason,
	u8 delay, const u8 *target_bssid)
{
	struct wpabuf *buf;
	int res;

	if (!wpa_s->current_bss) {
		wpa_printf(MSG_DEBUG,
			   "WNM: Current BSS not known - drop response");
		return;
	}
	wpa_printf(MSG_DEBUG,
		   "WNM: Send BSS Transition Management Response to " MACSTR
		   " dialog_token=%u status=%u reason=%u delay=%d",
		   MAC2STR(wpa_s->current_bss->bssid), dialog_token, status, reason, delay);

	buf = wpabuf_alloc(BTM_RESP_MIN_SIZE);
	if (!buf) {
		wpa_printf(MSG_DEBUG,
			   "WNM: Failed to allocate memory for BTM response");
		return;
	}

	wpabuf_put_u8(buf, WLAN_ACTION_WNM);
	wpabuf_put_u8(buf, WNM_BSS_TRANS_MGMT_RESP);
	wpabuf_put_u8(buf, dialog_token);
	wpabuf_put_u8(buf, status);
	wpabuf_put_u8(buf, delay);
	if (target_bssid) {
		wpabuf_put_data(buf, target_bssid, ETH_ALEN);
	} else if (status == WNM_BSS_TM_ACCEPT) {
		/*
		 * P802.11-REVmc clarifies that the Target BSSID field is always
		 * present when status code is zero, so use a fake value here if
		 * no BSSID is yet known.
		 */
		wpabuf_put_data(buf, "\0\0\0\0\0\0", ETH_ALEN);
	}

	if (status == WNM_BSS_TM_ACCEPT)
		wnm_add_cand_list(wpa_s, &buf);

#ifdef CONFIG_MBO
	if (status != WNM_BSS_TM_ACCEPT &&
	    wpa_bss_get_vendor_ie(wpa_s->current_bss, MBO_IE_VENDOR_TYPE)) {
		u8 mbo[10];
		size_t ret;

		ret = wpas_mbo_ie_bss_trans_reject(wpa_s, mbo, sizeof(mbo),
						   reason);
		if (ret) {
			if (wpabuf_resize(&buf, ret) < 0) {
				wpabuf_free(buf);
				wpa_printf(MSG_DEBUG,
					   "WNM: Failed to allocate memory for MBO IE");
				return;
			}

			wpabuf_put_data(buf, mbo, ret);
		}
	}
#endif /* CONFIG_MBO */

	res = wpa_drv_send_action(wpa_s, 0, 0,
				  wpabuf_head_u8(buf), wpabuf_len(buf), 0);
	if (res < 0) {
		wpa_printf(MSG_DEBUG,
			   "WNM: Failed to send BSS Transition Management Response");
	}

	wpabuf_free(buf);
}

void wnm_bss_tm_connect(struct wpa_supplicant *wpa_s,
			struct wpa_bss *bss, char *ssid,
			int after_new_scan)
{
	wpa_printf(MSG_DEBUG,
		"WNM: Transition to BSS " MACSTR
		" based on BSS Transition Management Request after_new_scan=%d)",
		MAC2STR(bss->bssid), after_new_scan);

	/* Send the BSS Management Response - Accept */
	if (wpa_s->wnm_reply) {
		wpa_s->wnm_reply = 0;
		wpa_printf(MSG_DEBUG,
			   "WNM: Sending successful BSS Transition Management Response");
		wnm_send_bss_transition_mgmt_resp(
			wpa_s, wpa_s->wnm_dialog_token, WNM_BSS_TM_ACCEPT,
			MBO_TRANSITION_REJECT_REASON_UNSPECIFIED, 0,
			bss->bssid);
	}

	if (bss == wpa_s->current_bss) {
		wpa_printf(MSG_DEBUG,
			   "WNM: Already associated with the preferred candidate");
		wnm_deallocate_memory(wpa_s);
		return;
	}

	wpa_printf(MSG_DEBUG, "WNM: Issuing connect");

	wpa_supplicant_connect(wpa_s, bss, ssid);
	wnm_deallocate_memory(wpa_s);
}


int wnm_scan_process(struct wpa_supplicant *wpa_s, int reply_on_fail)
{
	struct wpa_bss *bss;
	enum bss_trans_mgmt_status_code status = WNM_BSS_TM_REJECT_UNSPECIFIED;
	enum mbo_transition_reject_reason reason =
		MBO_TRANSITION_REJECT_REASON_UNSPECIFIED;

	if (!wpa_s->wnm_neighbor_report_elements) {
		wpa_printf(MSG_INFO, "WNM: Neighbor report not available");
		return 0;
        }

	wpa_dbg(wpa_s, MSG_DEBUG,
		"WNM: Process scan results for BSS Transition Management");
	if (os_reltime_before(&wpa_s->wnm_cand_valid_until,
			      &wpa_s->scan_trigger_time)) {
		wpa_printf(MSG_DEBUG, "WNM: Previously stored BSS transition candidate list is not valid anymore - drop it");
		wnm_deallocate_memory(wpa_s);
		return 0;
	}

	/* Compare the Neighbor Report and scan results */
	bss = compare_scan_neighbor_results(wpa_s, 0, &reason);
	if (!bss) {
		wpa_printf(MSG_INFO, "WNM: No BSS transition candidate match found");
		status = WNM_BSS_TM_REJECT_NO_SUITABLE_CANDIDATES;
		goto send_bss_resp_fail;
	}

	/* Associate to the network */
	wnm_bss_tm_connect(wpa_s, bss, NULL, 1);
	return 1;

send_bss_resp_fail:
	if (!reply_on_fail)
		return 0;

	/* Send reject response for all the failures */

	if (wpa_s->wnm_reply) {
		wpa_s->wnm_reply = 0;
		wnm_send_bss_transition_mgmt_resp(wpa_s,
						  wpa_s->wnm_dialog_token,
						  status, reason, 0, NULL);
	}
	wnm_deallocate_memory(wpa_s);

	return 0;
}


static int cand_pref_compar(const void *a, const void *b)
{
	const struct neighbor_report *aa = a;
	const struct neighbor_report *bb = b;

	if (!aa->preference_present && !bb->preference_present)
		return 0;
	if (!aa->preference_present)
		return 1;
	if (!bb->preference_present)
		return -1;
	if (bb->preference > aa->preference)
		return 1;
	if (bb->preference < aa->preference)
		return -1;
	return 0;
}


static void wnm_sort_cand_list(struct wpa_supplicant *wpa_s)
{
	if (!wpa_s->wnm_neighbor_report_elements)
		return;
	qsort(wpa_s->wnm_neighbor_report_elements,
	      wpa_s->wnm_num_neighbor_report, sizeof(struct neighbor_report),
	      cand_pref_compar);
}


static void wnm_dump_cand_list(struct wpa_supplicant *wpa_s)
{
#ifdef DEBUG_PRINT
	unsigned int i;

	wpa_printf(MSG_DEBUG, "WNM: BSS Transition Candidate List");
	if (!wpa_s->wnm_neighbor_report_elements)
		return;
	for (i = 0; i < wpa_s->wnm_num_neighbor_report; i++) {
		struct neighbor_report *nei;

		nei = &wpa_s->wnm_neighbor_report_elements[i];
		wpa_printf(MSG_DEBUG, "%u: " MACSTR
			   " info=0x%x op_class=%u chan=%u phy=%u pref=%d",
			   i, MAC2STR(nei->bssid), nei->bssid_info,
			   nei->regulatory_class,
			   nei->channel_number, nei->phy_type,
			   nei->preference_present ? nei->preference : -1);
	}
#endif
}

static void wnm_set_scan_freqs(struct wpa_supplicant *wpa_s)
{
	unsigned int i;
	int num_chan = 0;
	u8 chan = 0;

	wpa_s->next_scan_chan = 0;
	if (!wpa_s->wnm_neighbor_report_elements)
		return;

	for (i = 0; i < wpa_s->wnm_num_neighbor_report; i++) {
		struct neighbor_report *nei;

		nei = &wpa_s->wnm_neighbor_report_elements[i];
		if (nei->channel_number <= 0) {
			wpa_printf(MSG_DEBUG,
				   "WNM: Unknown neighbor operating channel_number for "
				   MACSTR " - scan all channels",
				   MAC2STR(nei->bssid));
			return;
		}
		if (wpa_s->current_bss && (os_memcmp(nei->bssid, wpa_s->current_bss->bssid, ETH_ALEN) == 0)) {
		    continue;
		}
		if (nei->channel_number != chan) {
			chan = nei->channel_number;
			num_chan++;
		}
	}

	if (num_chan == 1) {
		wpa_s->next_scan_chan = chan;
	}
}

static int wnm_fetch_scan_results(struct wpa_supplicant *wpa_s)
{
	/* ESP doesn't support this */
	return 0;
}

static void ieee802_11_rx_bss_trans_mgmt_req(struct wpa_supplicant *wpa_s,
					     const u8 *pos, const u8 *end,
					     int reply)
{
	unsigned int beacon_int;
	u8 valid_int;
#ifdef CONFIG_MBO
	const u8 *vendor;
#endif /* CONFIG_MBO */
#ifdef ESP_SUPPLICANT
	bool scan_required = false;
#endif

	if (wpa_s->disable_mbo_oce || wpa_s->disable_btm)
		return;

	if (end - pos < 5)
		return;

#ifdef CONFIG_MBO
	wpa_s->wnm_mbo_trans_reason_present = 0;
	wpa_s->wnm_mbo_transition_reason = 0;
#endif /* CONFIG_MBO */

	if (wpa_s->current_bss)
		beacon_int = wpa_s->current_bss->beacon_int;
	else
		beacon_int = 100; /* best guess */

	wpa_s->wnm_dialog_token = pos[0];
	wpa_s->wnm_mode = pos[1];
	wpa_s->wnm_dissoc_timer = WPA_GET_LE16(pos + 2);
	valid_int = pos[4];
	wpa_s->wnm_reply = reply;

	wpa_printf(MSG_DEBUG, "WNM: BSS Transition Management Request: "
		   "dialog_token=%u request_mode=0x%x "
		   "disassoc_timer=%u validity_interval=%u",
		   wpa_s->wnm_dialog_token, wpa_s->wnm_mode,
		   wpa_s->wnm_dissoc_timer, valid_int);

#if defined(CONFIG_MBO) && defined(CONFIG_TESTING_OPTIONS)
	if (wpa_s->reject_btm_req_reason) {
		wpa_printf(MSG_INFO,
			   "WNM: Testing - reject BSS Transition Management Request: reject_btm_req_reason=%d",
			   wpa_s->reject_btm_req_reason);
		wnm_send_bss_transition_mgmt_resp(
			wpa_s, wpa_s->wnm_dialog_token,
			wpa_s->reject_btm_req_reason,
			MBO_TRANSITION_REJECT_REASON_UNSPECIFIED, 0, NULL);
		return;
	}
#endif /* CONFIG_MBO && CONFIG_TESTING_OPTIONS */

	pos += 5;

	if (wpa_s->wnm_mode & WNM_BSS_TM_REQ_BSS_TERMINATION_INCLUDED) {
		if (end - pos < 12) {
			wpa_printf(MSG_DEBUG, "WNM: Too short BSS TM Request");
			return;
		}
		os_memcpy(wpa_s->wnm_bss_termination_duration, pos, 12);
		pos += 12; /* BSS Termination Duration */
	}

	if (wpa_s->wnm_mode & WNM_BSS_TM_REQ_ESS_DISASSOC_IMMINENT) {
		char url[256];

		if (end - pos < 1 || 1 + pos[0] > end - pos) {
			wpa_printf(MSG_DEBUG, "WNM: Invalid BSS Transition "
				   "Management Request (URL)");
			return;
		}
		os_memcpy(url, pos + 1, pos[0]);
		url[pos[0]] = '\0';
		pos += 1 + pos[0];

		wpa_msg(wpa_s, MSG_DEBUG, "ESS_DISASSOC_IMMINENT %u %s",
			wpa_s->wnm_dissoc_timer * beacon_int * 128 / 125, url);
	}

	if (wpa_s->wnm_mode & WNM_BSS_TM_REQ_DISASSOC_IMMINENT) {
		wpa_msg(wpa_s, MSG_DEBUG, "WNM: Disassociation Imminent - "
			"Disassociation Timer %u", wpa_s->wnm_dissoc_timer);
		if (wpa_s->wnm_dissoc_timer) {
			/* TODO: mark current BSS less preferred for
			 * selection */
#ifdef ESP_SUPPLICANT
			os_memset(wpa_s->next_scan_bssid, 0, ETH_ALEN);
			wpa_s->next_scan_chan = 0;
			scan_required = true;
#else
			wpa_printf(MSG_DEBUG, "Trying to find another BSS");
			wpa_supplicant_req_scan(wpa_s, 0, 0);
#endif
		}
	}

#ifdef CONFIG_MBO
	vendor = get_ie(pos, end - pos, WLAN_EID_VENDOR_SPECIFIC);
	if (vendor)
		wpas_mbo_ie_trans_req(wpa_s, vendor + 2, vendor[1]);
#endif /* CONFIG_MBO */

	if (wpa_s->wnm_mode & WNM_BSS_TM_REQ_PREF_CAND_LIST_INCLUDED) {
		unsigned int valid_ms;

		wpa_msg(wpa_s, MSG_DEBUG, "WNM: Preferred List Available");
		wnm_deallocate_memory(wpa_s);
		wpa_s->wnm_neighbor_report_elements = os_calloc(
			WNM_MAX_NEIGHBOR_REPORT,
			sizeof(struct neighbor_report));
		if (wpa_s->wnm_neighbor_report_elements == NULL)
			return;

		while (end - pos >= 2 &&
		       wpa_s->wnm_num_neighbor_report < WNM_MAX_NEIGHBOR_REPORT)
		{
			u8 tag = *pos++;
			u8 len = *pos++;

			wpa_printf(MSG_DEBUG, "WNM: Neighbor report tag %u",
				   tag);
			if (len > end - pos) {
				wpa_printf(MSG_DEBUG, "WNM: Truncated request");
				return;
			}
			if (tag == WLAN_EID_NEIGHBOR_REPORT) {
				struct neighbor_report *rep;
				rep = &wpa_s->wnm_neighbor_report_elements[
					wpa_s->wnm_num_neighbor_report];
				wnm_parse_neighbor_report(wpa_s, pos, len, rep);
				wpa_s->wnm_num_neighbor_report++;
#ifdef CONFIG_MBO
				if (wpa_s->wnm_mbo_trans_reason_present &&
				    wpa_s->wnm_num_neighbor_report == 1) {
					rep->is_first = 1;
					wpa_printf(MSG_DEBUG,
						   "WNM: First transition candidate is "
						   MACSTR, MAC2STR(rep->bssid));
				}
#endif /* CONFIG_MBO */
			}

			pos += len;
		}

		if (!wpa_s->wnm_num_neighbor_report) {
			wpa_printf(MSG_DEBUG,
				   "WNM: Candidate list included bit is set, but no candidates found");
			wnm_send_bss_transition_mgmt_resp(
				wpa_s, wpa_s->wnm_dialog_token,
				WNM_BSS_TM_REJECT_NO_SUITABLE_CANDIDATES,
				MBO_TRANSITION_REJECT_REASON_UNSPECIFIED, 0,
				NULL);
			return;
		}

		wnm_sort_cand_list(wpa_s);
		wnm_dump_cand_list(wpa_s);
		valid_ms = valid_int * beacon_int * 128 / 125;
		wpa_printf(MSG_DEBUG, "WNM: Candidate list valid for %u ms",
			   valid_ms);
		os_get_reltime(&wpa_s->wnm_cand_valid_until);
		wpa_s->wnm_cand_valid_until.sec += valid_ms / 1000;
		wpa_s->wnm_cand_valid_until.usec += (valid_ms % 1000) * 1000;
		wpa_s->wnm_cand_valid_until.sec +=
			wpa_s->wnm_cand_valid_until.usec / 1000000;
		wpa_s->wnm_cand_valid_until.usec %= 1000000;

		/*
		 * Fetch the latest scan results from the kernel and check for
		 * candidates based on those results first. This can help in
		 * finding more up-to-date information should the driver has
		 * done some internal scanning operations after the last scan
		 * result update in wpa_supplicant.
		 */
		if (wnm_fetch_scan_results(wpa_s) > 0)
			return;

		/*
		 * Try to use previously received scan results, if they are
		 * recent enough to use for a connection.
		 */
		if (wpa_s->last_scan_res_used > 0) {
			struct os_reltime now;

			os_get_reltime(&now);
			if (!os_reltime_expired(&now, &wpa_s->last_scan, 10)) {
				wpa_printf(MSG_DEBUG,
					   "WNM: Try to use recent scan results");
				if (wnm_scan_process(wpa_s, 0) > 0)
					return;
				wpa_printf(MSG_DEBUG,
					   "WNM: No match in previous scan results - try a new scan");
			}
		}
		wnm_set_scan_freqs(wpa_s);
		if (wpa_s->wnm_num_neighbor_report == 1) {
			os_memcpy(wpa_s->next_scan_bssid,
				  wpa_s->wnm_neighbor_report_elements[0].bssid,
				  ETH_ALEN);
			wpa_printf(MSG_DEBUG,
				   "WNM: Scan only for a specific BSSID since there is only a single candidate "
				   MACSTR, MAC2STR(wpa_s->next_scan_bssid));
		}
#ifdef ESP_SUPPLICANT
		scan_required = true;
	}
	if (scan_required) {
		wpa_printf(MSG_DEBUG, "Trying to find another BSS");
#endif
		wpa_supplicant_req_scan(wpa_s, 0, 0);
	} else if (reply) {
		enum bss_trans_mgmt_status_code status;
		if (wpa_s->wnm_mode & WNM_BSS_TM_REQ_ESS_DISASSOC_IMMINENT)
			status = WNM_BSS_TM_ACCEPT;
		else {
			wpa_msg(wpa_s, MSG_DEBUG, "WNM: BSS Transition Management Request did not include candidates");
			status = WNM_BSS_TM_REJECT_UNSPECIFIED;
		}
		wpa_s->wnm_reply = 0;
		wnm_send_bss_transition_mgmt_resp(
			wpa_s, wpa_s->wnm_dialog_token, status,
			MBO_TRANSITION_REJECT_REASON_UNSPECIFIED, 0, NULL);
	}
}


#define BTM_QUERY_MIN_SIZE	4

int wnm_send_bss_transition_mgmt_query(struct wpa_supplicant *wpa_s,
				       u8 query_reason,
				       const char *btm_candidates,
				       int cand_list)
{
	struct wpabuf *buf;
	int ret;

	wpa_printf(MSG_DEBUG, "WNM: Send BSS Transition Management Query to "
		   MACSTR " query_reason=%u%s",
		   MAC2STR(wpa_s->current_bss->bssid), query_reason,
		   cand_list ? " candidate list" : "");

	buf = wpabuf_alloc(BTM_QUERY_MIN_SIZE);
	if (!buf)
		return -1;

	wpabuf_put_u8(buf, WLAN_ACTION_WNM);
	wpabuf_put_u8(buf, WNM_BSS_TRANS_MGMT_QUERY);
	wpabuf_put_u8(buf, 1);
	wpabuf_put_u8(buf, query_reason);

	if (cand_list)
		wnm_add_cand_list(wpa_s, &buf);

	if (btm_candidates) {
		const size_t max_len = 1000;

		ret = wpabuf_resize(&buf, max_len);
		if (ret < 0) {
			wpabuf_free(buf);
			return ret;
		}

		ret = ieee802_11_parse_candidate_list(btm_candidates,
						      wpabuf_put(buf, 0),
						      max_len);
		if (ret < 0) {
			wpabuf_free(buf);
			return ret;
		}

		wpabuf_put(buf, ret);
	}

	ret = wpa_drv_send_action(wpa_s, 0, 0,
				  wpabuf_head_u8(buf), wpabuf_len(buf), 0);

	wpabuf_free(buf);
	return ret;
}

void ieee802_11_rx_wnm_action(struct wpa_supplicant *wpa_s,
			      u8 *sender, u8 *payload, size_t len)
{
	const u8 *pos, *end;
	u8 act;

	if (len < 2)
		return;

	pos = payload;
	act = *pos++;
	end = payload + len;

	wpa_printf(MSG_DEBUG, "WNM: RX action %u from " MACSTR,
		   act, MAC2STR(sender));

	switch (act) {
	case WNM_BSS_TRANS_MGMT_REQ:
		ieee802_11_rx_bss_trans_mgmt_req(wpa_s, pos, end,
						 0x01);
		break;
	default:
		wpa_printf(MSG_ERROR, "WNM: Unknown request");
		break;
	}
}
