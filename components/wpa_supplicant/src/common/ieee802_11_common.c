/*
 * IEEE 802.11 Common routines
 * Copyright (c) 2002-2019, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "defs.h"
#include "ieee802_11_defs.h"
#include "ieee802_11_common.h"
#include "common/wpa_supplicant_i.h"

/**
 * get_ie - Fetch a specified information element from IEs buffer
 * @ies: Information elements buffer
 * @len: Information elements buffer length
 * @eid: Information element identifier (WLAN_EID_*)
 * Returns: Pointer to the information element (id field) or %NULL if not found
 *
 * This function returns the first matching information element in the IEs
 * buffer or %NULL in case the element is not found.
 */
const u8 * get_ie(const u8 *ies, size_t len, u8 eid)
{
	const struct element *elem;

	if (!ies)
		return NULL;

	for_each_element_id(elem, eid, ies, len)
		return &elem->id;

	return NULL;
}

int ieee802_11_parse_candidate_list(const char *pos, u8 *nei_rep,
				    size_t nei_rep_len)
{
	u8 *nei_pos = nei_rep;
	const char *end;

	/*
	 * BSS Transition Candidate List Entries - Neighbor Report elements
	 * neighbor=<BSSID>,<BSSID Information>,<Operating Class>,
	 * <Channel Number>,<PHY Type>[,<hexdump of Optional Subelements>]
	 */
	while (pos) {
		u8 *nei_start;
		long int val;
		char *endptr, *tmp;

		pos = os_strstr(pos, " neighbor=");
		if (!pos)
			break;
		if (nei_pos + 15 > nei_rep + nei_rep_len) {
			wpa_printf(MSG_DEBUG,
				   "Not enough room for additional neighbor");
			return -1;
		}
		pos += 10;

		nei_start = nei_pos;
		*nei_pos++ = WLAN_EID_NEIGHBOR_REPORT;
		nei_pos++; /* length to be filled in */

		if (hwaddr_aton2(pos, nei_pos) < 0) {
			wpa_printf(MSG_DEBUG, "Invalid BSSID");
			return -1;
		}
		nei_pos += ETH_ALEN;
		pos += 17;
		if (*pos != ',') {
			wpa_printf(MSG_DEBUG, "Missing BSSID Information");
			return -1;
		}
		pos++;

		val = strtol(pos, &endptr, 0);
		WPA_PUT_LE32(nei_pos, val);
		nei_pos += 4;
		if (*endptr != ',') {
			wpa_printf(MSG_DEBUG, "Missing Operating Class");
			return -1;
		}
		pos = endptr + 1;

		*nei_pos++ = atoi(pos); /* Operating Class */
		pos = os_strchr(pos, ',');
		if (pos == NULL) {
			wpa_printf(MSG_DEBUG, "Missing Channel Number");
			return -1;
		}
		pos++;

		*nei_pos++ = atoi(pos); /* Channel Number */
		pos = os_strchr(pos, ',');
		if (pos == NULL) {
			wpa_printf(MSG_DEBUG, "Missing PHY Type");
			return -1;
		}
		pos++;

		*nei_pos++ = atoi(pos); /* PHY Type */
		end = os_strchr(pos, ' ');
		tmp = os_strchr(pos, ',');
		if (tmp && (!end || tmp < end)) {
			/* Optional Subelements (hexdump) */
			size_t len;

			pos = tmp + 1;
			end = os_strchr(pos, ' ');
			if (end)
				len = end - pos;
			else
				len = os_strlen(pos);
			if (nei_pos + len / 2 > nei_rep + nei_rep_len) {
				wpa_printf(MSG_DEBUG,
					   "Not enough room for neighbor subelements");
				return -1;
			}
			if (len & 0x01 ||
			    hexstr2bin(pos, nei_pos, len / 2) < 0) {
				wpa_printf(MSG_DEBUG,
					   "Invalid neighbor subelement info");
				return -1;
			}
			nei_pos += len / 2;
			pos = end;
		}

		nei_start[1] = nei_pos - nei_start - 2;
	}

	return nei_pos - nei_rep;
}

/**
 * ieee802_11_parse_elems - Parse information elements in management frames
 * @start: Pointer to the start of IEs
 * @len: Length of IE buffer in octets
 * @elems: Data structure for parsed elements
 * @show_errors: Whether to show parsing errors in debug log
 * Returns: Parsing result
 */
int ieee802_11_parse_elems(struct wpa_supplicant *wpa_s, const u8 *start, size_t len)
{
	const struct element *elem;

	if (!start)
		return 0;

	for_each_element(elem, start, len) {
		u8 id = elem->id;
		const u8 *pos = elem->data;

		switch (id) {
		case WLAN_EID_RRM_ENABLED_CAPABILITIES:
			os_memcpy(wpa_s->rrm_ie, pos, 5);
			wpa_s->rrm.rrm_used = true;
			break;
		case WLAN_EID_EXT_CAPAB:
			/* extended caps can go beyond 8 octacts but we aren't using them now */
			os_memcpy(wpa_s->extend_caps, pos, 5);
			break;
		default:
			break;

		}
	}
	return 0;
}

int ieee802_11_ext_capab(const u8 *ie, unsigned int capab)
{
	if (!ie || ie[1] <= capab / 8)
		return 0;
	return !!(ie[2 + capab / 8] & BIT(capab % 8));
}

u8 get_operating_class(u8 chan, int sec_channel)
{
	u8 op_class;

	if (chan < 1 || chan > 14)
		return 0;
	if (sec_channel == 1)
		op_class = 83;
	else if (sec_channel == -1)
		op_class = 84;
	else
		op_class = 81;

	return op_class;
}
