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

#ifndef __ESP_GATTC_API_H__
#define __ESP_GATTC_API_H__

#include "bt_types.h"
#include "esp_bt_defs.h"
#include "esp_gatt_defs.h"
#include "esp_err.h"

/// GATT Client callback function events
typedef enum {
    ESP_GATTC_REG_EVT = 0,                /*!> when GATT client is registered */
    ESP_GATTC_UNREG_EVT,                  /*!> when GATT client is unregistered */
    ESP_GATTC_OPEN_EVT,                   /*!> when GATT connection is set up */
    ESP_GATTC_READ_CHAR_EVT,              /*!> when GATT characteristic is read */
    ESP_GATTC_WRITE_CHAR_EVT,             /*!> when GATT characteristic write operation completes */
    ESP_GATTC_CLOSE_EVT,                  /*!> when GATT connection is closed */
    ESP_GATTC_SEARCH_CMPL_EVT,            /*!> when GATT service discovery is completed */
    ESP_GATTC_SEARCH_RES_EVT,             /*!> when GATT service discovery result is got */
    ESP_GATTC_READ_DESCR_EVT,             /*!> when GATT characteristic descriptor read completes */
    ESP_GATTC_WRITE_DESCR_EVT,            /*!> when GATT characteristic descriptor write completes */
    ESP_GATTC_NOTIFY_EVT,                 /*!> when GATT notification or indication arrives */
    ESP_GATTC_PREP_WRITE_EVT,             /*!> when GATT prepare-write operation conpletes */
    ESP_GATTC_EXEC_EVT,                   /*!> when write execution completes */
    ESP_GATTC_ACL_EVT,                    /*!> when ACL connection is up */
    ESP_GATTC_CANCEL_OPEN_EVT,            /*!> when GATT client ongoing connection is cancelled */
    ESP_GATTC_SRVC_CHG_EVT,               /*!> when "service changed" occurs */
    ESP_GATTC_ENC_CMPL_CB_EVT,            /*!> when encryption procedure completes */
    ESP_GATTC_CFG_MTU_EVT,                /*!> when configuration of MTU completes */
    ESP_GATTC_ADV_DATA_EVT,               /*!> when advertising of data */
    ESP_GATTC_MULT_ADV_ENB_EVT,           /*!> when multi-advertising is enabled */
    ESP_GATTC_MULT_ADV_UPD_EVT,           /*!> when multi-advertising parameters are updated */
    ESP_GATTC_MULT_ADV_DATA_EVT,          /*!> when multi-advertising data arrives */
    ESP_GATTC_MULT_ADV_DIS_EVT,           /*!> when multi-advertising is disabled */
    ESP_GATTC_CONGEST_EVT,                /*!> when GATT connection congestion comes */
    ESP_GATTC_BTH_SCAN_ENB_EVT,           /*!> when batch scan is enabled */
    ESP_GATTC_BTH_SCAN_CFG_EVT,           /*!> when batch scan storage is configured */
    ESP_GATTC_BTH_SCAN_RD_EVT,            /*!> when Batch scan read event is reported */
    ESP_GATTC_BTH_SCAN_THR_EVT,           /*!> when Batch scan threshold is set */
    ESP_GATTC_BTH_SCAN_PARAM_EVT,         /*!> when Batch scan parameters are set */
    ESP_GATTC_BTH_SCAN_DIS_EVT,           /*!> when Batch scan is disabled */
    ESP_GATTC_SCAN_FLT_CFG_EVT,           /*!> when Scan filter configuration completes */
    ESP_GATTC_SCAN_FLT_PARAM_EVT,         /*!> when Scan filter parameters are set */
    ESP_GATTC_SCAN_FLT_STATUS_EVT,        /*!> when Scan filter status is reported */
    ESP_GATTC_ADV_VSC_EVT,                /*!> when ADV VSC event is reported*/
    ESP_GATTC_GET_CHAR_EVT,               /*!> when characteristic is got from GATT server */
    ESP_GATTC_GET_DESCR_EVT,              /*!> when charcteristic descriptor is got from GATT server */
    ESP_GATTC_GET_INCL_SRVC_EVT,          /*!> when included service is got from GATT server */
    ESP_GATTC_REG_FOR_NOTIFY_EVT,         /*!> when registration for notification of a service completes */
    ESP_GATTC_UNREG_FOR_NOTIFY_EVT        /*!> when unregistration for notification of a service completes */
} esp_gattc_cb_event_t;

/// Maximum Transmission Unit used in GATT
#define ESP_GATT_DEF_BLE_MTU_SIZE   23

/// Maximum Transmission Unit allowed in GATT
#define ESP_GATT_MAX_MTU_SIZE       517

/**
 * @brief Gatt client callback parameters union
 */
typedef union {
    /**
	 * @brief ESP_GATTC_REG_EVT
	 */
    struct gattc_reg_evt_param {		
        esp_gatt_status_t status;		/*!< Operation status */
        esp_gatt_if_t gatt_if;			/*!< Gatt interface id, different application on gatt client different gatt_if */
        uint16_t app_id;				/*!< Application id which input in register API */
    } reg;								/*!< Gatt client callback param of ESP_GATTC_REG_EVT */

    /**
     * @brief ESP_GATTC_OPEN_EVT
     */
    struct gattc_open_evt_param {
        esp_gatt_status_t status;		/*!< Operation status */
        uint16_t conn_id;				/*!< Connection id */
        esp_gatt_if_t gatt_if;			/*!< Gatt interface id, different application on gatt client different gatt_if */
        esp_bd_addr_t remote_bda;		/*!< Remote bluetooth device address */
        uint16_t mtu;					/*!< MTU size */
    } open;								/*!< Gatt client callback param of ESP_GATTC_OPEN_EVT */

    /**
     * @brief ESP_GATTC_CLOSE_EVT
     */
    struct gattc_close_evt_param {
        esp_gatt_status_t status;		/*!< Operation status */
        uint16_t conn_id;				/*!< Connection id */
        esp_gatt_if_t gatt_if;			/*!< Gatt interface id, different application on gatt client different gatt_if */
        esp_bd_addr_t remote_bda;		/*!< Remote bluetooth device address */
        esp_gatt_conn_reason_t reason;	/*!< The reason of gatt connection close */
    } close;							/*!< Gatt client callback param of ESP_GATTC_CLOSE_EVT */

    /**
     * @brief ESP_GATTC_CFG_MTU_EVT
     */
    struct gattc_cfg_mtu_evt_param {
        esp_gatt_status_t status;		/*!< Operation status */
        uint16_t conn_id;				/*!< Connection id */
        uint16_t mtu;					/*!< MTU size */
    } cfg_mtu;							/*!< Gatt client callback param of ESP_GATTC_CFG_MTU_EVT */

    /**
     * @brief ESP_GATTC_SEARCH_CMPL_EVT
     */
    struct gattc_search_cmpl_evt_param {
        esp_gatt_status_t status;		/*!< Operation status */
        uint16_t conn_id;				/*!< Connection id */
    } search_cmpl;						/*!< Gatt client callback param of ESP_GATTC_SEARCH_CMPL_EVT */

    /**
     * @brief ESP_GATTC_SEARCH_RES_EVT
     */
    struct gattc_search_res_evt_param {
        uint16_t conn_id;				/*!< Connection id */
        esp_gatt_srvc_id_t srvc_id;	/*!< Service id, include service uuid and other information */
    } search_res;						/*!< Gatt client callback param of ESP_GATTC_SEARCH_RES_EVT */

    /**
     * @brief ESP_GATTC_READ_CHAR_EVT,  ESP_GATTC_READ_DESCR_EVT
     */
    struct gattc_read_char_evt_param {
        esp_gatt_status_t status;		/*!< Operation status */
        uint16_t conn_id;				/*!< Connection id */
        esp_gatt_srvc_id_t srvc_id; 	/*!< Service id, include service uuid and other information */
        esp_gatt_id_t char_id;			/*!< Characteristic id, include characteristic uuid and other information */
        esp_gatt_id_t descr_id;			/*!< Descriptor id, include descriptor uuid and other information */
        uint8_t *value;					/*!< Characteristic value */
        uint16_t value_type;			/*!< Characteristic value type */
        uint16_t value_len;				/*!< Characteristic value length */
    } read; 							/*!< Gatt client callback param of ESP_GATTC_READ_CHAR_EVT */

    /**
     * @brief ESP_GATTC_WRITE_CHAR_EVT, ESP_GATTC_PREP_WRITE_EVT, ESP_GATTC_WRITE_DESCR_EVT
     */
    struct gattc_write_evt_param {
        esp_gatt_status_t status;		/*!< Operation status */
        uint16_t conn_id;				/*!< Connection id */
        esp_gatt_srvc_id_t srvc_id; 	/*!< Service id, include service uuid and other information */
        esp_gatt_id_t char_id;			/*!< Characteristic id, include characteristic uuid and other information */
        esp_gatt_id_t descr_id;			/*!< Descriptor id, include descriptor uuid and other information */
    } write;							/*!< Gatt client callback param of ESP_GATTC_WRITE_DESCR_EVT */

    /**
     * @brief ESP_GATTC_EXEC_EVT
     */
    struct gattc_exec_cmpl_evt_param {
        esp_gatt_status_t status;		/*!< Operation status */
        uint16_t conn_id;				/*!< Connection id */
    } exec_cmpl;						/*!< Gatt client callback param of ESP_GATTC_EXEC_EVT */

    /**
     * @brief ESP_GATTC_NOTIFY_EVT
     */
    struct gattc_notify_evt_param {
        uint16_t conn_id;				/*!< Connection id */
        esp_bd_addr_t remote_bda;		/*!< Remote bluetooth device address */
        esp_gatt_srvc_id_t srvc_id; 	/*!< Service id, include service uuid and other information */
        esp_gatt_id_t char_id;			/*!< Characteristic id, include characteristic uuid and other information */
        esp_gatt_id_t descr_id;			/*!< Descriptor id, include descriptor uuid and other information */
        uint16_t value_len;				/*!< Notify attribute value */
        uint8_t *value;					/*!< Notify attribute value */
        bool is_notify;					/*!< True means notify, false means indicate */
    } notify;							/*!< Gatt client callback param of ESP_GATTC_NOTIFY_EVT */

    /**
     * @brief ESP_GATTC_SRVC_CHG_EVT
	 */
    struct gattc_srvc_chg_evt_param {
        esp_bd_addr_t remote_bda;		/*!< Remote bluetooth device address */
    } srvc_chg;							/*!< Gatt client callback param of ESP_GATTC_SRVC_CHG_EVT */

    /**
     * @brief ESP_GATTC_CONGEST_EVT
     */
    struct gattc_congest_evt_param {
        uint16_t conn_id;				/*!< Connection id */
        bool congested;					/*!< Congested or not */
    } congest;							/*!< Gatt client callback param of ESP_GATTC_CONGEST_EVT */

    /**
     * @brief ESP_GATTC_GET_CHAR_EVT
     */
    struct gattc_get_char_evt_param {
        esp_gatt_status_t status;		/*!< Operation status */
        uint16_t conn_id;				/*!< Connection id */
        esp_gatt_srvc_id_t srvc_id; 	/*!< Service id, include service uuid and other information */
        esp_gatt_id_t char_id;			/*!< Characteristic id, include characteristic uuid and other information */
        esp_gatt_char_prop_t char_prop; /*!< Characteristic property */
    } get_char;							/*!< Gatt client callback param of ESP_GATTC_GET_CHAR_EVT */

    /**
     * @brief ESP_GATTC_GET_DESCR_EVT
     */
    struct gattc_get_descr_evt_param {
        esp_gatt_status_t status;		/*!< Operation status */
        uint16_t conn_id;				/*!< Connection id */
        esp_gatt_srvc_id_t srvc_id; 	/*!< Service id, include service uuid and other information */
        esp_gatt_id_t char_id;			/*!< Characteristic id, include characteristic uuid and other information */
        esp_gatt_id_t descr_id;			/*!< Descriptor id, include descriptor uuid and other information */
    } get_descr;						/*!< Gatt client callback param of ESP_GATTC_GET_DESCR_EVT */

    /**
     * @brief ESP_GATTC_GET_INCL_SRVC_EVT
     */
    struct gattc_get_incl_srvc_evt_param {
        esp_gatt_status_t status;		/*!< Operation status */
        uint16_t conn_id;				/*!< Connection id */
        esp_gatt_srvc_id_t srvc_id; 	/*!< Service id, include service uuid and other information */
        esp_gatt_srvc_id_t incl_srvc_id;/*!< Included service id, include service uuid and other information */
    } get_incl_srvc;					/*!< Gatt client callback param of ESP_GATTC_GET_INCL_SRVC_EVT */

    /**
     * @brief ESP_GATTC_REG_FOR_NOTIFY_EVT
     */
    struct gattc_reg_for_notify_evt_param {
        esp_gatt_status_t status;		/*!< Operation status */
        esp_gatt_srvc_id_t srvc_id; 	/*!< Service id, include service uuid and other information */
        esp_gatt_id_t char_id;			/*!< Characteristic id, include characteristic uuid and other information */
    } reg_for_notify;					/*!< Gatt client callback param of ESP_GATTC_REG_FOR_NOTIFY_EVT */

	/**
     * @brief ESP_GATTC_UNREG_FOR_NOTIFY_EVT
     */
    struct gattc_unreg_for_notify_evt_param {
        esp_gatt_status_t status;		/*!< Operation status */
        esp_gatt_srvc_id_t srvc_id; 	/*!< Service id, include service uuid and other information */
        esp_gatt_id_t char_id;			/*!< Characteristic id, include characteristic uuid and other information */
    } unreg_for_notify;					/*!< Gatt client callback param of ESP_GATTC_UNREG_FOR_NOTIFY_EVT */


} esp_ble_gattc_cb_param_t;				/*!< GATT client callback parameter union type */


/**
 *
 * @function        esp_ble_gattc_app_register_callback
 *
 * @brief           This function is called to register application callbacks
 *                  with GATTC module.
 *
 * @param[in]       callback : pointer to the application callback function.
 *
 * @return          - ESP_OK: success
 *                  - other: failed
 *
 */
esp_err_t esp_ble_gattc_register_callback(esp_profile_cb_t callback);


/**
 *
 * @function        esp_ble_gattc_app_register
 *
 * @brief           This function is called to register application callbacks
 *                  with GATTC module.
 *
 * @param[in]       app_id : Application Identify (UUID), for different application
 *
 * @return          - ESP_OK: success
 *                  - other: failed
 *
 */
esp_err_t esp_ble_gattc_app_register(uint16_t app_id);


/**
 *
 * @function        esp_ble_gattc_app_unregister
 *
 * @brief           This function is called to unregister an application
 *                  from GATTC module.
 *
 * @param[in]       gatt_if : app identifier.
 *
 * @return          - ESP_OK: success
 *                  - other: failed
 *
 */
esp_err_t esp_ble_gattc_app_unregister(esp_gatt_if_t gatt_if);


/**
 *
 * @function        esp_ble_gattc_conn
 *
 * @brief           Open a direct connection or add a background auto connection
 *
 * @param[in]       gatt_if: application identity.
 * @param[in]       remote_bda: remote device bluetooth device address.
 * @param[in]       is_direct: direct connection or background auto connection
 *
 * @return          - ESP_OK: success
 *                  - other: failed
 *
 */
esp_err_t esp_ble_gattc_open(esp_gatt_if_t gatt_if, esp_bd_addr_t remote_bda, bool is_direct);


/**
 *
 * @function        esp_ble_gattc_close
 *
 * @brief           Close a connection to a GATT server.
 *
 * @param[in]       conn_id: connection ID to be closed.
 *
 * @return          - ESP_OK: success
 *                  - other: failed
 *
 */
esp_err_t esp_ble_gattc_close (uint16_t conn_id);


/**
 *
 * @function        esp_ble_gattc_config_mtu
 *
 * @brief           Configure the MTU size in the GATT channel. This can be done
 *                  only once per connection.
 *
 * @param[in]       conn_id: connection ID.
 *                  mtu: desired MTU size to use.
 *
 * @return          - ESP_OK: success
 *                  - other: failed
 *
 */
esp_err_t esp_ble_gattc_config_mtu (uint16_t conn_id, uint16_t mtu);


/**
 *
 * @function        esp_ble_gattc_search_service
 *
 * @brief           This function is called to request a GATT service discovery
 *                  on a GATT server. This function report service search result
 *                  by a callback event, and followed by a service search complete
 *                  event.
 *
 * @param[in]       conn_id: connection ID.
 * @param[in]       filter_uuid: a UUID of the service application is interested in.
 *                  If Null, discover for all services.
 *
 * @return          - ESP_OK: success
 *                  - other: failed
 *
 */
esp_err_t esp_ble_gattc_search_service(uint16_t conn_id, esp_bt_uuid_t *filter_uuid);


/**
 *
 * @function        esp_ble_gattc_get_characteristic
 *
 * @brief           This function is called to find the first characteristic of the
 *                  service on the given server.
 *
 * @param[in]       conn_id: connection ID which identify the server.
 *
 * @param[in]       srvc_id: service ID
 *
 * @param[in]       start_char_id:  the start characteristic ID
 *
 * @return          - ESP_OK: success
 *                  - other: failed
 *
 */
esp_err_t esp_ble_gattc_get_characteristic(uint16_t conn_id,
        esp_gatt_srvc_id_t *srvc_id, esp_gatt_id_t *start_char_id);


/**
 *
 * @function        esp_ble_gattc_get_descriptor
 *
 * @brief           This function is called to find the descriptor of the
 *                  service on the given server.
 *
 * @param[in]       conn_id: connection ID which identify the server.
 * @param[in]       srvc_id: the service ID of which the characteristic is belonged to.
 * @param[in]       char_id: Characteristic ID, if NULL find the first available
 *                  characteristic.
 * @param[in]       start_descr_id:  the start descriptor id
 *
 * @return          - ESP_OK: success
 *                  - other: failed
 *
 */
esp_err_t esp_ble_gattc_get_descriptor(uint16_t conn_id,
                                       esp_gatt_srvc_id_t *srvc_id, esp_gatt_id_t *char_id,
                                       esp_gatt_id_t *start_descr_id);


/**
 *
 * @function        esp_ble_gattc_get_include_service
 *
 * @brief           This function is called to find the first characteristic of the
 *                  service on the given server.
 *
 * @param[in]       conn_id: connection ID which identify the server.
 * @param[in]       srvc_id: the service ID of which the characteristic is belonged to.
 * @param[in]       start_incl_srvc_id: the start include service id
 *
 * @return          - ESP_OK: success
 *                  - other: failed
 *
 */
esp_err_t esp_ble_gattc_get_included_service(uint16_t conn_id,
        esp_gatt_srvc_id_t *srvc_id, esp_gatt_srvc_id_t *start_incl_srvc_id);


/**
 *
 * @function        esp_ble_gattc_read_char
 *
 * @brief           This function is called to read a service's characteristics of
 *                  the given characteriistic ID
 *
 * @param[in]       conn_id : connection ID.
 * @param[in]       srvc_id : service ID.
 * @param[in]       char_id : characteristic ID to read.
 * @param[in]       auth_req : authenticate request type
 *
 * @return          - ESP_OK: success
 *                  - other: failed
 *
 */
esp_err_t esp_ble_gattc_read_char (uint16_t conn_id,
                                   esp_gatt_srvc_id_t *srvc_id,
                                   esp_gatt_id_t *char_id,
                                   esp_gatt_auth_req_t auth_req);


/**
 *
 * @function        esp_ble_gattc_read_char_descr
 *
 * @brief           This function is called to read a characteristics descriptor.
 *
 * @param[in]       conn_id : connection ID.
 * @param[in]       srvc_id : service ID.
 * @param[in]       descr_id : characteristic descriptor ID to read.
 * @param[in]       auth_req : authenticate request type
 *
 * @return          - ESP_OK: success
 *                  - other: failed
 *
 */
esp_err_t esp_ble_gattc_read_char_descr (uint16_t conn_id,
        esp_gatt_srvc_id_t *srvc_id,
        esp_gatt_id_t  *char_id,
        esp_gatt_id_t  *descr_id,
        esp_gatt_auth_req_t auth_req);


/**
 *
 * @function        esp_ble_gattc_write_char
 *
 * @brief           This function is called to write characteristic value.
 *
 * @param[in]       conn_id : connection ID.
 * @param[in]       srvc_id : service ID.
 * @param[in]       char_id : characteristic ID to write.
 * @param[in]       value_len: length of the value to be written.
 * @param[in]       value : the value to be written.
 * @param[in]       write_type : the type of attribute write operation.
 * @param[in]       auth_req : authentication request.
 *
 *
 * @return          - ESP_OK: success
 *                  - other: failed
 *
 */
esp_err_t esp_ble_gattc_write_char( uint16_t conn_id,
                                    esp_gatt_srvc_id_t *srvc_id,
                                    esp_gatt_id_t *char_id,
                                    uint16_t value_len,
                                    uint8_t *value,
									esp_gatt_write_type_t write_type,
                                    esp_gatt_auth_req_t auth_req);


/**
 *
 * @function        esp_ble_gattc_write_char_descr
 *
 * @brief           This function is called to write characteristic descriptor value.
 *
 * @param[in]       conn_id : connection ID
 * @param[in]       srvc_id : service ID.
 * @param[in]       char_id : characteristic ID.
 * @param[in]       descr_id : characteristic descriptor ID to write.
 * @param[in]       value_len: length of the value to be written.
 * @param[in]       value : the value to be written.
 * @param[in]       write_type : the type of attribute write operation.
 * @param[in]       auth_req : authentication request.
 *
 *
 * @return          - ESP_OK: success
 *                  - other: failed
 *
 */
esp_err_t esp_ble_gattc_write_char_descr (uint16_t conn_id,
        esp_gatt_srvc_id_t *srvc_id,
        esp_gatt_id_t *char_id,
        esp_gatt_id_t *descr_id,
        uint16_t value_len,
        uint8_t *value,
		esp_gatt_write_type_t write_type,
        esp_gatt_auth_req_t auth_req);


/**
 *
 * @function        esp_ble_gattc_prepare_write
 *
 * @brief           This function is called to prepare write a characteristic value.
 *
 * @param[in]       conn_id : connection ID.
 * @param[in]       char_id : GATT characteristic ID of the service.
 * @param[in]       offset : offset of the write value.
 * @param[in]       value_len: length of the value to be written.
 * @param[in]       value : the value to be written.
 * @param[in]       auth_req : authentication request.
 *
 * @return          - ESP_OK: success
 *                  - other: failed
 *
 */
esp_err_t esp_ble_gattc_prepare_write(uint16_t conn_id,
                                      esp_gatt_srvc_id_t *srvc_id,
                                      esp_gatt_id_t *char_id,
                                      uint16_t offset,
                                      uint16_t value_len,
                                      uint8_t *value,
                                      esp_gatt_auth_req_t auth_req);

/**
 *
 * @function        esp_ble_gattc_execute_write
 *
 * @brief           This function is called to execute write a prepare write sequence.
 *
 * @param[in]       conn_id : connection ID.
 * @param[in]       is_execute : execute or cancel.
 *
 * @return          - ESP_OK: success
 *                  - other: failed
 *
 */
esp_err_t esp_ble_gattc_execute_write (uint16_t conn_id, bool is_execute);


/**
 *
 * @function        esp_ble_gattc_register_for_notify
 *
 * @brief           This function is called to register for notification of a service.
 *
 * @param[in]       gatt_if : gatt interface id.
 * @param[in]       bda : target GATT server.
 * @param[in]       srvc_id : pointer to GATT service ID.
 * @param[in]       char_id : pointer to GATT characteristic ID.
 *
 *
 * @return          - ESP_OK: registration succeeds
 *                  - other: failed
 *
 */
esp_gatt_status_t esp_ble_gattc_register_for_notify (esp_gatt_if_t gatt_if,
        esp_bd_addr_t server_bda,
        esp_gatt_srvc_id_t *srvc_id,
        esp_gatt_id_t *char_id);


/**
 *
 * @function        esp_ble_gattc_unregister_ntf
 *
 * @brief           This function is called to de-register for notification of a service.
 *
 * @param[in]       gatt_if : gatt interface id.
 * @param[in]       bda : target GATT server.
 * @param[in]       srvc_id : pointer to GATT service ID.
 * @param[in]       char_id : pointer to GATT characteristic ID.
 *
 *
 * @return          - ESP_OK: unregister succeeds
 *                  - other: failed
 *
 */
esp_gatt_status_t esp_ble_gattc_unregister_for_notify (esp_gatt_if_t gatt_if,
        esp_bd_addr_t server_bda,
        esp_gatt_srvc_id_t *srvc_id,
        esp_gatt_id_t *char_id);


#endif /* __ESP_GATTC_API_H__ */
