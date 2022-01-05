/*
* coap_mbedtls.c -- Mbed TLS Datagram Transport Layer Support for libcoap
*
* Copyright (C) 2019-2021 Jon Shallow <supjps-libcoap@jpshallow.com>
*               2019 Jitin George <jitin@espressif.com>
*
 * SPDX-License-Identifier: BSD-2-Clause
 *
* This file is part of the CoAP library libcoap. Please see README for terms
* of use.
*/

/*
 * Naming used to prevent confusion between coap sessions, mbedtls sessions etc.
 * when reading the code.
 *
 * c_context  A coap_context_t *
 * c_session  A coap_session_t *
 * m_context  A coap_mbedtls_context_t * (held in c_context->dtls_context)
 * m_env      A coap_mbedtls_env_t * (held in c_session->tls)
 */

#include "coap3/coap_internal.h"

#ifdef HAVE_MBEDTLS

/*
 * This code can be conditionally compiled to remove some components if
 * they are not required to make a lighter footprint - all based on how
 * the mbedtls library has been built.  These are not defined within the
 * libcoap environment.
 *
 * MBEDTLS_SSL_SRV_C - defined for server side functionality
 * MBEDTLS_SSL_CLI_C - defined for client side functionality
 * MBEDTLS_SSL_PROTO_DTLS - defined for DTLS support
 * MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED - defined if PSK is to be supported
 * or MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED - defined if PSK is to be supported
 *
 * Note: TLS is not currently supported until additional code is added
 */

#include <mbedtls/version.h>
#include <mbedtls/platform.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/library/entropy_poll.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/error.h>
#include <mbedtls/certs.h>
#include <mbedtls/timing.h>
#include <mbedtls/ssl_cookie.h>
#include <mbedtls/oid.h>
#include <mbedtls/debug.h>
#include <mbedtls/sha256.h>
#if defined(ESPIDF_VERSION) && defined(CONFIG_MBEDTLS_DEBUG)
#include <mbedtls/esp_debug.h>
#endif /* ESPIDF_VERSION && CONFIG_MBEDTLS_DEBUG */
#include <errno.h>

#define mbedtls_malloc(a) malloc(a)
#define mbedtls_realloc(a,b) realloc(a,b)
#define mbedtls_strdup(a) strdup(a)
#define mbedtls_strndup(a,b) strndup(a,b)

#ifndef MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED
/* definition changed in later mbedtls code versions */
#ifdef MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED
#define MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED
#endif /* MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED */
#endif /* ! MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED */

#ifdef _WIN32
#define strcasecmp _stricmp
#endif

#define IS_PSK (1 << 0)
#define IS_PKI (1 << 1)
#define IS_CLIENT (1 << 6)
#define IS_SERVER (1 << 7)

typedef struct coap_ssl_t {
  const uint8_t *pdu;
  unsigned pdu_len;
  unsigned peekmode;
} coap_ssl_t;

/*
 * This structure encapsulates the Mbed TLS session object.
 * It handles both TLS and DTLS.
 * c_session->tls points to this.
 */
typedef struct coap_mbedtls_env_t {
  mbedtls_ssl_context ssl;
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context ctr_drbg;
  mbedtls_ssl_config conf;
  mbedtls_timing_delay_context timer;
  mbedtls_x509_crt cacert;
  mbedtls_x509_crt public_cert;
  mbedtls_pk_context private_key;
  mbedtls_ssl_cookie_ctx cookie_ctx;
  /* If not set, need to do do_mbedtls_handshake */
  int established;
  int seen_client_hello;
  coap_tick_t last_timeout;
  unsigned int retry_scalar;
  coap_ssl_t coap_ssl_data;
} coap_mbedtls_env_t;

typedef struct pki_sni_entry {
  char *sni;
  coap_dtls_key_t pki_key;
  mbedtls_x509_crt cacert;
  mbedtls_x509_crt public_cert;
  mbedtls_pk_context private_key;
} pki_sni_entry;

typedef struct psk_sni_entry {
  char* sni;
  coap_dtls_spsk_info_t psk_info;
} psk_sni_entry;

typedef struct coap_mbedtls_context_t {
  coap_dtls_pki_t setup_data;
  size_t pki_sni_count;
  pki_sni_entry *pki_sni_entry_list;
  size_t psk_sni_count;
  psk_sni_entry *psk_sni_entry_list;
  char *root_ca_file;
  char *root_ca_path;
  int psk_pki_enabled;
} coap_mbedtls_context_t;

typedef enum coap_enc_method_t {
  COAP_ENC_PSK,
  COAP_ENC_PKI,
} coap_enc_method_t;

int coap_rng(void *ctx, unsigned char *buf, size_t len)
{
	return mbedtls_hardware_poll(ctx, buf, len, NULL);
}

static int coap_dgram_read(void *ctx, unsigned char *out, size_t outl)
{
  ssize_t ret = 0;
  coap_session_t *c_session = (coap_session_t *)ctx;
  coap_ssl_t *data;

  if (!c_session->tls) {
    errno = EAGAIN;
    return MBEDTLS_ERR_SSL_WANT_READ;
  }
  data = &((coap_mbedtls_env_t *)c_session->tls)->coap_ssl_data;

  if (out != NULL) {
    if (data->pdu_len > 0) {
      if (outl < data->pdu_len) {
        memcpy(out, data->pdu, outl);
        ret = outl;
        data->pdu += outl;
        data->pdu_len -= outl;
      }
      else {
        memcpy(out, data->pdu, data->pdu_len);
        ret = data->pdu_len;
        if (!data->peekmode) {
          data->pdu_len = 0;
          data->pdu = NULL;
        }
      }
    }
    else {
      ret = MBEDTLS_ERR_SSL_WANT_READ;
      errno = EAGAIN;
    }
  }
  return ret;
}

/*
 * return +ve data amount
 *        0   no more
 *        -1  error (error in errno)
 */
/* callback function given to mbedtls for sending data over socket */
static int
coap_dgram_write(void *ctx, const unsigned char *send_buffer,
                 size_t send_buffer_length)
{
  ssize_t result = -1;
  coap_session_t *c_session = (coap_session_t *)ctx;

  if (c_session) {
    coap_mbedtls_env_t *m_env = (coap_mbedtls_env_t *)c_session->tls;
    result = coap_session_send(c_session, send_buffer, send_buffer_length);
    if (result != (ssize_t)send_buffer_length) {
      coap_log(LOG_WARNING, "coap_network_send failed (%zd != %zu)\n",
               result, send_buffer_length);
      result = 0;
    }
    else if (m_env) {
      coap_tick_t now;
      coap_ticks(&now);
      m_env->last_timeout = now;
    }
  } else {
    result = 0;
  }
  return result;
}

#if defined(MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED) && defined(MBEDTLS_SSL_SRV_C)
/*
 * Server side PSK callback
 */
static int psk_server_callback(void *p_info, mbedtls_ssl_context *ssl,
                  const unsigned char *name, size_t name_len )
{
  coap_session_t *c_session = (coap_session_t *)p_info;
  uint8_t buf[COAP_DTLS_MAX_PSK];
  size_t psk_len;
  coap_dtls_spsk_t *setup_data;
  coap_mbedtls_env_t *m_env;

  coap_log(LOG_DEBUG, "got psk_identity: '%.*s'\n",
                      (int)name_len, name);

  if (c_session == NULL || c_session->context == NULL ||
    c_session->context->get_server_psk == NULL) {
    return -1;
  }
  m_env = (coap_mbedtls_env_t *)c_session->tls;
  setup_data = &c_session->context->spsk_setup_data;

  if (setup_data->validate_id_call_back) {
    coap_bin_const_t lidentity;
    lidentity.length = name_len;
    lidentity.s = (const uint8_t*)name;
    const coap_bin_const_t *psk_key =
             setup_data->validate_id_call_back(&lidentity,
                                               c_session,
                                               setup_data->id_call_back_arg);

    if (psk_key == NULL)
      return -1;
    mbedtls_ssl_set_hs_psk(ssl, psk_key->s, psk_key->length);
    coap_session_refresh_psk_key(c_session, psk_key);
    m_env->seen_client_hello = 1;
    return 0;
  }

  psk_len = c_session->context->get_server_psk(c_session,
                               (const uint8_t*)name,
                               name_len,
                               (uint8_t*)buf, sizeof(buf));
  m_env->seen_client_hello = 1;
  mbedtls_ssl_set_hs_psk(ssl, buf, psk_len);
  return 0;
}
#endif /* MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED && MBEDTLS_SSL_SRV_C */

static char*
get_san_or_cn_from_cert(mbedtls_x509_crt *crt)
{
  if (crt) {
    const mbedtls_asn1_named_data * cn_data;

    if (crt->MBEDTLS_PRIVATE(ext_types) & MBEDTLS_X509_EXT_SUBJECT_ALT_NAME) {
      mbedtls_asn1_sequence *seq = &crt->MBEDTLS_PRIVATE(subject_alt_names);
      while (seq && seq->MBEDTLS_PRIVATE(buf).MBEDTLS_PRIVATE(p) == NULL) {
        seq = seq->MBEDTLS_PRIVATE(next);
      }
      if (seq) {
        /* Return the Subject Alt Name */
        return mbedtls_strndup((const char *)seq->MBEDTLS_PRIVATE(buf).MBEDTLS_PRIVATE(p),
                             seq->MBEDTLS_PRIVATE(buf).MBEDTLS_PRIVATE(len));
      }
    }

    cn_data = mbedtls_asn1_find_named_data(&crt->MBEDTLS_PRIVATE(subject),
                                           MBEDTLS_OID_AT_CN,
                                           MBEDTLS_OID_SIZE(MBEDTLS_OID_AT_CN));
    if (cn_data) {
      /* Return the Common Name */
      return mbedtls_strndup((const char *)cn_data->MBEDTLS_PRIVATE(val).MBEDTLS_PRIVATE(p),
                             cn_data->MBEDTLS_PRIVATE(val).MBEDTLS_PRIVATE(len));
    }
  }
  return NULL;
}

static char *
get_error_string(int ret) {
  static char buf[128] = {0};
  mbedtls_strerror(ret, buf, sizeof(buf)-1);
  return buf;
}

/*
 * return 0 All OK
 *        -ve Error Code
 */
static int
cert_verify_callback_mbedtls(void *data, mbedtls_x509_crt *crt,
                             int depth, uint32_t *flags)
{
  coap_session_t *c_session = (coap_session_t*)data;
  coap_mbedtls_context_t *m_context =
           (coap_mbedtls_context_t *)c_session->context->dtls_context;
  coap_dtls_pki_t *setup_data = &m_context->setup_data;
  char *cn = NULL;

  if (*flags == 0)
    return 0;

  cn = get_san_or_cn_from_cert(crt);

  if (*flags & MBEDTLS_X509_BADCERT_EXPIRED) {
    if (setup_data->allow_expired_certs) {
      *flags &= ~MBEDTLS_X509_BADCERT_EXPIRED;
      coap_log(LOG_INFO,
               "   %s: %s: overridden: '%s' depth %d\n",
               coap_session_str(c_session),
               "The certificate has expired", cn ? cn : "?", depth);
    }
  }
  if (*flags & MBEDTLS_X509_BADCERT_FUTURE) {
    if (setup_data->allow_expired_certs) {
      *flags &= ~MBEDTLS_X509_BADCERT_FUTURE;
      coap_log(LOG_INFO,
               "   %s: %s: overridden: '%s' depth %d\n",
               coap_session_str(c_session),
               "The certificate has a future date", cn ? cn : "?", depth);
    }
  }
  if (*flags & MBEDTLS_X509_BADCERT_BAD_MD) {
    if (setup_data->allow_bad_md_hash) {
      *flags &= ~MBEDTLS_X509_BADCERT_BAD_MD;
      coap_log(LOG_INFO,
               "   %s: %s: overridden: '%s' depth %d\n",
               coap_session_str(c_session),
               "The certificate has a bad MD hash", cn ? cn : "?", depth);
    }
  }
  if (*flags & MBEDTLS_X509_BADCERT_BAD_KEY) {
    if (setup_data->allow_short_rsa_length) {
      *flags &= ~MBEDTLS_X509_BADCERT_BAD_KEY;
      coap_log(LOG_INFO,
               "   %s: %s: overridden: '%s' depth %d\n",
               coap_session_str(c_session),
               "The certificate has a short RSA length", cn ? cn : "?", depth);
    }
  }
  if (*flags & MBEDTLS_X509_BADCERT_NOT_TRUSTED) {
    uint32_t lflags;
    int self_signed = !mbedtls_x509_crt_verify(crt, crt, NULL, NULL, &lflags,
                                               NULL, NULL);
    if (self_signed && depth == 0) {
      if (setup_data->allow_self_signed &&
          !setup_data->check_common_ca) {
        *flags &= ~MBEDTLS_X509_BADCERT_NOT_TRUSTED;
        coap_log(LOG_INFO,
                 "   %s: %s: overridden: '%s' depth %d\n",
                 coap_session_str(c_session),
                 "Self-signed",
                 cn ? cn : "?", depth);
      }
    }
    else {
      if (!setup_data->verify_peer_cert) {
        *flags &= ~MBEDTLS_X509_BADCERT_NOT_TRUSTED;
        coap_log(LOG_INFO,
                 "   %s: %s: overridden: '%s' depth %d\n",
                 coap_session_str(c_session),
                 "The certificate's CA does not match", cn ? cn : "?", depth);
      }
    }
  }
  if (*flags & MBEDTLS_X509_BADCRL_EXPIRED) {
    if (setup_data->check_cert_revocation && setup_data->allow_expired_crl) {
      *flags &= ~MBEDTLS_X509_BADCRL_EXPIRED;
      coap_log(LOG_INFO,
               "   %s: %s: overridden: '%s' depth %d\n",
               coap_session_str(c_session),
               "The certificate's CRL has expired", cn ? cn : "?", depth);
    }
    else if (!setup_data->check_cert_revocation) {
      *flags &= ~MBEDTLS_X509_BADCRL_EXPIRED;
    }
  }
  if (*flags & MBEDTLS_X509_BADCRL_FUTURE) {
    if (setup_data->check_cert_revocation && setup_data->allow_expired_crl) {
      *flags &= ~MBEDTLS_X509_BADCRL_FUTURE;
      coap_log(LOG_INFO,
               "   %s: %s: overridden: '%s' depth %d\n",
               coap_session_str(c_session),
               "The certificate's CRL has a future date", cn ? cn : "?", depth);
    }
    else if (!setup_data->check_cert_revocation) {
      *flags &= ~MBEDTLS_X509_BADCRL_FUTURE;
    }
  }
  if (setup_data->cert_chain_validation &&
      depth > (setup_data->cert_chain_verify_depth + 1)) {
    *flags |= MBEDTLS_X509_BADCERT_OTHER;
    coap_log(LOG_WARNING,
             "   %s: %s: '%s' depth %d\n",
             coap_session_str(c_session),
             "The certificate's verify depth is too long",
             cn ? cn : "?", depth);
  }

  if (*flags & MBEDTLS_X509_BADCERT_CN_MISMATCH) {
    *flags &= ~MBEDTLS_X509_BADCERT_CN_MISMATCH;
  }
  if (setup_data->validate_cn_call_back) {
    if (!setup_data->validate_cn_call_back(cn,
           crt->MBEDTLS_PRIVATE(raw).MBEDTLS_PRIVATE(p),
           crt->MBEDTLS_PRIVATE(raw).MBEDTLS_PRIVATE(len),
           c_session,
           depth,
           *flags == 0,
           setup_data->cn_call_back_arg)) {
      *flags |= MBEDTLS_X509_BADCERT_CN_MISMATCH;
    }
  }
  if (*flags != 0) {
    char buf[128];
    char *tcp;
    int ret = mbedtls_x509_crt_verify_info(buf, sizeof(buf), "", *flags);

    if (ret >= 0) {
      tcp = strchr(buf, '\n');
      while (tcp) {
        *tcp = '\000';
        coap_log(LOG_WARNING,
                 "   %s: %s: issue 0x%x: '%s' depth %d\n",
                 coap_session_str(c_session),
                 buf, *flags, cn ? cn : "?", depth);
        tcp = strchr(tcp+1, '\n');
      }
    }
    else {
      coap_log(LOG_ERR, "mbedtls_x509_crt_verify_info returned -0x%x: '%s'\n",
               -ret, get_error_string(ret));
    }
  }

  if (cn)
    mbedtls_free(cn);

  return 0;
}

static int
setup_pki_credentials(mbedtls_x509_crt *cacert,
                      mbedtls_x509_crt *public_cert,
                      mbedtls_pk_context *private_key,
                      coap_mbedtls_env_t *m_env,
                      coap_mbedtls_context_t *m_context,
                      coap_session_t *c_session,
                      coap_dtls_pki_t *setup_data,
                      coap_dtls_role_t role)
{
  int ret;

  if (setup_data->is_rpk_not_cert) {
    coap_log(LOG_ERR,
          "RPK Support not available in Mbed TLS\n");
    return -1;
  }
  switch (setup_data->pki_key.key_type) {
  case COAP_PKI_KEY_PEM:
    if (setup_data->pki_key.key.pem.public_cert &&
        setup_data->pki_key.key.pem.public_cert[0] &&
        setup_data->pki_key.key.pem.private_key &&
        setup_data->pki_key.key.pem.private_key[0]) {

      mbedtls_x509_crt_init(public_cert);
      mbedtls_pk_init(private_key);

      ret = mbedtls_x509_crt_parse_file(public_cert,
                                    setup_data->pki_key.key.pem.public_cert);
      if (ret < 0) {
        coap_log(LOG_ERR, "mbedtls_x509_crt_parse_file returned -0x%x: '%s'\n",
                 -ret, get_error_string(ret));
        return ret;
      }

      ret = mbedtls_pk_parse_keyfile(private_key,
                              setup_data->pki_key.key.pem.private_key, NULL, coap_rng, NULL);
      if (ret < 0) {
        coap_log(LOG_ERR, "mbedtls_pk_parse_keyfile returned -0x%x: '%s'\n",
                 -ret, get_error_string(ret));
        return ret;
      }

      ret = mbedtls_ssl_conf_own_cert(&m_env->conf, public_cert, private_key);
      if (ret < 0) {
        coap_log(LOG_ERR, "mbedtls_ssl_conf_own_cert returned -0x%x: '%s'\n",
                 -ret, get_error_string(ret));
        return ret;
      }
    }
    else if (role == COAP_DTLS_ROLE_SERVER) {
      coap_log(LOG_ERR,
               "***setup_pki: (D)TLS: No Server Certificate + Private "
               "Key defined\n");
      return -1;
    }

    if (setup_data->pki_key.key.pem.ca_file &&
        setup_data->pki_key.key.pem.ca_file[0]) {
      mbedtls_x509_crt_init(cacert);
      ret = mbedtls_x509_crt_parse_file(cacert,
                                        setup_data->pki_key.key.pem.ca_file);
      if (ret < 0) {
        coap_log(LOG_ERR, "mbedtls_x509_crt_parse returned -0x%x: '%s'\n",
                 -ret, get_error_string(ret));
        return ret;
      }
      mbedtls_ssl_conf_ca_chain(&m_env->conf, cacert, NULL);
    }
    break;
  case COAP_PKI_KEY_PEM_BUF:
    if (setup_data->pki_key.key.pem_buf.public_cert &&
        setup_data->pki_key.key.pem_buf.public_cert_len &&
        setup_data->pki_key.key.pem_buf.private_key &&
        setup_data->pki_key.key.pem_buf.private_key_len) {
      uint8_t *buffer;
      size_t length;

      mbedtls_x509_crt_init(public_cert);
      mbedtls_pk_init(private_key);

      length = setup_data->pki_key.key.pem_buf.public_cert_len;
      if (setup_data->pki_key.key.pem_buf.public_cert[length-1] != '\000') {
        /* Need to allocate memory to add in NULL terminator */
        buffer = mbedtls_malloc(length + 1);
        if (!buffer) {
          coap_log(LOG_ERR, "mbedtls_malloc failed\n");
          return MBEDTLS_ERR_SSL_ALLOC_FAILED;
        }
        memcpy(buffer, setup_data->pki_key.key.pem_buf.public_cert, length);
        buffer[length] = '\000';
        length++;
        ret = mbedtls_x509_crt_parse(public_cert, buffer, length);
        mbedtls_free(buffer);
      }
      else {
        ret = mbedtls_x509_crt_parse(public_cert,
              setup_data->pki_key.key.pem_buf.public_cert,
              setup_data->pki_key.key.pem_buf.public_cert_len);
      }
      if (ret < 0) {
        coap_log(LOG_ERR, "mbedtls_x509_crt_parse returned -0x%x: '%s'\n",
                 -ret, get_error_string(ret));
        return ret;
      }

      length = setup_data->pki_key.key.pem_buf.private_key_len;
      if (setup_data->pki_key.key.pem_buf.private_key[length-1] != '\000') {
        /* Need to allocate memory to add in NULL terminator */
        buffer = mbedtls_malloc(length + 1);
        if (!buffer) {
          coap_log(LOG_ERR, "mbedtls_malloc failed\n");
          return MBEDTLS_ERR_SSL_ALLOC_FAILED;
        }
        memcpy(buffer, setup_data->pki_key.key.pem_buf.private_key, length);
        buffer[length] = '\000';
        length++;
        ret = mbedtls_pk_parse_key(private_key, buffer, length, NULL, 0, coap_rng, NULL);
        mbedtls_free(buffer);
      }
      else {
        ret = mbedtls_pk_parse_key(private_key,
              setup_data->pki_key.key.pem_buf.private_key,
              setup_data->pki_key.key.pem_buf.private_key_len, NULL, 0, coap_rng, NULL);
      }
      if (ret < 0) {
        coap_log(LOG_ERR, "mbedtls_pk_parse_keyfile returned -0x%x: '%s'\n",
                 -ret, get_error_string(ret));
        return ret;
      }

      ret = mbedtls_ssl_conf_own_cert(&m_env->conf, public_cert, private_key);
      if (ret < 0) {
        coap_log(LOG_ERR, "mbedtls_ssl_conf_own_cert returned -0x%x: '%s'\n",
                 -ret, get_error_string(ret));
        return ret;
      }
    } else if (role == COAP_DTLS_ROLE_SERVER) {
      coap_log(LOG_ERR,
              "***setup_pki: (D)TLS: No Server Certificate + Private "
              "Key defined\n");
      return -1;
    }

    if (setup_data->pki_key.key.pem_buf.ca_cert &&
        setup_data->pki_key.key.pem_buf.ca_cert_len) {
      uint8_t *buffer;
      size_t length;

      mbedtls_x509_crt_init(cacert);
      length = setup_data->pki_key.key.pem_buf.ca_cert_len;
      if (setup_data->pki_key.key.pem_buf.ca_cert[length-1] != '\000') {
        /* Need to allocate memory to add in NULL terminator */
        buffer = mbedtls_malloc(length + 1);
        if (!buffer) {
          coap_log(LOG_ERR, "mbedtls_malloc failed\n");
          return MBEDTLS_ERR_SSL_ALLOC_FAILED;
        }
        memcpy(buffer, setup_data->pki_key.key.pem_buf.ca_cert, length);
        buffer[length] = '\000';
        length++;
        ret = mbedtls_x509_crt_parse(cacert, buffer, length);
        mbedtls_free(buffer);
      }
      else {
        ret = mbedtls_x509_crt_parse(cacert,
                setup_data->pki_key.key.pem_buf.ca_cert,
                setup_data->pki_key.key.pem_buf.ca_cert_len);
      }
      if (ret < 0) {
        coap_log(LOG_ERR, "mbedtls_x509_crt_parse returned -0x%x: '%s'\n",
                 -ret, get_error_string(ret));
        return ret;
      }
      mbedtls_ssl_conf_ca_chain(&m_env->conf, cacert, NULL);
    }
    break;
  case COAP_PKI_KEY_ASN1:
    if (setup_data->pki_key.key.asn1.public_cert &&
        setup_data->pki_key.key.asn1.public_cert_len &&
        setup_data->pki_key.key.asn1.private_key &&
        setup_data->pki_key.key.asn1.private_key_len > 0) {

      mbedtls_x509_crt_init(public_cert);
      mbedtls_pk_init(private_key);
      ret = mbedtls_x509_crt_parse(public_cert,
              (const unsigned char *)setup_data->pki_key.key.asn1.public_cert,
              setup_data->pki_key.key.asn1.public_cert_len);
      if (ret < 0) {
        coap_log(LOG_ERR, "mbedtls_x509_crt_parse returned -0x%x: '%s'\n",
                 -ret, get_error_string(ret));
        return ret;
      }

      ret = mbedtls_pk_parse_key(private_key,
              (const unsigned char *)setup_data->pki_key.key.asn1.private_key,
              setup_data->pki_key.key.asn1.private_key_len, NULL, 0, coap_rng, NULL);
      if (ret < 0) {
        coap_log(LOG_ERR, "mbedtls_pk_parse_keyfile returned -0x%x: '%s'\n",
                 -ret, get_error_string(ret));
        return ret;
      }

      ret = mbedtls_ssl_conf_own_cert(&m_env->conf, public_cert, private_key);
      if (ret < 0) {
        coap_log(LOG_ERR, "mbedtls_ssl_conf_own_cert returned -0x%x: '%s'\n",
                 -ret, get_error_string(ret));
        return ret;
      }
    } else if (role == COAP_DTLS_ROLE_SERVER) {
      coap_log(LOG_ERR,
               "***setup_pki: (D)TLS: No Server Certificate + Private "
               "Key defined\n");
      return -1;
    }

    if (setup_data->pki_key.key.asn1.ca_cert &&
        setup_data->pki_key.key.asn1.ca_cert_len > 0) {
      mbedtls_x509_crt_init(cacert);
      ret = mbedtls_x509_crt_parse(cacert,
                  (const unsigned char *)setup_data->pki_key.key.asn1.ca_cert,
                  setup_data->pki_key.key.asn1.ca_cert_len);
      if (ret < 0) {
        coap_log(LOG_ERR, "mbedtls_x509_crt_parse returned -0x%x: '%s'\n",
                 -ret, get_error_string(ret));
        return ret;
      }
      mbedtls_ssl_conf_ca_chain(&m_env->conf, cacert, NULL);
    }
    break;

  case COAP_PKI_KEY_PKCS11:
    coap_log(LOG_ERR,
             "***setup_pki: (D)TLS: PKCS11 not currently supported\n");
    return -1;

  default:
    coap_log(LOG_ERR,
             "***setup_pki: (D)TLS: Unknown key type %d\n",
             setup_data->pki_key.key_type);
    return -1;
  }

  if (m_context->root_ca_file) {
    ret = mbedtls_x509_crt_parse_file(cacert, m_context->root_ca_file);
    if (ret < 0) {
      coap_log(LOG_ERR, "mbedtls_x509_crt_parse returned -0x%x: '%s'\n",
               -ret, get_error_string(ret));
      return ret;
    }
    mbedtls_ssl_conf_ca_chain(&m_env->conf, cacert, NULL);
  }
  if (m_context->root_ca_path) {
    ret = mbedtls_x509_crt_parse_file(cacert, m_context->root_ca_path);
    if (ret < 0) {
      coap_log(LOG_ERR, "mbedtls_x509_crt_parse returned -0x%x: '%s'\n",
               -ret, get_error_string(ret));
      return ret;
    }
    mbedtls_ssl_conf_ca_chain(&m_env->conf, cacert, NULL);
  }

#if defined(MBEDTLS_SSL_SRV_C)
  mbedtls_ssl_conf_cert_req_ca_list(&m_env->conf,
                                    setup_data->check_common_ca ?
                                      MBEDTLS_SSL_CERT_REQ_CA_LIST_ENABLED :
                                      MBEDTLS_SSL_CERT_REQ_CA_LIST_DISABLED);
#endif
  mbedtls_ssl_conf_authmode(&m_env->conf, setup_data->verify_peer_cert ?
                                           MBEDTLS_SSL_VERIFY_REQUIRED :
                                           MBEDTLS_SSL_VERIFY_NONE);
  /*
   * Verify Peer.
   *  Need to do all checking, even if setup_data->verify_peer_cert is not set
   */
  mbedtls_ssl_conf_verify(&m_env->conf,
                          cert_verify_callback_mbedtls, c_session);

  return 0;
}

#if defined(MBEDTLS_SSL_SRV_C)
/*
 * PKI SNI callback.
 */
static int
pki_sni_callback(void *p_info, mbedtls_ssl_context *ssl,
             const unsigned char *uname, size_t name_len)
{
  unsigned int i;
  coap_dtls_pki_t sni_setup_data;
  coap_session_t *c_session = (coap_session_t *)p_info;
  coap_mbedtls_env_t *m_env = (coap_mbedtls_env_t *)c_session->tls;
  coap_mbedtls_context_t *m_context =
           (coap_mbedtls_context_t *)c_session->context->dtls_context;
  int ret = 0;
  char *name;

  name = mbedtls_malloc(name_len+1);
  if (!name)
    return -1;

  memcpy(name, uname, name_len);
  name[name_len] = '\000';

  /* Is this a cached entry? */
  for (i = 0; i < m_context->pki_sni_count; i++) {
    if (strcasecmp(name, m_context->pki_sni_entry_list[i].sni) == 0) {
      break;
    }
  }
  if (i == m_context->pki_sni_count) {
    /*
     * New PKI SNI request
     */
    coap_dtls_key_t *new_entry;
    pki_sni_entry *pki_sni_entry_list;

    new_entry =
      m_context->setup_data.validate_sni_call_back(name,
                                 m_context->setup_data.sni_call_back_arg);
    if (!new_entry) {
      mbedtls_free(name);
      return -1;
    }

    pki_sni_entry_list = mbedtls_realloc(m_context->pki_sni_entry_list,
                                         (i+1)*sizeof(pki_sni_entry));

    if (pki_sni_entry_list == NULL) {
      mbedtls_free(name);
      return -1;
    }
    m_context->pki_sni_entry_list = pki_sni_entry_list;
    memset(&m_context->pki_sni_entry_list[i], 0,
           sizeof(m_context->pki_sni_entry_list[i]));
    m_context->pki_sni_entry_list[i].sni = name;
    m_context->pki_sni_entry_list[i].pki_key = *new_entry;
    sni_setup_data = m_context->setup_data;
    sni_setup_data.pki_key = *new_entry;
    if ((ret = setup_pki_credentials(&m_context->pki_sni_entry_list[i].cacert,
                         &m_context->pki_sni_entry_list[i].public_cert,
                         &m_context->pki_sni_entry_list[i].private_key,
                         m_env,
                         m_context,
                         c_session,
                         &sni_setup_data, COAP_DTLS_ROLE_SERVER)) < 0) {
      mbedtls_free(name);
      return -1;
    }
    /* name has been absorbed into pki_sni_entry_list[].sni entry */
    m_context->pki_sni_count++;
  }
  else {
    mbedtls_free(name);
  }

  mbedtls_ssl_set_hs_ca_chain(ssl, &m_context->pki_sni_entry_list[i].cacert,
                              NULL);
  return mbedtls_ssl_set_hs_own_cert(ssl,
                              &m_context->pki_sni_entry_list[i].public_cert,
                              &m_context->pki_sni_entry_list[i].private_key);
}

#if defined(MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED)
/*
 * PSK SNI callback.
 */
static int
psk_sni_callback(void *p_info, mbedtls_ssl_context *ssl,
             const unsigned char *uname, size_t name_len)
{
  unsigned int i;
  coap_session_t *c_session = (coap_session_t *)p_info;
  coap_mbedtls_context_t *m_context =
           (coap_mbedtls_context_t *)c_session->context->dtls_context;
  char *name;

  name = mbedtls_malloc(name_len+1);
  if (!name)
    return -1;

  memcpy(name, uname, name_len);
  name[name_len] = '\000';

  /* Is this a cached entry? */
  for (i = 0; i < m_context->psk_sni_count; i++) {
    if (strcasecmp(name, m_context->psk_sni_entry_list[i].sni) == 0) {
      break;
    }
  }
  if (i == m_context->psk_sni_count) {
    /*
     * New PSK SNI request
     */
    const coap_dtls_spsk_info_t *new_entry;
    psk_sni_entry *psk_sni_entry_list;

    new_entry =
      c_session->context->spsk_setup_data.validate_sni_call_back(name,
                      c_session,
                      c_session->context->spsk_setup_data.sni_call_back_arg);
    if (!new_entry) {
      mbedtls_free(name);
      return -1;
    }

    psk_sni_entry_list = mbedtls_realloc(m_context->psk_sni_entry_list,
                                         (i+1)*sizeof(psk_sni_entry));

    if (psk_sni_entry_list == NULL) {
      mbedtls_free(name);
      return -1;
    }
    m_context->psk_sni_entry_list = psk_sni_entry_list;
    m_context->psk_sni_entry_list[i].sni = name;
    m_context->psk_sni_entry_list[i].psk_info = *new_entry;
    /* name has been absorbed into psk_sni_entry_list[].sni entry */
    m_context->psk_sni_count++;
  }
  else {
    mbedtls_free(name);
  }

  coap_session_refresh_psk_hint(c_session,
                          &m_context->psk_sni_entry_list[i].psk_info.hint);
  coap_session_refresh_psk_key(c_session,
                          &m_context->psk_sni_entry_list[i].psk_info.key);
  return mbedtls_ssl_set_hs_psk(ssl,
                       m_context->psk_sni_entry_list[i].psk_info.key.s,
                       m_context->psk_sni_entry_list[i].psk_info.key.length);
}
#endif /* MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED */

static int setup_server_ssl_session(coap_session_t *c_session,
                                    coap_mbedtls_env_t *m_env)
{
  coap_mbedtls_context_t *m_context =
           (coap_mbedtls_context_t *)c_session->context->dtls_context;
  int ret = 0;
  m_context->psk_pki_enabled |= IS_SERVER;

  mbedtls_ssl_cookie_init(&m_env->cookie_ctx);
  if ((ret = mbedtls_ssl_config_defaults(&m_env->conf,
                  MBEDTLS_SSL_IS_SERVER,
                  c_session->proto == COAP_PROTO_DTLS ?
                   MBEDTLS_SSL_TRANSPORT_DATAGRAM :
                   MBEDTLS_SSL_TRANSPORT_STREAM,
                  MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
    coap_log(LOG_ERR, "mbedtls_ssl_config_defaults returned -0x%x: '%s'\n",
             -ret, get_error_string(ret));
    goto fail;
  }

  mbedtls_ssl_conf_rng(&m_env->conf, mbedtls_ctr_drbg_random, &m_env->ctr_drbg);

#if defined(MBEDTLS_SSL_PROTO_DTLS)
  mbedtls_ssl_conf_handshake_timeout(&m_env->conf, COAP_DTLS_RETRANSMIT_MS,
                                     COAP_DTLS_RETRANSMIT_TOTAL_MS);
#endif /* MBEDTLS_SSL_PROTO_DTLS */

  if (m_context->psk_pki_enabled & IS_PSK) {
#if defined(MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED)
    mbedtls_ssl_conf_psk_cb(&m_env->conf, psk_server_callback, c_session);
    if (c_session->context->spsk_setup_data.validate_sni_call_back) {
      mbedtls_ssl_conf_sni(&m_env->conf, psk_sni_callback, c_session);
    }
#else /* MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED */
    coap_log(LOG_WARNING, "PSK not enabled in Mbed TLS library\n");
#endif /* MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED */
  }

  if (m_context->psk_pki_enabled & IS_PKI) {
    ret = setup_pki_credentials(&m_env->cacert, &m_env->public_cert,
                                &m_env->private_key, m_env, m_context,
                                c_session, &m_context->setup_data,
                                COAP_DTLS_ROLE_SERVER);
    if (ret < 0) {
      coap_log(LOG_ERR, "PKI setup failed\n");
      return ret;
    }
    if (m_context->setup_data.validate_sni_call_back) {
      mbedtls_ssl_conf_sni(&m_env->conf, pki_sni_callback, c_session);
    }
  }

  if ((ret = mbedtls_ssl_cookie_setup(&m_env->cookie_ctx,
                                  mbedtls_ctr_drbg_random,
                                  &m_env->ctr_drbg)) != 0) {
    coap_log(LOG_ERR, "mbedtls_ssl_cookie_setup: returned -0x%x: '%s'\n",
             -ret, get_error_string(ret));
    goto fail;
  }

#if defined(MBEDTLS_SSL_PROTO_DTLS)
  mbedtls_ssl_conf_dtls_cookies(&m_env->conf, mbedtls_ssl_cookie_write,
                                mbedtls_ssl_cookie_check,
                                &m_env->cookie_ctx );
#if MBEDTLS_VERSION_NUMBER >= 0x02100100
  mbedtls_ssl_set_mtu(&m_env->ssl, (uint16_t)c_session->mtu);
#endif /* MBEDTLS_VERSION_NUMBER >= 0x02100100 */
#endif /* MBEDTLS_SSL_PROTO_DTLS */
fail:
  return ret;
}
#endif /* MBEDTLS_SSL_SRV_C */

static int *psk_ciphers = NULL;
static int *pki_ciphers = NULL;
static int processed_ciphers = 0;

static void
set_ciphersuites(mbedtls_ssl_config *conf, coap_enc_method_t method)
{
  if (!processed_ciphers) {
    const int *list = mbedtls_ssl_list_ciphersuites();
    const int *base = list;
    int *psk_list;
    int *pki_list;
    int psk_count = 1; /* account for empty terminator */
    int pki_count = 1;

    while (*list) {
      const mbedtls_ssl_ciphersuite_t *cur =
                                     mbedtls_ssl_ciphersuite_from_id(*list);

      if (cur) {
        if (cur->MBEDTLS_PRIVATE(max_minor_ver) < MBEDTLS_SSL_MINOR_VERSION_3) {
          /* Minimum of TLS1.2 required - skip */
        }
#if defined(MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED)
        else if (mbedtls_ssl_ciphersuite_uses_psk(cur)) {
          psk_count++;
        }
#endif /* MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED */
        else {
          pki_count++;
        }
      }
      list++;
    }
    list = base;

    psk_ciphers = mbedtls_malloc(psk_count * sizeof(psk_ciphers[0]));
    if (psk_ciphers == NULL) {
      coap_log(LOG_ERR, "set_ciphers: mbedtls_malloc with count %d failed\n", psk_count);
      return;
    }
    pki_ciphers = mbedtls_malloc(pki_count * sizeof(pki_ciphers[0]));
    if (pki_ciphers == NULL) {
      coap_log(LOG_ERR, "set_ciphers: mbedtls_malloc with count %d failed\n", pki_count);
      return;
    }

    psk_list = psk_ciphers;
    pki_list = pki_ciphers;

    while (*list) {
      const mbedtls_ssl_ciphersuite_t *cur =
                                     mbedtls_ssl_ciphersuite_from_id(*list);
      if (cur) {
        if (cur->MBEDTLS_PRIVATE(max_minor_ver) < MBEDTLS_SSL_MINOR_VERSION_3) {
          /* Minimum of TLS1.2 required - skip */
        }
#if defined(MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED)
        else if (mbedtls_ssl_ciphersuite_uses_psk(cur)) {
          *psk_list = *list;
          psk_list++;
        }
#endif /* MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED */
        else {
          *pki_list = *list;
          pki_list++;
        }
      }
      list++;
    }
    /* zero terminate */
    *psk_list = 0;
    *pki_list = 0;
    processed_ciphers = 1;
  }
  mbedtls_ssl_conf_ciphersuites(conf, method == COAP_ENC_PSK ? psk_ciphers : pki_ciphers);
}

static int setup_client_ssl_session(coap_session_t *c_session,
                                    coap_mbedtls_env_t *m_env)
{
  int ret;

  coap_mbedtls_context_t *m_context =
           (coap_mbedtls_context_t *)c_session->context->dtls_context;

  m_context->psk_pki_enabled |= IS_CLIENT;

  if ((ret = mbedtls_ssl_config_defaults(&m_env->conf,
                  MBEDTLS_SSL_IS_CLIENT,
                  c_session->proto == COAP_PROTO_DTLS ?
                   MBEDTLS_SSL_TRANSPORT_DATAGRAM :
                   MBEDTLS_SSL_TRANSPORT_STREAM,
                  MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
      coap_log(LOG_ERR, "mbedtls_ssl_config_defaults returned -0x%x: '%s'\n",
               -ret, get_error_string(ret));
      goto fail;
  }

#if defined(MBEDTLS_SSL_PROTO_DTLS)
  mbedtls_ssl_conf_handshake_timeout(&m_env->conf, COAP_DTLS_RETRANSMIT_MS,
                                     COAP_DTLS_RETRANSMIT_TOTAL_MS);
#endif /* MBEDTLS_SSL_PROTO_DTLS */

  mbedtls_ssl_conf_authmode(&m_env->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
  mbedtls_ssl_conf_rng(&m_env->conf, mbedtls_ctr_drbg_random, &m_env->ctr_drbg);

  if (m_context->psk_pki_enabled & IS_PSK) {
#if defined(MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED)
    uint8_t identity[COAP_DTLS_MAX_PSK_IDENTITY];
    size_t identity_len;
    uint8_t psk_key[COAP_DTLS_MAX_PSK];
    size_t psk_len;
    size_t max_identity_len = sizeof(identity);

    coap_log(LOG_INFO, "Setting PSK key\n");
    psk_len = c_session->context->get_client_psk(c_session,
                                             NULL,
                                             0,
                                             identity,
                                             &identity_len,
                                             max_identity_len,
                                             psk_key,
                                             sizeof(psk_key));
    assert(identity_len <= sizeof(identity));
    mbedtls_ssl_conf_psk(&m_env->conf, (const unsigned char *)psk_key,
                         psk_len, (const unsigned char *)identity,
                         identity_len);
    if (c_session->cpsk_setup_data.client_sni) {
      mbedtls_ssl_set_hostname(&m_env->ssl,
                               c_session->cpsk_setup_data.client_sni);
    }
    /* Identity Hint currently not supported in Mbed TLS so code removed */

    set_ciphersuites(&m_env->conf, COAP_ENC_PSK);
#else /* MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED */
    coap_log(LOG_WARNING, "PSK not enabled in Mbed TLS library\n");
#endif /* ! MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED */
  }
  else if ((m_context->psk_pki_enabled & IS_PKI) ||
           (m_context->psk_pki_enabled & (IS_PSK | IS_PKI)) == 0) {
    /*
     * If neither PSK or PKI have been set up, use PKI basics.
     * This works providing COAP_PKI_KEY_PEM has a value of 0.
     */
    mbedtls_ssl_conf_authmode(&m_env->conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
    ret = setup_pki_credentials(&m_env->cacert, &m_env->public_cert,
                                &m_env->private_key, m_env, m_context,
                                c_session, &m_context->setup_data,
                                COAP_DTLS_ROLE_CLIENT);
    if (ret < 0) {
      coap_log(LOG_ERR, "PKI setup failed\n");
      return ret;
    }
#if defined(MBEDTLS_SSL_SRV_C) && defined(MBEDTLS_SSL_ALPN)
    if (c_session->proto == COAP_PROTO_TLS) {
      const char *alpn_list[] = { "coap", NULL };

      ret = mbedtls_ssl_conf_alpn_protocols(&m_env->conf, alpn_list);
      if (ret != 0) {
        coap_log(LOG_ERR, "ALPN setup failed %d)\n", ret);
      }
    }
#endif /* MBEDTLS_SSL_SRV_C && MBEDTLS_SSL_ALPN */
    if (m_context->setup_data.client_sni) {
      mbedtls_ssl_set_hostname(&m_env->ssl, m_context->setup_data.client_sni);
    }
#if defined(MBEDTLS_SSL_PROTO_DTLS)
#if MBEDTLS_VERSION_NUMBER >= 0x02100100
    mbedtls_ssl_set_mtu(&m_env->ssl, (uint16_t)c_session->mtu);
#endif /* MBEDTLS_VERSION_NUMBER >= 0x02100100 */
#endif /* MBEDTLS_SSL_PROTO_DTLS */
    set_ciphersuites(&m_env->conf, COAP_ENC_PKI);
  }
  return 0;

fail:
  return ret;
}

static void mbedtls_cleanup(coap_mbedtls_env_t *m_env)
{
  if (!m_env) {
    return;
  }

  mbedtls_x509_crt_free(&m_env->cacert);
  mbedtls_x509_crt_free(&m_env->public_cert);
  mbedtls_pk_free(&m_env->private_key);
  mbedtls_entropy_free(&m_env->entropy);
  mbedtls_ssl_config_free(&m_env->conf);
  mbedtls_ctr_drbg_free(&m_env->ctr_drbg);
  mbedtls_ssl_free(&m_env->ssl);
  mbedtls_ssl_cookie_free(&m_env->cookie_ctx);
}

static void
coap_dtls_free_mbedtls_env(coap_mbedtls_env_t *m_env) {
  if (m_env) {
    mbedtls_cleanup(m_env);
    mbedtls_free(m_env);
  }
}

static const char *
report_mbedtls_alert(unsigned char alert) {
  switch (alert) {
  case MBEDTLS_SSL_ALERT_MSG_BAD_RECORD_MAC: return ": Bad Record MAC";
  case MBEDTLS_SSL_ALERT_MSG_HANDSHAKE_FAILURE: return ": Handshake failure";
  case MBEDTLS_SSL_ALERT_MSG_NO_CERT: return ": No Certificate provided";
  case MBEDTLS_SSL_ALERT_MSG_BAD_CERT: return ": Certificate is bad";
  case MBEDTLS_SSL_ALERT_MSG_UNKNOWN_CA: return ": CA is unknown";
  case MBEDTLS_SSL_ALERT_MSG_ACCESS_DENIED: return ": Access was denied";
  case MBEDTLS_SSL_ALERT_MSG_DECRYPT_ERROR: return ": Decrypt error";
  default: return "";
  }
}

/*
 * return -1  failure
 *         0  not completed
 *         1  established
 */
static int do_mbedtls_handshake(coap_session_t *c_session,
                                coap_mbedtls_env_t *m_env) {
  int ret;
  int alert;

  ret = mbedtls_ssl_handshake(&m_env->ssl);
  switch (ret) {
  case 0:
    m_env->established = 1;
    coap_log(LOG_DEBUG, "*  %s: Mbed TLS established\n",
                                            coap_session_str(c_session));
    ret = 1;
    break;
  case MBEDTLS_ERR_SSL_WANT_READ:
  case MBEDTLS_ERR_SSL_WANT_WRITE:
    errno = EAGAIN;
    ret = 0;
    break;
  case MBEDTLS_ERR_SSL_HELLO_VERIFY_REQUIRED:
    coap_log(LOG_DEBUG, "hello verification requested\n");
    goto reset;
  case MBEDTLS_ERR_SSL_INVALID_MAC:
    goto fail;
  case MBEDTLS_ERR_SSL_NO_CLIENT_CERTIFICATE:
    alert = MBEDTLS_SSL_ALERT_MSG_NO_CERT;
    goto fail_alert;
  /* FIXME: SSLv2 ClientHello deprecated from mbedTLS v3.0
   * NOTE: from mbedtls-3.0 migration guide
   * This doesn't affect people using the default configuration as it was already disabled by default.
   * This only affects TLS servers that have clients who send an SSLv2 ClientHello.
   * These days clients are very unlikely to do that. If you have a client that does,
   * please try contacting them and encouraging them to upgrade their software.
   *
   * case MBEDTLS_ERR_SSL_BAD_HS_CLIENT_HELLO:
   * alert = MBEDTLS_SSL_ALERT_MSG_HANDSHAKE_FAILURE;
   * goto fail_alert;
   */
  case MBEDTLS_ERR_X509_CERT_VERIFY_FAILED:
    goto fail;
  case MBEDTLS_ERR_SSL_FATAL_ALERT_MESSAGE:
    if (m_env->ssl.MBEDTLS_PRIVATE(in_msg)[1] != MBEDTLS_SSL_ALERT_MSG_CLOSE_NOTIFY)
      coap_log(LOG_WARNING, "***%s: Alert '%d'%s\n",
               coap_session_str(c_session), m_env->ssl.MBEDTLS_PRIVATE(in_msg)[1],
               report_mbedtls_alert(m_env->ssl.MBEDTLS_PRIVATE(in_msg)[1]));
    /* Fall through */
  case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
    c_session->dtls_event = COAP_EVENT_DTLS_CLOSED;
    ret = -1;
    break;
  default:
    coap_log(LOG_WARNING,
             "do_mbedtls_handshake: session establish "
             "returned -0x%x: '%s'\n",
             -ret, get_error_string(ret));
    ret = -1;
    break;
  }
  return ret;

fail_alert:
  mbedtls_ssl_send_alert_message(&m_env->ssl,
                                 MBEDTLS_SSL_ALERT_LEVEL_FATAL,
                                 alert);
fail:
  c_session->dtls_event = COAP_EVENT_DTLS_ERROR;
  coap_log(LOG_WARNING,
           "do_mbedtls_handshake: session establish "
           "returned '%s'\n",
           get_error_string(ret));
reset:
  mbedtls_ssl_session_reset(&m_env->ssl);
  return -1;
}

static void
mbedtls_debug_out(void *ctx COAP_UNUSED, int level,
                  const char *file, int line, const char *str) {
  int log_level;
  /*
   *  0 No debug
   *  1 Error
   *  2 State change
   *  3 Informational
   *  4 Verbose
   */
  switch (level) {
  case 4:
  case 3:
  case 2:
    log_level = COAP_LOG_CIPHERS;
    break;
  case 1:
    log_level = LOG_ERR;
    break;
  case 0:
  default:
    log_level = 0;
    break;
  }
  coap_log(log_level, "%s:%04d: %s", file, line, str);
}

static coap_mbedtls_env_t *coap_dtls_new_mbedtls_env(coap_session_t *c_session,
                                                     coap_dtls_role_t role)
{
  int ret = 0;
  coap_mbedtls_env_t *m_env = (coap_mbedtls_env_t *)c_session->tls;

  if (m_env)
      return m_env;

  m_env = (coap_mbedtls_env_t *)mbedtls_malloc(sizeof(coap_mbedtls_env_t));
  if (!m_env) {
      return NULL;
  }
  memset(m_env, 0, sizeof(coap_mbedtls_env_t));

  mbedtls_ssl_init(&m_env->ssl);
  mbedtls_ctr_drbg_init(&m_env->ctr_drbg);
  mbedtls_ssl_config_init(&m_env->conf);
  mbedtls_entropy_init(&m_env->entropy);

#if defined(ESPIDF_VERSION) && defined(CONFIG_MBEDTLS_DEBUG)
  mbedtls_esp_enable_debug_log(&m_env->conf, CONFIG_MBEDTLS_DEBUG_LEVEL);
#endif /* ESPIDF_VERSION && CONFIG_MBEDTLS_DEBUG */
  if ((ret = mbedtls_ctr_drbg_seed(&m_env->ctr_drbg,
                  mbedtls_entropy_func, &m_env->entropy, NULL, 0)) != 0) {
    coap_log(LOG_ERR, "mbedtls_ctr_drbg_seed returned -0x%x: '%s'\n",
             -ret, get_error_string(ret));
    goto fail;
  }

  if (role == COAP_DTLS_ROLE_CLIENT) {
    if (setup_client_ssl_session(c_session, m_env) != 0) {
      goto fail;
    }
#if defined(MBEDTLS_SSL_SRV_C)
  } else if (role == COAP_DTLS_ROLE_SERVER) {
    if (setup_server_ssl_session(c_session, m_env) != 0) {
      goto fail;
    }
#endif /* MBEDTLS_SSL_SRV_C */
  } else {
    goto fail;
  }

  mbedtls_ssl_conf_min_version(&m_env->conf, MBEDTLS_SSL_MAJOR_VERSION_3,
                               MBEDTLS_SSL_MINOR_VERSION_3);

  if ((ret = mbedtls_ssl_setup(&m_env->ssl, &m_env->conf)) != 0) {
    goto fail;
  }
  mbedtls_ssl_set_bio(&m_env->ssl, c_session, coap_dgram_write,
                      coap_dgram_read, NULL);
  mbedtls_ssl_set_timer_cb(&m_env->ssl, &m_env->timer,
                           mbedtls_timing_set_delay,
                           mbedtls_timing_get_delay);

  mbedtls_ssl_conf_dbg(&m_env->conf, mbedtls_debug_out, stdout);
  return m_env;

fail:
  if (m_env) {
    mbedtls_free(m_env);
  }
  return NULL;
}

int coap_dtls_is_supported(void) {
#if defined(MBEDTLS_SSL_PROTO_DTLS)
  return 1;
#else /* !MBEDTLS_SSL_PROTO_DTLS */
  coap_log(LOG_EMERG,
        "libcoap not compiled for DTLS with Mbed TLS"
        " - update Mbed TLS to include DTLS\n");
  return 0;
#endif /* !MBEDTLS_SSL_PROTO_DTLS */
}

int coap_tls_is_supported(void)
{
  return 0;
}

void *coap_dtls_new_context(coap_context_t *c_context)
{
  coap_mbedtls_context_t *m_context;
  (void)c_context;

  m_context = (coap_mbedtls_context_t *)mbedtls_malloc(sizeof(coap_mbedtls_context_t));
  if (m_context) {
      memset(m_context, 0, sizeof(coap_mbedtls_context_t));
  }
  return m_context;
}

/*
 * return 0 failed
 *        1 passed
 */
int
coap_dtls_context_set_spsk(coap_context_t *c_context,
                              coap_dtls_spsk_t *setup_data
) {
  coap_mbedtls_context_t *m_context =
                         ((coap_mbedtls_context_t *)c_context->dtls_context);

#if !defined(MBEDTLS_SSL_SRV_C)
  coap_log(LOG_EMERG, "coap_context_set_spsk:"
           " libcoap not compiled for Server Mode for Mbed TLS"
           " - update Mbed TLS to include Server Mode\n");
  return 0;
#endif /* !MBEDTLS_SSL_SRV_C */
  if (!m_context || !setup_data)
    return 0;

  m_context->psk_pki_enabled |= IS_PSK;
  return 1;
}

/*
 * return 0 failed
 *        1 passed
 */
int
coap_dtls_context_set_cpsk(coap_context_t *c_context,
                          coap_dtls_cpsk_t *setup_data
) {
#if !defined(MBEDTLS_SSL_CLI_C)
  coap_log(LOG_EMERG, "coap_context_set_spsk:"
           " libcoap not compiled for Client Mode for Mbed TLS"
           " - update Mbed TLS to include Client Mode\n");
  return 0;
#endif /* !MBEDTLS_SSL_CLI_C */
  coap_mbedtls_context_t *m_context =
                         ((coap_mbedtls_context_t *)c_context->dtls_context);

  if (!m_context || !setup_data)
    return 0;

  if (setup_data->validate_ih_call_back) {
    coap_log(LOG_WARNING,
        "CoAP Client with Mbed TLS does not support Identity Hint selection\n");
  }
  m_context->psk_pki_enabled |= IS_PSK;
  return 1;
}

int coap_dtls_context_set_pki(coap_context_t *c_context,
                              const coap_dtls_pki_t *setup_data,
                              const coap_dtls_role_t role COAP_UNUSED)
{
  coap_mbedtls_context_t *m_context =
             ((coap_mbedtls_context_t *)c_context->dtls_context);

  m_context->setup_data = *setup_data;
  if (!m_context->setup_data.verify_peer_cert) {
    /* Needs to be clear so that no CA DNs are transmitted */
    m_context->setup_data.check_common_ca = 0;
    /* Allow all of these but warn if issue */
    m_context->setup_data.allow_self_signed = 1;
    m_context->setup_data.allow_expired_certs = 1;
    m_context->setup_data.cert_chain_validation = 1;
    m_context->setup_data.cert_chain_verify_depth = 10;
    m_context->setup_data.check_cert_revocation = 1;
    m_context->setup_data.allow_no_crl = 1;
    m_context->setup_data.allow_expired_crl = 1;
    m_context->setup_data.allow_bad_md_hash = 1;
    m_context->setup_data.allow_short_rsa_length = 1;
  }
  m_context->psk_pki_enabled |= IS_PKI;
  return 1;
}

int coap_dtls_context_set_pki_root_cas(coap_context_t *c_context,
                                   const char *ca_file,
                                   const char *ca_path)
{
  coap_mbedtls_context_t *m_context =
             ((coap_mbedtls_context_t *)c_context->dtls_context);

  if (!m_context) {
    coap_log(LOG_WARNING,
             "coap_context_set_pki_root_cas: (D)TLS environment "
             "not set up\n");
    return 0;
  }

  if (ca_file == NULL && ca_path == NULL) {
    coap_log(LOG_WARNING,
             "coap_context_set_pki_root_cas: ca_file and/or ca_path "
             "not defined\n");
    return 0;
  }
  if (m_context->root_ca_file) {
      mbedtls_free(m_context->root_ca_file);
      m_context->root_ca_file = NULL;
  }

  if (ca_file) {
    m_context->root_ca_file = mbedtls_strdup(ca_file);
  }

  if (m_context->root_ca_path) {
    mbedtls_free(m_context->root_ca_path);
    m_context->root_ca_path = NULL;
  }

  if (ca_path) {
    m_context->root_ca_path = mbedtls_strdup(ca_path);
  }
  return 1;
}

int coap_dtls_context_check_keys_enabled(coap_context_t *c_context)
{
  coap_mbedtls_context_t *m_context =
                        ((coap_mbedtls_context_t *)c_context->dtls_context);
  return m_context->psk_pki_enabled ? 1 : 0;
}

void coap_dtls_free_context(void *dtls_context)
{
  coap_mbedtls_context_t *m_context = (coap_mbedtls_context_t *)dtls_context;
  unsigned int i;

  for (i = 0; i < m_context->pki_sni_count; i++) {
    mbedtls_free(m_context->pki_sni_entry_list[i].sni);

    mbedtls_x509_crt_free(&m_context->pki_sni_entry_list[i].public_cert);

    mbedtls_pk_free(&m_context->pki_sni_entry_list[i].private_key);

    mbedtls_x509_crt_free(&m_context->pki_sni_entry_list[i].cacert);
  }
  if (m_context->pki_sni_entry_list)
    mbedtls_free(m_context->pki_sni_entry_list);

  for (i = 0; i < m_context->psk_sni_count; i++) {
    mbedtls_free(m_context->psk_sni_entry_list[i].sni);
  }
  if (m_context->psk_sni_entry_list)
    mbedtls_free(m_context->psk_sni_entry_list);

  if (m_context->root_ca_path)
    mbedtls_free(m_context->root_ca_path);
  if (m_context->root_ca_file)
    mbedtls_free(m_context->root_ca_file);

  mbedtls_free(m_context);
}

void *coap_dtls_new_client_session(coap_session_t *c_session)
{
#if !defined(MBEDTLS_SSL_CLI_C)
  (void)c_session;
  coap_log(LOG_EMERG, "coap_dtls_new_client_session:"
           " libcoap not compiled for Client Mode for Mbed TLS"
           " - update Mbed TLS to include Client Mode\n");
  return NULL;
#else /* MBEDTLS_SSL_CLI_C */
  coap_mbedtls_env_t *m_env = coap_dtls_new_mbedtls_env(c_session,
                                                       COAP_DTLS_ROLE_CLIENT);
  int ret;

  if (m_env) {
    coap_tick_t now;
    coap_ticks(&now);
    m_env->last_timeout = now;
    ret = do_mbedtls_handshake(c_session, m_env);
    if (ret == -1) {
      coap_dtls_free_mbedtls_env(m_env);
      return NULL;
    }
  }
  return m_env;
#endif /* MBEDTLS_SSL_CLI_C */
}

void *coap_dtls_new_server_session(coap_session_t *c_session)
{
  coap_mbedtls_env_t *m_env =
         (coap_mbedtls_env_t *)c_session->tls;
  if (m_env) {
#if defined(MBEDTLS_SSL_PROTO_DTLS)
#if MBEDTLS_VERSION_NUMBER >= 0x02100100
    mbedtls_ssl_set_mtu(&m_env->ssl, (uint16_t)c_session->mtu);
#endif /* MBEDTLS_VERSION_NUMBER >= 0x02100100 */
#endif /* MBEDTLS_SSL_PROTO_DTLS */
  }
  return m_env;
}

void coap_dtls_free_session(coap_session_t *c_session)
{
  if (c_session && c_session->context && c_session->tls) {
    coap_dtls_free_mbedtls_env(c_session->tls);
    c_session->tls = NULL;
    coap_handle_event(c_session->context, COAP_EVENT_DTLS_CLOSED, c_session);
  }
  return;
}

void coap_dtls_session_update_mtu(coap_session_t *c_session)
{
#if defined(MBEDTLS_SSL_PROTO_DTLS)
  coap_mbedtls_env_t *m_env =
         (coap_mbedtls_env_t *)c_session->tls;
  if (m_env) {
#if MBEDTLS_VERSION_NUMBER >= 0x02100100
    mbedtls_ssl_set_mtu(&m_env->ssl, (uint16_t)c_session->mtu);
#endif /* MBEDTLS_VERSION_NUMBER >= 0x02100100 */
  }
#else /* ! MBEDTLS_SSL_PROTO_DTLS */
  (void)c_session;
#endif /* MBEDTLS_SSL_PROTO_DTLS */
}

int coap_dtls_send(coap_session_t *c_session,
                   const uint8_t *data,
                   size_t data_len)
{
  int ret;
  coap_mbedtls_env_t *m_env = (coap_mbedtls_env_t *)c_session->tls;

  assert(m_env != NULL);

  if (!m_env) {
      return -1;
  }
  c_session->dtls_event = -1;
  if (m_env->established) {
    ret = mbedtls_ssl_write(&m_env->ssl, (const unsigned char*) data, data_len);
    if (ret <= 0) {
      switch (ret) {
      case MBEDTLS_ERR_SSL_WANT_READ:
      case MBEDTLS_ERR_SSL_WANT_WRITE:
        ret = 0;
        break;
      case MBEDTLS_ERR_SSL_FATAL_ALERT_MESSAGE:
        c_session->dtls_event = COAP_EVENT_DTLS_CLOSED;
        ret = -1;
        break;
      default:
        coap_log(LOG_WARNING,
                 "coap_dtls_send: "
                 "returned -0x%x: '%s'\n",
                 -ret, get_error_string(ret));
        ret = -1;
        break;
      }
      if (ret == -1) {
        coap_log(LOG_WARNING, "coap_dtls_send: cannot send PDU\n");
      }
    }
  } else {
    ret = do_mbedtls_handshake(c_session, m_env);
    if (ret == 1) {
      /* Just connected, so send the data */
      return coap_dtls_send(c_session, data, data_len);
    }
    ret = -1;
  }

  if (c_session->dtls_event >= 0) {
    /* COAP_EVENT_DTLS_CLOSED event reported in coap_session_disconnected() */
    if (c_session->dtls_event != COAP_EVENT_DTLS_CLOSED)
      coap_handle_event(c_session->context, c_session->dtls_event, c_session);
    if (c_session->dtls_event == COAP_EVENT_DTLS_ERROR ||
      c_session->dtls_event == COAP_EVENT_DTLS_CLOSED) {
      coap_session_disconnected(c_session, COAP_NACK_TLS_FAILED);
      ret = -1;
    }
  }
  return ret;
}

int coap_dtls_is_context_timeout(void)
{
  return 0;
}

coap_tick_t coap_dtls_get_context_timeout(void *dtls_context COAP_UNUSED)
{
  return 0;
}

coap_tick_t coap_dtls_get_timeout(coap_session_t *c_session, coap_tick_t now)
{
  coap_mbedtls_env_t *m_env = (coap_mbedtls_env_t *)c_session->tls;
  int ret = mbedtls_timing_get_delay(&m_env->timer);
  unsigned int scalar = 1 << m_env->retry_scalar;

  assert(c_session->state == COAP_SESSION_STATE_HANDSHAKE);
  switch (ret) {
  case 0:
    /* int_ms has not timed out */
    if (m_env->last_timeout + COAP_DTLS_RETRANSMIT_COAP_TICKS * scalar > now) {
      /* Need to indicate remaining timeout time */
      return m_env->last_timeout + COAP_DTLS_RETRANSMIT_COAP_TICKS * scalar;
    }
    m_env->last_timeout = now;
    /* This may cause a minor extra delay */
    return now + COAP_DTLS_RETRANSMIT_COAP_TICKS * scalar;
  case 1:
    /* int_ms has timed out, but not fin_ms */
    /*
     * Need to make sure that we do not do this too frequently
     */
    if (m_env->last_timeout + COAP_DTLS_RETRANSMIT_COAP_TICKS * scalar > now) {
      return m_env->last_timeout + COAP_DTLS_RETRANSMIT_COAP_TICKS * scalar;
    }

    /* Reset for the next time */
    m_env->last_timeout = now;
    return now;
  case 2:
    /* fin_ms has timed out - timed out  - one final try */
    return now;
  default:
    break;
  }

  return 0;
}

void coap_dtls_handle_timeout(coap_session_t *c_session)
{
  coap_mbedtls_env_t *m_env = (coap_mbedtls_env_t *)c_session->tls;

  assert(m_env != NULL && c_session->state == COAP_SESSION_STATE_HANDSHAKE);
  m_env->retry_scalar++;
  if ((++c_session->dtls_timeout_count > c_session->max_retransmit) ||
      (do_mbedtls_handshake(c_session, m_env) < 0)) {
    /* Too many retries */
    coap_session_disconnected(c_session, COAP_NACK_TLS_FAILED);
  }
  return;
}

/*
 * return +ve data amount
 *          0 no more
 *         -1 error
 */
int coap_dtls_receive(coap_session_t *c_session,
                      const uint8_t *data,
                      size_t data_len)
{
  int ret = 1;

  c_session->dtls_event = -1;
  coap_mbedtls_env_t *m_env = (coap_mbedtls_env_t *)c_session->tls;
  coap_ssl_t *ssl_data;

  assert(m_env != NULL);

  ssl_data = &m_env->coap_ssl_data;
  if (ssl_data->pdu_len) {
    coap_log(LOG_ERR, "** %s: Previous data not read %u bytes\n",
             coap_session_str(c_session), ssl_data->pdu_len);
  }
  ssl_data->pdu = data;
  ssl_data->pdu_len = (unsigned)data_len;

  if (m_env->established) {
#if COAP_CONSTRAINED_STACK
    static coap_mutex_t b_static_mutex = COAP_MUTEX_INITIALIZER;
    static uint8_t pdu[COAP_RXBUFFER_SIZE];
#else /* ! COAP_CONSTRAINED_STACK */
    uint8_t pdu[COAP_RXBUFFER_SIZE];
#endif /* ! COAP_CONSTRAINED_STACK */

#if COAP_CONSTRAINED_STACK
    coap_mutex_lock(&b_static_mutex);
#endif /* COAP_CONSTRAINED_STACK */

    if (c_session->state == COAP_SESSION_STATE_HANDSHAKE) {
      coap_handle_event(c_session->context, COAP_EVENT_DTLS_CONNECTED,
                        c_session);
      coap_session_connected(c_session);
    }

    ret = mbedtls_ssl_read(&m_env->ssl, pdu, sizeof(pdu));
    if (ret > 0) {
      ret = coap_handle_dgram(c_session->context, c_session, pdu, (size_t)ret);
#if COAP_CONSTRAINED_STACK
      coap_mutex_unlock(&b_static_mutex);
#endif /* COAP_CONSTRAINED_STACK */
      goto finish;
    }
    switch (ret) {
    case 0:
    case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
    case MBEDTLS_ERR_SSL_FATAL_ALERT_MESSAGE:
      c_session->dtls_event = COAP_EVENT_DTLS_CLOSED;
      break;
    case MBEDTLS_ERR_SSL_WANT_READ:
      break;
    default:
      coap_log(LOG_WARNING,
               "coap_dtls_receive: "
               "returned -0x%x: '%s' (length %zd)\n",
               -ret, get_error_string(ret), data_len);
      break;
    }
#if COAP_CONSTRAINED_STACK
    coap_mutex_unlock(&b_static_mutex);
#endif /* COAP_CONSTRAINED_STACK */
    ret = -1;
  }
  else {
    ret = do_mbedtls_handshake(c_session, m_env);
    if (ret == 1) {
      /* Just connected, so send the data */
       coap_session_connected(c_session);
    } else {
      if (ssl_data->pdu_len) {
        /* Do the handshake again incase of internal timeout */
        ret = do_mbedtls_handshake(c_session, m_env);
        if (ret == 1) {
          /* Just connected, so send the data */
           coap_session_connected(c_session);
        } else {
          ret = -1;
        }
      }
      ret = -1;
    }
  }
  if (c_session->dtls_event >= 0) {
    /* COAP_EVENT_DTLS_CLOSED event reported in coap_session_disconnected() */
    if (c_session->dtls_event != COAP_EVENT_DTLS_CLOSED)
      coap_handle_event(c_session->context, c_session->dtls_event, c_session);
    if (c_session->dtls_event == COAP_EVENT_DTLS_ERROR ||
      c_session->dtls_event == COAP_EVENT_DTLS_CLOSED) {
      coap_session_disconnected(c_session, COAP_NACK_TLS_FAILED);
      ssl_data = NULL;
      ret = -1;
    }
  }
finish:
  if (ssl_data && ssl_data->pdu_len) {
    /* pdu data is held on stack which will not stay there */
    coap_log(LOG_DEBUG, "coap_dtls_receive: ret %d: remaining data %u\n", ret, ssl_data->pdu_len);
    ssl_data->pdu_len = 0;
    ssl_data->pdu = NULL;
  }
  return ret;
}

/*
 * return -1  failure
 *         0  not completed
 *         1  client hello seen
 */
int coap_dtls_hello(coap_session_t *c_session,
                    const uint8_t *data,
                    size_t data_len)
{
#if !defined(MBEDTLS_SSL_PROTO_DTLS) || !defined(MBEDTLS_SSL_SRV_C)
  (void)c_session;
  (void)data;
  (void)data_len;
  coap_log(LOG_EMERG, "coap_dtls_hello:"
           " libcoap not compiled for DTLS or Server Mode for Mbed TLS"
           " - update Mbed TLS to include DTLS and Server Mode\n");
  return -1;
#else /* MBEDTLS_SSL_PROTO_DTLS && MBEDTLS_SSL_SRV_C */
  coap_mbedtls_env_t *m_env = (coap_mbedtls_env_t *)c_session->tls;
  coap_ssl_t *ssl_data;
  int ret;

  if (!m_env) {
    m_env = coap_dtls_new_mbedtls_env(c_session, COAP_DTLS_ROLE_SERVER);
    if (m_env) {
      c_session->tls = m_env;
    }
    else {
      /* error should have already been reported */
      return -1;
    }
  }

  if((ret = mbedtls_ssl_set_client_transport_id(&m_env->ssl,
                            (unsigned char *)&c_session->addr_info.remote,
                            sizeof(c_session->addr_info.remote))) != 0) {
    coap_log(LOG_ERR,
             "mbedtls_ssl_set_client_transport_id() returned -0x%x: '%s'\n",
             -ret, get_error_string(ret));
    return -1;
  }

  ssl_data = &m_env->coap_ssl_data;
  if (ssl_data->pdu_len) {
    coap_log(LOG_ERR, "** %s: Previous data not read %u bytes\n",
             coap_session_str(c_session), ssl_data->pdu_len);
  }
  ssl_data->pdu = data;
  ssl_data->pdu_len = (unsigned)data_len;

  ret = do_mbedtls_handshake(c_session, m_env);
  if (ret == 0 || m_env->seen_client_hello) {
    /* The test for seen_client_hello gives the ability to setup a new
       c_session to continue the do_mbedtls_handshake past the client hello
       and safely allow updating of the m_env and separately
       letting a new session cleanly start up.
     */
    m_env->seen_client_hello = 0;
    ret = 1;
  }
  else {
    ret = 0;
  }

  if (ssl_data->pdu_len) {
    /* pdu data is held on stack which will not stay there */
    coap_log(LOG_DEBUG, "coap_dtls_hello: ret %d: remaining data %u\n", ret, ssl_data->pdu_len);
    ssl_data->pdu_len = 0;
    ssl_data->pdu = NULL;
  }
  return ret;
#endif /* MBEDTLS_SSL_PROTO_DTLS && MBEDTLS_SSL_SRV_C */
}

unsigned int coap_dtls_get_overhead(coap_session_t *c_session)
{
  coap_mbedtls_env_t *m_env = (coap_mbedtls_env_t *)c_session->tls;
  int expansion = mbedtls_ssl_get_record_expansion(&m_env->ssl);

  if (expansion == MBEDTLS_ERR_SSL_FEATURE_UNAVAILABLE) {
    return 13 + 8 + 8;
  }
  return expansion;
}

#if !COAP_DISABLE_TCP
void *coap_tls_new_client_session(coap_session_t *c_session COAP_UNUSED,
                                  int *connected COAP_UNUSED)
{
  return NULL;
}

void *coap_tls_new_server_session(coap_session_t *c_session COAP_UNUSED,
                                  int *connected COAP_UNUSED)
{
  return NULL;
}

void coap_tls_free_session( coap_session_t *c_session COAP_UNUSED)
{
}

ssize_t coap_tls_write(coap_session_t *c_session COAP_UNUSED,
                       const uint8_t *data COAP_UNUSED,
                       size_t data_len COAP_UNUSED
                       )
{
  return 0;
}

ssize_t coap_tls_read(coap_session_t *c_session COAP_UNUSED,
                      uint8_t *data COAP_UNUSED,
                      size_t data_len COAP_UNUSED
                      )
{
  return 0;
}
#endif /* !COAP_DISABLE_TCP */

void coap_dtls_startup(void)
{
}

void coap_dtls_shutdown(void) {
}

void *
coap_dtls_get_tls(const coap_session_t *c_session,
                  coap_tls_library_t *tls_lib) {
  if (tls_lib)
    *tls_lib = COAP_TLS_LIBRARY_MBEDTLS;
  if (c_session && c_session->tls) {
     coap_mbedtls_env_t *m_env;

    /* To get around const issue */
    memcpy(&m_env, &c_session->tls, sizeof(m_env));

    return (void *)&m_env->ssl;
  }
  return NULL;
}

static int keep_log_level = 0;

void coap_dtls_set_log_level(int level)
{
#if !defined(ESPIDF_VERSION)
  int use_level;
  /*
   * Mbed TLS debug levels filter
   *  0 No debug
   *  1 Error
   *  2 State change
   *  3 Informational
   *  4 Verbose
   */

  if (level <= LOG_ERR) {
    use_level = 1;
  }
  else {
    use_level = (level >= LOG_DEBUG) ? level - LOG_DEBUG + 2 : 0;
  }
  mbedtls_debug_set_threshold(use_level);
#endif /* !ESPIDF_VERSION) */
  keep_log_level = level;
}

int coap_dtls_get_log_level(void)
{
  return keep_log_level;
}

coap_tls_version_t * coap_get_tls_library_version(void)
{
  static coap_tls_version_t version;
  version.version = mbedtls_version_get_number();
  version.built_version = MBEDTLS_VERSION_NUMBER;
  version.type = COAP_TLS_LIBRARY_MBEDTLS;
  return &version;
}

coap_digest_ctx_t *
coap_digest_setup(void) {
  mbedtls_sha256_context *digest_ctx = mbedtls_malloc(sizeof(mbedtls_sha256_context));

  if (digest_ctx) {
    mbedtls_sha256_init(digest_ctx);
    mbedtls_sha256_starts(digest_ctx, 0);
  }
  return digest_ctx;
}

void
coap_digest_free(coap_digest_ctx_t *digest_ctx) {
  mbedtls_sha256_free(digest_ctx);
  mbedtls_free(digest_ctx);
}

int
coap_digest_update(coap_digest_ctx_t *digest_ctx,
                   const uint8_t *data,
                   size_t data_len) {
  int ret = mbedtls_sha256_update(digest_ctx, data, data_len);

  return ret == 0;
}

int
coap_digest_final(coap_digest_ctx_t *digest_ctx,
                         coap_digest_t *digest_buffer) {
  int ret = mbedtls_sha256_finish(digest_ctx, (uint8_t*)digest_buffer);

  coap_digest_free(digest_ctx);
  return ret == 0;
}

#else /* !HAVE_MBEDTLS */

#ifdef __clang__
/* Make compilers happy that do not like empty modules. As this function is
 * never used, we ignore -Wunused-function at the end of compiling this file
 */
#pragma GCC diagnostic ignored "-Wunused-function"
#endif
static inline void dummy(void) {
}

#endif /* HAVE_MBEDTLS */
