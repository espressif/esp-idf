/*
 * wpa_supplicant - Internal definitions
 * Copyright (c) 2003-2014, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef WPA_SUPPLICANT_I_H
#define WPA_SUPPLICANT_I_H


#include "drivers/driver.h"
#include "common/ieee802_11_defs.h"
/*
 * struct rrm_data - Data used for managing RRM features
 */
struct rrm_data {
	/* rrm_used - indication regarding the current connection */
	unsigned int rrm_used:1;

	/*
	 * notify_neighbor_rep - Callback for notifying report requester
	 */
	void (*notify_neighbor_rep)(void *ctx, const u8 *neighbor_rep, size_t len);

	/*
	 * neighbor_rep_cb_ctx - Callback context
	 * Received in the callback registration, and sent to the callback
	 * function as a parameter.
	 */
	void *neighbor_rep_cb_ctx;

	/* next_neighbor_rep_token - Next request's dialog token */
	u8 next_neighbor_rep_token;

	/* token - Dialog token of the current radio measurement */
	u8 token;

	/* destination address of the current radio measurement request */
	u8 dst_addr[ETH_ALEN];
};

#define SSID_MAX_LEN 32
struct beacon_rep_data {
	u8 token;
	u8 last_indication;
	struct wpa_driver_scan_params scan_params;
	u8 ssid[SSID_MAX_LEN];
	size_t ssid_len;
	u8 bssid[ETH_ALEN];
	enum beacon_report_detail report_detail;
	struct bitfield *eids;
};

enum scan_trigger_reason {
	REASON_INVALID,
	REASON_RRM_BEACON_REPORT,
	REASON_WNM_BSS_TRANS_REQ,
};

struct wpa_supplicant {
	int disable_btm;
	/* rrm ie */
	uint8_t rrm_ie[5];
	u8 extend_caps[8];

	int scanning;
	enum scan_trigger_reason scan_reason;
	u64 scan_start_tsf;
	u8 tsf_bssid[ETH_ALEN];
	struct wpa_bss *current_bss;

	struct dl_list bss; /* struct wpa_bss::list */
	struct dl_list bss_id; /* struct wpa_bss::list_id */
	size_t num_bss;
	unsigned int bss_update_idx;
	unsigned int bss_next_id;

	 /*
	  * Pointers to BSS entries in the order they were in the last scan
	  * results.
	  */
	struct wpa_bss **last_scan_res;
	unsigned int last_scan_res_used;
	unsigned int last_scan_res_size;
	struct os_reltime last_scan;

	struct os_reltime scan_trigger_time, scan_start_time;

	u8 next_scan_bssid[ETH_ALEN];
	/* type and subtype of frames for which supplicant has registered */
	uint32_t type, subtype;
	u8 next_scan_chan;
#ifdef CONFIG_WNM
	u8 wnm_dialog_token;
	u8 wnm_reply;
	u8 wnm_num_neighbor_report;
	u8 wnm_mode;
	u16 wnm_dissoc_timer;
	u8 wnm_bss_termination_duration[12];
	struct neighbor_report *wnm_neighbor_report_elements;
	struct os_reltime wnm_cand_valid_until;
#endif /* CONFIG_WNM */
	struct rrm_data rrm;
	struct beacon_rep_data beacon_rep_data;
	struct os_reltime beacon_rep_scan;
};

#endif
