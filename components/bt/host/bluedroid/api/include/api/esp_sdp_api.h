/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
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

#define ESP_SDP_SERVER_NAME_MAX              32  /*!< Service name max length */
#define SDP_OPP_SUPPORTED_FORMATS_MAX_LENGTH 15  /*!< OPP supported format list maximum length */

#define ESP_SDP_UUID_MAP_MAS  0x1132 /*!< Message Access Service UUID */
#define ESP_SDP_UUID_MAP_MNS  0x1133 /*!< Message Notification Service UUID */
#define ESP_SDP_UUID_PBAP_PSE 0x112F /*!< Phone Book Server Equipment UUID */
#define ESP_SDP_UUID_PBAP_PCE 0x112E /*!< Phone Book Client Equipment UUID */
#define ESP_SDP_UUID_OPP      0x1105 /*!< Object Push Profile UUID */
#define ESP_SDP_UUID_SAP      0x112D /*!< SIM Access Profile UUID */
#define ESP_SDP_UUID_DIP      0x1200 /*!< Device Identification Profile UUID */

#define ESP_SDP_BUILD_BT_UUID16(uuid16_val)                                                                                \
    (esp_bt_uuid_t) { .len = ESP_UUID_LEN_16, .uuid = {.uuid16 = (uint16_t)(uuid16_val),}, }

typedef enum {
    ESP_SDP_SUCCESS = 0,      /*!< Successful operation. */
    ESP_SDP_FAILURE,          /*!< Generic failure. */
    ESP_SDP_NO_RESOURCE,      /*!< No more resource */
    ESP_SDP_NEED_INIT,        /*!< SDP module shall init first */
    ESP_SDP_NEED_DEINIT,      /*!< SDP module shall deinit first */
    ESP_SDP_NO_CREATE_RECORD, /*!< No record created */
} esp_sdp_status_t;

/**
 * @brief SDP protocol status parameters
 */
typedef struct {
    bool sdp_inited;                   /*!< SDP initialization */
    uint8_t records_num;               /*!< Number of created records */
} esp_sdp_protocol_status_t;

/**
 * @brief SDP callback function events
 */
typedef enum {
    ESP_SDP_INIT_EVT               = 0, /*!< When SDP is initialized, the event comes */
    ESP_SDP_DEINIT_EVT             = 1, /*!< When SDP is de-initialized, the event comes */
    ESP_SDP_SEARCH_COMP_EVT        = 2, /*!< When SDP search complete, the event comes */
    ESP_SDP_CREATE_RECORD_COMP_EVT = 3, /*!< When create SDP records complete, the event comes */
    ESP_SDP_REMOVE_RECORD_COMP_EVT = 4, /*!< When remove a SDP record complete, the event comes */
} esp_sdp_cb_event_t;

/**
 * @brief SDP record type
 */
typedef enum {
    ESP_SDP_TYPE_RAW,        /*!< Used to carry raw SDP search data for unknown UUIDs */
    ESP_SDP_TYPE_MAP_MAS,    /*!< Message Access Profile - Server */
    ESP_SDP_TYPE_MAP_MNS,    /*!< Message Access Profile - Client (Notification Server) */
    ESP_SDP_TYPE_PBAP_PSE,   /*!< Phone Book Profile - Server */
    ESP_SDP_TYPE_PBAP_PCE,   /*!< Phone Book Profile - Client */
    ESP_SDP_TYPE_OPP_SERVER, /*!< Object Push Profile */
    ESP_SDP_TYPE_SAP_SERVER, /*!< SIM Access Profile */
    ESP_SDP_TYPE_DIP_SERVER, /*!< Device Identification Profile */
} esp_bluetooth_sdp_types_t;

/**
 * @brief SDP header structure
 */
typedef struct bluetooth_sdp_hdr_overlay {
    esp_bluetooth_sdp_types_t type;                 /*!< SDP type */
    esp_bt_uuid_t uuid;                             /*!< UUID type, include uuid and uuid length, only needed to be set for RAW record creation */
    uint32_t service_name_length;                   /*!< Service name length */
    char *service_name;                             /*!< Service name */
    int32_t rfcomm_channel_number;                  /*!< RFCOMM channel number, if not used set to -1*/
    int32_t l2cap_psm;                              /*!< L2CAP psm, if not used set to -1 */
    int32_t profile_version;                        /*!< Profile version */
    int user1_ptr_len;                              /*!< User data1 length, only used for searching RAW record */
    uint8_t *user1_ptr;                             /*!< User data1 pointer to the raw SDP response data, only used for searching RAW record */
    int user2_ptr_len __attribute__((deprecated));  /*!< User data2 length, only used for searching RAW record */
    uint8_t *user2_ptr __attribute__((deprecated)); /*!< User data2 pointer, only used for searching RAW record */
} esp_bluetooth_sdp_hdr_overlay_t;

/**
 * @brief Raw SDP record
 */
typedef struct bluetooth_sdp_raw_record {
    esp_bluetooth_sdp_hdr_overlay_t hdr; /*!< General info */
} esp_bluetooth_sdp_raw_record_t;

/**
 * @brief Message Access Profile - Server parameters
 */
typedef struct bluetooth_sdp_mas_record {
    esp_bluetooth_sdp_hdr_overlay_t hdr; /*!< General info */
    uint32_t mas_instance_id;            /*!< MAS Instance ID */
    uint32_t supported_features;         /*!< Map supported features */
    uint32_t supported_message_types;    /*!< Supported message types */
} esp_bluetooth_sdp_mas_record_t;

/**
 * @brief Message Access Profile - Client (Notification Server) parameters
 */
typedef struct bluetooth_sdp_mns_record {
    esp_bluetooth_sdp_hdr_overlay_t hdr; /*!< General info */
    uint32_t supported_features;         /*!< Supported features */
} esp_bluetooth_sdp_mns_record_t;

/**
 * @brief Phone Book Profile - Server parameters
 */
typedef struct bluetooth_sdp_pse_record {
    esp_bluetooth_sdp_hdr_overlay_t hdr; /*!< General info */
    uint32_t supported_features;         /*!< PBAP Supported Features */
    uint32_t supported_repositories;     /*!< Supported Repositories */
} esp_bluetooth_sdp_pse_record_t;

/**
 * @brief Phone Book Profile - Client parameters
 */
typedef struct bluetooth_sdp_pce_record {
    esp_bluetooth_sdp_hdr_overlay_t hdr; /*!< General info */
} esp_bluetooth_sdp_pce_record_t;

/**
 * @brief Object Push Profile parameters
 */
typedef struct bluetooth_sdp_ops_record {
    esp_bluetooth_sdp_hdr_overlay_t hdr;                                  /*!< General info */
    int supported_formats_list_len;                                       /*!< Supported formats list length */
    uint8_t supported_formats_list[SDP_OPP_SUPPORTED_FORMATS_MAX_LENGTH]; /*!< Supported formats list */
} esp_bluetooth_sdp_ops_record_t;

/**
 * @brief SIM Access Profile parameters
 */
typedef struct bluetooth_sdp_sap_record {
    esp_bluetooth_sdp_hdr_overlay_t hdr; /*!< General info */
} esp_bluetooth_sdp_sap_record_t;

/**
 * @brief Vendor ID source
 */
typedef enum {
    ESP_SDP_VENDOR_ID_SRC_BT  = 1, /*!< Bluetooth assigned vendor id source */
    ESP_SDP_VENDOR_ID_SRC_USB = 2, /*!< USB assigned vendor id source */
} esp_sdp_vendor_id_source_t;

/**
 * @brief Device Identification Profile parameters
 *
 * @note Only one primary Device Identification service record can be added in the SDP database. If primary
 * Device Identification service is created multiple times, only the last one will take effect.
 */
typedef struct bluetooth_sdp_dip_record {
    esp_bluetooth_sdp_hdr_overlay_t hdr; /*!< General info */
    uint16_t vendor;                     /*!< Vendor ID */
    uint16_t vendor_id_source;           /*!< Vendor ID source, 0x0001 for Bluetooth, 0x0002 for USB, other values reserved, see `esp_sdp_vendor_id_source_t` */
    uint16_t product;                    /*!< Product ID */
    uint16_t version;                    /*!< Release version in format 0xJJMN(JJ – major number, M – minor number, N – sub-minor number) */
    bool primary_record;                 /*!< Indicate if the record is primary, shall set to true if there is a only single device
                                            record, others shall be set to false */
} esp_bluetooth_sdp_dip_record_t;

/**
 * @brief SDP record parameters union
 */
typedef union {
    esp_bluetooth_sdp_hdr_overlay_t hdr; /*!< General info */
    esp_bluetooth_sdp_raw_record_t raw;  /*!< Raw SDP search data for unknown UUIDs */
    esp_bluetooth_sdp_mas_record_t mas;  /*!< Message Access Profile - Server */
    esp_bluetooth_sdp_mns_record_t mns;  /*!< Message Access Profile - Client (Notification Server) */
    esp_bluetooth_sdp_pse_record_t pse;  /*!< Phone Book Profile - Server */
    esp_bluetooth_sdp_pce_record_t pce;  /*!< Phone Book Profile - Client */
    esp_bluetooth_sdp_ops_record_t ops;  /*!< Object Push Profile */
    esp_bluetooth_sdp_sap_record_t sap;  /*!< SIM Access Profile */
    esp_bluetooth_sdp_dip_record_t dip;  /*!< Device Identification Profile */
} esp_bluetooth_sdp_record_t;

/**
 * @brief SDP callback parameters union
 */
typedef union {
    /**
     * @brief ESP_SDP_INIT_EVT
     */
    struct sdp_init_evt_param {
        esp_sdp_status_t status; /*!< Status */
    } init;                      /*!< SDP callback param of ESP_SDP_INIT_EVT */

    /**
     * @brief ESP_SDP_DEINIT_EVT
     */
    struct sdp_deinit_evt_param {
        esp_sdp_status_t status; /*!< Status */
    } deinit;                    /*!< SDP callback param of ESP_SDP_DEINIT_EVT */

    /**
     * @brief ESP_SDP_SEARCH_COMP_EVT
     */
    struct sdp_search_evt_param {
        esp_sdp_status_t status;             /*!< Status */
        esp_bd_addr_t remote_addr;           /*!< Remote device address */
        esp_bt_uuid_t sdp_uuid;              /*!< Service uuid */
        int record_count;                    /*!< Number of SDP records */
        esp_bluetooth_sdp_record_t *records; /*!< SDP records */
    } search;                                /*!< SDP callback param of ESP_SDP_SEARCH_COMP_EVT */

    /**
     * @brief ESP_SDP_CREATE_RECORD_COMP_EVT
     */
    struct sdp_create_record_evt_param {
        esp_sdp_status_t status; /*!< Status */
        int record_handle;       /*!< SDP record handle */
    } create_record;             /*!< SDP callback param of ESP_SDP_CREATE_RECORD_COMP_EVT */

    /**
     * @brief ESP_SDP_REMOVE_RECORD_COMP_EVT
     */
    struct sdp_remove_record_evt_param {
        esp_sdp_status_t status; /*!< Status */
    } remove_record;             /*!< SDP callback param of ESP_SDP_REMOVE_RECORD_COMP_EVT */

} esp_sdp_cb_param_t;

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

/**
 * @brief       This function is used to get the status of SDP
 *
 * @param[out]  status - sdp status
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_sdp_get_protocol_status(esp_sdp_protocol_status_t *status);

#ifdef __cplusplus
}
#endif

#endif ///__ESP_SDP_API_H__
