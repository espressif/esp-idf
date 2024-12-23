/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_GATTC_API_H__
#define __ESP_GATTC_API_H__

#include "esp_bt_defs.h"
#include "esp_gatt_defs.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief GATT Client callback function events
 */
typedef enum {
    ESP_GATTC_REG_EVT                 = 0,        /*!< This event is triggered when a GATT Client application is registered using `esp_ble_gattc_app_register`. */
    ESP_GATTC_UNREG_EVT               = 1,        /*!< This event is triggered when a GATT Client application is unregistered using `esp_ble_gattc_app_unregister`. */
    ESP_GATTC_OPEN_EVT                = 2,        /*!< This event is triggered when a GATT virtual connection is set up using `esp_ble_gattc_open`. */
    ESP_GATTC_READ_CHAR_EVT           = 3,        /*!< This event is triggered upon the completion of a GATT characteristic read operation using `esp_ble_gattc_read_char`. */
    ESP_GATTC_WRITE_CHAR_EVT          = 4,        /*!< This event is triggered upon the completion of a GATT characteristic write operation using `esp_ble_gattc_write_char`. */
    ESP_GATTC_CLOSE_EVT               = 5,        /*!< This event is triggered when a GATT virtual connection is closed via `esp_ble_gattc_close`, or when the physical connection is terminated. */
    ESP_GATTC_SEARCH_CMPL_EVT         = 6,        /*!< This event is triggered upon the completion of a service discovery using `esp_ble_gattc_search_service`. */
    ESP_GATTC_SEARCH_RES_EVT          = 7,        /*!< This event is triggered each time a service result is obtained using `esp_ble_gattc_search_service`. */
    ESP_GATTC_READ_DESCR_EVT          = 8,        /*!< This event is triggered upon the completion of a GATT characteristic descriptor read operation using `esp_ble_gattc_read_char_descr`. */
    ESP_GATTC_WRITE_DESCR_EVT         = 9,        /*!< This event is triggered upon the completion of a GATT characteristic descriptor write operation using `esp_ble_gattc_write_char_descr`. */
    ESP_GATTC_NOTIFY_EVT              = 10,       /*!< This event is triggered when a GATT notification or indication is received from the Server. */
    ESP_GATTC_PREP_WRITE_EVT          = 11,       /*!< This event is triggered upon the completion of a GATT prepare-write operation using `esp_ble_gattc_prepare_write`. */
    ESP_GATTC_EXEC_EVT                = 12,       /*!< This event is triggered upon the completion of a GATT write execution using `esp_ble_gattc_execute_write` .*/
    ESP_GATTC_ACL_EVT                 = 13,       /*!< Deprecated. */
    ESP_GATTC_CANCEL_OPEN_EVT         = 14,       /*!< Deprecated. */
    ESP_GATTC_SRVC_CHG_EVT            = 15,       /*!< This event is triggered when a service changed indication is received from the Server, indicating that the attribute database on the Server has been modified (e.g., services have been added, removed). */
    ESP_GATTC_ENC_CMPL_CB_EVT         = 17,       /*!< Deprecated. */
    ESP_GATTC_CFG_MTU_EVT             = 18,       /*!< This event is triggered upon the completion of the MTU configuration with `esp_ble_gattc_send_mtu_req`. */
    ESP_GATTC_ADV_DATA_EVT            = 19,       /*!< Deprecated. */
    ESP_GATTC_MULT_ADV_ENB_EVT        = 20,       /*!< Deprecated. */
    ESP_GATTC_MULT_ADV_UPD_EVT        = 21,       /*!< Deprecated. */
    ESP_GATTC_MULT_ADV_DATA_EVT       = 22,       /*!< Deprecated. */
    ESP_GATTC_MULT_ADV_DIS_EVT        = 23,       /*!< Deprecated. */
    ESP_GATTC_CONGEST_EVT             = 24,       /*!< This event is triggered when the GATT connection is congested. */
    ESP_GATTC_BTH_SCAN_ENB_EVT        = 25,       /*!< Deprecated. */
    ESP_GATTC_BTH_SCAN_CFG_EVT        = 26,       /*!< Deprecated. */
    ESP_GATTC_BTH_SCAN_RD_EVT         = 27,       /*!< Deprecated. */
    ESP_GATTC_BTH_SCAN_THR_EVT        = 28,       /*!< Deprecated. */
    ESP_GATTC_BTH_SCAN_PARAM_EVT      = 29,       /*!< Deprecated. */
    ESP_GATTC_BTH_SCAN_DIS_EVT        = 30,       /*!< Deprecated. */
    ESP_GATTC_SCAN_FLT_CFG_EVT        = 31,       /*!< Deprecated. */
    ESP_GATTC_SCAN_FLT_PARAM_EVT      = 32,       /*!< Deprecated. */
    ESP_GATTC_SCAN_FLT_STATUS_EVT     = 33,       /*!< Deprecated. */
    ESP_GATTC_ADV_VSC_EVT             = 34,       /*!< Deprecated. */
    ESP_GATTC_REG_FOR_NOTIFY_EVT      = 38,       /*!< This event is triggered upon the completion of a service notification registration using `esp_ble_gattc_register_for_notify`. */
    ESP_GATTC_UNREG_FOR_NOTIFY_EVT    = 39,       /*!< This event is triggered upon the completion of a service notification unregistration using `esp_ble_gattc_unregister_for_notify`. */
    ESP_GATTC_CONNECT_EVT             = 40,       /*!< This event is triggered when the physical connection is set up. */
    ESP_GATTC_DISCONNECT_EVT          = 41,       /*!< This event is triggered when the physical connection is terminated. */
    ESP_GATTC_READ_MULTIPLE_EVT       = 42,       /*!< This event is triggered when the multiple characteristic or descriptor values are retrieved using `esp_ble_gattc_read_multiple`. */
    ESP_GATTC_QUEUE_FULL_EVT          = 43,       /*!< This event is triggered when the GATTC command queue is full. */
    ESP_GATTC_SET_ASSOC_EVT           = 44,       /*!< This event is triggered when the association between the source and the remote address is added or deleted using `esp_ble_gattc_cache_assoc`. */
    ESP_GATTC_GET_ADDR_LIST_EVT       = 45,       /*!< This event is triggered when retrieving the address list from the GATTC cache is completed using `esp_ble_gattc_cache_get_addr_list`. */
    ESP_GATTC_DIS_SRVC_CMPL_EVT       = 46,       /*!< This event is triggered when the GATT service discovery is completed. */
    ESP_GATTC_READ_MULTI_VAR_EVT      = 47,       /*!< This event is triggered when multiple variable length characteristic values are retrieved using `esp_ble_gattc_read_multiple`. */
} esp_gattc_cb_event_t;


/**
 * @brief GATT Client callback parameters
 */
typedef union {

    /**
     * @brief Callback parameter for the event `ESP_GATTC_REG_EVT`
     */
    struct gattc_reg_evt_param {
        esp_gatt_status_t status;       /*!< Operation status */
        uint16_t app_id;                /*!< Application ID */
    } reg;                              /*!< Callback parameter for the event `ESP_GATTC_REG_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTC_OPEN_EVT`.
     */
    struct gattc_open_evt_param {
        esp_gatt_status_t status;       /*!< Operation status */
        uint16_t conn_id;               /*!< Connection ID */
        esp_bd_addr_t remote_bda;       /*!< Remote Bluetooth device address */
        uint16_t mtu;                   /*!< MTU size */
    } open;                             /*!< Callback parameter for the event `ESP_GATTC_OPEN_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTC_CLOSE_EVT`
     */
    struct gattc_close_evt_param {
        esp_gatt_status_t status;       /*!< Operation status */
        uint16_t conn_id;               /*!< Connection ID */
        esp_bd_addr_t remote_bda;       /*!< Remote bluetooth device address */
        esp_gatt_conn_reason_t reason;  /*!< The reason of GATT connection close */
    } close;                            /*!< Callback parameter for the event `ESP_GATTC_CLOSE_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTC_CFG_MTU_EVT`
     */
    struct gattc_cfg_mtu_evt_param {
        esp_gatt_status_t status;       /*!< Operation status */
        uint16_t conn_id;               /*!< Connection ID */
        uint16_t mtu;                   /*!< MTU size */
    } cfg_mtu;                          /*!< Callback parameter for the event `ESP_GATTC_CFG_MTU_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTC_SEARCH_CMPL_EVT`
     */
    struct gattc_search_cmpl_evt_param {
        esp_gatt_status_t status;                     /*!< Operation status */
        uint16_t conn_id;                             /*!< Connection ID */
        esp_service_source_t searched_service_source; /*!< The source of the service information */
    } search_cmpl;                                    /*!< Callback parameter for the event `ESP_GATTC_SEARCH_CMPL_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTC_SEARCH_RES_EVT`
     */
    struct gattc_search_res_evt_param {
        uint16_t conn_id;               /*!< Connection ID */
        uint16_t start_handle;          /*!< Service start handle */
        uint16_t end_handle;            /*!< Service end handle */
        esp_gatt_id_t srvc_id;          /*!< Service ID, including service UUID and other information */
        bool      is_primary;           /*!< True indicates a primary service, false otherwise */
    } search_res;                       /*!< Callback parameter for the event `ESP_GATTC_SEARCH_RES_EVT` */

    /**
     * @brief Callback parameter for the events: `ESP_GATTC_READ_CHAR_EVT`, `ESP_GATTC_READ_DESCR_EVT`,
     *        `ESP_GATTC_READ_MULTIPLE_EVT`, `ESP_GATTC_READ_MULTI_VAR_EVT`
     */
    struct gattc_read_char_evt_param {

        esp_gatt_status_t status;       /*!< Operation status */
        uint16_t conn_id;               /*!< Connection ID */
        uint16_t handle;                /*!< Characteristic handle */
        uint8_t *value;                 /*!< Characteristic value */
        uint16_t value_len;             /*!< Characteristic value length */
    } read;                             /*!< Callback parameter for events: `ESP_GATTC_READ_CHAR_EVT`, `ESP_GATTC_READ_DESCR_EVT`,
                                            `ESP_GATTC_READ_MULTIPLE_EVT`, `ESP_GATTC_READ_MULTI_VAR_EVT` */

    /**
     * @brief Callback parameter for the events: `ESP_GATTC_WRITE_CHAR_EVT`, `ESP_GATTC_PREP_WRITE_EVT`, `ESP_GATTC_WRITE_DESCR_EVT`.
     */
    struct gattc_write_evt_param {
        esp_gatt_status_t status;       /*!< Operation status */
        uint16_t conn_id;               /*!< Connection ID */
        uint16_t handle;                /*!< The characteristic or descriptor handle */
        uint16_t offset;                /*!< The position offset to write. This value is valid only for prepare write operation. */
    } write;                            /*!< Callback parameter for the events: `ESP_GATTC_WRITE_CHAR_EVT`, `ESP_GATTC_PREP_WRITE_EVT`, `ESP_GATTC_WRITE_DESCR_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTC_EXEC_EVT`
     */
    struct gattc_exec_cmpl_evt_param {
        esp_gatt_status_t status;       /*!< Operation status */
        uint16_t conn_id;               /*!< Connection ID */
    } exec_cmpl;                        /*!< Callback parameter for the event `ESP_GATTC_EXEC_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTC_NOTIFY_EVT`
     */
    struct gattc_notify_evt_param {
        uint16_t conn_id;               /*!< Connection ID */
        esp_bd_addr_t remote_bda;       /*!< Remote Bluetooth device address. */
        uint16_t handle;                /*!< The characteristic or descriptor handle */
        uint16_t value_len;             /*!< Notify attribute value length in bytes */
        uint8_t *value;                 /*!< Notify attribute value */
        bool is_notify;                 /*!< True means notification; false means indication */
    } notify;                           /*!< Callback parameter for the event `ESP_GATTC_NOTIFY_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTC_SRVC_CHG_EVT`
	 */
    struct gattc_srvc_chg_evt_param {
        esp_bd_addr_t remote_bda;       /*!< Remote Bluetooth device address */
    } srvc_chg;                         /*!< Callback parameter for the event `ESP_GATTC_SRVC_CHG_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTC_CONGEST_EVT`
     */
    struct gattc_congest_evt_param {
        uint16_t conn_id;               /*!< Connection ID */
        bool congested;                 /*!< True indicates that the connection is congested, false otherwise */
    } congest;                          /*!< Callback parameter for the event `ESP_GATTC_CONGEST_EVT` */
    /**
     * @brief Callback parameter for the event `ESP_GATTC_REG_FOR_NOTIFY_EVT`
     */
    struct gattc_reg_for_notify_evt_param {
        esp_gatt_status_t status;       /*!< Operation status */
        uint16_t handle;                /*!< The characteristic or descriptor handle */
    } reg_for_notify;                   /*!< Callback parameter for the event `ESP_GATTC_REG_FOR_NOTIFY_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTC_UNREG_FOR_NOTIFY_EVT`
     */
    struct gattc_unreg_for_notify_evt_param {
        esp_gatt_status_t status;       /*!< Operation status */
        uint16_t handle;                /*!< The characteristic or descriptor handle */
    } unreg_for_notify;                 /*!< Callback parameter for the event `ESP_GATTC_UNREG_FOR_NOTIFY_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTC_CONNECT_EVT`
     */
    struct gattc_connect_evt_param {
        uint16_t conn_id;                   /*!< Connection ID */
        uint8_t link_role;                  /*!< Link role : master role = 0; slave role = 1 */
        esp_bd_addr_t remote_bda;           /*!< Remote device address */
        esp_gatt_conn_params_t conn_params; /*!< Current connection parameters */
        esp_ble_addr_type_t ble_addr_type;  /*!< Remote device address type */
        uint16_t conn_handle;               /*!< HCI connection handle */
    } connect;                              /*!< Callback parameter for the event `ESP_GATTC_CONNECT_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTC_DISCONNECT_EVT`
     */
    struct gattc_disconnect_evt_param {
        esp_gatt_conn_reason_t reason;  /*!< Disconnection reason */
        uint16_t conn_id;               /*!< Connection ID */
        esp_bd_addr_t remote_bda;       /*!< Remote device address */
    } disconnect;                       /*!< Callback parameter for the event `ESP_GATTC_DISCONNECT_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTC_SET_ASSOC_EVT`
     */
    struct gattc_set_assoc_addr_cmp_evt_param {
        esp_gatt_status_t status;        /*!< Operation status */
    } set_assoc_cmp;                     /*!< Callback parameter for the event `ESP_GATTC_SET_ASSOC_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTC_GET_ADDR_LIST_EVT`
     */
    struct gattc_get_addr_list_evt_param {
        esp_gatt_status_t status;      /*!< Operation status */
        uint8_t num_addr;              /*!< The number of addresses in the local GATTC cache address list */
        esp_bd_addr_t *addr_list;      /*!< The pointer to the address list which has been retrieved from the local GATTC cache */
    } get_addr_list;                   /*!< Callback parameter for the event `ESP_GATTC_GET_ADDR_LIST_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTC_QUEUE_FULL_EVT`
     */
    struct gattc_queue_full_evt_param {
        esp_gatt_status_t status;      /*!< Operation status */
        uint16_t conn_id;              /*!< Connection ID */
        bool     is_full;              /*!< True indicates the GATTC command queue is full; false otherwise. */
    } queue_full;                      /*!< Callback parameter for the event `ESP_GATTC_QUEUE_FULL_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTC_DIS_SRVC_CMPL_EVT`
     */
    struct gattc_dis_srvc_cmpl_evt_param {
        esp_gatt_status_t status;      /*!< Operation status */
        uint16_t conn_id;              /*!< Connection ID */
    } dis_srvc_cmpl;                   /*!< Callback parameter for the event `ESP_GATTC_DIS_SRVC_CMPL_EVT` */

} esp_ble_gattc_cb_param_t;

/**
 * @brief GATT Client callback function type
 *
 * @param[in] event    Event type
 * @param[in] gattc_if GATT Client access interface
 * @param[in] param    The pointer to callback parameter
 */
typedef void (* esp_gattc_cb_t)(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);

/**
 * @brief  Register GATT Client application callbacks
 *
 * @param[in]      callback The pointer to the application callback function
 *
 * @note            Avoid performing time-consuming operations within the callback functions.
 *
 * @return
 *          - ESP_OK: Success
 *          - ESP_FAIL: Failure
 */
esp_err_t esp_ble_gattc_register_callback(esp_gattc_cb_t callback);

/**
 * @brief  Get the current application callbacks
 *
 * @return
 *        - esp_gattc_cb_t: Current callback
 */
esp_gattc_cb_t esp_ble_gattc_get_callback(void);

/**
 * @brief  Register a GATT Client application
 *
 * @param[in] app_id  The UUID for different application
 *
 * @note
 *      1. This function triggers `ESP_GATTC_REG_EVT`.
 *      2. The maximum number of applications is limited to 4.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_ERR_INVALID_ARG: The input `app_id` exceeds `ESP_APP_ID_MAX` (0x7fff) defined in esp_bt_defs.h
 *       - ESP_FAIL: Failure due to other reasons
 */
esp_err_t esp_ble_gattc_app_register(uint16_t app_id);

/**
 * @brief  Unregister a GATT Client application
 *
 * @param[in] gattc_if GATT Client access interface
 *
 * @note      This function triggers `ESP_GATTC_UNREG_EVT`.
 *
 * @return
 *        - ESP_OK: Success
 *        - ESP_FAIL: Failure
 */
esp_err_t esp_ble_gattc_app_unregister(esp_gatt_if_t gattc_if);

/**
 * @brief   Create an ACL connection
 *
 * @note
 *      1. Do not enable `BT_BLE_42_FEATURES_SUPPORTED` and `BT_BLE_50_FEATURES_SUPPORTED` in the menuconfig simultaneously.
 *      1. The function always triggers `ESP_GATTC_CONNECT_EVT` and `ESP_GATTC_OPEN_EVT`.
 *      2. When the device acts as GATT server, besides the above two events, this function triggers `ESP_GATTS_CONNECT_EVT` as well.
 *      3. This function will establish an ACL connection as a Central and a virtual connection as a GATT Client. If the ACL connection already exists, it will create a virtual connection only.

 *
 * @param[in]       gattc_if: GATT client access interface.
 * @param[in]       esp_gatt_create_conn: Pointer to the structure containing connection parameters.
 *
 * @return
 *                  - ESP_OK: Success
 *                  - others: Operation failed
 *
 */
esp_err_t esp_ble_gattc_enh_open(esp_gatt_if_t gattc_if, esp_ble_gatt_creat_conn_params_t *esp_gatt_create_conn);

/**
 * @brief  Create an ACL connection when `BT_BLE_42_FEATURES_SUPPORTED` is enabled in the menuconfig
 *
 * @param[in]       gattc_if         GATT Client access interface
 * @param[in]       remote_bda       Remote device address
 * @param[in]       remote_addr_type Remote device address type
 * @param[in]       is_direct        `True` indicates a direct connection, while `False` indicates a background auto connection. By now, background auto connection is not supported, please always pass True to this parameter.
 *
 * @note
 *      1. The function always triggers `ESP_GATTC_CONNECT_EVT` and `ESP_GATTC_OPEN_EVT`.
 *      2. When the device acts as GATT server, besides the above two events, this function triggers `ESP_GATTS_CONNECT_EVT` as well.
 *      3. This function will establish an ACL connection as a Central and a virtual connection as a GATT Client. If the ACL connection already exists, it will create a virtual connection only.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_FAIL: Failure
 */
#if (BLE_42_FEATURE_SUPPORT == TRUE)
esp_err_t esp_ble_gattc_open(esp_gatt_if_t gattc_if, esp_bd_addr_t remote_bda, esp_ble_addr_type_t remote_addr_type, bool is_direct);
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)

/**
 * @brief Create an ACL connection when `BT_BLE_50_FEATURES_SUPPORTED` is enabled in the menuconfig
 *
 * @param[in]       gattc_if         GATT Client access interface
 * @param[in]       remote_bda       Remote device address
 * @param[in]       remote_addr_type Remote device address type
 * @param[in]       is_direct        `True` indicates a direct connection, while `False` indicates a background auto connection. By now, background auto connection is not supported, please always pass True to this parameter.
 *
 * @note
 *      1. The function always triggers `ESP_GATTC_CONNECT_EVT` and `ESP_GATTC_OPEN_EVT`.
 *      2. When the device acts as GATT server, besides the above two events, this function triggers `ESP_GATTS_CONNECT_EVT` as well.
 *      3. This function will establish an ACL connection as a Central and a virtual connection as a GATT Client. If the ACL connection already exists, it will create a virtual connection only.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_FAIL: Failure
 */
#if (BLE_50_FEATURE_SUPPORT == TRUE)
esp_err_t esp_ble_gattc_aux_open(esp_gatt_if_t gattc_if, esp_bd_addr_t remote_bda, esp_ble_addr_type_t remote_addr_type, bool is_direct);
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)

/**
 * @brief  Close the virtual GATT Client connection
 *
 * @param[in]       gattc_if GATT Client access interface
 * @param[in]       conn_id  Connection ID to be closed
 *
 * @note
 *      1. This function triggers `ESP_GATTC_CLOSE_EVT`.
 *      2. There may be multiple virtual GATT server connections when multiple `app_id` got registered.
 *      3. This API closes one virtual GATT server connection only, if there exist other virtual GATT server connections. It does not close the physical connection.
 *      4. The API `esp_ble_gap_disconnect` can be used to disconnect the physical connection directly.
 *      5. If there is only one virtual GATT connection left, this API will terminate the ACL connection in addition and triggers `ESP_GATTC_DISCONNECT_EVT`. Then there is no need to call `esp_ble_gap_disconnect` anymore.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_FAIL: Failure
 */
esp_err_t esp_ble_gattc_close(esp_gatt_if_t gattc_if, uint16_t conn_id);

/**
 * @brief  Configure the MTU size in the GATT channel
 *
 *
 * @param[in]       gattc_if GATT Client access interface
 * @param[in]       conn_id  Connection ID
 *
 * @note
 *      1. This function triggers `ESP_GATTC_CFG_MTU_EVT`.
 *      2. You could call `esp_ble_gatt_set_local_mtu` to set the desired MTU size locally before this API. If not set, the GATT channel uses the default MTU size (23 bytes).
 *
 * @return
 *        - ESP_OK: Success
 *        - ESP_FAIL: Failure
 */
esp_err_t esp_ble_gattc_send_mtu_req (esp_gatt_if_t gattc_if, uint16_t conn_id);

/**
 * @brief  Search services from the local GATTC cache
 *
 * @param[in]       gattc_if    GATT Client access interface
 * @param[in]       conn_id     Connection ID
 * @param[in]       filter_uuid A UUID of the intended service. If NULL is passed, this API will return all services.
 *
 * @note
 *      1. This function triggers `ESP_GATTC_SEARCH_RES_EVT` each time a service is retrieved.
 *      2. This function triggers `ESP_GATTC_SEARCH_CMPL_EVT` when the search is completed.
 *      3. The 128-bit base UUID will be converted to a 16-bit UUID automatically in the search results. Other types of UUID remain unchanged.
 *
 * @return
 *       - ESP_OK:   Success
 *       - ESP_FAIL: Failure
 */
esp_err_t esp_ble_gattc_search_service(esp_gatt_if_t gattc_if, uint16_t conn_id, esp_bt_uuid_t *filter_uuid);

/**
 * @brief  Get the service with the given service UUID in the local GATTC cache
 *
 * @param[in]       gattc_if GATT Client access interface
 * @param[in]       conn_id  Connection ID
 * @param[in]       svc_uuid The pointer to the service UUID. If NULL is passed, the API will retrieve all services.
 * @param[out]      result   The pointer to the service which has been found in the local GATTC cache
 * @param[inout]    count    The number of services to retrieve. It will be updated with the actual number of services found.
 * @param[in]       offset   The position offset to retrieve
 *
 * @note
 *      1. This API does not trigger any event.
 *      2. `esp_ble_gattc_cache_refresh` can be used to discover services again.
 *
 * @return
 *      - ESP_OK: Success
 *      - ESP_FAIL: Failure
 */
esp_gatt_status_t esp_ble_gattc_get_service(esp_gatt_if_t gattc_if, uint16_t conn_id, esp_bt_uuid_t *svc_uuid,
                                            esp_gattc_service_elem_t *result, uint16_t *count, uint16_t offset);

/**
 * @brief  Get all characteristics with the given handle range in the local GATTC cache
 *
 * @param[in]       gattc_if     GATT Client access interface
 * @param[in]       conn_id      Connection ID
 * @param[in]       start_handle The attribute start handle
 * @param[in]       end_handle   The attribute end handle
 * @param[out]      result       The pointer to the characteristic in the service
 * @param[inout]    count        The number of characteristics to retrieve. It will be updated with the actual number of characteristics found.
 * @param[in]       offset       The position offset to retrieve
 *
 * @note
 *      1. This API does not trigger any event.
 *      2. `start_handle` must be greater than 0, and smaller than `end_handle`.
 *
 * @return
 *      - ESP_OK: Success
 *      - ESP_GATT_INVALID_HANDLE: Invalid GATT `start_handle` or `end_handle`
 *      - ESP_GATT_INVALID_PDU: NULL pointer to `result` or NULL pointer to `count` or the count value is 0
 *      - ESP_FAIL: Failure due to other reasons
 */
esp_gatt_status_t esp_ble_gattc_get_all_char(esp_gatt_if_t gattc_if,
                                             uint16_t conn_id,
                                             uint16_t start_handle,
                                             uint16_t end_handle,
                                             esp_gattc_char_elem_t *result,
                                             uint16_t *count, uint16_t offset);

/**
 * @brief  Get all descriptors with the given characteristic in the local GATTC cache
 *
 * @param[in]       gattc_if    GATT Client access interface
 * @param[in]       conn_id     Connection ID which identifies the server
 * @param[in]       char_handle The given characteristic handle
 * @param[out]      result      The pointer to the descriptor in the characteristic
 * @param[inout]    count       The number of descriptors to retrieve. It will be updated with the actual number of descriptors found.
 * @param[in]       offset      The position offset to retrieve
 *
 * @note
 *      1. This API does not trigger any event.
 *      2. `char_handle` must be greater than 0.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_GATT_INVALID_HANDLE: Invalid GATT `char_handle`
 *       - ESP_GATT_INVALID_PDU: NULL pointer to `result` or NULL pointer to `count` or the count value is 0
 *       - ESP_FAIL: Failure due to other reasons
 */
esp_gatt_status_t esp_ble_gattc_get_all_descr(esp_gatt_if_t gattc_if,
                                              uint16_t conn_id,
                                              uint16_t char_handle,
                                              esp_gattc_descr_elem_t *result,
                                              uint16_t *count, uint16_t offset);

/**
 * @brief  Get the characteristic with the given characteristic UUID in the local GATTC cache
 *
 * @param[in]       gattc_if      GATT Client access interface
 * @param[in]       conn_id       Connection ID
 * @param[in]       start_handle  The attribute start handle
 * @param[in]       end_handle    The attribute end handle
 * @param[in]       char_uuid     The characteristic UUID
 * @param[out]      result        The pointer to the characteristic in the service
 * @param[inout]    count         The number of characteristics to retrieve. It will be updated with the actual number of characteristics found.
 *
 * @note
 *      1. This API does not trigger any event.
 *      2. `start_handle` must be greater than 0, and smaller than `end_handle`.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_GATT_INVALID_HANDLE: Invalid GATT `start_handle` or `end_handle`
 *       - ESP_GATT_INVALID_PDU: NULL pointer to `result` or NULL pointer to `count` or the count value is 0
 *       - ESP_FAIL: Failure due to other reasons
 */
esp_gatt_status_t esp_ble_gattc_get_char_by_uuid(esp_gatt_if_t gattc_if,
                                                 uint16_t conn_id,
                                                 uint16_t start_handle,
                                                 uint16_t end_handle,
                                                 esp_bt_uuid_t char_uuid,
                                                 esp_gattc_char_elem_t *result,
                                                 uint16_t *count);

/**
 * @brief  Get the descriptor with the given characteristic UUID in the local GATTC cache
 *
 * @param[in]       gattc_if     GATT Client access interface
 * @param[in]       conn_id      Connection ID
 * @param[in]       start_handle The attribute start handle
 * @param[in]       end_handle   The attribute end handle
 * @param[in]       char_uuid    The characteristic UUID
 * @param[in]       descr_uuid   The descriptor UUID.
 * @param[out]      result       The pointer to the descriptor in the given characteristic.
 * @param[inout]    count        The number of descriptors want to retrieve. It will be updated with the actual number of descriptors found.
 *
 * @note
 *      1. This API does not trigger any event.
 *      2. `start_handle` must be greater than 0, and smaller than `end_handle`.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_GATT_INVALID_PDU: NULL pointer to `result` or NULL pointer to `count` or the count value is 0
 *       - ESP_FAIL: Failure due to other reasons
 */
esp_gatt_status_t esp_ble_gattc_get_descr_by_uuid(esp_gatt_if_t gattc_if,
                                                  uint16_t conn_id,
                                                  uint16_t start_handle,
                                                  uint16_t end_handle,
                                                  esp_bt_uuid_t char_uuid,
                                                  esp_bt_uuid_t descr_uuid,
                                                  esp_gattc_descr_elem_t *result,
                                                  uint16_t *count);

/**
 * @brief  Get the descriptor with the given characteristic handle in the local GATTC cache
 *
 * @param[in]       gattc_if    GATT Client access interface
 * @param[in]       conn_id     Connection ID which identifies the server
 * @param[in]       char_handle The characteristic handle
 * @param[in]       descr_uuid  The descriptor UUID
 * @param[out]      result      The pointer to the descriptor in the given characteristic
 * @param[inout]    count       The number of descriptors want to retrieve. It will be updated with the actual number of descriptors found.
 *
 * @note
 *      1. This API does not trigger any event.
 *      2. `char_handle` must be greater than 0.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_GATT_INVALID_HANDLE: Invalid GATT `char_handle`
 *       - ESP_GATT_INVALID_PDU: NULL pointer to `result` or NULL pointer to `count` or the count value is 0
 *       - ESP_FAIL: Failure due to other reasons
 */
esp_gatt_status_t esp_ble_gattc_get_descr_by_char_handle(esp_gatt_if_t gattc_if,
                                                         uint16_t conn_id,
                                                         uint16_t char_handle,
                                                         esp_bt_uuid_t descr_uuid,
                                                         esp_gattc_descr_elem_t *result,
                                                         uint16_t *count);

/**
 * @brief  Get the included services with the given service handle in the local GATTC cache.
 *
 * @param[in]       gattc_if     GATT Client access interface
 * @param[in]       conn_id      Connection ID which identifies the server
 * @param[in]       start_handle The attribute start handle
 * @param[in]       end_handle   The attribute end handle
 * @param[in]       incl_uuid    The included service UUID
 * @param[out]      result       The pointer to the included service with the given service handle.
 * @param[inout]    count        The number of included services to retrieve. It will be updated with the actual number of included services found.
 *
 * @note
 *      1. This API does not trigger any event.
 *      2. `start_handle` must be greater than 0, and smaller than `end_handle`.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_GATT_INVALID_PDU: NULL pointer to `result` or NULL pointer to `count` or the count value is 0
 *       - ESP_FAIL: Failure due to other reasons
 */
esp_gatt_status_t esp_ble_gattc_get_include_service(esp_gatt_if_t gattc_if,
                                                    uint16_t conn_id,
                                                    uint16_t start_handle,
                                                    uint16_t end_handle,
                                                    esp_bt_uuid_t *incl_uuid,
                                                    esp_gattc_incl_svc_elem_t *result,
                                                    uint16_t *count);

/**
 * @brief  Get the attribute count with the given service or characteristic in the local GATTC cache
 *
 * @param[in]       gattc_if     GATT Client access interface
 * @param[in]       conn_id      Connection ID
 * @param[in]       type         The attribute type
 * @param[in]       start_handle The attribute start handle. If the type is `ESP_GATT_DB_DESCRIPTOR`, this parameter will be ignored.
 * @param[in]       end_handle   The attribute end handle. If the type is `ESP_GATT_DB_DESCRIPTOR`, this parameter will be ignored.
 * @param[in]       char_handle  The characteristic handle. This parameter is valid only if the type is `ESP_GATT_DB_DESCRIPTOR`.
 * @param[out]      count        The number of attributes found in the local GATTC cache with the given attribute type
 *
 *
 * @note
 *     1. This API does not trigger any event.
 *     2. `start_handle` must be greater than 0, and smaller than `end_handle` if the `type` is not `ESP_GATT_DB_DESCRIPTOR`.
 *
 * @return
 *                  - ESP_OK: Success
 *                  - ESP_GATT_INVALID_HANDLE: Invalid GATT `start_handle`, `end_handle`
 *                  - ESP_GATT_INVALID_PDU: NULL pointer to `count`
 *                  - ESP_FAIL: Failure due to other reasons
 */
esp_gatt_status_t esp_ble_gattc_get_attr_count(esp_gatt_if_t gattc_if,
                                               uint16_t conn_id,
                                               esp_gatt_db_attr_type_t type,
                                               uint16_t start_handle,
                                               uint16_t end_handle,
                                               uint16_t char_handle,
                                               uint16_t *count);

/**
 * @brief  Get the GATT database elements
 *
 * @param[in]       gattc_if     GATT Client access interface
 * @param[in]       start_handle The attribute start handle
 * @param[in]       end_handle   The attribute end handle
 * @param[in]       conn_id      Connection ID
 * @param[out]      db           The pointer to GATT database elements
 * @param[inout]    count        The number of elements in the database to retrieve. It will be updated with the actual number of elements found.
 *
 * @note
 *       1. This API does not trigger any event.
 *       2. `start_handle` must be greater than 0, and smaller than `end_handle`.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_GATT_INVALID_HANDLE: Invalid GATT `start_handle`, `end_handle`
 *       - ESP_GATT_INVALID_PDU: NULL pointer to `db` or NULL pointer to `count` or the count value is 0
 *       - ESP_FAIL: Failure due to other reasons
 *
 */
esp_gatt_status_t esp_ble_gattc_get_db(esp_gatt_if_t gattc_if, uint16_t conn_id, uint16_t start_handle, uint16_t end_handle,
                                        esp_gattc_db_elem_t *db, uint16_t *count);

/**
 * @brief  Read the characteristics value of the given characteristic handle
 *
 * @param[in]       gattc_if GATT Client access interface
 * @param[in]       conn_id  Connection ID
 * @param[in]       handle   Characteristic handle to read
 * @param[in]       auth_req Authenticate request type
 *
 * @note
 *      1. This function triggers `ESP_GATTC_READ_CHAR_EVT`.
 *      2. This function should be called only after the connection has been established.
 *      3. `handle` must be greater than 0.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_ERR_INVALID_STATE: The connection has not been established.
 *       - ESP_GATT_INVALID_HANDLE: Invalid `handle`
 *       - ESP_FAIL: Failure due to other reasons
 */
esp_err_t esp_ble_gattc_read_char (esp_gatt_if_t gattc_if,
                                   uint16_t conn_id,
                                   uint16_t handle,
                                   esp_gatt_auth_req_t auth_req);

/**
 * @brief  Read the characteristics value of the given characteristic UUID.
 *
 * @param[in]       gattc_if     GATT Client access interface.
 * @param[in]       conn_id      Connection ID
 * @param[in]       start_handle The attribute start handle
 * @param[in]       end_handle   The attribute end handle
 * @param[in]       uuid         The pointer to UUID of attribute to read
 * @param[in]       auth_req     Authenticate request type
 *
 * @note
 *      1. This function triggers `ESP_GATTC_READ_CHAR_EVT`.
 *      2. This function should be called only after the connection has been established.
 *      3. `start_handle` must be greater than 0, and smaller than `end_handle`.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_GATT_ILLEGAL_PARAMETER: NULL pointer to  `uuid`
 *       - ESP_ERR_INVALID_STATE: The connection has not been established
 *       - ESP_FAIL: Failure due to other reasons
 */
esp_err_t esp_ble_gattc_read_by_type (esp_gatt_if_t gattc_if,
                                      uint16_t conn_id,
                                      uint16_t start_handle,
                                      uint16_t end_handle,
                                      esp_bt_uuid_t *uuid,
                                      esp_gatt_auth_req_t auth_req);

/**
 * @brief  Read multiple characteristic or descriptor values
 *
 * @param[in]       gattc_if   GATT Client access interface
 * @param[in]       conn_id    Connection ID which specifies the server
 * @param[in]       read_multi Pointer to `esp_gattc_multi_t`
 * @param[in]       auth_req   Authenticate request type
 *
 * @note
 *      1. This function triggers `ESP_GATTC_READ_MULTIPLE_EVT`.
 *      2. This function should be called only after the connection has been established.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_ERR_INVALID_STATE: The connection has not been established
 *       - ESP_FAIL: Failure due to other reasons
 */
esp_err_t esp_ble_gattc_read_multiple(esp_gatt_if_t gattc_if,
                                      uint16_t conn_id, esp_gattc_multi_t *read_multi,
                                      esp_gatt_auth_req_t auth_req);

/**
 * @brief  Read multiple variable length characteristic values.
 *
 * @param[in]       gattc_if   GATT Client access interface
 * @param[in]       conn_id    Connection ID
 * @param[in]       read_multi The pointer to the `esp_gattc_multi_t`
 * @param[in]       auth_req   Authenticate request type
 *
 * @note
 *      1. This function triggers `ESP_GATTC_READ_MULTI_VAR_EVT`.
 *      2. This function should be called only after the connection has been established.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_ERR_INVALID_STATE: The connection has not been established.
 *       - ESP_FAIL: Failure due to other reasons
 */
esp_err_t esp_ble_gattc_read_multiple_variable(esp_gatt_if_t gattc_if,
                                      uint16_t conn_id, esp_gattc_multi_t *read_multi,
                                      esp_gatt_auth_req_t auth_req);

/**
 * @brief  Read a characteristics descriptor
 *
 * @param[in]       gattc_if GATT Client access interface
 * @param[in]       conn_id  Connection ID
 * @param[in]       handle   Descriptor handle to read
 * @param[in]       auth_req Authenticate request type
 *
 * @note
 *      1. This function triggers `ESP_GATTC_READ_DESCR_EVT`.
 *      2. This function should be called only after the connection has been established.
 *      3. `handle` must be greater than 0.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_ERR_INVALID_STATE: The connection has not been established.
 *       - ESP_GATT_INVALID_HANDLE：Invalid `handle`
 *       - ESP_FAIL: Failure due to other reasons
 */
esp_err_t esp_ble_gattc_read_char_descr (esp_gatt_if_t gattc_if,
                                         uint16_t conn_id,
                                         uint16_t handle,
                                         esp_gatt_auth_req_t auth_req);

/**
 * @brief  Write the characteristic value of a given characteristic handle
 *
 * @param[in]       gattc_if   GATT Client access interface
 * @param[in]       conn_id    Connection ID
 * @param[in]       handle     The characteristic handle to write
 * @param[in]       value_len  The length of the value to write in bytes
 * @param[in]       value      The value to write
 * @param[in]       write_type The type of Attribute write operation
 * @param[in]       auth_req   Authentication request type
 *
 * @note
 *      1. This function triggers `ESP_GATTC_WRITE_CHAR_EVT`.
 *      2. This function should be called only after the connection has been established.
 *      3. `handle` must be greater than 0.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_ERR_INVALID_STATE: The connection has not been established.
 *       - ESP_GATT_INVALID_HANDLE：Invalid `handle`
 *       - ESP_FAIL: Failure due to other reasons
 */
esp_err_t esp_ble_gattc_write_char( esp_gatt_if_t gattc_if,
                                    uint16_t conn_id,
                                    uint16_t handle,
                                    uint16_t value_len,
                                    uint8_t *value,
                                    esp_gatt_write_type_t write_type,
                                    esp_gatt_auth_req_t auth_req);

/**
 * @brief  Write Characteristic descriptor value of a given descriptor handle
 *
 * @param[in]       gattc_if   GATT Client access interface
 * @param[in]       conn_id    Connection ID
 * @param[in]       handle     The descriptor handle to write.
 * @param[in]       value_len  The length of the value to write in bytes.
 * @param[in]       value      The value to write
 * @param[in]       write_type The type of Attribute write operation
 * @param[in]       auth_req   Authentication request type
 *
 * @note
 *      1. This function triggers `ESP_GATTC_WRITE_DESCR_EVT`.
 *      2. This function should be called only after the connection has been established.
 *      3. `handle` must be greater than 0.
 *
 * @return
 *      - ESP_OK: Success
 *      - ESP_ERR_INVALID_STATE: The connection has not been established.
 *      - ESP_GATT_INVALID_HANDLE：Invalid `handle`
 *      - ESP_FAIL: Failure due to other reasons
 */
esp_err_t esp_ble_gattc_write_char_descr (esp_gatt_if_t gattc_if,
                                         uint16_t conn_id,
                                         uint16_t handle,
                                         uint16_t value_len,
                                         uint8_t *value,
                                         esp_gatt_write_type_t write_type,
                                         esp_gatt_auth_req_t auth_req);

/**
 * @brief  Prepare to write a characteristic value which is longer than the MTU size to a specified characteristic handle
 *
 * @param[in]       gattc_if  GATT Client access interface
 * @param[in]       conn_id   Connection ID
 * @param[in]       handle    Characteristic handle to prepare to write
 * @param[in]       offset    The position offset to write
 * @param[in]       value_len The length of the value to write in bytes
 * @param[in]       value     The value to write
 * @param[in]       auth_req  Authentication request type
 *
 * @note
 *      1. This function should be called only after the connection has been established.
 *      2. After using this API, use `esp_ble_gattc_execute_write` to write.
 *      3. This function triggers `ESP_GATTC_PREP_WRITE_EVT`.
 *      4. If `value_len` is less than or equal to MTU size, it is recommended to `esp_ble_gattc_write_char` to write directly.
 *      5. `handle` must be greater than 0.
 *
 * @return
 *      - ESP_OK: Success
 *      - ESP_ERR_INVALID_STATE: The connection has not been established.
 *      - ESP_GATT_INVALID_HANDLE：Invalid `handle`
 *      - ESP_FAIL: Failure due to other reasons
 */
esp_err_t esp_ble_gattc_prepare_write(esp_gatt_if_t gattc_if,
                                      uint16_t conn_id,
                                      uint16_t handle,
                                      uint16_t offset,
                                      uint16_t value_len,
                                      uint8_t *value,
                                      esp_gatt_auth_req_t auth_req);

/**
 * @brief  Prepare to write a characteristic descriptor value at a given handle
 *
 * @param[in]       gattc_if  GATT Client access interface
 * @param[in]       conn_id   Connection ID
 * @param[in]       handle    Characteristic descriptor handle to prepare to write
 * @param[in]       offset    The position offset to write
 * @param[in]       value_len The length of the value to write in bytes
 * @param[in]       value     The value to write
 * @param[in]       auth_req  Authentication request type
 *
 * @note
 *      1. This function triggers `ESP_GATTC_WRITE_CHAR_EVT`.
 *      2. This function should be called only after the connection has been established.
 *      3. `handle` must be greater than 0.
 *
 * @return
 *      - ESP_OK: Success
 *      - ESP_ERR_INVALID_STATE: The connection has not been established.
 *      - ESP_GATT_INVALID_HANDLE：Invalid `handle`
 *      - ESP_FAIL: Failure due to other reasons
 */
esp_err_t esp_ble_gattc_prepare_write_char_descr(esp_gatt_if_t gattc_if,
                                                 uint16_t conn_id,
                                                 uint16_t handle,
                                                 uint16_t offset,
                                                 uint16_t value_len,
                                                 uint8_t *value,
                                                 esp_gatt_auth_req_t auth_req);

/**
 * @brief  Execute a prepared writing sequence
 *
 * @param[in]       gattc_if   GATT Client access interface
 * @param[in]       conn_id    Connection ID
 * @param[in]       is_execute True if it is to execute the writing sequence; false if it is to cancel the writing sequence.
 *
 * @note            This function triggers `ESP_GATTC_EXEC_EVT`.
 *
 * @return
 *        - ESP_OK: Success
 *        - ESP_FAIL: Failure
 */
esp_err_t esp_ble_gattc_execute_write (esp_gatt_if_t gattc_if, uint16_t conn_id, bool is_execute);

/**
 * @brief  Register to receive notification/indication of a characteristic.
 *
 * @param[in]       gattc_if   GATT Client access interface
 * @param[in]       server_bda Target GATT server device address
 * @param[in]       handle     Target GATT characteristic handle
 *
 * @note
 *       1. This function triggers `ESP_GATTC_REG_FOR_NOTIFY_EVT`.
 *       2. You should call `esp_ble_gattc_write_char_descr()` after this API to write Client Characteristic Configuration (CCC) descriptor to the value of 1 (Enable Notification) or 2 (Enable Indication).
 *       3. `handle` must be greater than 0.
 *
 * @return
 *        - ESP_OK: Success
 *        - ESP_GATT_INVALID_HANDLE：Invalid `handle`
 *        - ESP_FAIL: Failure due to other reasons
 */
esp_err_t esp_ble_gattc_register_for_notify (esp_gatt_if_t gattc_if,
                                             esp_bd_addr_t server_bda,
                                             uint16_t handle);

/**
 * @brief  Unregister the notification of a service.
 *
 * @param[in]       gattc_if   GATT Client access interface
 * @param[in]       server_bda Target GATT server device address
 * @param[in]       handle     Target GATT characteristic handle
 *
 * @note
 *      1. This function triggers `ESP_GATTC_UNREG_FOR_NOTIFY_EVT`.
 *      2. You should call `esp_ble_gattc_write_char_descr()` after this API to write Client Characteristic Configuration (CCC) descriptor value to 0.
 *      3. `handle` must be greater than 0
 * @return
 *       - ESP_OK: Success
 *       - ESP_GATT_INVALID_HANDLE：Invalid `handle`
 *       - ESP_FAIL: Failure due to other reasons
 */
esp_err_t esp_ble_gattc_unregister_for_notify (esp_gatt_if_t gattc_if,
                                               esp_bd_addr_t server_bda,
                                               uint16_t handle);

/**
 * @brief  Refresh the cache of the remote device
 *
 * @param[in]       remote_bda Remote device address
 *
 * @note
 *       1. If the device is connected, this API will restart the discovery of service information of the remote device.
 *       2. This function triggers `ESP_GATTC_DIS_SRVC_CMPL_EVT` only after the ACL connection is established. Otherwise, no events will be triggered.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_FAIL: Failure
 */
esp_err_t esp_ble_gattc_cache_refresh(esp_bd_addr_t remote_bda);

/**
 * @brief  Add or remove the association between the address in the local GATTC cache with the source address of the remote device
 *
 * @param[in]       gattc_if   GATT Client access interface
 * @param[in]       src_addr   The source address intended to be associated to the `assoc_addr` which has been stored in the local GATTC cache
 * @param[in]       assoc_addr The associated device address intended to share the attribute table with the source address
 * @param[in]       is_assoc   True if adding the association; false if removing the association.
 *
 * @note
 *      1. This API is primarily used when the client has a stored server-side database (`assoc_addr`) and needs to connect to
 *         another device (`src_addr`) with the same attribute database. By invoking this API, the stored database is utilized
 *         as the peer server database, eliminating the need for attribute database search and discovery. This reduces
 *         processing time and accelerates the connection process.
 *      2. The attribute table of a device with `assoc_addr` must be stored in the local GATTC cache first.
 *         Then, the attribute table of the device with `src_addr` must be the same as the one with `assoc_addr`.
 *      3. This function triggers `ESP_GATTC_SET_ASSOC_EVT`.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_FAIL: Failure
 */
esp_err_t esp_ble_gattc_cache_assoc(esp_gatt_if_t gattc_if, esp_bd_addr_t src_addr,
                                      esp_bd_addr_t assoc_addr, bool is_assoc);

/**
 * @brief  Get the address list stored in the local GATTC cache
 *
 * @param[in]       gattc_if GATT Client access interface
 *
 * @note            This function triggers `ESP_GATTC_GET_ADDR_LIST_EVT`.
 *
 * @return
 *        - ESP_OK: Success
 *        - ESP_FAIL: Failure
 */
esp_err_t esp_ble_gattc_cache_get_addr_list(esp_gatt_if_t gattc_if);

/**
 * @brief  Clean the service cache of the target device in the local GATTC cache
 *
 * @param[in]       remote_bda Remote device address
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_FAIL: Failure
 */
esp_err_t esp_ble_gattc_cache_clean(esp_bd_addr_t remote_bda);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_GATTC_API_H__ */
