/*
 * hostapd / Initialization and configuration
 * Copyright (c) 2002-2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef HOSTAPD_H
#define HOSTAPD_H

#include "wpa/defs.h"
#include "wpa/ap_config.h"

struct wpa_driver_ops;
struct wpa_ctrl_dst;
struct radius_server_data;
struct upnp_wps_device_sm;
struct hostapd_data;
struct sta_info;
struct hostap_sta_driver_data;
struct ieee80211_ht_capabilities;
struct full_dynamic_vlan;
enum wps_event;
union wps_event_data;

struct hostapd_iface;

struct hapd_interfaces {
	int (*reload_config)(struct hostapd_iface *iface);
	struct hostapd_config * (*config_read_cb)(const char *config_fname);
	int (*ctrl_iface_init)(struct hostapd_data *hapd);
	void (*ctrl_iface_deinit)(struct hostapd_data *hapd);
	int (*for_each_interface)(struct hapd_interfaces *interfaces,
				  int (*cb)(struct hostapd_iface *iface,
					    void *ctx), void *ctx);
	int (*driver_init)(struct hostapd_iface *iface);

	size_t count;
	int global_ctrl_sock;
	char *global_iface_path;
	char *global_iface_name;
	struct hostapd_iface **iface;
};


struct hostapd_probereq_cb {
	int (*cb)(void *ctx, const u8 *sa, const u8 *da, const u8 *bssid,
		  const u8 *ie, size_t ie_len, int ssi_signal);
	void *ctx;
};

#define HOSTAPD_RATE_BASIC 0x00000001

struct hostapd_rate_data {
	int rate; /* rate in 100 kbps */
	int flags; /* HOSTAPD_RATE_ flags */
};

struct hostapd_frame_info {
	u32 channel;
	u32 datarate;
	int ssi_signal; /* dBm */
};


/**
 * struct hostapd_data - hostapd per-BSS data structure
 */
struct hostapd_data {
//	struct hostapd_iface *iface;
	struct hostapd_config *iconf;
	struct hostapd_bss_config *conf;
	int interface_added; /* virtual interface added for this BSS */

	u8 own_addr[ETH_ALEN];

	int num_sta; /* number of entries in sta_list */
//	struct sta_info *sta_list; /* STA info list head */
//#define STA_HASH_SIZE 256
//#define STA_HASH(sta) (sta[5])
//	struct sta_info *sta_hash[STA_HASH_SIZE];

//	/*
//	 * Bitfield for indicating which AIDs are allocated. Only AID values
//	 * 1-2007 are used and as such, the bit at index 0 corresponds to AID
//	 * 1.
//	 */
//#define AID_WORDS ((2008 + 31) / 32)
//	u32 sta_aid[AID_WORDS];

//	const struct wpa_driver_ops *driver;
//	void *drv_priv;

//	void (*new_assoc_sta_cb)(struct hostapd_data *hapd,
//				 struct sta_info *sta, int reassoc);

//	void *msg_ctx; /* ctx for wpa_msg() calls */
//	void *msg_ctx_parent; /* parent interface ctx for wpa_msg() calls */

//	struct radius_client_data *radius;
//	u32 acct_session_id_hi, acct_session_id_lo;
//	struct radius_das_data *radius_das;

//	struct iapp_data *iapp;

//	struct hostapd_cached_radius_acl *acl_cache;
//	struct hostapd_acl_query_data *acl_queries;

	struct wpa_authenticator *wpa_auth;
//	struct eapol_authenticator *eapol_auth;

//	struct rsn_preauth_interface *preauth_iface;
//	time_t michael_mic_failure;
//	int michael_mic_failures;
//	int tkip_countermeasures;

//	int ctrl_sock;
//	struct wpa_ctrl_dst *ctrl_dst;

//	void *ssl_ctx;
//	void *eap_sim_db_priv;
//	struct radius_server_data *radius_srv;

//	int parameter_set_count;

	/* Time Advertisement */
//	u8 time_update_counter;
//	struct wpabuf *time_adv;

#ifdef CONFIG_FULL_DYNAMIC_VLAN
	struct full_dynamic_vlan *full_dynamic_vlan;
#endif /* CONFIG_FULL_DYNAMIC_VLAN */

//	struct l2_packet_data *l2;
//	struct wps_context *wps;

//	int beacon_set_done;
//	struct wpabuf *wps_beacon_ie;
//	struct wpabuf *wps_probe_resp_ie;
#ifdef CONFIG_WPS
	unsigned int ap_pin_failures;
	unsigned int ap_pin_failures_consecutive;
	struct upnp_wps_device_sm *wps_upnp;
	unsigned int ap_pin_lockout_time;
#endif /* CONFIG_WPS */

//	struct hostapd_probereq_cb *probereq_cb;
//	size_t num_probereq_cb;

//	void (*public_action_cb)(void *ctx, const u8 *buf, size_t len,
//				 int freq);
//	void *public_action_cb_ctx;

//	int (*vendor_action_cb)(void *ctx, const u8 *buf, size_t len,
//				int freq);
//	void *vendor_action_cb_ctx;

//	void (*wps_reg_success_cb)(void *ctx, const u8 *mac_addr,
//				   const u8 *uuid_e);
//	void *wps_reg_success_cb_ctx;

//	void (*wps_event_cb)(void *ctx, enum wps_event event,
//			     union wps_event_data *data);
//	void *wps_event_cb_ctx;

//	void (*sta_authorized_cb)(void *ctx, const u8 *mac_addr,
//				  int authorized, const u8 *p2p_dev_addr);
//	void *sta_authorized_cb_ctx;

//	void (*setup_complete_cb)(void *ctx);
//	void *setup_complete_cb_ctx;

#ifdef CONFIG_P2P
	struct p2p_data *p2p;
	struct p2p_group *p2p_group;
	struct wpabuf *p2p_beacon_ie;
	struct wpabuf *p2p_probe_resp_ie;

	/* Number of non-P2P association stations */
	int num_sta_no_p2p;

	/* Periodic NoA (used only when no non-P2P clients in the group) */
	int noa_enabled;
	int noa_start;
	int noa_duration;
#endif /* CONFIG_P2P */
#ifdef CONFIG_INTERWORKING
	size_t gas_frag_limit;
#endif /* CONFIG_INTERWORKING */

#ifdef CONFIG_SQLITE
	struct hostapd_eap_user tmp_eap_user;
#endif /* CONFIG_SQLITE */
};

#if 0
/**
 * struct hostapd_iface - hostapd per-interface data structure
 */
struct hostapd_iface {
	struct hapd_interfaces *interfaces;
	void *owner;
	char *config_fname;
	struct hostapd_config *conf;

	size_t num_bss;
	struct hostapd_data **bss;

	int num_ap; /* number of entries in ap_list */
	struct ap_info *ap_list; /* AP info list head */
	struct ap_info *ap_hash[STA_HASH_SIZE];
	struct ap_info *ap_iter_list;

	unsigned int drv_flags;

	/*
	 * A bitmap of supported protocols for probe response offload. See
	 * struct wpa_driver_capa in driver.h
	 */
	unsigned int probe_resp_offloads;

	struct hostapd_hw_modes *hw_features;
	int num_hw_features;
	struct hostapd_hw_modes *current_mode;
	/* Rates that are currently used (i.e., filtered copy of
	 * current_mode->channels */
	int num_rates;
	struct hostapd_rate_data *current_rates;
	int *basic_rates;
	int freq;

	u16 hw_flags;

	/* Number of associated Non-ERP stations (i.e., stations using 802.11b
	 * in 802.11g BSS) */
	int num_sta_non_erp;

	/* Number of associated stations that do not support Short Slot Time */
	int num_sta_no_short_slot_time;

	/* Number of associated stations that do not support Short Preamble */
	int num_sta_no_short_preamble;

	int olbc; /* Overlapping Legacy BSS Condition */

	/* Number of HT associated stations that do not support greenfield */
	int num_sta_ht_no_gf;

	/* Number of associated non-HT stations */
	int num_sta_no_ht;

	/* Number of HT associated stations 20 MHz */
	int num_sta_ht_20mhz;

	/* Overlapping BSS information */
	int olbc_ht;

	u16 ht_op_mode;
	void (*scan_cb)(struct hostapd_iface *iface);
};
#endif

#if 0
/* hostapd.c */
int hostapd_for_each_interface(struct hapd_interfaces *interfaces,
			       int (*cb)(struct hostapd_iface *iface,
					 void *ctx), void *ctx);
int hostapd_reload_config(struct hostapd_iface *iface);
struct hostapd_data *
hostapd_alloc_bss_data(struct hostapd_iface *hapd_iface,
		       struct hostapd_config *conf,
		       struct hostapd_bss_config *bss);
int hostapd_setup_interface(struct hostapd_iface *iface);
int hostapd_setup_interface_complete(struct hostapd_iface *iface, int err);
void hostapd_interface_deinit(struct hostapd_iface *iface);
void hostapd_interface_free(struct hostapd_iface *iface);
void hostapd_new_assoc_sta(struct hostapd_data *hapd, struct sta_info *sta,
			   int reassoc);
void hostapd_interface_deinit_free(struct hostapd_iface *iface);
int hostapd_enable_iface(struct hostapd_iface *hapd_iface);
int hostapd_reload_iface(struct hostapd_iface *hapd_iface);
int hostapd_disable_iface(struct hostapd_iface *hapd_iface);
int hostapd_add_iface(struct hapd_interfaces *ifaces, char *buf);
int hostapd_remove_iface(struct hapd_interfaces *ifaces, char *buf);

/* utils.c */
int hostapd_register_probereq_cb(struct hostapd_data *hapd,
				 int (*cb)(void *ctx, const u8 *sa,
					   const u8 *da, const u8 *bssid,
					   const u8 *ie, size_t ie_len,
					   int ssi_signal),
				 void *ctx);
void hostapd_prune_associations(struct hostapd_data *hapd, const u8 *addr);

/* drv_callbacks.c (TODO: move to somewhere else?) */
int hostapd_notif_assoc(struct hostapd_data *hapd, const u8 *addr,
			const u8 *ie, size_t ielen, int reassoc);
void hostapd_notif_disassoc(struct hostapd_data *hapd, const u8 *addr);
void hostapd_event_sta_low_ack(struct hostapd_data *hapd, const u8 *addr);
int hostapd_probe_req_rx(struct hostapd_data *hapd, const u8 *sa, const u8 *da,
			 const u8 *bssid, const u8 *ie, size_t ie_len,
			 int ssi_signal);
void hostapd_event_ch_switch(struct hostapd_data *hapd, int freq, int ht,
			     int offset);

const struct hostapd_eap_user *
hostapd_get_eap_user(struct hostapd_data *hapd, const u8 *identity,
		     size_t identity_len, int phase2);
#endif

#endif /* HOSTAPD_H */
