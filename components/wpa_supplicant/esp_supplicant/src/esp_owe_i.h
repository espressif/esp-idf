/*
 * SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_OWE_H
#define ESP_OWE_H

#ifdef CONFIG_OWE_STA

#include "esp_wifi_driver.h"

#define OWE_PMK_LEN    32
#define OWE_PMKID_LEN  16
#define OWE_DH_GRP19   19
#define OWE_PRIME_LEN  32

void owe_deinit(void);
void esp_wifi_register_owe_cb(struct wpa_funcs *wpa_cb);

#endif /* CONFIG_OWE_STA */

#ifdef CONFIG_OWE_SOFTAP

#include "ap/hostapd.h"

/*
OWE_DHIE_LEN = 1 byte   {WLAN_EID_EXTENSION}
             + 1 byte   {len of DHIE (1(pub_key len) + 2(dh group) + 32(len of pub_key)) = 35)}
             + 1 byte   {pub_key len}
             + 2 bytes  {DH group}
             + 32 bytes {public key}
*/
#define OWE_DHIE_LEN 37
struct wpabuf *esp_owe_build_assoc_resp_dhie(struct hostapd_data *hapd, const u8 *bssid, int *owe_ie_len);

#endif /* CONFIG_OWE_SOFTAP */
#endif /* ESP_OWE_H  */
