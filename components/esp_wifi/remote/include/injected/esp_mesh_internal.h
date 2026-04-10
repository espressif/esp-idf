/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_MESH_INTERNAL_H__
#define __ESP_MESH_INTERNAL_H__

#include "esp_err.h"
#include "esp_mesh.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_private/wifi.h"
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
/**
 * @brief Mesh attempts
 */
typedef struct {
    int scan;          /**< minimum scan times before being a root, default:10 */
    int vote;          /**< max vote times in self-healing, default:1000 */
    int fail;          /**< parent selection fail times, if the scan times reach this value,
                            device will disconnect with associated children and join self-healing. default:60 */
    int monitor_ie;    /**< acceptable times of parent networking IE change before update its own networking IE. default:3 */
} mesh_attempts_t;

/**
 * @brief Mesh switch parent
 */
typedef struct {
    int duration_ms;   /**< parent weak RSSI monitor duration, if the RSSI continues to be weak during this duration_ms,
                          device will search for a new parent. */
    int cnx_rssi;      /**< RSSI threshold for keeping a good connection with parent.
                          If set a value greater than -120 dBm, a timer will be armed to monitor parent RSSI at a period time of duration_ms. */
    int select_rssi;   /**< RSSI threshold for parent selection. It should be a value greater than switch_rssi. */
    int switch_rssi;   /**< Disassociate with current parent and switch to a new parent when the RSSI is greater than this set threshold. */
    int backoff_rssi;  /**< RSSI threshold for connecting to the root */
} mesh_switch_parent_t;

/**
 * @brief Mesh RSSI threshold
 */
typedef struct {
    int high;   /**<  high RSSI threshold, used to determine whether the new parent and the current parent are in the same RSSI range */
    int medium; /**<  medium RSSI threshold, used to determine whether the new parent and the current parent are in the same RSSI range */
    int low;    /**<  low RSSI threshold. If the parent's RSSI is lower than low for a period time of duration_ms,
                      then the mesh node will post MESH_WEAK_RSSI event.
                      Also used to determine whether the new parent and the current parent are in the same RSSI range */
} mesh_rssi_threshold_t;

/**
 * @brief Mesh networking IE
 */
typedef struct {
    /**< mesh networking IE head */
    uint8_t eid;             /**< element ID, vendor specific, 221 */
    uint8_t len;             /**< element length, the length after this member */
    uint8_t oui[3];          /**< organization identifier, 0x18fe34 */
    uint8_t type;            /**< ESP defined IE type, include Assoc IE, SSID IE, Ext Assoc IE, Roots IE, etc. */
    uint8_t encrypted : 1;   /**< whether mesh networking IE is encrypted */
    uint8_t version : 7;     /**< mesh networking IE version, equal to 2 if mesh PS is enabled, equal to 1 otherwise */
    /**< content */
    uint8_t mesh_type;       /**< mesh device type, include idle, root, node, etc, refer to mesh_type_t */
    uint8_t mesh_id[6];      /**< mesh ID, only the same mesh id can form a unified mesh network */
    uint8_t layer_cap;       /**< layer_cap = max_layer - layer, indicates the number of remaining available layers of the mesh network */
    uint8_t layer;           /**< the current layer of this node */
    uint8_t assoc_cap;       /**< the maximum connections of this mesh AP */
    uint8_t assoc;           /**< current connections of this mesh AP */
    uint8_t leaf_cap;        /**< the maximum number of leaves in the mesh network */
    uint8_t leaf_assoc;      /**< the number of current connected leaves */
    uint16_t root_cap;       /**< the capacity of the root, equal to the total child numbers plus 1, root node updates root_cap and self_cap */
    uint16_t self_cap;       /**< the capacity of myself, total child numbers plus 1, all nodes update this member */
    uint16_t layer2_cap;     /**< the capacity of layer2 node, total child numbers plus 1, layer2 node updates layer2_cap and self_cap, root sets this to 0 */
    uint16_t scan_ap_num;    /**< the number of mesh APs around */
    int8_t rssi;             /**< RSSI of the connected parent, default value is -120, root node will not update this */
    int8_t router_rssi;      /**< RSSI of the router, default value is -120 */
    uint8_t flag;            /**< flag of networking, indicates the status of the network, refer to MESH_ASSOC_FLAG_XXX */
    /**< vote related */
    uint8_t rc_addr[6];      /**< the address of the root candidate, i.e. the voted addesss before connection, root node will update this with self address */
    int8_t rc_rssi;          /**< the router RSSI of the root candidate */
    uint8_t vote_addr[6];    /**< the voted address after connection */
    int8_t vote_rssi;        /**< the router RSSI of the voted address */
    uint8_t vote_ttl;        /**< vote ttl, indicate the voting is from myself or from other nodes */
    uint16_t votes;          /**< the number of all voting nodes */
    uint16_t my_votes;       /**< the number of nodes that voted for me */
    uint8_t reason;          /**< the reason why the voting happens, root initiated or child initiated, refer to mesh_vote_reason_t */
    uint8_t child[6];        /**< child address, not used currently */
    uint8_t toDS;            /**< state represents whether the root is able to access external IP network */
} __attribute__((packed)) mesh_assoc_t;

/**
 * @brief Mesh chain layer
 */
typedef struct {
    uint16_t layer_cap;     /**< max layer of the network */
    uint16_t layer;         /**< current layer of this node */
} mesh_chain_layer_t;

/**
 * @brief Mesh chain assoc
 */
typedef struct {
    mesh_assoc_t tree;          /**< tree top, mesh_assoc IE */
    mesh_chain_layer_t chain;   /**< chain top, mesh_assoc IE */
} __attribute__((packed)) mesh_chain_assoc_t;

/* mesh max connections */
#define MESH_MAX_CONNECTIONS (10)

/**
 * @brief Mesh power save duties
 */
typedef struct {
    uint8_t device;     /**< device power save duty*/
    uint8_t parent;     /**< parent power save duty*/
    struct {
        bool used;      /**< whether the child is joined */
        uint8_t duty;   /**< power save duty of the child */
        uint8_t mac[6]; /**< mac address of the child */
    } child[MESH_MAX_CONNECTIONS]; /**< child */
} esp_mesh_ps_duties_t;

/*******************************************************
 *                Function Definitions
 *******************************************************/
/**
 * @brief      Set mesh softAP beacon interval
 *
 * @param[in]  interval_ms  beacon interval (msecs) (100 msecs ~ 60000 msecs)
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 *    - ESP_ERR_INVALID_ARG
 */
esp_err_t esp_mesh_set_beacon_interval(int interval_ms);

/**
 * @brief      Get mesh softAP beacon interval
 *
 * @param[out] interval_ms  beacon interval (msecs)
 *
 * @return
 *    - ESP_OK
 */
esp_err_t esp_mesh_get_beacon_interval(int *interval_ms);

/**
 * @brief     Set attempts for mesh self-organized networking
 *
 * @param[in] attempts
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_set_attempts(mesh_attempts_t *attempts);

/**
 * @brief      Get attempts for mesh self-organized networking
 *
 * @param[out] attempts
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 */
esp_err_t esp_mesh_get_attempts(mesh_attempts_t *attempts);

/**
 * @brief      Set parameters for parent switch
 *
 * @param[in]  paras  parameters for parent switch
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 */
esp_err_t esp_mesh_set_switch_parent_paras(mesh_switch_parent_t *paras);

/**
 * @brief      Get parameters for parent switch
 *
 * @param[out] paras  parameters for parent switch
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 */
esp_err_t esp_mesh_get_switch_parent_paras(mesh_switch_parent_t *paras);

/**
 * @brief      Set RSSI threshold of current parent
 *             - The default high RSSI threshold value is -78 dBm.
 *             - The default medium RSSI threshold value is -82 dBm.
 *             - The default low RSSI threshold value is -85 dBm.
 *
 * @param[in]  threshold  RSSI threshold
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 */
esp_err_t esp_mesh_set_rssi_threshold(const mesh_rssi_threshold_t *threshold);

/**
 * @brief      Get RSSI threshold of current parent
 *
 * @param[out] threshold  RSSI threshold
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 */
esp_err_t esp_mesh_get_rssi_threshold(mesh_rssi_threshold_t *threshold);

/**
 * @brief      Enable the minimum rate to 6 Mbps
 *
 * @attention  This API shall be called before Wi-Fi is started.
 *
 * @param[in]  is_6m  enable or not
 *
 * @return
 *    - ESP_OK
 */
esp_err_t esp_mesh_set_6m_rate(bool is_6m);

/**
 * @brief      Print the number of txQ waiting
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_print_txQ_waiting(void);

/**
 * @brief      Print the number of rxQ waiting
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_print_rxQ_waiting(void);

/**
 * @brief      Set passive scan time
 *
 * @param[in]  time_ms  passive scan time (msecs)
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 *    - ESP_ERR_ARGUMENT
 */
esp_err_t esp_mesh_set_passive_scan_time(int time_ms);

/**
 * @brief      Get passive scan time
 *
 * @return     interval_ms  passive scan time (msecs)
 */
int esp_mesh_get_passive_scan_time(void);

/**
 * @brief      Set announce interval
 *             - The default short interval is 500 milliseconds.
 *             - The default long interval is 3000 milliseconds.
 *
 * @param[in]  short_ms  shall be greater than the default value
 * @param[in]  long_ms  shall be greater than the default value
 *
 * @return
 *    - ESP_OK
 */
esp_err_t esp_mesh_set_announce_interval(int short_ms, int long_ms);

/**
 * @brief      Get announce interval
 *
 * @param[out] short_ms  short interval
 * @param[out] long_ms  long interval
 *
 * @return
 *    - ESP_OK
 */
esp_err_t esp_mesh_get_announce_interval(int *short_ms, int *long_ms);

/**
 * @brief      Get the running duties of device, parent and children
 *
 * @param[out] ps_duties ps duties
 *
 * @return
 *    - ESP_OK
 */
esp_err_t esp_mesh_ps_get_duties(esp_mesh_ps_duties_t* ps_duties);

/**
 * @brief      Enable mesh print scan result
 *
 * @param[in]  enable  enable or not
 *
 * @return
 *    - ESP_OK
 */
esp_err_t esp_mesh_print_scan_result(bool enable);
#ifdef __cplusplus
}
#endif
#endif /* __ESP_MESH_INTERNAL_H__ */
