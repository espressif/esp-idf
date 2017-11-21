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

#ifndef MDNS_TEST_MODE
#include <tcpip_adapter.h>
#include "esp_event.h"
#else
#include "esp32_compat.h"
#endif

/**
 * @brief   mDNS basic text item structure
 *          Used in mdns_service_add()
 */
typedef struct {
    const char * key;                       /*!< item key name */
    const char * value;                     /*!< item value string */
} mdns_txt_item_t;

/**
 * @brief   mDNS linked list text item structure
 *
 */
typedef struct mdns_txt_linked_item_s {
    const char * key;                       /*!< item key name */
    const char * value;                     /*!< item value string */
    struct mdns_txt_linked_item_s * next;   /*!< next result, or NULL for the last result in the list */
} mdns_txt_linked_item_t;

/**
 * @brief   mDNS query result structure
 *
 */
typedef struct mdns_result_s {
    const char * host;                      /*!< hostname */
    const char * instance;                  /*!< instance */
    mdns_txt_linked_item_t * txt;           /*!< txt data */
    uint16_t priority;                      /*!< service priority */
    uint16_t weight;                        /*!< service weight */
    uint16_t port;                          /*!< service port */
    struct ip4_addr addr;                   /*!< ip4 address */
    struct ip6_addr addrv6;                 /*!< ip6 address */
    const struct mdns_result_s * next;      /*!< next result, or NULL for the last result in the list */
} mdns_result_t;

/**
 * @brief  Initialize mDNS on given interface
 *
 * @return
 *     - ESP_OK on success
 *     - ESP_ERR_INVALID_ARG when bad tcpip_if is given
 *     - ESP_ERR_INVALID_STATE when the network returned error
 *     - ESP_ERR_NO_MEM on memory error
 *     - ESP_ERR_WIFI_NOT_INIT when WiFi is not initialized by eps_wifi_init
 */
esp_err_t mdns_init();

/**
 * @brief  Stop and free mDNS server
 *
 */
void mdns_free();

/**
 * @brief  Set the hostname for mDNS server
 *
 * @param  hostname     Hostname to set
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NO_MEM memory error
 */
esp_err_t mdns_set_hostname(const char * hostname);

/**
 * @brief  Set the default instance name for mDNS server
 *
 * @param  instance     Instance name to set
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NO_MEM memory error
 */
esp_err_t mdns_set_instance(const char * instance);

/**
 * @brief  Add service to mDNS server
 *
 * @param  service      service type (_http, _ftp, etc)
 * @param  proto        service protocol (_tcp, _udp)
 * @param  port         service port
 * @param  instance     instance name to set
 * @param  num_items    number of items in TXT data
 * @param  txt          string array of TXT data (eg. {"var=val","other=2"})
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NO_MEM memory error
 */
esp_err_t mdns_service_add(const char * service, const char * proto, uint16_t port, const char * instance, uint8_t num_items, mdns_txt_item_t txt[]);

/**
 * @brief  Remove service from mDNS server
 *
 * @param  service      service type (_http, _ftp, etc)
 * @param  proto        service protocol (_tcp, _udp)
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NOT_FOUND Service not found
 *     - ESP_FAIL unknown error
 */
esp_err_t mdns_service_remove(const char * service, const char * proto);

/**
 * @brief  Set instance name for service
 *
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
esp_err_t mdns_service_instance_set(const char * service, const char * proto, const char * instance);

/**
 * @brief  Set TXT data for service
 *
 * @param  service      service type (_http, _ftp, etc)
 * @param  proto        service protocol (_tcp, _udp)
 * @param  key          the key that you want to add/update
 * @param  value        the new value of the key
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NOT_FOUND Service not found
 *     - ESP_ERR_NO_MEM memory error
 */
esp_err_t mdns_service_txt_set(const char * service, const char * proto, const char * key, const char * value);

/**
 * @brief  Rem0ve TXT data for service
 *
 * @param  service      service type (_http, _ftp, etc)
 * @param  proto        service protocol (_tcp, _udp)
 * @param  key          the key that you want to remove
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NOT_FOUND Service not found
 *     - ESP_ERR_NO_MEM memory error
 */
esp_err_t mdns_service_txt_remove(const char * service, const char * proto, const char * key);

/**
 * @brief  Set service port
 *
 * @param  service      service type (_http, _ftp, etc)
 * @param  proto        service protocol (_tcp, _udp)
 * @param  port         service port
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NOT_FOUND Service not found
 */
esp_err_t mdns_service_port_set(const char * service, const char * proto, uint16_t port);

/**
 * @brief  Remove and free all services from mDNS server
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mdns_service_remove_all();

/**
 * @brief  Query mDNS for host or service
 *
 * @param  service      service type or host name
 * @param  proto        service protocol or NULL if searching for host
 * @param  timeout      time to wait for answers. If 0, mdns_query_end MUST be called to end the search
 *
 * @return the number of results found
 */
size_t mdns_query(const char * service, const char * proto, uint32_t timeout);

/**
 * @brief  Stop mDNS Query started with timeout = 0
 *
 * @return the number of results found
 */
size_t mdns_query_end();

/**
 * @brief  get the number of results currently in memory
 *
 * @return the number of results
 */
size_t mdns_result_get_count();

/**
 * @brief  Get mDNS Search result with given index
 *
 * @param  num          the index of the result
 *
 * @return the result or NULL if error
 */
const mdns_result_t * mdns_result_get(size_t num);

/**
 * @brief  Remove and free all search results from mDNS server
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mdns_result_free();

/**
 * @brief   Pass system event from the event handler initialized by esp_event_loop_init
 *          This method controls the service state on all active interfaces and is required
 *          for normal operation of the mdns service.
 *
 * @param  event        The system event from the handler
 */
void mdns_handle_system_event(system_event_id_t event);

#define MDNS_ANSWER_PTR_TTL         4500
#define MDNS_ANSWER_TXT_TTL         4500
#define MDNS_ANSWER_SRV_TTL         120
#define MDNS_ANSWER_A_TTL           120
#define MDNS_ANSWER_AAAA_TTL        120

void mdns_ptr_ttl_set(uint16_t ttl);
void mdns_srv_ttl_set(uint16_t ttl);
void mdns_txt_ttl_set(uint16_t ttl);
void mdns_a_ttl_set(uint16_t ttl);
void mdns_ptr_aaaa_set(uint16_t ttl);

#ifdef __cplusplus
}
#endif

#endif /* ESP_MDNS_H_ */
