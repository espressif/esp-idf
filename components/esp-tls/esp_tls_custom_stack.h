/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "esp_assert.h"
#include "esp_err.h"
#include "esp_tls.h"
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Version of the TLS stack operations structure
 *
 * This version number must be set in the `version` field of esp_tls_stack_ops_t
 * when registering a custom TLS stack. It allows ESP-TLS to detect incompatible
 * interface changes between ESP-IDF versions and custom stack implementations.
 */
#define ESP_TLS_STACK_OPS_VERSION 1

/**
 * @brief TLS stack operations vtable
 *
 * This structure defines the interface that external TLS stack implementations
 * must provide. All function pointers must be non-NULL.
 *
 * @note To register a custom TLS stack:
 *       1. Enable CONFIG_ESP_TLS_CUSTOM_STACK in menuconfig
 *       2. Implement all required functions in this structure
 *       3. Call esp_tls_register_stack() with your implementation before creating any TLS connections
 *
 * @note The TLS context (esp_tls_t) is managed by ESP-TLS. Your implementation should store
 *       stack-specific data in the appropriate fields of esp_tls_t (e.g., priv_ctx, priv_ssl)
 *       or allocate additional memory and store a pointer to it.
 *
 * @note For client connections: hostname and cfg (esp_tls_cfg_t) are provided
 * @note For server connections: hostname is NULL, cfg is esp_tls_cfg_server_t, server_params is provided
 */
typedef struct esp_tls_stack_ops {
    /**
     * @brief Structure version for compatibility checking
     *
     * Must be set to ESP_TLS_STACK_OPS_VERSION. This field allows ESP-TLS to
     * detect version mismatches between the ESP-IDF version and the custom
     * TLS stack implementation.
     */
    uint32_t version;

    /**
     * @brief Create SSL handle for a TLS connection
     *
     * This function initializes the TLS stack for a new connection. It should:
     * - Initialize the TLS/SSL context for the given connection
     * - Configure certificates, keys, and other TLS parameters from cfg
     * - Set up the underlying socket (tls->sockfd is already set)
     * - For client: configure SNI using hostname
     * - For server: use server_params if provided
     * - Store stack-specific context in tls->priv_ctx or tls->priv_ssl
     *
     * @param user_ctx User context pointer passed to esp_tls_register_stack()
     * @param hostname Hostname for the connection (client only, NULL for server)
     * @param hostlen Length of hostname (0 for server)
     * @param cfg TLS configuration:
     *            - For client: pointer to esp_tls_cfg_t
     *            - For server: pointer to esp_tls_cfg_server_t
     * @param tls TLS context (already allocated, sockfd is set)
     * @param server_params Server-specific parameters (NULL for client, non-NULL for server)
     *                      Contains server configuration callbacks
     * @return
     *         - ESP_OK: SSL handle created successfully
     *         - ESP_ERR_NO_MEM: Memory allocation failed
     *         - ESP_ERR_INVALID_ARG: Invalid configuration
     *         - Other error codes: Stack-specific errors
     */
    esp_err_t (*create_ssl_handle)(void *user_ctx, const char *hostname, size_t hostlen, const void *cfg, esp_tls_t *tls, void *server_params);

    /**
     * @brief Perform TLS handshake
     *
     * This function performs the TLS handshake. It should:
     * - Execute the TLS handshake protocol
     * - Handle handshake state (may need multiple calls)
     * - Verify certificates if configured
     * - Update tls->conn_state appropriately
     *
     * @param user_ctx User context pointer passed to esp_tls_register_stack()
     * @param tls TLS context (created by create_ssl_handle)
     * @param cfg TLS configuration (esp_tls_cfg_t for client, can be used for timeout checks)
     *
     * @return
     *         - 1: Handshake completed successfully
     *         - 0: Handshake in progress, call again later
     *         - ESP_TLS_ERR_SSL_WANT_READ: Need to read more data from socket
     *         - ESP_TLS_ERR_SSL_WANT_WRITE: Need to write more data to socket
     *         - Negative value: Handshake failed (error code)
     *
     * @note This function may be called multiple times until it returns 1 or a negative error.
     *       The ESP-TLS layer handles retries based on the return value.
     */
    int (*handshake)(void *user_ctx, esp_tls_t *tls, const esp_tls_cfg_t *cfg);

    /**
     * @brief Read data from TLS connection
     *
     * This function reads decrypted application data from the TLS connection.
     * It should:
     * - Read encrypted data from tls->sockfd
     * - Decrypt using the TLS stack
     * - Return decrypted application data
     * - Handle partial reads and blocking/non-blocking behavior
     *
     * @param user_ctx User context pointer passed to esp_tls_register_stack()
     * @param tls TLS context (handshake must be completed)
     * @param data Buffer to store decrypted data
     * @param datalen Maximum number of bytes to read (size of data buffer)
     *
     * @return
     *         - Positive value: Number of bytes read (0 < return <= datalen)
     *         - 0: Connection closed by peer
     *         - ESP_TLS_ERR_SSL_WANT_READ: Need to read more encrypted data from socket
     *         - ESP_TLS_ERR_SSL_WANT_WRITE: Need to write data (renegotiation)
     *         - Negative value: Error occurred
     *
     * @note This function is called by tls->read() callback, which is set during handshake.
     * @note The function should handle TLS record boundaries and may need multiple socket reads
     *       to decrypt a single application data record.
     */
    ssize_t (*read)(void *user_ctx, esp_tls_t *tls, char *data, size_t datalen);

    /**
     * @brief Write data to TLS connection
     *
     * This function encrypts and writes application data to the TLS connection.
     * It should:
     * - Encrypt application data using the TLS stack
     * - Write encrypted data to tls->sockfd
     * - Handle partial writes and blocking/non-blocking behavior
     *
     * @param user_ctx User context pointer passed to esp_tls_register_stack()
     * @param tls TLS context (handshake must be completed)
     * @param data Application data to encrypt and send
     * @param datalen Number of bytes to write
     *
     * @return
     *         - Positive value: Number of bytes written (should equal datalen if successful)
     *         - ESP_TLS_ERR_SSL_WANT_READ: Need to read data (renegotiation)
     *         - ESP_TLS_ERR_SSL_WANT_WRITE: Socket buffer full, try again later
     *         - Negative value: Error occurred
     *
     * @note This function is called by tls->write() callback, which is set during handshake.
     * @note The function may write less than datalen if the socket buffer is full (non-blocking mode).
     *       The caller should retry with remaining data.
     */
    ssize_t (*write)(void *user_ctx, esp_tls_t *tls, const char *data, size_t datalen);

    /**
     * @brief Delete TLS connection and free resources
     *
     * This function cleans up the TLS connection and frees all resources.
     * It should:
     * - Close the TLS/SSL session
     * - Free any stack-specific context stored in tls->priv_ctx or tls->priv_ssl
     * - Free any allocated memory
     * - Note: tls->sockfd is closed separately by ESP-TLS, don't close it here
     *
     * @param user_ctx User context pointer passed to esp_tls_register_stack()
     * @param tls TLS context to clean up
     *
     * @note This function should be idempotent (safe to call multiple times).
     * @note After this call, the tls context may be freed, so don't access it afterwards.
     */
    void (*conn_delete)(void *user_ctx, esp_tls_t *tls);

    /**
     * @brief Initialize network context
     *
     * This function initializes any network-related structures needed by the TLS stack.
     * It should:
     * - Initialize socket wrappers or network context structures
     * - Set up any network-related state
     * - This is typically called once per TLS context before create_ssl_handle
     *
     * @param user_ctx User context pointer passed to esp_tls_register_stack()
     * @param tls TLS context (sockfd is already set)
     *
     * @note For some stacks, this may be a no-op if network initialization is handled elsewhere.
     * @note This is called early in the connection setup, before create_ssl_handle.
     */
    void (*net_init)(void *user_ctx, esp_tls_t *tls);

    /**
     * @brief Get SSL context (stack-specific)
     *
     * This function returns the underlying SSL/TLS context object from the stack.
     * This allows users to access stack-specific APIs directly if needed.
     *
     * @param user_ctx User context pointer passed to esp_tls_register_stack()
     * @param tls TLS context
     *
     * @return Pointer to stack-specific SSL context (e.g., mbedtls_ssl_context*, SSL_CTX*, etc.)
     *         or NULL if not available
     *
     * @note The returned pointer is stack-specific and should be cast to the appropriate type
     *       by users who know which stack is registered.
     * @note This is optional but recommended for advanced users who need stack-specific features.
     */
    void *(*get_ssl_context)(void *user_ctx, esp_tls_t *tls);

    /**
     * @brief Get bytes available for reading
     *
     * This function returns the number of decrypted application data bytes
     * available to read without blocking.
     *
     * @param user_ctx User context pointer passed to esp_tls_register_stack()
     * @param tls TLS context
     *
     * @return
     *         - Positive value: Number of bytes available to read
     *         - 0: No data available
     *         - Negative value: Error occurred
     *
     * @note This checks the TLS stack's internal buffer, not the socket buffer.
     * @note Useful for non-blocking I/O to check if data is ready before calling read().
     */
    ssize_t (*get_bytes_avail)(void *user_ctx, esp_tls_t *tls);

    /**
     * @brief Initialize global CA store
     *
     * This function initializes a global certificate authority (CA) store that can be
     * used by all TLS connections. This is useful for applications that want to use
     * the same CA certificates for multiple connections.
     *
     * @param user_ctx User context pointer passed to esp_tls_register_stack()
     *
     * @return
     *         - ESP_OK: CA store initialized successfully
     *         - ESP_ERR_NO_MEM: Memory allocation failed
     *         - Other error codes: Stack-specific errors
     *
     * @note This is called once at application startup if esp_tls_init_global_ca_store() is used.
     * @note The global CA store is optional - individual connections can use their own CA certificates.
     */
    esp_err_t (*init_global_ca_store)(void *user_ctx);

    /**
     * @brief Set global CA store
     *
     * This function loads CA certificates into the global CA store.
     * The certificates are used for server certificate verification in client connections.
     *
     * @param user_ctx User context pointer passed to esp_tls_register_stack()
     * @param cacert_pem_buf CA certificate buffer in PEM format (NULL-terminated)
     * @param cacert_pem_bytes Size of CA certificate buffer (including NULL terminator for PEM)
     *
     * @return
     *         - ESP_OK: CA certificates loaded successfully
     *         - ESP_ERR_INVALID_ARG: Invalid certificate format
     *         - ESP_ERR_NO_MEM: Memory allocation failed
     *         - Other error codes: Stack-specific errors
     *
     * @note The buffer should contain one or more PEM-formatted CA certificates.
     * @note For PEM format, the buffer must be NULL-terminated.
     * @note This can be called multiple times to add more certificates.
     */
    esp_err_t (*set_global_ca_store)(void *user_ctx, const unsigned char *cacert_pem_buf, const unsigned int cacert_pem_bytes);

    /**
     * @brief Get global CA store (stack-specific)
     *
     * This function returns the underlying CA store object from the stack.
     * This allows users to access stack-specific CA store APIs directly if needed.
     *
     * @param user_ctx User context pointer passed to esp_tls_register_stack()
     *
     * @return Pointer to stack-specific CA store (e.g., mbedtls_x509_crt*, X509_STORE*, etc.)
     *         or NULL if not initialized
     *
     * @note The returned pointer is stack-specific and should be cast to the appropriate type.
     * @note This is optional but useful for advanced users who need stack-specific features.
     */
    void *(*get_global_ca_store)(void *user_ctx);

    /**
     * @brief Free global CA store
     *
     * This function frees all resources associated with the global CA store.
     *
     * @param user_ctx User context pointer passed to esp_tls_register_stack()
     *
     * @note This should free all certificates and memory allocated for the CA store.
     * @note After this call, the global CA store is no longer usable until reinitialized.
     */
    void (*free_global_ca_store)(void *user_ctx);

    /**
     * @brief Get list of supported ciphersuites
     *
     * This function returns a list of IANA ciphersuite identifiers supported by the stack.
     * This is used by esp_tls_get_ciphersuites_list() API.
     *
     * @param user_ctx User context pointer passed to esp_tls_register_stack()
     *
     * @return Pointer to zero-terminated array of IANA ciphersuite identifiers (integers)
     *         The array must be terminated with 0.
     *
     * @note The returned pointer should point to static/const data (not dynamically allocated).
     * @note Ciphersuite identifiers are IANA standard values (e.g., 0x0035 for TLS_RSA_WITH_AES_256_CBC_SHA).
     * @note This list is used for validation when users specify custom ciphersuite lists.
     */
    const int *(*get_ciphersuites_list)(void *user_ctx);

    /**
     * @brief Get client session (optional, for session ticket support)
     *
     * This function retrieves the client session ticket/session data that can be reused
     * for subsequent connections to the same server (RFC 5077).
     *
     * @param user_ctx User context pointer passed to esp_tls_register_stack()
     * @param tls TLS context (handshake must be completed)
     *
     * @return Pointer to client session data (stack-specific type), or NULL if:
     *         - Session tickets are not supported
     *         - No session ticket was received
     *         - Error occurred
     *
     * @note The returned session can be stored and reused in future connections to the same server.
     * @note This is only used if CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS is enabled.
     * @note The session data is stack-specific and opaque to ESP-TLS.
     * @note Can be NULL if session tickets are not supported by the stack.
     */
    void *(*get_client_session)(void *user_ctx, esp_tls_t *tls);

    /**
     * @brief Free client session (optional, for session ticket support)
     *
     * This function frees resources associated with a client session retrieved by get_client_session().
     *
     * @param user_ctx User context pointer passed to esp_tls_register_stack()
     * @param client_session Client session to free (returned by get_client_session())
     *
     * @note This should free all memory associated with the session.
     * @note This is only used if CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS is enabled.
     * @note Can be NULL if session tickets are not supported by the stack.
     */
    void (*free_client_session)(void *user_ctx, void *client_session);

    /**
     * @brief Initialize server session ticket context (optional, for server session ticket support)
     *
     * This function initializes the server-side session ticket context for issuing
     * and validating session tickets (RFC 5077).
     *
     * @param user_ctx User context pointer passed to esp_tls_register_stack()
     * @param cfg Pointer to esp_tls_server_session_ticket_ctx_t structure
     *            Contains ticket encryption keys and configuration
     *
     * @return
     *         - ESP_OK: Session ticket context initialized successfully
     *         - ESP_ERR_INVALID_ARG: Invalid configuration
     *         - ESP_ERR_NO_MEM: Memory allocation failed
     *         - Other error codes: Stack-specific errors
     *
     * @note This is called when setting up server session tickets.
     * @note The context should store encryption keys and other ticket-related state.
     * @note This is only used if CONFIG_ESP_TLS_SERVER_SESSION_TICKETS is enabled.
     * @note Can be NULL if server session tickets are not supported by the stack.
     */
    esp_err_t (*server_session_ticket_ctx_init)(void *user_ctx, void *cfg);

    /**
     * @brief Free server session ticket context (optional, for server session ticket support)
     *
     * This function frees resources associated with the server session ticket context.
     *
     * @param user_ctx User context pointer passed to esp_tls_register_stack()
     * @param cfg Pointer to esp_tls_server_session_ticket_ctx_t structure to free
     *
     * @note This should free all memory and clear sensitive data (encryption keys).
     * @note This is only used if CONFIG_ESP_TLS_SERVER_SESSION_TICKETS is enabled.
     * @note Can be NULL if server session tickets are not supported by the stack.
     */
    void (*server_session_ticket_ctx_free)(void *user_ctx, void *cfg);

    /**
     * @brief Create server session (optional, server-side only)
     *
     * This function creates a complete TLS server session, performing initialization
     * and handshake in a blocking manner. This is a convenience function that combines
     * server_session_init() and server_session_continue_async().
     *
     * @param user_ctx User context pointer passed to esp_tls_register_stack()
     * @param cfg Server configuration (esp_tls_cfg_server_t)
     * @param sockfd Socket file descriptor (already connected)
     * @param tls TLS context (already allocated)
     *
     * @return
     *         - 1: Server session created successfully, handshake completed
     *         - 0: Should not happen (this is a blocking function)
     *         - ESP_TLS_ERR_SSL_WANT_READ: Need to read more data (should retry)
     *         - ESP_TLS_ERR_SSL_WANT_WRITE: Need to write more data (should retry)
     *         - Negative value: Error occurred
     *
     * @note This function should handle the complete handshake internally.
     * @note For non-blocking operation, use server_session_init() + server_session_continue_async() instead.
     * @note This is optional - if NULL, ESP-TLS will use server_session_init() + server_session_continue_async().
     */
    int (*server_session_create)(void *user_ctx, esp_tls_cfg_server_t *cfg, int sockfd, esp_tls_t *tls);

    /**
     * @brief Initialize server session (optional, server-side only)
     *
     * This function initializes a TLS server session. It should:
     * - Set up the TLS context for server mode
     * - Configure server certificates and keys from cfg
     * - Prepare for handshake (but don't perform it yet)
     * - Set tls->read and tls->write callbacks
     *
     * @param user_ctx User context pointer passed to esp_tls_register_stack()
     * @param cfg Server configuration (esp_tls_cfg_server_t)
     * @param sockfd Socket file descriptor (already connected)
     * @param tls TLS context (already allocated)
     *
     * @return
     *         - ESP_OK: Server session initialized successfully
     *         - ESP_ERR_INVALID_ARG: Invalid configuration
     *         - ESP_ERR_NO_MEM: Memory allocation failed
     *         - Other error codes: Stack-specific errors
     *
     * @note After this, call server_session_continue_async() to perform the handshake.
     * @note This is optional - if NULL, server_session_create() must be implemented.
     */
    esp_err_t (*server_session_init)(void *user_ctx, esp_tls_cfg_server_t *cfg, int sockfd, esp_tls_t *tls);

    /**
     * @brief Continue async server session handshake (optional, server-side only)
     *
     * This function continues the TLS handshake for a server session initialized
     * with server_session_init(). It should be called repeatedly until it returns 0.
     *
     * @param user_ctx User context pointer passed to esp_tls_register_stack()
     * @param tls TLS context (initialized by server_session_init)
     *
     * @return
     *         - 0: Handshake completed successfully
     *         - ESP_TLS_ERR_SSL_WANT_READ: Need to read more data from client, call again later
     *         - ESP_TLS_ERR_SSL_WANT_WRITE: Need to write data to client, call again later
     *         - Negative value: Handshake failed (error code)
     *
     * @note This should be called in a loop until it returns 0 or a negative error.
     * @note The ESP-TLS layer handles the looping and timeout management.
     * @note This is optional - if NULL, server_session_create() must be implemented.
     */
    int (*server_session_continue_async)(void *user_ctx, esp_tls_t *tls);

    /**
     * @brief Delete server session (optional, server-side only)
     *
     * This function cleans up a server TLS session. It should:
     * - Close the TLS session
     * - Free any server-specific resources
     * - Note: tls->sockfd is closed separately, don't close it here
     *
     * @param user_ctx User context pointer passed to esp_tls_register_stack()
     * @param tls TLS context to clean up
     *
     * @note This is similar to conn_delete() but may have server-specific cleanup.
     * @note If NULL, conn_delete() will be used instead.
     */
    void (*server_session_delete)(void *user_ctx, esp_tls_t *tls);

    /**
     * @brief Calculate SHA1 hash (optional, for esp_crypto_sha1 API)
     *
     * This function calculates the SHA1 hash of the input data. It is used by
     * esp_crypto_sha1() API which is needed by components like esp_http_server
     * for WebSocket key calculation.
     *
     * @param user_ctx User context pointer passed to esp_tls_register_stack()
     * @param input Input data buffer
     * @param ilen Length of input data
     * @param output Output buffer for SHA1 hash (must be at least 20 bytes)
     *
     * @return
     *         - 0: Success
     *         - Negative value: Error occurred
     *
     * @note If NULL, esp_crypto_sha1() API calls will fail. This callback must be implemented
     *       if your application or any dependent component (e.g., esp_http_server for WebSocket)
     *       uses the esp_crypto_sha1() API.
     */
    int (*crypto_sha1)(void *user_ctx, const unsigned char *input, size_t ilen, unsigned char output[20]);

    /**
     * @brief Base64 encode data (optional, for esp_crypto_base64_encode API)
     *
     * This function performs Base64 encoding of the source data. It is used by
     * esp_crypto_base64_encode() API which is needed by components like esp_http_server
     * for WebSocket key encoding.
     *
     * @param user_ctx User context pointer passed to esp_tls_register_stack()
     * @param dst Destination buffer for encoded data
     * @param dlen Size of destination buffer
     * @param olen Pointer to store the number of bytes written
     * @param src Source data buffer
     * @param slen Length of source data
     *
     * @return
     *         - 0: Success
     *         - -0x002A: Buffer too small
     *         - Other negative value: Error occurred
     *
     * @note If NULL, esp_crypto_base64_encode() API calls will fail. This callback must be
     *       implemented if your application or any dependent component (e.g., esp_http_server
     *       for WebSocket) uses the esp_crypto_base64_encode() API.
     */
    int (*crypto_base64_encode)(void *user_ctx, unsigned char *dst, size_t dlen, size_t *olen,
                                const unsigned char *src, size_t slen);
} esp_tls_stack_ops_t;

/**
 * Compile-time check to detect structure layout changes.
 * If this assertion fails, you MUST increment ESP_TLS_STACK_OPS_VERSION.
 *
 * Field count: 1 (version) + 23 (function pointers) = 24
 */
ESP_STATIC_ASSERT(sizeof(esp_tls_stack_ops_t) == 24 * sizeof(void *), "esp_tls_stack_ops_t layout changed - update ESP_TLS_STACK_OPS_VERSION!");

/**
 * @brief Register a custom TLS stack implementation
 *
 * This function allows an external component to register its TLS stack implementation.
 * Once registered, all TLS connections created after this call will use the registered stack.
 *
 * @note This function must be called before creating any TLS connections (esp_tls_conn_new(), etc.).
 * @note This function can only be called once. Subsequent calls will return ESP_ERR_INVALID_STATE.
 * @note If CONFIG_ESP_TLS_CUSTOM_STACK is not enabled, this function will return ESP_ERR_NOT_SUPPORTED.
 * @note All required function pointers in ops must be non-NULL. Optional functions can be NULL.
 * @note The ops structure pointer is stored by reference, not copied. The structure must remain
 *       valid in memory for the entire duration of TLS operations (typically the lifetime of the
 *       application). Use a static or global variable, not a stack-allocated or dynamically
 *       allocated structure that may be freed. Using a `const` static structure allows the
 *       compiler to place it in flash (.rodata), saving ~80 bytes of RAM.
 *
 * @param ops Pointer to TLS stack operations vtable containing your implementation.
 *            Must point to a static/global structure (not on stack) as it's stored by reference.
 *            Required fields:
 *            - version (must be set to ESP_TLS_STACK_OPS_VERSION)
 *            - create_ssl_handle
 *            - handshake
 *            - read
 *            - write
 *            - conn_delete
 *            - net_init
 *            - get_ssl_context
 *            - get_bytes_avail
 *            - init_global_ca_store
 *            - set_global_ca_store
 *            - get_global_ca_store
 *            - free_global_ca_store
 *            - get_ciphersuites_list
 *            Optional functions (can be NULL if not supported):
 *            - get_client_session (for CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS)
 *            - free_client_session (for CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS)
 *            - server_session_ticket_ctx_init (for CONFIG_ESP_TLS_SERVER_SESSION_TICKETS)
 *            - server_session_ticket_ctx_free (for CONFIG_ESP_TLS_SERVER_SESSION_TICKETS)
 *            - server_session_create (server-side, can be NULL if server_session_init is provided)
 *            - server_session_init (server-side, can be NULL if server_session_create is provided)
 *            - server_session_continue_async (server-side, can be NULL if server_session_create is provided)
 *            - server_session_delete (server-side, can be NULL, conn_delete will be used)
 * @param user_ctx User context pointer that will be passed to all callbacks as the first parameter.
 *                 This allows C++ implementations to avoid singletons by passing instance pointers.
 *                 Can be NULL if not needed.
 *
 * @return
 *         - ESP_OK: Stack registered successfully
 *         - ESP_ERR_INVALID_ARG: ops is NULL or any required function pointer is NULL
 *         - ESP_ERR_INVALID_VERSION: ops->version does not match ESP_TLS_STACK_OPS_VERSION
 *         - ESP_ERR_INVALID_STATE: A stack is already registered
 *         - ESP_ERR_NOT_SUPPORTED: CONFIG_ESP_TLS_CUSTOM_STACK is not enabled
 *
 * @code{.c}
 * // Example usage:
 * static const esp_tls_stack_ops_t my_tls_ops = {
 *     .version = ESP_TLS_STACK_OPS_VERSION,
 *     .create_ssl_handle = my_create_ssl_handle,
 *     .handshake = my_handshake,
 *     .read = my_read,
 *     .write = my_write,
 *     .conn_delete = my_conn_delete,
 *     .net_init = my_net_init,
 *     .get_ssl_context = my_get_ssl_context,
 *     .get_bytes_avail = my_get_bytes_avail,
 *     .init_global_ca_store = my_init_global_ca_store,
 *     .set_global_ca_store = my_set_global_ca_store,
 *     .get_global_ca_store = my_get_global_ca_store,
 *     .free_global_ca_store = my_free_global_ca_store,
 *     .get_ciphersuites_list = my_get_ciphersuites_list,
 *     // Optional functions...
 * };
 *
 * void app_main(void) {
 *     esp_err_t ret = esp_tls_register_stack(&my_tls_ops, NULL);
 *     if (ret != ESP_OK) {
 *         ESP_LOGE("APP", "Failed to register TLS stack: %s", esp_err_to_name(ret));
 *         return;
 *     }
 *     // ... create TLS connections as usual ...
 * }
 * @endcode
 */
esp_err_t esp_tls_register_stack(const esp_tls_stack_ops_t *ops, void *user_ctx);

/**
 * @brief Unregister the custom TLS stack implementation
 *
 * This function removes the registered TLS stack, allowing a new stack to be registered.
 * This is primarily useful for testing scenarios where you need to switch between
 * different TLS stack implementations.
 *
 * @note This function should NOT be called while any TLS connections are active.
 *       Ensure all TLS connections are closed before calling this function.
 * @note If CONFIG_ESP_TLS_CUSTOM_STACK is not enabled, this function will return ESP_ERR_NOT_SUPPORTED.
 *
 * @return
 *         - ESP_OK: Stack unregistered successfully
 *         - ESP_ERR_INVALID_STATE: No stack was registered
 *         - ESP_ERR_NOT_SUPPORTED: CONFIG_ESP_TLS_CUSTOM_STACK is not enabled
 */
esp_err_t esp_tls_unregister_stack(void);

/**
 * @brief Get the registered TLS stack operations
 *
 * @return Pointer to registered TLS stack operations, or NULL if no stack is registered
 *         or CONFIG_ESP_TLS_CUSTOM_STACK is not enabled
 */
const esp_tls_stack_ops_t *esp_tls_get_registered_stack(void);

/**
 * @brief Internal wrapper functions that call through the registered stack's vtable
 *
 * These functions provide a uniform interface to the registered TLS stack implementation.
 * They forward calls to the appropriate function pointer in the registered esp_tls_stack_ops_t structure.
 *
 * @note These are internal functions used by ESP-TLS and should not be called directly by applications.
 * @{
 */

/** @brief Create SSL handle for a TLS connection */
esp_err_t esp_tls_custom_stack_create_ssl_handle(const char *hostname, size_t hostlen, const void *cfg, esp_tls_t *tls, void *server_params);

/** @brief Perform TLS handshake */
int esp_tls_custom_stack_handshake(esp_tls_t *tls, const esp_tls_cfg_t *cfg);

/** @brief Read data from TLS connection */
ssize_t esp_tls_custom_stack_read(esp_tls_t *tls, char *data, size_t datalen);

/** @brief Write data to TLS connection */
ssize_t esp_tls_custom_stack_write(esp_tls_t *tls, const char *data, size_t datalen);

/** @brief Delete TLS connection and free resources */
void esp_tls_custom_stack_conn_delete(esp_tls_t *tls);

/** @brief Initialize network context for TLS connection */
void esp_tls_custom_stack_net_init(esp_tls_t *tls);

/** @brief Get SSL context (stack-specific) */
void *esp_tls_custom_stack_get_ssl_context(esp_tls_t *tls);

/** @brief Get number of bytes available for reading */
ssize_t esp_tls_custom_stack_get_bytes_avail(esp_tls_t *tls);

/** @brief Initialize global CA certificate store */
esp_err_t esp_tls_custom_stack_init_global_ca_store(void);

/** @brief Set global CA certificate store */
esp_err_t esp_tls_custom_stack_set_global_ca_store(const unsigned char *cacert_pem_buf, const unsigned int cacert_pem_bytes);

/** @brief Get global CA certificate store (stack-specific) */
void *esp_tls_custom_stack_get_global_ca_store(void);

/** @brief Free global CA certificate store */
void esp_tls_custom_stack_free_global_ca_store(void);

/** @brief Get list of supported ciphersuites */
const int *esp_tls_custom_stack_get_ciphersuites_list(void);

/** @brief Get client session ticket for reuse */
void *esp_tls_custom_stack_get_client_session(esp_tls_t *tls);

/** @brief Free client session ticket */
void esp_tls_custom_stack_free_client_session(void *client_session);

/** @brief Initialize server session ticket context */
esp_err_t esp_tls_custom_stack_server_session_ticket_ctx_init(void *cfg);

/** @brief Free server session ticket context */
void esp_tls_custom_stack_server_session_ticket_ctx_free(void *cfg);

/** @brief Create server session (blocking) */
int esp_tls_custom_stack_server_session_create(esp_tls_cfg_server_t *cfg, int sockfd, esp_tls_t *tls);

/** @brief Initialize server session (non-blocking) */
esp_err_t esp_tls_custom_stack_server_session_init(esp_tls_cfg_server_t *cfg, int sockfd, esp_tls_t *tls);

/** @brief Continue async server session handshake */
int esp_tls_custom_stack_server_session_continue_async(esp_tls_t *tls);

/** @brief Delete server session */
void esp_tls_custom_stack_server_session_delete(esp_tls_t *tls);

/** @brief Calculate SHA1 hash using registered stack's implementation */
int esp_tls_custom_stack_crypto_sha1(const unsigned char *input, size_t ilen, unsigned char output[20]);

/** @brief Base64 encode using registered stack's implementation */
int esp_tls_custom_stack_crypto_base64_encode(unsigned char *dst, size_t dlen, size_t *olen,
                                              const unsigned char *src, size_t slen);

/** @} */

#ifdef __cplusplus
}
#endif
