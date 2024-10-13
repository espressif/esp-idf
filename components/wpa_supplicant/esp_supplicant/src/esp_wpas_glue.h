/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef WPAS_GLUE_H
#define WPAS_GLUE_H

u8 *wpa_sm_alloc_eapol(struct wpa_sm *sm, u8 type,
                       const void *data, u16 data_len,
                       size_t *msg_len, void **data_pos);

int wpa_sm_mlme_setprotection(struct wpa_sm *sm, const u8 *addr,
                              int protect_type, int key_type);

void wpa_sm_deauthenticate(struct wpa_sm *sm, u8 reason_code);

void wpa_sm_disassociate(struct wpa_sm *sm, int reason_code);

int wpa_sm_get_beacon_ie(struct wpa_sm *sm);

void wpa_sm_free_eapol(u8 *buffer);

u8 *wpa_alloc_eapol(void *sm, u8 type,
                    const void *data, u16 data_len,
                    size_t *msg_len, void **data_pos);

void wpa_free_eapol(u8 *buffer);

int wpa_ether_send(void *ctx, const u8 *dest, u16 proto,
                   const u8 *data, size_t data_len);

void wpa_supplicant_transition_disable(struct wpa_sm *sm, u8 bitmap);

int hostapd_send_eapol(const u8 *source, const u8 *sta_addr,
                       const u8 *data, size_t data_len);
#endif /* WPAS_GLUE_H */
