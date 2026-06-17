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
 * OWE_DHIE_LEN: DH Parameter element length for group 19 (secp256r1).
 *
 * Wire format (IEEE 802.11 Extension element):
 *   byte 1     WLAN_EID_EXTENSION
 *   byte 2     length of remainder (extension ID + group + pubkey), typically 35
 *   byte 3     WLAN_EID_EXT_OWE_DH_PARAM (extension element ID)
 *   bytes 4–5  DH group ID (little-endian), e.g. IANA_SECP256R1 (19)
 *   bytes 6–37 DH public key (32 octets for this group/key representation)
 *
 * Total = 2 + 35 = 37 octets.
 */
#define OWE_DHIE_LEN 37
#define OWE_IE_INIT_LEN (257 + OWE_DHIE_LEN) /* RSNE + DH IE */
struct wpabuf *esp_owe_build_assoc_resp_dhie(struct hostapd_data *hapd, const u8 *bssid, int *owe_ie_len);

#endif /* CONFIG_OWE_SOFTAP */
#endif /* ESP_OWE_H  */
