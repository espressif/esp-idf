/*
 * WPA Supplicant / shared MSCHAPV2 helper functions / RFC 2433 / RFC 2759
 * Copyright (c) 2004-2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */


#include "utils/includes.h"

#include "utils/common.h"
#include "sha1.h"
#include "ms_funcs.h"
#include "crypto.h"

/**
 * utf8_to_ucs2 - Convert UTF-8 string to UCS-2 encoding
 * @utf8_string: UTF-8 string (IN)
 * @utf8_string_len: Length of utf8_string (IN)
 * @ucs2_buffer: UCS-2 buffer (OUT)
 * @ucs2_buffer_size: Length of UCS-2 buffer (IN)
 * @ucs2_string_size: Number of 2-byte words in the resulting UCS-2 string
 * Returns: 0 on success, -1 on failure
 */
static int utf8_to_ucs2(const u8 *utf8_string, size_t utf8_string_len,
	     u8 *ucs2_buffer, size_t ucs2_buffer_size,
	     size_t *ucs2_string_size)
{
	size_t i, j;

	for (i = 0, j = 0; i < utf8_string_len; i++) {
		u8 c = utf8_string[i];
		if (j >= ucs2_buffer_size) {
			/* input too long */
			return -1;
		}
		if (c <= 0x7F) {
			WPA_PUT_LE16(ucs2_buffer + j, c);
			j += 2;
		} else if (i == utf8_string_len - 1 ||
			   j >= ucs2_buffer_size - 1) {
			/* incomplete surrogate */
			return -1;
		} else {
			u8 c2 = utf8_string[++i];
			if ((c & 0xE0) == 0xC0) {
				/* two-byte encoding */
				WPA_PUT_LE16(ucs2_buffer + j,
					     ((c & 0x1F) << 6) | (c2 & 0x3F));
				j += 2;
			} else if (i == utf8_string_len ||
				   j >= ucs2_buffer_size - 1) {
				/* incomplete surrogate */
				return -1;
			} else {
				/* three-byte encoding */
				u8 c3 = utf8_string[++i];
				WPA_PUT_LE16(ucs2_buffer + j,
					     ((c & 0xF) << 12) |
					     ((c2 & 0x3F) << 6) | (c3 & 0x3F));
			}
		}
	}

	if (ucs2_string_size)
		*ucs2_string_size = j / 2;
	return 0;
}


/**
 * challenge_hash - ChallengeHash() - RFC 2759, Sect. 8.2
 * @peer_challenge: 16-octet PeerChallenge (IN)
 * @auth_challenge: 16-octet AuthenticatorChallenge (IN)
 * @username: 0-to-256-char UserName (IN)
 * @username_len: Length of username
 * @challenge: 8-octet Challenge (OUT)
 * Returns: 0 on success, -1 on failure
 */
static int challenge_hash(const u8 *peer_challenge, const u8 *auth_challenge,
	       const u8 *username, size_t username_len,
	       u8 *challenge)
{
	u8 hash[SHA1_MAC_LEN];
	const unsigned char *addr[3];
	size_t len[3];

	addr[0] = peer_challenge;
	len[0] = 16;
	addr[1] = auth_challenge;
	len[1] = 16;
	addr[2] = username;
	len[2] = username_len;

	if (sha1_vector(3, addr, len, hash))
		return -1;
	os_memcpy(challenge, hash, 8);
	return 0;
}


/**
 * nt_password_hash - NtPasswordHash() - RFC 2759, Sect. 8.3
 * @password: 0-to-256-unicode-char Password (IN; UTF-8)
 * @password_len: Length of password
 * @password_hash: 16-octet PasswordHash (OUT)
 * Returns: 0 on success, -1 on failure
 */
int nt_password_hash(const u8 *password, size_t password_len,
		 u8 *password_hash)
{
	u8 buf[512], *pos;
	size_t len, max_len;

	max_len = sizeof(buf);
	if (utf8_to_ucs2(password, password_len, buf, max_len, &len) < 0)
		return -1;

	len *= 2;
	pos = buf;
	return md4_vector(1, (const u8 **) &pos, &len, password_hash);
}


/**
 * hash_nt_password_hash - HashNtPasswordHash() - RFC 2759, Sect. 8.4
 * @password_hash: 16-octet PasswordHash (IN)
 * @password_hash_hash: 16-octet PasswordHashHash (OUT)
 * Returns: 0 on success, -1 on failure
 */
int hash_nt_password_hash(const u8 *password_hash, u8 *password_hash_hash)
{
	size_t len = 16;
	return md4_vector(1, &password_hash, &len, password_hash_hash);
}


/**
 * challenge_response - ChallengeResponse() - RFC 2759, Sect. 8.5
 * @challenge: 8-octet Challenge (IN)
 * @password_hash: 16-octet PasswordHash (IN)
 * @response: 24-octet Response (OUT)
 */
void challenge_response(const u8 *challenge, const u8 *password_hash,
		   u8 *response)
{
	u8 zpwd[7];
	des_encrypt(challenge, password_hash, response);
	des_encrypt(challenge, password_hash + 7, response + 8);
	zpwd[0] = password_hash[14];
	zpwd[1] = password_hash[15];
	os_memset(zpwd + 2, 0, 5);
	des_encrypt(challenge, zpwd, response + 16);
}


/**
 * generate_nt_response - GenerateNTResponse() - RFC 2759, Sect. 8.1
 * @auth_challenge: 16-octet AuthenticatorChallenge (IN)
 * @peer_challenge: 16-octet PeerChallenge (IN)
 * @username: 0-to-256-char UserName (IN)
 * @username_len: Length of username
 * @password: 0-to-256-unicode-char Password (IN; UTF-8)
 * @password_len: Length of password
 * @response: 24-octet Response (OUT)
 * Returns: 0 on success, -1 on failure
 */
int generate_nt_response(const u8 *auth_challenge, const u8 *peer_challenge,
		     const u8 *username, size_t username_len,
		     const u8 *password, size_t password_len,
		     u8 *response)
{
	u8 challenge[8];
	u8 password_hash[16];

	if (challenge_hash(peer_challenge, auth_challenge, username,
			   username_len, challenge))
		return -1;
	if (nt_password_hash(password, password_len, password_hash))
		return -1;
	challenge_response(challenge, password_hash, response);
	return 0;
}


/**
 * generate_nt_response_pwhash - GenerateNTResponse() - RFC 2759, Sect. 8.1
 * @auth_challenge: 16-octet AuthenticatorChallenge (IN)
 * @peer_challenge: 16-octet PeerChallenge (IN)
 * @username: 0-to-256-char UserName (IN)
 * @username_len: Length of username
 * @password_hash: 16-octet PasswordHash (IN)
 * @response: 24-octet Response (OUT)
 * Returns: 0 on success, -1 on failure
 */
int generate_nt_response_pwhash(const u8 *auth_challenge,
			    const u8 *peer_challenge,
			    const u8 *username, size_t username_len,
			    const u8 *password_hash,
			    u8 *response)
{
	u8 challenge[8];

	if (challenge_hash(peer_challenge, auth_challenge,
			   username, username_len,
			   challenge))
		return -1;
	challenge_response(challenge, password_hash, response);
	return 0;
}


/**
 * generate_authenticator_response_pwhash - GenerateAuthenticatorResponse() - RFC 2759, Sect. 8.7
 * @password_hash: 16-octet PasswordHash (IN)
 * @nt_response: 24-octet NT-Response (IN)
 * @peer_challenge: 16-octet PeerChallenge (IN)
 * @auth_challenge: 16-octet AuthenticatorChallenge (IN)
 * @username: 0-to-256-char UserName (IN)
 * @username_len: Length of username
 * @response: 20-octet AuthenticatorResponse (OUT) (note: this value is usually
 * encoded as a 42-octet ASCII string (S=hexdump_of_response)
 * Returns: 0 on success, -1 on failure
 */
int generate_authenticator_response_pwhash(
	const u8 *password_hash,
	const u8 *peer_challenge, const u8 *auth_challenge,
	const u8 *username, size_t username_len,
	const u8 *nt_response, u8 *response)
{
	static const u8 magic1[39] = {
		0x4D, 0x61, 0x67, 0x69, 0x63, 0x20, 0x73, 0x65, 0x72, 0x76,
		0x65, 0x72, 0x20, 0x74, 0x6F, 0x20, 0x63, 0x6C, 0x69, 0x65,
		0x6E, 0x74, 0x20, 0x73, 0x69, 0x67, 0x6E, 0x69, 0x6E, 0x67,
		0x20, 0x63, 0x6F, 0x6E, 0x73, 0x74, 0x61, 0x6E, 0x74
	};
	static const u8 magic2[41] = {
		0x50, 0x61, 0x64, 0x20, 0x74, 0x6F, 0x20, 0x6D, 0x61, 0x6B,
		0x65, 0x20, 0x69, 0x74, 0x20, 0x64, 0x6F, 0x20, 0x6D, 0x6F,
		0x72, 0x65, 0x20, 0x74, 0x68, 0x61, 0x6E, 0x20, 0x6F, 0x6E,
		0x65, 0x20, 0x69, 0x74, 0x65, 0x72, 0x61, 0x74, 0x69, 0x6F,
		0x6E
	};

	u8 password_hash_hash[16], challenge[8];
	const unsigned char *addr1[3];
	const size_t len1[3] = { 16, 24, sizeof(magic1) };
	const unsigned char *addr2[3];
	const size_t len2[3] = { SHA1_MAC_LEN, 8, sizeof(magic2) };

	addr1[0] = password_hash_hash;
	addr1[1] = nt_response;
	addr1[2] = magic1;

	addr2[0] = response;
	addr2[1] = challenge;
	addr2[2] = magic2;

	if (hash_nt_password_hash(password_hash, password_hash_hash))
		return -1;
	if (sha1_vector(3, addr1, len1, response))
		return -1;

	if (challenge_hash(peer_challenge, auth_challenge, username,
			   username_len, challenge))
		return -1;
	return sha1_vector(3, addr2, len2, response);
}


/**
 * generate_authenticator_response - GenerateAuthenticatorResponse() - RFC 2759, Sect. 8.7
 * @password: 0-to-256-unicode-char Password (IN; UTF-8)
 * @password_len: Length of password
 * @nt_response: 24-octet NT-Response (IN)
 * @peer_challenge: 16-octet PeerChallenge (IN)
 * @auth_challenge: 16-octet AuthenticatorChallenge (IN)
 * @username: 0-to-256-char UserName (IN)
 * @username_len: Length of username
 * @response: 20-octet AuthenticatorResponse (OUT) (note: this value is usually
 * encoded as a 42-octet ASCII string (S=hexdump_of_response)
 * Returns: 0 on success, -1 on failure
 */
int generate_authenticator_response(const u8 *password, size_t password_len,
				const u8 *peer_challenge,
				const u8 *auth_challenge,
				const u8 *username, size_t username_len,
				const u8 *nt_response, u8 *response)
{
	u8 password_hash[16];
	if (nt_password_hash(password, password_len, password_hash))
		return -1;
	return generate_authenticator_response_pwhash(
		password_hash, peer_challenge, auth_challenge,
		username, username_len, nt_response, response);
}


/**
 * nt_challenge_response - NtChallengeResponse() - RFC 2433, Sect. A.5
 * @challenge: 8-octet Challenge (IN)
 * @password: 0-to-256-unicode-char Password (IN; UTF-8)
 * @password_len: Length of password
 * @response: 24-octet Response (OUT)
 * Returns: 0 on success, -1 on failure
 */
int nt_challenge_response(const u8 *challenge, const u8 *password,
		      size_t password_len, u8 *response)
{
	u8 password_hash[16];
	if (nt_password_hash(password, password_len, password_hash))
		return -1;
	challenge_response(challenge, password_hash, response);
	return 0;
}


/**
 * get_master_key - GetMasterKey() - RFC 3079, Sect. 3.4
 * @password_hash_hash: 16-octet PasswordHashHash (IN)
 * @nt_response: 24-octet NTResponse (IN)
 * @master_key: 16-octet MasterKey (OUT)
 * Returns: 0 on success, -1 on failure
 */
int get_master_key(const u8 *password_hash_hash, const u8 *nt_response,
	       u8 *master_key)
{
	static const u8 magic1[27] = {
		0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x74,
		0x68, 0x65, 0x20, 0x4d, 0x50, 0x50, 0x45, 0x20, 0x4d,
		0x61, 0x73, 0x74, 0x65, 0x72, 0x20, 0x4b, 0x65, 0x79
	};
	const unsigned char *addr[3];
	const size_t len[3] = { 16, 24, sizeof(magic1) };
	u8 hash[SHA1_MAC_LEN];

	addr[0] = password_hash_hash;
	addr[1] = nt_response;
	addr[2] = magic1;

	if (sha1_vector(3, addr, len, hash))
		return -1;
	os_memcpy(master_key, hash, 16);
	return 0;
}


/**
 * get_asymetric_start_key - GetAsymetricStartKey() - RFC 3079, Sect. 3.4
 * @master_key: 16-octet MasterKey (IN)
 * @session_key: 8-to-16 octet SessionKey (OUT)
 * @session_key_len: SessionKeyLength (Length of session_key) (IN)
 * @is_send: IsSend (IN, BOOLEAN)
 * @is_server: IsServer (IN, BOOLEAN)
 * Returns: 0 on success, -1 on failure
 */
int get_asymetric_start_key(const u8 *master_key, u8 *session_key,
			size_t session_key_len, int is_send,
			int is_server)
{
	static const u8 magic2[84] = {
		0x4f, 0x6e, 0x20, 0x74, 0x68, 0x65, 0x20, 0x63, 0x6c, 0x69,
		0x65, 0x6e, 0x74, 0x20, 0x73, 0x69, 0x64, 0x65, 0x2c, 0x20,
		0x74, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x74, 0x68,
		0x65, 0x20, 0x73, 0x65, 0x6e, 0x64, 0x20, 0x6b, 0x65, 0x79,
		0x3b, 0x20, 0x6f, 0x6e, 0x20, 0x74, 0x68, 0x65, 0x20, 0x73,
		0x65, 0x72, 0x76, 0x65, 0x72, 0x20, 0x73, 0x69, 0x64, 0x65,
		0x2c, 0x20, 0x69, 0x74, 0x20, 0x69, 0x73, 0x20, 0x74, 0x68,
		0x65, 0x20, 0x72, 0x65, 0x63, 0x65, 0x69, 0x76, 0x65, 0x20,
		0x6b, 0x65, 0x79, 0x2e
	};
	static const u8 magic3[84] = {
		0x4f, 0x6e, 0x20, 0x74, 0x68, 0x65, 0x20, 0x63, 0x6c, 0x69,
		0x65, 0x6e, 0x74, 0x20, 0x73, 0x69, 0x64, 0x65, 0x2c, 0x20,
		0x74, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x74, 0x68,
		0x65, 0x20, 0x72, 0x65, 0x63, 0x65, 0x69, 0x76, 0x65, 0x20,
		0x6b, 0x65, 0x79, 0x3b, 0x20, 0x6f, 0x6e, 0x20, 0x74, 0x68,
		0x65, 0x20, 0x73, 0x65, 0x72, 0x76, 0x65, 0x72, 0x20, 0x73,
		0x69, 0x64, 0x65, 0x2c, 0x20, 0x69, 0x74, 0x20, 0x69, 0x73,
		0x20, 0x74, 0x68, 0x65, 0x20, 0x73, 0x65, 0x6e, 0x64, 0x20,
		0x6b, 0x65, 0x79, 0x2e
	};
	static const u8 shs_pad1[40] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	static const u8 shs_pad2[40] = {
		0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2,
		0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2,
		0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2,
		0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2
	};
	u8 digest[SHA1_MAC_LEN];
	const unsigned char *addr[4];
	const size_t len[4] = { 16, 40, 84, 40 };

	addr[0] = master_key;
	addr[1] = shs_pad1;
	if (is_send) {
		addr[2] = is_server ? magic3 : magic2;
	} else {
		addr[2] = is_server ? magic2 : magic3;
	}
	addr[3] = shs_pad2;

	if (sha1_vector(4, addr, len, digest))
		return -1;

	if (session_key_len > SHA1_MAC_LEN)
		session_key_len = SHA1_MAC_LEN;
	os_memcpy(session_key, digest, session_key_len);
	return 0;
}


#define PWBLOCK_LEN 516

/**
 * encrypt_pw_block_with_password_hash - EncryptPwBlockWithPasswordHash() - RFC 2759, Sect. 8.10
 * @password: 0-to-256-unicode-char Password (IN; UTF-8)
 * @password_len: Length of password
 * @password_hash: 16-octet PasswordHash (IN)
 * @pw_block: 516-byte PwBlock (OUT)
 * Returns: 0 on success, -1 on failure
 */
int encrypt_pw_block_with_password_hash(
	const u8 *password, size_t password_len,
	const u8 *password_hash, u8 *pw_block)
{
	size_t ucs2_len, offset;
	u8 *pos;

	os_memset(pw_block, 0, PWBLOCK_LEN);

	if (utf8_to_ucs2(password, password_len, pw_block, 512, &ucs2_len) < 0)
		return -1;

	if (ucs2_len > 256)
		return -1;

	offset = (256 - ucs2_len) * 2;
	if (offset != 0) {
		os_memmove(pw_block + offset, pw_block, ucs2_len * 2);
		if (os_get_random(pw_block, offset) < 0)
			return -1;
	}
	/*
	 * PasswordLength is 4 octets, but since the maximum password length is
	 * 256, only first two (in little endian byte order) can be non-zero.
	 */
	pos = &pw_block[2 * 256];
	WPA_PUT_LE16(pos, password_len * 2);
	rc4_skip(password_hash, 16, 0, pw_block, PWBLOCK_LEN);
	return 0;
}


/**
 * new_password_encrypted_with_old_nt_password_hash - NewPasswordEncryptedWithOldNtPasswordHash() - RFC 2759, Sect. 8.9
 * @new_password: 0-to-256-unicode-char NewPassword (IN; UTF-8)
 * @new_password_len: Length of new_password
 * @old_password: 0-to-256-unicode-char OldPassword (IN; UTF-8)
 * @old_password_len: Length of old_password
 * @encrypted_pw_block: 516-octet EncryptedPwBlock (OUT)
 * Returns: 0 on success, -1 on failure
 */
int new_password_encrypted_with_old_nt_password_hash(
	const u8 *new_password, size_t new_password_len,
	const u8 *old_password, size_t old_password_len,
	u8 *encrypted_pw_block)
{
	u8 password_hash[16];

	if (nt_password_hash(old_password, old_password_len, password_hash))
		return -1;
	if (encrypt_pw_block_with_password_hash(new_password, new_password_len,
						password_hash,
						encrypted_pw_block))
		return -1;
	return 0;
}


/**
 * nt_password_hash_encrypted_with_block - NtPasswordHashEncryptedWithBlock() - RFC 2759, Sect 8.13
 * @password_hash: 16-octer PasswordHash (IN)
 * @block: 16-octet Block (IN)
 * @cypher: 16-octer Cypher (OUT)
 */
void nt_password_hash_encrypted_with_block(const u8 *password_hash,
				      const u8 *block, u8 *cypher)
{
	des_encrypt(password_hash, block, cypher);
	des_encrypt(password_hash + 8, block + 7, cypher + 8);
}


/**
 * old_nt_password_hash_encrypted_with_new_nt_password_hash - OldNtPasswordHashEncryptedWithNewNtPasswordHash() - RFC 2759, Sect. 8.12
 * @new_password: 0-to-256-unicode-char NewPassword (IN; UTF-8)
 * @new_password_len: Length of new_password
 * @old_password: 0-to-256-unicode-char OldPassword (IN; UTF-8)
 * @old_password_len: Length of old_password
 * @encrypted_password_hash: 16-octet EncryptedPasswordHash (OUT)
 * Returns: 0 on success, -1 on failure
 */
int old_nt_password_hash_encrypted_with_new_nt_password_hash(
	const u8 *new_password, size_t new_password_len,
	const u8 *old_password, size_t old_password_len,
	u8 *encrypted_password_hash)
{
	u8 old_password_hash[16], new_password_hash[16];

	if (nt_password_hash(old_password, old_password_len,
			     old_password_hash) ||
	    nt_password_hash(new_password, new_password_len,
			     new_password_hash))
		return -1;
	nt_password_hash_encrypted_with_block(old_password_hash,
					      new_password_hash,
					      encrypted_password_hash);
	return 0;
}

