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
    int scan;          /**< minimum scan times before being a root, default:10 */
    int vote;          /**< max vote times in self-healing, default:10000 */
    int fail;          /**< parent selection fail times, if the scan times reach this value,
                            will disconnect with associated children and join self-healing. default:60 */
    int monitor_ie;    /**< acceptable times of parent ie change before update self ie, default:3 */
} mesh_attempts_t;

typedef struct {
    int duration_ms;   /* parent weak RSSI monitor duration, if the RSSI continues to be weak during this duration_ms,
                          will switch to a better parent */
    int cnx_rssi;      /* RSSI threshold for keeping a good connection with parent */
    int select_rssi;   /* RSSI threshold for parent selection, should be a value greater than switch_rssi */
    int switch_rssi;   /* RSSI threshold for action to reselect a better parent */
    int backoff_rssi;  /* RSSI threshold for connecting to the root */
} mesh_switch_parent_t;

/**
 * @brief mesh networking IE
 */
typedef struct {
    /**< mesh networking IE head */
    uint8_t eid;             /**< element ID */
    uint8_t len;             /**< element length */
    uint8_t oui[3];          /**< organization identifier */
    /**< mesh networking IE content */
    uint8_t type;            /** mesh networking IE type */
    uint8_t encryped : 1;    /**< if mesh networking IE is encrypted */
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
    int8_t rssi;             /**< rssi of the parent */
    int8_t router_rssi;      /**< rssi of the router */
    uint8_t flag;            /**< flag of networking */
    uint8_t rc_addr[6];      /**< root address */
    int8_t rc_rssi;          /**< root rssi */
    uint8_t vote_addr[6];    /**< voter address */
    int8_t vote_rssi;        /**< vote rssi of the router */
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
 * @brief    set attempts for mesh self-organized networking
 *
 * @param    attempts
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
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_get_attempts(mesh_attempts_t *attempts);

/**
 * @brief     set parameters for parent switch
 *
 * @param     paras  parameters for parent switch
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_set_switch_parent_paras(mesh_switch_parent_t *paras);

/**
 * @brief     get parameters for parent switch
 *
 * @param     paras  parameters for parent switch
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_get_switch_parent_paras(mesh_switch_parent_t *paras);

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


#ifdef __cplusplus
}
#endif
#endif /* __ESP_MESH_INTERNAL_H__ */
