/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef ESP_SUPPLICANT

#include "utils/includes.h"
#include "utils/common.h"
#include "common/eapol_common.h"
#include "rsn_supp/wpa.h"
#include "rsn_supp/pmksa_cache.h"
#include "esp_wpas_glue.h"
#include "esp_private/wifi.h"

u8 *wpa_alloc_eapol(void *sm, u8 type,
                    const void *data, u16 data_len,
                    size_t *msg_len, void **data_pos)
{
    void *buffer;
    struct ieee802_1x_hdr *hdr;

    *msg_len = sizeof(struct ieee802_1x_hdr) + data_len;

    buffer = os_malloc(*msg_len + sizeof(struct l2_ethhdr));

    if (buffer == NULL) {
        return NULL;
    }

    /* XXX: reserve l2_ethhdr is enough */
    hdr = (struct ieee802_1x_hdr *)((char *)buffer + sizeof(struct l2_ethhdr));

    hdr->version = DEFAULT_EAPOL_VERSION;
    hdr->type = type;
    hdr->length = host_to_be16(data_len);

    if (data) {
        memcpy(hdr + 1, data, data_len);
    } else {
        memset(hdr + 1, 0, data_len);
    }

    if (data_pos) {
        *data_pos = hdr + 1;
    }

    return (u8 *) hdr;
}

void wpa_free_eapol(u8 *buffer)
{
    if (!buffer) {
        return;
    }
    buffer = buffer - sizeof(struct l2_ethhdr);
    os_free(buffer);
}

int wpa_ether_send(void *ctx, const u8 *dest, u16 proto,
                   const u8 *data, size_t data_len)
{
    void *buffer = (void *)(data - sizeof(struct l2_ethhdr));
    struct l2_ethhdr *eth = (struct l2_ethhdr *)buffer;

    os_memcpy(eth->h_dest, dest, ETH_ALEN);
    os_memcpy(eth->h_source, gWpaSm.own_addr, ETH_ALEN);
    eth->h_proto = host_to_be16(proto);

    return esp_wifi_internal_tx(WIFI_IF_STA, buffer, sizeof(struct l2_ethhdr) + data_len);
}

int hostapd_send_eapol(const u8 *source, const u8 *sta_addr,
		       const u8 *data, size_t data_len)
{
    void *buffer = os_malloc(data_len + sizeof(struct l2_ethhdr));
    struct l2_ethhdr *eth = buffer;

    if (!buffer){
        wpa_printf( MSG_DEBUG, "send_eapol, buffer=%p", buffer);
        return -1;
    }

    memcpy(eth->h_dest, sta_addr, ETH_ALEN);
    memcpy(eth->h_source, source, ETH_ALEN);
    eth->h_proto = host_to_be16(ETH_P_EAPOL);

    memcpy((char *)buffer + sizeof(struct l2_ethhdr), data, data_len);
    esp_wifi_internal_tx(WIFI_IF_AP, buffer, sizeof(struct l2_ethhdr) + data_len);
    os_free(buffer);
    return 0;

}

void wpa_supplicant_transition_disable(struct wpa_sm *sm, u8 bitmap)
{
    wpa_printf(MSG_DEBUG, "TRANSITION_DISABLE %02x", bitmap);

    if  ((bitmap & TRANSITION_DISABLE_WPA3_PERSONAL) &&
          wpa_key_mgmt_sae(sm->key_mgmt)) {
        esp_wifi_sta_disable_wpa2_authmode_internal();
    }
}

u8 *wpa_sm_alloc_eapol(struct wpa_sm *sm, u8 type,
                       const void *data, u16 data_len,
                       size_t *msg_len, void **data_pos)
{
    return wpa_alloc_eapol(sm, type, data, data_len, msg_len, data_pos);
}

void wpa_sm_free_eapol(u8 *buffer)
{
    return wpa_free_eapol(buffer);
}

void  wpa_sm_deauthenticate(struct wpa_sm *sm, u8 reason_code)
{
    /*only need send deauth frame when associated*/
    if (WPA_SM_STATE(sm) >= WPA_ASSOCIATED) {
        pmksa_cache_clear_current(sm);
        wpa_deauthenticate(reason_code);
    }
}

/**
 * mlme_setprotection - MLME-SETPROTECTION.request primitive
 * @priv: Private driver interface data
 * @addr: Address of the station for which to set protection (may be
 * %NULL for group keys)
 * @protect_type: MLME_SETPROTECTION_PROTECT_TYPE_*
 * @key_type: MLME_SETPROTECTION_KEY_TYPE_*
 * Returns: 0 on success, -1 on failure
 *
 * This is an optional function that can be used to set the driver to
 * require protection for Tx and/or Rx frames. This uses the layer
 * interface defined in IEEE 802.11i-2004 clause 10.3.22.1
 * (MLME-SETPROTECTION.request). Many drivers do not use explicit
 * set protection operation; instead, they set protection implicitly
 * based on configured keys.
 */
int  wpa_sm_mlme_setprotection(struct wpa_sm *sm, const u8 *addr,
                               int protect_type, int key_type)
{
    return 0;
}

/*
 *use above two functions to get wpa_ie and rsn_ie, then don't need wpa_sm_get_beacon_ie function
*/
int  wpa_sm_get_beacon_ie(struct wpa_sm *sm)
{
    return 0;
}

/**
 * wpa_supplicant_disassociate - Disassociate the current connection
 * @wpa_s: Pointer to wpa_supplicant data
 * @reason_code: IEEE 802.11 reason code for the disassociate frame
 *
 * This function is used to request %wpa_supplicant to disassociate with the
 * current AP.
 */
void  wpa_sm_disassociate(struct wpa_sm *sm, int reason_code)
{
    /*check if need clear internal state and data value*/
}
#endif
