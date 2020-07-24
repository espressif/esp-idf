/*
 * Driver interface definition
 * Copyright (c) 2003-2017, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 *
 * This file defines a driver interface used by both %wpa_supplicant and
 * hostapd. The first part of the file defines data structures used in various
 * driver operations. This is followed by the struct wpa_driver_ops that each
 * driver wrapper will beed to define with callback functions for requesting
 * driver operations. After this, there are definitions for driver event
 * reporting with wpa_supplicant_event() and some convenience helper functions
 * that can be used to report events.
 */

#ifndef DRIVER_H
#define DRIVER_H

#define WPA_SUPPLICANT_DRIVER_VERSION 4

#include "common/defs.h"
#include "common/ieee802_11_defs.h"
#include "common/wpa_common.h"
#include "utils/list.h"
struct wpa_bss;
struct wpa_supplicant;
/**
 * struct wpa_scan_res - Scan result for an BSS/IBSS
 * @flags: information flags about the BSS/IBSS (WPA_SCAN_*)
 * @bssid: BSSID
 * @freq: frequency of the channel in MHz (e.g., 2412 = channel 1)
 * @beacon_int: beacon interval in TUs (host byte order)
 * @caps: capability information field in host byte order
 * @qual: signal quality
 * @noise: noise level
 * @level: signal level
 * @tsf: Timestamp
 * @age: Age of the information in milliseconds (i.e., how many milliseconds
 * ago the last Beacon or Probe Response frame was received)
 * @snr: Signal-to-noise ratio in dB (calculated during scan result processing)
 * @parent_tsf: Time when the Beacon/Probe Response frame was received in terms
 * of TSF of the BSS specified by %tsf_bssid.
 * @tsf_bssid: The BSS that %parent_tsf TSF time refers to.
 * @ie_len: length of the following IE field in octets
 * @beacon_ie_len: length of the following Beacon IE field in octets
 *
 * This structure is used as a generic format for scan results from the
 * driver. Each driver interface implementation is responsible for converting
 * the driver or OS specific scan results into this format.
 *
 * If the driver does not support reporting all IEs, the IE data structure is
 * constructed of the IEs that are available. This field will also need to
 * include SSID in IE format. All drivers are encouraged to be extended to
 * report all IEs to make it easier to support future additions.
 *
 * This structure data is followed by ie_len octets of IEs from Probe Response
 * frame (or if the driver does not indicate source of IEs, these may also be
 * from Beacon frame). After the first set of IEs, another set of IEs may follow
 * (with beacon_ie_len octets of data) if the driver provides both IE sets.
 */
struct wpa_scan_res {
	unsigned int flags;
	u8 bssid[ETH_ALEN];
	int chan;
	u16 beacon_int;
	u16 caps;
	int noise;
	int level;
	u64 tsf;
	unsigned int age;
	u64 parent_tsf;
	u8 tsf_bssid[ETH_ALEN];
	size_t ie_len;
	size_t beacon_ie_len;
	/* Followed by ie_len + beacon_ie_len octets of IE data */
};

/**
 * struct wpa_scan_results - Scan results
 * @res: Array of pointers to allocated variable length scan result entries
 * @num: Number of entries in the scan result array
 * @fetch_time: Time when the results were fetched from the driver
 */
struct wpa_scan_results {
	struct wpa_scan_res **res;
	size_t num;
	struct os_reltime fetch_time;
};

#define WPAS_MAX_SCAN_SSIDS 1

/**
 * struct wpa_driver_scan_ssid - SSIDs to scan for
 * @ssid - specific SSID to scan for (ProbeReq)
 *	%NULL or zero-length SSID is used to indicate active scan
 *	with wildcard SSID.
 * @ssid_len - Length of the SSID in octets
 */
struct wpa_driver_scan_ssid {
	const u8 *ssid;
	size_t ssid_len;
};

/**
 * struct wpa_driver_scan_params - Scan parameters
 * Data for struct wpa_driver_ops::scan2().
 */
struct wpa_driver_scan_params {
	/**
	 * ssids - SSIDs to scan for
	 */
	struct wpa_driver_scan_ssid ssids[WPAS_MAX_SCAN_SSIDS];

	/**
	 * num_ssids - Number of entries in ssids array
	 * Zero indicates a request for a passive scan.
	 */
	size_t num_ssids;

	/**
	 * freqs - Array of frequencies to scan or %NULL for all frequencies
	 *
	 * The frequency is set in MHz. The array is zero-terminated.
	 */
	int channel;

	/**
	 * bssid - Specific BSSID to scan for
	 *
	 * This optional parameter can be used to replace the default wildcard
	 * BSSID with a specific BSSID to scan for if results are needed from
	 * only a single BSS.
	 */
	const u8 *bssid;

	 /**
	  * duration - Dwell time on each channel
	  *
	  * This optional parameter can be used to set the dwell time on each
	  * channel. In TUs.
	  */
	 u16 duration;

	 unsigned int duration_mandatory;
};

/**
 * struct scan_info - Optional data for EVENT_SCAN_RESULTS events
 * @aborted: Whether the scan was aborted
 * @freqs: Scanned frequencies in MHz (%NULL = all channels scanned)
 * @num_freqs: Number of entries in freqs array
 * @ssids: Scanned SSIDs (%NULL or zero-length SSID indicates wildcard
 *	SSID)
 * @num_ssids: Number of entries in ssids array
 * @external_scan: Whether the scan info is for an external scan
 * @nl_scan_event: 1 if the source of this scan event is a normal scan,
 * 	0 if the source of the scan event is a vendor scan
 * @scan_start_tsf: Time when the scan started in terms of TSF of the
 *	BSS that the interface that requested the scan is connected to
 *	(if available).
 * @scan_start_tsf_bssid: The BSSID according to which %scan_start_tsf
 *	is set.
 */
struct scan_info {
	int aborted;
	const int *freqs;
	size_t num_freqs;
	struct wpa_driver_scan_ssid ssids[WPAS_MAX_SCAN_SSIDS];
	size_t num_ssids;
	int external_scan;
	int nl_scan_event;
	u64 scan_start_tsf;
	u8 scan_start_tsf_bssid[ETH_ALEN];
} scan_info;


/* driver_common.c */
void wpa_scan_results_free(struct wpa_scan_results *res);

int wpa_drv_send_action(struct wpa_supplicant *wpa_s,
			unsigned int chan, unsigned int wait,
			const u8 *data, size_t data_len, int no_cck);

void wpa_supplicant_connect(struct wpa_supplicant *wpa_s,
			    struct wpa_bss *bss, char *ssid);
#endif /* DRIVER_H */
