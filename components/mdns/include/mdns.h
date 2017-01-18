// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
#ifndef ESP_MDNS_H_
#define ESP_MDNS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <tcpip_adapter.h>

struct mdns_server_s;
typedef struct mdns_server_s mdns_server_t;

/**
 * @brief mDNS query result structure
 *
 */
typedef struct mdns_result_s {
    const char * host;                  /*!< hostname */
    const char * instance;              /*!< instance */
    const char * txt;                   /*!< txt data */
    uint16_t priority;                  /*!< service priority */
    uint16_t weight;                    /*!< service weight */
    uint16_t port;                      /*!< service port */
    struct ip4_addr addr;               /*!< ip4 address */
    struct ip6_addr addrv6;             /*!< ip6 address */
    const struct mdns_result_s * next;  /*!< next result, or NULL for the last result in the list */
} mdns_result_t;

/**
 * @brief  Initialize mDNS on given interface
 *
 * @param  tcpip_if     Interface that the server will listen on
 * @param  server       Server pointer to populate on success
 *
 * @return
 *     - ESP_OK on success
 *     - ESP_ERR_INVALID_ARG when bad tcpip_if is given
 *     - ESP_ERR_INVALID_STATE when the network returned error
 *     - ESP_ERR_NO_MEM on memory error
 *     - ESP_ERR_WIFI_NOT_INIT when WiFi is not initialized by eps_wifi_init
 */
esp_err_t mdns_init(tcpip_adapter_if_t tcpip_if, mdns_server_t ** server);

/**
 * @brief  Stop and free mDNS server
 *
 * @param  server       mDNS Server to free
 *
 */
void mdns_free(mdns_server_t * server);

/**
 * @brief  Set the hostname for mDNS server
 *
 * @param  server       mDNS Server
 * @param  hostname     Hostname to set
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NO_MEM memory error
 */
esp_err_t mdns_set_hostname(mdns_server_t * server, const char * hostname);

/**
 * @brief  Set the default instance name for mDNS server
 *
 * @param  server       mDNS Server
 * @param  instance     Instance name to set
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NO_MEM memory error
 */
esp_err_t mdns_set_instance(mdns_server_t * server, const char * instance);

/**
 * @brief  Add service to mDNS server
 *
 * @param  server       mDNS Server
 * @param  service      service type (_http, _ftp, etc)
 * @param  proto        service protocol (_tcp, _udp)
 * @param  port         service port
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NO_MEM memory error
 */
esp_err_t mdns_service_add(mdns_server_t * server, const char * service, const char * proto, uint16_t port);

/**
 * @brief  Remove service from mDNS server
 *
 * @param  server       mDNS Server
 * @param  service      service type (_http, _ftp, etc)
 * @param  proto        service protocol (_tcp, _udp)
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NOT_FOUND Service not found
 *     - ESP_FAIL unknown error
 */
esp_err_t mdns_service_remove(mdns_server_t * server, const char * service, const char * proto);

/**
 * @brief  Set instance name for service
 *
 * @param  server       mDNS Server
 * @param  service      service type (_http, _ftp, etc)
 * @param  proto        service protocol (_tcp, _udp)
 * @param  instance     instance name to set
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NOT_FOUND Service not found
 *     - ESP_ERR_NO_MEM memory error
 */
esp_err_t mdns_service_instance_set(mdns_server_t * server, const char * service, const char * proto, const char * instance);

/**
 * @brief  Set TXT data for service
 *
 * @param  server       mDNS Server
 * @param  service      service type (_http, _ftp, etc)
 * @param  proto        service protocol (_tcp, _udp)
 * @param  num_items    number of items in TXT data
 * @param  txt          string array of TXT data (eg. {"var=val","other=2"})
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NOT_FOUND Service not found
 *     - ESP_ERR_NO_MEM memory error
 */
esp_err_t mdns_service_txt_set(mdns_server_t * server, const char * service, const char * proto, uint8_t num_items, const char ** txt);

/**
 * @brief  Set service port
 *
 * @param  server       mDNS Server
 * @param  service      service type (_http, _ftp, etc)
 * @param  proto        service protocol (_tcp, _udp)
 * @param  port         service port
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NOT_FOUND Service not found
 */
esp_err_t mdns_service_port_set(mdns_server_t * server, const char * service, const char * proto, uint16_t port);

/**
 * @brief  Remove and free all services from mDNS server
 *
 * @param  server       mDNS Server
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mdns_service_remove_all(mdns_server_t * server);

/**
 * @brief  Query mDNS for host or service
 *
 * @param  server       mDNS Server
 * @param  service      service type or host name
 * @param  proto        service protocol or NULL if searching for host
 * @param  timeout      time to wait for answers. If 0, mdns_query_end MUST be called to end the search
 *
 * @return the number of results found
 */
size_t mdns_query(mdns_server_t * server, const char * service, const char * proto, uint32_t timeout);

/**
 * @brief  Stop mDNS Query started with timeout = 0
 *
 * @param  server       mDNS Server
 *
 * @return the number of results found
 */
size_t mdns_query_end(mdns_server_t * server);

/**
 * @brief  get the number of results currently in memoty
 *
 * @param  server       mDNS Server
 *
 * @return the number of results
 */
size_t mdns_result_get_count(mdns_server_t * server);

/**
 * @brief  Get mDNS Search result with given index
 *
 * @param  server       mDNS Server
 * @param  num          the index of the result
 *
 * @return the result or NULL if error
 */
const mdns_result_t * mdns_result_get(mdns_server_t * server, size_t num);

/**
 * @brief  Remove and free all search results from mDNS server
 *
 * @param  server       mDNS Server
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mdns_result_free(mdns_server_t * server);

#ifdef __cplusplus
}
#endif

#endif /* ESP_MDNS_H_ */
