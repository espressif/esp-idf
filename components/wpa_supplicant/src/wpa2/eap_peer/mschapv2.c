/*
 * MSCHAPV2
 */

#ifdef EAP_MSCHAPv2

#include "wpa/includes.h"
#include "wpa/common.h"
#include "crypto/ms_funcs.h"
#include "wpa2/eap_peer/mschapv2.h"

const u8 * mschapv2_remove_domain(const u8 *username, size_t *len)
{
	size_t i;

	/*
	 * MSCHAPV2 does not include optional domain name in the
	 * challenge-response calculation, so remove domain prefix
	 * (if present)
	 */
	for (i = 0; i < *len; i++) {
		if (username[i] == '\\') {
			*len -= i + 1;
			return username + i + 1;
		}
	}

	return username;
}

int mschapv2_derive_response(const u8 *identity, size_t identity_len,
			 const u8 *password, size_t password_len,
			 int pwhash,
			 const u8 *auth_challenge,
			 const u8 *peer_challenge,
			 u8 *nt_response, u8 *auth_response,
			 u8 *master_key)
{
	const u8 *username;
	size_t username_len;
	u8 password_hash[16], password_hash_hash[16];

	username_len = identity_len;
	username = mschapv2_remove_domain(identity, &username_len);

	if (pwhash) {
		if (generate_nt_response_pwhash(auth_challenge, peer_challenge,
						username, username_len,
						password, nt_response) ||
		    generate_authenticator_response_pwhash(
				password, peer_challenge, auth_challenge,
				username, username_len, nt_response,
				auth_response))
			return -1;
	} else {
		if (generate_nt_response(auth_challenge, peer_challenge,
					 username, username_len,
					 password, password_len,
					 nt_response) ||
		    generate_authenticator_response(password, password_len,
						    peer_challenge,
						    auth_challenge,
						    username, username_len,
						    nt_response,
						    auth_response))
			return -1;
	}

	if (pwhash) {
		if (hash_nt_password_hash(password, password_hash_hash))
			return -1;
	} else {
		if (nt_password_hash(password, password_len, password_hash) ||
		    hash_nt_password_hash(password_hash, password_hash_hash))
			return -1;
	}
	if (get_master_key(password_hash_hash, nt_response, master_key))
		return -1;

	return 0;
}

int mschapv2_verify_auth_response(const u8 *auth_response,
			      const u8 *buf, size_t buf_len)
{
	u8 recv_response[MSCHAPV2_AUTH_RESPONSE_LEN];
	if (buf_len < 2 + 2 * MSCHAPV2_AUTH_RESPONSE_LEN ||
	    buf[0] != 'S' || buf[1] != '=' ||
	    hexstr2bin((char *)(buf + 2), recv_response,
		       MSCHAPV2_AUTH_RESPONSE_LEN) ||
	    os_memcmp(auth_response, recv_response,
		      MSCHAPV2_AUTH_RESPONSE_LEN) != 0)
		return -1;
	return 0;
}

#endif /* EAP_MSCHAPv2 */
