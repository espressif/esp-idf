/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_BLE_MESH_RPR_MODEL_API_H_
#define _ESP_BLE_MESH_RPR_MODEL_API_H_

#include "esp_ble_mesh_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_BLE_MESH_MODEL_OP_RPR_SCAN_CAPS_GET         ESP_BLE_MESH_MODEL_OP_2(0x80, 0x4F)
#define ESP_BLE_MESH_MODEL_OP_RPR_SCAN_CAPS_STATUS      ESP_BLE_MESH_MODEL_OP_2(0x80, 0x50)
#define ESP_BLE_MESH_MODEL_OP_RPR_SCAN_GET              ESP_BLE_MESH_MODEL_OP_2(0x80, 0x51)
#define ESP_BLE_MESH_MODEL_OP_RPR_SCAN_START            ESP_BLE_MESH_MODEL_OP_2(0x80, 0x52)
#define ESP_BLE_MESH_MODEL_OP_RPR_SCAN_STOP             ESP_BLE_MESH_MODEL_OP_2(0x80, 0x53)
#define ESP_BLE_MESH_MODEL_OP_RPR_SCAN_STATUS           ESP_BLE_MESH_MODEL_OP_2(0x80, 0x54)
#define ESP_BLE_MESH_MODEL_OP_RPR_SCAN_REPORT           ESP_BLE_MESH_MODEL_OP_2(0x80, 0x55)
#define ESP_BLE_MESH_MODEL_OP_RPR_EXT_SCAN_START        ESP_BLE_MESH_MODEL_OP_2(0x80, 0x56)
#define ESP_BLE_MESH_MODEL_OP_RPR_EXT_SCAN_REPORT       ESP_BLE_MESH_MODEL_OP_2(0x80, 0x57)
#define ESP_BLE_MESH_MODEL_OP_RPR_LINK_GET              ESP_BLE_MESH_MODEL_OP_2(0x80, 0x58)
#define ESP_BLE_MESH_MODEL_OP_RPR_LINK_OPEN             ESP_BLE_MESH_MODEL_OP_2(0x80, 0x59)
#define ESP_BLE_MESH_MODEL_OP_RPR_LINK_CLOSE            ESP_BLE_MESH_MODEL_OP_2(0x80, 0x5A)
#define ESP_BLE_MESH_MODEL_OP_RPR_LINK_STATUS           ESP_BLE_MESH_MODEL_OP_2(0x80, 0x5B)
#define ESP_BLE_MESH_MODEL_OP_RPR_LINK_REPORT           ESP_BLE_MESH_MODEL_OP_2(0x80, 0x5C)
#define ESP_BLE_MESH_MODEL_OP_RPR_PDU_SEND              ESP_BLE_MESH_MODEL_OP_2(0x80, 0x5D)
#define ESP_BLE_MESH_MODEL_OP_RPR_PDU_OUTBOUND_REPORT   ESP_BLE_MESH_MODEL_OP_2(0x80, 0x5E)
#define ESP_BLE_MESH_MODEL_OP_RPR_PDU_REPORT            ESP_BLE_MESH_MODEL_OP_2(0x80, 0x5F)

#define ESP_BLE_MESH_RPR_SRV_MAX_SCANNED_ITEMS_MIN                          0x04

#define ESP_BLE_MESH_RPR_NOT_SUPPORT_ACTIVE_SCAN                            0x00
#define ESP_BLE_MESH_RPR_SUPPORT_ACTIVE_SCAN                                0x01

#define ESP_BLE_MESH_RPR_SCAN_IDLE                                          0x00
#define ESP_BLE_MESH_RPR_SCAN_MULTIPLE_DEVICE                               0x01
#define ESP_BLE_MESH_RPR_SCAN_SINGLE_DEVICE                                 0x02

#define ESP_BLE_MESH_RPR_SCAN_NOT_IN_PROGRESS                               0x00

#define ESP_BLE_MESH_RPR_PROHIBIT_SCAN_TIMEOUT                              0x00

#define ESP_BLE_MESH_RPR_EXT_SCAN_TIMEOUT_MIN                               0x01
#define ESP_BLE_MESH_RPR_EXT_SCAN_TIMEOUT_MAX                               0x15

#define ESP_BLE_MESH_RPR_AD_TYPE_FILTER_CNT_MIN                             0x01
#define ESP_BLE_MESH_RPR_AD_TYPE_FILTER_CNT_MAX                             0x10

#define ESP_BLE_MESH_RPR_LINK_OPEN_TIMEOUT_MIN                              0x01
#define ESP_BLE_MESH_RPR_LINK_OPEN_TIMEOUT_MAX                              0x3C

/* The default value of the Timeout parameter is 10 seconds */
#define ESP_BLE_MESH_RPR_LINK_TIMEOUT_DEFAULT                               0x0A

#define ESP_BLE_MESH_RPR_REASON_SUCCESS                                     0x00
#define ESP_BLE_MESH_RPR_REASON_FAIL                                        0x02

#define ESP_BLE_MESH_RPR_LINK_IDLE                                          0x00
#define ESP_BLE_MESH_RPR_LINK_OPENING                                       0x01
#define ESP_BLE_MESH_RPR_LINK_ACTIVE                                        0x02
#define ESP_BLE_MESH_RPR_OUTBOUND_PACKET_TRANSFER                           0x03
#define ESP_BLE_MESH_RPR_LINK_CLOSING                                       0x04

#define ESP_BLE_MESH_RPR_STATUS_SUCCESS                                     0x00
#define ESP_BLE_MESH_RPR_STATUS_SCANNING_CANNOT_START                       0x01
#define ESP_BLE_MESH_RPR_STATUS_INVALID_STATE                               0x02
#define ESP_BLE_MESH_RPR_STATUS_LIMITED_RESOURCES                           0x03
#define ESP_BLE_MESH_RPR_STATUS_LINK_CANNOT_OPEN                            0x04
#define ESP_BLE_MESH_RPR_STATUS_LINK_OPEN_FAILED                            0x05
#define ESP_BLE_MESH_RPR_STATUS_LINK_CLOSED_BY_DEVICE                       0x06
#define ESP_BLE_MESH_RPR_STATUS_LINK_CLOSED_BY_SERVER                       0x07
#define ESP_BLE_MESH_RPR_STATUS_LINK_CLOSED_BY_CLIENT                       0x08
#define ESP_BLE_MESH_RPR_STATUS_LINK_CLOSED_AS_CANNOT_RECEIVE_PDU           0x09
#define ESP_BLE_MESH_RPR_STATUS_LINK_CLOSED_AS_CANNOT_SEND_PDU              0x0A
#define ESP_BLE_MESH_RPR_STATUS_LINK_CLOSED_AS_CANNOT_DELIVER_PDU_REPORT    0x0B

/** @def    ESP_BLE_MESH_MODEL_RPR_SRV
 *
 *  @brief  Define a new Remote Provisioning Server model.
 *
 *  @note   If supported, the model shall be supported by a primary element
 *          and may be supported by any secondary element.
 *
 *  @param  srv_data Pointer to a unique Remote Provisioning Server model
 *                   user_data.
 *
 *  @return New Remote Provisioning Server model instance.
 */
#define ESP_BLE_MESH_MODEL_RPR_SRV(srv_data) \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_RPR_SRV, \
                               NULL, NULL, srv_data)

/** @def    ESP_BLE_MESH_MODEL_RPR_CLI
 *
 *  @brief  Define a new Remote Provisioning Client model.
 *
 *  @note   If supported, the model shall be supported by a primary element
 *          and may be supported by any secondary element.
 *
 *  @param  cli_data Pointer to a unique Remote Provisioning Client model
 *                   user_data.
 *
 *  @return New Remote Provisioning Client model instance.
 */
#define ESP_BLE_MESH_MODEL_RPR_CLI(cli_data) \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_RPR_CLI, \
                               NULL, NULL, cli_data)

/** Remote Provisioning Server model context */
/* typedef struct {
 *     void *dummy;
 * } esp_ble_mesh_rpr_srv_t;
 */

/** Parameters of Remote Provisioning Scan Start */
typedef struct {
    uint8_t scan_items_limit;                           /*!< Maximum number of scanned items to be reported */
    uint8_t timeout;                                    /*!< Time limit for a scan (in seconds) */

    bool    uuid_en;                                    /*!< Indicate if Device UUID is present */
    uint8_t uuid[16];                                   /*!< Device UUID (Optional) */
} esp_ble_mesh_rpr_scan_start_t;

/** Parameters of Remote Provisioning Extended Scan Start */
typedef struct {
    uint8_t ad_type_filter_count;                       /*!< Number of AD Types in the ADTypeFilter field */
    uint8_t ad_type_filter[16];                         /*!< List of AD Types to be reported. Minimum is 1, maximum is 16 */

    bool    uuid_en;                                    /*!< Indicate if Device UUID is present */
    uint8_t uuid[16];                                   /*!< Device UUID (Optional) */
    uint8_t timeout;                                    /*!< Time limit for a scan (in seconds) (C.1) */
} esp_ble_mesh_rpr_ext_scan_start_t;

/** Parameters of Remote Provisioning Link Open */
typedef struct {
    bool    uuid_en;                                    /*!< Indicate if Device UUID is present */
    uint8_t uuid[16];                                   /*!< Device UUID (Optional) */

    bool    timeout_en;                                 /*!< Indicate if Link open timeout is present */
    uint8_t timeout;                                    /*!< Link open timeout in seconds (C.1) */

    uint8_t nppi;                                       /*!< Node Provisioning Protocol Interface (C.2) */
} esp_ble_mesh_rpr_link_open_t;

/** Parameters of Remote Provisioning Link Close */
typedef struct {
    uint8_t reason;                                     /*!< Provisioning bearer link close reason code */
} esp_ble_mesh_rpr_link_close_t;

/**
 * @brief Remote Provisioning Client model message union
 */
typedef union {
    esp_ble_mesh_rpr_scan_start_t     scan_start;       /*!< For ESP_BLE_MESH_MODEL_OP_RPR_SCAN_START */
    esp_ble_mesh_rpr_ext_scan_start_t ext_scan_start;   /*!< For ESP_BLE_MESH_MODEL_OP_RPR_EXT_SCAN_START */
    esp_ble_mesh_rpr_link_open_t      link_open;        /*!< For ESP_BLE_MESH_MODEL_OP_RPR_LINK_OPEN */
    esp_ble_mesh_rpr_link_close_t     link_close;       /*!< For ESP_BLE_MESH_MODEL_OP_RPR_LINK_CLOSE */
} esp_ble_mesh_rpr_client_msg_t;

/** This enum value is the action of Remote Provisioning Client model */
typedef enum {
    ESP_BLE_MESH_RPR_CLIENT_ACT_START_RPR,
    ESP_BLE_MESH_RPR_CLIENT_ACT_MAX,
} esp_ble_mesh_rpr_client_act_type_t;

/** Parameters of starting remote provisioning */
typedef struct {
    esp_ble_mesh_model_t *model;                        /*!< Pointer of Remote Provisioning Client */
    uint16_t rpr_srv_addr;                              /*!< Unicast address of Remote Provisioning Server */
} esp_ble_mesh_rpr_client_start_rpr_t;

/**
 * @brief Remote Provisioning Client model action union
 */
typedef union {
    esp_ble_mesh_rpr_client_start_rpr_t start_rpr;      /*!< Start remote provisioning */
} esp_ble_mesh_rpr_client_act_param_t;

/** Parameters of Remote Provisioning Scan Capabilities Status */
typedef struct {
    uint8_t max_scan_items;                             /*!< The maximum number of UUIDs that can be reported during scanning */
    uint8_t active_scan;                                /*!< Indication if active scan is supported */
} esp_ble_mesh_rpr_scan_caps_status_t;

/** Parameters of Remote Provisioning Scan Status */
typedef struct {
    uint8_t status;                                     /*!< Status for the requesting message */
    uint8_t rpr_scanning;                               /*!< The Remote Provisioning Scan state value */
    uint8_t scan_items_limit;                           /*!< Maximum number of scanned items to be reported */
    uint8_t timeout;                                    /*!< Time limit for a scan (in seconds) */
} esp_ble_mesh_rpr_scan_status_t;

/** Parameters of Remote Provisioning Scan Report */
typedef struct {
    int8_t   rssi;                                      /*!< An indication of received signal strength measured in dBm */
    uint8_t  uuid[16];                                  /*!< Device UUID */
    uint16_t oob_info;                                  /*!< OOB information */
    uint32_t uri_hash;                                  /*!< URI Hash (Optional) */
} esp_ble_mesh_rpr_scan_report_t;

/** Parameters of Remote Provisioning Extended Scan Report */
typedef struct {
    uint8_t  status;                                    /*!< Status for the requesting message */
    uint8_t  uuid[16];                                  /*!< Device UUID */

    bool     oob_info_en;                               /*!< Indicate if OOB Information is present */
    uint16_t oob_info;                                  /*!< OOB Information (Optional) */
    struct net_buf_simple *adv_structures;              /*!< Concatenated list of AD Structures (C.1) */
} esp_ble_mesh_rpr_ext_scan_report_t;

/** Parameters of Remote Provisioning Link Status */
typedef struct {
    uint8_t status;                                     /*!< Status for the requesting message */
    uint8_t rpr_state;                                  /*!< Remote Provisioning Link state */
} esp_ble_mesh_rpr_link_status_t;

/** Parameters of Remote Provisioning Link Report */
typedef struct {
    uint8_t status;                                     /*!< Status of the provisioning bearer or the NPPI */
    uint8_t rpr_state;                                  /*!< Remote Provisioning Link state */
    bool    reason_en;                                  /*!< Indicate if Link close Reason code is present */
    uint8_t reason;                                     /*!< Link close Reason code (Optional) */
} esp_ble_mesh_rpr_link_report_t;

/**
 * @brief Remote Provisioning Client model received message union
 */
typedef union {
    esp_ble_mesh_rpr_scan_caps_status_t scan_caps_status;   /*!< For ESP_BLE_MESH_MODEL_OP_RPR_SCAN_CAPS_STATUS */
    esp_ble_mesh_rpr_scan_status_t      scan_status;        /*!< For ESP_BLE_MESH_MODEL_OP_RPR_SCAN_STATUS */
    esp_ble_mesh_rpr_scan_report_t      scan_report;        /*!< For ESP_BLE_MESH_MODEL_OP_RPR_SCAN_REPORT */
    esp_ble_mesh_rpr_ext_scan_report_t  ext_scan_report;    /*!< For ESP_BLE_MESH_MODEL_OP_RPR_EXT_SCAN_REPORT */
    esp_ble_mesh_rpr_link_status_t      link_status;        /*!< For ESP_BLE_MESH_MODEL_OP_RPR_LINK_STATUS */
    esp_ble_mesh_rpr_link_report_t      link_report;        /*!< For ESP_BLE_MESH_MODEL_OP_RPR_LINK_REPORT */
} esp_ble_mesh_rpr_client_recv_cb_t;

/** This enum value is the event type of the performed action */
typedef enum {
    ESP_BLE_MESH_START_RPR_COMP_SUB_EVT,
} esp_ble_mesh_rpr_client_act_evt_t;

/** Remote Provisioning Client model callback parameters */
typedef union {
    /** Event parameters of sending messages */
    struct {
        int err_code;                                   /*!< Result of sending a message */
        esp_ble_mesh_client_common_param_t *params;     /*!< Client common parameters */
    } send;                                             /*!< Event parameters of sending messages */
    /** Event parameters of receiving messages */
    struct {
        esp_ble_mesh_client_common_param_t *params;     /*!< Client common parameters */
        esp_ble_mesh_rpr_client_recv_cb_t val;          /*!< Parameters of received status message */
    } recv;                                             /*!< Event parameters of receiving messages */
    /** Event parameters of performed actions */
    struct {
        esp_ble_mesh_rpr_client_act_evt_t sub_evt;      /*!< Event type of the performed action */
        /**
         * @brief ESP_BLE_MESH_START_RPR_COMP_SUB_EVT
         */
        struct {
            int err_code;                               /*!< Result of starting remote provisioning */
            esp_ble_mesh_model_t *model;                /*!< Pointer of Remote Provisioning Client */
            uint16_t rpr_srv_addr;                      /*!< Unicast address of Remote Provisioning Server */
        } start_rpr_comp;                               /*!< Event parameter of ESP_BLE_MESH_START_RPR_COMP_SUB_EVT */
    } act;                                              /*!< Event parameters of performed actions */
    /**
     * @brief ESP_BLE_MESH_RPR_CLIENT_LINK_OPEN_EVT
     */
    struct {
        esp_ble_mesh_model_t *model;                    /*!< Pointer of Remote Provisioning Client */
        uint16_t rpr_srv_addr;                          /*!< Unicast address of Remote Provisioning Server */
    } link_open;                                        /*!< Event parameters of ESP_BLE_MESH_RPR_CLIENT_LINK_OPEN_EVT */
    /**
     * @brief ESP_BLE_MESH_RPR_CLIENT_LINK_CLOSE_EVT
     */
    struct {
        esp_ble_mesh_model_t *model;                    /*!< Pointer of Remote Provisioning Client */
        uint16_t rpr_srv_addr;                          /*!< Unicast address of Remote Provisioning Server */
        uint8_t  reason;                                /*!< Reason of closing provisioning link */
    } link_close;                                       /*!< Event parameters of ESP_BLE_MESH_RPR_CLIENT_LINK_CLOSE_EVT */
    /**
     * @brief ESP_BLE_MESH_RPR_CLIENT_PROV_COMP_EVT
     */
    struct {
        esp_ble_mesh_model_t *model;                    /*!< Pointer of Remote Provisioning Client */
        uint16_t rpr_srv_addr;                          /*!< Unicast address of Remote Provisioning Server */
        uint8_t  nppi;                                  /*!< NPPI Procedure */
        uint16_t index;                                 /*!< Index of the provisioned node  */
        uint8_t  uuid[16];                              /*!< Device UUID */
        uint16_t unicast_addr;                          /*!< Primary element address */
        uint8_t  element_num;                           /*!< Element number */
        uint16_t net_idx;                               /*!< NetKey Index */
    } prov;                                             /*!< Event parameters of ESP_BLE_MESH_RPR_CLIENT_PROV_COMP_EVT */
} esp_ble_mesh_rpr_client_cb_param_t;                   /*!< Remote Provisioning Client model callback parameters */

/** This enum value is the event of Remote Provisioning Client model */
typedef enum {
    ESP_BLE_MESH_RPR_CLIENT_SEND_COMP_EVT,
    ESP_BLE_MESH_RPR_CLIENT_SEND_TIMEOUT_EVT,
    ESP_BLE_MESH_RPR_CLIENT_RECV_RSP_EVT,
    ESP_BLE_MESH_RPR_CLIENT_RECV_PUB_EVT,
    ESP_BLE_MESH_RPR_CLIENT_ACT_COMP_EVT,
    ESP_BLE_MESH_RPR_CLIENT_LINK_OPEN_EVT,
    ESP_BLE_MESH_RPR_CLIENT_LINK_CLOSE_EVT,
    ESP_BLE_MESH_RPR_CLIENT_PROV_COMP_EVT,
    ESP_BLE_MESH_RPR_CLIENT_EVT_MAX,
} esp_ble_mesh_rpr_client_cb_event_t;

/**
 * @brief Remote Provisioning Server model related context.
 */

/**
 * @brief Remote Provisioning Server model callback value union
 */
typedef union {
    /**
     * @brief ESP_BLE_MESH_RPR_SERVER_SCAN_START_EVT
     */
    struct {
        esp_ble_mesh_model_t *model;                    /*!< Pointer to the server model structure */
        uint8_t  scan_items_limit;                      /*!< Maximum number of scanned items to be reported */
        uint8_t  timeout;                               /*!< Time limit for a scan (in seconds) */
        uint8_t  uuid[16];                              /*!< Device UUID (All ZERO if not present) */
        uint16_t net_idx;                               /*!< NetKey Index used by Remote Provisioning Client */
        uint16_t rpr_cli_addr;                          /*!< Unicast address of Remote Provisioning Client */
    } scan_start;
    /**
     * @brief ESP_BLE_MESH_RPR_SERVER_SCAN_STOP_EVT
     */
    struct {
        esp_ble_mesh_model_t *model;                    /*!< Pointer to the server model structure */
        uint8_t  uuid[16];                              /*!< Device UUID (All ZERO if not present) */
        uint16_t net_idx;                               /*!< NetKey Index used by Remote Provisioning Client */
        uint16_t rpr_cli_addr;                          /*!< Unicast address of Remote Provisioning Client */
    } scan_stop;
    /**
     * @brief ESP_BLE_MESH_RPR_SERVER_EXT_SCAN_START_EVT
     */
    struct {
        esp_ble_mesh_model_t *model;                    /*!< Pointer to the server model structure */
        uint8_t  ad_type_filter_count;                  /*!< Number of AD Types in the ADTypeFilter field */
        uint8_t *ad_type_filter;                        /*!< List of AD Types to be reported */
        uint8_t  uuid[16];                              /*!< Device UUID (All ZERO if not present) */
        uint8_t  timeout;                               /*!< Time limit for a scan (in seconds) */
        uint8_t  index;                                 /*!< Index of the extended scan instance */
        uint16_t net_idx;                               /*!< NetKey Index used by Remote Provisioning Client */
        uint16_t rpr_cli_addr;                          /*!< Unicast address of Remote Provisioning Client */
    } ext_scan_start;
    /**
     * @brief ESP_BLE_MESH_RPR_SERVER_EXT_SCAN_STOP_EVT
     */
    struct {
        esp_ble_mesh_model_t *model;                    /*!< Pointer to the server model structure */
        uint8_t  uuid[16];                              /*!< Device UUID (ZERO if not present)*/
        uint8_t  timeout;                               /*!< Time limit for extended scan (in seconds) */
        uint8_t  index;                                 /*!< Index of the extended scan instance */
        uint16_t net_idx;                               /*!< NetKey Index used by Remote Provisioning Client */
        uint16_t rpr_cli_addr;                          /*!< Unicast address of Remote Provisioning Client */
    } ext_scan_stop;
    /**
     * @brief ESP_BLE_MESH_RPR_SERVER_LINK_OPEN_EVT
     */
    struct {
        esp_ble_mesh_model_t *model;                    /*!< Pointer to the server model structure */
        uint8_t  uuid[16];                              /*!< Device UUID (ZERO if not present)*/
        uint8_t  status;                                /*!< Status of Link Open procedure */
        uint8_t  timeout;                               /*!< Time limit for opening a link (in seconds) */
        uint8_t  nppi;                                  /*!< Node Provisioning Protocol Interface */
        uint16_t net_idx;                               /*!< NetKey Index used by Remote Provisioning Client */
        uint16_t rpr_cli_addr;                          /*!< Unicast address of Remote Provisioning Client */
    } link_open;
    /**
     * @brief ESP_BLE_MESH_RPR_SERVER_LINK_CLOSE_EVT
     */
    struct {
        esp_ble_mesh_model_t *model;                    /*!< Pointer to the server model structure */
        uint8_t  uuid[16];                              /*!< Device UUID (ZERO if not present)*/
        uint8_t  nppi;                                  /*!< Node Provisioning Protocol Interface */
        bool     close_by_device;                       /*!< Indicate if the link is closed by the Unprovisioned Device */
        uint8_t  reason;                                /*!< Provisioning bearer link close reason code */
        uint16_t net_idx;                               /*!< NetKey Index used by Remote Provisioning Client */
        uint16_t rpr_cli_addr;                          /*!< Unicast address of Remote Provisioning Client */
    } link_close;
    /**
     * @brief ESP_BLE_MESH_RPR_SERVER_PROV_COMP_EVT. TODO: Duplicate with Link Close event?
     */
    struct {
        esp_ble_mesh_model_t *model;                    /*!< Pointer to the server model structure */
        uint8_t  uuid[16];                              /*!< Device UUID (ZERO if not present)*/
        uint8_t  nppi;                                  /*!< Provisioning bearer link close reason code */
        uint16_t net_idx;                               /*!< NetKey Index used by Remote Provisioning Client */
        uint16_t rpr_cli_addr;                          /*!< Unicast address of Remote Provisioning Client */
    } prov_comp;
} esp_ble_mesh_rpr_server_cb_param_t;

/** This enum value is the event of Remote Provisioning Server model */
typedef enum {
    ESP_BLE_MESH_RPR_SERVER_SCAN_START_EVT,
    ESP_BLE_MESH_RPR_SERVER_SCAN_STOP_EVT,
    ESP_BLE_MESH_RPR_SERVER_EXT_SCAN_START_EVT,
    ESP_BLE_MESH_RPR_SERVER_EXT_SCAN_STOP_EVT,
    ESP_BLE_MESH_RPR_SERVER_LINK_OPEN_EVT,
    ESP_BLE_MESH_RPR_SERVER_LINK_CLOSE_EVT,
    ESP_BLE_MESH_RPR_SERVER_PROV_COMP_EVT,
    ESP_BLE_MESH_RPR_SERVER_EVT_MAX,
} esp_ble_mesh_rpr_server_cb_event_t;

/**
 *  @brief Remote Provisioning client and server model functions.
 */

/**
 * @brief   Remote Provisioning Client model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_rpr_client_cb_t)(esp_ble_mesh_rpr_client_cb_event_t event,
                                              esp_ble_mesh_rpr_client_cb_param_t *param);

/**
 * @brief       Register BLE Mesh Remote Provisioning Client model callback.
 *
 * @param[in]   callback: Pointer to the callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_rpr_client_callback(esp_ble_mesh_rpr_client_cb_t callback);

/**
 * @brief       Get the value of Remote Provisioning Server model state with the corresponding get message.
 *
 * @param[in]   params: Pointer to BLE Mesh common client parameters.
 * @param[in]   msg:    Pointer to Remote Provisioning Client message.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_rpr_client_send(esp_ble_mesh_client_common_param_t *params,
                                       esp_ble_mesh_rpr_client_msg_t *msg);

/**
 * @brief       Remote Provisioning Client model perform related actions, e.g. start remote provisioning.
 *
 * @param[in]   type:  Type of the action to be performed.
 * @param[in]   param: Parameters of the action to be performed.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_rpr_client_action(esp_ble_mesh_rpr_client_act_type_t type,
                                         esp_ble_mesh_rpr_client_act_param_t *param);

/**
 * @brief   Remote Provisioning Server model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_rpr_server_cb_t)(esp_ble_mesh_rpr_server_cb_event_t event,
                                              esp_ble_mesh_rpr_server_cb_param_t *param);

/**
 * @brief       Register BLE Mesh Remote Provisioning Server model callback.
 *
 * @param[in]   callback: Pointer to the callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_rpr_server_callback(esp_ble_mesh_rpr_server_cb_t callback);

#ifdef __cplusplus
}
#endif

#endif /* _ESP_BLE_MESH_RPR_MODEL_API_H_ */
