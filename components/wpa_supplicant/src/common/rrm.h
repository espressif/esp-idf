
#ifndef RRM_H
#define RRM_H

#include "common/defs.h"
#include "utils/list.h"
#include "esp_wifi_types.h"
#define RRM_NEIGHBOR_REPORT_TIMEOUT 1 /* 1 second for AP to send a report */

void wpas_rrm_reset(struct wpa_supplicant *wpa_s);
void wpas_rrm_process_neighbor_rep(struct wpa_supplicant *wpa_s,
				   const u8 *report, size_t report_len);
int wpas_rrm_send_neighbor_rep_request(struct wpa_supplicant *wpa_s,
				       const struct wpa_ssid_value *ssid,
				       int lci, int civic,
				       void (*cb)(void *ctx,
						  const u8 *neighbor_rep, size_t len),
				       void *cb_ctx);
void wpas_rrm_handle_radio_measurement_request(struct wpa_supplicant *wpa_s,
					       const u8 *src, const u8 *dst,
					       const u8 *frame, size_t len);
void wpas_rrm_handle_link_measurement_request(struct wpa_supplicant *wpa_s,
					      const u8 *src,
					      const u8 *frame, size_t len,
					      int rssi);
int wpas_beacon_rep_scan_process(struct wpa_supplicant *wpa_s,
				 u64 scan_start_tsf);
void wpas_clear_beacon_rep_data(struct wpa_supplicant *wpa_s);

#endif
