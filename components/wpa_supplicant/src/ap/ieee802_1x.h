/*
 * hostapd / IEEE 802.1X-2004 Authenticator
 * Copyright (c) 2002-2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef IEEE802_1X_H
#define IEEE802_1X_H

struct hostapd_data;
struct sta_info;
struct eapol_state_machine;
struct hostapd_config;
struct hostapd_bss_config;
struct hostapd_radius_attr;
struct radius_msg;


void ieee802_1x_receive(struct hostapd_data *hapd, const u8 *sa, const u8 *buf,
			size_t len);

#endif /* IEEE802_1X_H */
