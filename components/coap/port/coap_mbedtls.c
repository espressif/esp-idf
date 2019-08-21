/*
* coap_mbedtls.c -- mbedTLS Datagram Transport Layer Support for libcoap
*
* Copyright (C) 2019 Jon Shallow <supjps-libcoap@jpshallow.com>
*               2019 Jitin George <jitin@espressif.com>
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

#include "coap_config.h"

#ifdef HAVE_MBEDTLS

/*
 * Once PS #335 has been merged in, then code following a rebase needs to be
 * updated removing sections that are "#ifndef PSK2_PR", and then remove all
 * references to PSK2_PR.
 */
#undef PSK2_PR

#include "libcoap.h"
#include "coap_dtls.h"
#include "net.h"
#include "mem.h"
#include "coap_debug.h"
#include "prng.h"
#include "coap_mutex.h"

#include <mbedtls/version.h>
#include <mbedtls/platform.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/error.h>
#include <mbedtls/certs.h>
#include <mbedtls/timing.h>
#include <mbedtls/ssl_cookie.h>
#include <mbedtls/debug.h>
#if defined(ESPIDF_VERSION) && defined(CONFIG_MBEDTLS_DEBUG)
#include <mbedtls/esp_debug.h>
#endif /* ESPIDF_VERSION && CONFIG_MBEDTLS_DEBUG */
#include <errno.h>
#include <arpa/inet.h>

#define mbedtls_malloc(a) malloc(a)
#define mbedtls_realloc(a,b) realloc(a,b)
#define mbedtls_strdup(a) strdup(a)

#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#else /* __GNUC__ */
#define UNUSED
#endif /* __GNUC__ */

#define IS_PSK (1 << 0)
#define IS_PKI (1 << 1)
#define IS_CLIENT (1 << 6)
#define IS_SERVER (1 << 7)

typedef struct coap_ssl_t {
  const uint8_t *pdu;
  unsigned pdu_len;
  unsigned peekmode;
  coap_tick_t timeout;
} coap_ssl_t;

/*
 * This structure encapsulates the mbedTLS session object.
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
  coap_ssl_t coap_ssl_data;
} coap_mbedtls_env_t;

typedef struct pki_sni_entry {
  char *sni;
  coap_dtls_key_t pki_key;
  mbedtls_x509_crt cacert;
  mbedtls_x509_crt public_cert;
  mbedtls_pk_context private_key;
} pki_sni_entry;

#ifdef PSK2_PR
typedef struct psk_sni_entry {
  coap_string_t sni;
  coap_dtls_spsk_info_t psk_info;
} psk_sni_entry;
#endif /* PSK2_PR */

typedef struct coap_mbedtls_context_t {
  coap_dtls_pki_t setup_data;
  size_t pki_sni_count;
  pki_sni_entry *pki_sni_entry_list;
#ifdef PSK2_PR
  size_t psk_sni_count;
  psk_sni_entry *psk_sni_entry_list;
#endif /* PSK2_PR */
  char *root_ca_file;
  char *root_ca_path;
  int psk_pki_enabled;
} coap_mbedtls_context_t;

static int coap_dgram_read(void *ctx, unsigned char *out, size_t outl)
{
  ssize_t ret = 0;
  coap_session_t *c_session = (struct coap_session_t *)ctx;
  coap_ssl_t *data = &((coap_mbedtls_env_t *)c_session->tls)->coap_ssl_data;

  if (!c_session->tls) {
    errno = EAGAIN;
    return MBEDTLS_ERR_SSL_WANT_READ;
  }

  if (out != NULL) {
    if (data != NULL && data->pdu_len > 0) {
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
       return ret;
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
  coap_session_t *c_session = (struct coap_session_t *)ctx;

  if (c_session) {
    result = coap_session_send(c_session, send_buffer, send_buffer_length);
    if (result != (int)send_buffer_length) {
      coap_log(LOG_WARNING, "coap_network_send failed (%zd != %zd)\n",
               result, send_buffer_length);
      result = 0;
    }
  } else {
    result = 0;
  }
  return result;
}

#if !defined(ESPIDF_VERSION) || defined(CONFIG_MBEDTLS_SSL_PROTO_DTLS)
static char*
get_ip_addr(const struct coap_address_t *addr)
{
  const void *addrptr = NULL;
  size_t buf_len;

  if (!addr) {
    return NULL;
  }
  switch (addr->addr.sa.sa_family) {
  case AF_INET:
    addrptr = &addr->addr.sin.sin_addr;
    buf_len = INET_ADDRSTRLEN;
    break;
  case AF_INET6:
    addrptr = &addr->addr.sin6.sin6_addr;
    buf_len = INET6_ADDRSTRLEN;
    break;
  default:
    return NULL;
  }
  char *str = (char *)mbedtls_calloc(1, buf_len);
  if (!str) {
    coap_log(LOG_ERR, "Memory allocation failed\n");
    return NULL;
  }
  if (inet_ntop(addr->addr.sa.sa_family, addrptr, str,
                buf_len) == 0) {
    perror("coap_print_addr");
    return 0;
  }
  return str;
}
#endif /* !ESPIDF_VERSION || CONFIG_MBEDTLS_SSL_PROTO_DTLS */

#if !defined(ESPIDF_VERSION) || (defined(CONFIG_MBEDTLS_SSL_PROTO_DTLS) && defined(CONFIG_MBEDTLS_PSK_MODES))
/*
 * Server side PSK callback
 */
static int psk_server_callback(void *p_info, mbedtls_ssl_context *ssl,
                  const unsigned char *name, size_t name_len )
{
  coap_session_t *c_session =
                 (coap_session_t *)p_info;
  uint8_t buf[128];
  size_t psk_len;
#ifdef PSK2_PR
  coap_dtls_spsk_t *setup_data;
#endif /* PSK2_PR */
  coap_mbedtls_env_t *m_env;

  coap_log(LOG_DEBUG, "got psk_identity: '%.*s'\n",
                      (int)name_len, name);

  if (c_session == NULL || c_session->context == NULL ||
    c_session->context->get_server_psk == NULL) {
    return -1;
  }
  m_env = (coap_mbedtls_env_t *)c_session->tls;
#ifdef PSK2_PR
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
#endif /* PSK2_PR */

  psk_len = c_session->context->get_server_psk(c_session,
                               (const uint8_t*)name,
                               name_len,
                               (uint8_t*)buf, sizeof(buf));
  m_env->seen_client_hello = 1;
  mbedtls_ssl_set_hs_psk(ssl, buf, psk_len);
  return 0;
}
#endif /* !ESPIDF_VERSION || CONFIG_MBEDTLS_SSL_PROTO_DTLS */

static char*
get_san_or_cn_from_cert(mbedtls_x509_crt *crt)
{
  if (crt) {
#if COAP_CONSTRAINED_STACK
    static coap_mutex_t a_static_mutex = COAP_MUTEX_INITIALIZER;
    static char buf[1024];
#else /* ! COAP_CONSTRAINED_STACK */
    char buf[1024];
#endif /* ! COAP_CONSTRAINED_STACK */
    char *cn;
    char *cp;
    char *tcp;
    int n;

#if COAP_CONSTRAINED_STACK
    coap_mutex_lock(&a_static_mutex);
#endif /* COAP_CONSTRAINED_STACK */

    mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "", crt );

    /* Look first to see if Subject Alt Name is defined */
    cp = strstr(buf, "subject alt name");
    if (cp) {
      cp = strchr(cp, ':');
      if (cp) {
        cp++;
        while (*cp == ' ') cp++;
        tcp = strchr(cp, '\n');
        if (tcp)
          *tcp = '\000';
        /* Take only the first entry */
        tcp = strchr(cp, ',');
        if (tcp)
          *tcp = '\000';
        /* Return the Subject Alt Name */
#if COAP_CONSTRAINED_STACK
          coap_mutex_unlock(&a_static_mutex);
#endif /* COAP_CONSTRAINED_STACK */
        return mbedtls_strdup(cp);
      }
    }

    /* Pull CN= out of subject name */
    cp = strstr(buf, "subject name");
    if (cp) {
      cp = strchr(cp, ':');
      if (cp) {
        cp++;
        while (*cp == ' ') cp++;
        tcp = strchr(cp, '\n');
        if (tcp)
          *tcp = '\000';

        /* Need to emulate strcasestr() here.  Looking for CN= */
        n = strlen(cp) - 3;
        cn = cp;
        while (n > 0) {
          if (((cn[0] == 'C') || (cn[0] == 'c')) &&
              ((cn[1] == 'N') || (cn[1] == 'n')) &&
              (cn[2] == '=')) {
             cn += 3;
             break;
          }
          cn++;
          n--;
        }
        if (n > 0) {
          tcp = strchr(cn, ',');
          if (tcp)
            *tcp = '\000';
#if COAP_CONSTRAINED_STACK
          coap_mutex_unlock(&a_static_mutex);
#endif /* COAP_CONSTRAINED_STACK */
          return mbedtls_strdup(cn);
        }
      }
    }
#if COAP_CONSTRAINED_STACK
    coap_mutex_unlock(&a_static_mutex);
#endif /* COAP_CONSTRAINED_STACK */
  }
  return NULL;
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

  if (!setup_data->verify_peer_cert) {
    /* Nothing is being checked */
    *flags = 0;
    return 0;
  }

  cn = get_san_or_cn_from_cert(crt);

  if (*flags & MBEDTLS_X509_BADCERT_EXPIRED) {
    if (setup_data->allow_expired_certs) {
      *flags &= ~MBEDTLS_X509_BADCERT_EXPIRED;
      coap_log(LOG_WARNING,
               "   %s: %s: overridden: '%s' depth %d\n",
               coap_session_str(c_session),
               "The certificate has expired", cn ? cn : "?", depth);
    }
  }
  if (*flags & MBEDTLS_X509_BADCERT_FUTURE) {
    if (setup_data->allow_expired_certs) {
      *flags &= ~MBEDTLS_X509_BADCERT_FUTURE;
      coap_log(LOG_WARNING,
               "   %s: %s: overridden: '%s' depth %d\n",
               coap_session_str(c_session),
               "The certificate has a future date", cn ? cn : "?", depth);
    }
  }
  if (*flags & MBEDTLS_X509_BADCERT_BAD_MD) {
    if (setup_data->allow_bad_md_hash) {
      *flags &= ~MBEDTLS_X509_BADCERT_BAD_MD;
      coap_log(LOG_WARNING,
               "   %s: %s: overridden: '%s' depth %d\n",
               coap_session_str(c_session),
               "The certificate has a bad MD hash", cn ? cn : "?", depth);
    }
  }
  if (*flags & MBEDTLS_X509_BADCERT_BAD_KEY) {
    if (setup_data->allow_short_rsa_length) {
      *flags &= ~MBEDTLS_X509_BADCERT_BAD_KEY;
      coap_log(LOG_WARNING,
               "   %s: %s: overridden: '%s' depth %d\n",
               coap_session_str(c_session),
               "The certificate has a short RSA length", cn ? cn : "?", depth);
    }
  }
  if (*flags & MBEDTLS_X509_BADCRL_EXPIRED) {
    if (setup_data->check_cert_revocation && setup_data->allow_expired_crl) {
      *flags &= ~MBEDTLS_X509_BADCRL_EXPIRED;
      coap_log(LOG_WARNING,
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
      coap_log(LOG_WARNING,
               "   %s: %s: overridden: '%s' depth %d\n",
               coap_session_str(c_session),
               "The certificate's CRL has a future date", cn ? cn : "?", depth);
    }
    else if (!setup_data->check_cert_revocation) {
      *flags &= ~MBEDTLS_X509_BADCRL_FUTURE;
    }
  }

  if (*flags & MBEDTLS_X509_BADCERT_CN_MISMATCH) {
    *flags &= ~MBEDTLS_X509_BADCERT_CN_MISMATCH;
  }
  if (setup_data->validate_cn_call_back) {
    if (!setup_data->validate_cn_call_back(cn,
           crt->raw.p,
           crt->raw.len,
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

    mbedtls_x509_crt_verify_info(buf, sizeof(buf), "", *flags);
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
        coap_log(LOG_ERR, "mbedtls_x509_crt_parse_file returned -0x%x\n\n",
                 -ret);
        return ret;
      }

      ret = mbedtls_pk_parse_keyfile(private_key,
                              setup_data->pki_key.key.pem.private_key, NULL);
      if (ret < 0) {
        coap_log(LOG_ERR, "mbedtls_pk_parse_keyfile returned -0x%x\n\n", -ret);
        return ret;
      }

      ret = mbedtls_ssl_conf_own_cert(&m_env->conf, public_cert, private_key);
      if (ret < 0) {
        coap_log(LOG_ERR, "mbedtls_ssl_conf_own_cert returned -0x%x\n\n", -ret);
        return ret;
      }
    }
    else if (role == COAP_DTLS_ROLE_SERVER) {
      coap_log(LOG_ERR,
               "***setup_pki: (D)TLS: No %s Certificate + Private "
               "Key defined\n",
                role == COAP_DTLS_ROLE_SERVER ? "Server" : "Client");
      return -1;
    }

    if (setup_data->pki_key.key.pem.ca_file &&
        setup_data->pki_key.key.pem.ca_file[0]) {
      mbedtls_x509_crt_init(cacert);
      ret = mbedtls_x509_crt_parse_file(cacert,
                                        setup_data->pki_key.key.pem.ca_file);
      if (ret < 0) {
        coap_log(LOG_ERR, "mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
        return ret;
      }
      mbedtls_ssl_conf_authmode(&m_env->conf, setup_data->require_peer_cert ?
                                              MBEDTLS_SSL_VERIFY_REQUIRED :
                                              MBEDTLS_SSL_VERIFY_OPTIONAL);
      mbedtls_ssl_conf_ca_chain(&m_env->conf, cacert, NULL);
    }
    break;
  case COAP_PKI_KEY_PEM_BUF:
    if (setup_data->pki_key.key.pem_buf.public_cert &&
        setup_data->pki_key.key.pem_buf.public_cert_len &&
        setup_data->pki_key.key.pem_buf.private_key &&
        setup_data->pki_key.key.pem_buf.private_key_len > 0) {
      mbedtls_x509_crt_init(public_cert);
      mbedtls_pk_init(private_key);
      ret = mbedtls_x509_crt_parse(public_cert,
            (const unsigned char *)setup_data->pki_key.key.pem_buf.public_cert,
            setup_data->pki_key.key.pem_buf.public_cert_len);
      if (ret < 0) {
        coap_log(LOG_ERR, "mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
        return ret;
      }

      ret = mbedtls_pk_parse_key(private_key,
            (const unsigned char *)setup_data->pki_key.key.pem_buf.private_key,
            setup_data->pki_key.key.pem_buf.private_key_len, NULL, 0);
      if (ret < 0) {
        coap_log(LOG_ERR, "mbedtls_pk_parse_keyfile returned -0x%x\n\n", -ret);
        return ret;
      }

      ret = mbedtls_ssl_conf_own_cert(&m_env->conf, public_cert, private_key);
      if (ret < 0) {
        coap_log(LOG_ERR, "mbedtls_ssl_conf_own_cert returned -0x%x\n\n", -ret);
        return ret;
      }
    } else if (role == COAP_DTLS_ROLE_SERVER) {
      coap_log(LOG_ERR,
              "***setup_pki: (D)TLS: No %s Certificate + Private "
              "Key defined\n",
              role == COAP_DTLS_ROLE_SERVER ? "Server" : "Client");
      return -1;
    }

    if (setup_data->pki_key.key.pem_buf.ca_cert &&
        setup_data->pki_key.key.pem_buf.ca_cert_len > 0) {
      mbedtls_x509_crt_init(cacert);
      ret = mbedtls_x509_crt_parse(cacert,
              (const unsigned char *)setup_data->pki_key.key.pem_buf.ca_cert,
              setup_data->pki_key.key.pem_buf.ca_cert_len);
      if (ret < 0) {
        coap_log(LOG_ERR, "mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
        return ret;
      }
      mbedtls_ssl_conf_authmode(&m_env->conf, setup_data->require_peer_cert ?
                                              MBEDTLS_SSL_VERIFY_REQUIRED :
                                              MBEDTLS_SSL_VERIFY_OPTIONAL);
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
        coap_log(LOG_ERR, "mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
        return ret;
      }

      ret = mbedtls_pk_parse_key(private_key,
              (const unsigned char *)setup_data->pki_key.key.asn1.private_key,
              setup_data->pki_key.key.asn1.private_key_len, NULL, 0);
      if (ret < 0) {
        coap_log(LOG_ERR, "mbedtls_pk_parse_keyfile returned -0x%x\n\n", -ret);
        return ret;
      }

      ret = mbedtls_ssl_conf_own_cert(&m_env->conf, public_cert, private_key);
      if (ret < 0) {
        coap_log(LOG_ERR, "mbedtls_ssl_conf_own_cert returned -0x%x\n\n", -ret);
        return ret;
      }
    } else if (role == COAP_DTLS_ROLE_SERVER) {
      coap_log(LOG_ERR,
               "***setup_pki: (D)TLS: No %s Certificate + Private "
               "Key defined\n",
               role == COAP_DTLS_ROLE_SERVER ? "Server" : "Client");
      return -1;
    }

    if (setup_data->pki_key.key.asn1.ca_cert &&
        setup_data->pki_key.key.asn1.ca_cert_len > 0) {
      mbedtls_x509_crt_init(cacert);
      ret = mbedtls_x509_crt_parse(cacert,
                  (const unsigned char *)setup_data->pki_key.key.asn1.ca_cert,
                  setup_data->pki_key.key.asn1.ca_cert_len);
      if (ret < 0) {
        coap_log(LOG_ERR, "mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
        return ret;
      }
      mbedtls_ssl_conf_authmode(&m_env->conf, setup_data->require_peer_cert ?
                                              MBEDTLS_SSL_VERIFY_REQUIRED :
                                              MBEDTLS_SSL_VERIFY_OPTIONAL);
      mbedtls_ssl_conf_ca_chain(&m_env->conf, cacert, NULL);
    }
    break;
  default:
    coap_log(LOG_ERR,
             "***setup_pki: (D)TLS: Unknown key type %d\n",
             setup_data->pki_key.key_type);
      return -1;
  }

  if (m_context->root_ca_file) {
    ret = mbedtls_x509_crt_parse_file(cacert, m_context->root_ca_file);
    if (ret < 0) {
      coap_log(LOG_ERR, "mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
      return ret;
    }
    mbedtls_ssl_conf_ca_chain(&m_env->conf, cacert, NULL);
  }
  if (m_context->root_ca_path) {
    ret = mbedtls_x509_crt_parse_file(cacert, m_context->root_ca_path);
    if (ret < 0) {
      coap_log(LOG_ERR, "mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
      return ret;
    }
    mbedtls_ssl_conf_ca_chain(&m_env->conf, cacert, NULL);
  }

  /*
   * Verify Peer.
   *  Need to do all checking, even if setup_data->verify_peer_cert is not set
   */
  mbedtls_ssl_conf_verify(&m_env->conf,
                          cert_verify_callback_mbedtls, c_session);

  return 0;
}

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

  /* Is this a cached entry? */
  for (i = 0; i < m_context->pki_sni_count; i++) {
    if (name_len == strlen(m_context->pki_sni_entry_list[i].sni) &&
        memcmp(uname, m_context->pki_sni_entry_list[i].sni, name_len) == 0) {
      break;
    }
  }
  if (i == m_context->pki_sni_count) {
    /*
     * New PKI SNI request
     */
    char *name;
    coap_dtls_key_t *new_entry;

    name = mbedtls_malloc(name_len+1);
    memcpy(name, uname, name_len);
    name[name_len] = '\000';
    new_entry =
      m_context->setup_data.validate_sni_call_back(name,
                                 m_context->setup_data.sni_call_back_arg);
    if (!new_entry) {
      ret = -1;
      mbedtls_free(name);
      goto end;
    }

    m_context->pki_sni_entry_list =
             mbedtls_realloc(m_context->pki_sni_entry_list,
                                   (i+1)*sizeof(pki_sni_entry));
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
      ret = -1;
      mbedtls_free(name);
      goto end;
    }
    m_context->pki_sni_count++;
  }

end:
  if (ret != -1) {
    mbedtls_ssl_set_hs_ca_chain(ssl, &m_context->pki_sni_entry_list[i].cacert,
                                NULL);
    return mbedtls_ssl_set_hs_own_cert(ssl,
                                &m_context->pki_sni_entry_list[i].public_cert,
                                &m_context->pki_sni_entry_list[i].private_key);
  }
  return ret;
}

#ifdef PSK2_PR
/*
 * PSK SNI callback.
 */
static int
psk_sni_callback(void *p_info, mbedtls_ssl_context *ssl,
             const unsigned char *uname, size_t name_len)
{
  unsigned int i;
  coap_dtls_spsk_t sni_setup_data;
  coap_session_t *c_session = (coap_session_t *)p_info;
  coap_mbedtls_context_t *m_context =
           (coap_mbedtls_context_t *)c_session->context->dtls_context;
  int ret = 0;

  /* Is this a cached entry? */
  for (i = 0; i < m_context->psk_sni_count; i++) {
    if (name_len == m_context->psk_sni_entry_list[i].sni.length &&
        memcmp(uname, m_context->psk_sni_entry_list[i].sni.s, name_len) == 0) {
      break;
    }
  }
  if (i == m_context->psk_sni_count) {
    /*
     * New PSK SNI request
     */
    coap_str_const_t lsni;
    uint8_t *name;
    const coap_dtls_spsk_info_t *new_entry;

    name = mbedtls_malloc(name_len+1);
    memcpy(name, uname, name_len);
    name[name_len] = '\000';

    lsni.s = name;
    lsni.length = name_len;
    new_entry =
      c_session->context->spsk_setup_data.validate_sni_call_back(&lsni,
                      c_session,
                      c_session->context->spsk_setup_data.sni_call_back_arg);
    if (!new_entry) {
      ret = -1;
      mbedtls_free(name);
      goto end;
    }

    m_context->psk_sni_entry_list =
             mbedtls_realloc(m_context->psk_sni_entry_list,
                                   (i+1)*sizeof(psk_sni_entry));

    m_context->psk_sni_entry_list[i].sni.s = name;
    m_context->psk_sni_entry_list[i].sni.length = name_len;
    m_context->psk_sni_entry_list[i].psk_info = *new_entry;
    sni_setup_data = c_session->context->spsk_setup_data;
    sni_setup_data.psk_info = *new_entry;
    m_context->psk_sni_count++;
  }

end:
  if (ret != -1) {
    coap_session_refresh_psk_hint(c_session,
                            &m_context->psk_sni_entry_list[i].psk_info.hint);
    coap_session_refresh_psk_key(c_session,
                            &m_context->psk_sni_entry_list[i].psk_info.key);
    return mbedtls_ssl_set_hs_psk(ssl,
                         m_context->psk_sni_entry_list[i].psk_info.key.s,
                         m_context->psk_sni_entry_list[i].psk_info.key.length);
  }
  return ret;
}
#endif /* PSK2_PR */

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
    coap_log(LOG_ERR, "mbedtls_ssl_config_defaults returned -0x%x\n", -ret);
    goto fail;
  }

  mbedtls_ssl_conf_rng(&m_env->conf, mbedtls_ctr_drbg_random, &m_env->ctr_drbg);

#if !defined(ESPIDF_VERSION) || defined(CONFIG_MBEDTLS_SSL_PROTO_DTLS)
  mbedtls_ssl_conf_handshake_timeout(&m_env->conf, 1000, 60000);

  if (m_context->psk_pki_enabled & IS_PSK) {
#if !defined(ESPIDF_VERSION) || defined(CONFIG_MBEDTLS_PSK_MODES)
    mbedtls_ssl_conf_psk_cb(&m_env->conf, psk_server_callback, c_session);
#ifdef PSK2_PR
    if (c_session->context->spsk_setup_data.validate_sni_call_back) {
      mbedtls_ssl_conf_sni(&m_env->conf, psk_sni_callback, c_session);
    }
#endif /* PSK2_PR */
#endif /* !ESPIDF_VERSION || CONFIG_MBEDTLS_PSK_MODES */
  }
#endif /* !ESPIDF_VERSION || CONFIG_MBEDTLS_SSL_PROTO_DTLS */

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
    coap_log(LOG_ERR, "mbedtls_ssl_cookie_setup: returned -0x%x\n", -ret);
    goto fail;
  }

#if !defined(ESPIDF_VERSION) || defined(CONFIG_MBEDTLS_SSL_PROTO_DTLS)
  mbedtls_ssl_conf_dtls_cookies(&m_env->conf, mbedtls_ssl_cookie_write,
                                mbedtls_ssl_cookie_check,
                                &m_env->cookie_ctx );
    mbedtls_ssl_set_mtu(&m_env->ssl, c_session->mtu);
#endif /* !ESPIDF_VERSION || CONFIG_MBEDTLS_SSL_PROTO_DTLS */
fail:
  return ret;
}

#define MAX_CIPHERS 100
static int psk_ciphers[MAX_CIPHERS];
static int pki_ciphers[MAX_CIPHERS];
static int processed_ciphers = 0;

static void
set_ciphersuites(mbedtls_ssl_config *conf, int is_psk)
{
  if (!processed_ciphers) {
    const int *list = mbedtls_ssl_list_ciphersuites();
    int *psk_list = psk_ciphers;
    int *pki_list = pki_ciphers;

    while (*list) {
      const mbedtls_ssl_ciphersuite_t *cur =
                                     mbedtls_ssl_ciphersuite_from_id(*list);

      if (cur) {
        if (mbedtls_ssl_ciphersuite_uses_psk(cur)) {
          if (&psk_ciphers[MAX_CIPHERS] - psk_list > 1) {
            *psk_list = *list;
            psk_list++;
          }
          else {
            static int done = 0;

            if (!done) {
              done = 1;
              coap_log(LOG_ERR, "psk_ciphers[MAX_CIPHERS] insufficient\n");
            }
          }
        }
        else {
          if (&pki_ciphers[MAX_CIPHERS] - pki_list > 1) {
            *pki_list = *list;
            pki_list++;
          }
          else {
            static int done = 0;

            if (!done) {
              done = 1;
              coap_log(LOG_ERR, "pki_ciphers[MAX_CIPHERS] insufficient\n");
            }
          }
        }
      }
      list++;
    }
    /* zero terminate */
    *psk_list = 0;
    *pki_list = 0;
    processed_ciphers = 1;
  }
  mbedtls_ssl_conf_ciphersuites(conf, is_psk ? psk_ciphers : pki_ciphers);
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
      coap_log(LOG_ERR, "mbedtls_ssl_config_defaults returned -0x%x", -ret);
      goto fail;
  }

#if !defined(ESPIDF_VERSION) || defined(CONFIG_MBEDTLS_SSL_PROTO_DTLS)
  mbedtls_ssl_conf_handshake_timeout(&m_env->conf, 1000, 60000);
#endif /* !ESPIDF_VERSION || CONFIG_MBEDTLS_SSL_PROTO_DTLS */

  mbedtls_ssl_conf_authmode(&m_env->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
  mbedtls_ssl_conf_rng(&m_env->conf, mbedtls_ctr_drbg_random, &m_env->ctr_drbg);

  if (m_context->psk_pki_enabled & IS_PSK) {
#if !defined(ESPIDF_VERSION) || defined(CONFIG_MBEDTLS_PSK_MODES)
    uint8_t identity[64];
    size_t identity_len;
    uint8_t psk_key[64];
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
    assert(identity_len < sizeof(identity));
    mbedtls_ssl_conf_psk(&m_env->conf, (const unsigned char *)psk_key,
                         psk_len, (const unsigned char *)identity,
                         identity_len);
#ifdef PSK2_PR
    if (c_session->cpsk_setup_data.client_sni) {
      mbedtls_ssl_set_hostname(&m_env->ssl,
                               c_session->cpsk_setup_data.client_sni);
    }
#if 0
/* Identity Hint currently not supported in MbedTLS */
    if (c_session->cpsk_setup_data.validate_ih_call_back) {
      coap_log(LOG_DEBUG,
         "CoAP Client restricted to (D)TLS1.2 with Identity Hint callback\n");
      mbedtls_ssl_conf_max_version(&m_env->conf, MBEDTLS_SSL_MAJOR_VERSION_3,
                                                 MBEDTLS_SSL_MINOR_VERSION_3);
    }
#endif
#endif /* PSK2_PR */
    set_ciphersuites(&m_env->conf, 1);
#endif /* !ESPIDF_VERSION || CONFIG_MBEDTLS_PSK_MODES */
  }
  else if ((m_context->psk_pki_enabled & IS_PKI) ||
           (m_context->psk_pki_enabled & (IS_PSK | IS_PKI)) == 0) {
    /*
     * If neither PSK or PKI have been set up, use PKI basics.
     * This works providing COAP_PKI_KEY_PEM has a value of 0.
     */
    if ((m_context->psk_pki_enabled & (IS_PSK | IS_PKI)) == 0) {
      mbedtls_ssl_conf_authmode(&m_env->conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
    }
    ret = setup_pki_credentials(&m_env->cacert, &m_env->public_cert,
                                &m_env->private_key, m_env, m_context,
                                c_session, &m_context->setup_data,
                                COAP_DTLS_ROLE_CLIENT);
    if (ret < 0) {
      coap_log(LOG_ERR, "PKI setup failed\n");
      return ret;
    }
    if (c_session->proto == COAP_PROTO_TLS) {
      const char *alpn_list[2];

      memset(alpn_list, 0, sizeof(alpn_list));
      alpn_list[0] = "coap";
      ret = mbedtls_ssl_conf_alpn_protocols(&m_env->conf, alpn_list);
      if (ret != 0) {
        coap_log(LOG_ERR, "ALPN setup failed %d)\n", ret);
      }
    }
    if (m_context->setup_data.client_sni) {
      mbedtls_ssl_set_hostname(&m_env->ssl, m_context->setup_data.client_sni);
    }
#if !defined(ESPIDF_VERSION) || defined(CONFIG_MBEDTLS_SSL_PROTO_DTLS)
    mbedtls_ssl_set_mtu(&m_env->ssl, c_session->mtu);
#endif /* !ESPIDF_VERSION || CONFIG_MBEDTLS_SSL_PROTO_DTLS */
    set_ciphersuites(&m_env->conf, 0);
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
    free(m_env);
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
  char buf[128];

  ret = mbedtls_ssl_handshake(&m_env->ssl);
  switch (ret) {
  case 0:
    m_env->established = 1;
    coap_log(LOG_DEBUG, "*  %s: MbedTLS established\n",
                                            coap_session_str(c_session));
    ret = 1;
    break;
  case MBEDTLS_ERR_SSL_WANT_READ:
  case MBEDTLS_ERR_SSL_WANT_WRITE:
    errno = EAGAIN;
    ret = 0;
    break;
  case MBEDTLS_ERR_SSL_HELLO_VERIFY_REQUIRED:
    coap_log(LOG_INFO, "hello verification requested\n");
    ret = -1;
    mbedtls_ssl_session_reset(&m_env->ssl);
    break;
  case MBEDTLS_ERR_SSL_FATAL_ALERT_MESSAGE:
    c_session->dtls_event = COAP_EVENT_DTLS_CLOSED;
    ret = -1;
    break;
  default:
    mbedtls_strerror(ret, buf, sizeof(buf));
    coap_log(LOG_WARNING,
             "do_mbedtls_handshake: session establish "
             "returned -0x%x: '%s'\n",
             -ret, buf);
    ret = -1;
    break;
  }
  return ret;
}

static void
mbedtls_debug_out(void *ctx UNUSED, int level,
                  const char *file, int line, const char *str) {
  int log_level;

  switch (level) {
  case 4:
  case 3:
  case 2:
    log_level = LOG_DEBUG;
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

  m_env = (coap_mbedtls_env_t *)calloc(1, sizeof(coap_mbedtls_env_t));
  if (!m_env) {
      return NULL;
  }

  mbedtls_ssl_init(&m_env->ssl);
  mbedtls_ctr_drbg_init(&m_env->ctr_drbg);
  mbedtls_ssl_config_init(&m_env->conf);
  mbedtls_entropy_init(&m_env->entropy);

#if defined(ESPIDF_VERSION) && defined(CONFIG_MBEDTLS_DEBUG)
  mbedtls_esp_enable_debug_log(&m_env->conf, CONFIG_MBEDTLS_DEBUG_LEVEL);
#endif /* ESPIDF_VERSION && CONFIG_MBEDTLS_DEBUG */
  if ((ret = mbedtls_ctr_drbg_seed(&m_env->ctr_drbg,
                  mbedtls_entropy_func, &m_env->entropy, NULL, 0)) != 0) {
    coap_log(LOG_ERR, "mbedtls_ctr_drbg_seed returned -0x%x", -ret);
    goto fail;
  }

  if (role == COAP_DTLS_ROLE_CLIENT) {
    if (setup_client_ssl_session(c_session, m_env) != 0) {
      goto fail;
    }
  } else if (role == COAP_DTLS_ROLE_SERVER) {
    if (setup_server_ssl_session(c_session, m_env) != 0) {
      goto fail;
    }
  } else {
    goto fail;
  }

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
    free(m_env);
  }
  return NULL;
}

int coap_dtls_is_supported(void) {
#if !defined(ESPIDF_VERSION) || defined(CONFIG_MBEDTLS_SSL_PROTO_DTLS)
  return 1;
#else /* ESPIDF_VERSION && !CONFIG_MBEDTLS_SSL_PROTO_DTLS */
  coap_log(LOG_EMERG,
        "libcoap not compiled for DTLS with MbedTLS"
        " - update MbedTLS to include DTLS\n");
  return 0;
#endif /* ESPIDF_VERSION && !CONFIG_MBEDTLS_SSL_PROTO_DTLS */
}

int coap_tls_is_supported(void)
{
  return 0;
}

void *coap_dtls_new_context(struct coap_context_t *c_context)
{
  coap_mbedtls_context_t *m_context;
  (void)c_context;

  m_context = (coap_mbedtls_context_t *)calloc(1, sizeof(coap_mbedtls_context_t));
  if (m_context) {
      memset(m_context, 0, sizeof(coap_mbedtls_context_t));
  }
  return m_context;
}

#ifndef PSK2_PR
int coap_dtls_context_set_psk(struct coap_context_t *c_context,
                          const char *identity_hint UNUSED,
                          coap_dtls_role_t role UNUSED)
{
  coap_mbedtls_context_t *m_context =
              ((coap_mbedtls_context_t *)c_context->dtls_context);
  m_context->psk_pki_enabled |= IS_PSK;
  return 1;
}
#else /* PSK2_PR */
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
  coap_mbedtls_context_t *m_context =
                         ((coap_mbedtls_context_t *)c_context->dtls_context);

  if (!m_context || !setup_data)
    return 0;

  if (setup_data->validate_ih_call_back) {
    coap_log(LOG_WARNING,
        "CoAP Client with MbedTLS does not support Identity Hint selection\n");
  }
  m_context->psk_pki_enabled |= IS_PSK;
  return 1;
}

#endif /* PSK2_PR */

int coap_dtls_context_set_pki(struct coap_context_t *c_context,
                          coap_dtls_pki_t *setup_data,
                          coap_dtls_role_t role UNUSED)
{
  coap_mbedtls_context_t *m_context =
             ((coap_mbedtls_context_t *)c_context->dtls_context);

  m_context->setup_data = *setup_data;
  m_context->psk_pki_enabled |= IS_PKI;
  return 1;
}

int coap_dtls_context_set_pki_root_cas(struct coap_context_t *c_context,
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
      free(m_context->root_ca_file);
      m_context->root_ca_file = NULL;
  }

  if (ca_file) {
    m_context->root_ca_file = mbedtls_strdup(ca_file);
  }

  if (m_context->root_ca_path) {
    free(m_context->root_ca_path);
    m_context->root_ca_path = NULL;
  }

  if (ca_path) {
    m_context->root_ca_path = mbedtls_strdup(ca_path);
  }
  return 1;
}

int coap_dtls_context_check_keys_enabled(struct coap_context_t *c_context)
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
#ifdef PSK2_PR
  for (i = 0; i < m_context->psk_sni_count; i++) {
    mbedtls_free(m_context->psk_sni_entry_list[i].sni.s);
  }
  if (m_context->psk_sni_entry_list)
    mbedtls_free(m_context->pki_sni_entry_list);

#endif /* PSK2_PR */

  free(m_context);
}

void *coap_dtls_new_client_session(coap_session_t *c_session)
{
  coap_mbedtls_env_t *m_env = coap_dtls_new_mbedtls_env(c_session,
                                                       COAP_DTLS_ROLE_CLIENT);
  int ret;

  if (m_env) {
    ret = do_mbedtls_handshake(c_session, m_env);
    if (ret == -1) {
      coap_dtls_free_mbedtls_env(m_env);
      return NULL;
    }
  }
  return m_env;
}

void *coap_dtls_new_server_session(coap_session_t *c_session)
{
  coap_mbedtls_env_t *m_env =
         (coap_mbedtls_env_t *)c_session->tls;
  if (m_env) {
    m_env->seen_client_hello = 1;
#if !defined(ESPIDF_VERSION) || defined(CONFIG_MBEDTLS_SSL_PROTO_DTLS)
    mbedtls_ssl_set_mtu(&m_env->ssl, c_session->mtu);
#endif /* !ESPIDF_VERSION || CONFIG_MBEDTLS_SSL_PROTO_DTLS */
  }
  return m_env;
}

void coap_dtls_free_session(coap_session_t *c_session)
{
  if (c_session && c_session->context) {
    coap_dtls_free_mbedtls_env(c_session->tls);
    c_session->tls = NULL;
  }
  return;
}

void coap_dtls_session_update_mtu(coap_session_t *c_session)
{
#if !defined(ESPIDF_VERSION) || defined(CONFIG_MBEDTLS_SSL_PROTO_DTLS)
  coap_mbedtls_env_t *m_env =
         (coap_mbedtls_env_t *)c_session->tls;
  if (m_env) {
    mbedtls_ssl_set_mtu(&m_env->ssl, c_session->mtu);
  }
#endif /* !ESPIDF_VERSION || CONFIG_MBEDTLS_SSL_PROTO_DTLS */
}

int coap_dtls_send(coap_session_t *c_session,
                   const uint8_t *data,
                   size_t data_len)
{
  int ret;
  coap_mbedtls_env_t *m_env = (coap_mbedtls_env_t *)c_session->tls;
  char buf[128];

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
        mbedtls_strerror(ret, buf, sizeof(buf));
        coap_log(LOG_WARNING,
                 "coap_dtls_send: "
                 "returned -0x%x: '%s'\n",
                 -ret, buf);
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

coap_tick_t coap_dtls_get_context_timeout(void *dtls_context UNUSED)
{
  return 0;
}

coap_tick_t coap_dtls_get_timeout(coap_session_t *c_session, coap_tick_t now)
{
  coap_mbedtls_env_t *m_env = (coap_mbedtls_env_t *)c_session->tls;
  int ret = mbedtls_timing_get_delay(&m_env->timer);

  switch (ret) {
  case 0:
  case 1:
    /* int_ms has timed out, but not fin_ms */
    return now + 1;
  case 2:
    /* fin_ms has timed out - time for a retry */
    return now;
  default:
    break;
  }

  return 0;
}

void coap_dtls_handle_timeout(coap_session_t *c_session)
{
  coap_mbedtls_env_t *m_env = (coap_mbedtls_env_t *)c_session->tls;

  assert(m_env != NULL);
  if (((c_session->state == COAP_SESSION_STATE_HANDSHAKE) &&
       (++c_session->dtls_timeout_count > c_session->max_retransmit)) ||
      (do_mbedtls_handshake(c_session, m_env) < 0)) {
    /* Too many retries */
    coap_session_disconnected(c_session, COAP_NACK_TLS_FAILED);
  }
  return;
}

int coap_dtls_receive(coap_session_t *c_session,
                      const uint8_t *data,
                      size_t data_len)
{
  int ret = 1;

  c_session->dtls_event = -1;
  coap_mbedtls_env_t *m_env = (coap_mbedtls_env_t *)c_session->tls;
  assert(m_env != NULL);

  coap_ssl_t *ssl_data = &m_env->coap_ssl_data;
  if (ssl_data->pdu_len) {
    coap_log(LOG_INFO, "** %s: Previous data not read %u bytes\n",
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

    ret = mbedtls_ssl_read(&m_env->ssl, pdu, (int)sizeof(pdu));
    if (ret > 0) {
      ret = coap_handle_dgram(c_session->context, c_session, pdu, (size_t)ret);
#if COAP_CONSTRAINED_STACK
      coap_mutex_unlock(&b_static_mutex);
#endif /* COAP_CONSTRAINED_STACK */
      return ret;
    }
    else if (ret == 0 || ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
      c_session->dtls_event = COAP_EVENT_DTLS_CLOSED;
    }
    else if (ret != MBEDTLS_ERR_SSL_WANT_READ) {
      char buf[128];

      mbedtls_strerror(ret, buf, sizeof(buf));
      coap_log(LOG_WARNING,
               "coap_dtls_receive: "
               "returned -0x%x: '%s' (length %zd)\n",
               -ret, buf, data_len);
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
    coap_handle_event(c_session->context, c_session->dtls_event, c_session);
    if (c_session->dtls_event == COAP_EVENT_DTLS_ERROR ||
      c_session->dtls_event == COAP_EVENT_DTLS_CLOSED) {
      coap_session_disconnected(c_session, COAP_NACK_TLS_FAILED);
      ret = -1;
    }
  }
  return ret;
}

int coap_dtls_hello(coap_session_t *c_session,
                    const uint8_t *data,
                    size_t data_len)
{
#if defined(ESPIDF_VERSION) && !defined(CONFIG_MBEDTLS_SSL_PROTO_DTLS)
  (void)c_session;
  (void)data;
  (void)data_len;
  return -1;
#else /* !ESPIDF_VERSION) || CONFIG_MBEDTLS_SSL_PROTO_DTLS */
  coap_mbedtls_env_t *m_env = (coap_mbedtls_env_t *)c_session->tls;
  coap_ssl_t *ssl_data = m_env ? &m_env->coap_ssl_data : NULL;
  int ret;

  if (m_env) {
    char *str = get_ip_addr(&c_session->remote_addr);
    if (!str) {
      return -1;
    }
    if((ret = mbedtls_ssl_set_client_transport_id(&m_env->ssl,
                                   (unsigned char *)str, strlen(str))) != 0) {
      coap_log(LOG_ERR,
               "mbedtls_ssl_set_client_transport_id() returned -0x%x\n\n",
               -ret);
      free(str);
      return -1;
    }
    free(str);
  }

  if (!m_env) {
    m_env = coap_dtls_new_mbedtls_env(c_session, COAP_DTLS_ROLE_SERVER);
    if (m_env) {
      c_session->tls = m_env;
      ssl_data = &m_env->coap_ssl_data;
      ssl_data->pdu = data;
      ssl_data->pdu_len = (unsigned)data_len;
      char *str = get_ip_addr(&c_session->remote_addr);
      if (!str) {
        return -1;
      }
      if((ret = mbedtls_ssl_set_client_transport_id(&m_env->ssl,
                    (unsigned char *)str, strlen(str)) ) != 0) {
        coap_log(LOG_ERR,
                   "mbedtls_ssl_set_client_transport_id() returned -0x%x\n",
                   -ret);
        free(str);
        return -1;
      }
      ret = do_mbedtls_handshake(c_session, m_env);
      if (ret == 0 || m_env->seen_client_hello) {
        m_env->seen_client_hello = 0;
        free(str);
        return 1;
      }
      free(str);
    }
    return 0;
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
     return 1;
  }
  return 0;
#endif /* !ESPIDF_VERSION || CONFIG_MBEDTLS_SSL_PROTO_DTLS */
}

unsigned int coap_dtls_get_overhead(coap_session_t *c_session UNUSED)
{
  return 13 + 8 + 8;
}

void *coap_tls_new_client_session(coap_session_t *c_session UNUSED, int *connected UNUSED)
{
  return NULL;
}

void *coap_tls_new_server_session(coap_session_t *c_session UNUSED, int *connected UNUSED)
{
  return NULL;
}

void coap_tls_free_session( coap_session_t *c_session UNUSED)
{
  return;
}

ssize_t coap_tls_write(coap_session_t *c_session UNUSED,
                       const uint8_t *data UNUSED,
                       size_t data_len UNUSED
                       )
{
  return 0;
}

ssize_t coap_tls_read(coap_session_t *c_session UNUSED,
                      uint8_t *data UNUSED,
                      size_t data_len UNUSED
                      )
{
  return 0;
}

void coap_dtls_startup(void)
{
  return;
}

static int keep_log_level = 0;

void coap_dtls_set_log_level(int level)
{
#if !defined(ESPIDF_VERSION)
  int use_level;
  /*
   * MbedTLS debug levels filter
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
  return;
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
