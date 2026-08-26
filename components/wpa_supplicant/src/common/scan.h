/*
 * WPA Supplicant - Scanning
 * Copyright (c) 2003-2014, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef SCAN_H
#define SCAN_H

/*
 * Noise floor values to use when we have signal strength
 * measurements, but no noise floor measurements. These values were
 * measured in an office environment with many APs.
 */
#define DEFAULT_NOISE_FLOOR_2GHZ (-89)

struct wpa_driver_scan_params;
void wpa_supplicant_req_scan(struct wpa_supplicant *wpa_s, int sec, int usec);
int wpa_supplicant_trigger_scan(struct wpa_supplicant *wpa_s,
				struct wpa_driver_scan_params *params);
const u8 * wpa_scan_get_ie(const struct wpa_scan_res *res, u8 ie);
void wpa_scan_free_params(struct wpa_driver_scan_params *params);
#endif /* SCAN_H */
