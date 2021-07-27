/**
 * Copyright 2020 Espressif Systems (Shanghai) PTE LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ESP_SCAN_I_H
#define ESP_SCAN_I_H
void esp_scan_init(struct wpa_supplicant *wpa_s);
void esp_scan_deinit(struct wpa_supplicant *wpa_s);
int esp_handle_beacon_probe(u8 type, u8 *frame, size_t len, u8 *sender,
			    u32 rssi, u8 channel, u64 current_tsf);

void esp_supplicant_handle_scan_done_evt(void);
#endif
