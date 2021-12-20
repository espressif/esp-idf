/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ESP_MDNS_H_
#define ESP_MDNS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <esp_netif.h>

#define MDNS_TYPE_A                 0x0001
#define MDNS_TYPE_PTR               0x000C
#define MDNS_TYPE_TXT               0x0010
#define MDNS_TYPE_AAAA              0x001C
#define MDNS_TYPE_SRV               0x0021
#define MDNS_TYPE_OPT               0x0029
#define MDNS_TYPE_NSEC              0x002F
#define MDNS_TYPE_ANY               0x00FF

/**
 * @brief   Asynchronous query handle
 */
typedef struct mdns_search_once_s mdns_search_once_t;


typedef enum {
    MDNS_EVENT_ENABLE_IP4                   = 1 << 1,
    MDNS_EVENT_ENABLE_IP6                   = 1 << 2,
    MDNS_EVENT_ANNOUNCE_IP4                 = 1 << 3,
    MDNS_EVENT_ANNOUNCE_IP6                 = 1 << 4,
    MDNS_EVENT_DISABLE_IP4                  = 1 << 5,
    MDNS_EVENT_DISABLE_IP6                  = 1 << 6,
} mdns_event_actions_t;

/**
 * @brief   mDNS enum to specify the ip_protocol type
 */
typedef enum {
    MDNS_IP_PROTOCOL_V4,
    MDNS_IP_PROTOCOL_V6,
    MDNS_IP_PROTOCOL_MAX
} mdns_ip_protocol_t;

/**
 * @brief   mDNS basic text item structure
 *          Used in mdns_service_add()
 */
typedef struct {
    const char * key;                       /*!< item key name */
    const char * value;                     /*!< item value string */
} mdns_txt_item_t;

/**
 * @brief   mDNS query linked list IP item
 */
typedef struct mdns_ip_addr_s {
    esp_ip_addr_t addr;                     /*!< IP address */
    struct mdns_ip_addr_s * next;           /*!< next IP, or NULL for the last IP in the list */
} mdns_ip_addr_t;

/**
 * @brief mDNS query type to be explicitly set to either Unicast or Multicast
 */
typedef enum {
    MDNS_QUERY_UNICAST,
    MDNS_QUERY_MULTICAST,
} mdns_query_transmission_type_t;

/**
 * @brief   mDNS query result structure
 */
typedef struct mdns_result_s {
    struct mdns_result_s * next;            /*!< next result, or NULL for the last result in the list */

    esp_netif_t* esp_netif;                 /*!< ptr to corresponding esp-netif */
    uint32_t ttl;                           /*!< time to live */

    mdns_ip_protocol_t ip_protocol;         /*!< ip_protocol type of the interface (v4/v6) */
    // PTR
    char * instance_name;                   /*!< instance name */
    char * service_type;                    /*!< service type */
    char * proto;                           /*!< srevice protocol */
    // SRV
    char * hostname;                        /*!< hostname */
    uint16_t port;                          /*!< service port */
    // TXT
    mdns_txt_item_t * txt;                  /*!< txt record */
    uint8_t *txt_value_len;                 /*!< array of txt value len of each record */
    size_t txt_count;                       /*!< number of txt items */
    // A and AAAA
    mdns_ip_addr_t * addr;                  /*!< linked list of IP addresses found */
} mdns_result_t;

typedef void (*mdns_query_notify_t)(mdns_search_once_t *search);

/**
 * @brief  Initialize mDNS on given interface
 *
 * @return
 *     - ESP_OK on success
 *     - ESP_ERR_INVALID_STATE when failed to register event handler
 *     - ESP_ERR_NO_MEM on memory error
 *     - ESP_FAIL when failed to start mdns task
 */
esp_err_t mdns_init(void);

/**
 * @brief  Stop and free mDNS server
 *
 */
void mdns_free(void);

/**
 * @brief  Set the hostname for mDNS server
 *         required if you want to advertise services
 *
 * @param  hostname     Hostname to set
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NO_MEM memory error
 */
esp_err_t mdns_hostname_set(const char * hostname);

/**
 * @brief  Adds a hostname and address to be delegated
 *         A/AAAA queries will be replied for the hostname and
 *         services can be added to this host.
 *
 * @param  hostname     Hostname to add
 * @param  address_list The IP address list of the host
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_STATE mDNS is not running
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NO_MEM memory error
 *
 */
esp_err_t mdns_delegate_hostname_add(const char * hostname, const mdns_ip_addr_t *address_list);

/**
 * @brief  Remove a delegated hostname
 *         All the services added to this host will also be removed.
 *
 * @param  hostname     Hostname to remove
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_STATE mDNS is not running
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NO_MEM memory error
 *
 */
esp_err_t mdns_delegate_hostname_remove(const char * hostname);

/**
 * @brief  Query whether a hostname has been added
 *
 * @param  hostname     Hostname to query
 *
 * @return
 *     - true   The hostname has been added.
 *     - false  The hostname has not been added.
 *
 */
bool mdns_hostname_exists(const char * hostname);

/**
 * @brief  Set the default instance name for mDNS server
 *
 * @param  instance_name     Instance name to set
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NO_MEM memory error
 */
esp_err_t mdns_instance_name_set(const char * instance_name);

/**
 * @brief  Add service to mDNS server
 *
 * @note The value length of txt items will be automatically decided by strlen
 *
 * @param  instance_name    instance name to set. If NULL,
 *                          global instance name or hostname will be used.
 *                          Note that MDNS_MULTIPLE_INSTANCE config option
 *                          needs to be enabled for adding multiple instances
 *                          with the same instance type.
 * @param  service_type     service type (_http, _ftp, etc)
 * @param  proto            service protocol (_tcp, _udp)
 * @param  port             service port
 * @param  txt              string array of TXT data (eg. {{"var","val"},{"other","2"}})
 * @param  num_items        number of items in TXT data
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NO_MEM memory error
 *     - ESP_FAIL failed to add service
 */
esp_err_t mdns_service_add(const char * instance_name, const char * service_type, const char * proto, uint16_t port, mdns_txt_item_t txt[], size_t num_items);

/**
 * @brief  Add service to mDNS server with a delegated hostname
 *
 * @note The value length of txt items will be automatically decided by strlen
 *
 * @param  instance_name    instance name to set. If NULL,
 *                          global instance name or hostname will be used
 *                          Note that MDNS_MULTIPLE_INSTANCE config option
 *                          needs to be enabled for adding multiple instances
 *                          with the same instance type.
 * @param  service_type     service type (_http, _ftp, etc)
 * @param  proto            service protocol (_tcp, _udp)
 * @param  hostname         service hostname. If NULL, local hostname will be used.
 * @param  port             service port
 * @param  txt              string array of TXT data (eg. {{"var","val"},{"other","2"}})
 * @param  num_items        number of items in TXT data
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NO_MEM memory error
 *     - ESP_FAIL failed to add service
 */
esp_err_t mdns_service_add_for_host(const char * instance_name, const char * service_type, const char * proto,
                                    const char * hostname, uint16_t port, mdns_txt_item_t txt[], size_t num_items);

/**
 * @brief  Check whether a service has been added.
 *
 * @param  service_type     service type (_http, _ftp, etc)
 * @param  proto            service protocol (_tcp, _udp)
 * @param  hostname         service hostname. If NULL, checks for the local hostname.
 *
 * @return
 *     - true   Correspondding service has been added.
 *     - false  Service not found.
 */
bool mdns_service_exists(const char * service_type, const char * proto, const char * hostname);


/**
 * @brief  Check whether a service has been added.
 *
 * @param  instance         instance name
 * @param  service_type     service type (_http, _ftp, etc)
 * @param  proto            service protocol (_tcp, _udp)
 * @param  hostname         service hostname. If NULL, checks for the local hostname.
 *
 * @return
 *     - true   Correspondding service has been added.
 *     - false  Service not found.
 */
bool mdns_service_exists_with_instance(const char *instance, const char *service_type, const char *proto,
                                       const char *hostname);

/**
 * @brief  Remove service from mDNS server
 *
 * @param  service_type service type (_http, _ftp, etc)
 * @param  proto        service protocol (_tcp, _udp)
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NOT_FOUND Service not found
 *     - ESP_ERR_NO_MEM memory error
 */
esp_err_t mdns_service_remove(const char * service_type, const char * proto);

/**
 * @brief  Remove service from mDNS server with hostname
 *
 * @param  instance     instance name
 * @param  service_type service type (_http, _ftp, etc)
 * @param  proto        service protocol (_tcp, _udp)
 * @param  hostname     service hostname. If NULL, local hostname will be used.
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NOT_FOUND Service not found
 *     - ESP_ERR_NO_MEM memory error
 */
esp_err_t mdns_service_remove_for_host(const char *instance, const char * service_type, const char * proto, const char *hostname);

/**
 * @brief  Set instance name for service
 *
 * @param  service_type     service type (_http, _ftp, etc)
 * @param  proto            service protocol (_tcp, _udp)
 * @param  instance_name    instance name to set
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NOT_FOUND Service not found
 *     - ESP_ERR_NO_MEM memory error
 */
esp_err_t mdns_service_instance_name_set(const char * service_type, const char * proto, const char * instance_name);

/**
 * @brief  Set instance name for service with hostname
 *
 * @param  instance_old     original instance name
 * @param  service_type     service type (_http, _ftp, etc)
 * @param  proto            service protocol (_tcp, _udp)
 * @param  hostname         service hostname. If NULL, local hostname will be used.
 * @param  instance_name    instance name to set
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NOT_FOUND Service not found
 *     - ESP_ERR_NO_MEM memory error
 */
esp_err_t mdns_service_instance_name_set_for_host(const char * instance_old, const char * service_type, const char * proto, const char * hostname,
                                                  const char * instance_name);

/**
 * @brief  Set service port
 *
 * @param  service_type service type (_http, _ftp, etc)
 * @param  proto        service protocol (_tcp, _udp)
 * @param  port         service port
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NOT_FOUND Service not found
 *     - ESP_ERR_NO_MEM memory error
 */
esp_err_t mdns_service_port_set(const char * service_type, const char * proto, uint16_t port);


/**
 * @brief  Set service port with hostname
 *
 * @param  instance     instance name
 * @param  service_type service type (_http, _ftp, etc)
 * @param  proto        service protocol (_tcp, _udp)
 * @param  hostname     service hostname. If NULL, local hostname will be used.
 * @param  port         service port
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NOT_FOUND Service not found
 *     - ESP_ERR_NO_MEM memory error
 */
esp_err_t mdns_service_port_set_for_host(const char * instance, const char * service_type, const char * proto, const char * hostname,
                                         uint16_t port);

/**
 * @brief  Replace all TXT items for service
 *
 * @note The value length of txt items will be automatically decided by strlen
 *
 * @param  service_type service type (_http, _ftp, etc)
 * @param  proto        service protocol (_tcp, _udp)
 * @param  txt          array of TXT data (eg. {{"var","val"},{"other","2"}})
 * @param  num_items    number of items in TXT data
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NOT_FOUND Service not found
 *     - ESP_ERR_NO_MEM memory error
 */
esp_err_t mdns_service_txt_set(const char * service_type, const char * proto, mdns_txt_item_t txt[], uint8_t num_items);

/**
 * @brief  Replace all TXT items for service with hostname
 *
 * @note The value length of txt items will be automatically decided by strlen
 *
 * @param  instance     instance name
 * @param  service_type service type (_http, _ftp, etc)
 * @param  proto        service protocol (_tcp, _udp)
 * @param  hostname     service hostname. If NULL, local hostname will be used.
 * @param  txt          array of TXT data (eg. {{"var","val"},{"other","2"}})
 * @param  num_items    number of items in TXT data
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NOT_FOUND Service not found
 *     - ESP_ERR_NO_MEM memory error
 */
esp_err_t mdns_service_txt_set_for_host(const char * instance, const char * service_type, const char * proto, const char * hostname,
                                        mdns_txt_item_t txt[], uint8_t num_items);

/**
 * @brief  Set/Add TXT item for service TXT record
 *
 * @note The value length will be automatically decided by strlen
 *
 * @param  service_type service type (_http, _ftp, etc)
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
esp_err_t mdns_service_txt_item_set(const char * service_type, const char * proto, const char * key, const char * value);

/**
 * @brief  Set/Add TXT item for service TXT record
 *
 * @param  service_type service type (_http, _ftp, etc)
 * @param  proto        service protocol (_tcp, _udp)
 * @param  key          the key that you want to add/update
 * @param  value        the new value of the key
 * @param  value_len    the length of the value
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NOT_FOUND Service not found
 *     - ESP_ERR_NO_MEM memory error
 */
esp_err_t mdns_service_txt_item_set_with_explicit_value_len(const char *service_type, const char *proto,
                                                            const char *key, const char *value, uint8_t value_len);

/**
 * @brief  Set/Add TXT item for service TXT record with hostname
 *
 * @note The value length will be automatically decided by strlen
 *
 * @param  instance     instance name
 * @param  service_type service type (_http, _ftp, etc)
 * @param  proto        service protocol (_tcp, _udp)
 * @param  hostname     service hostname. If NULL, local hostname will be used.
 * @param  key          the key that you want to add/update
 * @param  value        the new value of the key
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NOT_FOUND Service not found
 *     - ESP_ERR_NO_MEM memory error
 */
esp_err_t mdns_service_txt_item_set_for_host(const char * instance, const char * service_type, const char * proto, const char * hostname,
                                             const char * key, const char * value);

/**
 * @brief  Set/Add TXT item for service TXT record with hostname and txt value length
 *
 * @param  instance     instance name
 * @param  service_type service type (_http, _ftp, etc)
 * @param  proto        service protocol (_tcp, _udp)
 * @param  hostname     service hostname. If NULL, local hostname will be used.
 * @param  key          the key that you want to add/update
 * @param  value        the new value of the key
 * @param  value_len    the length of the value
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NOT_FOUND Service not found
 *     - ESP_ERR_NO_MEM memory error
 */
esp_err_t mdns_service_txt_item_set_for_host_with_explicit_value_len(const char * instance, const char *service_type, const char *proto,
                                                                     const char *hostname, const char *key,
                                                                     const char *value, uint8_t value_len);

/**
 * @brief  Remove TXT item for service TXT record
 *
 * @param  service_type service type (_http, _ftp, etc)
 * @param  proto        service protocol (_tcp, _udp)
 * @param  key          the key that you want to remove
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NOT_FOUND Service not found
 *     - ESP_ERR_NO_MEM memory error
 */
esp_err_t mdns_service_txt_item_remove(const char * service_type, const char * proto, const char * key);

/**
 * @brief  Remove TXT item for service TXT record with hostname
 *
 * @param  instance     instance name
 * @param  service_type service type (_http, _ftp, etc)
 * @param  proto        service protocol (_tcp, _udp)
 * @param  hostname     service hostname. If NULL, local hostname will be used.
 * @param  key          the key that you want to remove
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NOT_FOUND Service not found
 *     - ESP_ERR_NO_MEM memory error
 */
esp_err_t mdns_service_txt_item_remove_for_host(const char * instance, const char * service_type, const char * proto, const char * hostname,
                                                const char * key);

/**
 * @brief  Add subtype for service.
 *
 * @param  instance_name    instance name. If NULL, will find the first service with the same service type and protocol.
 * @param  service_type     service type (_http, _ftp, etc)
 * @param  proto            service protocol (_tcp, _udp)
 * @param  hostname         service hostname. If NULL, local hostname will be used.
 * @param  subtype          The subtype to add.
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NOT_FOUND Service not found
 *     - ESP_ERR_NO_MEM memory error
 */
esp_err_t mdns_service_subtype_add_for_host(const char *instance_name, const char *service_type, const char *proto,
                                            const char *hostname, const char *subtype);

/**
 * @brief  Remove and free all services from mDNS server
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t mdns_service_remove_all(void);

/**
 * @brief Deletes the finished query. Call this only after the search has ended!
 *
 * @param search pointer to search object
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_STATE  search has not finished
 *     - ESP_ERR_INVALID_ARG    pointer to search object is NULL
 */
esp_err_t mdns_query_async_delete(mdns_search_once_t* search);

/**
 * @brief Get results from search pointer. Results available as a pointer to the output parameter.
 *        Pointer to search object has to be deleted via `mdns_query_async_delete` once the query has finished.
 *        The results although have to be freed manually.
 *
 * @param search pointer to search object
 * @param timeout time in milliseconds to wait for answers
 * @param results pointer to the results of the query
 * @param num_results pointer to the number of the actual result items (set to NULL to ignore this return value)
 *
 * @return
 *      True if search has finished before or at timeout
 *      False if search timeout is over
 */
bool mdns_query_async_get_results(mdns_search_once_t* search, uint32_t timeout, mdns_result_t ** results, uint8_t * num_results);

/**
 * @brief  Query mDNS for host or service asynchronousely.
 *         Search has to be tested for progress and deleted manually!
 *
 * @param  name         service instance or host name (NULL for PTR queries)
 * @param  service_type service type (_http, _arduino, _ftp etc.) (NULL for host queries)
 * @param  proto        service protocol (_tcp, _udp, etc.) (NULL for host queries)
 * @param  type         type of query (MDNS_TYPE_*)
 * @param  timeout      time in milliseconds during which mDNS query is active
 * @param  max_results  maximum results to be collected
 * @param  notifier     Notification function to be called when the result is ready, can be NULL
 *
 * @return mdns_search_once_s pointer to new search object if query initiated successfully.
 *         NULL otherwise.
 */
mdns_search_once_t *mdns_query_async_new(const char *name, const char *service_type, const char *proto, uint16_t type,
                                         uint32_t timeout, size_t max_results, mdns_query_notify_t notifier);

/**
 * @brief  Generic mDNS query
 *         All following query methods are derived from this one
 *
 * @param  name         service instance or host name (NULL for PTR queries)
 * @param  service_type service type (_http, _arduino, _ftp etc.) (NULL for host queries)
 * @param  proto        service protocol (_tcp, _udp, etc.) (NULL for host queries)
 * @param  type         type of query (MDNS_TYPE_*)
 * @param  transmission_type      either Unicast query, or Multicast query
 * @param  timeout      time in milliseconds to wait for answers.
 * @param  max_results  maximum results to be collected
 * @param  results      pointer to the results of the query
 *                      results must be freed using mdns_query_results_free below
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_STATE  mDNS is not running
 *     - ESP_ERR_NO_MEM         memory error
 *     - ESP_ERR_INVALID_ARG    timeout was not given
 */
esp_err_t mdns_query_generic(const char * name, const char * service_type, const char * proto, uint16_t type,
                             mdns_query_transmission_type_t transmission_type, uint32_t timeout, size_t max_results, mdns_result_t ** results);

/**
 * @brief  Query mDNS for host or service
 *
 * Note that querying PTR types sends Multicast query, all other types send Unicast queries
 *
 * @param  name         service instance or host name (NULL for PTR queries)
 * @param  service_type service type (_http, _arduino, _ftp etc.) (NULL for host queries)
 * @param  proto        service protocol (_tcp, _udp, etc.) (NULL for host queries)
 * @param  type         type of query (MDNS_TYPE_*)
 * @param  timeout      time in milliseconds to wait for answers.
 * @param  max_results  maximum results to be collected
 * @param  results      pointer to the results of the query
 *                      results must be freed using mdns_query_results_free below
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_STATE  mDNS is not running
 *     - ESP_ERR_NO_MEM         memory error
 *     - ESP_ERR_INVALID_ARG    timeout was not given
 */
esp_err_t mdns_query(const char * name, const char * service_type, const char * proto, uint16_t type, uint32_t timeout, size_t max_results, mdns_result_t ** results);

/**
 * @brief  Free query results
 *
 * @param  results      linked list of results to be freed
 */
void mdns_query_results_free(mdns_result_t * results);

/**
 * @brief  Query mDNS for service
 *
 * @param  service_type service type (_http, _arduino, _ftp etc.)
 * @param  proto        service protocol (_tcp, _udp, etc.)
 * @param  timeout      time in milliseconds to wait for answer.
 * @param  max_results  maximum results to be collected
 * @param  results      pointer to the results of the query
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_STATE  mDNS is not running
 *     - ESP_ERR_NO_MEM         memory error
 *     - ESP_ERR_INVALID_ARG    parameter error
 */
esp_err_t mdns_query_ptr(const char * service_type, const char * proto, uint32_t timeout, size_t max_results, mdns_result_t ** results);

/**
 * @brief  Query mDNS for SRV record
 *
 * @param  instance_name    service instance name
 * @param  service_type     service type (_http, _arduino, _ftp etc.)
 * @param  proto            service protocol (_tcp, _udp, etc.)
 * @param  timeout          time in milliseconds to wait for answer.
 * @param  result           pointer to the result of the query
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_STATE  mDNS is not running
 *     - ESP_ERR_NO_MEM         memory error
 *     - ESP_ERR_INVALID_ARG    parameter error
 */
esp_err_t mdns_query_srv(const char * instance_name, const char * service_type, const char * proto, uint32_t timeout, mdns_result_t ** result);

/**
 * @brief  Query mDNS for TXT record
 *
 * @param  instance_name    service instance name
 * @param  service_type     service type (_http, _arduino, _ftp etc.)
 * @param  proto            service protocol (_tcp, _udp, etc.)
 * @param  timeout          time in milliseconds to wait for answer.
 * @param  result           pointer to the result of the query
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_STATE  mDNS is not running
 *     - ESP_ERR_NO_MEM         memory error
 *     - ESP_ERR_INVALID_ARG    parameter error
 */
esp_err_t mdns_query_txt(const char * instance_name, const char * service_type, const char * proto, uint32_t timeout, mdns_result_t ** result);

/**
 * @brief  Query mDNS for A record
 *
 * @param  host_name    host name to look for
 * @param  timeout      time in milliseconds to wait for answer.
 * @param  addr         pointer to the resulting IP4 address
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_STATE  mDNS is not running
 *     - ESP_ERR_NO_MEM         memory error
 *     - ESP_ERR_INVALID_ARG    parameter error
 */
esp_err_t mdns_query_a(const char * host_name, uint32_t timeout, esp_ip4_addr_t * addr);

#if CONFIG_LWIP_IPV6
/**
 * @brief  Query mDNS for A record
 *
 * Please note that hostname must not contain domain name, as mDNS uses '.local' domain.
 *
 * @param  host_name    host name to look for
 * @param  timeout      time in milliseconds to wait for answer. If 0, max_results needs to be defined
 * @param  addr         pointer to the resulting IP6 address
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_STATE  mDNS is not running
 *     - ESP_ERR_NO_MEM         memory error
 *     - ESP_ERR_INVALID_ARG    parameter error
 */
esp_err_t mdns_query_aaaa(const char * host_name, uint32_t timeout, esp_ip6_addr_t * addr);
#endif


/**
 * @brief   Register custom esp_netif with mDNS functionality
 *          mDNS service runs by on default interfaces (STA, AP, ETH). This API enables running the service
 *          on any customized interface, either using standard WiFi or Ethernet driver or any kind of user
 *          defined driver.
 *
 * @param   esp_netif Pointer to esp-netif interface
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_STATE  mDNS is not running or this netif is already registered
 *     - ESP_ERR_NO_MEM         not enough memory for this in interface in the netif list (see CONFIG_MDNS_MAX_INTERFACES)
 */
esp_err_t mdns_register_esp_netif(esp_netif_t *esp_netif);

/**
 * @brief   Unregister esp-netif already registered in mDNS service
 *
 * @param   esp_netif Pointer to esp-netif interface
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_STATE  mDNS is not running
 *     - ESP_ERR_NOT_FOUND      this esp-netif was not registered in mDNS service
 */
esp_err_t mdns_unregister_esp_netif(esp_netif_t *esp_netif);

/**
 * @brief   Set esp_netif to a desired state, or perform a desired action, such as enable/disable this interface
 *          or send announcement packets to this netif
 * @param esp_netif  Pointer to esp-netif interface
 * @param event_action  Disable/Enable/Announce on this interface over IPv4/IPv6 protocol.
 *                      Actions enumerated in mdns_event_actions_t type.
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_STATE  mDNS is not running or this netif is not registered
 *     - ESP_ERR_NO_MEM         memory error
 */
esp_err_t mdns_set_esp_netif_action(esp_netif_t *esp_netif, mdns_event_actions_t event_action);

#ifdef __cplusplus
}
#endif

#endif /* ESP_MDNS_H_ */
