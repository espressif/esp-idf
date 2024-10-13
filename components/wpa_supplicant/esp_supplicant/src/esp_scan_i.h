/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_SCAN_I_H
#define ESP_SCAN_I_H

#define SUPPLICANT_SCAN_ACTIVE_SCAN_MIN_DURATION 10
#define SUPPLICANT_SCAN_ACTIVE_SCAN_MAX_DURATION 70

void esp_scan_init(struct wpa_supplicant *wpa_s);
void esp_scan_deinit(struct wpa_supplicant *wpa_s);
int esp_handle_beacon_probe(u8 type, u8 *frame, size_t len, u8 *sender,
                            int8_t rssi, u8 channel, u64 current_tsf);

void esp_supplicant_handle_scan_done_evt(void);
#endif
