/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_TRANSPORT_SSL_H_
#define _ESP_TRANSPORT_SSL_H_

#include "esp_transport.h"
#include "esp_tls.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief       Create new SSL transport, the transport handle must be release esp_transport_destroy callback
 *
 * @return      the allocated esp_transport_handle_t, or NULL if the handle can not be allocated
 */
esp_transport_handle_t esp_transport_ssl_init(void);

/**
 * @brief      Set SSL certificate data (as PEM format).
 *             Note that, this function stores the pointer to data, rather than making a copy.
 *             So this data must remain valid until after the connection is cleaned up
 *
 * @param      t     ssl transport
 * @param[in]  data  The pem data
 * @param[in]  len   The length
 */
void esp_transport_ssl_set_cert_data(esp_transport_handle_t t, const char *data, int len);

/**
 * @brief      Set SSL certificate data (as DER format).
 *             Note that, this function stores the pointer to data, rather than making a copy.
 *             So this data must remain valid until after the connection is cleaned up
 *
 * @param      t     ssl transport
 * @param[in]  data  The der data
 * @param[in]  len   The length
 */
void esp_transport_ssl_set_cert_data_der(esp_transport_handle_t t, const char *data, int len);

/**
 * @brief      Enable the use of certification bundle for server verification for
 *             an SSL connection.
 *             It must be first enabled in menuconfig.
 *
 * @param      t    ssl transport
 * @param[in]  crt_bundle_attach    Function pointer to esp_crt_bundle_attach
 */
void esp_transport_ssl_crt_bundle_attach(esp_transport_handle_t t, esp_err_t ((*crt_bundle_attach)(void *conf)));

/**
 * @brief      Enable global CA store for SSL connection
 *
 * @param      t    ssl transport
 */
void esp_transport_ssl_enable_global_ca_store(esp_transport_handle_t t);

/**
 * @brief      Set TLS protocol version for ESP-TLS connection
 *
 * @param      t    ssl transport
 * @param[in]  tls_version      TLS version
 */
void esp_transport_ssl_set_tls_version(esp_transport_handle_t t, esp_tls_proto_ver_t tls_version);

/**
 * @brief      Set SSL client certificate data for mutual authentication (as PEM format).
 *             Note that, this function stores the pointer to data, rather than making a copy.
 *             So this data must remain valid until after the connection is cleaned up
 *
 * @param      t     ssl transport
 * @param[in]  data  The pem data
 * @param[in]  len   The length
 */
void esp_transport_ssl_set_client_cert_data(esp_transport_handle_t t, const char *data, int len);

#ifdef CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN
/**
 * @brief      Set SSL client key data for mutual authentication when using ECDSA peripheral.
 *
 * @param      t            ssl transport
 * @param[in]  efuse_blk    Efuse block where ECDSA private key is stored
 */
void esp_transport_ssl_set_client_key_ecdsa_peripheral(esp_transport_handle_t t, uint8_t ecdsa_efuse_blk);
#endif

/**
 * @brief      Set SSL client certificate data for mutual authentication (as DER format).
 *             Note that, this function stores the pointer to data, rather than making a copy.
 *             So this data must remain valid until after the connection is cleaned up
 *
 * @param      t     ssl transport
 * @param[in]  data  The der data
 * @param[in]  len   The length
 */
void esp_transport_ssl_set_client_cert_data_der(esp_transport_handle_t t, const char *data, int len);

/**
 * @brief      Set SSL client key data for mutual authentication (as PEM format).
 *             Note that, this function stores the pointer to data, rather than making a copy.
 *             So this data must remain valid until after the connection is cleaned up
 *
 * @param      t     ssl transport
 * @param[in]  data  The pem data
 * @param[in]  len   The length
 */
void esp_transport_ssl_set_client_key_data(esp_transport_handle_t t, const char *data, int len);

/**
 * @brief      Set SSL client key password if the key is password protected. The configured
 *             password is passed to the underlying TLS stack to decrypt the client key
 *
 * @param      t     ssl transport
 * @param[in]  password  Pointer to the password
 * @param[in]  password_len   Password length
 */
void esp_transport_ssl_set_client_key_password(esp_transport_handle_t t, const char *password, int password_len);

/**
 * @brief      Set SSL client key data for mutual authentication (as DER format).
 *             Note that, this function stores the pointer to data, rather than making a copy.
 *             So this data must remain valid until after the connection is cleaned up
 *
 * @param      t     ssl transport
 * @param[in]  data  The der data
 * @param[in]  len   The length
 */
void esp_transport_ssl_set_client_key_data_der(esp_transport_handle_t t, const char *data, int len);

/**
 * @brief      Set the list of supported application protocols to be used with ALPN.
 *             Note that, this function stores the pointer to data, rather than making a copy.
 *             So this data must remain valid until after the connection is cleaned up
 *
 * @param      t            ssl transport
 * @param[in]  alpn_porot   The list of ALPN protocols, the last entry must be NULL
 */
void esp_transport_ssl_set_alpn_protocol(esp_transport_handle_t t, const char **alpn_protos);

/**
 * @brief      Skip validation of certificate's common name field
 *
 * @note       Skipping CN validation is not recommended
 *
 * @param      t     ssl transport
 */
void esp_transport_ssl_skip_common_name_check(esp_transport_handle_t t);

/**
 * @brief      Set the server certificate's common name field
 *
 * @note
 *             If non-NULL, server certificate CN must match this name,
 *             If NULL, server certificate CN must match hostname.
 * @param      t             ssl transport
 *             common_name   A string containing the common name to be set
 */
void esp_transport_ssl_set_common_name(esp_transport_handle_t t, const char *common_name);

/**
 * @brief      Set the ssl context to use secure element (atecc608a) for client(device) private key and certificate
 *
 * @note       Recommended to be used with ESP32 interfaced to ATECC608A based secure element
 *
 * @param      t     ssl transport
 */
void esp_transport_ssl_use_secure_element(esp_transport_handle_t t);

/**
 * @brief      Set the ds_data handle in ssl context.(used for the digital signature operation)
 *
 * @param      t        ssl transport
 *             ds_data  the handle for ds data params
 */
void esp_transport_ssl_set_ds_data(esp_transport_handle_t t, void *ds_data);

/**
 * @brief      Set PSK key and hint for PSK server/client verification in esp-tls component.
 *             Important notes:
 *             - This function stores the pointer to data, rather than making a copy.
 *             So this data must remain valid until after the connection is cleaned up
 *             - ESP_TLS_PSK_VERIFICATION config option must be enabled in menuconfig
 *             - certificate verification takes priority so it must not be configured
 *             to enable PSK method.
 *
 * @param      t             ssl transport
 * @param[in]  psk_hint_key  psk key and hint structure defined in esp_tls.h
 */
void esp_transport_ssl_set_psk_key_hint(esp_transport_handle_t t, const psk_hint_key_t* psk_hint_key);

/**
 * @brief      Set keep-alive status in current ssl context
 *
 * @param[in]  t               ssl transport
 * @param[in]  keep_alive_cfg  The handle for keep-alive configuration
 */
void esp_transport_ssl_set_keep_alive(esp_transport_handle_t t, esp_transport_keep_alive_t *keep_alive_cfg);

/**
 * @brief      Set name of interface that socket can be binded on
 *             So the data can transport on this interface
 *
 * @param[in]  t        The transport handle
 * @param[in]  if_name  The interface name
 */
void esp_transport_ssl_set_interface_name(esp_transport_handle_t t, struct ifreq *if_name);

/**
 * @brief      Set addr family of transport
 *
 * @param[in]  t            The transport handle
 * @param[in]  addr_family  The addr family
 */
void esp_transport_ssl_set_addr_family(esp_transport_handle_t t, esp_tls_addr_family_t addr_family);

#ifdef CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS
/**
 * @brief   Session ticket operation
 */
typedef enum {
    ESP_TRANSPORT_SESSION_TICKET_INIT,      /*!< Allocate and initialize a TLS session */
    ESP_TRANSPORT_SESSION_TICKET_SAVE,      /*!< Save TLS session so it can be restored for the next handshake */
    ESP_TRANSPORT_SESSION_TICKET_USE,       /*!< Use already saved session to reconnect faster */
    ESP_TRANSPORT_SESSION_TICKET_FREE       /*!< Deallocate and deinit the TLS session */
} esp_transport_session_ticket_operation_t;

/**
 * @brief      Perform desired session ticket operation (init, save, use)
 *
 * @param[in]  t            The transport handle
 * @param[in]  operation    Operation to perform with TLS session
 *
 * @note This operation is only available if CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS=y
 */
esp_err_t esp_transport_ssl_session_ticket_operation(esp_transport_handle_t t, esp_transport_session_ticket_operation_t operation);
#endif // CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS

#ifdef __cplusplus
}
#endif
#endif /* _ESP_TRANSPORT_SSL_H_ */
