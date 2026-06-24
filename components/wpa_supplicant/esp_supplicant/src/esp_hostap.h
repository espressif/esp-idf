/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_HOSTAP_H
#define ESP_HOSTAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sdkconfig.h"
#ifdef CONFIG_ESP_WIFI_SOFTAP_SUPPORT
void *hostap_init(void);
bool hostap_deinit(void *data);
u16 esp_send_assoc_resp(struct hostapd_data *data, const u8 *addr,
                        u16 status_code, bool omit_rsnxe, int subtype);
int esp_send_sae_auth_reply(struct hostapd_data *hapd,
                            const u8 *dst, const u8 *bssid,
                            u16 auth_alg, u16 auth_transaction, u16 resp,
                            const u8 *ies, size_t ies_len);
#endif

#ifdef __cplusplus
}
#endif

#endif /* ESP_HOSTAP_H */
