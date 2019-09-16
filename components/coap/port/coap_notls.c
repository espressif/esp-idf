/*
* coap_notls.c -- Stub Datagram Transport Layer Support for libcoap
*
* Copyright (C) 2016 Olaf Bergmann <bergmann@tzi.org>
*
* This file is part of the CoAP library libcoap. Please see README for terms
* of use.
*/

#include "coap_config.h"

#if !defined(HAVE_LIBTINYDTLS) && !defined(HAVE_OPENSSL) && !defined(HAVE_LIBGNUTLS) && !defined(HAVE_MBEDTLS)

#include "net.h"

#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#else /* __GNUC__ */
#define UNUSED
#endif /* __GNUC__ */

int
coap_dtls_is_supported(void) {
  return 0;
}

int
coap_tls_is_supported(void) {
  return 0;
}

coap_tls_version_t *
coap_get_tls_library_version(void) {
  static coap_tls_version_t version;
  version.version = 0;
  version.type = COAP_TLS_LIBRARY_NOTLS;
  return &version;
}

int
coap_dtls_context_set_pki(coap_context_t *ctx UNUSED,
                          coap_dtls_pki_t* setup_data UNUSED,
                          coap_dtls_role_t role UNUSED
) {
  return 0;
}

int
coap_dtls_context_set_pki_root_cas(struct coap_context_t *ctx UNUSED,
                                   const char *ca_file UNUSED,
                                   const char *ca_path UNUSED
) {
  return 0;
}

int
coap_dtls_context_set_psk(coap_context_t *ctx UNUSED,
                          const char *hint UNUSED,
                          coap_dtls_role_t role UNUSED
) {
  return 0;
}

int
coap_dtls_context_check_keys_enabled(coap_context_t *ctx UNUSED)
{
  return 0;
}

static int dtls_log_level = 0;

void coap_dtls_startup(void) {
}

void
coap_dtls_set_log_level(int level) {
  dtls_log_level = level;
}

int
coap_dtls_get_log_level(void) {
  return dtls_log_level;
}

void *
coap_dtls_new_context(struct coap_context_t *coap_context UNUSED) {
  return NULL;
}

void
coap_dtls_free_context(void *handle UNUSED) {
}

void *coap_dtls_new_server_session(coap_session_t *session UNUSED) {
  return NULL;
}

void *coap_dtls_new_client_session(coap_session_t *session UNUSED) {
  return NULL;
}

void coap_dtls_free_session(coap_session_t *coap_session UNUSED) {
}

void coap_dtls_session_update_mtu(coap_session_t *session UNUSED) {
}

int
coap_dtls_send(coap_session_t *session UNUSED,
  const uint8_t *data UNUSED,
  size_t data_len UNUSED
) {
  return -1;
}

int coap_dtls_is_context_timeout(void) {
  return 1;
}

coap_tick_t coap_dtls_get_context_timeout(void *dtls_context UNUSED) {
  return 0;
}

coap_tick_t
coap_dtls_get_timeout(coap_session_t *session UNUSED, coap_tick_t now UNUSED) {
  return 0;
}

void coap_dtls_handle_timeout(coap_session_t *session UNUSED) {
}

int
coap_dtls_receive(coap_session_t *session UNUSED,
  const uint8_t *data UNUSED,
  size_t data_len UNUSED
) {
  return -1;
}

int
coap_dtls_hello(coap_session_t *session UNUSED,
  const uint8_t *data UNUSED,
  size_t data_len UNUSED
) {
  return 0;
}

unsigned int coap_dtls_get_overhead(coap_session_t *session UNUSED) {
  return 0;
}

void *coap_tls_new_client_session(coap_session_t *session UNUSED, int *connected UNUSED) {
  return NULL;
}

void *coap_tls_new_server_session(coap_session_t *session UNUSED, int *connected UNUSED) {
  return NULL;
}

void coap_tls_free_session(coap_session_t *coap_session UNUSED) {
}

ssize_t coap_tls_write(coap_session_t *session UNUSED,
                       const uint8_t *data UNUSED,
                       size_t data_len UNUSED
) {
  return -1;
}

ssize_t coap_tls_read(coap_session_t *session UNUSED,
                      uint8_t *data UNUSED,
                      size_t data_len UNUSED
) {
  return -1;
}

#undef UNUSED

#else /* !HAVE_LIBTINYDTLS && !HAVE_OPENSSL && !HAVE_LIBGNUTLS */

#ifdef __clang__
/* Make compilers happy that do not like empty modules. As this function is
 * never used, we ignore -Wunused-function at the end of compiling this file
 */
#pragma GCC diagnostic ignored "-Wunused-function"
#endif
static inline void dummy(void) {
}

#endif /* !HAVE_LIBTINYDTLS && !HAVE_OPENSSL && !HAVE_LIBGNUTLS */
