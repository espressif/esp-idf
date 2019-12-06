// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _ESP_TLS_H_
#define _ESP_TLS_H_

#include <stdbool.h>
#include <sys/socket.h>
#include <fcntl.h>
#include "esp_err.h"
#ifdef CONFIG_ESP_TLS_USING_MBEDTLS
#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/esp_debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"
#elif CONFIG_ESP_TLS_USING_WOLFSSL
#include "wolfssl/wolfcrypt/settings.h"
#include "wolfssl/ssl.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_ERR_ESP_TLS_BASE           0x8000             /*!< Starting number of ESP-TLS error codes */
#define ESP_ERR_ESP_TLS_CANNOT_RESOLVE_HOSTNAME           (ESP_ERR_ESP_TLS_BASE + 0x01)  /*!< Error if hostname couldn't be resolved upon tls connection */
#define ESP_ERR_ESP_TLS_CANNOT_CREATE_SOCKET              (ESP_ERR_ESP_TLS_BASE + 0x02)  /*!< Failed to create socket */
#define ESP_ERR_ESP_TLS_UNSUPPORTED_PROTOCOL_FAMILY       (ESP_ERR_ESP_TLS_BASE + 0x03)  /*!< Unsupported protocol family */
#define ESP_ERR_ESP_TLS_FAILED_CONNECT_TO_HOST            (ESP_ERR_ESP_TLS_BASE + 0x04)  /*!< Failed to connect to host */
#define ESP_ERR_ESP_TLS_SOCKET_SETOPT_FAILED              (ESP_ERR_ESP_TLS_BASE + 0x05)  /*!< failed to set socket option */
#define ESP_ERR_MBEDTLS_CERT_PARTLY_OK                    (ESP_ERR_ESP_TLS_BASE + 0x06)  /*!< mbedtls parse certificates was partly successful */
#define ESP_ERR_MBEDTLS_CTR_DRBG_SEED_FAILED              (ESP_ERR_ESP_TLS_BASE + 0x07)  /*!< mbedtls api returned error */
#define ESP_ERR_MBEDTLS_SSL_SET_HOSTNAME_FAILED           (ESP_ERR_ESP_TLS_BASE + 0x08)  /*!< mbedtls api returned error */
#define ESP_ERR_MBEDTLS_SSL_CONFIG_DEFAULTS_FAILED        (ESP_ERR_ESP_TLS_BASE + 0x09)  /*!< mbedtls api returned error */
#define ESP_ERR_MBEDTLS_SSL_CONF_ALPN_PROTOCOLS_FAILED    (ESP_ERR_ESP_TLS_BASE + 0x0A)  /*!< mbedtls api returned error */
#define ESP_ERR_MBEDTLS_X509_CRT_PARSE_FAILED             (ESP_ERR_ESP_TLS_BASE + 0x0B)  /*!< mbedtls api returned error */
#define ESP_ERR_MBEDTLS_SSL_CONF_OWN_CERT_FAILED          (ESP_ERR_ESP_TLS_BASE + 0x0C)  /*!< mbedtls api returned error */
#define ESP_ERR_MBEDTLS_SSL_SETUP_FAILED                  (ESP_ERR_ESP_TLS_BASE + 0x0D)  /*!< mbedtls api returned error */
#define ESP_ERR_MBEDTLS_SSL_WRITE_FAILED                  (ESP_ERR_ESP_TLS_BASE + 0x0E)  /*!< mbedtls api returned error */
#define ESP_ERR_MBEDTLS_PK_PARSE_KEY_FAILED               (ESP_ERR_ESP_TLS_BASE + 0x0F)  /*!< mbedtls api returned failed  */
#define ESP_ERR_MBEDTLS_SSL_HANDSHAKE_FAILED              (ESP_ERR_ESP_TLS_BASE + 0x10)  /*!< mbedtls api returned failed  */
#define ESP_ERR_MBEDTLS_SSL_CONF_PSK_FAILED               (ESP_ERR_ESP_TLS_BASE + 0x11)  /*!< mbedtls api returned failed  */

#ifdef CONFIG_ESP_TLS_USING_MBEDTLS
#define ESP_TLS_ERR_SSL_WANT_READ                          MBEDTLS_ERR_SSL_WANT_READ
#define ESP_TLS_ERR_SSL_WANT_WRITE                         MBEDTLS_ERR_SSL_WANT_WRITE
#define ESP_TLS_ERR_SSL_TIMEOUT                            MBEDTLS_ERR_SSL_TIMEOUT
#elif CONFIG_ESP_TLS_USING_WOLFSSL /* CONFIG_ESP_TLS_USING_MBEDTLS */
#define ESP_TLS_ERR_SSL_WANT_READ                          WOLFSSL_ERROR_WANT_READ
#define ESP_TLS_ERR_SSL_WANT_WRITE                         WOLFSSL_ERROR_WANT_WRITE
#define ESP_TLS_ERR_SSL_TIMEOUT                            WOLFSSL_CBIO_ERR_TIMEOUT
#endif /*CONFIG_ESP_TLS_USING_WOLFSSL */
typedef struct esp_tls_last_error* esp_tls_error_handle_t;

/**
*  @brief Error structure containing relevant errors in case tls error occurred
*/
typedef struct esp_tls_last_error {
    esp_err_t last_error;               /*!< error code (based on ESP_ERR_ESP_TLS_BASE) of the last occurred error */
    int       esp_tls_error_code;       /*!< esp_tls error code from last esp_tls failed api */
    int       esp_tls_flags;            /*!< last certification verification flags */
} esp_tls_last_error_t;

/**
 *  @brief ESP-TLS Connection State
 */
typedef enum esp_tls_conn_state {
    ESP_TLS_INIT = 0,
    ESP_TLS_CONNECTING,
    ESP_TLS_HANDSHAKE,
    ESP_TLS_FAIL,
    ESP_TLS_DONE,
} esp_tls_conn_state_t;

typedef enum esp_tls_role {
    ESP_TLS_CLIENT = 0,
    ESP_TLS_SERVER,
} esp_tls_role_t;

/**
 *  @brief ESP-TLS preshared key and hint structure
 */
typedef struct psk_key_hint {
    const uint8_t* key;                     /*!< key in PSK authentication mode in binary format */
    const size_t   key_size;                /*!< length of the key */
    const char* hint;                       /*!< hint in PSK authentication mode in string format */
} psk_hint_key_t;

/**
 * @brief      ESP-TLS configuration parameters 
 * 
 * @note       Note about format of certificates:
 *             - This structure includes certificates of a Certificate Authority, of client or server as well
 *             as private keys, which may be of PEM or DER format. In case of PEM format, the buffer must be
 *             NULL terminated (with NULL character included in certificate size).
 *             - Certificate Authority's certificate may be a chain of certificates in case of PEM format,
 *             but could be only one certificate in case of DER format
 *             - Variables names of certificates and private key buffers and sizes are defined as unions providing
 *             backward compatibility for legacy *_pem_buf and *_pem_bytes names which suggested only PEM format
 *             was supported. It is encouraged to use generic names such as cacert_buf and cacert_bytes.
 */ 
typedef struct esp_tls_cfg {
    const char **alpn_protos;               /*!< Application protocols required for HTTP2.
                                                 If HTTP2/ALPN support is required, a list
                                                 of protocols that should be negotiated. 
                                                 The format is length followed by protocol
                                                 name. 
                                                 For the most common cases the following is ok:
                                                 const char **alpn_protos = { "h2", NULL };
                                                 - where 'h2' is the protocol name */
 
    union {
    const unsigned char *cacert_buf;        /*!< Certificate Authority's certificate in a buffer.
                                                 Format may be PEM or DER, depending on mbedtls-support
                                                 This buffer should be NULL terminated in case of PEM */
    const unsigned char *cacert_pem_buf;    /*!< CA certificate buffer legacy name */
    };

    union {
    unsigned int cacert_bytes;              /*!< Size of Certificate Authority certificate
                                                 pointed to by cacert_buf
                                                 (including NULL-terminator in case of PEM format) */
    unsigned int cacert_pem_bytes;          /*!< Size of Certificate Authority certificate legacy name */
    };

    union {
    const unsigned char *clientcert_buf;    /*!< Client certificate in a buffer
                                                 Format may be PEM or DER, depending on mbedtls-support
                                                 This buffer should be NULL terminated in case of PEM */
    const unsigned char *clientcert_pem_buf;     /*!< Client certificate legacy name */
    };

    union {
    unsigned int clientcert_bytes;          /*!< Size of client certificate pointed to by
                                                 clientcert_pem_buf
                                                 (including NULL-terminator in case of PEM format) */
    unsigned int clientcert_pem_bytes;      /*!< Size of client certificate legacy name */
    };

    union {
    const unsigned char *clientkey_buf;     /*!< Client key in a buffer
                                                 Format may be PEM or DER, depending on mbedtls-support
                                                 This buffer should be NULL terminated in case of PEM */
    const unsigned char *clientkey_pem_buf; /*!< Client key legacy name */
    };

    union {
    unsigned int clientkey_bytes;           /*!< Size of client key pointed to by
                                                 clientkey_pem_buf
                                                 (including NULL-terminator in case of PEM format) */
    unsigned int clientkey_pem_bytes;       /*!< Size of client key legacy name */
    };

    const unsigned char *clientkey_password;/*!< Client key decryption password string */

    unsigned int clientkey_password_len;    /*!< String length of the password pointed to by
                                                 clientkey_password */

    bool non_block;                         /*!< Configure non-blocking mode. If set to true the 
                                                 underneath socket will be configured in non 
                                                 blocking mode after tls session is established */

    int timeout_ms;                         /*!< Network timeout in milliseconds */

    bool use_global_ca_store;               /*!< Use a global ca_store for all the connections in which
                                                 this bool is set. */

    const char *common_name;                /*!< If non-NULL, server certificate CN must match this name.
                                                 If NULL, server certificate CN must match hostname. */

    bool skip_common_name;                  /*!< Skip any validation of server certificate CN field */

    const psk_hint_key_t* psk_hint_key;     /*!< Pointer to PSK hint and key. if not NULL (and certificates are NULL)
                                                 then PSK authentication is enabled with configured setup.
                                                 Important note: the pointer must be valid for connection */

} esp_tls_cfg_t;

#ifdef CONFIG_ESP_TLS_SERVER
typedef struct esp_tls_cfg_server {
    const char **alpn_protos;                   /*!< Application protocols required for HTTP2.
                                                     If HTTP2/ALPN support is required, a list
                                                     of protocols that should be negotiated.
                                                     The format is length followed by protocol
                                                     name.
                                                     For the most common cases the following is ok:
                                                     const char **alpn_protos = { "h2", NULL };
                                                     - where 'h2' is the protocol name */

    union {
    const unsigned char *cacert_buf;        /*!< Client CA certificate in a buffer.
                                                     This buffer should be NULL terminated */
    const unsigned char *cacert_pem_buf;    /*!< Client CA certificate legacy name */
    };

    union {
    unsigned int cacert_bytes;              /*!< Size of client CA certificate
                                                     pointed to by cacert_pem_buf */
    unsigned int cacert_pem_bytes;          /*!< Size of client CA certificate legacy name */
    };

    union {
    const unsigned char *servercert_buf;        /*!< Server certificate in a buffer
                                                     This buffer should be NULL terminated */
    const unsigned char *servercert_pem_buf;    /*!< Server certificate legacy name */
    };

    union {
    unsigned int servercert_bytes;             /*!< Size of server certificate pointed to by
                                                     servercert_pem_buf */
    unsigned int servercert_pem_bytes;          /*!< Size of server certificate legacy name */
    };

    union {
    const unsigned char *serverkey_buf;         /*!< Server key in a buffer
                                                     This buffer should be NULL terminated */
    const unsigned char *serverkey_pem_buf;     /*!< Server key legacy name */
    };

    union {
    unsigned int serverkey_bytes;               /*!< Size of server key pointed to by
                                                     serverkey_pem_buf */
    unsigned int serverkey_pem_bytes;           /*!< Size of server key legacy name */
    };

    const unsigned char *serverkey_password;    /*!< Server key decryption password string */

    unsigned int serverkey_password_len;        /*!< String length of the password pointed to by
                                                     serverkey_password */

} esp_tls_cfg_server_t;
#endif /* ! CONFIG_ESP_TLS_SERVER */

/**
 * @brief      ESP-TLS Connection Handle 
 */
typedef struct esp_tls {
#ifdef CONFIG_ESP_TLS_USING_MBEDTLS
    mbedtls_ssl_context ssl;                                                    /*!< TLS/SSL context */
 
    mbedtls_entropy_context entropy;                                            /*!< mbedTLS entropy context structure */
 
    mbedtls_ctr_drbg_context ctr_drbg;                                          /*!< mbedTLS ctr drbg context structure.
                                                                                     CTR_DRBG is deterministic random 
                                                                                     bit generation based on AES-256 */
 
    mbedtls_ssl_config conf;                                                    /*!< TLS/SSL configuration to be shared 
                                                                                     between mbedtls_ssl_context 
                                                                                     structures */
 
    mbedtls_net_context server_fd;                                              /*!< mbedTLS wrapper type for sockets */
 
    mbedtls_x509_crt cacert;                                                    /*!< Container for the X.509 CA certificate */

    mbedtls_x509_crt *cacert_ptr;                                               /*!< Pointer to the cacert being used. */

    mbedtls_x509_crt clientcert;                                                /*!< Container for the X.509 client certificate */

    mbedtls_pk_context clientkey;                                               /*!< Container for the private key of the client
                                                                                     certificate */
#ifdef CONFIG_ESP_TLS_SERVER
    mbedtls_x509_crt servercert;                                                /*!< Container for the X.509 server certificate */

    mbedtls_pk_context serverkey;                                               /*!< Container for the private key of the server
                                                                                   certificate */
#endif
#elif CONFIG_ESP_TLS_USING_WOLFSSL
    void *priv_ctx;
    void *priv_ssl;
#endif
    int sockfd;                                                                 /*!< Underlying socket file descriptor. */
 
    ssize_t (*read)(struct esp_tls  *tls, char *data, size_t datalen);          /*!< Callback function for reading data from TLS/SSL
                                                                                     connection. */
 
    ssize_t (*write)(struct esp_tls *tls, const char *data, size_t datalen);    /*!< Callback function for writing data to TLS/SSL
                                                                                     connection. */

    esp_tls_conn_state_t  conn_state;                                           /*!< ESP-TLS Connection state */

    fd_set rset;                                                                /*!< read file descriptors */

    fd_set wset;                                                                /*!< write file descriptors */

    bool is_tls;                                                                /*!< indicates connection type (TLS or NON-TLS) */

    esp_tls_role_t role;                                                        /*!< esp-tls role
                                                                                     - ESP_TLS_CLIENT
                                                                                     - ESP_TLS_SERVER */

    esp_tls_error_handle_t error_handle;                                        /*!< handle to error descriptor */

} esp_tls_t;


/**
 * @brief      Create TLS connection
 *
 * This function allocates and initializes esp-tls structure handle.
 *
 * @return      tls     Pointer to esp-tls as esp-tls handle if successfully initialized,
 *                      NULL if allocation error
 */
esp_tls_t *esp_tls_init(void);




/**
 * @brief      Create a new blocking TLS/SSL connection
 *
 * This function establishes a TLS/SSL connection with the specified host in blocking manner.
 *
 * Note: This API is present for backward compatibility reasons. Alternative function
 * with the same functionality is `esp_tls_conn_new_sync` (and its asynchronous version
 * `esp_tls_conn_new_async`)
 *
 * @param[in]  hostname  Hostname of the host.
 * @param[in]  hostlen   Length of hostname.
 * @param[in]  port      Port number of the host.
 * @param[in]  cfg       TLS configuration as esp_tls_cfg_t. If you wish to open
 *                       non-TLS connection, keep this NULL. For TLS connection,
 *                       a pass pointer to esp_tls_cfg_t. At a minimum, this
 *                       structure should be zero-initialized.
 *
 * @return pointer to esp_tls_t, or NULL if connection couldn't be opened.
 */
esp_tls_t *esp_tls_conn_new(const char *hostname, int hostlen, int port, const esp_tls_cfg_t *cfg)  __attribute__ ((deprecated));

/**
 * @brief      Create a new blocking TLS/SSL connection
 *
 * This function establishes a TLS/SSL connection with the specified host in blocking manner.
 *
 * @param[in]  hostname  Hostname of the host.
 * @param[in]  hostlen   Length of hostname.
 * @param[in]  port      Port number of the host.
 * @param[in]  cfg       TLS configuration as esp_tls_cfg_t. If you wish to open
 *                       non-TLS connection, keep this NULL. For TLS connection,
 *                       a pass pointer to esp_tls_cfg_t. At a minimum, this
 *                       structure should be zero-initialized.
 * @param[in]  tls       Pointer to esp-tls as esp-tls handle.
 *
 * @return
 *             - -1      If connection establishment fails.
 *             -  1      If connection establishment is successful.
 *             -  0      Reserved for connection state is in progress.
 */
int esp_tls_conn_new_sync(const char *hostname, int hostlen, int port, const esp_tls_cfg_t *cfg, esp_tls_t *tls);

/**
 * @brief      Create a new blocking TLS/SSL connection with a given "HTTP" url
 *
 * The behaviour is same as esp_tls_conn_new() API. However this API accepts host's url.
 * 
 * @param[in]  url  url of host.
 * @param[in]  cfg  TLS configuration as esp_tls_cfg_t. If you wish to open
 *                  non-TLS connection, keep this NULL. For TLS connection,
 *                  a pass pointer to 'esp_tls_cfg_t'. At a minimum, this
 *                  structure should be zero-initialized.
 * @return pointer to esp_tls_t, or NULL if connection couldn't be opened.
 */
esp_tls_t *esp_tls_conn_http_new(const char *url, const esp_tls_cfg_t *cfg);
   
/**
 * @brief      Create a new non-blocking TLS/SSL connection
 *
 * This function initiates a non-blocking TLS/SSL connection with the specified host, but due to
 * its non-blocking nature, it doesn't wait for the connection to get established.
 *
 * @param[in]  hostname  Hostname of the host.
 * @param[in]  hostlen   Length of hostname.
 * @param[in]  port      Port number of the host.
 * @param[in]  cfg       TLS configuration as esp_tls_cfg_t. `non_block` member of
 *                       this structure should be set to be true.
 * @param[in]  tls       pointer to esp-tls as esp-tls handle.
 *
 * @return
 *             - -1      If connection establishment fails.
 *             -  0      If connection establishment is in progress.
 *             -  1      If connection establishment is successful.
 */
int esp_tls_conn_new_async(const char *hostname, int hostlen, int port, const esp_tls_cfg_t *cfg, esp_tls_t *tls);

/**
 * @brief      Create a new non-blocking TLS/SSL connection with a given "HTTP" url
 *
 * The behaviour is same as esp_tls_conn_new() API. However this API accepts host's url.
 *
 * @param[in]  url     url of host.
 * @param[in]  cfg     TLS configuration as esp_tls_cfg_t.
 * @param[in]  tls     pointer to esp-tls as esp-tls handle.
 *
 * @return
 *             - -1     If connection establishment fails.
 *             -  0     If connection establishment is in progress.
 *             -  1     If connection establishment is successful.
 */
int esp_tls_conn_http_new_async(const char *url, const esp_tls_cfg_t *cfg, esp_tls_t *tls);

/**
 * @brief      Write from buffer 'data' into specified tls connection.
 * 
 * @param[in]  tls      pointer to esp-tls as esp-tls handle.
 * @param[in]  data     Buffer from which data will be written.
 * @param[in]  datalen  Length of data buffer.
 * 
 * @return 
 *             - >0  if write operation was successful, the return value is the number 
 *                   of bytes actually written to the TLS/SSL connection.  
 *             -  0  if write operation was not successful. The underlying
 *                   connection was closed.
 *             - <0  if write operation was not successful, because either an 
 *                   error occured or an action must be taken by the calling process.   
 */
static inline ssize_t esp_tls_conn_write(esp_tls_t *tls, const void *data, size_t datalen)
{
    return tls->write(tls, (char *)data, datalen);
}

/**
 * @brief      Read from specified tls connection into the buffer 'data'.
 * 
 * @param[in]  tls      pointer to esp-tls as esp-tls handle.
 * @param[in]  data     Buffer to hold read data.
 * @param[in]  datalen  Length of data buffer. 
 *
 * @return
 *             - >0  if read operation was successful, the return value is the number
 *                   of bytes actually read from the TLS/SSL connection.
 *             -  0  if read operation was not successful. The underlying
 *                   connection was closed.
 *             - <0  if read operation was not successful, because either an
 *                   error occured or an action must be taken by the calling process.
 */
static inline ssize_t esp_tls_conn_read(esp_tls_t *tls, void  *data, size_t datalen)
{
    return tls->read(tls, (char *)data, datalen);
}

/**
 * @brief      Close the TLS/SSL connection and free any allocated resources.
 * 
 * This function should be called to close each tls connection opened with esp_tls_conn_new() or
 * esp_tls_conn_http_new() APIs. 
 *
 * @param[in]  tls  pointer to esp-tls as esp-tls handle.    
 */
void esp_tls_conn_delete(esp_tls_t *tls);

/**
 * @brief      Return the number of application data bytes remaining to be
 *             read from the current record
 *
 * This API is a wrapper over mbedtls's mbedtls_ssl_get_bytes_avail() API.
 *
 * @param[in]  tls  pointer to esp-tls as esp-tls handle.
 *
 * @return
 *            - -1  in case of invalid arg
 *            - bytes available in the application data
 *              record read buffer
 */
ssize_t esp_tls_get_bytes_avail(esp_tls_t *tls);

/**
 * @brief      Create a global CA store, initially empty.
 *
 * This function should be called if the application wants to use the same CA store for multiple connections.
 * This function initialises the global CA store which can be then set by calling esp_tls_set_global_ca_store().
 * To be effective, this function must be called before any call to esp_tls_set_global_ca_store().
 *
 * @return
 *             - ESP_OK             if creating global CA store was successful.
 *             - ESP_ERR_NO_MEM     if an error occured when allocating the mbedTLS resources.
 */
esp_err_t esp_tls_init_global_ca_store(void);

/**
 * @brief      Set the global CA store with the buffer provided in pem format.
 *
 * This function should be called if the application wants to set the global CA store for
 * multiple connections i.e. to add the certificates in the provided buffer to the certificate chain.
 * This function implicitly calls esp_tls_init_global_ca_store() if it has not already been called.
 * The application must call this function before calling esp_tls_conn_new().
 *
 * @param[in]  cacert_pem_buf    Buffer which has certificates in pem format. This buffer
 *                               is used for creating a global CA store, which can be used
 *                               by other tls connections.
 * @param[in]  cacert_pem_bytes  Length of the buffer.
 *
 * @return
 *             - ESP_OK  if adding certificates was successful.
 *             - Other   if an error occured or an action must be taken by the calling process.
 */
esp_err_t esp_tls_set_global_ca_store(const unsigned char *cacert_pem_buf, const unsigned int cacert_pem_bytes);

/**
 * @brief      Free the global CA store currently being used.
 *
 * The memory being used by the global CA store to store all the parsed certificates is
 * freed up. The application can call this API if it no longer needs the global CA store.
 */
void esp_tls_free_global_ca_store(void);

/**
 * @brief      Returns last error in esp_tls with detailed mbedtls related error codes.
 *             The error information is cleared internally upon return
 *
 * @param[in]  h              esp-tls error handle.
 * @param[out] esp_tls_code   last error code returned from mbedtls api (set to zero if none)
 *                            This pointer could be NULL if caller does not care about esp_tls_code
 * @param[out] esp_tls_flags  last certification verification flags (set to zero if none)
 *                            This pointer could be NULL if caller does not care about esp_tls_code
 *
 * @return
 *            - ESP_ERR_INVALID_STATE if invalid parameters
 *            - ESP_OK (0) if no error occurred
 *            - specific error code (based on ESP_ERR_ESP_TLS_BASE) otherwise
 */
esp_err_t esp_tls_get_and_clear_last_error(esp_tls_error_handle_t h, int *esp_tls_code, int *esp_tls_flags);

#if CONFIG_ESP_TLS_USING_MBEDTLS
/**
 * @brief      Get the pointer to the global CA store currently being used.
 *
 * The application must first call esp_tls_set_global_ca_store(). Then the same
 * CA store could be used by the application for APIs other than esp_tls.
 *
 * @note       Modifying the pointer might cause a failure in verifying the certificates.
 *
 * @return
 *             - Pointer to the global CA store currently being used    if successful.
 *             - NULL                                                   if there is no global CA store set.
 */
mbedtls_x509_crt *esp_tls_get_global_ca_store(void);

#ifdef CONFIG_ESP_TLS_SERVER
/**
 * @brief      Create TLS/SSL server session
 *
 * This function creates a TLS/SSL server context for already accepted client connection
 * and performs TLS/SSL handshake with the client
 *
 * @param[in]  cfg      Pointer to esp_tls_cfg_server_t
 * @param[in]  sockfd   FD of accepted connection
 * @param[out] tls      Pointer to allocated esp_tls_t
 *
 * @return
 *          - 0  if successful
 *          - <0 in case of error
 *
 */
int esp_tls_server_session_create(esp_tls_cfg_server_t *cfg, int sockfd, esp_tls_t *tls);

/**
 * @brief      Close the server side TLS/SSL connection and free any allocated resources.
 *
 * This function should be called to close each tls connection opened with esp_tls_server_session_create()
 *
 * @param[in]  tls  pointer to esp_tls_t
 */
void esp_tls_server_session_delete(esp_tls_t *tls);
#endif /* ! CONFIG_ESP_TLS_SERVER */
#endif /* CONFIG_ESP_TLS_USING_MBEDTLS */

#ifdef __cplusplus
}
#endif

#endif /* ! _ESP_TLS_H_ */
