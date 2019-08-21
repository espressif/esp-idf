/*
 * MSCHAPV2
 */


#ifndef MSCHAPV2_H
#define MSCHAPV2_H

#define MSCHAPV2_CHAL_LEN		16
#define MSCHAPV2_NT_RESPONSE_LEN	24
#define MSCHAPV2_AUTH_RESPONSE_LEN	20
#define MSCHAPV2_MASTER_KEY_LEN		16

const u8 * mschapv2_remove_domain(const u8 *username, size_t *len);
int mschapv2_derive_response(const u8 *username, size_t username_len,
			     const u8 *password, size_t password_len,
			     int pwhash,
			     const u8 *auth_challenge,
			     const u8 *peer_challenge,
			     u8 *nt_response, u8 *auth_response,
			     u8 *master_key);
int mschapv2_verify_auth_response(const u8 *auth_response,
				  const u8 *buf, size_t buf_len);
#endif /* MSCHAPV2_H */
