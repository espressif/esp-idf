/*
 * hostapd / IEEE 802.11 Management
 * Copyright (c) 2002-2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef IEEE802_11_H
#define IEEE802_11_H

enum wpa_validate_result;
int auth_sae_queued_addr(struct hostapd_data *hapd, const u8 *addr);
int auth_sae_queue(struct hostapd_data *hapd, u8 *buf, size_t len, u8 *bssid, u16 status, u32 auth_transaction);
int handle_auth_sae(struct hostapd_data *hapd, struct sta_info *sta,
                    u8 *buf, size_t len, u8 *bssid,
                    u16 auth_transaction, u16 status);
u16 wpa_res_to_status_code(enum wpa_validate_result res);

#endif /* IEEE802_11_H */
