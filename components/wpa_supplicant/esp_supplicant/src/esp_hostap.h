/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
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
u16 esp_send_assoc_resp(struct hostapd_data *data, struct sta_info *sta,
                    const u8 *addr, u16 status_code, bool omit_rsnxe,
                    int subtype);
#endif

#ifdef __cplusplus
}
#endif

#endif /* ESP_HOSTAP_H */
