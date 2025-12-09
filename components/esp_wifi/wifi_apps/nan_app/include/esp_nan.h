/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_log.h"
#include "esp_err.h"
#include "lwip/inet.h"
#include "esp_wifi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_ESP_WIFI_NAN_SYNC_ENABLE
#define WIFI_NAN_SYNC_CONFIG_DEFAULT() { \
    .op_channel = 6, \
    .master_pref = 2, \
    .scan_time = 3, \
    .warm_up_sec = 5, \
};

#define NDP_STATUS_ACCEPTED     1
#define NDP_STATUS_REJECTED     2

#define NAN_MAX_PEERS_RECORD    15
#define ESP_NAN_PUBLISH         2
#define ESP_NAN_SUBSCRIBE       1

/** Parameters of a peer service record */
struct nan_peer_record {
    uint8_t peer_svc_id;   /**< Identifier of Peer's service */
    uint8_t own_svc_id;    /**< Identifier of own service associated with Peer */
    uint8_t peer_nmi[6];   /**< Peer's NAN Management Interface address */
    uint8_t peer_svc_type; /**< Peer's service type (Publish/Subscribe) */
    uint8_t ndp_id;        /**< Specifies if the peer has any active datapath */
    uint8_t peer_ndi[6];   /**< Peer's NAN Data Interface address, only valid when ndp_id is non-zero */
};

/**
  * @brief      Start NAN Synchronization using the provided parameters.
  * @note       Discovery traffic begins only after publish/subscribe services are started.
  *
  * @attention  This API should be called after esp_wifi_init().
  *
  * @param      nan_cfg  NAN related parameters to be configured.
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_wifi_nan_sync_start(const wifi_nan_sync_config_t *nan_cfg);

/**
  * @brief      Stop NAN Synchronization discovery and end publish/subscribe services
  *
  * @attention  This API will end datapaths if any in NAN synchronization
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_wifi_nan_sync_stop(void);

/**
  * @brief      Send a NAN datapath request to a matched publisher within the synchronized NAN cluster
  *
  * @attention  This API should be called by the Subscriber after a match occurs with a Publisher.
  *
  * @param      req  NAN Datapath Request parameters.
  *
  * @return
  *    - non-zero NAN Datapath identifier: If NAN datapath req was accepted by publisher
  *    - zero: If NAN datapath req was rejected by publisher or a timeout occurs
  */
uint8_t esp_wifi_nan_datapath_req(wifi_nan_datapath_req_t *req);

/**
  * @brief      Respond to a NAN datapath request within the synchronized NAN cluster
  *
  * @attention  This API should be called if ndp_resp_needed is set 1 in wifi_nan_publish_cfg_t and
  *             a WIFI_EVENT_NDP_INDICATION event is received due to an incoming NDP request.
  *
  * @param      resp  NAN Datapath Response parameters.
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_wifi_nan_datapath_resp(wifi_nan_datapath_resp_t *resp);

/**
  * @brief      Terminate an active NAN datapath within the synchronized NAN cluster
  *
  * @param      req  NAN Datapath end request parameters.
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_wifi_nan_datapath_end(wifi_nan_datapath_end_req_t *req);

/**
  * @brief      Get IPv6 Link Local address using MAC address
  *
  * @param[out]      ip6  Derived IPv6 Link Local address.
  * @param[in]       mac_addr  Input MAC Address.
  */
void esp_wifi_nan_get_ipv6_linklocal_from_mac(ip6_addr_t *ip6, uint8_t *mac_addr);

/**
 * brief         Get own Service information from Service ID OR Name.
 *
 * @attention    If service information is to be fetched from service name, set own_svc_id as zero.
 * @note         Returns records discovered while participating in a synchronized NAN cluster.
 *
 * @param[inout] own_svc_id As input, it indicates Service ID to search for.
 *                          As output, it indicates Service ID of the service found using Service Name.
 * @param[inout] svc_name   As input, it indicates Service Name to search for.
 *                          As output, it indicates Service Name of the service found using Service ID.
 * @param[out]   num_peer_records  Number of peers discovered by corresponding service.
 * @return
 *   - ESP_OK: succeed
 *   - ESP_FAIL: failed
 */

esp_err_t esp_wifi_nan_get_own_svc_info(uint8_t *own_svc_id, char *svc_name, int *num_peer_records);

/**
 * brief         Get a list of Peers discovered by the given Service.
 *
 * @note         Reports peers discovered via synchronized NAN operations.
 *
 * @param[inout] num_peer_records As input param, it stores max peers peer_record can hold.
 *               As output param, it specifies the actual number of peers this API returns.
 * @param        own_svc_id  Service ID of own service.
 * @param[out]   peer_record Pointer to first peer record.
 * @return
 *   - ESP_OK: succeed
 *   - ESP_FAIL: failed
 */

esp_err_t esp_wifi_nan_get_peer_records(int *num_peer_records, uint8_t own_svc_id, struct nan_peer_record *peer_record);

/**
 * brief         Find Peer's Service information using Peer MAC and optionally Service Name.
 *
 * @note         Provides peer information available from synchronized NAN discovery.
 *
 * @param       svc_name    Service Name of the published/subscribed service.
 * @param       peer_mac    Peer's NAN Management Interface MAC address.
 * @param[out]  peer_info   Peer's service information structure.
 * @return
 *   - ESP_OK: succeed
 *   - ESP_FAIL: failed
 */

esp_err_t esp_wifi_nan_get_peer_info(char *svc_name, uint8_t *peer_mac, struct nan_peer_record *peer_info);

#endif /* CONFIG_ESP_WIFI_NAN_SYNC_ENABLE */

#if defined(CONFIG_ESP_WIFI_NAN_SYNC_ENABLE) ||  defined(CONFIG_ESP_WIFI_NAN_USD_ENABLE)
/**
  * @brief      Start publishing a service to NAN peers within a synchronized cluster or to NAN-USD peers
  *
  * @attention  Call after `esp_wifi_nan_sync_start()` when operating in NAN-Sync, or after
  *             `esp_wifi_nan_usd_start()` when operating in NAN-USD.
  * @attention  When NAN-USD discovery is active the publish request is routed through the
  *             USD engine, otherwise it uses NAN synchronization.
  * @attention  A maximum of two services is allowed simultaneously
  *             (e.g., one publish and one subscribe, or two publish/subscribe).
  *             This limit is defined by the ESP_WIFI_NAN_MAX_SVC_SUPPORTED.
  *
  * @param      publish_cfg  Configuration parameters for publishing a service.
  *                          When operating in NAN-USD, populate `publish_cfg->usd_publish_config`
  *                          (for example using `esp_wifi_usd_get_default_publish_cfg()`) to tune
  *                          the USD dwell and channel parameters.
  *
  * @return
  *    - non-zero: Publish service identifier
  *    - zero: failed
  */
uint8_t esp_wifi_nan_publish_service(const wifi_nan_publish_cfg_t *publish_cfg);

/**
  * @brief      Subscribe to a service published by a NAN peer within a synchronized cluster or by a NAN-USD peer
  *
  * @attention  Call after `esp_wifi_nan_sync_start()` when operating in NAN-Sync, or after
  *             `esp_wifi_nan_usd_start()` when operating in NAN-USD.
  * @attention  With NAN-USD discovery enabled the subscribe request is handled by the USD
  *             engine, otherwise it uses NAN synchronization.
  * @attention  A maximum of two services is allowed simultaneously
  *             (e.g., one publish and one subscribe, or two publish/subscribe).
  *             This limit is defined by the ESP_WIFI_NAN_MAX_SVC_SUPPORTED.
  *
  * @param      subscribe_cfg  Configuration parameters for subscribing for a service.
  *                            When operating in NAN-USD, populate `subscribe_cfg->usd_subscribe_config`
  *                            (for example using `esp_wifi_usd_get_default_subscribe_cfg()`) to tune
  *                            the USD dwell and channel parameters.
  *
  * @return
  *    - non-zero: Subscribe service identifier
  *    - zero: failed
  */
uint8_t esp_wifi_nan_subscribe_service(const wifi_nan_subscribe_cfg_t *subscribe_cfg);

/**
  * @brief      Send a follow-up message to a matched peer in NAN synchronization or USD
  *
  * @attention  This API should be called after a NAN/USD service is discovered due to a match.
  *
  * @param      fup_params  Configuration parameters for sending a Follow-up message.
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_wifi_nan_send_message(wifi_nan_followup_params_t *fup_params);

/**
  * @brief      Cancel a NAN synchronization or NAN-USD service
  *
  * @param      service_id Publish/Subscribe service id to be cancelled.
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_wifi_nan_cancel_service(uint8_t service_id);

#endif /* NAN-Sync || NAN-USD */

#ifdef CONFIG_ESP_WIFI_NAN_USD_ENABLE
/**
  * @brief      Start NAN Unsynchronized service discovery (USD)
  *
  * @attention  This API should be called after esp_wifi_start().
  * @note       USD currently operates over the Station interface; this may change in future releases.
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_wifi_nan_usd_start(void);

/**
  * @brief      Stop NAN Unsynchronized service discovery (USD) and end publish/subscribe services
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_wifi_nan_usd_stop(void);

/**
  * @brief      Get default configuration for USD publish operation.
  *
  * @note       This function returns a default configuration structure for initiating
  *             a USD (Unsynchronized Service Discovery) publish session.
  *             - The default channel is set to channel 6 (2.437 GHz) in the 2.4 GHz band.
  *             - The channel list will include all 20 MHz channels in the 2.4 GHz frequency band,
  *               as permitted by the regulatory domain of the current geographic location.
  *             - Default values for dwell periods are set as per Wi-Fi Aware Specification:
  *                 - Nmin = 5, Nmax = 10
  *                 - Mmin = 5, Mmax = 10
  *
  * @return     wifi_nan_usd_config_t structure with pre-filled default values for publishing.
  */
wifi_nan_usd_config_t esp_wifi_usd_get_default_publish_cfg(void);

/**
 * @brief      Get default configuration for USD subscribe operation.
 *
 * @note       This function returns a default configuration structure for initiating
 *             a USD (Unsynchronized Service Discovery) subscribe session.
 *             - The default channel is set to channel 6 (2.437 GHz) in the 2.4 GHz band.
 *             - The channel list will include all 20 MHz channels in the 2.4 GHz frequency band,
 *               as permitted by the regulatory domain of the current geographic location.
 *             - Default values for dwell periods are set as per Wi-Fi Aware Specification:
 *                 - Nmin = 5, Nmax = 10
 *                 - Mmin = 5, Mmax = 10
 *
 *             @warning Currently, the subscribe function operates only on the default channel.
 *                      It does not alternate between the default channel and channels in the channel list.
 *
 * @return     wifi_nan_usd_config_t structure with pre-filled default values for subscribing.
 */
wifi_nan_usd_config_t esp_wifi_usd_get_default_subscribe_cfg(void);

#endif /* CONFIG_ESP_WIFI_NAN_USD_ENABLE */

#ifdef __cplusplus
}
#endif
