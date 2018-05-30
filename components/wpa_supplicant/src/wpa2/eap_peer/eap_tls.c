/*
 * EAP peer method: EAP-TLS (RFC 2716)
 * Copyright (c) 2004-2008, 2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */
#ifdef EAP_TLS

#include "wpa/includes.h"

#include "wpa/common.h"
#include "wpa2/tls/tls.h"
#include "wpa2/eap_peer/eap_i.h"
#include "wpa2/eap_peer/eap_defs.h"
#include "wpa2/eap_peer/eap_tls_common.h"
#include "wpa2/eap_peer/eap_config.h"
#include "wpa2/eap_peer/eap_methods.h"

struct eap_tls_data {
	struct eap_ssl_data ssl;
	u8 *key_data;
	u8 *session_id;
	size_t id_len;
	void *ssl_ctx;
	u8 eap_type;
};



static void eap_tls_deinit(struct eap_sm *sm, void *priv)
{
	struct eap_tls_data *data = priv;
	if (data == NULL)
		return;
	eap_peer_tls_ssl_deinit(sm, &data->ssl);
	os_free(data->key_data);
	os_free(data->session_id);
	os_free(data);
}


static void * eap_tls_init(struct eap_sm *sm)
{
	struct eap_tls_data *data;
	struct eap_peer_config *config = eap_get_config(sm);
	if (config == NULL ||
	    config->private_key == 0) {
		wpa_printf(MSG_INFO, "EAP-TLS: Private key not configured");
		return NULL;
	}

	data = (struct eap_tls_data *)os_zalloc(sizeof(*data));
	if (data == NULL)
		return NULL;

	data->ssl_ctx = sm->ssl_ctx;

	if (eap_peer_tls_ssl_init(sm, &data->ssl, config, EAP_TYPE_TLS)) {
		wpa_printf(MSG_INFO, "EAP-TLS: Failed to initialize SSL.");
		eap_tls_deinit(sm, data);
		return NULL;
	}

	data->eap_type = EAP_TYPE_TLS;

	return data;
}

static struct wpabuf * eap_tls_failure(struct eap_sm *sm,
				       struct eap_tls_data *data,
				       struct eap_method_ret *ret, int res,
				       struct wpabuf *resp, u8 id)
{
	wpa_printf(MSG_DEBUG, "EAP-TLS: TLS processing failed");

	ret->methodState = METHOD_DONE;
	ret->decision = DECISION_FAIL;

	if (res == -1) {
		struct eap_peer_config *config = eap_get_config(sm);
		if (config) {
			/*
			 * The TLS handshake failed. So better forget the old
			 * PIN. It may be wrong, we cannot be sure but trying
			 * the wrong one again might block it on the card--so
			 * better ask the user again.
			 */
			os_free(config->pin);
			config->pin = NULL;
		}
	}

	if (resp) {
		/*
		 * This is likely an alert message, so send it instead of just
		 * ACKing the error.
		 */
		return resp;
	}

	return eap_peer_tls_build_ack(id, data->eap_type, 0);
}


static void eap_tls_success(struct eap_sm *sm, struct eap_tls_data *data,
			    struct eap_method_ret *ret)
{
	wpa_printf(MSG_DEBUG, "EAP-TLS: Done");

	ret->methodState = METHOD_DONE;
	ret->decision = DECISION_UNCOND_SUCC;

	os_free(data->key_data);
	data->key_data = eap_peer_tls_derive_key(sm, &data->ssl,
						 "client EAP encryption",
						 EAP_TLS_KEY_LEN +
						 EAP_EMSK_LEN);
	if (data->key_data) {
		wpa_hexdump_key(MSG_DEBUG, "EAP-TLS: Derived key",
				data->key_data, EAP_TLS_KEY_LEN);
		wpa_hexdump_key(MSG_DEBUG, "EAP-TLS: Derived EMSK",
				data->key_data + EAP_TLS_KEY_LEN,
				EAP_EMSK_LEN);
	} else {
		wpa_printf(MSG_INFO, "EAP-TLS: Failed to derive key");
	}

	os_free(data->session_id);
	data->session_id = eap_peer_tls_derive_session_id(sm, &data->ssl,
							  EAP_TYPE_TLS,
			                                  &data->id_len);
	if (data->session_id) {
		wpa_hexdump(MSG_DEBUG, "EAP-TLS: Derived Session-Id",
			    data->session_id, data->id_len);
	} else {
		wpa_printf(MSG_ERROR, "EAP-TLS: Failed to derive Session-Id");
	}
}


static struct wpabuf * eap_tls_process(struct eap_sm *sm, void *priv,
				       struct eap_method_ret *ret,
				       const struct wpabuf *reqData)
{
	size_t left;
	int res;
	struct wpabuf *resp;
	u8 flags, id;
	const u8 *pos;
	struct eap_tls_data *data = priv;

	pos = eap_peer_tls_process_init(sm, &data->ssl, data->eap_type, ret,
					reqData, &left, &flags);
	if (pos == NULL)
		return NULL;
	id = eap_get_id(reqData);

	if (flags & EAP_TLS_FLAGS_START) {
		wpa_printf(MSG_DEBUG, "EAP-TLS: Start");
		left = 0; /* make sure that this frame is empty, even though it
			   * should always be, anyway */
	}

	resp = NULL;
	res = eap_peer_tls_process_helper(sm, &data->ssl, data->eap_type, 0,
					  id, pos, left, &resp);

	if (res < 0) {
		return eap_tls_failure(sm, data, ret, res, resp, id);
	}

	if (tls_connection_established(data->ssl_ctx, data->ssl.conn))
		eap_tls_success(sm, data, ret);

	if (res == 1) {
		wpabuf_free(resp);
		return eap_peer_tls_build_ack(id, data->eap_type, 0);
	}

	return resp;
}

static bool eap_tls_isKeyAvailable(struct eap_sm *sm, void *priv)
{
	struct eap_tls_data *data = priv;

	return data->key_data != NULL;
}
static u8 * eap_tls_getKey(struct eap_sm *sm, void *priv, size_t *len)
{
	struct eap_tls_data *data = priv;
	u8 *key;

	if (data->key_data == NULL)
		return NULL;

	key = os_malloc(EAP_TLS_KEY_LEN);
	if (key == NULL)
		return NULL;

	*len = EAP_TLS_KEY_LEN;
	os_memcpy(key, data->key_data, EAP_TLS_KEY_LEN);

	return key;
}

int eap_peer_tls_register(void)
{
	struct eap_method *eap;
	int ret;

	eap = eap_peer_method_alloc(EAP_VENDOR_IETF, EAP_TYPE_TLS,
				    "TLS");

	if (eap == NULL)
		return -1;

	eap->init = eap_tls_init;
	eap->deinit = eap_tls_deinit;
	eap->process = eap_tls_process;
	eap->isKeyAvailable = eap_tls_isKeyAvailable;
	eap->getKey = eap_tls_getKey;

	ret = eap_peer_method_register(eap);
	if (ret)
		eap_peer_method_free(eap);
	return ret;
}


#endif /* EAP_TLS */
