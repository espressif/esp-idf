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

#ifndef __ESP_MESH_INTERNAL_H__
#define __ESP_MESH_INTERNAL_H__

#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_wifi_internal.h"
#include "esp_wifi_crypto_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************
 *                Constants
 *******************************************************/

/*******************************************************
 *                Structures
 *******************************************************/
typedef struct {
    int scan;          /**< minimum scan times before being a root, default:10. */
    int vote;          /**< max vote times in self-healing, default:1000. */
    int fail;          /**< parent selection fail times. If the scan times reach this value,
                            device will disconnect with associated children and join self-healing, default:120. */
    int monitor_ie;    /**< acceptable times of parent networking IE change before update self networking IE, default:10. */
} mesh_attempts_t;

typedef struct {
    int duration_ms;   /* parent weak RSSI monitor duration. If the RSSI with current parent is less than cnx_rssi continuously
                          within this duration_ms, device will search for a better parent. */
    int cnx_rssi;      /* RSSI threshold for keeping a good connection with parent.
                          If set a value greater than -120 dBm, device will arm a timer to monitor current RSSI at a period time of
                          duration_ms. */
    int select_rssi;   /* RSSI threshold for parent selection, should be a value greater than switch_rssi. */
    int switch_rssi;   /* RSSI threshold for parent switch. Device will disassociate current parent and switch to a new parent when
                          the RSSI with the new parent is greater than this set threshold. */
    int backoff_rssi;  /* RSSI threshold for connecting to the root */
} mesh_switch_parent_t;

typedef struct {
    int high;
    int medium;
    int low;
} mesh_rssi_threshold_t;

/**
 * @brief mesh networking IE
 */
typedef struct {
    /**< mesh networking IE head */
    uint8_t eid;             /**< element ID */
    uint8_t len;             /**< element length */
    uint8_t oui[3];          /**< organization identifier */
    /**< mesh networking IE content */
    uint8_t type;            /** ESP defined IE type */
    uint8_t encryped : 1;    /**< whether mesh networking IE is encrypted */
    uint8_t version : 7;     /**< mesh networking IE version */
    /**< content */
    uint8_t mesh_type;       /**< mesh device type */
    uint8_t mesh_id[6];      /**< mesh ID */
    uint8_t layer_cap;       /**< max layer */
    uint8_t layer;           /**< current layer */
    uint8_t assoc_cap;       /**< max connections of mesh AP */
    uint8_t assoc;           /**< current connections */
    uint8_t leaf_cap;        /**< leaf capacity */
    uint8_t leaf_assoc;      /**< the number of current connected leaf */
    uint16_t root_cap;       /**< root capacity */
    uint16_t self_cap;       /**< self capacity */
    uint16_t layer2_cap;     /**< layer2 capacity */
    uint16_t scan_ap_num;    /**< the number of scanned APs */
    int8_t rssi;             /**< RSSI of the parent */
    int8_t router_rssi;      /**< RSSI of the router */
    uint8_t flag;            /**< flag of networking */
    uint8_t rc_addr[6];      /**< root address */
    int8_t rc_rssi;          /**< root RSSI */
    uint8_t vote_addr[6];    /**< voter address */
    int8_t vote_rssi;        /**< vote RSSI of the router */
    uint8_t vote_ttl;        /**< vote ttl */
    uint16_t votes;          /**< votes */
    uint16_t my_votes;       /**< my votes */
    uint8_t reason;          /**< reason */
    uint8_t child[6];        /**< child address */
    uint8_t toDS;            /**< toDS state */
} __attribute__((packed)) mesh_assoc_t;

/*******************************************************
 *                Function Definitions
 *******************************************************/
/**
 * @brief     set mesh softAP beacon interval
 *
 * @param     interval  beacon interval(ms) (100ms ~ 60000ms)
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 *    - ESP_ERR_WIFI_ARG
 */
esp_err_t esp_mesh_set_beacon_interval(int interval_ms);

/**
 * @brief     get mesh softAP beacon interval
 *
 * @param     interval  beacon interval(ms)
 *
 * @return
 *    - ESP_OK
 */
esp_err_t esp_mesh_get_beacon_interval(int *interval_ms);

/**
 * @brief     set attempts for mesh self-organized networking
 *
 * @param     attempts
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_set_attempts(mesh_attempts_t *attempts);

/**
 * @brief    get attempts for mesh self-organized networking
 *
 * @param    attempts
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 */
esp_err_t esp_mesh_get_attempts(mesh_attempts_t *attempts);

/**
 * @brief     set parameters for parent switch
 *
 * @param     paras  parameters for parent switch
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 */
esp_err_t esp_mesh_set_switch_parent_paras(mesh_switch_parent_t *paras);

/**
 * @brief     get parameters for parent switch
 *
 * @param     paras  parameters for parent switch
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 */
esp_err_t esp_mesh_get_switch_parent_paras(mesh_switch_parent_t *paras);

/**
 * @brief     set RSSI threshold
 *            The default high RSSI threshold value is -78 dBm.
 *            The default medium RSSI threshold value is -82 dBm.
 *            The default low RSSI threshold value is -85 dBm.
 *
 * @param     threshold  RSSI threshold
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 */
esp_err_t esp_mesh_set_rssi_threshold(const mesh_rssi_threshold_t *threshold);

/**
 * @brief     get RSSI threshold
 * @param     threshold  RSSI threshold
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 */
esp_err_t esp_mesh_get_rssi_threshold(mesh_rssi_threshold_t *threshold);

/**
 * @brief     enable the minimum rate to 6Mbps
 *
 * @attention This API shall be called before WiFi start.
 *
 * @param     is_6m  enable or not
 *
 * @return
 *    - ESP_OK
 */
esp_err_t esp_mesh_set_6m_rate(bool is_6m);

/**
 * @brief     print the number of txQ waiting
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_print_txQ_waiting(void);

/**
 * @brief     print the number of rxQ waiting
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_print_rxQ_waiting(void);

/**
 * @brief     set passive scan time
 *
 * @param     interval_ms  passive scan time(ms)
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 *    - ESP_ERR_ARGUMENT
 */
esp_err_t esp_mesh_set_passive_scan_time(int time_ms);

/**
 * @brief     get passive scan time
 *
 * @return    interval_ms  passive scan time(ms)
 */
int esp_mesh_get_passive_scan_time(void);

/**
 * @brief     set announce interval
 *            The default short interval is 500 milliseconds.
 *            The default long interval is 3000 milliseconds.
 *
 * @param     short_ms  shall be greater than the default value
 * @param     long_ms  shall be greater than the default value
 *
 * @return
 *    - ESP_OK
 */
esp_err_t esp_mesh_set_announce_interval(int short_ms, int long_ms);

/**
 * @brief     get announce interval
 *
 * @param     short_ms  short interval
 * @param     long_ms  long interval
 *
 * @return
 *    - ESP_OK
 */
esp_err_t esp_mesh_get_announce_interval(int *short_ms, int *long_ms);

#ifdef __cplusplus
}
#endif
#endif /* __ESP_MESH_INTERNAL_H__ */
