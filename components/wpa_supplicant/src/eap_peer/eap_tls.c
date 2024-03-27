/*
 * EAP peer method: EAP-TLS (RFC 5216, RFC 9190)
 * Copyright (c) 2004-2008, 2012-2019, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */
#include "utils/includes.h"

#ifdef EAP_TLS
#include "utils/common.h"
#include "crypto/tls.h"
#include "eap_peer/eap_i.h"
#include "eap_peer/eap_defs.h"
#include "eap_peer/eap_tls_common.h"
#include "eap_peer/eap_config.h"
#include "eap_peer/eap_methods.h"


static void eap_tls_deinit(struct eap_sm *sm, void *priv);


struct eap_tls_data {
	struct eap_ssl_data ssl;
	u8 *key_data;
	u8 *session_id;
	size_t id_len;
	void *ssl_ctx;
	u8 eap_type;
	struct wpabuf *pending_resp;
	bool prot_success_received;
};


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


static void eap_tls_free_key(struct eap_tls_data *data)
{
	if (data->key_data) {
		bin_clear_free(data->key_data, EAP_TLS_KEY_LEN + EAP_EMSK_LEN);
		data->key_data = NULL;
	}
}


static void eap_tls_deinit(struct eap_sm *sm, void *priv)
{
	struct eap_tls_data *data = priv;
	if (data == NULL)
		return;
	eap_peer_tls_ssl_deinit(sm, &data->ssl);
	eap_tls_free_key(data);
	os_free(data->session_id);
	wpabuf_free(data->pending_resp);
	os_free(data);
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
	const char *label;
	const u8 eap_tls13_context[] = { EAP_TYPE_TLS };
	const u8 *context = NULL;
	size_t context_len = 0;

	wpa_printf(MSG_DEBUG, "EAP-TLS: Done");

	if (data->ssl.tls_out) {
		wpa_printf(MSG_DEBUG, "EAP-TLS: Fragment(s) remaining");
		return;
	}

	if (data->ssl.tls_v13) {
		label = "EXPORTER_EAP_TLS_Key_Material";
		context = eap_tls13_context;
		context_len = 1;

		/* A possible NewSessionTicket may be received before
		 * EAP-Success, so need to allow it to be received. */
		ret->methodState = METHOD_MAY_CONT;
		ret->decision = DECISION_COND_SUCC;
	} else {
		label = "client EAP encryption";

		ret->methodState = METHOD_DONE;
		ret->decision = DECISION_UNCOND_SUCC;
	}

	eap_tls_free_key(data);
	data->key_data = eap_peer_tls_derive_key(sm, &data->ssl, label,
						 context, context_len,
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

	if (sm->waiting_ext_cert_check && data->pending_resp) {
		struct eap_peer_config *config = eap_get_config(sm);

		if (config->pending_ext_cert_check == EXT_CERT_CHECK_GOOD) {
			wpa_printf(MSG_DEBUG,
				   "EAP-TLS: External certificate check succeeded - continue handshake");
			resp = data->pending_resp;
			data->pending_resp = NULL;
			sm->waiting_ext_cert_check = 0;
			return resp;
		}

		if (config->pending_ext_cert_check == EXT_CERT_CHECK_BAD) {
			wpa_printf(MSG_DEBUG,
				   "EAP-TLS: External certificate check failed - force authentication failure");
			ret->methodState = METHOD_DONE;
			ret->decision = DECISION_FAIL;
			sm->waiting_ext_cert_check = 0;
			return NULL;
		}

		wpa_printf(MSG_DEBUG,
			   "EAP-TLS: Continuing to wait external server certificate validation");
		return NULL;
	}

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

	if (sm->waiting_ext_cert_check) {
		wpa_printf(MSG_DEBUG,
			   "EAP-TLS: Waiting external server certificate validation");
		wpabuf_free(data->pending_resp);
		data->pending_resp = resp;
		return NULL;
	}

	/* RFC 9190 Section 2.5 */
	if (res == 2 && data->ssl.tls_v13 && wpabuf_len(resp) == 1 &&
	    *wpabuf_head_u8(resp) == 0) {
		wpa_printf(MSG_DEBUG,
			   "EAP-TLS: ACKing protected success indication (appl data 0x00)");
		eap_peer_tls_reset_output(&data->ssl);
		res = 1;
		ret->methodState = METHOD_DONE;
		ret->decision = DECISION_UNCOND_SUCC;
		data->prot_success_received = true;
	}

	if (tls_connection_established(data->ssl_ctx, data->ssl.conn) &&
	    (!data->ssl.tls_v13 || data->prot_success_received))
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
