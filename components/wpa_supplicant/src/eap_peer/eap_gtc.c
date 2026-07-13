/*
 * EAP peer method: EAP-GTC (RFC 3748)
 * Copyright (c) 2004-2006, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#ifdef EAP_GTC

#include "utils/common.h"
#include "eap_peer/eap_i.h"
#include "eap_peer/eap_common.h"
#include "eap_peer/eap_config.h"
#include "eap_peer/eap_methods.h"

struct eap_gtc_data {
	int prefix;
};


static void * eap_gtc_init(struct eap_sm *sm)
{
	struct eap_gtc_data *data;
	data = os_zalloc(sizeof(*data));
	if (data == NULL)
		return NULL;

	if (sm->m && sm->m->vendor == EAP_VENDOR_IETF &&
	    sm->m->method == EAP_TYPE_FAST) {
		wpa_printf(MSG_DEBUG, "EAP-GTC: EAP-FAST tunnel - use prefix "
			   "with challenge/response");
		data->prefix = 1;
	}
	return data;
}


static void eap_gtc_deinit(struct eap_sm *sm, void *priv)
{
	struct eap_gtc_data *data = priv;
	os_free(data);
}


static struct wpabuf * eap_gtc_process(struct eap_sm *sm, void *priv,
				       struct eap_method_ret *ret,
				       const struct wpabuf *reqData)
{
	struct eap_gtc_data *data = priv;
	struct wpabuf *resp;
	const u8 *pos, *password, *identity;
	size_t password_len, identity_len, len, plen;
	u8 id;

	pos = eap_hdr_validate(EAP_VENDOR_IETF, EAP_TYPE_GTC, reqData, &len);
	if (pos == NULL) {
		ret->ignore = true;
		return NULL;
	}
	id = eap_get_id(reqData);

	wpa_hexdump_ascii(MSG_MSGDUMP, "EAP-GTC: Request message", pos, len);
	if (data->prefix &&
	    (len < 10 || os_memcmp(pos, "CHALLENGE=", 10) != 0)) {
		wpa_printf(MSG_DEBUG, "EAP-GTC: Challenge did not start with "
			   "expected prefix");

		/* Send an empty response in order to allow tunneled
		 * acknowledgement of the failure. This will also cover the
		 * error case which seems to use EAP-MSCHAPv2 like error
		 * reporting with EAP-GTC inside EAP-FAST tunnel. */
		resp = eap_msg_alloc(EAP_VENDOR_IETF, EAP_TYPE_GTC,
				     0, EAP_CODE_RESPONSE, id);
		return resp;
	}

	password = eap_get_config_password(sm, &password_len);

	if (password == NULL) {
		wpa_printf(MSG_INFO, "EAP-GTC: Password not configured");
		eap_sm_request_password(sm);
		ret->ignore = true;
		return NULL;
	}

	ret->ignore = false;

	ret->methodState = data->prefix ? METHOD_MAY_CONT : METHOD_DONE;
	ret->decision = DECISION_COND_SUCC;
	ret->allowNotifications = false;

	plen = password_len;
	identity = eap_get_config_identity(sm, &identity_len);
	if (identity == NULL)
		return NULL;
	if (data->prefix)
		plen += 9 + identity_len + 1;
	resp = eap_msg_alloc(EAP_VENDOR_IETF, EAP_TYPE_GTC, plen,
			     EAP_CODE_RESPONSE, id);
	if (resp == NULL)
		return NULL;
	if (data->prefix) {
		wpabuf_put_data(resp, "RESPONSE=", 9);
		wpabuf_put_data(resp, identity, identity_len);
		wpabuf_put_u8(resp, '\0');
	}
	wpabuf_put_data(resp, password, password_len);
	wpa_hexdump_ascii_key(MSG_MSGDUMP, "EAP-GTC: Response",
			      wpabuf_head_u8(resp) + sizeof(struct eap_hdr) +
			      1, plen);

	return resp;
}


int eap_peer_gtc_register(void)
{
	struct eap_method *eap;
	int ret;

	eap = eap_peer_method_alloc(EAP_VENDOR_IETF, EAP_TYPE_GTC, "GTC");
	if (eap == NULL)
		return -1;

	eap->init = eap_gtc_init;
	eap->deinit = eap_gtc_deinit;
	eap->process = eap_gtc_process;

	ret = eap_peer_method_register(eap);
	if (ret)
		eap_peer_method_free(eap);
	return ret;
}

#endif /* EAP_GTC */
