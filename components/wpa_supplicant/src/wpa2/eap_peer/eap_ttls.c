/*
 * EAP peer method: EAP-TTLS (RFC 5281)
 * Copyright (c) 2004-2011, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifdef EAP_TTLS

#include "wpa/includes.h"

#include "wpa/common.h"
#include "crypto/ms_funcs.h"
#include "crypto/sha1.h"
#include "wpa2/tls/tls.h"
//#include "eap_common/chap.h"
#include "wpa2/eap_peer/eap.h"
#include "wpa2/eap_peer/eap_ttls.h"
#include "wpa2/eap_peer/mschapv2.h"
//#include "wpa2/eap_peer/chap.h"
#include "wpa2/eap_peer/eap_i.h"
#include "wpa2/eap_peer/eap_tls_common.h"
#include "wpa2/eap_peer/eap_config.h"
#include "wpa2/eap_peer/eap_methods.h"


#define EAP_TTLS_VERSION 0

static void eap_ttls_deinit(struct eap_sm *sm, void *priv);

struct eap_ttls_data {
	struct eap_ssl_data ssl;

	int ttls_version;

	const struct eap_method *phase2_method;
	void *phase2_priv;
	int phase2_success;
	int phase2_start;

	enum phase2_types {
		EAP_TTLS_PHASE2_EAP,
		EAP_TTLS_PHASE2_MSCHAPV2,
		EAP_TTLS_PHASE2_MSCHAP,
		EAP_TTLS_PHASE2_PAP,
		EAP_TTLS_PHASE2_CHAP
	} phase2_type;
	struct eap_method_type phase2_eap_type;
	struct eap_method_type *phase2_eap_types;
	size_t num_phase2_eap_types;

	u8 auth_response[MSCHAPV2_AUTH_RESPONSE_LEN];
	int auth_response_valid;
	u8 master_key[MSCHAPV2_MASTER_KEY_LEN]; /* MSCHAPv2 master key */
	u8 ident;
	int resuming; /* starting a resumed session */
	int reauth; /* reauthentication */
	u8 *key_data;
	u8 *session_id;
	size_t id_len;

	struct wpabuf *pending_phase2_req;

#ifdef EAP_TNC
	int ready_for_tnc;
	int tnc_started;
#endif /* EAP_TNC */
};


static void * eap_ttls_init(struct eap_sm *sm)
{
	struct eap_ttls_data *data;
	struct eap_peer_config *config = eap_get_config(sm);
	//char *selected;

	data = (struct eap_ttls_data *)os_zalloc(sizeof(*data));
	if (data == NULL)
		return NULL;
	data->ttls_version = EAP_TTLS_VERSION;
	data->phase2_type = EAP_TTLS_PHASE2_MSCHAPV2;
	
/*	
    selected = "MSCHAPV2";
	//TODO: Now only support EAP-TTLS/MSCHAPV2
	if (config && config->phase2) {
		if (os_strstr(config->phase2, "autheap=")) {
			selected = "EAP";
			data->phase2_type = EAP_TTLS_PHASE2_EAP;
		} else if (os_strstr(config->phase2, "auth=MSCHAPV2")) {
			selected = "MSCHAPV2";
			data->phase2_type = EAP_TTLS_PHASE2_MSCHAPV2;
		} else if (os_strstr(config->phase2, "auth=MSCHAP")) {
			selected = "MSCHAP";
			data->phase2_type = EAP_TTLS_PHASE2_MSCHAP;
		} else if (os_strstr(config->phase2, "auth=PAP")) {
			selected = "PAP";
			data->phase2_type = EAP_TTLS_PHASE2_PAP;
		} else if (os_strstr(config->phase2, "auth=CHAP")) {
			selected = "CHAP";
			data->phase2_type = EAP_TTLS_PHASE2_CHAP;
		}
	}
    wpa_printf(MSG_DEBUG, "EAP-TTLS: Phase2 type: %s", selected);

	if (data->phase2_type == EAP_TTLS_PHASE2_EAP) {
		if (eap_peer_select_phase2_methods(config, "autheap=",
						   &data->phase2_eap_types,
						   &data->num_phase2_eap_types)
		    < 0) {
			eap_ttls_deinit(sm, data);
			return NULL;
		}

		data->phase2_eap_type.vendor = EAP_VENDOR_IETF;
		data->phase2_eap_type.method = EAP_TYPE_NONE;
	}
*/

	if (eap_peer_tls_ssl_init(sm, &data->ssl, config, EAP_TYPE_TTLS)) {
		wpa_printf(MSG_ERROR, "EAP-TTLS: Failed to initialize SSL.\n");
		eap_ttls_deinit(sm, data);
		return NULL;
	}

	return data;
}

static void eap_ttls_phase2_eap_deinit(struct eap_sm *sm,
			   struct eap_ttls_data *data)
{
	if (data->phase2_priv && data->phase2_method) {
		data->phase2_method->deinit(sm, data->phase2_priv);
		data->phase2_method = NULL;
		data->phase2_priv = NULL;
	}
}

static void eap_ttls_deinit(struct eap_sm *sm, void *priv)
{
	struct eap_ttls_data *data = priv;
	if (data == NULL)
		return;
	eap_ttls_phase2_eap_deinit(sm, data);
	os_free(data->phase2_eap_types);
	eap_peer_tls_ssl_deinit(sm, &data->ssl);
	os_free(data->key_data);
	os_free(data->session_id);
	wpabuf_free(data->pending_phase2_req);
	os_free(data);
}

static u8 * eap_ttls_avp_hdr(u8 *avphdr, u32 avp_code, u32 vendor_id,
		 int mandatory, size_t len)
{
	struct ttls_avp_vendor *avp;
	u8 flags;
	size_t hdrlen;

	avp = (struct ttls_avp_vendor *) avphdr;
	flags = mandatory ? AVP_FLAGS_MANDATORY : 0;
	if (vendor_id) {
		flags |= AVP_FLAGS_VENDOR;
		hdrlen = sizeof(*avp);
		avp->vendor_id = host_to_be32(vendor_id);
	} else {
		hdrlen = sizeof(struct ttls_avp);
	}

	avp->avp_code = host_to_be32(avp_code);
	avp->avp_length = host_to_be32((flags << 24) | (u32) (hdrlen + len));

	return avphdr + hdrlen;
}


static u8 * eap_ttls_avp_add(u8 *start, u8 *avphdr, u32 avp_code,
		 u32 vendor_id, int mandatory,
		 const u8 *data, size_t len)
{
	u8 *pos;
	pos = eap_ttls_avp_hdr(avphdr, avp_code, vendor_id, mandatory, len);
	os_memcpy(pos, data, len);
	pos += len;
	AVP_PAD(start, pos);
	return pos;
}

#if 0
static int eap_ttls_avp_encapsulate(struct wpabuf **resp, u32 avp_code,
			 int mandatory)
{
	struct wpabuf *msg;
	u8 *avp, *pos;

	msg = wpabuf_alloc(sizeof(struct ttls_avp) + wpabuf_len(*resp) + 4);
	if (msg == NULL) {
		wpabuf_free(*resp);
		*resp = NULL;
		return -1;
	}

	avp = wpabuf_mhead(msg);
	pos = eap_ttls_avp_hdr(avp, avp_code, 0, mandatory, wpabuf_len(*resp));
	os_memcpy(pos, wpabuf_head(*resp), wpabuf_len(*resp));
	pos += wpabuf_len(*resp);
	AVP_PAD(avp, pos);
	wpabuf_free(*resp);
	wpabuf_put(msg, pos - avp);
	*resp = msg;
	return 0;
}
#endif

static int eap_ttls_v0_derive_key(struct eap_sm *sm,
		       struct eap_ttls_data *data)
{
	os_free(data->key_data);
	data->key_data = eap_peer_tls_derive_key(sm, &data->ssl,
						 "ttls keying material",
						 EAP_TLS_KEY_LEN);
	if (!data->key_data) {
		wpa_printf(MSG_ERROR, "EAP-TTLS: Failed to derive key\n");
		return -1;
	}

	os_free(data->session_id);
	data->session_id = eap_peer_tls_derive_session_id(sm, &data->ssl,
							  EAP_TYPE_TTLS,
	                                                  &data->id_len);
	if (data->session_id) {
	} else {
		wpa_printf(MSG_ERROR, "EAP-TTLS: Failed to derive Session-Id\n");
	}

	return 0;
}


static u8 * eap_ttls_implicit_challenge(struct eap_sm *sm,
			    struct eap_ttls_data *data, size_t len)
{
	return eap_peer_tls_derive_key(sm, &data->ssl, "ttls challenge", len);
}

#if 0
static void eap_ttls_phase2_select_eap_method(struct eap_ttls_data *data,
					      u8 method)
{
	size_t i;
	for (i = 0; i < data->num_phase2_eap_types; i++) {
		if (data->phase2_eap_types[i].vendor != EAP_VENDOR_IETF ||
		    data->phase2_eap_types[i].method != method)
			continue;

		data->phase2_eap_type.vendor =
			data->phase2_eap_types[i].vendor;
		data->phase2_eap_type.method =
			data->phase2_eap_types[i].method;
		wpa_printf(MSG_DEBUG, "EAP-TTLS: Selected "
			  "Phase 2 EAP vendor %d method %d\n",
			  data->phase2_eap_type.vendor,
			  data->phase2_eap_type.method);
		break;
	}
}

static int eap_ttls_phase2_eap_process(struct eap_sm *sm,
				       struct eap_ttls_data *data,
				       struct eap_method_ret *ret,
				       struct eap_hdr *hdr, size_t len,
				       struct wpabuf **resp)
{
	struct wpabuf msg;
	struct eap_method_ret iret;

	os_memset(&iret, 0, sizeof(iret));
	wpabuf_set(&msg, hdr, len);
	*resp = data->phase2_method->process(sm, data->phase2_priv, &iret,
					     &msg);
	if ((iret.methodState == METHOD_DONE ||
	     iret.methodState == METHOD_MAY_CONT) &&
	    (iret.decision == DECISION_UNCOND_SUCC ||
	     iret.decision == DECISION_COND_SUCC ||
	     iret.decision == DECISION_FAIL)) {
		ret->methodState = iret.methodState;
		ret->decision = iret.decision;
	}

	return 0;
}


static int eap_ttls_phase2_request_eap_method(struct eap_sm *sm,
					      struct eap_ttls_data *data,
					      struct eap_method_ret *ret,
					      struct eap_hdr *hdr, size_t len,
					      u8 method, struct wpabuf **resp)
{
#ifdef EAP_TNC
	if (data->tnc_started && data->phase2_method &&
	    data->phase2_priv && method == EAP_TYPE_TNC &&
	    data->phase2_eap_type.method == EAP_TYPE_TNC)
		return eap_ttls_phase2_eap_process(sm, data, ret, hdr, len,
						   resp);

	if (data->ready_for_tnc && !data->tnc_started &&
	    method == EAP_TYPE_TNC) {
		wpa_printf(MSG_DEBUG, "EAP-TTLS: Start TNC after completed "
			  "EAP method\n");
		data->tnc_started = 1;
	}

	if (data->tnc_started) {
		if (data->phase2_eap_type.vendor != EAP_VENDOR_IETF ||
		    data->phase2_eap_type.method == EAP_TYPE_TNC) {
			wpa_printf(MSG_ERROR, "EAP-TTLS: Unexpected EAP "
				  "type %d for TNC\n", method);
			return -1;
		}

		data->phase2_eap_type.vendor = EAP_VENDOR_IETF;
		data->phase2_eap_type.method = method;
		wpa_printf(MSG_DEBUG, "EAP-TTLS: Selected "
			  "Phase 2 EAP vendor %d method %d (TNC)\n",
			  data->phase2_eap_type.vendor,
			   data->phase2_eap_type.method);

		if (data->phase2_type == EAP_TTLS_PHASE2_EAP)
			eap_ttls_phase2_eap_deinit(sm, data);
	}
#endif /* EAP_TNC */

	if (data->phase2_eap_type.vendor == EAP_VENDOR_IETF &&
	    data->phase2_eap_type.method == EAP_TYPE_NONE)
		eap_ttls_phase2_select_eap_method(data, method);

	if (method != data->phase2_eap_type.method || method == EAP_TYPE_NONE)
	{
		return -1;
		if (eap_peer_tls_phase2_nak(data->phase2_eap_types,
					    data->num_phase2_eap_types,
					    hdr, resp))
			return -1;
		return 0;

	}

	if (data->phase2_priv == NULL) {
		data->phase2_method = eap_peer_get_eap_method(
			EAP_VENDOR_IETF, method);
		if (data->phase2_method) {
			sm->init_phase2 = 1;
			data->phase2_priv = data->phase2_method->init(sm);
			sm->init_phase2 = 0;
		}
	}
	if (data->phase2_priv == NULL || data->phase2_method == NULL) {
		wpa_printf(MSG_ERROR, "EAP-TTLS: failed to initialize "
			  "Phase 2 EAP method %d\n", method);
		return -1;
	}

	return eap_ttls_phase2_eap_process(sm, data, ret, hdr, len, resp);
}

#if 0
static int eap_ttls_phase2_request_eap(struct eap_sm *sm,
				       struct eap_ttls_data *data,
				       struct eap_method_ret *ret,
				       struct eap_hdr *hdr,
				       struct wpabuf **resp)
{
	size_t len = be_to_host16(hdr->length);
	u8 *pos;
	struct eap_peer_config *config = eap_get_config(sm);

	if (len <= sizeof(struct eap_hdr)) {
		wpa_printf(MSG_ERROR, "EAP-TTLS: too short "
			  "Phase 2 request (len=%lu)\n", (unsigned long) len);
		return -1;
	}
	pos = (u8 *) (hdr + 1);
	wpa_printf(MSG_DEBUG, "EAP-TTLS: Phase 2 EAP Request: type=%d\n", *pos);
	switch (*pos) {
	case EAP_TYPE_IDENTITY:
		*resp = eap_sm_build_identity_resp(sm, hdr->identifier, 1);
		break;
	default:
		if (eap_ttls_phase2_request_eap_method(sm, data, ret, hdr, len,
						       *pos, resp) < 0)
			return -1;
		break;
	}

	if (*resp == NULL &&
	    (config->pending_req_identity || config->pending_req_password ||
	     config->pending_req_otp)) {
		return 0;
	}

	if (*resp == NULL)
		return -1;

	return eap_ttls_avp_encapsulate(resp, RADIUS_ATTR_EAP_MESSAGE, 1);
}
#endif
#endif

static int eap_ttls_phase2_request_mschapv2(struct eap_sm *sm,
				 struct eap_ttls_data *data,
				 struct eap_method_ret *ret,
				 struct wpabuf **resp)
{
#ifdef EAP_MSCHAPv2
	struct wpabuf *msg;
	u8 *buf, *pos, *challenge, *peer_challenge;
	const u8 *identity, *password;
	size_t identity_len, password_len;
	int pwhash;

	wpa_printf(MSG_DEBUG, "EAP-TTLS: Phase 2 MSCHAPV2 Request\n");

	identity = eap_get_config_identity(sm, &identity_len);
	password = eap_get_config_password2(sm, &password_len, &pwhash);
	if (identity == NULL || password == NULL)
		return -1;

	msg = wpabuf_alloc(identity_len + 1000);
	if (msg == NULL) {
		wpa_printf(MSG_ERROR, "EAP-TTLS/MSCHAPV2: Failed to allocate memory\n");
		return -1;
	}
	pos = buf = wpabuf_mhead(msg);

	/* User-Name */
	pos = eap_ttls_avp_add(buf, pos, RADIUS_ATTR_USER_NAME, 0, 1,
			       identity, identity_len);

	/* MS-CHAP-Challenge */
	challenge = eap_ttls_implicit_challenge(
		sm, data, EAP_TTLS_MSCHAPV2_CHALLENGE_LEN + 1);
	if (challenge == NULL) {
		wpabuf_free(msg);
		wpa_printf(MSG_ERROR, "EAP-TTLS/MSCHAPV2: Failed to derive "
			  "implicit challenge\n");
		return -1;
	}

	pos = eap_ttls_avp_add(buf, pos, RADIUS_ATTR_MS_CHAP_CHALLENGE,
			       RADIUS_VENDOR_ID_MICROSOFT, 1,
			       challenge, EAP_TTLS_MSCHAPV2_CHALLENGE_LEN);

	/* MS-CHAP2-Response */
	pos = eap_ttls_avp_hdr(pos, RADIUS_ATTR_MS_CHAP2_RESPONSE,
			       RADIUS_VENDOR_ID_MICROSOFT, 1,
			       EAP_TTLS_MSCHAPV2_RESPONSE_LEN);
	data->ident = challenge[EAP_TTLS_MSCHAPV2_CHALLENGE_LEN];
	*pos++ = data->ident;
	*pos++ = 0; /* Flags */
	if (os_get_random(pos, EAP_TTLS_MSCHAPV2_CHALLENGE_LEN) < 0) {
		os_free(challenge);
		wpabuf_free(msg);
		wpa_printf(MSG_ERROR, "EAP-TTLS/MSCHAPV2: Failed to get "
			  "random data for peer challenge\n");
		return -1;
	}
	peer_challenge = pos;
	pos += EAP_TTLS_MSCHAPV2_CHALLENGE_LEN;
	os_memset(pos, 0, 8); /* Reserved, must be zero */
	pos += 8;
	if (mschapv2_derive_response(identity, identity_len, password,
				     password_len, pwhash, challenge,
				     peer_challenge, pos, data->auth_response,
				     data->master_key)) {
		os_free(challenge);
		wpabuf_free(msg);
		wpa_printf(MSG_ERROR, "EAP-TTLS/MSCHAPV2: Failed to derive "
			  "response\n");
		return -1;
	}
	data->auth_response_valid = 1;

	pos += 24;
	os_free(challenge);
	AVP_PAD(buf, pos);

	wpabuf_put(msg, pos - buf);
	*resp = msg;

	if (sm->workaround) {
		/* At least FreeRADIUS seems to be terminating
		 * EAP-TTLS/MSHCAPV2 without the expected MS-CHAP-v2 Success
		 * packet. */
		wpa_printf(MSG_DEBUG, "EAP-TTLS/MSCHAPV2: EAP workaround - "
			  "allow success without tunneled response\n");
		ret->methodState = METHOD_MAY_CONT;
		ret->decision = DECISION_COND_SUCC;
	}

	return 0;
#else /* EAP_MSCHAPv2 */
    printf("[Debug] Set EEEEE \n");
	wpa_printf(MSG_ERROR, "EAP-TTLS: MSCHAPv2 not included in the build\n");
	return -1;
#endif /* EAP_MSCHAPv2 */
}

#if 0
//only support MSCHAPv2
static int eap_ttls_phase2_request_mschap(struct eap_sm *sm,
					  struct eap_ttls_data *data,
					  struct eap_method_ret *ret,
					  struct wpabuf **resp)
{
	struct wpabuf *msg;
	u8 *buf, *pos, *challenge;
	const u8 *identity, *password;
	size_t identity_len, password_len;
	int pwhash;

	wpa_printf(MSG_DEBUG, "EAP-TTLS: Phase 2 MSCHAP Request\n");

	identity = eap_get_config_identity(sm, &identity_len);
	password = eap_get_config_password2(sm, &password_len, &pwhash);
	if (identity == NULL || password == NULL)
		return -1;

	msg = wpabuf_alloc(identity_len + 1000);
	if (msg == NULL) {
		wpa_printf(MSG_ERROR, "EAP-TTLS/MSCHAP: Failed to allocate memory\n");
		return -1;
	}
	pos = buf = wpabuf_mhead(msg);

	// User-Name
	pos = eap_ttls_avp_add(buf, pos, RADIUS_ATTR_USER_NAME, 0, 1,
			       identity, identity_len);

	// MS-CHAP-Challenge
	challenge = eap_ttls_implicit_challenge(
		sm, data, EAP_TTLS_MSCHAP_CHALLENGE_LEN + 1);
	if (challenge == NULL) {
		wpabuf_free(msg);
		wpa_printf(MSG_ERROR, "EAP-TTLS/MSCHAP: Failed to derive "
			  "implicit challenge\n");
		return -1;
	}

	pos = eap_ttls_avp_add(buf, pos, RADIUS_ATTR_MS_CHAP_CHALLENGE,
			       RADIUS_VENDOR_ID_MICROSOFT, 1,
			       challenge, EAP_TTLS_MSCHAP_CHALLENGE_LEN);

	// MS-CHAP-Response
	pos = eap_ttls_avp_hdr(pos, RADIUS_ATTR_MS_CHAP_RESPONSE,
			       RADIUS_VENDOR_ID_MICROSOFT, 1,
			       EAP_TTLS_MSCHAP_RESPONSE_LEN);
	data->ident = challenge[EAP_TTLS_MSCHAP_CHALLENGE_LEN];
	*pos++ = data->ident;
	*pos++ = 1; // Flags: Use NT style passwords
	os_memset(pos, 0, 24); // LM-Response
	pos += 24;
	if (pwhash) {
		challenge_response(challenge, password, pos); // NT-Response
	} else {
		nt_challenge_response(challenge, password, password_len,
				      pos); // NT-Response
	}
	pos += 24;
	os_free(challenge);
	AVP_PAD(buf, pos);

	wpabuf_put(msg, pos - buf);
	*resp = msg;

	// EAP-TTLS/MSCHAP does not provide tunneled success
	// notification, so assume that Phase2 succeeds.
	ret->methodState = METHOD_DONE;
	ret->decision = DECISION_COND_SUCC;

	return 0;
}


static int eap_ttls_phase2_request_pap(struct eap_sm *sm,
				       struct eap_ttls_data *data,
				       struct eap_method_ret *ret,
				       struct wpabuf **resp)
{
	struct wpabuf *msg;
	u8 *buf, *pos;
	size_t pad;
	const u8 *identity, *password;
	size_t identity_len, password_len;

	wpa_printf(MSG_DEBUG, "EAP-TTLS: Phase 2 PAP Request\n");

	identity = eap_get_config_identity(sm, &identity_len);
	password = eap_get_config_password(sm, &password_len);
	if (identity == NULL || password == NULL)
		return -1;

	msg = wpabuf_alloc(identity_len + password_len + 100);
	if (msg == NULL) {
		wpa_printf(MSG_ERROR, "EAP-TTLS/PAP: Failed to allocate memory\n");
		return -1;
	}
	pos = buf = wpabuf_mhead(msg);

	// User-Name
	pos = eap_ttls_avp_add(buf, pos, RADIUS_ATTR_USER_NAME, 0, 1,
			       identity, identity_len);

	// User-Password; in RADIUS, this is encrypted, but EAP-TTLS encrypts
	// the data, so no separate encryption is used in the AVP itself.
	// However, the password is padded to obfuscate its length.
	pad = password_len == 0 ? 16 : (16 - (password_len & 15)) & 15;
	pos = eap_ttls_avp_hdr(pos, RADIUS_ATTR_USER_PASSWORD, 0, 1,
			       password_len + pad);
	os_memcpy(pos, password, password_len);
	pos += password_len;
	os_memset(pos, 0, pad);
	pos += pad;
	AVP_PAD(buf, pos);

	wpabuf_put(msg, pos - buf);
	*resp = msg;

	// EAP-TTLS/PAP does not provide tunneled success notification,
	// so assume that Phase2 succeeds.
	ret->methodState = METHOD_DONE;
	ret->decision = DECISION_COND_SUCC;

	return 0;
}


static int eap_ttls_phase2_request_chap(struct eap_sm *sm,
					struct eap_ttls_data *data,
					struct eap_method_ret *ret,
					struct wpabuf **resp)
{
	struct wpabuf *msg;
	u8 *buf, *pos, *challenge;
	const u8 *identity, *password;
	size_t identity_len, password_len;

	wpa_printf(MSG_DEBUG, "EAP-TTLS: Phase 2 CHAP Request\n");

	identity = eap_get_config_identity(sm, &identity_len);
	password = eap_get_config_password(sm, &password_len);
	if (identity == NULL || password == NULL)
		return -1;

	msg = wpabuf_alloc(identity_len + 1000);
	if (msg == NULL) {
		wpa_printf(MSG_ERROR, "EAP-TTLS/CHAP: Failed to allocate memory\n");
		return -1;
	}
	pos = buf = wpabuf_mhead(msg);

	// User-Name
	pos = eap_ttls_avp_add(buf, pos, RADIUS_ATTR_USER_NAME, 0, 1,
			       identity, identity_len);

	// CHAP-Challenge
	challenge = eap_ttls_implicit_challenge(
		sm, data, EAP_TTLS_CHAP_CHALLENGE_LEN + 1);
	if (challenge == NULL) {
		wpabuf_free(msg);
		wpa_printf(MSG_ERROR, "EAP-TTLS/CHAP: Failed to derive "
			  "implicit challenge\n");
		return -1;
	}

	pos = eap_ttls_avp_add(buf, pos, RADIUS_ATTR_CHAP_CHALLENGE, 0, 1,
			       challenge, EAP_TTLS_CHAP_CHALLENGE_LEN);

	// CHAP-Password
	pos = eap_ttls_avp_hdr(pos, RADIUS_ATTR_CHAP_PASSWORD, 0, 1,
			       1 + EAP_TTLS_CHAP_PASSWORD_LEN);
	data->ident = challenge[EAP_TTLS_CHAP_CHALLENGE_LEN];
	*pos++ = data->ident;

	// MD5(Ident + Password + Challenge)
	chap_md5(data->ident, password, password_len, challenge,
		 EAP_TTLS_CHAP_CHALLENGE_LEN, pos);

	pos += EAP_TTLS_CHAP_PASSWORD_LEN;
	os_free(challenge);
	AVP_PAD(buf, pos);

	wpabuf_put(msg, pos - buf);
	*resp = msg;

	// EAP-TTLS/CHAP does not provide tunneled success
	// notification, so assume that Phase2 succeeds.
	ret->methodState = METHOD_DONE;
	ret->decision = DECISION_COND_SUCC;

	return 0;
}
#endif

static int eap_ttls_phase2_request(struct eap_sm *sm,
			struct eap_ttls_data *data,
			struct eap_method_ret *ret,
			struct eap_hdr *hdr,
			struct wpabuf **resp)
{
	int res = 0;
	size_t len;
	enum phase2_types phase2_type = data->phase2_type;

#ifdef EAP_TNC
	if (data->tnc_started) {
		printf("[debug] set phase2_type \n");
		wpa_printf(MSG_DEBUG, "EAP-TTLS: Processing TNC\n");
		phase2_type = EAP_TTLS_PHASE2_EAP;
	}
#endif /* EAP_TNC */

	if (phase2_type == EAP_TTLS_PHASE2_MSCHAPV2) {
		if (eap_get_config_identity(sm, &len) == NULL) {
			wpa_printf(MSG_ERROR, "EAP-TTLS: Identity not configured\n");
			printf("[Debug] Return because no identity  EAP_TTLS_PHASE2_MSCHAPV2\n");
			return 0;
		}
		if (eap_get_config_password(sm, &len) == NULL) {
			wpa_printf(MSG_ERROR, "EAP-TTLS: Password not configured\n");
			printf("[Debug] Return because no password  EAP_TTLS_PHASE2_MSCHAPV2\n");
			return 0;
		}

		res = eap_ttls_phase2_request_mschapv2(sm, data, ret, resp);
	} else {
		wpa_printf(MSG_ERROR, "EAP-TTLS: Phase 2 - Unknown type %d\n", phase2_type);
		res = -1;
	}

/*	if (phase2_type == EAP_TTLS_PHASE2_MSCHAPV2 ||
	    phase2_type == EAP_TTLS_PHASE2_MSCHAP ||
	    phase2_type == EAP_TTLS_PHASE2_PAP ||
	    phase2_type == EAP_TTLS_PHASE2_CHAP) {
		if (eap_get_config_identity(sm, &len) == NULL) {
			wpa_printf(MSG_ERROR, "EAP-TTLS: Identity not configured\n");
			//eap_sm_request_identity(sm);
			if (eap_get_config_password(sm, &len) == NULL);
			//	eap_sm_request_password(sm);
            printf("[Debug] Return because no identity  EAP_TTLS_PHASE2_MSCHAPV2 EAP_TTLS_PHASE2_MSCHAP\n");
			return 0;
		}

		if (eap_get_config_password(sm, &len) == NULL) {
			wpa_printf(MSG_ERROR, "EAP-TTLS: Password not configured\n"); 
			printf("[Debug] Return because no password  EAP_TTLS_PHASE2_MSCHAPV2 EAP_TTLS_PHASE2_MSCHAP\n");
			//eap_sm_request_password(sm);
			return 0;
		}
	}

	switch (phase2_type) {
	case EAP_TTLS_PHASE2_EAP:
	    printf("[Debug] EAP_TTLS_PHASE2_EAP typed \n");
		res = eap_ttls_phase2_request_eap(sm, data, ret, hdr, resp);
		break;
	case EAP_TTLS_PHASE2_MSCHAPV2:
	    printf("[Debug] EAP_TTLS_PHASE2_MSCHAPV2 typed \n");
		res = eap_ttls_phase2_request_mschapv2(sm, data, ret, resp);
		break;
	case EAP_TTLS_PHASE2_MSCHAP:
	    printf("[Debug] EAP_TTLS_PHASE2_MSCHAP typed \n");
		res = eap_ttls_phase2_request_mschap(sm, data, ret, resp);
		break;
	case EAP_TTLS_PHASE2_PAP:
	    printf("[Debug] EAP_TTLS_PHASE2_PAP typed \n");
		res = eap_ttls_phase2_request_pap(sm, data, ret, resp);
		break;
	case EAP_TTLS_PHASE2_CHAP:
	    printf("[Debug] EAP_TTLS_PHASE2_CHAP typed \n");
		res = eap_ttls_phase2_request_chap(sm, data, ret, resp);
		break;
	default:
	    printf("[Debug] Default typed \n");
		wpa_printf(MSG_ERROR, "EAP-TTLS: Phase 2 - Unknown\n");
		res = -1;
		break;
	}*/

	if (res < 0) {
		ret->methodState = METHOD_DONE;
		ret->decision = DECISION_FAIL;
	}

	return res;
}


struct ttls_parse_avp {
	u8 *mschapv2;
	u8 *eapdata;
	size_t eap_len;
	int mschapv2_error;
};


static int eap_ttls_parse_attr_eap(const u8 *dpos, size_t dlen,
			struct ttls_parse_avp *parse)
{
	wpa_printf(MSG_DEBUG, "EAP-TTLS: AVP - EAP Message\n");
	if (parse->eapdata == NULL) {
		parse->eapdata = os_malloc(dlen);
		if (parse->eapdata == NULL) {
			wpa_printf(MSG_ERROR, "EAP-TTLS: Failed to allocate "
				  "memory for Phase 2 EAP data\n");
			return -1;
		}
		os_memcpy(parse->eapdata, dpos, dlen);
		parse->eap_len = dlen;
	} else {
		u8 *neweap = (u8 *)os_realloc(parse->eapdata, parse->eap_len + dlen);
		if (neweap == NULL) {
			wpa_printf(MSG_ERROR, "EAP-TTLS: Failed to allocate "
				  "memory for Phase 2 EAP data\n");
			return -1;
		}
		os_memcpy(neweap + parse->eap_len, dpos, dlen);
		parse->eapdata = neweap;
		parse->eap_len += dlen;
	}

	return 0;
}


static int eap_ttls_parse_avp(u8 *pos, size_t left,
		   struct ttls_parse_avp *parse)
{
	struct ttls_avp *avp;
	u32 avp_code, avp_length, vendor_id = 0;
	u8 avp_flags, *dpos;
	size_t dlen;

	avp = (struct ttls_avp *) pos;
	avp_code = be_to_host32(avp->avp_code);
	avp_length = be_to_host32(avp->avp_length);
	avp_flags = (avp_length >> 24) & 0xff;
	avp_length &= 0xffffff;
	wpa_printf(MSG_DEBUG, "EAP-TTLS: AVP: code=%d flags=0x%02x "
		  "length=%d\n", (int) avp_code, avp_flags,
		  (int) avp_length);

	if (avp_length > left) {
		wpa_printf(MSG_ERROR, "EAP-TTLS: AVP overflow "
			  "(len=%d, left=%lu) - dropped\n",
			  (int) avp_length, (unsigned long) left);
		return -1;
	}

	if (avp_length < sizeof(*avp)) {
		wpa_printf(MSG_ERROR, "EAP-TTLS: Invalid AVP length %d\n",
			  avp_length);
		return -1;
	}

	dpos = (u8 *) (avp + 1);
	dlen = avp_length - sizeof(*avp);
	if (avp_flags & AVP_FLAGS_VENDOR) {
		if (dlen < 4) {
			wpa_printf(MSG_ERROR, "EAP-TTLS: Vendor AVP underflow\n");
			return -1;
		}
		vendor_id = WPA_GET_BE32(dpos);
		wpa_printf(MSG_DEBUG, "EAP-TTLS: AVP vendor_id %d\n",
			  (int) vendor_id);
		dpos += 4;
		dlen -= 4;
	}

	if (vendor_id == 0 && avp_code == RADIUS_ATTR_EAP_MESSAGE) {
		if (eap_ttls_parse_attr_eap(dpos, dlen, parse) < 0)
			return -1;
	} else if (vendor_id == 0 && avp_code == RADIUS_ATTR_REPLY_MESSAGE) {
		/* This is an optional message that can be displayed to
		 * the user. */
	} else if (vendor_id == RADIUS_VENDOR_ID_MICROSOFT &&
		   avp_code == RADIUS_ATTR_MS_CHAP2_SUCCESS) {
		if (dlen != 43) {
			wpa_printf(MSG_ERROR, "EAP-TTLS: Unexpected "
				  "MS-CHAP2-Success length "
				  "(len=%lu, expected 43)\n",
				  (unsigned long) dlen);
			return -1;
		}
		parse->mschapv2 = dpos;
	} else if (vendor_id == RADIUS_VENDOR_ID_MICROSOFT &&
		   avp_code == RADIUS_ATTR_MS_CHAP_ERROR) {
		parse->mschapv2_error = 1;
	} else if (avp_flags & AVP_FLAGS_MANDATORY) {
		wpa_printf(MSG_ERROR, "EAP-TTLS: Unsupported mandatory AVP "
			  "code %d vendor_id %d - dropped\n",
			  (int) avp_code, (int) vendor_id);
		return -1;
	} else {
		wpa_printf(MSG_INFO, "EAP-TTLS: Ignoring unsupported AVP "
			  "code %d vendor_id %d\n",
			  (int) avp_code, (int) vendor_id);
	}

	return avp_length;
}


static int eap_ttls_parse_avps(struct wpabuf *in_decrypted,
		    struct ttls_parse_avp *parse)
{
	u8 *pos;
	size_t left, pad;
	int avp_length;

	pos = wpabuf_mhead(in_decrypted);
	left = wpabuf_len(in_decrypted);
	if (left < sizeof(struct ttls_avp)) {
		wpa_printf(MSG_ERROR, "EAP-TTLS: Too short Phase 2 AVP frame"
			  " len=%lu expected %lu or more - dropped\n",
			  (unsigned long) left,
			  (unsigned long) sizeof(struct ttls_avp));
		return -1;
	}

	/* Parse AVPs */
	os_memset(parse, 0, sizeof(*parse));

	while (left > 0) {
		avp_length = eap_ttls_parse_avp(pos, left, parse);
		if (avp_length < 0)
			return -1;

		pad = (4 - (avp_length & 3)) & 3;
		pos += avp_length + pad;
		if (left < avp_length + pad)
			left = 0;
		else
			left -= avp_length + pad;
	}

	return 0;
}


static u8 * eap_ttls_fake_identity_request(void)
{
	struct eap_hdr *hdr;
	u8 *buf;

	wpa_printf(MSG_DEBUG, "EAP-TTLS: empty data in beginning of "
		  "Phase 2 - use fake EAP-Request Identity\n");
	buf = os_malloc(sizeof(*hdr) + 1);
	if (buf == NULL) {
		wpa_printf(MSG_ERROR, "EAP-TTLS: failed to allocate "
			  "memory for fake EAP-Identity Request\n");
		return NULL;
	}

	hdr = (struct eap_hdr *) buf;
	hdr->code = EAP_CODE_REQUEST;
	hdr->identifier = 0;
	hdr->length = host_to_be16(sizeof(*hdr) + 1);
	buf[sizeof(*hdr)] = EAP_TYPE_IDENTITY;

	return buf;
}


static int eap_ttls_encrypt_response(struct eap_sm *sm,
			  struct eap_ttls_data *data,
			  struct wpabuf *resp, u8 identifier,
			  struct wpabuf **out_data)
{
	if (resp == NULL)
		return 0;

	if (eap_peer_tls_encrypt(sm, &data->ssl, EAP_TYPE_TTLS,
				 data->ttls_version, identifier,
				 resp, out_data)) {
		wpa_printf(MSG_ERROR, "EAP-TTLS: Failed to encrypt a Phase 2 frame\n");
		return -1;
	}
	wpabuf_free(resp);

	return 0;
}

#if 0
static int eap_ttls_process_phase2_eap(struct eap_sm *sm,
				       struct eap_ttls_data *data,
				       struct eap_method_ret *ret,
				       struct ttls_parse_avp *parse,
				       struct wpabuf **resp)
{
	struct eap_hdr *hdr;
	size_t len;

	if (parse->eapdata == NULL) {
		wpa_printf(MSG_ERROR, "EAP-TTLS: No EAP Message in the "
			  "packet - dropped\n");
		return -1;
	}

	hdr = (struct eap_hdr *) parse->eapdata;

	if (parse->eap_len < sizeof(*hdr)) {
		wpa_printf(MSG_ERROR, "EAP-TTLS: Too short Phase 2 EAP "
			  "frame (len=%lu, expected %lu or more) - dropped\n",
			  (unsigned long) parse->eap_len,
			  (unsigned long) sizeof(*hdr));
		return -1;
	}
	len = be_to_host16(hdr->length);
	if (len > parse->eap_len) {
		wpa_printf(MSG_ERROR, "EAP-TTLS: Length mismatch in Phase 2 "
			  "EAP frame (EAP hdr len=%lu, EAP data len in "
			  "AVP=%lu)\n",
			  (unsigned long) len,
			  (unsigned long) parse->eap_len);
		return -1;
	}
	wpa_printf(MSG_DEBUG, "EAP-TTLS: received Phase 2: code=%d "
		  "identifier=%d length=%lu\n",
		  hdr->code, hdr->identifier, (unsigned long) len);
	switch (hdr->code) {
	case EAP_CODE_REQUEST:
		if (eap_ttls_phase2_request(sm, data, ret, hdr, resp)) {
			wpa_printf(MSG_ERROR, "EAP-TTLS: Phase2 Request "
				  "processing failed\n");
			return -1;
		}
		break;
	default:
		wpa_printf(MSG_ERROR, "EAP-TTLS: Unexpected code=%d in "
			  "Phase 2 EAP header\n", hdr->code);
		return -1;
	}

	return 0;
}
#endif

static int eap_ttls_process_phase2_mschapv2(struct eap_sm *sm,
				 struct eap_ttls_data *data,
				 struct eap_method_ret *ret,
				 struct ttls_parse_avp *parse)
{
#ifdef EAP_MSCHAPv2
	if (parse->mschapv2_error) {
		wpa_printf(MSG_ERROR, "EAP-TTLS/MSCHAPV2: Received "
			  "MS-CHAP-Error - failed\n");
		ret->methodState = METHOD_DONE;
		ret->decision = DECISION_FAIL;
		/* Reply with empty data to ACK error */
		return 1;
	}

	if (parse->mschapv2 == NULL) {
#ifdef EAP_TNC
		if (data->phase2_success && parse->eapdata) {
			/*
			 * Allow EAP-TNC to be started after successfully
			 * completed MSCHAPV2.
			 */
			return 1;
		}
#endif /* EAP_TNC */
		wpa_printf(MSG_ERROR, "EAP-TTLS: no MS-CHAP2-Success AVP "
			  "received for Phase2 MSCHAPV2\n");
		return -1;
	}
	if (parse->mschapv2[0] != data->ident) {
		wpa_printf(MSG_ERROR, "EAP-TTLS: Ident mismatch for Phase 2 "
			  "MSCHAPV2 (received Ident 0x%02x, expected 0x%02x)\n",
			  parse->mschapv2[0], data->ident);
		return -1;
	}
	if (!data->auth_response_valid ||
	    mschapv2_verify_auth_response(data->auth_response,
					  parse->mschapv2 + 1, 42)) {
		wpa_printf(MSG_ERROR, "EAP-TTLS: Invalid authenticator "
			  "response in Phase 2 MSCHAPV2 success request\n");
		return -1;
	}

	wpa_printf(MSG_DEBUG, "EAP-TTLS: Phase 2 MSCHAPV2 "
		  "authentication succeeded\n");
	ret->methodState = METHOD_DONE;
	ret->decision = DECISION_UNCOND_SUCC;
	data->phase2_success = 1;

	/*
	 * Reply with empty data; authentication server will reply
	 * with EAP-Success after this.
	 */
	return 1;
#else /* EAP_MSCHAPv2 */
	wpa_printf(MSG_ERROR, "EAP-TTLS: MSCHAPv2 not included in the build\n");
	return -1;
#endif /* EAP_MSCHAPv2 */
}


#ifdef EAP_TNC
static int eap_ttls_process_tnc_start(struct eap_sm *sm,
				      struct eap_ttls_data *data,
				      struct eap_method_ret *ret,
				      struct ttls_parse_avp *parse,
				      struct wpabuf **resp)
{
	/* TNC uses inner EAP method after non-EAP TTLS phase 2. */
	if (parse->eapdata == NULL) {
		wpa_printf(MSG_ERROR, "EAP-TTLS: Phase 2 received "
			  "unexpected tunneled data (no EAP)\n");
		return -1;
	}

	if (!data->ready_for_tnc) {
		wpa_printf(MSG_ERROR, "EAP-TTLS: Phase 2 received "
			  "EAP after non-EAP, but not ready for TNC\n");
		return -1;
	}

	wpa_printf(MSG_DEBUG, "EAP-TTLS: Start TNC after completed "
		  "non-EAP method\n");
	data->tnc_started = 1;

	if (eap_ttls_process_phase2_eap(sm, data, ret, parse, resp) < 0)
		return -1;

	return 0;
}
#endif /* EAP_TNC */


static int eap_ttls_process_decrypted(struct eap_sm *sm,
			   struct eap_ttls_data *data,
			   struct eap_method_ret *ret,
			   u8 identifier,
			   struct ttls_parse_avp *parse,
			   struct wpabuf *in_decrypted,
			   struct wpabuf **out_data)
{
	struct wpabuf *resp = NULL;
	//struct eap_peer_config *config = eap_get_config(sm);
	int res;
	enum phase2_types phase2_type = data->phase2_type;

#ifdef EAP_TNC
	if (data->tnc_started)
		phase2_type = EAP_TTLS_PHASE2_EAP;
#endif /* EAP_TNC */

	switch (phase2_type) {
	case EAP_TTLS_PHASE2_MSCHAPV2:
		res = eap_ttls_process_phase2_mschapv2(sm, data, ret, parse);
#ifdef EAP_TNC
		if (res == 1 && parse->eapdata && data->phase2_success) {
			/*
			 * TNC may be required as the next
			 * authentication method within the tunnel.
			 */
			ret->methodState = METHOD_MAY_CONT;
			data->ready_for_tnc = 1;
			if (eap_ttls_process_tnc_start(sm, data, ret, parse,
						       &resp) == 0)
				break;
		}
#endif /* EAP_TNC */
		return res;
/*	case EAP_TTLS_PHASE2_EAP:
		if (eap_ttls_process_phase2_eap(sm, data, ret, parse, &resp) <
		    0)
			return -1;
		break;
	case EAP_TTLS_PHASE2_MSCHAP:
	case EAP_TTLS_PHASE2_PAP:
	case EAP_TTLS_PHASE2_CHAP:
#ifdef EAP_TNC
		if (eap_ttls_process_tnc_start(sm, data, ret, parse, &resp) <
		    0)
			return -1;
		break;
#else // EAP_TNC
		// EAP-TTLS/{MSCHAP,PAP,CHAP} should not send any TLS tunneled
		// requests to the supplicant
		wpa_printf(MSG_ERROR, "EAP-TTLS: Phase 2 received unexpected "
			  "tunneled data\n");
		return -1;
#endif // EAP_TNC
*/
	default:
		return -1;
	}

	if (resp) {
		if (eap_ttls_encrypt_response(sm, data, resp, identifier,
					      out_data) < 0)
			return -1;
	} else {
		wpabuf_free(data->pending_phase2_req);
		data->pending_phase2_req = wpabuf_dup(in_decrypted);
	}/* else if (config->pending_req_identity ||
		   config->pending_req_password ||
		   config->pending_req_otp ||
		   config->pending_req_new_password) {
		wpabuf_free(data->pending_phase2_req);
		data->pending_phase2_req = wpabuf_dup(in_decrypted);
	}*/

	return 0;
}


static int eap_ttls_implicit_identity_request(struct eap_sm *sm,
				   struct eap_ttls_data *data,
				   struct eap_method_ret *ret,
				   u8 identifier,
				   struct wpabuf **out_data)
{
	int retval = 0;
	struct eap_hdr *hdr;
	struct wpabuf *resp;

	hdr = (struct eap_hdr *) eap_ttls_fake_identity_request();
	if (hdr == NULL) {
		ret->methodState = METHOD_DONE;
		ret->decision = DECISION_FAIL;
		return -1;
	}

	resp = NULL;
	if (eap_ttls_phase2_request(sm, data, ret, hdr, &resp)) {
		wpa_printf(MSG_ERROR, "EAP-TTLS: Phase2 Request "
			  "processing failed\n");
		retval = -1;
	} else {
		//struct eap_peer_config *config = eap_get_config(sm);
		if (resp == NULL) {/* &&
		    (config->pending_req_identity ||
		     config->pending_req_password ||
		     config->pending_req_otp ||
		     config->pending_req_new_password)) {*/
			/*
			 * Use empty buffer to force implicit request
			 * processing when EAP request is re-processed after
			 * user input.
			 */
			wpabuf_free(data->pending_phase2_req);
			data->pending_phase2_req = wpabuf_alloc(0);
		}

		retval = eap_ttls_encrypt_response(sm, data, resp, identifier,
						   out_data);
	}

	os_free(hdr);

	if (retval < 0) {
		ret->methodState = METHOD_DONE;
		ret->decision = DECISION_FAIL;
	}

	return retval;
}


static int eap_ttls_phase2_start(struct eap_sm *sm, struct eap_ttls_data *data,
		      struct eap_method_ret *ret, u8 identifier,
		      struct wpabuf **out_data)
{
	data->phase2_start = 0;

	/*
	 * EAP-TTLS does not use Phase2 on fast re-auth; this must be done only
	 * if TLS part was indeed resuming a previous session. Most
	 * Authentication Servers terminate EAP-TTLS before reaching this
	 * point, but some do not. Make wpa_supplicant stop phase 2 here, if
	 * needed.
	 */
	if (data->reauth &&
	    tls_connection_resumed(sm->ssl_ctx, data->ssl.conn)) {
		wpa_printf(MSG_DEBUG, "EAP-TTLS: Session resumption - "
			  "skip phase 2\n");
		*out_data = eap_peer_tls_build_ack(identifier, EAP_TYPE_TTLS,
						   data->ttls_version);
		ret->methodState = METHOD_DONE;
		ret->decision = DECISION_UNCOND_SUCC;
		data->phase2_success = 1;
		return 0;
	}

	return eap_ttls_implicit_identity_request(sm, data, ret, identifier,
						  out_data);
}


static int eap_ttls_decrypt(struct eap_sm *sm, struct eap_ttls_data *data,
		 struct eap_method_ret *ret, u8 identifier,
		 const struct wpabuf *in_data,
		 struct wpabuf **out_data)
{
	struct wpabuf *in_decrypted = NULL;
	int retval = 0;
	struct ttls_parse_avp parse;

	os_memset(&parse, 0, sizeof(parse));

	wpa_printf(MSG_DEBUG, "EAP-TTLS: received %lu bytes encrypted data for"
		  " Phase 2\n",
		  in_data ? (unsigned long) wpabuf_len(in_data) : 0);

	if (data->pending_phase2_req) {
		wpa_printf(MSG_DEBUG, "EAP-TTLS: Pending Phase 2 request - "
			  "skip decryption and use old data\n");
		/* Clear TLS reassembly state. */
		eap_peer_tls_reset_input(&data->ssl);

		in_decrypted = data->pending_phase2_req;
		data->pending_phase2_req = NULL;
		if (wpabuf_len(in_decrypted) == 0) {
			wpabuf_free(in_decrypted);
			return eap_ttls_implicit_identity_request(
				sm, data, ret, identifier, out_data);
		}
		goto continue_req;
	}

	if ((in_data == NULL || wpabuf_len(in_data) == 0) &&
	    data->phase2_start) {
		return eap_ttls_phase2_start(sm, data, ret, identifier,
					     out_data);
	}

	if (in_data == NULL || wpabuf_len(in_data) == 0) {
		/* Received TLS ACK - requesting more fragments */
		return eap_peer_tls_encrypt(sm, &data->ssl, EAP_TYPE_TTLS,
					    data->ttls_version,
					    identifier, NULL, out_data);
	}

	retval = eap_peer_tls_decrypt(sm, &data->ssl, in_data, &in_decrypted);
	if (retval)
		goto done;

continue_req:
	data->phase2_start = 0;

	if (eap_ttls_parse_avps(in_decrypted, &parse) < 0) {
		retval = -1;
		goto done;
	}

	retval = eap_ttls_process_decrypted(sm, data, ret, identifier,
					    &parse, in_decrypted, out_data);

done:
	wpabuf_free(in_decrypted);
	os_free(parse.eapdata);

	if (retval < 0) {
		ret->methodState = METHOD_DONE;
		ret->decision = DECISION_FAIL;
	}

	return retval;
}


static int eap_ttls_process_handshake(struct eap_sm *sm,
			   struct eap_ttls_data *data,
			   struct eap_method_ret *ret,
			   u8 identifier,
			   const u8 *in_data, size_t in_len,
			   struct wpabuf **out_data)
{
	int res;

	res = eap_peer_tls_process_helper(sm, &data->ssl, EAP_TYPE_TTLS,
					  data->ttls_version, identifier,
					  in_data, in_len, out_data);

	if (tls_connection_established(sm->ssl_ctx, data->ssl.conn)) {
		wpa_printf(MSG_DEBUG, "EAP-TTLS: TLS done, proceed to "
			  "Phase 2\n");
		if (data->resuming) {
			wpa_printf(MSG_DEBUG, "EAP-TTLS: fast reauth - may "
				  "skip Phase 2\n");
			ret->decision = DECISION_COND_SUCC;
			ret->methodState = METHOD_MAY_CONT;
		}
		data->phase2_start = 1;
		eap_ttls_v0_derive_key(sm, data);

		if (*out_data == NULL || wpabuf_len(*out_data) == 0) {
			if (eap_ttls_decrypt(sm, data, ret, identifier,
					     NULL, out_data)) {
				wpa_printf(MSG_ERROR, "EAP-TTLS: "
					  "failed to process early "
					  "start for Phase 2\n");
			}
			res = 0;
		}
		data->resuming = 0;
	}

	if (res == 2) {
		struct wpabuf msg;
		/*
		 * Application data included in the handshake message.
		 */
		wpabuf_free(data->pending_phase2_req);
		data->pending_phase2_req = *out_data;
		*out_data = NULL;
		wpabuf_set(&msg, in_data, in_len);
		res = eap_ttls_decrypt(sm, data, ret, identifier, &msg,
				       out_data);
	}

	return res;
}


static void eap_ttls_check_auth_status(struct eap_sm *sm, 
			   struct eap_ttls_data *data,
			   struct eap_method_ret *ret)
{
	if (ret->methodState == METHOD_DONE) {
		ret->allowNotifications = FALSE;
		if (ret->decision == DECISION_UNCOND_SUCC ||
		    ret->decision == DECISION_COND_SUCC) {
			wpa_printf(MSG_DEBUG, "EAP-TTLS: Authentication "
				  "completed successfully");
			data->phase2_success = 1;
#ifdef EAP_TNC
			if (!data->ready_for_tnc && !data->tnc_started) {
				/*
				 * TNC may be required as the next
				 * authentication method within the tunnel.
				 */
				ret->methodState = METHOD_MAY_CONT;
				data->ready_for_tnc = 1;
			}
#endif /* EAP_TNC */
		}
	} else if (ret->methodState == METHOD_MAY_CONT &&
		   (ret->decision == DECISION_UNCOND_SUCC ||
		    ret->decision == DECISION_COND_SUCC)) {
			wpa_printf(MSG_DEBUG, "EAP-TTLS: Authentication "
				  "completed successfully (MAY_CONT)\n");
			data->phase2_success = 1;
	}
}


static struct wpabuf * eap_ttls_process(struct eap_sm *sm, void *priv,
		 struct eap_method_ret *ret,
		 const struct wpabuf *reqData)
{
	size_t left;
	int res;
	u8 flags, id;
	struct wpabuf *resp;
	const u8 *pos;
	struct eap_ttls_data *data = priv;

	pos = eap_peer_tls_process_init(sm, &data->ssl, EAP_TYPE_TTLS, ret,
					reqData, &left, &flags);
	if (pos == NULL)
		return NULL;
	id = eap_get_id(reqData);

	if (flags & EAP_TLS_FLAGS_START) {
		wpa_printf(MSG_DEBUG, "EAP-TTLS: Start (server ver=%d, own "
			  "ver=%d)\n", flags & EAP_TLS_VERSION_MASK,
			  data->ttls_version);

		/* RFC 5281, Ch. 9.2:
		 * "This packet MAY contain additional information in the form
		 * of AVPs, which may provide useful hints to the client"
		 * For now, ignore any potential extra data.
		 */
		left = 0;
	}

	resp = NULL;
	if (tls_connection_established(sm->ssl_ctx, data->ssl.conn) &&
	    !data->resuming) {
		struct wpabuf msg;
		wpabuf_set(&msg, pos, left);
		res = eap_ttls_decrypt(sm, data, ret, id, &msg, &resp);
	} else {
		res = eap_ttls_process_handshake(sm, data, ret, id,
						 pos, left, &resp);
	}

	eap_ttls_check_auth_status(sm, data, ret);
	/* FIX: what about res == -1? Could just move all error processing into
	 * the other functions and get rid of this res==1 case here. */
	if (res == 1) {
		wpabuf_free(resp);
		return eap_peer_tls_build_ack(id, EAP_TYPE_TTLS,
					      data->ttls_version);
	}

	return resp;
}


static bool eap_ttls_has_reauth_data(struct eap_sm *sm, void *priv)
{
	struct eap_ttls_data *data = priv;
	return tls_connection_established(sm->ssl_ctx, data->ssl.conn) &&
		data->phase2_success;
}


static void eap_ttls_deinit_for_reauth(struct eap_sm *sm, void *priv)
{
	struct eap_ttls_data *data = priv;
	wpabuf_free(data->pending_phase2_req);
	data->pending_phase2_req = NULL;
#ifdef EAP_TNC
	data->ready_for_tnc = 0;
	data->tnc_started = 0;
#endif /* EAP_TNC */
}


static void * eap_ttls_init_for_reauth(struct eap_sm *sm, void *priv)
{
	struct eap_ttls_data *data = priv;
	os_free(data->key_data);
	data->key_data = NULL;
	os_free(data->session_id);
	data->session_id = NULL;
	if (eap_peer_tls_reauth_init(sm, &data->ssl)) {
		os_free(data);
		return NULL;
	}
	if (data->phase2_priv && data->phase2_method &&
	    data->phase2_method->init_for_reauth)
		data->phase2_method->init_for_reauth(sm, data->phase2_priv);
	data->phase2_start = 0;
	data->phase2_success = 0;
	data->resuming = 1;
	data->reauth = 1;
	return priv;
}


static int eap_ttls_get_status(struct eap_sm *sm, void *priv, char *buf,
		    size_t buflen, int verbose)
{
	struct eap_ttls_data *data = priv;
	int len, ret;

	len = eap_peer_tls_status(sm, &data->ssl, buf, buflen, verbose);
	ret = snprintf(buf + len, buflen - len,
			  "EAP-TTLSv%d Phase2 method=",
			  data->ttls_version);
	if (ret < 0 || (size_t) ret >= buflen - len)
		return len;
	len += ret;
	switch (data->phase2_type) {
	/*case EAP_TTLS_PHASE2_EAP:
		ret = os_snprintf(buf + len, buflen - len, "EAP-%s\n",
				  data->phase2_method ?
				  data->phase2_method->name : "?");
		break;*/
	case EAP_TTLS_PHASE2_MSCHAPV2:
		ret = snprintf(buf + len, buflen - len, "MSCHAPV2\n");
		break;
	/*case EAP_TTLS_PHASE2_MSCHAP:
		ret = os_snprintf(buf + len, buflen - len, "MSCHAP\n");
		break;
	case EAP_TTLS_PHASE2_PAP:
		ret = os_snprintf(buf + len, buflen - len, "PAP\n");
		break;
	case EAP_TTLS_PHASE2_CHAP:
		ret = os_snprintf(buf + len, buflen - len, "CHAP\n");
		break;*/
	default:
		ret = 0;
		break;
	}
	if (ret < 0 || (size_t) ret >= buflen - len)
		return len;
	len += ret;

	return len;
}


static bool eap_ttls_isKeyAvailable(struct eap_sm *sm, void *priv)
{
	struct eap_ttls_data *data = priv;
	return data->key_data != NULL && data->phase2_success;
}


static u8 * eap_ttls_getKey(struct eap_sm *sm, void *priv, size_t *len)
{
	struct eap_ttls_data *data = priv;
	u8 *key;

	if (data->key_data == NULL || !data->phase2_success)
		return NULL;

	key = os_malloc(EAP_TLS_KEY_LEN);
	if (key == NULL)
		return NULL;

	*len = EAP_TLS_KEY_LEN;
	os_memcpy(key, data->key_data, EAP_TLS_KEY_LEN);

	return key;
}


static u8 * eap_ttls_get_session_id(struct eap_sm *sm, void *priv, size_t *len)
{
	struct eap_ttls_data *data = priv;
	u8 *id;

	if (data->session_id == NULL || !data->phase2_success)
		return NULL;

	id = os_malloc(data->id_len);
	if (id == NULL)
		return NULL;

	*len = data->id_len;
	os_memcpy(id, data->session_id, data->id_len);

	return id;
}

int eap_peer_ttls_register(void)
{
	struct eap_method *eap;
	int ret;

	eap = eap_peer_method_alloc(EAP_VENDOR_IETF, EAP_TYPE_TTLS,
				    "TTLS");
	if (eap == NULL)
		return -1;

	eap->init = eap_ttls_init;
	eap->deinit = eap_ttls_deinit;
	eap->process = eap_ttls_process;
	eap->isKeyAvailable = eap_ttls_isKeyAvailable;
	eap->getKey = eap_ttls_getKey;
	eap->getSessionId = eap_ttls_get_session_id;
	eap->get_status = eap_ttls_get_status;
	eap->has_reauth_data = eap_ttls_has_reauth_data;
	eap->deinit_for_reauth = eap_ttls_deinit_for_reauth;
	eap->init_for_reauth = eap_ttls_init_for_reauth;

	ret = eap_peer_method_register(eap);
	if (ret)
		eap_peer_method_free(eap);
	return ret;
}

#endif /* EAP_TTLS */
