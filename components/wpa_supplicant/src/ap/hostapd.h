/*
 * hostapd / Initialization and configuration
 * Copyright (c) 2002-2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef HOSTAPD_H
#define HOSTAPD_H

#include "common/defs.h"
#include "ap/ap_config.h"

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

struct hostapd_sae_commit_queue {
	struct dl_list list;
	size_t len;
	u8 bssid[ETH_ALEN];
	u32 auth_transaction;
	u16 status;
	u8 msg[];
};

#ifdef CONFIG_WPS
enum hapd_wps_status {
	WPS_SUCCESS_STATUS = 1,
	WPS_FAILURE_STATUS
};

enum pbc_status {
	WPS_PBC_STATUS_DISABLE,
	WPS_PBC_STATUS_ACTIVE,
	WPS_PBC_STATUS_TIMEOUT,
	WPS_PBC_STATUS_OVERLAP
};

struct wps_stat {
	enum hapd_wps_status status;
	enum pbc_status pbc_status;
	u8 peer_addr[ETH_ALEN];
};
#endif

/**
 * struct hostapd_data - hostapd per-BSS data structure
 */
struct hostapd_data {
	struct hostapd_config *iconf;
	struct hostapd_bss_config *conf;
	int interface_added; /* virtual interface added for this BSS */

	u8 own_addr[ETH_ALEN];
	struct sta_info *sta_list; /* STA info list head */
#define STA_HASH_SIZE 16
#define STA_HASH(sta) (sta[5] & 0xf)
	struct sta_info *sta_hash[STA_HASH_SIZE];
	int num_sta; /* number of entries in sta_list */

	struct eapol_authenticator *eapol_auth;
	struct wpa_authenticator *wpa_auth;

#ifdef CONFIG_FULL_DYNAMIC_VLAN
	struct full_dynamic_vlan *full_dynamic_vlan;
#endif /* CONFIG_FULL_DYNAMIC_VLAN */

#ifdef CONFIG_WPS
	struct wps_context *wps;
	unsigned int ap_pin_failures;
	unsigned int ap_pin_failures_consecutive;
	struct upnp_wps_device_sm *wps_upnp;
	unsigned int ap_pin_lockout_time;

	struct wps_stat wps_stats;
	void (*wps_event_cb)(void *ctx, enum wps_event event,
			     union wps_event_data *data);
#endif /* CONFIG_WPS */

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
#ifdef CONFIG_SAE

#define COMEBACK_KEY_SIZE 8
#define COMEBACK_PENDING_IDX_SIZE 256

	/** Key used for generating SAE anti-clogging tokens */
	u8 comeback_key[COMEBACK_KEY_SIZE];
	struct os_reltime last_comeback_key_update;
	u16 comeback_idx;
	u16 comeback_pending_idx[COMEBACK_PENDING_IDX_SIZE];
	int dot11RSNASAERetransPeriod;
	struct dl_list sae_commit_queue; /* struct hostapd_sae_commit_queue */
#endif /* CONFIG_SAE */

#ifdef CONFIG_INTERWORKING
	size_t gas_frag_limit;
#endif /* CONFIG_INTERWORKING */

#ifdef CONFIG_SQLITE
	struct hostapd_eap_user tmp_eap_user;
#endif /* CONFIG_SQLITE */
};

struct hostapd_data *hostapd_get_hapd_data(void);

const struct hostapd_eap_user *
hostapd_get_eap_user(struct hostapd_data *hapd, const u8 *identity,
		     size_t identity_len, int phase2);

#endif /* HOSTAPD_H */
