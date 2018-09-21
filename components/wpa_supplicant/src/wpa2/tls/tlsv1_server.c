/*
 * TLS v1.0/v1.1/v1.2 server (RFC 2246, RFC 4346, RFC 5246)
 * Copyright (c) 2006-2011, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "wpa/includes.h"

#include "wpa/common.h"
#include "crypto/sha1.h"
#include "wpa2/tls/tls.h"
#include "wpa2/tls/tlsv1_common.h"
#include "wpa2/tls/tlsv1_record.h"
#include "wpa2/tls/tlsv1_server.h"
#include "wpa2/tls/tlsv1_server_i.h"

/* TODO:
 * Support for a message fragmented across several records (RFC 2246, 6.2.1)
 */


void tlsv1_server_alert(struct tlsv1_server *conn, u8 level, u8 description)
{
	conn->alert_level = level;
	conn->alert_description = description;
}


int tlsv1_server_derive_keys(struct tlsv1_server *conn,
			     const u8 *pre_master_secret,
			     size_t pre_master_secret_len)
{
	u8 seed[2 * TLS_RANDOM_LEN];
	u8 key_block[TLS_MAX_KEY_BLOCK_LEN];
	u8 *pos;
	size_t key_block_len;

	if (pre_master_secret) {
		wpa_hexdump_key(MSG_MSGDUMP, "TLSv1: pre_master_secret",
				pre_master_secret, pre_master_secret_len);
		os_memcpy(seed, conn->client_random, TLS_RANDOM_LEN);
		os_memcpy(seed + TLS_RANDOM_LEN, conn->server_random,
			  TLS_RANDOM_LEN);
		if (tls_prf(conn->rl.tls_version,
			    pre_master_secret, pre_master_secret_len,
			    "master secret", seed, 2 * TLS_RANDOM_LEN,
			    conn->master_secret, TLS_MASTER_SECRET_LEN)) {
			wpa_printf(MSG_DEBUG, "TLSv1: Failed to derive "
				   "master_secret");
			return -1;
		}
		wpa_hexdump_key(MSG_MSGDUMP, "TLSv1: master_secret",
				conn->master_secret, TLS_MASTER_SECRET_LEN);
	}

	os_memcpy(seed, conn->server_random, TLS_RANDOM_LEN);
	os_memcpy(seed + TLS_RANDOM_LEN, conn->client_random, TLS_RANDOM_LEN);
	key_block_len = 2 * (conn->rl.hash_size + conn->rl.key_material_len +
			     conn->rl.iv_size);
	if (tls_prf(conn->rl.tls_version,
		    conn->master_secret, TLS_MASTER_SECRET_LEN,
		    "key expansion", seed, 2 * TLS_RANDOM_LEN,
		    key_block, key_block_len)) {
		wpa_printf(MSG_DEBUG, "TLSv1: Failed to derive key_block");
		return -1;
	}
	wpa_hexdump_key(MSG_MSGDUMP, "TLSv1: key_block",
			key_block, key_block_len);

	pos = key_block;

	/* client_write_MAC_secret */
	os_memcpy(conn->rl.read_mac_secret, pos, conn->rl.hash_size);
	pos += conn->rl.hash_size;
	/* server_write_MAC_secret */
	os_memcpy(conn->rl.write_mac_secret, pos, conn->rl.hash_size);
	pos += conn->rl.hash_size;

	/* client_write_key */
	os_memcpy(conn->rl.read_key, pos, conn->rl.key_material_len);
	pos += conn->rl.key_material_len;
	/* server_write_key */
	os_memcpy(conn->rl.write_key, pos, conn->rl.key_material_len);
	pos += conn->rl.key_material_len;

	/* client_write_IV */
	os_memcpy(conn->rl.read_iv, pos, conn->rl.iv_size);
	pos += conn->rl.iv_size;
	/* server_write_IV */
	os_memcpy(conn->rl.write_iv, pos, conn->rl.iv_size);
	pos += conn->rl.iv_size;

	return 0;
}


/**
 * tlsv1_server_handshake - Process TLS handshake
 * @conn: TLSv1 server connection data from tlsv1_server_init()
 * @in_data: Input data from TLS peer
 * @in_len: Input data length
 * @out_len: Length of the output buffer.
 * Returns: Pointer to output data, %NULL on failure
 */
u8 * tlsv1_server_handshake(struct tlsv1_server *conn,
			    const u8 *in_data, size_t in_len,
			    size_t *out_len)
{
	const u8 *pos, *end;
	u8 *msg = NULL, *in_msg, *in_pos, *in_end, alert, ct;
	size_t in_msg_len;
	int used;

	if (in_data == NULL || in_len == 0) {
		wpa_printf(MSG_DEBUG, "TLSv1: No input data to server");
		return NULL;
	}

	pos = in_data;
	end = in_data + in_len;
	in_msg = os_malloc(in_len);
	if (in_msg == NULL)
		return NULL;

	/* Each received packet may include multiple records */
	while (pos < end) {
		in_msg_len = in_len;
		used = tlsv1_record_receive(&conn->rl, pos, end - pos,
					    in_msg, &in_msg_len, &alert);
		if (used < 0) {
			wpa_printf(MSG_DEBUG, "TLSv1: Processing received "
				   "record failed");
			tlsv1_server_alert(conn, TLS_ALERT_LEVEL_FATAL, alert);
			goto failed;
		}
		if (used == 0) {
			/* need more data */
			wpa_printf(MSG_DEBUG, "TLSv1: Partial processing not "
				   "yet supported");
			tlsv1_server_alert(conn, TLS_ALERT_LEVEL_FATAL, alert);
			goto failed;
		}
		ct = pos[0];

		in_pos = in_msg;
		in_end = in_msg + in_msg_len;

		/* Each received record may include multiple messages of the
		 * same ContentType. */
		while (in_pos < in_end) {
			in_msg_len = in_end - in_pos;
			if (tlsv1_server_process_handshake(conn, ct, in_pos,
							   &in_msg_len) < 0)
				goto failed;
			in_pos += in_msg_len;
		}

		pos += used;
	}

	os_free(in_msg);
	in_msg = NULL;

	msg = tlsv1_server_handshake_write(conn, out_len);

failed:
	os_free(in_msg);
	if (conn->alert_level) {
		if (conn->state == FAILED) {
			/* Avoid alert loops */
			wpa_printf(MSG_DEBUG, "TLSv1: Drop alert loop");
			os_free(msg);
			return NULL;
		}
		conn->state = FAILED;
		os_free(msg);
		msg = tlsv1_server_send_alert(conn, conn->alert_level,
					      conn->alert_description,
					      out_len);
	}

	return msg;
}


/**
 * tlsv1_server_encrypt - Encrypt data into TLS tunnel
 * @conn: TLSv1 server connection data from tlsv1_server_init()
 * @in_data: Pointer to plaintext data to be encrypted
 * @in_len: Input buffer length
 * @out_data: Pointer to output buffer (encrypted TLS data)
 * @out_len: Maximum out_data length 
 * Returns: Number of bytes written to out_data, -1 on failure
 *
 * This function is used after TLS handshake has been completed successfully to
 * send data in the encrypted tunnel.
 */
int tlsv1_server_encrypt(struct tlsv1_server *conn,
			 const u8 *in_data, size_t in_len,
			 u8 *out_data, size_t out_len)
{
	size_t rlen;

	wpa_hexdump_key(MSG_MSGDUMP, "TLSv1: Plaintext AppData",
			in_data, in_len);

	if (tlsv1_record_send(&conn->rl, TLS_CONTENT_TYPE_APPLICATION_DATA,
			      out_data, out_len, in_data, in_len, &rlen) < 0) {
		wpa_printf(MSG_DEBUG, "TLSv1: Failed to create a record");
		tlsv1_server_alert(conn, TLS_ALERT_LEVEL_FATAL,
				   TLS_ALERT_INTERNAL_ERROR);
		return -1;
	}

	return rlen;
}


/**
 * tlsv1_server_decrypt - Decrypt data from TLS tunnel
 * @conn: TLSv1 server connection data from tlsv1_server_init()
 * @in_data: Pointer to input buffer (encrypted TLS data)
 * @in_len: Input buffer length
 * @out_data: Pointer to output buffer (decrypted data from TLS tunnel)
 * @out_len: Maximum out_data length
 * Returns: Number of bytes written to out_data, -1 on failure
 *
 * This function is used after TLS handshake has been completed successfully to
 * receive data from the encrypted tunnel.
 */
int tlsv1_server_decrypt(struct tlsv1_server *conn,
			 const u8 *in_data, size_t in_len,
			 u8 *out_data, size_t out_len)
{
	const u8 *in_end, *pos;
	int used;
	u8 alert, *out_end, *out_pos, ct;
	size_t olen;

	pos = in_data;
	in_end = in_data + in_len;
	out_pos = out_data;
	out_end = out_data + out_len;

	while (pos < in_end) {
		ct = pos[0];
		olen = out_end - out_pos;
		used = tlsv1_record_receive(&conn->rl, pos, in_end - pos,
					    out_pos, &olen, &alert);
		if (used < 0) {
			wpa_printf(MSG_DEBUG, "TLSv1: Record layer processing "
				   "failed");
			tlsv1_server_alert(conn, TLS_ALERT_LEVEL_FATAL, alert);
			return -1;
		}
		if (used == 0) {
			/* need more data */
			wpa_printf(MSG_DEBUG, "TLSv1: Partial processing not "
				   "yet supported");
			tlsv1_server_alert(conn, TLS_ALERT_LEVEL_FATAL, alert);
			return -1;
		}

		if (ct == TLS_CONTENT_TYPE_ALERT) {
			if (olen < 2) {
				wpa_printf(MSG_DEBUG, "TLSv1: Alert "
					   "underflow");
				tlsv1_server_alert(conn, TLS_ALERT_LEVEL_FATAL,
						   TLS_ALERT_DECODE_ERROR);
				return -1;
			}
			wpa_printf(MSG_DEBUG, "TLSv1: Received alert %d:%d",
				   out_pos[0], out_pos[1]);
			if (out_pos[0] == TLS_ALERT_LEVEL_WARNING) {
				/* Continue processing */
				pos += used;
				continue;
			}

			tlsv1_server_alert(conn, TLS_ALERT_LEVEL_FATAL,
					   out_pos[1]);
			return -1;
		}

		if (ct != TLS_CONTENT_TYPE_APPLICATION_DATA) {
			wpa_printf(MSG_DEBUG, "TLSv1: Unexpected content type "
				   "0x%x", pos[0]);
			tlsv1_server_alert(conn, TLS_ALERT_LEVEL_FATAL,
					   TLS_ALERT_UNEXPECTED_MESSAGE);
			return -1;
		}

		out_pos += olen;
		if (out_pos > out_end) {
			wpa_printf(MSG_DEBUG, "TLSv1: Buffer not large enough "
				   "for processing the received record");
			tlsv1_server_alert(conn, TLS_ALERT_LEVEL_FATAL,
					   TLS_ALERT_INTERNAL_ERROR);
			return -1;
		}

		pos += used;
	}

	return out_pos - out_data;
}


/**
 * tlsv1_server_global_init - Initialize TLSv1 server
 * Returns: 0 on success, -1 on failure
 *
 * This function must be called before using any other TLSv1 server functions.
 */
int tlsv1_server_global_init(void)
{
	return crypto_global_init();
}


/**
 * tlsv1_server_global_deinit - Deinitialize TLSv1 server
 *
 * This function can be used to deinitialize the TLSv1 server that was
 * initialized by calling tlsv1_server_global_init(). No TLSv1 server functions
 * can be called after this before calling tlsv1_server_global_init() again.
 */
void tlsv1_server_global_deinit(void)
{
	crypto_global_deinit();
}


/**
 * tlsv1_server_init - Initialize TLSv1 server connection
 * @cred: Pointer to server credentials from tlsv1_server_cred_alloc()
 * Returns: Pointer to TLSv1 server connection data or %NULL on failure
 */
struct tlsv1_server * tlsv1_server_init(struct tlsv1_credentials *cred)
{
	struct tlsv1_server *conn;
	size_t count;
	u16 *suites;

	conn = (struct tlsv1_server *)os_zalloc(sizeof(*conn));
	if (conn == NULL)
		return NULL;

	conn->cred = cred;

	conn->state = CLIENT_HELLO;

	if (tls_verify_hash_init(&conn->verify) < 0) {
		wpa_printf(MSG_DEBUG, "TLSv1: Failed to initialize verify "
			   "hash");
		os_free(conn);
		return NULL;
	}

	count = 0;
	suites = conn->cipher_suites;
	suites[count++] = TLS_RSA_WITH_AES_256_CBC_SHA;
	suites[count++] = TLS_RSA_WITH_AES_128_CBC_SHA;
#ifdef CONFIG_DES3
	suites[count++] = TLS_RSA_WITH_3DES_EDE_CBC_SHA;
#endif
	suites[count++] = TLS_RSA_WITH_RC4_128_SHA;
	suites[count++] = TLS_RSA_WITH_RC4_128_MD5;
	conn->num_cipher_suites = count;

	return conn;
}


static void tlsv1_server_clear_data(struct tlsv1_server *conn)
{
	tlsv1_record_set_cipher_suite(&conn->rl, TLS_NULL_WITH_NULL_NULL);
	tlsv1_record_change_write_cipher(&conn->rl);
	tlsv1_record_change_read_cipher(&conn->rl);
	tls_verify_hash_free(&conn->verify);

	crypto_public_key_free(conn->client_rsa_key);
	conn->client_rsa_key = NULL;

	os_free(conn->session_ticket);
	conn->session_ticket = NULL;
	conn->session_ticket_len = 0;
	conn->use_session_ticket = 0;

	os_free(conn->dh_secret);
	conn->dh_secret = NULL;
	conn->dh_secret_len = 0;
}


/**
 * tlsv1_server_deinit - Deinitialize TLSv1 server connection
 * @conn: TLSv1 server connection data from tlsv1_server_init()
 */
void tlsv1_server_deinit(struct tlsv1_server *conn)
{
	tlsv1_server_clear_data(conn);
	os_free(conn);
}


/**
 * tlsv1_server_established - Check whether connection has been established
 * @conn: TLSv1 server connection data from tlsv1_server_init()
 * Returns: 1 if connection is established, 0 if not
 */
int tlsv1_server_established(struct tlsv1_server *conn)
{
	return conn->state == ESTABLISHED;
}


/**
 * tlsv1_server_prf - Use TLS-PRF to derive keying material
 * @conn: TLSv1 server connection data from tlsv1_server_init()
 * @label: Label (e.g., description of the key) for PRF
 * @server_random_first: seed is 0 = client_random|server_random,
 * 1 = server_random|client_random
 * @out: Buffer for output data from TLS-PRF
 * @out_len: Length of the output buffer
 * Returns: 0 on success, -1 on failure
 */
int tlsv1_server_prf(struct tlsv1_server *conn, const char *label,
		     int server_random_first, u8 *out, size_t out_len)
{
	u8 seed[2 * TLS_RANDOM_LEN];

	if (conn->state != ESTABLISHED)
		return -1;

	if (server_random_first) {
		os_memcpy(seed, conn->server_random, TLS_RANDOM_LEN);
		os_memcpy(seed + TLS_RANDOM_LEN, conn->client_random,
			  TLS_RANDOM_LEN);
	} else {
		os_memcpy(seed, conn->client_random, TLS_RANDOM_LEN);
		os_memcpy(seed + TLS_RANDOM_LEN, conn->server_random,
			  TLS_RANDOM_LEN);
	}

	return tls_prf(conn->rl.tls_version,
		       conn->master_secret, TLS_MASTER_SECRET_LEN,
		       label, seed, 2 * TLS_RANDOM_LEN, out, out_len);
}


/**
 * tlsv1_server_get_cipher - Get current cipher name
 * @conn: TLSv1 server connection data from tlsv1_server_init()
 * @buf: Buffer for the cipher name
 * @buflen: buf size
 * Returns: 0 on success, -1 on failure
 *
 * Get the name of the currently used cipher.
 */
int tlsv1_server_get_cipher(struct tlsv1_server *conn, char *buf,
			    size_t buflen)
{
#ifndef ESPRESSIF_USE	
    char *cipher;

	switch (conn->rl.cipher_suite) {
	case TLS_RSA_WITH_RC4_128_MD5:
		cipher = "RC4-MD5";
		break;
	case TLS_RSA_WITH_RC4_128_SHA:
		cipher = "RC4-SHA";
		break;
#ifdef CONFIG_DES
	case TLS_RSA_WITH_DES_CBC_SHA:
		cipher = "DES-CBC-SHA";
		break;
#endif
#ifdef CONFIG_DES3
	case TLS_RSA_WITH_3DES_EDE_CBC_SHA:
		cipher = "DES-CBC3-SHA";
		break;
#endif
	case TLS_DH_anon_WITH_AES_128_CBC_SHA:
		cipher = "ADH-AES-128-SHA";
		break;
	case TLS_RSA_WITH_AES_256_CBC_SHA:
		cipher = "AES-256-SHA";
		break;
	case TLS_RSA_WITH_AES_128_CBC_SHA:
		cipher = "AES-128-SHA";
		break;
	default:
		return -1;
	}

	//if (os_strlcpy(buf, cipher, buflen) >= buflen)
	//	return -1;

	os_memcpy((u8 *)buf, (u8 *)cipher, buflen);

	return 0;
#else
    char cipher[20];

    switch (conn->rl.cipher_suite) {
        case TLS_RSA_WITH_RC4_128_MD5:
		    strcpy(cipher, "RC4-MD5");
		    break;
        case TLS_RSA_WITH_RC4_128_SHA:
            strcpy(cipher, "RC4-SHA");
            break;
        case TLS_RSA_WITH_DES_CBC_SHA:
            strcpy(cipher, "DES-CBC-SHA");
            break;
        case TLS_RSA_WITH_3DES_EDE_CBC_SHA:
            strcpy(cipher, "DES-CBC3-SHA");
            break;
        case TLS_DH_anon_WITH_AES_128_CBC_SHA:
            strcpy(cipher, "ADH-AES-128-SHA");
            break;
        case TLS_RSA_WITH_AES_256_CBC_SHA:
            strcpy(cipher, "AES-256-SHA");
            break;
        case TLS_RSA_WITH_AES_128_CBC_SHA:
            strcpy(cipher, "AES-128-SHA");
            break;
        default:
            return -1;
	}
    os_memcpy((u8 *)buf, (u8 *)cipher, buflen);

    return 0;
#endif
}


/**
 * tlsv1_server_shutdown - Shutdown TLS connection
 * @conn: TLSv1 server connection data from tlsv1_server_init()
 * Returns: 0 on success, -1 on failure
 */
int tlsv1_server_shutdown(struct tlsv1_server *conn)
{
	conn->state = CLIENT_HELLO;

	if (tls_verify_hash_init(&conn->verify) < 0) {
		wpa_printf(MSG_DEBUG, "TLSv1: Failed to re-initialize verify "
			   "hash");
		return -1;
	}

	tlsv1_server_clear_data(conn);

	return 0;
}


/**
 * tlsv1_server_resumed - Was session resumption used
 * @conn: TLSv1 server connection data from tlsv1_server_init()
 * Returns: 1 if current session used session resumption, 0 if not
 */
int tlsv1_server_resumed(struct tlsv1_server *conn)
{
	return 0;
}


/**
 * tlsv1_server_get_keys - Get master key and random data from TLS connection
 * @conn: TLSv1 server connection data from tlsv1_server_init()
 * @keys: Structure of key/random data (filled on success)
 * Returns: 0 on success, -1 on failure
 */
int tlsv1_server_get_keys(struct tlsv1_server *conn, struct tls_keys *keys)
{
	os_memset(keys, 0, sizeof(*keys));
	if (conn->state == CLIENT_HELLO)
		return -1;

	keys->client_random = conn->client_random;
	keys->client_random_len = TLS_RANDOM_LEN;

	if (conn->state != SERVER_HELLO) {
		keys->server_random = conn->server_random;
		keys->server_random_len = TLS_RANDOM_LEN;
		keys->master_key = conn->master_secret;
		keys->master_key_len = TLS_MASTER_SECRET_LEN;
	}

	return 0;
}


/**
 * tlsv1_server_get_keyblock_size - Get TLS key_block size
 * @conn: TLSv1 server connection data from tlsv1_server_init()
 * Returns: Size of the key_block for the negotiated cipher suite or -1 on
 * failure
 */
int tlsv1_server_get_keyblock_size(struct tlsv1_server *conn)
{
	if (conn->state == CLIENT_HELLO || conn->state == SERVER_HELLO)
		return -1;

	return 2 * (conn->rl.hash_size + conn->rl.key_material_len +
		    conn->rl.iv_size);
}


/**
 * tlsv1_server_set_cipher_list - Configure acceptable cipher suites
 * @conn: TLSv1 server connection data from tlsv1_server_init()
 * @ciphers: Zero (TLS_CIPHER_NONE) terminated list of allowed ciphers
 * (TLS_CIPHER_*).
 * Returns: 0 on success, -1 on failure
 */
int tlsv1_server_set_cipher_list(struct tlsv1_server *conn, u8 *ciphers)
{
	size_t count;
	u16 *suites;

	/* TODO: implement proper configuration of cipher suites */
	if (ciphers[0] == TLS_CIPHER_ANON_DH_AES128_SHA) {
		count = 0;
		suites = conn->cipher_suites;
		suites[count++] = TLS_RSA_WITH_AES_256_CBC_SHA;
		suites[count++] = TLS_RSA_WITH_AES_128_CBC_SHA;
		suites[count++] = TLS_RSA_WITH_3DES_EDE_CBC_SHA;
		suites[count++] = TLS_RSA_WITH_RC4_128_SHA;
		suites[count++] = TLS_RSA_WITH_RC4_128_MD5;
		suites[count++] = TLS_DH_anon_WITH_AES_256_CBC_SHA;
		suites[count++] = TLS_DH_anon_WITH_AES_128_CBC_SHA;
		suites[count++] = TLS_DH_anon_WITH_3DES_EDE_CBC_SHA;
		suites[count++] = TLS_DH_anon_WITH_RC4_128_MD5;
		suites[count++] = TLS_DH_anon_WITH_DES_CBC_SHA;
		conn->num_cipher_suites = count;
	}

	return 0;
}


int tlsv1_server_set_verify(struct tlsv1_server *conn, int verify_peer)
{
	conn->verify_peer = verify_peer;
	return 0;
}


void tlsv1_server_set_session_ticket_cb(struct tlsv1_server *conn,
					tlsv1_server_session_ticket_cb cb,
					void *ctx)
{
	wpa_printf(MSG_DEBUG, "TLSv1: SessionTicket callback set %p (ctx %p)",
		   cb, ctx);
	conn->session_ticket_cb = cb;
	conn->session_ticket_cb_ctx = ctx;
}
