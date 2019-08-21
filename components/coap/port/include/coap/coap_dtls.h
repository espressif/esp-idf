/*
 * coap_dtls.h -- (Datagram) Transport Layer Support for libcoap
 *
 * Copyright (C) 2016 Olaf Bergmann <bergmann@tzi.org>
 * Copyright (C) 2017 Jean-Claude Michelou <jcm@spinetix.com>
 *
 * This file is part of the CoAP library libcoap. Please see README for terms
 * of use.
 */

#ifndef COAP_DTLS_H_
#define COAP_DTLS_H_

#include "coap_time.h"
#include "str.h"

struct coap_context_t;
struct coap_session_t;
struct coap_dtls_pki_t;

/**
 * @defgroup dtls DTLS Support
 * API functions for interfacing with DTLS libraries.
 * @{
 */

/**
 * Check whether DTLS is available.
 *
 * @return @c 1 if support for DTLS is enabled, or @c 0 otherwise.
 */
int coap_dtls_is_supported(void);

/**
 * Check whether TLS is available.
 *
 * @return @c 1 if support for TLS is enabled, or @c 0 otherwise.
 */
int coap_tls_is_supported(void);

typedef enum coap_tls_library_t {
  COAP_TLS_LIBRARY_NOTLS = 0, /**< No DTLS library */
  COAP_TLS_LIBRARY_TINYDTLS,  /**< Using TinyDTLS library */
  COAP_TLS_LIBRARY_OPENSSL,   /**< Using OpenSSL library */
  COAP_TLS_LIBRARY_GNUTLS,    /**< Using GnuTLS library */
  COAP_TLS_LIBRARY_MBEDTLS,   /**< Using MbedTLS library */
} coap_tls_library_t;

/**
 * The structure used for returning the underlying (D)TLS library
 * information.
 */
typedef struct coap_tls_version_t {
  uint64_t version; /**< (D)TLS runtime Library Version */
  coap_tls_library_t type; /**< Library type. One of COAP_TLS_LIBRARY_* */
  uint64_t built_version; /**< (D)TLS Built against Library Version */
} coap_tls_version_t;

/**
 * Determine the type and version of the underlying (D)TLS library.
 *
 * @return The version and type of library libcoap was compiled against.
 */
coap_tls_version_t *coap_get_tls_library_version(void);

/**
 * Additional Security setup handler that can be set up by
 * coap_context_set_pki().
 * Invoked when libcoap has done the validation checks at the TLS level,
 * but the application needs to do some additional checks/changes/updates.
 *
 * @param tls_session The security session definition - e.g. SSL * for OpenSSL.
 *                    NULL if server call-back.
 *                    This will be dependent on the underlying TLS library -
 *                    see coap_get_tls_library_version()
 * @param setup_data A structure containing setup data originally passed into
 *                   coap_context_set_pki() or coap_new_client_session_pki().
 *
 * @return @c 1 if successful, else @c 0.
 */
typedef int (*coap_dtls_security_setup_t)(void* tls_session,
                                        struct coap_dtls_pki_t *setup_data);

/**
 * CN Validation call-back that can be set up by coap_context_set_pki().
 * Invoked when libcoap has done the validation checks at the TLS level,
 * but the application needs to check that the CN is allowed.
 * CN is the SubjectAltName in the cert, if not present, then the leftmost
 * Common Name (CN) component of the subject name.
 *
 * @param cn  The determined CN from the certificate
 * @param asn1_public_cert  The ASN.1 DER encoded X.509 certificate
 * @param asn1_length  The ASN.1 length
 * @param coap_session  The CoAP session associated with the certificate update
 * @param depth  Depth in cert chain.  If 0, then client cert, else a CA
 * @param validated  TLS layer can find no issues if 1
 * @param arg  The same as was passed into coap_context_set_pki()
 *             in setup_data->cn_call_back_arg
 *
 * @return @c 1 if accepted, else @c 0 if to be rejected.
 */
typedef int (*coap_dtls_cn_callback_t)(const char *cn,
             const uint8_t *asn1_public_cert,
             size_t asn1_length,
             struct coap_session_t *coap_session,
             unsigned depth,
             int validated,
             void *arg);

/**
 * The enum used for determining the provided PKI ASN.1 (DER) Private Key
 * formats.
 */
typedef enum coap_asn1_privatekey_type_t {
  COAP_ASN1_PKEY_NONE,     /**< NONE */
  COAP_ASN1_PKEY_RSA,      /**< RSA type */
  COAP_ASN1_PKEY_RSA2,     /**< RSA2 type */
  COAP_ASN1_PKEY_DSA,      /**< DSA type */
  COAP_ASN1_PKEY_DSA1,     /**< DSA1 type */
  COAP_ASN1_PKEY_DSA2,     /**< DSA2 type */
  COAP_ASN1_PKEY_DSA3,     /**< DSA3 type */
  COAP_ASN1_PKEY_DSA4,     /**< DSA4 type */
  COAP_ASN1_PKEY_DH,       /**< DH type */
  COAP_ASN1_PKEY_DHX,      /**< DHX type */
  COAP_ASN1_PKEY_EC,       /**< EC type */
  COAP_ASN1_PKEY_HMAC,     /**< HMAC type */
  COAP_ASN1_PKEY_CMAC,     /**< CMAC type */
  COAP_ASN1_PKEY_TLS1_PRF, /**< TLS1_PRF type */
  COAP_ASN1_PKEY_HKDF      /**< HKDF type */
} coap_asn1_privatekey_type_t;

/**
 * The enum used for determining the PKI key formats.
 */
typedef enum coap_pki_key_t {
  COAP_PKI_KEY_PEM = 0,        /**< The PKI key type is PEM file */
  COAP_PKI_KEY_ASN1,           /**< The PKI key type is ASN.1 (DER) */
  COAP_PKI_KEY_PEM_BUF,        /**< The PKI key type is PEM buffer */
} coap_pki_key_t;

/**
 * The structure that holds the PKI PEM definitions.
 */
typedef struct coap_pki_key_pem_t {
  const char *ca_file;       /**< File location of Common CA in PEM format */
  const char *public_cert;   /**< File location of Public Cert in PEM format */
  const char *private_key;   /**< File location of Private Key in PEM format */
} coap_pki_key_pem_t;

/**
 * The structure that holds the PKI PEM buffer definitions.
 */
typedef struct coap_pki_key_pem_buf_t {
  const uint8_t *ca_cert;     /**< PEM buffer Common CA Cert */
  const uint8_t *public_cert; /**< PEM buffer Public Cert */
  const uint8_t *private_key; /**< PEM buffer Private Key */
  size_t ca_cert_len;         /**< PEM buffer CA Cert length */
  size_t public_cert_len;     /**< PEM buffer Public Cert length */
  size_t private_key_len;     /**< PEM buffer Private Key length */
} coap_pki_key_pem_buf_t;

/**
 * The structure that holds the PKI ASN.1 (DER) definitions.
 */
typedef struct coap_pki_key_asn1_t {
  const uint8_t *ca_cert;     /**< ASN1 (DER) Common CA Cert */
  const uint8_t *public_cert; /**< ASN1 (DER) Public Cert */
  const uint8_t *private_key; /**< ASN1 (DER) Private Key */
  size_t ca_cert_len;         /**< ASN1 CA Cert length */
  size_t public_cert_len;     /**< ASN1 Public Cert length */
  size_t private_key_len;     /**< ASN1 Private Key length */
  coap_asn1_privatekey_type_t private_key_type; /**< Private Key Type */
} coap_pki_key_asn1_t;

/**
 * The structure that holds the PKI key information.
 */
typedef struct coap_dtls_key_t {
  coap_pki_key_t key_type;          /**< key format type */
  union {
    coap_pki_key_pem_t pem;          /**< for PEM file keys */
    coap_pki_key_pem_buf_t pem_buf;  /**< for PEM memory keys */
    coap_pki_key_asn1_t asn1;        /**< for ASN.1 (DER) file keys */
  } key;
} coap_dtls_key_t;

/**
 * Server Name Indication (SNI) Validation call-back that can be set up by
 * coap_context_set_pki().
 * Invoked if the SNI is not previously seen and prior to sending a certificate
 * set back to the client so that the appropriate certificate set can be used
 * based on the requesting SNI.
 *
 * @param sni  The requested SNI
 * @param arg  The same as was passed into coap_context_set_pki()
 *             in setup_data->sni_call_back_arg
 *
 * @return New set of certificates to use, or @c NULL if SNI is to be rejected.
 */
typedef coap_dtls_key_t *(*coap_dtls_sni_callback_t)(const char *sni,
             void* arg);


#define COAP_DTLS_PKI_SETUP_VERSION 1 /**< Latest PKI setup version */

/**
 * The structure used for defining the PKI setup data to be used.
 */
typedef struct coap_dtls_pki_t {
  uint8_t version; /** Set to 1 to support this version of the struct */

  /* Options to enable different TLS functionality in libcoap */
  uint8_t verify_peer_cert;        /**< 1 if peer cert is to be verified */
  uint8_t require_peer_cert;       /**< 1 if peer cert is required */
  uint8_t allow_self_signed;       /**< 1 if self signed certs are allowed */
  uint8_t allow_expired_certs;     /**< 1 if expired certs are allowed */
  uint8_t cert_chain_validation;   /**< 1 if to check cert_chain_verify_depth */
  uint8_t cert_chain_verify_depth; /**< recommended depth is 3 */
  uint8_t check_cert_revocation;   /**< 1 if revocation checks wanted */
  uint8_t allow_no_crl;            /**< 1 ignore if CRL not there */
  uint8_t allow_expired_crl;       /**< 1 if expired crl is allowed */
  uint8_t allow_bad_md_hash;       /**< 1 if expired certs are allowed */
  uint8_t allow_short_rsa_length;  /**< 1 if expired certs are allowed */
  uint8_t reserved[4];             /**< Reserved - must be set to 0 for
                                        future compatibility */
                                   /* Size of 4 chosen to align to next
                                    * parameter, so if newly defined option
                                    * it can use one of the reserverd slot so
                                    * no need to change
                                    * COAP_DTLS_PKI_SETUP_VERSION and just
                                    * decrement the reserved[] count.
                                    */

  /** CN check call-back function.
   * If not NULL, is called when the TLS connection has passed the configured
   * TLS options above for the application to verify if the CN is valid.
   */
  coap_dtls_cn_callback_t validate_cn_call_back;
  void *cn_call_back_arg;  /**< Passed in to the CN call-back function */

  /** SNI check call-back function.
   * If not @p NULL, called if the SNI is not previously seen and prior to
   * sending a certificate set back to the client so that the appropriate
   * certificate set can be used based on the requesting SNI.
   */
  coap_dtls_sni_callback_t validate_sni_call_back;
  void *sni_call_back_arg;  /**< Passed in to the sni call-back function */

  /** Additional Security call-back handler that is invoked when libcoap has
   * done the standerd, defined validation checks at the TLS level,
   * If not @p NULL, called from within the TLS Client Hello connection
   * setup.
   */
  coap_dtls_security_setup_t additional_tls_setup_call_back;

  char* client_sni;    /**<  If not NULL, SNI to use in client TLS setup.
                             Owned by the client app and must remain valid
                             during the call to coap_new_client_session_pki() */

  coap_dtls_key_t pki_key;  /**< PKI key definition */
} coap_dtls_pki_t;

/** @} */

/**
 * @defgroup dtls_internal DTLS Support (Internal)
 * Internal API functions for interfacing with DTLS libraries.
 * @{
 */

/**
 * Creates a new DTLS context for the given @p coap_context. This function
 * returns a pointer to a new DTLS context object or @c NULL on error.
 *
 * Internal function.
 *
 * @param coap_context The CoAP context where the DTLS object shall be used.
 *
 * @return A DTLS context object or @c NULL on error.
 */
void *
coap_dtls_new_context(struct coap_context_t *coap_context);

typedef enum coap_dtls_role_t {
  COAP_DTLS_ROLE_CLIENT, /**< Internal function invoked for client */
  COAP_DTLS_ROLE_SERVER  /**< Internal function invoked for server */
} coap_dtls_role_t;

/**
 * Set the DTLS context's default PSK information.
 * This does the PSK specifics following coap_dtls_new_context().
 * If @p COAP_DTLS_ROLE_SERVER, then identity hint will also get set.
 * If @p COAP_DTLS_ROLE_SERVER, then the information will get put into the
 * TLS library's context (from which sessions are derived).
 * If @p COAP_DTLS_ROLE_CLIENT, then the information will get put into the
 * TLS library's session.
 *
 * Internal function.
 *
 * @param coap_context The CoAP context.
 * @param identity_hint The default PSK server identity hint sent to a client.
 *                      Required parameter.  If @p NULL, will be set to "".
 *                      Empty string is a valid hint.
 *                      This parameter is ignored if COAP_DTLS_ROLE_CLIENT
 * @param role  One of @p COAP_DTLS_ROLE_CLIENT or @p COAP_DTLS_ROLE_SERVER
 *
 * @return @c 1 if successful, else @c 0.
 */

int
coap_dtls_context_set_psk(struct coap_context_t *coap_context,
                          const char *identity_hint,
                          coap_dtls_role_t role);

/**
 * Set the DTLS context's default server PKI information.
 * This does the PKI specifics following coap_dtls_new_context().
 * If @p COAP_DTLS_ROLE_SERVER, then the information will get put into the
 * TLS library's context (from which sessions are derived).
 * If @p COAP_DTLS_ROLE_CLIENT, then the information will get put into the
 * TLS library's session.
 *
 * Internal function.
 *
 * @param coap_context The CoAP context.
 * @param setup_data     Setup information defining how PKI is to be setup.
 *                       Required parameter.  If @p NULL, PKI will not be
 *                       set up.
 * @param role  One of @p COAP_DTLS_ROLE_CLIENT or @p COAP_DTLS_ROLE_SERVER
 *
 * @return @c 1 if successful, else @c 0.
 */

int
coap_dtls_context_set_pki(struct coap_context_t *coap_context,
                          coap_dtls_pki_t *setup_data,
                          coap_dtls_role_t role);

/**
 * Set the dtls context's default Root CA information for a client or server.
 *
 * Internal function.
 *
 * @param coap_context   The current coap_context_t object.
 * @param ca_file        If not @p NULL, is the full path name of a PEM encoded
 *                       file containing all the Root CAs to be used.
 * @param ca_dir         If not @p NULL, points to a directory containing PEM
 *                       encoded files containing all the Root CAs to be used.
 *
 * @return @c 1 if successful, else @c 0.
 */

int
coap_dtls_context_set_pki_root_cas(struct coap_context_t *coap_context,
                                   const char *ca_file,
                                   const char *ca_dir);

/**
 * Check whether one of the coap_dtls_context_set_{psk|pki}() functions have
 * been called.
 *
 * Internal function.
 *
 * @param coap_context The current coap_context_t object.
 *
 * @return @c 1 if coap_dtls_context_set_{psk|pki}() called, else @c 0.
 */

int coap_dtls_context_check_keys_enabled(struct coap_context_t *coap_context);

/**
 * Releases the storage allocated for @p dtls_context.
 *
 * Internal function.
 *
 * @param dtls_context The DTLS context as returned by coap_dtls_new_context().
 */
void coap_dtls_free_context(void *dtls_context);

/**
 * Create a new client-side session. This should send a HELLO to the server.
 *
 * Internal function.
 *
 * @param coap_session   The CoAP session.
 *
 * @return Opaque handle to underlying TLS library object containing security
 *         parameters for the session.
*/
void *coap_dtls_new_client_session(struct coap_session_t *coap_session);

/**
 * Create a new DTLS server-side session.
 * Called after coap_dtls_hello() has returned @c 1, signalling that a validated
 * HELLO was received from a client.
 * This should send a HELLO to the server.
 *
 * Internal function.
 *
 * @param coap_session   The CoAP session.
 *
 * @return Opaque handle to underlying TLS library object containing security
 *         parameters for the DTLS session.
 */
void *coap_dtls_new_server_session(struct coap_session_t *coap_session);

/**
 * Terminates the DTLS session (may send an ALERT if necessary) then frees the
 * underlying TLS library object containing security parameters for the session.
 *
 * Internal function.
 *
 * @param coap_session   The CoAP session.
 */
void coap_dtls_free_session(struct coap_session_t *coap_session);

/**
 * Notify of a change in the CoAP session's MTU, for example after
 * a PMTU update.
 *
 * Internal function.
 *
 * @param coap_session   The CoAP session.
 */
void coap_dtls_session_update_mtu(struct coap_session_t *coap_session);

/**
 * Send data to a DTLS peer.
 *
 * Internal function.
 *
 * @param coap_session The CoAP session.
 * @param data      pointer to data.
 * @param data_len  Number of bytes to send.
 *
 * @return @c 0 if this would be blocking, @c -1 if there is an error or the
 *         number of cleartext bytes sent.
 */
int coap_dtls_send(struct coap_session_t *coap_session,
                   const uint8_t *data,
                   size_t data_len);

/**
 * Check if timeout is handled per CoAP session or per CoAP context.
 *
 * Internal function.
 *
 * @return @c 1 of timeout and retransmit is per context, @c 0 if it is
 *         per session.
 */
int coap_dtls_is_context_timeout(void);

/**
 * Do all pending retransmits and get next timeout
 *
 * Internal function.
 *
 * @param dtls_context The DTLS context.
 *
 * @return @c 0 if no event is pending or date of the next retransmit.
 */
coap_tick_t coap_dtls_get_context_timeout(void *dtls_context);

/**
 * Get next timeout for this session.
 *
 * Internal function.
 *
 * @param coap_session The CoAP session.
 * @param now The current time in ticks.
 *
 * @return @c 0 If no event is pending or ticks time of the next retransmit.
 */
coap_tick_t coap_dtls_get_timeout(struct coap_session_t *coap_session,
                                  coap_tick_t now);

/**
 * Handle a DTLS timeout expiration.
 *
 * Internal function.
 *
 * @param coap_session The CoAP session.
 */
void coap_dtls_handle_timeout(struct coap_session_t *coap_session);

/**
 * Handling incoming data from a DTLS peer.
 *
 * Internal function.
 *
 * @param coap_session The CoAP session.
 * @param data      Encrypted datagram.
 * @param data_len  Encrypted datagram size.
 *
 * @return Result of coap_handle_dgram on the decrypted CoAP PDU
 *         or @c -1 for error.
 */
int coap_dtls_receive(struct coap_session_t *coap_session,
                      const uint8_t *data,
                      size_t data_len);

/**
 * Handling client HELLO messages from a new candiate peer.
 * Note that session->tls is empty.
 *
 * Internal function.
 *
 * @param coap_session The CoAP session.
 * @param data      Encrypted datagram.
 * @param data_len  Encrypted datagram size.
 *
 * @return @c 0 if a cookie verification message has been sent, @c 1 if the
 *        HELLO contains a valid cookie and a server session should be created,
 *        @c -1 if the message is invalid.
 */
int coap_dtls_hello(struct coap_session_t *coap_session,
                    const uint8_t *data,
                    size_t data_len);

/**
 * Get DTLS overhead over cleartext PDUs.
 *
 * Internal function.
 *
 * @param coap_session The CoAP session.
 *
 * @return Maximum number of bytes added by DTLS layer.
 */
unsigned int coap_dtls_get_overhead(struct coap_session_t *coap_session);

/**
 * Create a new TLS client-side session.
 *
 * Internal function.
 *
 * @param coap_session The CoAP session.
 * @param connected Updated with whether the connection is connected yet or not.
 *                  @c 0 is not connected, @c 1 is connected.
 *
 * @return Opaque handle to underlying TLS library object containing security
 *         parameters for the session.
*/
void *coap_tls_new_client_session(struct coap_session_t *coap_session, int *connected);

/**
 * Create a TLS new server-side session.
 *
 * Internal function.
 *
 * @param coap_session The CoAP session.
 * @param connected Updated with whether the connection is connected yet or not.
 *                  @c 0 is not connected, @c 1 is connected.
 *
 * @return Opaque handle to underlying TLS library object containing security
 *         parameters for the session.
 */
void *coap_tls_new_server_session(struct coap_session_t *coap_session, int *connected);

/**
 * Terminates the TLS session (may send an ALERT if necessary) then frees the
 * underlying TLS library object containing security parameters for the session.
 *
 * Internal function.
 *
 * @param coap_session The CoAP session.
 */
void coap_tls_free_session( struct coap_session_t *coap_session );

/**
 * Send data to a TLS peer, with implicit flush.
 *
 * Internal function.
 *
 * @param coap_session The CoAP session.
 * @param data      Pointer to data.
 * @param data_len  Number of bytes to send.
 *
 * @return          @c 0 if this should be retried, @c -1 if there is an error
 *                  or the number of cleartext bytes sent.
 */
ssize_t coap_tls_write(struct coap_session_t *coap_session,
                       const uint8_t *data,
                       size_t data_len
                       );

/**
 * Read some data from a TLS peer.
 *
 * Internal function.
 *
 * @param coap_session The CoAP session.
 * @param data      Pointer to data.
 * @param data_len  Maximum number of bytes to read.
 *
 * @return          @c 0 if this should be retried, @c -1 if there is an error
 *                  or the number of cleartext bytes read.
 */
ssize_t coap_tls_read(struct coap_session_t *coap_session,
                      uint8_t *data,
                      size_t data_len
                      );

/**
 * Initialize the underlying (D)TLS Library layer.
 *
 * Internal function.
 *
 */
void coap_dtls_startup(void);

/** @} */

/**
 * @ingroup logging
 * Sets the (D)TLS logging level to the specified @p level.
 * Note: coap_log_level() will influence output if at a specified level.
 *
 * @param level The logging level to use - LOG_*
 */
void coap_dtls_set_log_level(int level);

/**
 * @ingroup logging
 * Get the current (D)TLS logging.
 *
 * @return The current log level (one of LOG_*).
 */
int coap_dtls_get_log_level(void);


#endif /* COAP_DTLS_H */
