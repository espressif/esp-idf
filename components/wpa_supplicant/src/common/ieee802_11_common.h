/*
 * IEEE 802.11 Common routines
 * Copyright (c) 2002-2019, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef IEEE802_11_COMMON_H
#define IEEE802_11_COMMON_H

#include "defs.h"
#include "ieee802_11_defs.h"
#include "esp_wifi_driver.h"

struct element {
	u8 id;
	u8 datalen;
	u8 data[];
} STRUCT_PACKED;

/* element iteration helpers */
#define for_each_element(_elem, _data, _datalen)                        \
	for (_elem = (const struct element *) (_data);                  \
			(const u8 *) (_data) + (_datalen) - (const u8 *) _elem >=  \
			(int) sizeof(*_elem) &&                                 \
			(const u8 *) (_data) + (_datalen) - (const u8 *) _elem >=  \
			(int) sizeof(*_elem) + _elem->datalen;                  \
			_elem = (const struct element *) (_elem->data + _elem->datalen))

#define for_each_element_id(element, _id, data, datalen)                \
	for_each_element(element, data, datalen)                        \
	if (element->id == (_id))

struct wpa_supplicant;

struct oper_class_map {
        enum hostapd_hw_mode mode;
        u8 op_class;
        u8 min_chan;
        u8 max_chan;
        u8 inc;
        enum { BW20, BW40PLUS, BW40MINUS, BW40, BW80, BW2160, BW160, BW80P80,
               BW320, BW4320, BW6480, BW8640} bw;
        enum { P2P_SUPP, NO_P2P_SUPP } p2p;
};

extern const struct oper_class_map global_op_class[];
extern size_t global_op_class_size;

int ieee802_11_parse_candidate_list(const char *pos, u8 *nei_rep,
				    size_t nei_rep_len);
const u8 * get_ie(const u8 *ies, size_t len, u8 eid);
int ieee802_11_parse_elems(struct wpa_supplicant *wpa_s, const u8 *start, size_t len);
int ieee802_11_ext_capab(const u8 *ie, unsigned int capab);
const u8 * get_vendor_ie(const u8 *ies, size_t len, u32 vendor_type);
size_t mbo_add_ie(u8 *buf, size_t len, const u8 *attr, size_t attr_len);
u8 get_operating_class(u8 chan, int sec_channel);
int ieee802_11_ie_count(const u8 *ies, size_t ies_len);
struct wpabuf * ieee802_11_vendor_ie_concat(const u8 *ies, size_t ies_len,
					    u32 oui_type);
#endif /* IEEE802_11_COMMON_H */
