/*
 * SPDX-FileCopyrightText: 2017-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _ESP_TLS_H_
#define _ESP_TLS_H_

#include <stdbool.h>
#include "esp_err.h"
#include "esp_tls_errors.h"
#include "sdkconfig.h"
#ifdef CONFIG_ESP_TLS_USING_MBEDTLS
#include "mbedtls/ssl.h"
#include "mbedtls/x509_crt.h"
#ifdef CONFIG_ESP_TLS_SERVER_SESSION_TICKETS
#include "mbedtls/ssl_ticket.h"
#endif
#endif


#ifdef __cplusplus
extern "C" {
#endif

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
    size_t   key_size;                      /*!< length of the key */
    const char* hint;                       /*!< hint in PSK authentication mode in string format */
} psk_hint_key_t;

/**
 * @brief esp-tls client session ticket ctx
 */
#ifdef CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS
typedef struct esp_tls_client_session {
    mbedtls_ssl_session saved_session;
} esp_tls_client_session_t;
#endif /* CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS */

/**
*  @brief Keep alive parameters structure
*/
typedef struct tls_keep_alive_cfg {
    bool keep_alive_enable;               /*!< Enable keep-alive timeout */
    int keep_alive_idle;                  /*!< Keep-alive idle time (second) */
    int keep_alive_interval;              /*!< Keep-alive interval time (second) */
    int keep_alive_count;                 /*!< Keep-alive packet retry send count */
} tls_keep_alive_cfg_t;

/*
* @brief ESP-TLS Address families
*/
typedef enum esp_tls_addr_family {
    ESP_TLS_AF_UNSPEC = 0,                /**< Unspecified address family. */
    ESP_TLS_AF_INET,                      /**< IPv4 address family. */
    ESP_TLS_AF_INET6,                     /**< IPv6 address family. */
} esp_tls_addr_family_t;

/*
* @brief ESP-TLS TLS Protocol version
*/
typedef enum {
   ESP_TLS_VER_ANY = 0,         /* No preference */
   ESP_TLS_VER_TLS_1_2 = 0x1,   /* (D)TLS 1.2 */
   ESP_TLS_VER_TLS_1_3 = 0x2,   /* (D)TLS 1.3 */
   ESP_TLS_VER_TLS_MAX,         /* to indicate max */
} esp_tls_proto_ver_t;

typedef enum {
    ESP_TLS_DYN_BUF_RX_STATIC = 1,    /*!< Strategy to disable dynamic RX buffer allocations and convert to static allocation post-handshake, reducing memory fragmentation */
    ESP_TLS_DYN_BUF_STRATEGY_MAX,     /*!< to indicate max */
} esp_tls_dyn_buf_strategy_t;

/**
 * @brief ECDSA curve options for TLS connections
 */
typedef enum {
    ESP_TLS_ECDSA_CURVE_SECP256R1 = 0,   /*!< Use SECP256R1 curve */
#if SOC_ECDSA_SUPPORT_CURVE_P384
    ESP_TLS_ECDSA_CURVE_SECP384R1,       /*!< Use SECP384R1 curve */
#endif
    ESP_TLS_ECDSA_CURVE_MAX,            /*!< to indicate max */
} esp_tls_ecdsa_curve_t;

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

    bool use_ecdsa_peripheral;              /*!< Use the ECDSA peripheral for the private key operations */

    uint8_t ecdsa_key_efuse_blk;            /*!< The efuse block where ECDSA key is stored. For SECP384R1 curve, if two blocks are used, set this to the low block and use ecdsa_key_efuse_blk_high for the high block. */

    uint8_t ecdsa_key_efuse_blk_high;       /*!< The high efuse block for ECDSA key (used only for SECP384R1 curve). If not set (0), only ecdsa_key_efuse_blk is used. */

    esp_tls_ecdsa_curve_t ecdsa_curve;      /*!< ECDSA curve to use (SECP256R1 or SECP384R1) */

    bool non_block;                         /*!< Configure non-blocking mode. If set to true the
                                                 underneath socket will be configured in non
                                                 blocking mode after tls session is established */

    bool use_secure_element;                /*!< Enable this option to use secure element or
                                                 atecc608a chip */

    int timeout_ms;                         /*!< Network timeout in milliseconds.
                                                 Note: If this value is not set, by default the timeout is
                                                 set to 10 seconds. If you wish that the session should wait
                                                 indefinitely then please use a larger value e.g., INT32_MAX */

    bool use_global_ca_store;               /*!< Use a global ca_store for all the connections in which
                                                 this bool is set. */

    const char *common_name;                /*!< If non-NULL, server certificate CN must match this name.
                                                 If NULL, server certificate CN must match hostname. */

    bool skip_common_name;                  /*!< Skip any validation of server certificate CN field.
                                                 This field should be set to false for SNI to function correctly. */

    tls_keep_alive_cfg_t *keep_alive_cfg;   /*!< Enable TCP keep-alive timeout for SSL connection */

#if defined(CONFIG_ESP_TLS_PSK_VERIFICATION)
    const psk_hint_key_t* psk_hint_key;     /*!< Pointer to PSK hint and key. if not NULL (and certificates are NULL)
                                                 then PSK authentication is enabled with configured setup.
                                                 Important note: the pointer must be valid for connection */
#endif /* CONFIG_ESP_TLS_PSK_VERIFICATION */

    esp_err_t (*crt_bundle_attach)(void *conf);
                                            /*!< Function pointer to esp_crt_bundle_attach. Enables the use of certification
                                                 bundle for server verification, must be enabled in menuconfig */

    void *ds_data;                          /*!< Pointer for digital signature peripheral context */
    bool is_plain_tcp;                      /*!< Use non-TLS connection: When set to true, the esp-tls uses
                                                 plain TCP transport rather then TLS/SSL connection.
                                                 Note, that it is possible to connect using a plain tcp transport
                                                 directly with esp_tls_plain_tcp_connect() API */

    struct ifreq *if_name;                  /*!< The name of interface for data to go through. Use the default interface without setting */

#ifdef CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS
    esp_tls_client_session_t *client_session; /*! Pointer for the client session ticket context. */
#endif /* CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS */

    esp_tls_addr_family_t addr_family;      /*!< The address family to use when connecting to a host. */
    const int *ciphersuites_list;           /*!< Pointer to a zero-terminated array of IANA identifiers of TLS ciphersuites.
                                                Please check the list validity by esp_tls_get_ciphersuites_list() API */
    esp_tls_proto_ver_t tls_version;        /*!< TLS protocol version of the connection, e.g., TLS 1.2, TLS 1.3 (default - no preference) */

#if CONFIG_MBEDTLS_DYNAMIC_BUFFER
    esp_tls_dyn_buf_strategy_t esp_tls_dyn_buf_strategy; /*!< ESP-TLS dynamic buffer strategy */
#endif

} esp_tls_cfg_t;

#if defined(CONFIG_ESP_TLS_SERVER_SESSION_TICKETS)
/**
 * @brief Data structures necessary to support TLS session tickets according to RFC5077
 */
typedef struct esp_tls_server_session_ticket_ctx {
    mbedtls_ssl_ticket_context ticket_ctx;                                     /*!< Session ticket generation context */
} esp_tls_server_session_ticket_ctx_t;
#endif

#if defined(CONFIG_ESP_TLS_SERVER_CERT_SELECT_HOOK)
/**
 * @brief tls handshake callback
 * Can be used to configure per-handshake attributes for the TLS connection.
 * E.g. Client certificate / Key, Authmode, Client CA verification, etc.
 *
 * @param ssl mbedtls_ssl_context that can be used for changing settings
 * @return The reutn value of the callback must be 0 if successful,
 *         or a specific MBEDTLS_ERR_XXX code, which will cause the handhsake to abort
 */
typedef mbedtls_ssl_hs_cb_t esp_tls_handshake_callback;
#else
// When CONFIG_ESP_TLS_SERVER_CERT_SELECT_HOOK is not defined,
// the following typedef is only kept for compatibility reasons, not to be used.
typedef void* esp_tls_handshake_callback;
#endif

/**
 * @brief ESP-TLS Server configuration parameters
 */
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

#ifdef CONFIG_ESP_TLS_SERVER_MIN_AUTH_MODE_OPTIONAL
    bool client_cert_authmode_optional;              /*!< Set client certificate authentication mode to optional.
                                                     By default, client certificate authentication mode is set to required */
#endif // CONFIG_ESP_TLS_SERVER_MIN_AUTH_MODE_OPTIONAL
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

    bool use_ecdsa_peripheral;                  /*!< Use ECDSA peripheral to use private key */

    uint8_t ecdsa_key_efuse_blk;                /*!< The efuse block where ECDSA key is stored. For SECP384R1 curve, if two blocks are used, set this to the low block and use ecdsa_key_efuse_blk_high for the high block. */

    uint8_t ecdsa_key_efuse_blk_high;           /*!< The high efuse block for ECDSA key (used only for SECP384R1 curve). If not set (0), only ecdsa_key_efuse_blk is used. */

    esp_tls_ecdsa_curve_t ecdsa_curve;          /*!< ECDSA curve to use (SECP256R1 or SECP384R1) */

    bool use_secure_element;                    /*!< Enable this option to use secure element or
                                                 atecc608a chip */

    uint32_t tls_handshake_timeout_ms;                   /*!< TLS handshake timeout in milliseconds.
                                                    Note: If this value is not set, by default the timeout is
                                                    set to 10 seconds. If you wish that the session should wait
                                                    indefinitely then please use a larger value e.g., INT32_MAX */

#if defined(CONFIG_ESP_TLS_SERVER_SESSION_TICKETS)
    esp_tls_server_session_ticket_ctx_t * ticket_ctx; /*!< Session ticket generation context.
                                                    You have to call esp_tls_cfg_server_session_tickets_init
                                                    to use it.
                                                    Call esp_tls_cfg_server_session_tickets_free
                                                    to free the data associated with this context. */
#endif

    void *userdata;                             /*!< User data to be added to the ssl context.
                                                  Can be retrieved by callbacks */
#if defined(CONFIG_ESP_TLS_SERVER_CERT_SELECT_HOOK)
    esp_tls_handshake_callback cert_select_cb;  /*!< Certificate selection callback that gets called after ClientHello is processed.
                                                     Can be used as an SNI callback, but also has access to other
                                                     TLS extensions, such as ALPN and server_certificate_type . */
#endif

#if defined(CONFIG_ESP_TLS_PSK_VERIFICATION)
    const psk_hint_key_t* psk_hint_key;         /*!< Pointer to PSK hint and key. if not NULL (and the certificate/key is NULL)
                                                  then PSK authentication is enabled with configured setup.
                                                  Important note: the pointer must be valid for connection */
#endif

    esp_tls_proto_ver_t tls_version;            /*!< TLS protocol version for this server, e.g., TLS 1.2, TLS 1.3
                                                     (default - no preference). Enables TLS version control per server instance. */

    const int *ciphersuites_list;               /*!< Pointer to a zero-terminated array of IANA identifiers of TLS ciphersuites.
                                                     Please check the list validity by esp_tls_get_ciphersuites_list() API.
                                                     This allows per-server cipher suite configuration. */
} esp_tls_cfg_server_t;

/**
 * @brief Initialize the server side TLS session ticket context
 *
 * This function initializes the server side tls session ticket context
 * which holds all necessary data structures to enable tls session tickets
 * according to RFC5077.
 * Use esp_tls_cfg_server_session_tickets_free to free the data.
 *
 * @param[in]  cfg server configuration as esp_tls_cfg_server_t
 * @return
 *             ESP_OK if setup succeeded
 *             ESP_ERR_INVALID_ARG if context is already initialized
 *             ESP_ERR_NO_MEM if memory allocation failed
 *             ESP_ERR_NOT_SUPPORTED if session tickets are not available due to build configuration
 *             ESP_FAIL if setup failed
 */
esp_err_t esp_tls_cfg_server_session_tickets_init(esp_tls_cfg_server_t *cfg);

/**
 * @brief Free the server side TLS session ticket context
 *
 * @param cfg server configuration as esp_tls_cfg_server_t
 */
void esp_tls_cfg_server_session_tickets_free(esp_tls_cfg_server_t *cfg);

typedef struct esp_tls esp_tls_t;

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
 * @param[in]  hostname  Hostname of the host.
 * @param[in]  hostlen   Length of hostname.
 * @param[in]  port      Port number of the host.
 * @param[in]  cfg       TLS configuration as esp_tls_cfg_t. For a TLS
 *                       connection, pass a pointer to a esp_tls_cfg_t. For a
 *                       plain TCP connection, pass a pointer to a
 *                       esp_tls_cfg_t with is_plain_tcp set to true. At a
 *                       minimum, this pointer should be not NULL and the
 *                       structure should be zero-initialized
 * @param[in]  tls       Pointer to esp-tls as esp-tls handle.
 *
 * @return
 *             - -1      If connection establishment fails.
 *             -  1      If connection establishment is successful.
 *             -  0      If connection state is in progress.
 */
int esp_tls_conn_new_sync(const char *hostname, int hostlen, int port, const esp_tls_cfg_t *cfg, esp_tls_t *tls);

/**
 * @brief      Create a new blocking TLS/SSL connection with a given "HTTP" url
 *
 * The behaviour is same as esp_tls_conn_new_sync() API. However this API accepts host's url.
 *
 * @param[in]  url       url of host.
 * @param[in]  cfg       TLS configuration as esp_tls_cfg_t. If you wish to open
 *                       non-TLS connection, keep this NULL. For TLS connection,
 *                       a pass pointer to 'esp_tls_cfg_t'. At a minimum, this
 *                       structure should be zero-initialized.
 * @param[in]  tls       Pointer to esp-tls as esp-tls handle.
 *
 * @return
 *             - -1      If connection establishment fails.
 *             -  1      If connection establishment is successful.
 *             -  0      If connection state is in progress.
 */
int esp_tls_conn_http_new_sync(const char *url, const esp_tls_cfg_t *cfg, esp_tls_t *tls);

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
 * The behaviour is same as esp_tls_conn_new_async() API. However this API accepts host's url.
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
 *             - >=0  if write operation was successful, the return value is the number
 *                   of bytes actually written to the TLS/SSL connection.
 *             - <0  if write operation was not successful, because either an
 *                   error occurred or an action must be taken by the calling process.
 *             - ESP_TLS_ERR_SSL_WANT_READ/
 *               ESP_TLS_ERR_SSL_WANT_WRITE.
 *                  if the handshake is incomplete and waiting for data to be available for reading.
 *                  In this case this functions needs to be called again when the underlying transport is ready for operation.
 */
ssize_t esp_tls_conn_write(esp_tls_t *tls, const void *data, size_t datalen);

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
 *                   error occurred or an action must be taken by the calling process.
 */
ssize_t esp_tls_conn_read(esp_tls_t *tls, void  *data, size_t datalen);

/**
 * @brief      Close the TLS/SSL connection and free any allocated resources.
 *
 * This function should be called to close each tls connection opened with
 * esp_tls_conn_new_sync() (or esp_tls_conn_http_new_sync()) and
 * esp_tls_conn_new_async() (or esp_tls_conn_http_new_async()) APIs.
 *
 * @param[in]  tls  pointer to esp-tls as esp-tls handle.
 *
 * @return    - 0 on success
 *            - -1 if socket error or an invalid argument
 */
int esp_tls_conn_destroy(esp_tls_t *tls);

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
 * @brief       Returns the connection socket file descriptor from esp_tls session
 *
 * @param[in]   tls          handle to esp_tls context
 *
 * @param[out]  sockfd       int pointer to sockfd value.
 *
 * @return     - ESP_OK on success and value of sockfd will be updated with socket file descriptor for connection
 *             - ESP_ERR_INVALID_ARG if (tls == NULL || sockfd == NULL)
 */
esp_err_t esp_tls_get_conn_sockfd(esp_tls_t *tls, int *sockfd);

/**
 * @brief       Sets the connection socket file descriptor for the esp_tls session
 *
 * @param[in]   tls          handle to esp_tls context
 *
 * @param[in]   sockfd       sockfd value to set.
 *
 * @return     - ESP_OK on success and value of sockfd for the tls connection shall updated with the provided value
 *             - ESP_ERR_INVALID_ARG if (tls == NULL || sockfd < 0)
 */
esp_err_t esp_tls_set_conn_sockfd(esp_tls_t *tls, int sockfd);

/**
 * @brief       Gets the connection state for the esp_tls session
 *
 * @param[in]   tls          handle to esp_tls context
 *
 * @param[out]   conn_state   pointer to the connection state value.
 *
 * @return     - ESP_OK on success and value of sockfd for the tls connection shall updated with the provided value
 *             - ESP_ERR_INVALID_ARG (Invalid arguments)
 */
esp_err_t esp_tls_get_conn_state(esp_tls_t *tls, esp_tls_conn_state_t *conn_state);

/**
 * @brief       Sets the connection state for the esp_tls session
 *
 * @param[in]   tls          handle to esp_tls context
 *
 * @param[in]   conn_state   connection state value to set.
 *
 * @return     - ESP_OK on success and value of sockfd for the tls connection shall updated with the provided value
 *             - ESP_ERR_INVALID_ARG (Invalid arguments)
 */
esp_err_t esp_tls_set_conn_state(esp_tls_t *tls, esp_tls_conn_state_t conn_state);

/**
 * @brief       Returns the ssl context
 *
 * @param[in]   tls          handle to esp_tls context
 *
 *
 * @return     - ssl_ctx pointer to ssl context of underlying TLS layer on success
 *             - NULL  in case of error
 */
void *esp_tls_get_ssl_context(esp_tls_t *tls);

/**
 * @brief      Create a global CA store, initially empty.
 *
 * This function should be called if the application wants to use the same CA store for multiple connections.
 * This function initialises the global CA store which can be then set by calling esp_tls_set_global_ca_store().
 * To be effective, this function must be called before any call to esp_tls_set_global_ca_store().
 *
 * @return
 *             - ESP_OK             if creating global CA store was successful.
 *             - ESP_ERR_NO_MEM     if an error occurred when allocating the mbedTLS resources.
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
 *             - Other   if an error occurred or an action must be taken by the calling process.
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

/**
 * @brief      Returns the last error captured in esp_tls of a specific type
 *             The error information is cleared internally upon return
 *
 * @param[in]  h              esp-tls error handle.
 * @param[in]  err_type       specific error type
 * @param[out] error_code     last error code returned from mbedtls api (set to zero if none)
 *                            This pointer could be NULL if caller does not care about esp_tls_code
 * @return
 *            - ESP_ERR_INVALID_STATE if invalid parameters
 *            - ESP_OK if a valid error returned and was cleared
 */
esp_err_t esp_tls_get_and_clear_error_type(esp_tls_error_handle_t h, esp_tls_error_type_t err_type, int *error_code);

/**
 * @brief       Returns the ESP-TLS error_handle
 *
 * @param[in]   tls             handle to esp_tls context
 *
 * @param[out]  error_handle    pointer to the error handle.
 *
 * @return
 *             - ESP_OK             on success and error_handle will be updated with the ESP-TLS error handle.
 *
 *             - ESP_ERR_INVALID_ARG if (tls == NULL || error_handle == NULL)
 */
esp_err_t esp_tls_get_error_handle(esp_tls_t *tls, esp_tls_error_handle_t *error_handle);

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

/**
 * @brief Get supported TLS ciphersuites list.
 *
 * See https://www.iana.org/assignments/tls-parameters/tls-parameters.xhtml#tls-parameters-4 for the list of ciphersuites
 *
 * @return  Pointer to a zero-terminated array of IANA identifiers of TLS ciphersuites.
 *
 */
const int *esp_tls_get_ciphersuites_list(void);

/**
 * @brief      Initialize server side TLS/SSL connection
 *
 * This function should be used to initialize the server side TLS/SSL connection when the
 * application wants to handle the TLS/SSL connection asynchronously with the help of
 * esp_tls_server_session_continue_async() function.
 *
 * @param[in]  cfg      Pointer to esp_tls_cfg_server_t
 * @param[in]  sockfd   FD of accepted connection
 * @param[out] tls      Pointer to allocated esp_tls_t
 *
 * @return
 *          - ESP_OK if successful
 *          - ESP_ERR_INVALID_ARG if invalid arguments
 *          - ESP_FAIL if server session setup failed
 */
esp_err_t esp_tls_server_session_init(esp_tls_cfg_server_t *cfg, int sockfd, esp_tls_t *tls);

/**
 * @brief      Asynchronous continue of esp_tls_server_session_init
 *
 * This function should be called in a loop by the user until it returns 0. If this functions returns
 * something other than 0, ESP_TLS_ERR_SSL_WANT_READ or ESP_TLS_ERR_SSL_WANT_WRITE,
 * the esp-tls context must not be used and should be freed using esp_tls_conn_destroy();
 *
 * @param[in]  tls  pointer to esp_tls_t
 *
 * @return
 *          - 0  if successful
 *          - <0 in case of error
 *          - ESP_TLS_ERR_SSL_WANT_READ/ESP_TLS_ERR_SSL_WANT_WRITE
 *            if the handshake is incomplete and waiting for data to be available for reading.
 */
int esp_tls_server_session_continue_async(esp_tls_t *tls);

#endif /* CONFIG_ESP_TLS_USING_MBEDTLS */
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

/**
 * @brief Creates a plain TCP connection, returning a valid socket fd on success or an error handle
 *
 * @param[in]  host      Hostname of the host.
 * @param[in]  hostlen   Length of hostname.
 * @param[in]  port      Port number of the host.
 * @param[in]  cfg       ESP-TLS configuration as esp_tls_cfg_t.
 * @param[out] error_handle ESP-TLS error handle holding potential errors occurred during connection
 * @param[out] sockfd    Socket descriptor if successfully connected on TCP layer
 * @return     ESP_OK   on success
 *             ESP_ERR_INVALID_ARG if invalid output parameters
 *             ESP-TLS based error codes on failure
 */
esp_err_t esp_tls_plain_tcp_connect(const char *host, int hostlen, int port, const esp_tls_cfg_t *cfg, esp_tls_error_handle_t error_handle, int *sockfd);

#ifdef CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS
/**
 * @brief Obtain the client session ticket
 *
 * This function should be called when the TLS connection is already established.
 * This can be passed again in the esp_tls_cfg_t structure, to appropriate tls session create (e.g. esp_tls_conn_http_new_sync) API for session resumption.
 *
 * @param[in]  esp_tls context as esp_tls_t
 * @return
 *             Pointer to the saved client session.
 *             NULL     on Failure
 */
esp_tls_client_session_t *esp_tls_get_client_session(esp_tls_t *tls);

/**
 * @brief Free the client session
 *
 * This function should be called after esp_tls_get_client_session().
 *
 * @param[in]  client_session context as esp_tls_client_session_t
 *
 */
void esp_tls_free_client_session(esp_tls_client_session_t *client_session);
#endif /* CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS */
#ifdef __cplusplus
}
#endif

#endif /* ! _ESP_TLS_H_ */
