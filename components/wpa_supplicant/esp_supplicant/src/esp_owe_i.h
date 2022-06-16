/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
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
#endif /* ESP_OWE_H  */
