/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: Blake Felt
 */

#ifndef __ESP_HIDD_API_H__
#define __ESP_HIDD_API_H__

#include "esp_bt_defs.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/// subclass of hid device
#define ESP_HID_CLASS_UNKNOWN      (0x00<<2)           /*!< unknown HID device subclass */
#define ESP_HID_CLASS_JOS          (0x01<<2)           /*!< joystick */
#define ESP_HID_CLASS_GPD          (0x02<<2)           /*!< game pad */
#define ESP_HID_CLASS_RMC          (0x03<<2)           /*!< remote control */
#define ESP_HID_CLASS_SED          (0x04<<2)           /*!< sensing device */
#define ESP_HID_CLASS_DGT          (0x05<<2)           /*!< digitizer tablet */
#define ESP_HID_CLASS_CDR          (0x06<<2)           /*!< card reader */
#define ESP_HID_CLASS_KBD          (0x10<<2)           /*!< keyboard */
#define ESP_HID_CLASS_MIC          (0x20<<2)           /*!< pointing device */
#define ESP_HID_CLASS_COM          (0x30<<2)           /*!< combo keyboard/pointing */

/**
 * @brief HIDD handshake result code
 */
typedef enum {
    ESP_HID_PAR_HANDSHAKE_RSP_SUCCESS = 0,                 /*!< successful */
    ESP_HID_PAR_HANDSHAKE_RSP_NOT_READY = 1,               /*!< not ready, device is too busy to accept data */
    ESP_HID_PAR_HANDSHAKE_RSP_ERR_INVALID_REP_ID = 2,      /*!< invalid report ID */
    ESP_HID_PAR_HANDSHAKE_RSP_ERR_UNSUPPORTED_REQ = 3,     /*!< device does not support the request */
    ESP_HID_PAR_HANDSHAKE_RSP_ERR_INVALID_PARAM = 4,       /*!< parameter value is out of range or inappropriate */
    ESP_HID_PAR_HANDSHAKE_RSP_ERR_UNKNOWN = 14,            /*!< device could not identify the error condition */
    ESP_HID_PAR_HANDSHAKE_RSP_ERR_FATAL = 15,              /*!< restart is essential to resume functionality */
} esp_hidd_handshake_error_t;

/**
 * @brief HIDD report types
 */
typedef enum {
    ESP_HIDD_REPORT_TYPE_OTHER = 0,                  /*!< unknown report type */
    ESP_HIDD_REPORT_TYPE_INPUT,                      /*!< input report */
    ESP_HIDD_REPORT_TYPE_OUTPUT,                     /*!< output report */
    ESP_HIDD_REPORT_TYPE_FEATURE,                    /*!< feature report */
    ESP_HIDD_REPORT_TYPE_INTRDATA,                   /*!< special value for reports to be sent on interrupt channel, INPUT is assumed */
} esp_hidd_report_type_t;

/**
 * @brief HIDD connection state
 */
typedef enum {
    ESP_HIDD_CONN_STATE_CONNECTED,                   /*!< HID connection established */
    ESP_HIDD_CONN_STATE_CONNECTING,                  /*!< connection to remote Bluetooth device */
    ESP_HIDD_CONN_STATE_DISCONNECTED,                /*!< connection released */
    ESP_HIDD_CONN_STATE_DISCONNECTING,               /*!< disconnecting to remote Bluetooth device*/
    ESP_HIDD_CONN_STATE_UNKNOWN,                     /*!< unknown connection state */
} esp_hidd_connection_state_t;

/**
 * @brief HID device protocol modes
 */
typedef enum {
    ESP_HIDD_REPORT_MODE = 0x00,                     /*!< Report Protocol Mode */
    ESP_HIDD_BOOT_MODE = 0x01,                       /*!< Boot Protocol Mode */
    ESP_HIDD_UNSUPPORTED_MODE = 0xff,                /*!< unsupported */
} esp_hidd_protocol_mode_t;

/**
 * @brief HID Boot Protocol report IDs
 */
typedef enum {
    ESP_HIDD_BOOT_REPORT_ID_KEYBOARD = 1,            /*!< report ID of Boot Protocol keyboard report */
    ESP_HIDD_BOOT_REPORT_ID_MOUSE = 2,               /*!< report ID of Boot Protocol mouse report */
} esp_hidd_boot_report_id_t;

/**
 * @brief HID Boot Protocol report size including report ID
 */
enum {
    ESP_HIDD_BOOT_REPORT_SIZE_KEYBOARD = 9,          /*!< report size of Boot Protocol keyboard report */
    ESP_HIDD_BOOT_REPORT_SIZE_MOUSE = 4,             /*!< report size of Boot Protocol mouse report */
};

/**
 * @brief HID device characteristics for SDP server
 */
typedef struct {
    const char *name;                                /*!< service name */
    const char *description;                         /*!< service description */
    const char *provider;                            /*!< provider name */
    uint8_t subclass;                                /*!< HID device subclass */
    uint8_t *desc_list;                              /*!< HID descriptor list */
    int desc_list_len;                               /*!< size in bytes of HID descriptor list */
} esp_hidd_app_param_t;

/**
 * @brief HIDD Quality of Service parameters negotiated over L2CAP
 */
typedef struct {
    uint8_t service_type;                            /*!< the level of service, 0 indicates no traffic */
    uint32_t token_rate;                             /*!< token rate in bytes per second, 0 indicates "don't care" */
    uint32_t token_bucket_size;                      /*!< limit on the burstness of the application data */
    uint32_t peak_bandwidth;                         /*!< bytes per second, value 0 indicates "don't care" */
    uint32_t access_latency;                         /*!< maximum acceptable delay in microseconds */
    uint32_t delay_variation;                        /*!< the difference in microseconds between the max and min delay */
} esp_hidd_qos_param_t;

/**
 * @brief HID device callback function events
 */
typedef enum {
    ESP_HIDD_INIT_EVT = 0,       /*!< When HID device is initialized, the event comes */
    ESP_HIDD_DEINIT_EVT,         /*!< When HID device is deinitialized, the event comes */
    ESP_HIDD_REGISTER_APP_EVT,   /*!< When HID device application registered, the event comes */
    ESP_HIDD_UNREGISTER_APP_EVT, /*!< When HID device application unregistered, the event comes */
    ESP_HIDD_OPEN_EVT,           /*!< When HID device connection to host opened, the event comes */
    ESP_HIDD_CLOSE_EVT,          /*!< When HID device connection to host closed, the event comes */
    ESP_HIDD_SEND_REPORT_EVT,    /*!< When HID device send report to lower layer, the event comes */
    ESP_HIDD_REPORT_ERR_EVT,     /*!< When HID device report handshanke error to lower layer, the event comes */
    ESP_HIDD_GET_REPORT_EVT,     /*!< When HID device receives GET_REPORT request from host, the event comes */
    ESP_HIDD_SET_REPORT_EVT,     /*!< When HID device receives SET_REPORT request from host, the event comes */
    ESP_HIDD_SET_PROTOCOL_EVT,   /*!< When HID device receives SET_PROTOCOL request from host, the event comes */
    ESP_HIDD_INTR_DATA_EVT,      /*!< When HID device receives DATA from host on intr, the event comes */
    ESP_HIDD_VC_UNPLUG_EVT,      /*!< When HID device initiates Virtual Cable Unplug, the event comes */
    ESP_HIDD_API_ERR_EVT         /*!< When HID device has API error, the event comes */
} esp_hidd_cb_event_t;

typedef enum {
    ESP_HIDD_SUCCESS,
    ESP_HIDD_ERROR,         /*!< general ESP HD error */
    ESP_HIDD_NO_RES,        /*!< out of system resources */
    ESP_HIDD_BUSY,          /*!< Temporarily can not handle this request. */
    ESP_HIDD_NO_DATA,       /*!< No data. */
    ESP_HIDD_NEED_INIT,     /*!< HIDD module shall init first */
    ESP_HIDD_NEED_DEINIT,   /*!< HIDD module shall deinit first */
    ESP_HIDD_NEED_REG,      /*!< HIDD module shall register first */
    ESP_HIDD_NEED_DEREG,    /*!< HIDD module shall deregister first */
    ESP_HIDD_NO_CONNECTION, /*!< connection may have been closed */
} esp_hidd_status_t;

/**
 * @brief HID device profile status parameters
 */
typedef struct {
    bool hidd_inited;                      /*!< HID device initialization */
    uint8_t conn_num;                      /*!< Number of connections */
    uint8_t plug_vc_dev_num;               /*!< Number of plugged virtual cable devices */
    uint8_t reg_app_num;                   /*!< Number of HID device application registrations */
} esp_hidd_profile_status_t;

/**
 * @brief HID device callback parameters union
 */
typedef union {
    /**
     * @brief ESP_HIDD_INIT_EVT
     */
    struct hidd_init_evt_param {
        esp_hidd_status_t status; /*!< operation status */
    } init;                       /*!< HIDD callback param of ESP_HIDD_INIT_EVT */

    /**
     * @brief ESP_HIDD_DEINIT_EVT
     */
    struct hidd_deinit_evt_param {
        esp_hidd_status_t status; /*!< operation status */
    } deinit;                     /*!< HIDD callback param of ESP_HIDD_DEINIT_EVT */

    /**
     * @brief ESP_HIDD_REGISTER_APP_EVT
     */
    struct hidd_register_app_evt_param {
        esp_hidd_status_t status; /*!< operation status */
        bool in_use;              /*!< indicate whether use virtual cable plug host address */
        esp_bd_addr_t bd_addr;    /*!< host address */
    } register_app;               /*!< HIDD callback param of ESP_HIDD_REGISTER_APP_EVT */

    /**
     * @brief ESP_HIDD_UNREGISTER_APP_EVT
     */
    struct hidd_unregister_app_evt_param {
        esp_hidd_status_t status; /*!< operation status         */
    } unregister_app;             /*!< HIDD callback param of ESP_HIDD_UNREGISTER_APP_EVT */

    /**
     * @brief ESP_HIDD_OPEN_EVT
     */
    struct hidd_open_evt_param {
        esp_hidd_status_t status;                /*!< operation status         */
        esp_hidd_connection_state_t conn_status; /*!< connection status */
        esp_bd_addr_t bd_addr;                   /*!< host address */
    } open;                                      /*!< HIDD callback param of ESP_HIDD_OPEN_EVT */

    /**
     * @brief ESP_HIDD_CLOSE_EVT
     */
    struct hidd_close_evt_param {
        esp_hidd_status_t status;                /*!< operation status         */
        esp_hidd_connection_state_t conn_status; /*!< connection status        */
    } close;                                     /*!< HIDD callback param of ESP_HIDD_CLOSE_EVT */

    /**
     * @brief ESP_HIDD_SEND_REPORT_EVT
     */
    struct hidd_send_report_evt_param {
        esp_hidd_status_t status;           /*!< operation status         */
        uint8_t reason;                     /*!< lower layer failed reason(ref hiddefs.h)       */
        esp_hidd_report_type_t report_type; /*!< report type        */
        uint8_t report_id;                  /*!< report id         */
    } send_report;                          /*!< HIDD callback param of ESP_HIDD_SEND_REPORT_EVT */

    /**
     * @brief ESP_HIDD_REPORT_ERR_EVT
     */
    struct hidd_report_err_evt_param {
        esp_hidd_status_t status; /*!< operation status         */
        uint8_t reason;           /*!< lower layer failed reason(ref hiddefs.h)           */
    } report_err;                 /*!< HIDD callback param of ESP_HIDD_REPORT_ERR_EVT */

    /**
     * @brief ESP_HIDD_GET_REPORT_EVT
     */
    struct hidd_get_report_evt_param {
        esp_hidd_report_type_t report_type; /*!< report type        */
        uint8_t report_id;                  /*!< report id         */
        uint16_t buffer_size;               /*!< buffer size         */
    } get_report;                           /*!< HIDD callback param of ESP_HIDD_GET_REPORT_EVT */

    /**
     * @brief ESP_HIDD_SET_REPORT_EVT
     */
    struct hidd_set_report_evt_param {
        esp_hidd_report_type_t report_type; /*!< report type        */
        uint8_t report_id;                  /*!< report id         */
        uint16_t len;                       /*!< set_report data length         */
        uint8_t *data;                      /*!< set_report data pointer         */
    } set_report;                           /*!< HIDD callback param of ESP_HIDD_SET_REPORT_EVT */

    /**
     * @brief ESP_HIDD_SET_PROTOCOL_EVT
     */
    struct hidd_set_protocol_evt_param {
        esp_hidd_protocol_mode_t protocol_mode; /*!< protocol mode        */
    } set_protocol;                             /*!< HIDD callback param of ESP_HIDD_SET_PROTOCOL_EVT */

    /**
     * @brief ESP_HIDD_INTR_DATA_EVT
     */
    struct hidd_intr_data_evt_param {
        uint8_t report_id; /*!< interrupt channel report id         */
        uint16_t len;      /*!< interrupt channel report data length         */
        uint8_t *data;     /*!< interrupt channel report data pointer         */
    } intr_data;           /*!< HIDD callback param of ESP_HIDD_INTR_DATA_EVT */

    /**
     * @brief ESP_HIDD_VC_UNPLUG_EVT
     */
    struct hidd_vc_unplug_param {
        esp_hidd_status_t status;                /*!< operation status         */
        esp_hidd_connection_state_t conn_status; /*!< connection status        */
    } vc_unplug;                                 /*!< HIDD callback param of ESP_HIDD_VC_UNPLUG_EVT */
} esp_hidd_cb_param_t;

/**
 * @brief           HID device callback function type.
 * @param           event: Event type
 * @param           param: Point to callback parameter, currently is union type
 */
typedef void (*esp_hd_cb_t)(esp_hidd_cb_event_t event, esp_hidd_cb_param_t *param);

/**
 * @brief           This function is called to init callbacks with HID device module.
 *
 * @param[in]       callback: pointer to the init callback function.
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_bt_hid_device_register_callback(esp_hd_cb_t callback);

/**
 * @brief           Initializes HIDD interface. This function should be called after
 *                  esp_bluedroid_init()/esp_bluedroid_init_with_cfg() and esp_bluedroid_enable() success, and should be
 *                  called after esp_bt_hid_device_register_callback. When the operation is complete, the callback
 *                  function will be called with ESP_HIDD_INIT_EVT.
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_bt_hid_device_init(void);

/**
 * @brief           De-initializes HIDD interface. This function should be called after
 *                  esp_bluedroid_init()/esp_bluedroid_init_with_cfg() and esp_bluedroid_enable() success, and should be
 *                  called after esp_bt_hid_device_init(). When the operation is complete, the callback function will be
 *                  called with ESP_HIDD_DEINIT_EVT.
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_bt_hid_device_deinit(void);

/**
 * @brief           Registers HIDD parameters with SDP and sets l2cap Quality of Service. This function should be
 *                  called after esp_bluedroid_init()/esp_bluedroid_init_with_cfg() and esp_bluedroid_enable() success,
 *                  and should be called after esp_bt_hid_device_init(). When the operation is complete, the callback
 *                  function will be called with ESP_HIDD_REGISTER_APP_EVT.
 *
 * @param[in]       app_param: HIDD parameters
 * @param[in]       in_qos: incoming QoS parameters
 * @param[in]       out_qos: outgoing QoS parameters
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_bt_hid_device_register_app(esp_hidd_app_param_t *app_param, esp_hidd_qos_param_t *in_qos,
                                         esp_hidd_qos_param_t *out_qos);

/**
 * @brief           Removes HIDD parameters from SDP and resets l2cap Quality of Service. This function should be
 *                  called after esp_bluedroid_init()/esp_bluedroid_init_with_cfg() and esp_bluedroid_enable() success,
 *                  and should be called after esp_bt_hid_device_init(). When the operation is complete, the callback
 *                  function will be called with ESP_HIDD_UNREGISTER_APP_EVT.
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_bt_hid_device_unregister_app(void);

/**
 * @brief           Connects to the peer HID Host with virtual cable. This function should be called after
 *                  esp_bluedroid_init()/esp_bluedroid_init_with_cfg() and esp_bluedroid_enable() success, and should be
 *                  called after esp_bt_hid_device_init(). When the operation is complete, the callback function will
 *                  be called with ESP_HIDD_OPEN_EVT.
 *
 * @note            The connection between the HID Host and the HID Device is established as a virtual cable by default.
 *                  A new HID Host connection request will only be accepted after the previous HID Host has been
 *                  explicitly unplugged. For details on disconnection and virtual cable unplugging, please refer to API
 *                  `esp_bt_hid_device_disconnect` and `esp_bt_hid_device_virtual_cable_unplug`.
 *
 * @param[in]       bd_addr: Remote host bluetooth device address.
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_bt_hid_device_connect(esp_bd_addr_t bd_addr);

/**
 * @brief           Disconnects from the currently connected HID Host. This function should be called after
 *                  esp_bluedroid_init()/esp_bluedroid_init_with_cfg() and esp_bluedroid_enable() success, and should be
 *                  called after esp_bt_hid_device_init(). When the operation is complete, the callback function will
 *                  be called with ESP_HIDD_CLOSE_EVT.
 *
 * @note            The disconnect operation will not remove the virtually cabled device. If the connect request from the
 *                  different HID Host, it will reject the request.
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_bt_hid_device_disconnect(void);

/**
 * @brief           Sends HID report to the currently connected HID Host. This function should be called after
 *                  esp_bluedroid_init()/esp_bluedroid_init_with_cfg() and esp_bluedroid_enable() success, and should be
 *                  called after esp_bt_hid_device_init(). When the operation is complete, the callback function will
 *                  be called with ESP_HIDD_SEND_REPORT_EVT.
 *
 * @param[in]       type: type of report
 * @param[in]       id: report id as defined by descriptor
 * @param[in]       len: length of report
 * @param[in]       data: report data
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_bt_hid_device_send_report(esp_hidd_report_type_t type, uint8_t id, uint16_t len, uint8_t *data);

/**
 * @brief           Sends HID Handshake with error info for invalid set_report to the currently connected HID Host.
 *                  This function should be called after esp_bluedroid_init()/esp_bluedroid_init_with_cfg() and
 *                  esp_bluedroid_enable() success, and should be called after esp_bt_hid_device_init(). When the
 *                  operation is complete, the callback function will be called with ESP_HIDD_REPORT_ERR_EVT.
 *
 * @param[in]       error: type of error
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_bt_hid_device_report_error(esp_hidd_handshake_error_t error);

/**
 * @brief           Remove the virtually cabled device. This function should be called after
 *                  esp_bluedroid_init()/esp_bluedroid_init_with_cfg() and esp_bluedroid_enable() success, and should be
 *                  called after esp_bt_hid_device_init(). When the operation is complete, the callback function will be
 *                  called with ESP_HIDD_VC_UNPLUG_EVT.
 *
 * @note            If the connection exists, then HID Device will send a `VIRTUAL_CABLE_UNPLUG` control command to
 *                  the peer HID Host, and the connection will be destroyed. If the connection does not exist, then HID
 *                  Device will only unplug on it's single side. Once the unplug operation is success, the related
 *                  pairing and bonding information will be removed, then the HID Device can accept connection request
 *                  from the different HID Host,
 *
 * @return          - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_bt_hid_device_virtual_cable_unplug(void);

/**
 * @brief       This function is used to get the status of hid device
 *
 * @param[out]  profile_status - HID device status
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_bt_hid_device_get_profile_status(esp_hidd_profile_status_t *profile_status);

#ifdef __cplusplus
}
#endif

#endif
