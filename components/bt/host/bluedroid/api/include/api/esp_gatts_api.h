/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_GATTS_API_H__
#define __ESP_GATTS_API_H__

#include "esp_bt_defs.h"
#include "esp_gatt_defs.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/// GATT Server callback function events
typedef enum {
    ESP_GATTS_REG_EVT                 = 0,       /*!< This event is triggered when a GATT Server application is registered using `esp_ble_gatts_app_register`. */
    ESP_GATTS_READ_EVT                = 1,       /*!< This event is triggered when the read request from the Client is received. */
    ESP_GATTS_WRITE_EVT               = 2,       /*!< This event is triggered when the write request from the Client is received. */
    ESP_GATTS_EXEC_WRITE_EVT          = 3,       /*!< This event is triggered when the write execution request from the Client is received. */
    ESP_GATTS_MTU_EVT                 = 4,       /*!< This event is triggered when the MTU configuration request from the Client is received. */
    ESP_GATTS_CONF_EVT                = 5,       /*!< This event is triggered when the confirmation from the Client is received. */
    ESP_GATTS_UNREG_EVT               = 6,       /*!< This event is triggered when a GATT Server application is unregistered using `esp_ble_gatts_app_unregister`. */
    ESP_GATTS_CREATE_EVT              = 7,       /*!< This event is triggered when a GATT Server service is created using `esp_ble_gatts_create_service`. */
    ESP_GATTS_ADD_INCL_SRVC_EVT       = 8,       /*!< This event is triggered when an included service is added using `esp_ble_gatts_add_included_service`. */
    ESP_GATTS_ADD_CHAR_EVT            = 9,       /*!< This event is triggered when a characteristic is added to the service using `esp_ble_gatts_add_char`. */
    ESP_GATTS_ADD_CHAR_DESCR_EVT      = 10,      /*!< This event is triggered when a characteristic descriptor is added to the service using `esp_ble_gatts_add_char_descr`. */
    ESP_GATTS_DELETE_EVT              = 11,      /*!< This event is triggered when the service is deleted using `esp_ble_gatts_delete_service`. */
    ESP_GATTS_START_EVT               = 12,      /*!< This event is triggered when the service is started using `esp_ble_gatts_start_service`. */
    ESP_GATTS_STOP_EVT                = 13,      /*!< This event is triggered when the service is stopped using `esp_ble_gatts_stop_service`.  */
    ESP_GATTS_CONNECT_EVT             = 14,      /*!< This event is triggered when a physical connection is set up. */
    ESP_GATTS_DISCONNECT_EVT          = 15,      /*!< This event is triggered when a physical connection is terminated. */
    ESP_GATTS_OPEN_EVT                = 16,      /*!< This event is triggered when a virtual connection is created using `esp_ble_gatts_open`. */
    ESP_GATTS_CANCEL_OPEN_EVT         = 17,      /*!< Deprecated. */
    ESP_GATTS_CLOSE_EVT               = 18,      /*!< This event is triggered when a virtual connection is closed using `esp_ble_gatts_close`. */
    ESP_GATTS_LISTEN_EVT              = 19,      /*!< Deprecated. */
    ESP_GATTS_CONGEST_EVT             = 20,      /*!< This event is triggered when the GATT connection is congested. */
    /* following is extra event */
    ESP_GATTS_RESPONSE_EVT            = 21,      /*!< This event is triggered when a response is sent to the request using `esp_ble_gatts_send_response`. */
    ESP_GATTS_CREAT_ATTR_TAB_EVT      = 22,      /*!< This event is triggered when a service attribute table is created using `esp_ble_gatts_create_attr_tab`. */
    ESP_GATTS_SET_ATTR_VAL_EVT        = 23,      /*!< This event is triggered when an attribute value is set using `esp_ble_gatts_set_attr_value`. */
    ESP_GATTS_SEND_SERVICE_CHANGE_EVT = 24,      /*!< This event is triggered when a service change indication is sent using `esp_ble_gatts_send_service_change_indication`. */
} esp_gatts_cb_event_t;

/**
 * @brief GATT Server callback parameters
 */
typedef union {
    /**
     * @brief Callback parameter for the event `ESP_GATTS_REG_EVT`
     */
    struct gatts_reg_evt_param {
        esp_gatt_status_t status;       /*!< Operation status */
        uint16_t app_id;                /*!< Application ID */
    } reg;                              /*!< Callback parameter for the event `ESP_GATTS_REG_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTS_READ_EVT`
     */
    struct gatts_read_evt_param {
        uint16_t conn_id;               /*!< Connection ID */
        uint32_t trans_id;              /*!< Transfer ID */
        esp_bd_addr_t bda;              /*!< The device address to read */
        uint16_t handle;                /*!< The attribute handle */
        uint16_t offset;                /*!< The position offset to read. If the length of value is less than or equal to the MTU size, this value is 0. */
        bool is_long;                   /*!< True indicates that the length of value is greater than the MTU size; false otherwise. */
        bool need_rsp;                  /*!< True indicates that the `esp_ble_gatts_send_response` is required in the following step; false otherwise. */
    } read;                             /*!< Callback parameter for the event `ESP_GATTS_READ_EVT` */


    /**
     * @brief Callback parameter for the event `ESP_GATTS_WRITE_EVT`
     */
    struct gatts_write_evt_param {
        uint16_t conn_id;               /*!< Connection ID */
        uint32_t trans_id;              /*!< Transfer ID */
        esp_bd_addr_t bda;              /*!< The device address to write */
        uint16_t handle;                /*!< The attribute handle */
        uint16_t offset;                /*!< The position offset to write. If the length of value is less than or equal to the MTU size, this value is 0.*/
        bool need_rsp;                  /*!< True indicates that the `esp_ble_gatts_send_response` is required in the following step; false otherwise. */
        bool is_prep;                   /*!< True indicates the write operation is a prepared write operation */
        uint16_t len;                   /*!< The length of the write attribute value in bytes */
        uint8_t *value;                 /*!< The write attribute value */
    } write;                            /*!< Callback parameter for the event `ESP_GATTS_WRITE_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTS_EXEC_WRITE_EVT`
     */
    struct gatts_exec_write_evt_param {
        uint16_t conn_id;               /*!< Connection ID */
        uint32_t trans_id;              /*!< Transfer ID */
        esp_bd_addr_t bda;              /*!< The bluetooth device address to write */
        #define ESP_GATT_PREP_WRITE_CANCEL 0x00 /*!< Flag to indicate the cancellation of a prepare write operation */
        #define ESP_GATT_PREP_WRITE_EXEC   0x01 /*!< Flag to indicate the execution of a prepare write operation */
        uint8_t exec_write_flag;        /*!< Execute write flag: `ESP_GATT_PREP_WRITE_CANCEL` or `ESP_GATT_PREP_WRITE_EXEC` */
    } exec_write;                       /*!< Callback parameter for the event `ESP_GATTS_EXEC_WRITE_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTS_MTU_EVT`
     */
    struct gatts_mtu_evt_param {
        uint16_t conn_id;               /*!< Connection ID */
        uint16_t mtu;                   /*!< MTU size */
    } mtu;                              /*!< Callback parameter for the event `ESP_GATTS_MTU_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTS_CONF_EVT`
     */
    struct gatts_conf_evt_param {
        esp_gatt_status_t status;       /*!< Operation status */
        uint16_t conn_id;               /*!< Connection ID */
        uint16_t handle;                /*!< Attribute handle */
        uint16_t len;                   /*!< The length of indication or notification value in bytes. The length is invalid if the notification or indication failed. */
        uint8_t *value;                 /*!< The indication or notification value. The value is invalid if the notification or indication failed. */
    } conf;                             /*!< Callback parameter for the event `ESP_GATTS_CONF_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTS_CREATE_EVT`
     */
    struct gatts_create_evt_param {
        esp_gatt_status_t status;       /*!< Operation status */
        uint16_t service_handle;        /*!< Service attribute handle */
        esp_gatt_srvc_id_t service_id;  /*!< Service ID, including service UUID and other information */
    } create;                           /*!< Callback parameter for the event `ESP_GATTS_CREATE_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTS_ADD_INCL_SRVC_EVT`
     */
    struct gatts_add_incl_srvc_evt_param {
        esp_gatt_status_t status;       /*!< Operation status */
        uint16_t attr_handle;           /*!< Included service attribute handle */
        uint16_t service_handle;        /*!< Service attribute handle */
    } add_incl_srvc;                    /*!< Callback parameter for the event `ESP_GATTS_ADD_INCL_SRVC_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTS_ADD_CHAR_EVT`
     */
    struct gatts_add_char_evt_param {
        esp_gatt_status_t status;       /*!< Operation status */
        uint16_t attr_handle;           /*!< Characteristic attribute handle */
        uint16_t service_handle;        /*!< Service attribute handle */
        esp_bt_uuid_t char_uuid;        /*!< Characteristic UUID */
    } add_char;                         /*!< Callback parameter for the event `ESP_GATTS_ADD_CHAR_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTS_ADD_CHAR_DESCR_EVT`
     */
    struct gatts_add_char_descr_evt_param {
        esp_gatt_status_t status;       /*!< Operation status */
        uint16_t attr_handle;           /*!< Descriptor attribute handle */
        uint16_t service_handle;        /*!< Service attribute handle */
        esp_bt_uuid_t descr_uuid;       /*!< Characteristic descriptor UUID */
    } add_char_descr;                   /*!< Callback parameter for the event `ESP_GATTS_ADD_CHAR_DESCR_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTS_DELETE_EVT`
     */
    struct gatts_delete_evt_param {
        esp_gatt_status_t status;       /*!< Operation status */
        uint16_t service_handle;        /*!< Service attribute handle */
    } del;                              /*!< Callback parameter for the event `ESP_GATTS_DELETE_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTS_START_EVT`
     */
    struct gatts_start_evt_param {
        esp_gatt_status_t status;       /*!< Operation status */
        uint16_t service_handle;        /*!< Service attribute handle */
    } start;                            /*!< Callback parameter for the event `ESP_GATTS_START_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTS_STOP_EVT`
     */
    struct gatts_stop_evt_param {
        esp_gatt_status_t status;       /*!< Operation status */
        uint16_t service_handle;        /*!< Service attribute handle */
    } stop;                             /*!< Callback parameter for the event `ESP_GATTS_STOP_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTS_CONNECT_EVT`
     */
    struct gatts_connect_evt_param {
        uint16_t conn_id;                   /*!< Connection ID */
        uint8_t link_role;                  /*!< Link role: master role = 0; slave role = 1 */
        esp_bd_addr_t remote_bda;           /*!< Remote device address */
        esp_gatt_conn_params_t conn_params; /*!< Current connection parameters */
        esp_ble_addr_type_t ble_addr_type;  /*!< Remote device address type */
        uint16_t conn_handle;               /*!< HCI connection handle */
    } connect;                              /*!< Callback parameter for the event `ESP_GATTS_CONNECT_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTS_DISCONNECT_EVT`
     */
    struct gatts_disconnect_evt_param {
        uint16_t conn_id;               /*!< Connection ID */
        esp_bd_addr_t remote_bda;       /*!< Remote device address */
        esp_gatt_conn_reason_t reason;  /*!< The reason of disconnection */
    } disconnect;                       /*!< Callback parameter for the event `ESP_GATTS_DISCONNECT_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTS_OPEN_EVT`
     */
    struct gatts_open_evt_param {
        esp_gatt_status_t status;       /*!< Operation status */
    } open;                             /*!< Callback parameter for the event `ESP_GATTS_OPEN_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTS_CANCEL_OPEN_EVT`
     */
    struct gatts_cancel_open_evt_param {
        esp_gatt_status_t status;       /*!< Operation status */
    } cancel_open;                      /*!< Callback parameter for the event `ESP_GATTS_CANCEL_OPEN_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTS_CLOSE_EVT`
     */
    struct gatts_close_evt_param {
        esp_gatt_status_t status;       /*!< Operation status */
        uint16_t conn_id;               /*!< Connection ID */
    } close;                            /*!< Callback parameter for the event `ESP_GATTS_CLOSE_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTS_CONGEST_EVT`
     */
    struct gatts_congest_evt_param {
        uint16_t conn_id;               /*!< Connection ID */
        bool congested;                 /*!< True indicates the connection is congested; false otherwise. */
    } congest;                          /*!< Callback parameter for the event `ESP_GATTS_CONGEST_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTS_RESPONSE_EVT`
     */
    struct gatts_rsp_evt_param {
        esp_gatt_status_t status;       /*!< Operation status */
        uint16_t conn_id;               /*!< Connection ID */
        uint16_t handle;                /*!< Attribute handle which sends the response */
    } rsp;                              /*!< Callback parameter for the event `ESP_GATTS_RESPONSE_EVT` */

    /**
     * @brief Callback parameter for the event `ESP_GATTS_CREAT_ATTR_TAB_EVT`
     */
    struct gatts_add_attr_tab_evt_param{
        esp_gatt_status_t status;       /*!< Operation status */
        esp_bt_uuid_t svc_uuid;         /*!< Service UUID type */
        uint8_t svc_inst_id;            /*!< Service ID */
        uint16_t num_handle;            /*!< The number of the attribute handles which have been added to the GATT Service table */
        uint16_t *handles;              /*!< The handles which have been added to the table */
    } add_attr_tab;                     /*!< Callback parameter for the event `ESP_GATTS_CREAT_ATTR_TAB_EVT` */


   /**
    * @brief Callback parameter for the event `ESP_GATTS_SET_ATTR_VAL_EVT`
    */
    struct gatts_set_attr_val_evt_param{
        uint16_t srvc_handle;           /*!< The service handle */
        uint16_t attr_handle;           /*!< The attribute handle */
        esp_gatt_status_t status;       /*!< Operation status */
    } set_attr_val;                     /*!< Callback parameter for the event `ESP_GATTS_SET_ATTR_VAL_EVT` */

    /**
    * @brief Callback parameter for the event `ESP_GATTS_SEND_SERVICE_CHANGE_EVT`
    */
    struct gatts_send_service_change_evt_param{
        esp_gatt_status_t status;        /*!< Operation status */
    } service_change;                    /*!< Callback parameter for the event `ESP_GATTS_SEND_SERVICE_CHANGE_EVT` */

} esp_ble_gatts_cb_param_t;

/**
 * @brief GATT Server callback function type
 *
 * @param[in] event    Event type
 * @param[in] gatts_if GATT Server access interface. Typically, different `gatts_if` values correspond to different profiles.
 * @param[in] param    The pointer to the callback parameter, which is of a union type.
 */
typedef void (* esp_gatts_cb_t)(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

/**
 * @brief  Register GATT Server application callbacks
 *
 * @param[in] callback The pointer to the application callback function
 *
 * @note            Avoid performing time-consuming operations within the callback functions.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_FAIL: Failure
 */
esp_err_t esp_ble_gatts_register_callback(esp_gatts_cb_t callback);

/**
 * @brief  Get the current GATT Server application callback
 *
 * @return
 *        - esp_gatts_cb_t: Current callback
 */
esp_gatts_cb_t esp_ble_gatts_get_callback(void);

/**
 * @brief  Register GATT Server application
 *
 * @param[in] app_id: The UUID for different application
 *
 * @note
 *      1. This function triggers `ESP_GATTS_REG_EVT`.
 *      2. The maximum number of applications is limited to 6.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_ERR_INVALID_ARG: The input `app_id` exceeds `ESP_APP_ID_MAX` (0x7fff) defined in esp_bt_defs.h.
 *       - ESP_FAIL: Failure due to other reasons
 */
esp_err_t esp_ble_gatts_app_register(uint16_t app_id);

/**
 * @brief  Unregister an GATT Server application
 *
 * @param[in]  gatts_if GATT Server access interface.
 *
 * @note
 *      1. This function triggers `ESP_GATTS_UNREG_EVT`.
 *      2. The maximum number of applications is limited to 6.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_FAIL: Failure
 */
esp_err_t esp_ble_gatts_app_unregister(esp_gatt_if_t gatts_if);

/**
 * @brief  Create a GATT Server service
 *
 * @param[in]       gatts_if   GATT Server access interface
 * @param[in]       service_id The pointer to the Service ID
 * @param[in]       num_handle The number of handles requested for this service.
 *
 * @note
 *       1. This function triggers `ESP_GATTS_CREATE_EVT`.
 *       2. `num_handle` should not be greater than CONFIG_BT_GATT_MAX_SR_ATTRIBUTES.
 *
 * @return
 *        - ESP_OK: Success
 *        - ESP_FAIL: Failure
 */
esp_err_t esp_ble_gatts_create_service(esp_gatt_if_t gatts_if,
                                       esp_gatt_srvc_id_t *service_id, uint16_t num_handle);


/**
 * @brief  Create a service attribute table
 *
 * @param[in]       gatts_attr_db The pointer to the service attribute table
 * @param[in]       gatts_if      GATT Server access interface
 * @param[in]       max_nb_attr   The number of attributes to be added to the service database
 * @param[in]       srvc_inst_id  The instance ID of the service
 *
 * @note
 *      1. This function triggers `ESP_GATTS_CREAT_ATTR_TAB_EVT`.
 *      2. `max_nb_attr` should not be greater than CONFIG_BT_GATT_MAX_SR_ATTRIBUTES.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_ERR_INVALID_ARG: Invalid `max_nb_attr`
 *       - ESP_FAIL: Failure
 */
esp_err_t esp_ble_gatts_create_attr_tab(const esp_gatts_attr_db_t *gatts_attr_db,
                                            esp_gatt_if_t gatts_if,
                                            uint16_t max_nb_attr,
                                            uint8_t srvc_inst_id);
/**
 * @brief  Add an included service
 *
 * @param[in]       service_handle  Target service handle to add
 * @param[in]       included_service_handle The handle of included service to be added
 *
 * @note
 *       1. This function triggers `ESP_GATTS_ADD_INCL_SRVC_EVT`.
 *       2. This function has to be called between `esp_ble_gatts_create_service` and `esp_ble_gatts_add_char`.
 *
 * @return
 *        - ESP_OK: Success
 *        - ESP_FAIL: Failure
 */
esp_err_t esp_ble_gatts_add_included_service(uint16_t service_handle, uint16_t included_service_handle);

/**
 * @brief  Add a characteristic into a service.
 *
 * @param[in]       service_handle  Target service handle to add the characteristic
 * @param[in]       char_uuid       The pointer to the characteristic UUID
 * @param[in]       perm            Characteristic value declaration attribute permission
 * @param[in]       property        Characteristic Properties
 * @param[in]       char_val        The pointer to the characteristic value
 * @param[in]       control         The pointer to the attribute response control byte
 *
 * @note
 *      1. This function triggers `ESP_GATTS_ADD_CHAR_EVT`.
 *      2. `control->auto_rsp` should be set to `ESP_GATT_AUTO_RSP` or `ESP_GATT_RSP_BY_APP`.
 *      3. For stack respond attribute (`ESP_GATT_AUTO_RSP`), `char_val` should not be NULL and `char_val->attr_max_len` must be greater than 0.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_ERR_INVALID_ARG: Invalid arguments
 *       - ESP_FAIL: Failure due to other reasons
 */
esp_err_t esp_ble_gatts_add_char(uint16_t service_handle,  esp_bt_uuid_t  *char_uuid,
                                 esp_gatt_perm_t perm, esp_gatt_char_prop_t property, esp_attr_value_t *char_val,
                                 esp_attr_control_t *control);

/**
 * @brief  Add a characteristic descriptor
 *
 * @param[in]       service_handle Target service handle to add the characteristic descriptor
 * @param[in]       descr_uuid     The pointer to the descriptor UUID
 * @param[in]       perm           Descriptor access permission
 * @param[in]       char_descr_val The pointer to the characteristic descriptor value
 * @param[in]       control        The pointer to the attribute response control byte
 *
 * @note
 *      1. This function triggers `ESP_GATTS_ADD_CHAR_DESCR_EVT`.
 *      2. `control->auto_rsp` should be set to `ESP_GATT_AUTO_RSP` or `ESP_GATT_RSP_BY_APP`.
 *      3. For stack respond attribute (`ESP_GATT_AUTO_RSP`), `char_val` should not be NULL and `char_val->attr_max_len` must be greater than 0.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_ERR_INVALID_ARG: Invalid arguments
 *       - ESP_FAIL: Failure due to other reasons
 */
esp_err_t esp_ble_gatts_add_char_descr (uint16_t service_handle,
                                        esp_bt_uuid_t   *descr_uuid,
                                        esp_gatt_perm_t perm, esp_attr_value_t *char_descr_val,
                                        esp_attr_control_t *control);

/**
 * @brief  Delete a service
 *
 * @param[in]       service_handle Target service handle to delete
 *
 * @note    This function triggers `ESP_GATTS_DELETE_EVT`.
 *
 * @return
 *         - ESP_OK: Success
 *         - ESP_FAIL: Failure
 */
esp_err_t esp_ble_gatts_delete_service(uint16_t service_handle);

/**
 * @brief  Start a service
 *
 * @param[in]       service_handle Target service handle to start
 *
 * @note      This function triggers `ESP_GATTS_START_EVT`.
 *
 * @return
 *         - ESP_OK: Success
 *         - ESP_FAIL: Failure
 */
esp_err_t esp_ble_gatts_start_service(uint16_t service_handle);

/**
 * @brief  Stop a service.
 *
 * @param[in]       service_handle Target service handle to stop
 *
 * @note            This function triggers `ESP_GATTS_STOP_EVT`.
 *
 * @return
 *          - ESP_OK: Success
 *          - ESP_FAIL: Failure
 */
esp_err_t esp_ble_gatts_stop_service(uint16_t service_handle);

/**
 * @brief  Send indication or notification to a GATT Client
 *
 * @param[in]       gatts_if    GATT Server access interface
 * @param[in]       conn_id     Connection ID
 * @param[in]       attr_handle Attribute handle to indicate
 * @param[in]       value_len   Indication value length in bytes
 * @param[in]       value       Value to indicate
 * @param[in]       need_confirm True if a confirmation is required, which is a GATT indication; false if the confirmation is not required, which is a GATT notification.
 *
 * @note
 *       1. This function triggers `ESP_GATTS_CONF_EVT`.
 *       2. The size of indication or notification data must be less than or equal to MTU size, see `esp_ble_gattc_send_mtu_req`.
 *       3. This function should be called only after the connection has been established.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_ERR_INVALID_STATE: The connection has not been established.
 *       - ESP_FAIL: Failure due to other reasons
 */
esp_err_t esp_ble_gatts_send_indicate(esp_gatt_if_t gatts_if, uint16_t conn_id, uint16_t attr_handle,
                                      uint16_t value_len, uint8_t *value, bool need_confirm);

/**
 * @brief  Send a response to a request
 *
 * @param[in]       gatts_if GATT Server access interface
 * @param[in]       conn_id  Connection ID
 * @param[in]       trans_id Transfer ID
 * @param[in]       status   Response status
 * @param[in]       rsp      The pointer to the response data
 *
 * @note
 *      1. This function triggers `ESP_GATTS_RESPONSE_EVT`.
 *      2. This function should be called only after the connection has been established.
 *
 * @return
 *        - ESP_OK: Success
 *        - ESP_ERR_INVALID_STATE: The connection has not been established.
 *        - ESP_FAIL: Failure due to other reasons
 */
esp_err_t esp_ble_gatts_send_response(esp_gatt_if_t gatts_if, uint16_t conn_id, uint32_t trans_id,
                                      esp_gatt_status_t status, esp_gatt_rsp_t *rsp);

/**
 * @brief  Set the attribute value
 *
 * @param[in]       attr_handle Target attribute handle to set the value
 * @param[in]       length      The value length in bytes
 * @param[in]       value       The pointer to the attribute value
 *
 * @note    This function triggers `ESP_GATTS_SET_ATTR_VAL_EVT`.
 *
 * @return
 *          - ESP_OK: Success
 *          - ESP_FAIL: Failure
 */
esp_err_t esp_ble_gatts_set_attr_value(uint16_t attr_handle, uint16_t length, const uint8_t *value);

/**
 * @brief  Retrieve attribute value
 *
 * @param[in]   attr_handle Attribute handle
 * @param[out]  length      The pointer to the attribute value length in bytes
 * @param[out]  value       The pointer to attribute value payload. This value cannot be modified by user.
 *
 * @note
 *      1. This function does not trigger any event.
 *      2. `attr_handle` must be greater than 0.
 *
 * @return
 *        - ESP_OK: Success
 *        - ESP_GATT_INVALID_HANDLE: Invalid `attr_handle`
 *        - ESP_FAIL: Failure due to other reasons
 */
esp_gatt_status_t esp_ble_gatts_get_attr_value(uint16_t attr_handle, uint16_t *length, const uint8_t **value);

/**
 * @brief  Create an ACL connection when `BT_BLE_42_FEATURES_SUPPORTED` is enabled in the menuconfig
 *
 * @param[in]       gatts_if   GATT Server access interface
 * @param[in]       remote_bda Remote  device address
 * @param[in]       is_direct `True` indicates a direct connection, while `False` indicates a background auto connection. Currently, background auto connection is not supported, so please always set this parameter to True.
 *
 * @note
 *      1. The function always triggers `ESP_GATTS_CONNECT_EVT` and `ESP_GATTS_OPEN_EVT`.
 *      2. When the device acts as GATT Server, besides the above two events, this function triggers `ESP_GATTS_CONNECT_EVT` as well.
 *      3. This function will establish an ACL connection as a Central and a virtual connection as a GATT Server. If the ACL connection already exists, it will create a virtual connection only.
 *
 * @return
 *      - ESP_OK: Success
 *      - ESP_FAIL: Failure
 */
esp_err_t esp_ble_gatts_open(esp_gatt_if_t gatts_if, esp_bd_addr_t remote_bda, bool is_direct);

/**
 * @brief  Close a connection with a remote device
 *
 * @param[in]       gatts_if GATT Server access interface
 * @param[in]       conn_id  Connection ID to be closed
 *
 * @note
 *      1. This function triggers `ESP_GATTS_CLOSE_EVT`.
 *      2. There may be multiple virtual GATT server connections when multiple `app_id` got registered.
 *      3. This API closes one virtual GATT server connection only, if there exist other virtual GATT server connections. It does not close the physical connection.
 *      4. The API `esp_ble_gap_disconnect` can be used to disconnect the physical connection directly.
 *      5. If there is only one virtual GATT connection left, this API will terminate the ACL connection in addition, and trigger `ESP_GATTS_DISCONNECT_EVT`. Then there is no need to call `esp_ble_gap_disconnect` anymore.
 *
 * @return
 *      - ESP_OK: Success
 *      - ESP_FAIL: Failure
 */
esp_err_t esp_ble_gatts_close(esp_gatt_if_t gatts_if, uint16_t conn_id);

/**
 * @brief  Send service change indication
 *
 * @param[in]       gatts_if   GATT Server access interface
 * @param[in]       remote_bda Remote device address.
 *                  If remote_bda is NULL then it will send service change
 *                  indication to all the connected devices and if not then
 *                  to a specific device.
 *
 * @note This function triggers `ESP_GATTS_SEND_SERVICE_CHANGE_EVT`.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_FAIL: Failure
 */
esp_err_t esp_ble_gatts_send_service_change_indication(esp_gatt_if_t gatts_if, esp_bd_addr_t remote_bda);

/**
 * @brief   Display the Server's local attribute database.
 *
 * This API prints the local attribute database of the BLE server, including details
 * of all services, characteristics, and descriptors.
 *
 * @note
 *    1. This function does not trigger any event.
 *    2. It is primarily intended for debugging purposes to verify the server's current configuration.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_FAIL: Failure
 */
esp_err_t esp_ble_gatts_show_local_database(void);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_GATTS_API_H__ */
