/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_SDP_API_H__
#define __ESP_SDP_API_H__

#include "esp_err.h"
#include "esp_bt_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_SDP_SERVER_NAME_MAX 32                 /*!< Service name max length */
#define SDP_OPP_SUPPORTED_FORMATS_MAX_LENGTH 15    /*!< OPP supported format list maximum length */

typedef enum {
    ESP_SDP_SUCCESS   = 0,          /*!< Successful operation. */
    ESP_SDP_FAILURE,                /*!< Generic failure. */
    ESP_SDP_NO_RESOURCE,            /*!< No more resource */
    ESP_SDP_NEED_INIT,              /*!< SDP module shall init first */
    ESP_SDP_NEED_DEINIT,            /*!< SDP module shall deinit first */
    ESP_SDP_NO_CREATE_RECORD,       /*!< No record created */
} esp_sdp_status_t;

/**
 * @brief SDP callback function events
 */
typedef enum {
    ESP_SDP_INIT_EVT                    = 0,                /*!< When SDP is initialized, the event comes */
    ESP_SDP_DEINIT_EVT                  = 1,                /*!< When SDP is deinitialized, the event comes */
    ESP_SDP_SEARCH_COMP_EVT             = 2,                /*!< When SDP search complete, the event comes */
    ESP_SDP_CREATE_RECORD_COMP_EVT      = 3,                /*!< When create SDP records complete, the event comes */
    ESP_SDP_REMOVE_RECORD_COMP_EVT      = 4,                /*!< When remove a SDP record complete, the event comes */
} esp_sdp_cb_event_t;

/**
 * @brief SDP record type
 */
typedef enum {
    ESP_SDP_TYPE_RAW,          /*!< Used to carry raw SDP search data for unknown UUIDs */
    ESP_SDP_TYPE_MAP_MAS,      /*!< Message Access Profile - Server */
    ESP_SDP_TYPE_MAP_MNS,      /*!< Message Access Profile - Client (Notification Server) */
    ESP_SDP_TYPE_PBAP_PSE,     /*!< Phone Book Profile - Server */
    ESP_SDP_TYPE_PBAP_PCE,     /*!< Phone Book Profile - Client */
    ESP_SDP_TYPE_OPP_SERVER,   /*!< Object Push Profile */
    ESP_SDP_TYPE_SAP_SERVER    /*!< SIM Access Profile */
} esp_bluetooth_sdp_types_t;

/**
 * @brief Some signals need additional pointers, hence we introduce a
 *        generic way to handle these pointers.
 */
typedef struct bluetooth_sdp_hdr_overlay {
    esp_bluetooth_sdp_types_t type;            /*!< SDP type */
    esp_bt_uuid_t  uuid;                       /*!< UUID type， include uuid and uuid length */
    uint32_t       service_name_length;        /*!< Service name length */
    char          *service_name;               /*!< service name */
    int32_t        rfcomm_channel_number;      /*!< rfcomm channel number, if not used set to -1*/
    int32_t        l2cap_psm;                  /*!< l2cap psm, if not used set to -1 */
    int32_t        profile_version;            /*!< profile version */

    // User pointers, only used for some signals - see esp_bluetooth_sdp_ops_record_t
    int            user1_ptr_len;              /*!< see esp_bluetooth_sdp_ops_record_t */
    uint8_t       *user1_ptr;                  /*!< see esp_bluetooth_sdp_ops_record_t */
    int            user2_ptr_len;              /*!< see esp_bluetooth_sdp_ops_record_t */
    uint8_t       *user2_ptr;                  /*!< see esp_bluetooth_sdp_ops_record_t */
} esp_bluetooth_sdp_hdr_overlay_t;

/**
 * @brief Message Access Profile - Server parameters
 */
typedef struct bluetooth_sdp_mas_record {
    esp_bluetooth_sdp_hdr_overlay_t hdr;       /*!< General info */
    uint32_t    mas_instance_id;               /*!< MAS Instance ID */
    uint32_t    supported_features;            /*!< Map supported features */
    uint32_t    supported_message_types;       /*!< Supported message types */
} esp_bluetooth_sdp_mas_record_t;

/**
 * @brief Message Access Profile - Client (Notification Server) parameters
 */
typedef struct bluetooth_sdp_mns_record {
    esp_bluetooth_sdp_hdr_overlay_t hdr;       /*!< General info */
    uint32_t    supported_features;            /*!< Supported features */
} esp_bluetooth_sdp_mns_record_t;

/**
 * @brief Phone Book Profile - Server parameters
 */
typedef struct bluetooth_sdp_pse_record {
    esp_bluetooth_sdp_hdr_overlay_t hdr;       /*!< General info */
    uint32_t    supported_features;            /*!< Pbap Supported Features */
    uint32_t    supported_repositories;        /*!< Supported Repositories */
} esp_bluetooth_sdp_pse_record_t;

/**
 * @brief Phone Book Profile - Client parameters
 */
typedef struct bluetooth_sdp_pce_record {
    esp_bluetooth_sdp_hdr_overlay_t hdr;       /*!< General info */
} esp_bluetooth_sdp_pce_record_t;

/**
 * @brief Object Push Profile parameters
 */
typedef struct bluetooth_sdp_ops_record {
    esp_bluetooth_sdp_hdr_overlay_t hdr;       /*!< General info */
    int         supported_formats_list_len;    /*!< Supported formats list length */
    uint8_t     supported_formats_list[SDP_OPP_SUPPORTED_FORMATS_MAX_LENGTH];  /*!< Supported formats list */
} esp_bluetooth_sdp_ops_record_t;

/**
 * @brief SIM Access Profile parameters
 */
typedef struct bluetooth_sdp_sap_record {
    esp_bluetooth_sdp_hdr_overlay_t hdr;       /*!< General info */
} esp_bluetooth_sdp_sap_record_t;

/**
 * @brief SDP record parameters union
 */
typedef union {
    esp_bluetooth_sdp_hdr_overlay_t   hdr;     /*!< General info */
    esp_bluetooth_sdp_mas_record_t    mas;     /*!< Message Access Profile - Server */
    esp_bluetooth_sdp_mns_record_t    mns;     /*!< Message Access Profile - Client (Notification Server) */
    esp_bluetooth_sdp_pse_record_t    pse;     /*!< Phone Book Profile - Server */
    esp_bluetooth_sdp_pce_record_t    pce;     /*!< Phone Book Profile - Client */
    esp_bluetooth_sdp_ops_record_t    ops;     /*!< Object Push Profile */
    esp_bluetooth_sdp_sap_record_t    sap;     /*!< SIM Access Profile */
} esp_bluetooth_sdp_record_t;

/**
 * @brief SDP callback parameters union
 */
typedef union {
    /**
     * @brief ESP_SDP_INIT_EVT
     */
    struct sdp_init_evt_param {
        esp_sdp_status_t    status;         /*!< status */
    } init;                                 /*!< SDP callback param of ESP_SDP_INIT_EVT */

    /**
     * @brief ESP_SDP_DEINIT_EVT
     */
    struct sdp_deinit_evt_param {
        esp_sdp_status_t    status;         /*!< status */
    } deinit;                               /*!< SDP callback param of ESP_SDP_DEINIT_EVT */

    /**
     * @brief ESP_SDP_SEARCH_COMP_EVT
     */
    struct sdp_search_evt_param {
        esp_sdp_status_t    status;         /*!< status */
        esp_bd_addr_t       remote_addr;    /*!< remote device address */
        esp_bt_uuid_t       sdp_uuid;       /*!< service uuid */
        int                 record_count;   /*!< Number of SDP records */
        esp_bluetooth_sdp_record_t *records;/*!< SDP records */
    } search;                               /*!< SDP callback param of ESP_SDP_SEARCH_COMP_EVT */

    /**
     * @brief ESP_SDP_CREATE_RECORD_COMP_EVT
     */
    struct sdp_crate_record_evt_param {
        esp_sdp_status_t    status;         /*!< status */
        int                 record_handle;  /*!< SDP record handle */
    } create_record;                        /*!< SDP callback param of ESP_SDP_CREATE_RECORD_COMP_EVT */

    /**
     * @brief ESP_SDP_REMOVE_RECORD_COMP_EVT
     */
    struct sdp_remove_record_evt_param {
        esp_sdp_status_t    status;         /*!< status */
    } remove_record;                        /*!< SDP callback param of ESP_SDP_REMOVE_RECORD_COMP_EVT */

} esp_sdp_cb_param_t;                       /*!< SDP callback parameter union type */


/**
 * @brief       SDP callback function type.
 *
 * @param       event:      Event type
 * @param       param:      Point to callback parameter, currently is union type
 */
typedef void (* esp_sdp_cb_t)(esp_sdp_cb_event_t event, esp_sdp_cb_param_t *param);

/**
 * @brief       This function is called to init callbacks with SDP module.
 *
 * @param[in]   callback:   pointer to the init callback function.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_sdp_register_callback(esp_sdp_cb_t callback);

/**
 * @brief       This function is called to init SDP module.
 *              When the operation is completed, the callback function will be called with ESP_SDP_INIT_EVT.
 *              This function should be called after esp_bluedroid_enable() completes successfully.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_sdp_init(void);

/**
 * @brief       This function is called to de-initialize SDP module.
 *              The operation will remove all SDP records, then the callback function will be called
 *              with ESP_SDP_REMOVE_RECORD_COMP_EVT, and the number of ESP_SDP_REMOVE_RECORD_COMP_EVT is
 *              equal to the number of SDP records.When the operation is completed, the callback function
 *              will be called with ESP_SDP_DEINIT_EVT. This function should be called after esp_sdp_init()
 *              completes successfully.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_sdp_deinit(void);

/**
 * @brief       This function is called to performs service discovery for the services provided by the given peer device.
 *              When the operation is completed, the callback function will be called with ESP_SDP_SEARCH_COMP_EVT.
 *              This function must be called after esp_sdp_init() successful and before esp_sdp_deinit().
 *
 * @param[in]   bd_addr:   Remote device bluetooth device address.
 * @param[in]   uuid:      Service UUID of the remote device.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_sdp_search_record(esp_bd_addr_t bd_addr, esp_bt_uuid_t uuid);

/**
 * @brief       This function is called to create SDP records.
 *              When the operation is completed, the callback function will be called with ESP_SDP_CREATE_RECORD_COMP_EVT.
 *              This function must be called after esp_sdp_init() successful and before esp_sdp_deinit().
 *
 * @param[in]   record:   The SDP record to create.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_sdp_create_record(esp_bluetooth_sdp_record_t *record);

/**
 * @brief       This function is called to remove a SDP record.
 *              When the operation is completed, the callback function will be called with ESP_SDP_REMOVE_RECORD_COMP_EVT.
 *              This function must be called after esp_sdp_init() successful and before esp_sdp_deinit().
 *
 * @param[in]   record_handle:    The SDP record handle.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_sdp_remove_record(int record_handle);

#ifdef __cplusplus
}
#endif

#endif ///__ESP_SDP_API_H__
