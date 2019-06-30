/*
 * TLS interface functions and an internal TLS implementation
 * Copyright (c) 2004-2011, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 *
 * This file interface functions for hostapd/wpa_supplicant to use the
 * integrated TLSv1 implementation.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "crypto/sha1.h"
#include "crypto/md5.h"
#include "tls/tls.h"
#include "tls/tlsv1_client.h"
#include "tls/tlsv1_server.h"

#ifndef CONFIG_TLS_INTERNAL_CLIENT
#define CONFIG_TLS_INTERNAL_CLIENT
#endif

static int tls_ref_count = 0;

struct tls_global {
	int server;
	struct tlsv1_credentials *server_cred;
	int check_crl;
};

struct tls_connection {
	struct tlsv1_client *client;
	struct tlsv1_server *server;
};


void * tls_init(void)
{
	struct tls_global *global;

	if (tls_ref_count == 0) {
#ifdef CONFIG_TLS_INTERNAL_CLIENT
		if (tlsv1_client_global_init())
			return NULL;
#endif /* CONFIG_TLS_INTERNAL_CLIENT */
#ifdef CONFIG_TLS_INTERNAL_SERVER
		if (tlsv1_server_global_init())
			return NULL;
#endif /* CONFIG_TLS_INTERNAL_SERVER */
	}
	tls_ref_count++;

	global = (struct tls_global *)os_zalloc(sizeof(*global));
	if (global == NULL)
		return NULL;

	return global;
}

void tls_deinit(void *ssl_ctx)
{
	struct tls_global *global = ssl_ctx;
	tls_ref_count--;
	if (tls_ref_count == 0) {
#ifdef CONFIG_TLS_INTERNAL_CLIENT
		tlsv1_client_global_deinit();
#endif /* CONFIG_TLS_INTERNAL_CLIENT */
#ifdef CONFIG_TLS_INTERNAL_SERVER
		tlsv1_cred_free(global->server_cred);
		tlsv1_server_global_deinit();
#endif /* CONFIG_TLS_INTERNAL_SERVER */
	}
	os_free(global);
}


int tls_get_errors(void *tls_ctx)
{
	return 0;
}


struct tls_connection * tls_connection_init(void *tls_ctx)
{
	struct tls_connection *conn;
	struct tls_global *global = tls_ctx;

	conn = (struct tls_connection *)os_zalloc(sizeof(*conn));
	if (conn == NULL)
		return NULL;
#ifdef CONFIG_TLS_INTERNAL_CLIENT
	if (!global->server) {
		conn->client = tlsv1_client_init();
		if (conn->client == NULL) {
			os_free(conn);
			return NULL;
		}
	}
#endif /* CONFIG_TLS_INTERNAL_CLIENT */
#ifdef CONFIG_TLS_INTERNAL_SERVER
	if (global->server) {
		conn->server = tlsv1_server_init(global->server_cred);
		if (conn->server == NULL) {
			os_free(conn);
			return NULL;
		}
	}
#endif /* CONFIG_TLS_INTERNAL_SERVER */

	return conn;
}


void tls_connection_deinit(void *tls_ctx, struct tls_connection *conn)
{
	if (conn == NULL)
		return;
#ifdef CONFIG_TLS_INTERNAL_CLIENT
	if (conn->client)
		tlsv1_client_deinit(conn->client);
#endif /* CONFIG_TLS_INTERNAL_CLIENT */
#ifdef CONFIG_TLS_INTERNAL_SERVER
	if (conn->server)
		tlsv1_server_deinit(conn->server);
#endif /* CONFIG_TLS_INTERNAL_SERVER */
	os_free(conn);
}


int tls_connection_established(void *tls_ctx, struct tls_connection *conn)
{
#ifdef CONFIG_TLS_INTERNAL_CLIENT
	if (conn->client)
		return tlsv1_client_established(conn->client);
#endif /* CONFIG_TLS_INTERNAL_CLIENT */
#ifdef CONFIG_TLS_INTERNAL_SERVER
	if (conn->server)
		return tlsv1_server_established(conn->server);
#endif /* CONFIG_TLS_INTERNAL_SERVER */
	return 0;
}


int tls_connection_shutdown(void *tls_ctx, struct tls_connection *conn)
{
#ifdef CONFIG_TLS_INTERNAL_CLIENT
	if (conn->client)
		return tlsv1_client_shutdown(conn->client);
#endif /* CONFIG_TLS_INTERNAL_CLIENT */
#ifdef CONFIG_TLS_INTERNAL_SERVER
	if (conn->server)
		return tlsv1_server_shutdown(conn->server);
#endif /* CONFIG_TLS_INTERNAL_SERVER */
	return -1;
}


int tls_connection_set_params(void *tls_ctx, struct tls_connection *conn,
			      const struct tls_connection_params *params)
{
#ifdef CONFIG_TLS_INTERNAL_CLIENT
	struct tlsv1_credentials *cred;

	if (conn->client == NULL)
		return -1;

	cred = tlsv1_cred_alloc();
	if (cred == NULL)
		return -1;

	if (tlsv1_set_ca_cert(cred, params->ca_cert,
			      params->ca_cert_blob, params->ca_cert_blob_len,
			      params->ca_path)) {
		wpa_printf(MSG_INFO, "TLS: Failed to configure trusted CA "
			   "certificates");
		tlsv1_cred_free(cred);
		return -1;
	}

	if (tlsv1_set_cert(cred, params->client_cert,
			   params->client_cert_blob,
			   params->client_cert_blob_len)) {
		wpa_printf(MSG_INFO, "TLS: Failed to configure client "
			   "certificate");
		tlsv1_cred_free(cred);
		return -1;
	}

	if (tlsv1_set_private_key(cred, params->private_key,
				  params->private_key_passwd,
				  params->private_key_blob,
				  params->private_key_blob_len)) {
		wpa_printf(MSG_INFO, "TLS: Failed to load private key");
		tlsv1_cred_free(cred);
		return -1;
	}

	if (tlsv1_client_set_cred(conn->client, cred) < 0) {
		tlsv1_cred_free(cred);
		return -1;
	}

	tlsv1_client_set_time_checks(
		conn->client, !(params->flags & TLS_CONN_DISABLE_TIME_CHECKS));

	return 0;
#else /* CONFIG_TLS_INTERNAL_CLIENT */
	return -1;
#endif /* CONFIG_TLS_INTERNAL_CLIENT */
}


int tls_global_set_params(void *tls_ctx,
			  const struct tls_connection_params *params)
{
#ifdef CONFIG_TLS_INTERNAL_SERVER
	struct tls_global *global = tls_ctx;
	struct tlsv1_credentials *cred;

	/* Currently, global parameters are only set when running in server
	 * mode. */
	global->server = 1;
	tlsv1_cred_free(global->server_cred);
	global->server_cred = cred = tlsv1_cred_alloc();
	if (cred == NULL)
		return -1;

	if (tlsv1_set_ca_cert(cred, params->ca_cert, params->ca_cert_blob,
			      params->ca_cert_blob_len, params->ca_path)) {
		wpa_printf(MSG_INFO, "TLS: Failed to configure trusted CA "
			   "certificates");
		return -1;
	}

	if (tlsv1_set_cert(cred, params->client_cert, params->client_cert_blob,
			   params->client_cert_blob_len)) {
		wpa_printf(MSG_INFO, "TLS: Failed to configure server "
			   "certificate");
		return -1;
	}

	if (tlsv1_set_private_key(cred, params->private_key,
				  params->private_key_passwd,
				  params->private_key_blob,
				  params->private_key_blob_len)) {
		wpa_printf(MSG_INFO, "TLS: Failed to load private key");
		return -1;
	}

	if (tlsv1_set_dhparams(cred, params->dh_file, params->dh_blob,
			       params->dh_blob_len)) {
		wpa_printf(MSG_INFO, "TLS: Failed to load DH parameters");
		return -1;
	}

	return 0;
#else /* CONFIG_TLS_INTERNAL_SERVER */
	return -1;
#endif /* CONFIG_TLS_INTERNAL_SERVER */
}


int tls_global_set_verify(void *tls_ctx, int check_crl)
{
	struct tls_global *global = tls_ctx;
	global->check_crl = check_crl;
	return 0;
}


int tls_connection_set_verify(void *tls_ctx, struct tls_connection *conn,
			      int verify_peer)
{
#ifdef CONFIG_TLS_INTERNAL_SERVER
	if (conn->server)
		return tlsv1_server_set_verify(conn->server, verify_peer);
#endif /* CONFIG_TLS_INTERNAL_SERVER */
	return -1;
}


int tls_connection_get_keys(void *tls_ctx, struct tls_connection *conn,
			    struct tls_keys *keys)
{
#ifdef CONFIG_TLS_INTERNAL_CLIENT
	if (conn->client)
		return tlsv1_client_get_keys(conn->client, keys);
#endif /* CONFIG_TLS_INTERNAL_CLIENT */
#ifdef CONFIG_TLS_INTERNAL_SERVER
	if (conn->server)
		return tlsv1_server_get_keys(conn->server, keys);
#endif /* CONFIG_TLS_INTERNAL_SERVER */
	return -1;
}


int tls_connection_prf(void *tls_ctx, struct tls_connection *conn,
		       const char *label, int server_random_first,
		       u8 *out, size_t out_len)
{
#ifdef CONFIG_TLS_INTERNAL_CLIENT
	if (conn->client) {
		return tlsv1_client_prf(conn->client, label,
					server_random_first,
					out, out_len);
	}
#endif /* CONFIG_TLS_INTERNAL_CLIENT */
#ifdef CONFIG_TLS_INTERNAL_SERVER
	if (conn->server) {
		return tlsv1_server_prf(conn->server, label,
					server_random_first,
					out, out_len);
	}
#endif /* CONFIG_TLS_INTERNAL_SERVER */
	return -1;
}


struct wpabuf * tls_connection_handshake(void *tls_ctx,
					 struct tls_connection *conn,
					 const struct wpabuf *in_data,
					 struct wpabuf **appl_data)
{
	return tls_connection_handshake2(tls_ctx, conn, in_data, appl_data,
					 NULL);
}


struct wpabuf * tls_connection_handshake2(void *tls_ctx,
					  struct tls_connection *conn,
					  const struct wpabuf *in_data,
					  struct wpabuf **appl_data,
					  int *need_more_data)
{
#ifdef CONFIG_TLS_INTERNAL_CLIENT
	u8 *res, *ad;
	size_t res_len, ad_len;
	struct wpabuf *out;

	if (conn->client == NULL)
		return NULL;

	ad = NULL;
	res = tlsv1_client_handshake(conn->client,
				     in_data ? wpabuf_head(in_data) : NULL,
				     in_data ? wpabuf_len(in_data) : 0,
				     &res_len, &ad, &ad_len, need_more_data);
	if (res == NULL) {
		return NULL;
	}
	out = wpabuf_alloc_ext_data(res, res_len);
	if (out == NULL) {
		os_free(res);
		os_free(ad);
		return NULL;
	}
	if (appl_data) {
		if (ad) {
			*appl_data = wpabuf_alloc_ext_data(ad, ad_len);
			if (*appl_data == NULL)
				os_free(ad);
		} else
			*appl_data = NULL;
	} else
		os_free(ad);
    
	return out;
#else /* CONFIG_TLS_INTERNAL_CLIENT */
	return NULL;
#endif /* CONFIG_TLS_INTERNAL_CLIENT */
}


struct wpabuf * tls_connection_server_handshake(void *tls_ctx,
						struct tls_connection *conn,
						const struct wpabuf *in_data,
						struct wpabuf **appl_data)
{
#ifdef CONFIG_TLS_INTERNAL_SERVER
	u8 *res;
	size_t res_len;
	struct wpabuf *out;

	if (conn->server == NULL)
		return NULL;

	if (appl_data)
		*appl_data = NULL;

	res = tlsv1_server_handshake(conn->server, wpabuf_head(in_data),
				     wpabuf_len(in_data), &res_len);
	if (res == NULL && tlsv1_server_established(conn->server))
		return wpabuf_alloc(0);
	if (res == NULL)
		return NULL;
	out = wpabuf_alloc_ext_data(res, res_len);
	if (out == NULL) {
		os_free(res);
		return NULL;
	}

	return out;
#else /* CONFIG_TLS_INTERNAL_SERVER */
	return NULL;
#endif /* CONFIG_TLS_INTERNAL_SERVER */
}


struct wpabuf * tls_connection_encrypt(void *tls_ctx,
				       struct tls_connection *conn,
				       const struct wpabuf *in_data)
{
#ifdef CONFIG_TLS_INTERNAL_CLIENT
	if (conn->client) {
		struct wpabuf *buf;
		int res;
		buf = wpabuf_alloc(wpabuf_len(in_data) + 300);
		if (buf == NULL)
			return NULL;
		res = tlsv1_client_encrypt(conn->client, wpabuf_head(in_data),
					   wpabuf_len(in_data),
					   wpabuf_mhead(buf),
					   wpabuf_size(buf));
		if (res < 0) {
			wpabuf_free(buf);
			return NULL;
		}
		wpabuf_put(buf, res);
		return buf;
	}
#endif /* CONFIG_TLS_INTERNAL_CLIENT */
#ifdef CONFIG_TLS_INTERNAL_SERVER
	if (conn->server) {
		struct wpabuf *buf;
		int res;
		buf = wpabuf_alloc(wpabuf_len(in_data) + 300);
		if (buf == NULL)
			return NULL;
		res = tlsv1_server_encrypt(conn->server, wpabuf_head(in_data),
					   wpabuf_len(in_data),
					   wpabuf_mhead(buf),
					   wpabuf_size(buf));
		if (res < 0) {
			wpabuf_free(buf);
			return NULL;
		}
		wpabuf_put(buf, res);
		return buf;
	}
#endif /* CONFIG_TLS_INTERNAL_SERVER */
	return NULL;
}


struct wpabuf * tls_connection_decrypt(void *tls_ctx,
				       struct tls_connection *conn,
				       const struct wpabuf *in_data)
{
	return tls_connection_decrypt2(tls_ctx, conn, in_data, NULL);
}


struct wpabuf * tls_connection_decrypt2(void *tls_ctx,
					struct tls_connection *conn,
					const struct wpabuf *in_data,
					int *need_more_data)
{
	if (need_more_data)
		*need_more_data = 0;

#ifdef CONFIG_TLS_INTERNAL_CLIENT
	if (conn->client) {
		return tlsv1_client_decrypt(conn->client, wpabuf_head(in_data),
					    wpabuf_len(in_data),
					    need_more_data);
	}
#endif /* CONFIG_TLS_INTERNAL_CLIENT */
#ifdef CONFIG_TLS_INTERNAL_SERVER
	if (conn->server) {
		struct wpabuf *buf;
		int res;
		buf = wpabuf_alloc((wpabuf_len(in_data) + 500) * 3);
		if (buf == NULL)
			return NULL;
		res = tlsv1_server_decrypt(conn->server, wpabuf_head(in_data),
					   wpabuf_len(in_data),
					   wpabuf_mhead(buf),
					   wpabuf_size(buf));
		if (res < 0) {
			wpabuf_free(buf);
			return NULL;
		}
		wpabuf_put(buf, res);
		return buf;
	}
#endif /* CONFIG_TLS_INTERNAL_SERVER */
	return NULL;
}


int tls_connection_resumed(void *tls_ctx, struct tls_connection *conn)
{
#ifdef CONFIG_TLS_INTERNAL_CLIENT
	if (conn->client)
		return tlsv1_client_resumed(conn->client);
#endif /* CONFIG_TLS_INTERNAL_CLIENT */
#ifdef CONFIG_TLS_INTERNAL_SERVER
	if (conn->server)
		return tlsv1_server_resumed(conn->server);
#endif /* CONFIG_TLS_INTERNAL_SERVER */
	return -1;
}


int tls_connection_set_cipher_list(void *tls_ctx, struct tls_connection *conn,
				   u8 *ciphers)
{
#ifdef CONFIG_TLS_INTERNAL_CLIENT
	if (conn->client)
		return tlsv1_client_set_cipher_list(conn->client, ciphers);
#endif /* CONFIG_TLS_INTERNAL_CLIENT */
#ifdef CONFIG_TLS_INTERNAL_SERVER
	if (conn->server)
		return tlsv1_server_set_cipher_list(conn->server, ciphers);
#endif /* CONFIG_TLS_INTERNAL_SERVER */
	return -1;
}


int tls_get_cipher(void *tls_ctx, struct tls_connection *conn,
		   char *buf, size_t buflen)
{
	if (conn == NULL)
		return -1;
#ifdef CONFIG_TLS_INTERNAL_CLIENT
	if (conn->client)
		return tlsv1_client_get_cipher(conn->client, buf, buflen);
#endif /* CONFIG_TLS_INTERNAL_CLIENT */
#ifdef CONFIG_TLS_INTERNAL_SERVER
	if (conn->server)
		return tlsv1_server_get_cipher(conn->server, buf, buflen);
#endif /* CONFIG_TLS_INTERNAL_SERVER */
	return -1;
}


int tls_connection_enable_workaround(void *tls_ctx,
				     struct tls_connection *conn)
{
	return -1;
}


int tls_connection_client_hello_ext(void *tls_ctx, struct tls_connection *conn,
				    int ext_type, const u8 *data,
				    size_t data_len)
{
#ifdef CONFIG_TLS_INTERNAL_CLIENT
	if (conn->client) {
		return tlsv1_client_hello_ext(conn->client, ext_type,
					      data, data_len);
	}
#endif /* CONFIG_TLS_INTERNAL_CLIENT */
	return -1;
}


int tls_connection_get_failed(void *tls_ctx, struct tls_connection *conn)
{
	return 0;
}


int tls_connection_get_read_alerts(void *tls_ctx, struct tls_connection *conn)
{
	return 0;
}


int tls_connection_get_write_alerts(void *tls_ctx,
				    struct tls_connection *conn)
{
	return 0;
}


int tls_connection_get_keyblock_size(void *tls_ctx,
				     struct tls_connection *conn)
{
#ifdef CONFIG_TLS_INTERNAL_CLIENT
	if (conn->client)
		return tlsv1_client_get_keyblock_size(conn->client);
#endif /* CONFIG_TLS_INTERNAL_CLIENT */
#ifdef CONFIG_TLS_INTERNAL_SERVER
	if (conn->server)
		return tlsv1_server_get_keyblock_size(conn->server);
#endif /* CONFIG_TLS_INTERNAL_SERVER */
	return -1;
}


unsigned int tls_capabilities(void *tls_ctx)
{
	return 0;
}


int tls_connection_set_session_ticket_cb(void *tls_ctx,
					 struct tls_connection *conn,
					 tls_session_ticket_cb cb,
					 void *ctx)
{
#ifdef CONFIG_TLS_INTERNAL_CLIENT
	if (conn->client) {
		tlsv1_client_set_session_ticket_cb(conn->client, cb, ctx);
		return 0;
	}
#endif /* CONFIG_TLS_INTERNAL_CLIENT */
#ifdef CONFIG_TLS_INTERNAL_SERVER
	if (conn->server) {
		tlsv1_server_set_session_ticket_cb(conn->server, cb, ctx);
		return 0;
	}
#endif /* CONFIG_TLS_INTERNAL_SERVER */
	return -1;
}



/**
 * tls_prf_sha1_md5 - Pseudo-Random Function for TLS (TLS-PRF, RFC 2246)
 * @secret: Key for PRF
 * @secret_len: Length of the key in bytes
 * @label: A unique label for each purpose of the PRF
 * @seed: Seed value to bind into the key
 * @seed_len: Length of the seed
 * @out: Buffer for the generated pseudo-random key
 * @outlen: Number of bytes of key to generate
 * Returns: 0 on success, -1 on failure.
 *
 * This function is used to derive new, cryptographically separate keys from a
 * given key in TLS. This PRF is defined in RFC 2246, Chapter 5.
 */
int tls_prf_sha1_md5(const u8 *secret, size_t secret_len, const char *label,
		     const u8 *seed, size_t seed_len, u8 *out, size_t outlen)
{
	size_t L_S1, L_S2, i;
	const u8 *S1, *S2;
	u8 A_MD5[MD5_MAC_LEN], A_SHA1[SHA1_MAC_LEN];
	u8 P_MD5[MD5_MAC_LEN], P_SHA1[SHA1_MAC_LEN];
	int MD5_pos, SHA1_pos;
	const u8 *MD5_addr[3];
	size_t MD5_len[3];
	const unsigned char *SHA1_addr[3];
	size_t SHA1_len[3];

	if (secret_len & 1)
		return -1;

	MD5_addr[0] = A_MD5;
	MD5_len[0] = MD5_MAC_LEN;
	MD5_addr[1] = (unsigned char *) label;
	MD5_len[1] = os_strlen(label);
	MD5_addr[2] = seed;
	MD5_len[2] = seed_len;

	SHA1_addr[0] = A_SHA1;
	SHA1_len[0] = SHA1_MAC_LEN;
	SHA1_addr[1] = (unsigned char *) label;
	SHA1_len[1] = os_strlen(label);
	SHA1_addr[2] = seed;
	SHA1_len[2] = seed_len;

	/* RFC 2246, Chapter 5
	 * A(0) = seed, A(i) = HMAC(secret, A(i-1))
	 * P_hash = HMAC(secret, A(1) + seed) + HMAC(secret, A(2) + seed) + ..
	 * PRF = P_MD5(S1, label + seed) XOR P_SHA-1(S2, label + seed)
	 */

	L_S1 = L_S2 = (secret_len + 1) / 2;
	S1 = secret;
	S2 = secret + L_S1;
	if (secret_len & 1) {
		/* The last byte of S1 will be shared with S2 */
		S2--;
	}

	hmac_md5_vector(S1, L_S1, 2, &MD5_addr[1], &MD5_len[1], A_MD5);
	hmac_sha1_vector(S2, L_S2, 2, &SHA1_addr[1], &SHA1_len[1], A_SHA1);

	MD5_pos = MD5_MAC_LEN;
	SHA1_pos = SHA1_MAC_LEN;
	for (i = 0; i < outlen; i++) {
		if (MD5_pos == MD5_MAC_LEN) {
			hmac_md5_vector(S1, L_S1, 3, MD5_addr, MD5_len, P_MD5);
			MD5_pos = 0;
			hmac_md5(S1, L_S1, A_MD5, MD5_MAC_LEN, A_MD5);
		}
		if (SHA1_pos == SHA1_MAC_LEN) {
			hmac_sha1_vector(S2, L_S2, 3, SHA1_addr, SHA1_len,
					 P_SHA1);
			SHA1_pos = 0;
			hmac_sha1(S2, L_S2, A_SHA1, SHA1_MAC_LEN, A_SHA1);
		}

		out[i] = P_MD5[MD5_pos] ^ P_SHA1[SHA1_pos];

		MD5_pos++;
		SHA1_pos++;
	}

	return 0;
}
