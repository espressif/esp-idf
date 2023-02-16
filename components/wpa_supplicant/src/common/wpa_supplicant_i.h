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

struct wpa_bss_tmp_disallowed {
	struct dl_list list;
	u8 bssid[ETH_ALEN];
#ifndef ESP_SUPPLICANT
	int rssi_threshold;
#endif
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

#ifdef CONFIG_SAE_PK
struct sae_pk_elems {
	u8 *fils_pk;
	u8 fils_pk_len;
	u8 *fils_key_confirm;
	u8 fils_key_confirm_len;
	u8 *sae_pk;
	u8 sae_pk_len;
};
#endif

struct wpa_supplicant {

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
	int disable_btm;
	unsigned int disable_mbo_oce;
	u8 extend_caps[8];
	u8 wnm_dialog_token;
	u8 wnm_reply;
	u8 wnm_num_neighbor_report;
	u8 wnm_mode;
	u16 wnm_dissoc_timer;
	u8 wnm_bss_termination_duration[12];
	struct neighbor_report *wnm_neighbor_report_elements;
	struct os_reltime wnm_cand_valid_until;
#ifdef CONFIG_MBO
        unsigned int wnm_mbo_trans_reason_present:1;
        u8 wnm_mbo_transition_reason;
	/* Multiband operation non-preferred channel */
	struct wpa_mbo_non_pref_channel {
		enum mbo_non_pref_chan_reason reason;
		u8 oper_class;
		u8 chan;
		u8 preference;
	} *non_pref_chan;
	size_t non_pref_chan_num;
	u8 mbo_wnm_token;
	/**
	 * enable_oce - Enable OCE if it is enabled by user and device also
	 *              supports OCE.
	 * User can enable OCE with wpa_config's 'oce' parameter as follows -
	 *  - Set BIT(0) to enable OCE in non-AP STA mode.
	 *  - Set BIT(1) to enable OCE in STA-CFON mode.
	 */
	u8 enable_oce;
	struct dl_list bss_tmp_disallowed;
#endif /* CONFIG_MBO */
#endif /* CONFIG_WNM */
#ifdef CONFIG_RRM
	/* rrm ie */
	uint8_t rrm_ie[5];
	struct rrm_data rrm;
	struct beacon_rep_data beacon_rep_data;
	struct os_reltime beacon_rep_scan;
#endif
#ifdef CONFIG_SAE_PK
	struct sae_pk_elems sae_pk_elems;
#endif

};

struct non_pref_chan_s;

/* MBO functions */
int wpas_mbo_ie(struct wpa_supplicant *wpa_s, u8 *buf, size_t len,
		int add_oce_capa);
const u8 * mbo_attr_from_mbo_ie(const u8 *mbo_ie, enum mbo_attr_id attr);
const u8 * wpas_mbo_get_bss_attr(struct wpa_bss *bss, enum mbo_attr_id attr);
const u8 * mbo_get_attr_from_ies(const u8 *ies, size_t ies_len,
				 enum mbo_attr_id attr);
void wpas_mbo_scan_ie(struct wpa_supplicant *wpa_s, struct wpabuf *ie);
void wpas_mbo_ie_trans_req(struct wpa_supplicant *wpa_s, const u8 *ie,
			   size_t len);
size_t wpas_mbo_ie_bss_trans_reject(struct wpa_supplicant *wpa_s, u8 *pos,
				    size_t len,
				    enum mbo_transition_reject_reason reason);
void wpas_mbo_update_cell_capa(struct wpa_supplicant *wpa_s, u8 mbo_cell_capa);
struct wpabuf * mbo_build_anqp_buf(struct wpa_supplicant *wpa_s,
				   struct wpa_bss *bss, u32 mbo_subtypes);
void mbo_parse_rx_anqp_resp(struct wpa_supplicant *wpa_s,
			    struct wpa_bss *bss, const u8 *sa,
			    const u8 *data, size_t slen);
void wpas_update_mbo_connect_params(struct wpa_supplicant *wpa_s);

int wpas_mbo_update_non_pref_chan(struct wpa_supplicant *wpa_s,
				  struct non_pref_chan_s *non_pref_chan);

#endif /* WPA_SUPPLICANT_I_H */
