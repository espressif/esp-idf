/*
 * WPA Supplicant - Glue code to setup EAPOL and RSN modules
 * Copyright (c) 2003-2008, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#ifndef WPAS_GLUE_H
#define WPAS_GLUE_H

u8 * wpa_sm_alloc_eapol(struct wpa_sm *sm, u8 type,
				      const void *data, u16 data_len,
				      size_t *msg_len, void **data_pos);

int wpa_sm_mlme_setprotection(struct wpa_sm *sm, const u8 *addr,
					    int protect_type, int key_type);

void wpa_sm_deauthenticate(struct wpa_sm *sm, uint8 reason_code);

void wpa_sm_disassociate(struct wpa_sm *sm, int reason_code);

int wpa_sm_get_beacon_ie(struct wpa_sm *sm);

#endif /* WPAS_GLUE_H */
