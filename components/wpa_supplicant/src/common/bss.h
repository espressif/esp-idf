/*
 * BSS table
 * Copyright (c) 2009-2019, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef BSS_H
#define BSS_H

struct wpa_scan_res;

/**
 * struct wpa_bss - BSS table
 *
 * This structure is used to store information about neighboring BSSes in
 * generic format. It is mainly updated based on scan results from the driver.
 */
struct wpa_bss {
	/** List entry for struct wpa_supplicant::bss */
	struct dl_list list;
	/** List entry for struct wpa_supplicant::bss_id */
	struct dl_list list_id;
	/** Unique identifier for this BSS entry */
	unsigned int id;
	/** Index of the last scan update */
	unsigned int last_update_idx;
	/** Information flags about the BSS/IBSS (WPA_BSS_*) */
	unsigned int flags;
	/** BSSID */
	u8 bssid[ETH_ALEN];
	/** SSID */
	u8 ssid[SSID_MAX_LEN];
	/** Length of SSID */
	size_t ssid_len;
	/** Frequency of the channel in MHz (e.g., 2412 = channel 1) */
	int channel;
	/** Beacon interval in TUs (host byte order) */
	u16 beacon_int;
	/** Capability information field in host byte order */
	u16 caps;
	/** Signal quality */
	int qual;
	/** Noise level */
	int noise;
	/** Signal level */
	int level;
	/** Timestamp of last Beacon/Probe Response frame */
	u64 tsf;
	/** Timestamp of parent aganist which it was taken */
	u64 parent_tsf;
	/** Time of the last update (i.e., Beacon or Probe Response RX) */
	struct os_reltime last_update;
	/** Length of the following IE field in octets (from Probe Response) */
	size_t ie_len;
	/** Length of the following Beacon IE field in octets */
	size_t beacon_ie_len;
	/* followed by ie_len octets of IEs */
	/* followed by beacon_ie_len octets of IEs */
};

void wpa_bss_update_start(struct wpa_supplicant *wpa_s);
void wpa_bss_update_scan_res(struct wpa_supplicant *wpa_s,
			     struct wpa_scan_res *res,
			     struct os_reltime *fetch_time);
void wpa_bss_remove(struct wpa_supplicant *wpa_s, struct wpa_bss *bss,
		    const char *reason);
void wpa_bss_update_end(struct wpa_supplicant *wpa_s);
int wpa_bss_init(struct wpa_supplicant *wpa_s);
void wpa_bss_deinit(struct wpa_supplicant *wpa_s);
void wpa_bss_flush(struct wpa_supplicant *wpa_s);
struct wpa_bss * wpa_bss_get(struct wpa_supplicant *wpa_s, const u8 *bssid,
			     const u8 *ssid, size_t ssid_len);
struct wpa_bss * wpa_bss_get_bssid(struct wpa_supplicant *wpa_s,
				   const u8 *bssid);
const u8 * wpa_bss_get_ie(const struct wpa_bss *bss, u8 ie);
int wpa_bss_ext_capab(const struct wpa_bss *bss, unsigned int capab);
struct wpa_bss * wpa_bss_get_next_bss(struct wpa_supplicant *wpa_s,
				   struct wpa_bss *prev_bss);

void calculate_update_time(const struct os_reltime *fetch_time,
			   unsigned int age_ms,
			   struct os_reltime *update_time);

#endif /* BSS_H */
