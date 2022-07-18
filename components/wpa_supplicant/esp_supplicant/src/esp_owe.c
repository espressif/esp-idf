/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef CONFIG_OWE_STA

#include "crypto/crypto.h"
#include "esp_owe_i.h"
#include "rsn_supp/wpa.h"

uint8_t *owe_build_dhie(uint16_t group)
{
    struct wpa_sm *sm = NULL;
    sm = get_wpa_sm();
    return (uint8_t *)(owe_build_assoc_req(sm, group));
}

void owe_deinit(void)
{
    struct wpa_sm *sm;
    sm = get_wpa_sm();
    if (sm->key_mgmt == WPA_KEY_MGMT_OWE) {
        if (sm->owe_ie) {
            wpabuf_free(sm->owe_ie);
            sm->owe_ie = NULL;
        }
        crypto_ecdh_deinit(sm->owe_ecdh);
        sm->owe_ecdh = NULL;
    }
}

void esp_wifi_register_owe_cb(struct wpa_funcs *wpa_cb)
{
    wpa_cb->owe_build_dhie = owe_build_dhie;
    wpa_cb->owe_process_assoc_resp = owe_process_assoc_resp;
}
#endif /* CONFIG_OWE_STA */
