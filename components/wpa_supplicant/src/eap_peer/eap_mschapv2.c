/*
 * EAP peer method: EAP-MSCHAPV2 (draft-kamath-pppext-eap-mschapv2-00.txt)
 * Copyright (c) 2004-2008, 2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */


#ifdef EAP_MSCHAPv2

#include "rsn_supp/wpa.h"
#include "crypto/random.h"
#include "crypto/ms_funcs.h"
#include "crypto/tls.h"
#include "eap_peer/eap_i.h"
#include "eap_peer/eap_defs.h"
#include "eap_peer/eap_tls_common.h"
#include "eap_peer/eap_config.h"
#include "eap_peer/mschapv2.h"
#include "eap_peer/eap_methods.h"
#include "common/wpa_ctrl.h"

#define MSCHAPV2_OP_CHALLENGE		1
#define MSCHAPV2_OP_RESPONSE		2
#define MSCHAPV2_OP_SUCCESS		3
#define MSCHAPV2_OP_FAILURE		4
#define MSCHAPV2_OP_CHANGE_PASSWORD	7

#define PASSWD_CHANGE_CHAL_LEN		16
#define MSCHAPV2_KEY_LEN		16

#define ERROR_RESTRICTED_LOGON_HOURS	646
#define ERROR_ACCT_DISABLED		647
#define ERROR_PASSWD_EXPIRED		648
#define ERROR_NO_DIALIN_PERMISSION	649
#define ERROR_AUTHENTICATION_FAILURE	691
#define ERROR_CHANGING_PASSWORD		709

struct eap_mschapv2_hdr {
	u8 op_code;
	u8 mschapv2_id;
	u8 ms_length[2];
} __packed;

struct ms_response {
	u8 peer_challenge[MSCHAPV2_CHAL_LEN];
	u8 reserved[8];
	u8 nt_response[MSCHAPV2_NT_RESPONSE_LEN];
	u8 flags;
} __packed;

struct ms_change_password {
	u8 encr_password[516];
	u8 encr_hash[16];
	u8 peer_challenge[MSCHAPV2_CHAL_LEN];
	u8 reserved[8];
	u8 nt_response[MSCHAPV2_NT_RESPONSE_LEN];
	u8 flags[2];
} __packed;

struct eap_mschapv2_data {
	u8 auth_response[MSCHAPV2_AUTH_RESPONSE_LEN];
	int auth_response_valid;

	int prev_error;
	u8 passwd_change_challenge[PASSWD_CHANGE_CHAL_LEN];
	int passwd_change_challenge_valid;
	int passwd_change_version;

	u8 *peer_challenge;
	u8 *auth_challenge;

	int phase2;
	u8 master_key[MSCHAPV2_MASTER_KEY_LEN];
	int master_key_valid;
	int success;

	struct wpabuf *prev_challenge;
};

static void
eap_mschapv2_deinit(struct eap_sm *sm, void *priv)
{
	struct eap_mschapv2_data *data = priv;

	os_free(data->peer_challenge);
	os_free(data->auth_challenge);
	wpabuf_free(data->prev_challenge);
	os_free(data);
}

static void *
eap_mschapv2_init(struct eap_sm *sm)
{
	struct eap_mschapv2_data *data;

	//Do not init insecure unencapsulated MSCHAPv2 as Phase 1 method, only init if Phase 2
	if(!sm->init_phase2)
		return NULL;

	data = (struct eap_mschapv2_data *)os_zalloc(sizeof(*data));
	if (data == NULL)
		return NULL;

	if (sm->peer_challenge) {
		data->peer_challenge = os_memdup(sm->peer_challenge,
						 MSCHAPV2_CHAL_LEN);
		if (data->peer_challenge == NULL) {
			eap_mschapv2_deinit(sm, data);
			return NULL;
		}
	}

	if (sm->auth_challenge) {
		data->auth_challenge = os_memdup(sm->auth_challenge,
						 MSCHAPV2_CHAL_LEN);
		if (data->auth_challenge == NULL) {
			eap_mschapv2_deinit(sm, data);
			return NULL;
		}
	}

	data->phase2 = sm->init_phase2;

	return data;
}

static struct wpabuf *
eap_mschapv2_challenge_reply(
	struct eap_sm *sm, struct eap_mschapv2_data *data,
	u8 id, u8 mschapv2_id, const u8 *auth_challenge)
{
	struct wpabuf *resp;
	struct eap_mschapv2_hdr *ms;
	u8 *peer_challenge;
	int ms_len;
	struct ms_response *r;
	size_t identity_len, password_len;
	const u8 *identity, *password;
	int pwhash;

	wpa_printf(MSG_DEBUG, "EAP-MSCHAPV2: Generate Challenge Response\n");

	identity = eap_get_config_identity(sm, &identity_len);
	password = eap_get_config_password2(sm, &password_len, &pwhash);
	if (identity == NULL || password == NULL)
		return NULL;

	ms_len = sizeof(*ms) + 1 + sizeof(*r) + identity_len;
	resp = eap_msg_alloc(EAP_VENDOR_IETF, EAP_TYPE_MSCHAPV2,
			     ms_len, EAP_CODE_RESPONSE, id);
	if (resp == NULL)
		return NULL;

	ms = wpabuf_put(resp, sizeof(*ms));
	ms->op_code = MSCHAPV2_OP_RESPONSE;
	ms->mschapv2_id = mschapv2_id;
	if (data->prev_error) {
		/*
		 * TODO: this does not seem to be enough when processing two
		 * or more failure messages. IAS did not increment mschapv2_id
		 * in its own packets, but it seemed to expect the peer to
		 * increment this for all packets(?).
		 */
		ms->mschapv2_id++;
	}
	WPA_PUT_BE16(ms->ms_length, ms_len);
	wpabuf_put_u8(resp, sizeof(*r));

	/* Response */
	r = wpabuf_put(resp, sizeof(*r));
	peer_challenge = r->peer_challenge;
	if (data->peer_challenge) {
		wpa_printf(MSG_DEBUG, "EAP-MSCHAPV2: peer_challenge generated "
			   "in Phase 1");
		peer_challenge = data->peer_challenge;
		os_memset(r->peer_challenge, 0, MSCHAPV2_CHAL_LEN);
	} else if (random_get_bytes(peer_challenge, MSCHAPV2_CHAL_LEN)) {
		wpabuf_free(resp);
		return NULL;
	}
	os_memset(r->reserved, 0, 8);
	if (data->auth_challenge) {
		wpa_printf(MSG_DEBUG, "EAP-MSCHAPV2: auth_challenge generated "
			   "in Phase 1");
		auth_challenge = data->auth_challenge;
	}
	if (mschapv2_derive_response(identity, identity_len, password,
				     password_len, pwhash, auth_challenge,
				     peer_challenge, r->nt_response,
				     data->auth_response, data->master_key)) {
		wpa_printf(MSG_ERROR, "EAP-MSCHAPV2: Failed to derive "
			   "response");
		wpabuf_free(resp);
		return NULL;
	}
	data->auth_response_valid = 1;
	data->master_key_valid = 1;

	r->flags = 0; /* reserved, must be zero */

	wpabuf_put_data(resp, identity, identity_len);
	wpa_printf(MSG_DEBUG, "EAP-MSCHAPV2: TX identifier %d mschapv2_id %d "
		   "(response)", id, ms->mschapv2_id);
	return resp;
}


/**
 * eap_mschapv2_process - Process an EAP-MSCHAPv2 challenge message
 * @sm: Pointer to EAP state machine allocated with eap_peer_sm_init()
 * @data: Pointer to private EAP method data from eap_mschapv2_init()
 * @ret: Return values from EAP request validation and processing
 * @req: Pointer to EAP-MSCHAPv2 header from the request
 * @req_len: Length of the EAP-MSCHAPv2 data
 * @id: EAP identifier used in the request
 * Returns: Pointer to allocated EAP response packet (eapRespData) or %NULL if
 * no reply available
 */
static struct wpabuf * eap_mschapv2_challenge(
	struct eap_sm *sm, struct eap_mschapv2_data *data,
	struct eap_method_ret *ret, const struct eap_mschapv2_hdr *req,
	size_t req_len, u8 id)
{
	size_t len, challenge_len;
	const u8 *pos, *challenge;

	if (eap_get_config_identity(sm, &len) == NULL ||
	    eap_get_config_password(sm, &len) == NULL)
		return NULL;

	wpa_printf(MSG_DEBUG, "EAP-MSCHAPV2: Received challenge");
	if (req_len < sizeof(*req) + 1) {
		wpa_printf(MSG_INFO, "EAP-MSCHAPV2: Too short challenge data "
			   "(len %lu)", (unsigned long) req_len);
		ret->ignore = true;
		return NULL;
	}
	pos = (const u8 *)(req + 1);
	challenge_len = *pos++;
	len = req_len - sizeof(*req) - 1;
	if (challenge_len != MSCHAPV2_CHAL_LEN) {
		wpa_printf(MSG_INFO, "EAP-MSCHAPV2: Invalid challenge length "
			   "%lu", (unsigned long) challenge_len);
		ret->ignore = true;
		return NULL;
	}

	if (len < challenge_len) {
		wpa_printf(MSG_INFO, "EAP-MSCHAPV2: Too short challenge"
			   " packet: len=%lu challenge_len=%lu",
			   (unsigned long) len, (unsigned long) challenge_len);
		ret->ignore = true;
		return NULL;
	}

	if (data->passwd_change_challenge_valid) {
		wpa_printf(MSG_DEBUG, "EAP-MSCHAPV2: Using challenge from the "
			   "failure message");
		challenge = data->passwd_change_challenge;
	} else
		challenge = pos;
	pos += challenge_len;
	len -= challenge_len;
	wpa_hexdump_ascii(MSG_DEBUG, "EAP-MSCHAPV2: Authentication Servername",
		    pos, len);

	ret->ignore = false;
	ret->methodState = METHOD_MAY_CONT;
	ret->decision = DECISION_FAIL;
	ret->allowNotifications = true;

	return eap_mschapv2_challenge_reply(sm, data, id, req->mschapv2_id,
					    challenge);
}

static void
eap_mschapv2_password_changed(struct eap_sm *sm,
			      struct eap_mschapv2_data *data)
{
	struct eap_peer_config *config = eap_get_config(sm);
	if (config && config->new_password) {
		wpa_msg(sm->msg_ctx, MSG_INFO,
			WPA_EVENT_PASSWORD_CHANGED
			"EAP-MSCHAPV2: Password changed successfully");
		data->prev_error = 0;
		os_free(config->password);
		if (config->flags & EAP_CONFIG_FLAGS_EXT_PASSWORD) {
			/* TODO: update external storage */
		} else if (config->flags & EAP_CONFIG_FLAGS_PASSWORD_NTHASH) {
			config->password = os_malloc(16);
			config->password_len = 16;
			if (config->password) {
				nt_password_hash(config->new_password,
						 config->new_password_len,
						 config->password);
			}
			os_free(config->new_password);
		} else {
			config->password = config->new_password;
			config->password_len = config->new_password_len;
		}
		config->new_password = NULL;
		config->new_password_len = 0;
	}
}

static struct wpabuf *
eap_mschapv2_success(struct eap_sm *sm,
		     struct eap_mschapv2_data *data,
		     struct eap_method_ret *ret,
		     const struct eap_mschapv2_hdr *req,
		     size_t req_len, u8 id)
{
	struct wpabuf *resp;
	const u8 *pos;
	size_t len;

	wpa_printf(MSG_DEBUG, "EAP-MSCHAPV2: Received success");
	len = req_len - sizeof(*req);
	pos = (const u8 *) (req + 1);
	if (!data->auth_response_valid ||
	    mschapv2_verify_auth_response(data->auth_response, pos, len)) {
		wpa_printf(MSG_WARNING, "EAP-MSCHAPV2: Invalid authenticator "
			   "response in success request");
		ret->methodState = METHOD_DONE;
		ret->decision = DECISION_FAIL;
		return NULL;
	}
	pos += 2 + 2 * MSCHAPV2_AUTH_RESPONSE_LEN;
	len -= 2 + 2 * MSCHAPV2_AUTH_RESPONSE_LEN;
	while (len > 0 && *pos == ' ') {
		pos++;
		len--;
	}
	wpa_hexdump_ascii(MSG_DEBUG, "EAP-MSCHAPV2: Success message",
			  pos, len);
	wpa_printf(MSG_INFO, "EAP-MSCHAPV2: Authentication succeeded");

	/* Note: Only op_code of the EAP-MSCHAPV2 header is included in success
	 * message. */
	resp = eap_msg_alloc(EAP_VENDOR_IETF, EAP_TYPE_MSCHAPV2, 1,
			     EAP_CODE_RESPONSE, id);
	if (resp == NULL) {
		wpa_printf(MSG_DEBUG, "EAP-MSCHAPV2: Failed to allocate "
			   "buffer for success response");
		ret->ignore = true;
		return NULL;
	}

	wpabuf_put_u8(resp, MSCHAPV2_OP_SUCCESS); /* op_code */

	ret->methodState = METHOD_DONE;
	ret->decision = DECISION_UNCOND_SUCC;
	ret->allowNotifications = false;
	data->success = 1;

	if (data->prev_error == ERROR_PASSWD_EXPIRED)
		eap_mschapv2_password_changed(sm, data);

	return resp;
}


static int eap_mschapv2_failure_txt(struct eap_sm *sm,
				    struct eap_mschapv2_data *data, char *txt)
{
	char *pos = "";
	int retry = 1;
	struct eap_peer_config *config = eap_get_config(sm);

	/* For example:
	 * E=691 R=1 C=<32 octets hex challenge> V=3 M=Authentication Failure
	 */

	pos = txt;

	if (pos && os_strncmp(pos, "E=", 2) == 0) {
		pos += 2;
		data->prev_error = atoi(pos);
		wpa_printf(MSG_DEBUG, "EAP-MSCHAPV2: error %d",
			   data->prev_error);
		pos = (char *)os_strchr(pos, ' ');
		if (pos)
			pos++;
	}

	if (pos && os_strncmp(pos, "R=", 2) == 0) {
		pos += 2;
		retry = atoi(pos);
		wpa_printf(MSG_DEBUG, "EAP-MSCHAPV2: retry is %sallowed",
			   retry == 1 ? "" : "not ");
		pos = (char *)os_strchr(pos, ' ');
		if (pos)
			pos++;
	}

	if (pos && os_strncmp(pos, "C=", 2) == 0) {
		int hex_len;
		pos += 2;
		hex_len = (char *)os_strchr(pos, ' ') - (char *)pos;
		if (hex_len == PASSWD_CHANGE_CHAL_LEN * 2) {
			if (hexstr2bin(pos, data->passwd_change_challenge,
				       PASSWD_CHANGE_CHAL_LEN)) {
				wpa_printf(MSG_DEBUG, "EAP-MSCHAPV2: invalid "
					   "failure challenge");
			} else {
				wpa_hexdump(MSG_DEBUG, "EAP-MSCHAPV2: failure "
					    "challenge",
					    data->passwd_change_challenge,
					    PASSWD_CHANGE_CHAL_LEN);
				data->passwd_change_challenge_valid = 1;
			}
		} else {
			wpa_printf(MSG_DEBUG, "EAP-MSCHAPV2: invalid failure "
				   "challenge len %d", hex_len);
		}
		pos = os_strchr(pos, ' ');
		if (pos)
			pos++;
	} else {
		wpa_printf(MSG_DEBUG, "EAP-MSCHAPV2: required challenge field "
			   "was not present in failure message");
	}

	if (pos && os_strncmp(pos, "V=", 2) == 0) {
		pos += 2;
		data->passwd_change_version = atoi(pos);
		wpa_printf(MSG_DEBUG, "EAP-MSCHAPV2: password changing "
			   "protocol version %d", data->passwd_change_version);
		pos = (char *)os_strchr(pos, ' ');
		if (pos)
			pos++;
	}

	if (pos && os_strncmp(pos, "M=", 2) == 0) {
		pos += 2;
	}
	if (data->prev_error == ERROR_AUTHENTICATION_FAILURE && retry &&
	    config && config->phase2 &&
	    os_strstr(config->phase2, "mschapv2_retry=0")) {
		wpa_printf(MSG_DEBUG,
			   "EAP-MSCHAPV2: mark password retry disabled based on local configuration");
		retry = 0;
	}
	if (data->prev_error == ERROR_PASSWD_EXPIRED &&
	    data->passwd_change_version == 3 && config) {
		if (config->new_password == NULL) {
			wpa_msg(sm->msg_ctx, MSG_INFO,
				"EAP-MSCHAPV2: Password expired - password "
				"change required");
			eap_sm_request_new_password(sm);
		}
	} else if (retry == 1 && config) {
		/* TODO: could prevent the current password from being used
		 * again at least for some period of time */
		if (!config->mschapv2_retry)
			eap_sm_request_identity(sm);
		eap_sm_request_password(sm);
		config->mschapv2_retry = 1;
	} else if (config) {
		/* TODO: prevent retries using same username/password */
		config->mschapv2_retry = 0;
	}

	return retry == 1;
}


static struct wpabuf * eap_mschapv2_change_password(
	struct eap_sm *sm, struct eap_mschapv2_data *data,
	struct eap_method_ret *ret, const struct eap_mschapv2_hdr *req, u8 id)
{
	struct wpabuf *resp;
	int ms_len;
	const u8 *username, *password, *new_password;
	size_t username_len, password_len, new_password_len;
	struct eap_mschapv2_hdr *ms;
	struct ms_change_password *cp;
	u8 password_hash[16], password_hash_hash[16];
	int pwhash;

	username = eap_get_config_identity(sm, &username_len);
	password = eap_get_config_password2(sm, &password_len, &pwhash);
	new_password = eap_get_config_new_password(sm, &new_password_len);
	if (username == NULL || password == NULL || new_password == NULL)
		return NULL;

	username = mschapv2_remove_domain(username, &username_len);

	ret->ignore = false;
	ret->methodState = METHOD_MAY_CONT;
	ret->decision = DECISION_COND_SUCC;
	ret->allowNotifications = TRUE;

	ms_len = sizeof(*ms) + sizeof(*cp);
	resp = eap_msg_alloc(EAP_VENDOR_IETF, EAP_TYPE_MSCHAPV2, ms_len,
			     EAP_CODE_RESPONSE, id);
	if (resp == NULL)
		return NULL;

	ms = wpabuf_put(resp, sizeof(*ms));
	ms->op_code = MSCHAPV2_OP_CHANGE_PASSWORD;
	ms->mschapv2_id = req->mschapv2_id + 1;
	WPA_PUT_BE16(ms->ms_length, ms_len);
	cp = wpabuf_put(resp, sizeof(*cp));

	/* Encrypted-Password */
	if (pwhash) {
		if (encrypt_pw_block_with_password_hash(
			    new_password, new_password_len,
			    password, cp->encr_password))
			goto fail;
	} else {
		if (new_password_encrypted_with_old_nt_password_hash(
			    new_password, new_password_len,
			    password, password_len, cp->encr_password))
			goto fail;
	}

	/* Encrypted-Hash */
	if (pwhash) {
		u8 new_password_hash[16];
		if (nt_password_hash(new_password, new_password_len,
				     new_password_hash) ||
		    nt_password_hash_encrypted_with_block(password,
							  new_password_hash,
							  cp->encr_hash))
			goto fail;
	} else {
		if (old_nt_password_hash_encrypted_with_new_nt_password_hash(
			    new_password, new_password_len,
			    password, password_len, cp->encr_hash))
			goto fail;
	}

	/* Peer-Challenge */
	if (random_get_bytes(cp->peer_challenge, MSCHAPV2_CHAL_LEN))
		goto fail;

	/* Reserved, must be zero */
	os_memset(cp->reserved, 0, 8);

	/* NT-Response */
	wpa_hexdump(MSG_DEBUG, "EAP-MSCHAPV2: auth_challenge",
		    data->passwd_change_challenge, PASSWD_CHANGE_CHAL_LEN);
	wpa_hexdump(MSG_DEBUG, "EAP-MSCHAPV2: peer_challenge",
		    cp->peer_challenge, MSCHAPV2_CHAL_LEN);
	wpa_hexdump_ascii(MSG_DEBUG, "EAP-MSCHAPV2: username",
			  username, username_len);
	wpa_hexdump_ascii_key(MSG_DEBUG, "EAP-MSCHAPV2: new password",
			      new_password, new_password_len);
	generate_nt_response(data->passwd_change_challenge, cp->peer_challenge,
			     username, username_len,
			     new_password, new_password_len,
			     cp->nt_response);
	wpa_hexdump(MSG_DEBUG, "EAP-MSCHAPV2: NT-Response",
		    cp->nt_response, MSCHAPV2_NT_RESPONSE_LEN);

	/* Authenticator response is not really needed yet, but calculate it
	 * here so that challenges need not be saved. */
	generate_authenticator_response(new_password, new_password_len,
					cp->peer_challenge,
					data->passwd_change_challenge,
					username, username_len,
					cp->nt_response, data->auth_response);
	data->auth_response_valid = 1;

	/* Likewise, generate master_key here since we have the needed data
	 * available. */
	if (nt_password_hash(new_password, new_password_len, password_hash) ||
	    hash_nt_password_hash(password_hash, password_hash_hash) ||
	    get_master_key(password_hash_hash, cp->nt_response,
			   data->master_key)) {
		data->auth_response_valid = 0;
		goto fail;
	}
	data->master_key_valid = 1;

	/* Flags */
	os_memset(cp->flags, 0, 2);

	wpa_printf(MSG_DEBUG, "EAP-MSCHAPV2: TX identifier %d mschapv2_id %d "
		   "(change pw)", id, ms->mschapv2_id);

	return resp;

fail:
	wpabuf_free(resp);
	return NULL;
}


/**
 * eap_mschapv2_process - Process an EAP-MSCHAPv2 failure message
 * @sm: Pointer to EAP state machine allocated with eap_peer_sm_init()
 * @data: Pointer to private EAP method data from eap_mschapv2_init()
 * @ret: Return values from EAP request validation and processing
 * @req: Pointer to EAP-MSCHAPv2 header from the request
 * @req_len: Length of the EAP-MSCHAPv2 data
 * @id: EAP identifier used in th erequest
 * Returns: Pointer to allocated EAP response packet (eapRespData) or %NULL if
 * no reply available
 */
static struct wpabuf * eap_mschapv2_failure(struct eap_sm *sm,
					    struct eap_mschapv2_data *data,
					    struct eap_method_ret *ret,
					    const struct eap_mschapv2_hdr *req,
					    size_t req_len, u8 id)
{
	struct wpabuf *resp;
	const u8 *msdata = (const u8 *) (req + 1);
	char *buf;
	size_t len = req_len - sizeof(*req);
	int retry = 0;

	wpa_printf(MSG_DEBUG, "EAP-MSCHAPV2: Received failure");
	wpa_hexdump_ascii(MSG_DEBUG, "EAP-MSCHAPV2: Failure data",
			  msdata, len);
	/*
	 * eap_mschapv2_failure_txt() expects a nul terminated string, so we
	 * must allocate a large enough temporary buffer to create that since
	 * the received message does not include nul termination.
	 */
	buf = (char *)dup_binstr(msdata, len);
	if (buf) {
		retry = eap_mschapv2_failure_txt(sm, data, buf);
		os_free(buf);
	}

	ret->ignore = false;
	ret->methodState = METHOD_DONE;
	ret->decision = DECISION_FAIL;
	ret->allowNotifications = false;

	if (data->prev_error == ERROR_PASSWD_EXPIRED &&
	    data->passwd_change_version == 3) {
		struct eap_peer_config *config = eap_get_config(sm);
		if (config && config->new_password)
			return eap_mschapv2_change_password(sm, data, ret, req,
							    id);
		if (config && config->pending_req_new_password)
			return NULL;
	} else if (retry && data->prev_error == ERROR_AUTHENTICATION_FAILURE) {
		/* TODO: could try to retry authentication, e.g, after having
		 * changed the username/password. In this case, EAP MS-CHAP-v2
		 * Failure Response would not be sent here. */
		return NULL;
	}

	/* Note: Only op_code of the EAP-MSCHAPV2 header is included in failure
	 * message. */
	resp = eap_msg_alloc(EAP_VENDOR_IETF, EAP_TYPE_MSCHAPV2, 1,
			     EAP_CODE_RESPONSE, id);
	if (resp == NULL)
		return NULL;

	wpabuf_put_u8(resp, MSCHAPV2_OP_FAILURE); /* op_code */

	return resp;
}


static int eap_mschapv2_check_config(struct eap_sm *sm)
{
	struct eap_peer_config *config = eap_get_config(sm);

	if (config == NULL)
		return -1;

	if (config->identity == NULL ||
	    config->identity_len == 0) {
		wpa_printf(MSG_ERROR, "EAP-MSCHAPV2: idetity not configured\n");
		return -1;
	}

	if (config->password == NULL ||
	    config->password_len == 0) {
		wpa_printf(MSG_ERROR, "EAP-MSCHAPV2: Password not configured\n");
		return -1;
	}

	return 0;
}


static int eap_mschapv2_check_mslen(struct eap_sm *sm, size_t len,
				    const struct eap_mschapv2_hdr *ms)
{
	size_t ms_len = WPA_GET_BE16(ms->ms_length);

	if (ms_len == len)
		return 0;

	wpa_printf(MSG_INFO, "EAP-MSCHAPV2: Invalid header: len=%lu "
		   "ms_len=%lu", (unsigned long) len, (unsigned long) ms_len);
	if (sm->workaround) {
		/* Some authentication servers use invalid ms_len,
		 * ignore it for interoperability. */
		wpa_printf(MSG_INFO, "EAP-MSCHAPV2: workaround, ignore"
			   " invalid ms_len %lu (len %lu)",
			   (unsigned long) ms_len,
			   (unsigned long) len);
		return 0;
	}
	wpa_printf(MSG_ERROR, "EAP-MSCHAPV2: Invalid header len=%lu ms_len=%lu\n",
		  (unsigned long)len, (unsigned long)ms_len);

	return -1;
}


static void eap_mschapv2_copy_challenge(struct eap_mschapv2_data *data,
					const struct wpabuf *reqData)
{
	/*
	 * Store a copy of the challenge message, so that it can be processed
	 * again in case retry is allowed after a possible failure.
	 */
	wpabuf_free(data->prev_challenge);
	data->prev_challenge = wpabuf_dup(reqData);
}


/**
 * eap_mschapv2_process - Process an EAP-MSCHAPv2 request
 * @sm: Pointer to EAP state machine allocated with eap_peer_sm_init()
 * @priv: Pointer to private EAP method data from eap_mschapv2_init()
 * @ret: Return values from EAP request validation and processing
 * @reqData: EAP request to be processed (eapReqData)
 * Returns: Pointer to allocated EAP response packet (eapRespData) or %NULL if
 * no reply available
 */
static struct wpabuf * eap_mschapv2_process(struct eap_sm *sm, void *priv,
					    struct eap_method_ret *ret,
					    const struct wpabuf *reqData)
{
	u8 id;
	size_t len;
	const u8 *pos;
	int using_prev_challenge = 0;
	const struct eap_mschapv2_hdr *ms;
	struct eap_mschapv2_data *data = priv;
	struct eap_peer_config *config = eap_get_config(sm);

	if (eap_mschapv2_check_config(sm)) {
		ret->ignore = true;
		return NULL;
	}

	if (config->mschapv2_retry && data->prev_challenge &&
	    data->prev_error == ERROR_AUTHENTICATION_FAILURE) {
		wpa_printf(MSG_DEBUG, "EAP-MSCHAPV2: Replacing pending packet "
			   "with the previous challenge");

		reqData = data->prev_challenge;
		using_prev_challenge = 1;
		config->mschapv2_retry = 0;
	}

	pos = eap_hdr_validate(EAP_VENDOR_IETF, EAP_TYPE_MSCHAPV2, reqData,
			       &len);
	if (pos == NULL || len < sizeof(*ms) + 1) {
		ret->ignore = true;
		return NULL;
	}

	ms = (const struct eap_mschapv2_hdr *) pos;
	if (eap_mschapv2_check_mslen(sm, len, ms)) {
		ret->ignore = true;
		return NULL;
	}

	id = eap_get_id(reqData);
	wpa_printf(MSG_DEBUG, "EAP-MSCHAPV2: RX identifier %d mschapv2_id %d",
		   id, ms->mschapv2_id);

	switch (ms->op_code) {
	case MSCHAPV2_OP_CHALLENGE:
		if (!using_prev_challenge)
			eap_mschapv2_copy_challenge(data, reqData);
		return eap_mschapv2_challenge(sm, data, ret, ms, len, id);
	case MSCHAPV2_OP_SUCCESS:
		return eap_mschapv2_success(sm, data, ret, ms, len, id);
	case MSCHAPV2_OP_FAILURE:
		return eap_mschapv2_failure(sm, data, ret, ms, len, id);
	default:
		wpa_printf(MSG_ERROR, "EAP-MSCHAPV2: Unknown op code %d - ignored\n",
			  ms->op_code);
		ret->ignore = TRUE;
		return NULL;
	}
}


static bool eap_mschapv2_isKeyAvailable(struct eap_sm *sm, void *priv)
{
	struct eap_mschapv2_data *data = priv;
	return data->success && data->master_key_valid;
}


static u8 * eap_mschapv2_getKey(struct eap_sm *sm, void *priv, size_t *len)
{
	struct eap_mschapv2_data *data = priv;
	u8 *key;
	int key_len;

	if (!data->master_key_valid || !data->success)
		return NULL;

	key_len = 2 * MSCHAPV2_KEY_LEN;

	key = os_malloc(key_len);
	if (key == NULL)
		return NULL;

	/* MSK = server MS-MPPE-Recv-Key | MS-MPPE-Send-Key, i.e.,
	 *	peer MS-MPPE-Send-Key | MS-MPPE-Recv-Key */
	get_asymetric_start_key(data->master_key, key, MSCHAPV2_KEY_LEN, 1, 0);
	get_asymetric_start_key(data->master_key, key + MSCHAPV2_KEY_LEN,
				MSCHAPV2_KEY_LEN, 0, 0);

	wpa_hexdump_key(MSG_DEBUG, "EAP-MSCHAPV2: Derived key",
			key, key_len);

	*len = key_len;
	return key;
}


/**
 * eap_peer_mschapv2_register - Register EAP-MSCHAPv2 peer method
 * Returns: 0 on success, -1 on failure
 *
 * This function is used to register EAP-MSCHAPv2 peer method into the EAP
 * method list.
 */
int eap_peer_mschapv2_register(void)
{
	struct eap_method *eap;
	int ret;

	eap = eap_peer_method_alloc(EAP_VENDOR_IETF, EAP_TYPE_MSCHAPV2,
				    "MSCHAPV2");

	if (eap == NULL)
		return -1;

	eap->init = eap_mschapv2_init;
	eap->deinit = eap_mschapv2_deinit;
	eap->process = eap_mschapv2_process;
	eap->isKeyAvailable = eap_mschapv2_isKeyAvailable;
	eap->getKey = eap_mschapv2_getKey;

	ret = eap_peer_method_register(eap);
	if (ret)
		eap_peer_method_free(eap);
	return ret;
}

#endif /* EAP_MSCHAPv2 */
