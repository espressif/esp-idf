/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
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

#define WIFI_NAN_CONFIG_DEFAULT() { \
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
  * @brief      Start NAN Discovery with provided configuration
  *
  * @attention  This API should be called after esp_wifi_init().
  *
  * @param      nan_cfg  NAN related parameters to be configured.
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_wifi_nan_start(const wifi_nan_config_t *nan_cfg);

/**
  * @brief      Stop NAN Discovery, end NAN Services and Datapaths
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_wifi_nan_stop(void);

/**
  * @brief      Start Publishing a service to the NAN Peers in vicinity
  *
  * @attention  This API should be called after esp_wifi_nan_start().
  *
  * @param      publish_cfg  Configuration parameters for publishing a service.
  * @param      ndp_resp_needed Setting this true will require user response for every NDP Req using esp_wifi_nan_datapath_resp API.
  *
  * @return
  *    - non-zero: Publish service identifier
  *    - zero: failed
  */
uint8_t esp_wifi_nan_publish_service(const wifi_nan_publish_cfg_t *publish_cfg, bool ndp_resp_needed);

/**
  * @brief      Subscribe for a service within the NAN cluster
  *
  * @attention  This API should be called after esp_wifi_nan_start().
  *
  * @param      subscribe_cfg  Configuration parameters for subscribing for a service.
  *
  * @return
  *    - non-zero: Subscribe service identifier
  *    - zero: failed
  */
uint8_t esp_wifi_nan_subscribe_service(const wifi_nan_subscribe_cfg_t *subscribe_cfg);

/**
  * @brief      Send a follow-up message to the NAN Peer with matched service
  *
  * @attention  This API should be called after a NAN service is discovered due to a match.
  *
  * @param      fup_params  Configuration parameters for sending a Follow-up message.
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_wifi_nan_send_message(wifi_nan_followup_params_t *fup_params);

/**
  * @brief      Cancel a NAN service
  *
  * @param      service_id Publish/Subscribe service id to be cancelled.
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_wifi_nan_cancel_service(uint8_t service_id);

/**
  * @brief      Send NAN Datapath Request to a NAN Publisher with matched service
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
  * @brief      Respond to a NAN Datapath request with Accept or Reject
  *
  * @attention  This API should be called if ndp_resp_needed is set True by the Publisher and
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
  * @brief      Terminate a NAN Datapath
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
 * @param       svc_name    Service Name of the published/subscribed service.
 * @param       peer_mac    Peer's NAN Management Interface MAC address.
 * @param[out]  peer_info   Peer's service information structure.
 * @return
 *   - ESP_OK: succeed
 *   - ESP_FAIL: failed
 */
esp_err_t esp_wifi_nan_get_peer_info(char *svc_name, uint8_t *peer_mac, struct nan_peer_record *peer_info);

#ifdef __cplusplus
}
#endif
