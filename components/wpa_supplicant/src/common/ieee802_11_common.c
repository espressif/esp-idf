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
static int ieee802_11_parse_vendor_specific(const u8 *pos, size_t elen,
					    struct ieee802_11_elems *elems,
					    int show_errors)
{
	u32 oui;

	/* first 3 bytes in vendor specific information element are the IEEE
	 * OUI of the vendor. The following byte is used a vendor specific
	 * sub-type. */
	if (elen < 4) {
		if (show_errors) {
			wpa_printf(MSG_MSGDUMP, "short vendor specific "
				   "information element ignored (len=%lu)",
				   (unsigned long) elen);
		}
		return -1;
	}

	oui = WPA_GET_BE24(pos);
	switch (oui) {
	case OUI_WFA:
		switch (pos[3]) {
		case SAE_PK_OUI_TYPE:
			elems->sae_pk = pos + 4;
			elems->sae_pk_len = elen - 4;
			break;
		default:
			wpa_printf(MSG_EXCESSIVE, "Unknown WFA "
				"information element ignored "
				"(type=%d len=%lu)",
				pos[3], (unsigned long) elen);
			//TODO discuss if to ignore it
			break;
			//return -1;
		}
		break;
	default:
		wpa_printf(MSG_EXCESSIVE, "unknown vendor specific "
			"information element ignored (vendor OUI "
			"%02x:%02x:%02x len=%lu)",
			pos[0], pos[1], pos[2], (unsigned long) elen);
		//TODO discuss if to ignore it
		break;
		//return -1;
	}

	return 0;
}
#endif /* CONFIG_SAE_PK */

#ifdef CONFIG_SAE_PK
static int ieee802_11_parse_extension(const u8 *pos, size_t elen,
				      struct ieee802_11_elems *elems,
				      const u8 *start, size_t len,
				      int show_errors)
{
	u8 ext_id;

	if (elen < 1) {
		wpa_printf(MSG_DEBUG,
			"short information element (Ext)");
		return -1;
	}

	ext_id = *pos++;
	elen--;

	switch (ext_id) {
	case WLAN_EID_EXT_FILS_KEY_CONFIRM:
		elems->fils_key_confirm = pos;
		elems->fils_key_confirm_len = elen;
		break;
	case WLAN_EID_EXT_FILS_PUBLIC_KEY:
		if (elen < 1)
			break;
		elems->fils_pk = pos;
		elems->fils_pk_len = elen;
		break;
	default:
		wpa_printf(MSG_EXCESSIVE,
			"IEEE 802.11 element parsing ignored unknown element extension (ext_id=%u elen=%u)",
		   ext_id, (unsigned int) elen);
		//TODO discuss if to ignore it
		break;
		//return -1;
	}

	return 0;
}
#endif /* CONFIG_SAE_PK */

#if defined(CONFIG_RRM) || defined(CONFIG_WMM) || defined(CONFIG_SAE_PK)
static ParseRes __ieee802_11_parse_elems(const u8 *start, size_t len,
					 struct ieee802_11_elems *elems,
					 int show_errors)
{
	const struct element *elem;
	u8 unknown = 0;

	if (!start)
		return ParseOK;

	for_each_element(elem, start, len) {
		u8 id = elem->id, elen = elem->datalen;
		const u8 *pos = elem->data;
		switch (id) {
#ifdef CONFIG_RRM
		case WLAN_EID_RRM_ENABLED_CAPABILITIES:
			elems->rrm_enabled = pos;
			elems->rrm_enabled_len = elen;
			break;
#endif
#ifdef CONFIG_SAE_PK
		case WLAN_EID_EXTENSION:
			if (ieee802_11_parse_extension(pos, elen, elems, start,
						       len, show_errors) != 0) {
				unknown++;
			}
			break;
		case WLAN_EID_VENDOR_SPECIFIC:
			if (ieee802_11_parse_vendor_specific(pos, elen,
							     elems,
							     show_errors) != 0) {
				unknown++;
			}
			break;
#endif /*CONFIG_SAE_PK*/
#ifdef CONFIG_WNM
		case WLAN_EID_EXT_CAPAB:
			/* extended caps can go beyond 8 octacts but we aren't using them now */
			elems->ext_capab = pos;
			elems->ext_capab_len = elen;
			break;
#endif
		default:
			break;

		}
	}
	if (unknown)
		return ParseFailed;

	return ParseOK;
}
#endif

/**
 * ieee802_11_parse_elems - Parse information elements in management frames
 * @start: Pointer to the start of IEs
 * @len: Length of IE buffer in octets
 * @elems: Data structure for parsed elements
 * @show_errors: Whether to show parsing errors in debug log
 * Returns: Parsing result
 */
#if defined(CONFIG_RRM) || defined(CONFIG_WMM) || defined(CONFIG_SAE_PK)
ParseRes ieee802_11_parse_elems(const u8 *start, size_t len, struct ieee802_11_elems *elems, int show_errors)
{
	os_memset(elems, 0, sizeof(*elems));

	return __ieee802_11_parse_elems(start, len, elems, show_errors);
}
#endif

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

const struct oper_class_map global_op_class[] = {
        { HOSTAPD_MODE_IEEE80211G, 81, 1, 13, 1, BW20, P2P_SUPP },
        { HOSTAPD_MODE_IEEE80211G, 82, 14, 14, 1, BW20, NO_P2P_SUPP },

        { HOSTAPD_MODE_IEEE80211G, 83, 1, 9, 1, BW40PLUS, NO_P2P_SUPP },
        { HOSTAPD_MODE_IEEE80211G, 84, 5, 13, 1, BW40MINUS, NO_P2P_SUPP },

        { HOSTAPD_MODE_IEEE80211A, 115, 36, 48, 4, BW20, P2P_SUPP },
        { HOSTAPD_MODE_IEEE80211A, 116, 36, 44, 8, BW40PLUS, P2P_SUPP },
        { HOSTAPD_MODE_IEEE80211A, 117, 40, 48, 8, BW40MINUS, P2P_SUPP },
        { HOSTAPD_MODE_IEEE80211A, 118, 52, 64, 4, BW20, NO_P2P_SUPP },
        { HOSTAPD_MODE_IEEE80211A, 119, 52, 60, 8, BW40PLUS, NO_P2P_SUPP },
        { HOSTAPD_MODE_IEEE80211A, 120, 56, 64, 8, BW40MINUS, NO_P2P_SUPP },
        { HOSTAPD_MODE_IEEE80211A, 121, 100, 144, 4, BW20, NO_P2P_SUPP },
        { HOSTAPD_MODE_IEEE80211A, 122, 100, 140, 8, BW40PLUS, NO_P2P_SUPP },
        { HOSTAPD_MODE_IEEE80211A, 123, 104, 144, 8, BW40MINUS, NO_P2P_SUPP },
        { HOSTAPD_MODE_IEEE80211A, 124, 149, 161, 4, BW20, P2P_SUPP },
        { HOSTAPD_MODE_IEEE80211A, 125, 149, 177, 4, BW20, P2P_SUPP },
        { HOSTAPD_MODE_IEEE80211A, 126, 149, 173, 8, BW40PLUS, P2P_SUPP },
        { HOSTAPD_MODE_IEEE80211A, 127, 153, 177, 8, BW40MINUS, P2P_SUPP }
};

size_t global_op_class_size = ARRAY_SIZE(global_op_class);

bool ieee802_11_rsnx_capab_len(const u8 *rsnxe, size_t rsnxe_len,
                              unsigned int capab)
{
       const u8 *end;
       size_t flen, i;
       u32 capabs = 0;

       if (!rsnxe || rsnxe_len == 0)
               return false;
       end = rsnxe + rsnxe_len;
       flen = (rsnxe[0] & 0x0f) + 1;
       if (rsnxe + flen > end)
               return false;
       if (flen > 4)
               flen = 4;
       for (i = 0; i < flen; i++)
           capabs |= (u32) rsnxe[i] << (8 * i);

       return capabs & BIT(capab);
}


bool ieee802_11_rsnx_capab(const u8 *rsnxe, unsigned int capab)
{
       if (!rsnxe)
               return false;
       if (rsnxe[0] == WLAN_EID_VENDOR_SPECIFIC && rsnxe[1] >= 4 + 1)
               return ieee802_11_rsnx_capab_len(rsnxe + 2 + 4, rsnxe[1] - 4,
                                                capab);
       return ieee802_11_rsnx_capab_len(rsnxe + 2, rsnxe[1], capab);
}

u8 get_operating_class(u8 chan, int sec_channel)
{
	u8 op_class = 0;

	if (chan >= 1 && chan < 14) {
		if (sec_channel == 1)
			op_class = 83;
		else if (sec_channel == -1)
			op_class = 84;
		else
			op_class = 81;
	}

	if (chan == 14)
		op_class = 82;

#if CONFIG_SOC_WIFI_SUPPORT_5G
	if (chan >= 36 && chan <= 48) {
		if (sec_channel == 1)
			op_class = 116;
		else if (sec_channel == -1)
			op_class = 117;
		else
			op_class = 115;
	}
	if (chan >= 52 && chan <= 64) {
		if (sec_channel == 1)
			op_class = 119;
		else if (sec_channel == -1)
			op_class = 120;
		else
			op_class = 118;
	}
	if (chan >= 149 && chan <= 177) {
		if (sec_channel == 1)
			op_class = 126;
		else if (sec_channel == -1)
			op_class = 127;
		else
			op_class = 125;
	}
	if (chan >= 100 && chan <= 144) {
		if (sec_channel == 1)
			op_class = 122;
		else if (sec_channel == -1)
			op_class = 123;
		else
			op_class = 121;
	}
#endif

	return op_class;
}
