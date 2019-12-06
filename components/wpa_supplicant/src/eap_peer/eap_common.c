/*
 * EAP common peer/server definitions
 * Copyright (c) 2004-2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "eap_peer/eap_defs.h"
#include "eap_peer/eap_common.h"

/**
 * eap_hdr_len_valid - Validate EAP header length field
 * @msg: EAP frame (starting with EAP header)
 * @min_payload: Minimum payload length needed
 * Returns: 1 for valid header, 0 for invalid
 *
 * This is a helper function that does minimal validation of EAP messages. The
 * length field is verified to be large enough to include the header and not
 * too large to go beyond the end of the buffer.
 */
int eap_hdr_len_valid(const struct wpabuf *msg, size_t min_payload)
{
	const struct eap_hdr *hdr;
	size_t len;

	if (msg == NULL)
		return 0;

	hdr = wpabuf_head(msg);

	if (wpabuf_len(msg) < sizeof(*hdr)) {
		wpa_printf(MSG_INFO, "EAP: Too short EAP frame");
		return 0;
	}

	len = be_to_host16(hdr->length);
	if (len < sizeof(*hdr) + min_payload || len > wpabuf_len(msg)) {
		wpa_printf(MSG_INFO, "EAP: Invalid EAP length");
		return 0;
	}

	return 1;
}


/**
 * eap_hdr_validate - Validate EAP header
 * @vendor: Expected EAP Vendor-Id (0 = IETF)
 * @eap_type: Expected EAP type number
 * @msg: EAP frame (starting with EAP header)
 * @plen: Pointer to variable to contain the returned payload length
 * Returns: Pointer to EAP payload (after type field), or %NULL on failure
 *
 * This is a helper function for EAP method implementations. This is usually
 * called in the beginning of struct eap_method::process() function to verify
 * that the received EAP request packet has a valid header. This function is
 * able to process both legacy and expanded EAP headers and in most cases, the
 * caller can just use the returned payload pointer (into *plen) for processing
 * the payload regardless of whether the packet used the expanded EAP header or
 * not.
 */
const u8 * eap_hdr_validate(int vendor, EapType eap_type,
			    const struct wpabuf *msg, size_t *plen)
{
	const struct eap_hdr *hdr;
	const u8 *pos;
	size_t len;

	if (!eap_hdr_len_valid(msg, 1))
		return NULL;

	hdr = wpabuf_head(msg);
	len = be_to_host16(hdr->length);
	pos = (const u8 *) (hdr + 1);

	if (*pos == EAP_TYPE_EXPANDED) {
		int exp_vendor;
		u32 exp_type;
		if (len < sizeof(*hdr) + 8) {
			wpa_printf(MSG_INFO, "EAP: Invalid expanded EAP "
				   "length");
			return NULL;
		}
		pos++;
		exp_vendor = WPA_GET_BE24(pos);
		pos += 3;
		exp_type = WPA_GET_BE32(pos);
		pos += 4;
		if (exp_vendor != vendor || exp_type != (u32) eap_type) {
			wpa_printf(MSG_INFO, "EAP: Invalid expanded frame "
				   "type");
			return NULL;
		}

		*plen = len - sizeof(*hdr) - 8;
		return pos;
	} else {
		if (vendor != EAP_VENDOR_IETF || *pos != eap_type) {
			wpa_printf(MSG_INFO, "EAP: Invalid frame type");
			return NULL;
		}
		*plen = len - sizeof(*hdr) - 1;
		return pos + 1;
	}
}


/**
 * eap_msg_alloc - Allocate a buffer for an EAP message
 * @vendor: Vendor-Id (0 = IETF)
 * @type: EAP type
 * @payload_len: Payload length in bytes (data after Type)
 * @code: Message Code (EAP_CODE_*)
 * @identifier: Identifier
 * Returns: Pointer to the allocated message buffer or %NULL on error
 *
 * This function can be used to allocate a buffer for an EAP message and fill
 * in the EAP header. This function is automatically using expanded EAP header
 * if the selected Vendor-Id is not IETF. In other words, most EAP methods do
 * not need to separately select which header type to use when using this
 * function to allocate the message buffers. The returned buffer has room for
 * payload_len bytes and has the EAP header and Type field already filled in.
 */
struct wpabuf * eap_msg_alloc(int vendor, EapType type, size_t payload_len,
			      u8 code, u8 identifier)
{
	struct wpabuf *buf;
	struct eap_hdr *hdr;
	size_t len;

	len = sizeof(struct eap_hdr) + (vendor == EAP_VENDOR_IETF ? 1 : 8) +
		payload_len;
	buf = wpabuf_alloc(len);
	if (buf == NULL)
		return NULL;

	hdr = wpabuf_put(buf, sizeof(*hdr));
	hdr->code = code;
	hdr->identifier = identifier;
	hdr->length = host_to_be16(len);

	if (vendor == EAP_VENDOR_IETF) {
		wpabuf_put_u8(buf, type);
	} else {
		wpabuf_put_u8(buf, EAP_TYPE_EXPANDED);
		wpabuf_put_be24(buf, vendor);
		wpabuf_put_be32(buf, type);
	}

	return buf;
}


/**
 * eap_update_len - Update EAP header length
 * @msg: EAP message from eap_msg_alloc
 *
 * This function updates the length field in the EAP header to match with the
 * current length for the buffer. This allows eap_msg_alloc() to be used to
 * allocate a larger buffer than the exact message length (e.g., if exact
 * message length is not yet known).
 */
void eap_update_len(struct wpabuf *msg)
{
	struct eap_hdr *hdr;
	hdr = wpabuf_mhead(msg);
	if (wpabuf_len(msg) < sizeof(*hdr))
		return;
	hdr->length = host_to_be16(wpabuf_len(msg));
}


/**
 * eap_get_id - Get EAP Identifier from wpabuf
 * @msg: Buffer starting with an EAP header
 * Returns: The Identifier field from the EAP header
 */
u8 eap_get_id(const struct wpabuf *msg)
{
	const struct eap_hdr *eap;

	if (wpabuf_len(msg) < sizeof(*eap))
		return 0;

	eap = wpabuf_head(msg);
	return eap->identifier;
}


/**
 * eap_get_id - Get EAP Type from wpabuf
 * @msg: Buffer starting with an EAP header
 * Returns: The EAP Type after the EAP header
 */
EapType eap_get_type(const struct wpabuf *msg)
{
	if (wpabuf_len(msg) < sizeof(struct eap_hdr) + 1)
		return EAP_TYPE_NONE;

	return ((const u8 *) wpabuf_head(msg))[sizeof(struct eap_hdr)];
}
