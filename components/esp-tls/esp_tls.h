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

#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/esp_debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      ESP-TLS configuration parameters 
 */ 
typedef struct esp_tls_cfg {
    const char **alpn_protos;               /*!< Application protocols required for HTTP2.
                                                 If HTTP2/ALPN support is required, a list
                                                 of protocols that should be negotiated. 
                                                 The format is length followed by protocol
                                                 name. 
                                                 For the most common cases the following is ok:
                                                 "\x02h2"
                                                 - where the first '2' is the length of the protocol and
                                                 - the subsequent 'h2' is the protocol name */
 
    const unsigned char *cacert_pem_buf;    /*!< Certificate Authority's certificate in a buffer */
 
    unsigned int cacert_pem_bytes;          /*!< Size of Certificate Authority certificate
                                                 pointed to by cacert_pem_buf */
 
    bool non_block;                         /*!< Configure non-blocking mode. If set to true the 
                                                 underneath socket will be configured in non 
                                                 blocking mode after tls session is established */

    int timeout_ms;                         /*!< Network timeout in milliseconds */
} esp_tls_cfg_t;

/**
 * @brief      ESP-TLS Connection Handle 
 */
typedef struct esp_tls {
    mbedtls_ssl_context ssl;                                                    /*!< TLS/SSL context */
 
    mbedtls_entropy_context entropy;                                            /*!< mbedTLS entropy context structure */
 
    mbedtls_ctr_drbg_context ctr_drbg;                                          /*!< mbedTLS ctr drbg context structure.
                                                                                     CTR_DRBG is deterministic random 
                                                                                     bit generation based on AES-256 */
 
    mbedtls_ssl_config conf;                                                    /*!< TLS/SSL configuration to be shared 
                                                                                     between mbedtls_ssl_context 
                                                                                     structures */
 
    mbedtls_net_context server_fd;                                              /*!< mbedTLS wrapper type for sockets */
 
    mbedtls_x509_crt cacert;                                                    /*!< Container for an X.509 certificate */
 
    int sockfd;                                                                 /*!< Underlying socket file descriptor. */
 
    ssize_t (*read)(struct esp_tls  *tls, char *data, size_t datalen);          /*!< Callback function for reading data from TLS/SSL
                                                                                     connection. */
 
    ssize_t (*write)(struct esp_tls *tls, const char *data, size_t datalen);    /*!< Callback function for writing data to TLS/SSL
                                                                                     connection. */
} esp_tls_t;

/**
 * @brief      Create a new TLS/SSL connection
 *
 * This function establishes a TLS/SSL connection with the specified host.
 * 
 * @param[in]  hostname  Hostname of the host.
 * @param[in]  hostlen   Length of hostname.
 * @param[in]  port      Port number of the host.
 * @param[in]  cfg       TLS configuration as esp_tls_cfg_t. If you wish to open 
 *                       non-TLS connection, keep this NULL. For TLS connection,
 *                       a pass pointer to esp_tls_cfg_t. At a minimum, this
 *                       structure should be zero-initialized.
 * @return pointer to esp_tls_t, or NULL if connection couldn't be opened.
 */
esp_tls_t *esp_tls_conn_new(const char *hostname, int hostlen, int port, const esp_tls_cfg_t *cfg);

/**
 * @brief      Create a new TLS/SSL connection with a given "HTTP" url    
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
size_t esp_tls_get_bytes_avail(esp_tls_t *tls);

#ifdef __cplusplus
}
#endif

#endif /* ! _ESP_TLS_H_ */
