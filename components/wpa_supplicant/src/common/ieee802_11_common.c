/*
 * IEEE 802.11 Common routines
 * Copyright (c) 2002-2019, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "includes.h"

#include "common.h"
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

int ieee802_11_ie_count(const u8 *ies, size_t ies_len)
{
	const struct element *elem;
	int count = 0;

	if (ies == NULL)
		return 0;

	for_each_element(elem, ies, ies_len)
		count++;

	return count;
}



const u8 * get_vendor_ie(const u8 *ies, size_t len, u32 vendor_type)
{
	const struct element *elem;

	for_each_element_id(elem, WLAN_EID_VENDOR_SPECIFIC, ies, len) {
		if (elem->datalen >= 4 &&
				vendor_type == WPA_GET_BE32(elem->data))
			return &elem->id;
	}

	return NULL;
}


size_t mbo_add_ie(u8 *buf, size_t len, const u8 *attr, size_t attr_len)
{
	/*
	 * MBO IE requires 6 bytes without the attributes: EID (1), length (1),
	 * OUI (3), OUI type (1).
	 */
	if (len < 6 + attr_len) {
		wpa_printf(MSG_DEBUG,
			   "MBO: Not enough room in buffer for MBO IE: buf len = %zu, attr_len = %zu",
			   len, attr_len);
		return 0;
	}

	*buf++ = WLAN_EID_VENDOR_SPECIFIC;
	*buf++ = attr_len + 4;
	WPA_PUT_BE24(buf, OUI_WFA);
	buf += 3;
	*buf++ = MBO_OUI_TYPE;
	os_memcpy(buf, attr, attr_len);

	return 6 + attr_len;
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

#ifdef CONFIG_SAE_PK
static int ieee802_11_parse_vendor_specific(struct wpa_supplicant *wpa_s, const struct element *elem, const u8* pos)
{
	u32 oui;
	oui = WPA_GET_BE24(pos);
	switch (oui) {
	case OUI_WFA:
		switch (pos[3]) {
		case SAE_PK_OUI_TYPE:
			wpa_s->sae_pk_elems.sae_pk_len = elem->datalen - 4;
			wpa_s->sae_pk_elems.sae_pk = (u8*)os_zalloc(sizeof(u8)*(elem->datalen-4));
			if (!wpa_s->sae_pk_elems.sae_pk) {
			    wpa_printf(MSG_EXCESSIVE, "Can not allocate memory for sae_pk");
			    return -1;
			}
			os_memcpy(wpa_s->sae_pk_elems.sae_pk, pos+4, elem->datalen-4);
			break;
		default:
			wpa_printf(MSG_EXCESSIVE, "Unknown WFA "
				"information element ignored "
				"(type=%d len=%lu)",
				pos[3], (unsigned long) elem->datalen);
			return -1;
		}
		break;
	default:
		 wpa_printf(MSG_EXCESSIVE, "unknown vendor specific "
			"information element ignored (vendor OUI "
			"%02x:%02x:%02x len=%lu)",
			pos[0], pos[1], pos[2], (unsigned long) elem->datalen);
		return -1;
	}

	return 0;
}

static int ieee802_11_parse_extension(struct wpa_supplicant *wpa_s, const struct element *elem, const u8* pos){
	// do not consider extension_id element len in datalen
	if (elem->datalen < 1) {
		wpa_printf(MSG_DEBUG,
			"short information element (Ext)");
		return -1;
	}
	u8 ext_id;
	ext_id = *pos++;
	switch (ext_id) {
	case WLAN_EID_EXT_FILS_KEY_CONFIRM:
		wpa_s->sae_pk_elems.fils_key_confirm_len = elem->datalen - 1;
		wpa_s->sae_pk_elems.fils_key_confirm = (u8*)os_zalloc(sizeof(u8)*(elem->datalen - 1));
		os_memcpy(wpa_s->sae_pk_elems.fils_key_confirm, pos, elem->datalen - 1);
		break;
	case WLAN_EID_EXT_FILS_PUBLIC_KEY:
		wpa_s->sae_pk_elems.fils_pk_len = elem->datalen - 1;
		wpa_s->sae_pk_elems.fils_pk = (u8*)os_zalloc(sizeof(u8)*(elem->datalen - 1));
		os_memcpy(wpa_s->sae_pk_elems.fils_pk, pos, elem->datalen - 1);
		break;
	default:
		wpa_printf(MSG_EXCESSIVE,
			"IEEE 802.11 element parsing ignored unknown element extension (ext_id=%u elen=%u)",
		   ext_id, (unsigned int) elem->datalen-1);
		return -1;
	}

	return 0;
}
#endif /* CONFIG_SAE_PK */

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
#if defined(CONFIG_RRM) || defined(CONFIG_WNM) || defined(CONFIG_SAE_PK)
	const struct element *elem;
	u8 unknown = 0;

	if (!start)
		return 0;

	for_each_element(elem, start, len) {
		u8 id = elem->id;
		const u8 *pos = elem->data;
		switch (id) {
#ifdef CONFIG_RRM
		case WLAN_EID_RRM_ENABLED_CAPABILITIES:
			os_memcpy(wpa_s->rrm_ie, pos, 5);
			wpa_s->rrm.rrm_used = true;
			break;
#endif
#ifdef CONFIG_SAE_PK
		case WLAN_EID_EXTENSION:
			if(ieee802_11_parse_extension(wpa_s, elem, pos) != 0){
				unknown++;
			}
			break;
		case WLAN_EID_VENDOR_SPECIFIC:
			if(ieee802_11_parse_vendor_specific(wpa_s, elem, pos) != 0){
				unknown++;
			}
			break;
#endif /*CONFIG_SAE_PK*/
#ifdef CONFIG_WNM
		case WLAN_EID_EXT_CAPAB:
			/* extended caps can go beyond 8 octacts but we aren't using them now */
			os_memcpy(wpa_s->extend_caps, pos, 5);
			break;
#endif
		default:
			break;

		}
	}
	if (unknown)
		return -1;

#endif /* defined(CONFIG_RRM) || defined(CONFIG_WNM) || defined(CONFIG_SAE_PK) */
	return 0;
}

struct wpabuf * ieee802_11_vendor_ie_concat(const u8 *ies, size_t ies_len,
					    u32 oui_type)
{
	struct wpabuf *buf;
	const struct element *elem, *found = NULL;

	for_each_element_id(elem, WLAN_EID_VENDOR_SPECIFIC, ies, ies_len) {
		if (elem->datalen >= 4 &&
				WPA_GET_BE32(elem->data) == oui_type) {
			found = elem;
			break;
		}
	}

	if (!found)
		return NULL; /* No specified vendor IE found */

	buf = wpabuf_alloc(ies_len);
	if (buf == NULL)
		return NULL;

	/*
	 * There may be multiple vendor IEs in the message, so need to
	 * concatenate their data fields.
	 */
	for_each_element_id(elem, WLAN_EID_VENDOR_SPECIFIC, ies, ies_len) {
		if (elem->datalen >= 4 && WPA_GET_BE32(elem->data) == oui_type)
			wpabuf_put_data(buf, elem->data + 4, elem->datalen - 4);
	}

	return buf;
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
