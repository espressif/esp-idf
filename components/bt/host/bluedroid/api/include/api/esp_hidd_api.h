// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
// Copyright 2019      Blake Felt
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

#ifndef __ESP_HIDD_API_H__
#define __ESP_HIDD_API_H__

#include "esp_bt_defs.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* sub_class of hid device */
#define ESP_HID_CLASS_UNKNOWN      (0x00<<2)
#define ESP_HID_CLASS_JOS          (0x01<<2)           /* joy stick */
#define ESP_HID_CLASS_GPD          (0x02<<2)           /* game pad */
#define ESP_HID_CLASS_RMC          (0x03<<2)           /* remote control */
#define ESP_HID_CLASS_SED          (0x04<<2)           /* sensing device */
#define ESP_HID_CLASS_DGT          (0x05<<2)           /* Digitizer tablet */
#define ESP_HID_CLASS_CDR          (0x06<<2)           /* card reader */
#define ESP_HID_CLASS_KBD          (0x10<<2)           /* keyboard */
#define ESP_HID_CLASS_MIC          (0x20<<2)           /* pointing device */
#define ESP_HID_CLASS_COM          (0x30<<2)           /* Combo keyboard/pointing */

/**
 * @brief HIDD handshake error
 */
typedef enum {
    ESP_HID_PAR_HANDSHAKE_RSP_SUCCESS = 0,
    ESP_HID_PAR_HANDSHAKE_RSP_NOT_READY = 1,
    ESP_HID_PAR_HANDSHAKE_RSP_ERR_INVALID_REP_ID = 2,
    ESP_HID_PAR_HANDSHAKE_RSP_ERR_UNSUPPORTED_REQ = 3,
    ESP_HID_PAR_HANDSHAKE_RSP_ERR_INVALID_PARAM = 4,
    ESP_HID_PAR_HANDSHAKE_RSP_ERR_UNKNOWN = 14,
    ESP_HID_PAR_HANDSHAKE_RSP_ERR_FATAL = 15
} esp_hidd_handshake_error_t;

/**
 * @brief HIDD report types
 */
typedef enum {
    ESP_HIDD_REPORT_TYPE_OTHER = 0,
    ESP_HIDD_REPORT_TYPE_INPUT,
    ESP_HIDD_REPORT_TYPE_OUTPUT,
    ESP_HIDD_REPORT_TYPE_FEATURE,
    // special value for reports to be sent on INTR(INPUT is assumed)
    ESP_HIDD_REPORT_TYPE_INTRDATA
} esp_hidd_report_type_t;

/**
 * @brief HIDD connection state
 */
typedef enum {
    ESP_HIDD_CONN_STATE_CONNECTED,
    ESP_HIDD_CONN_STATE_CONNECTING,
    ESP_HIDD_CONN_STATE_DISCONNECTED,
    ESP_HIDD_CONN_STATE_DISCONNECTING,
    ESP_HIDD_CONN_STATE_UNKNOWN
} esp_hidd_connection_state_t;

/**
 * @brief HID device protocol modes
 */
typedef enum {
    ESP_HIDD_REPORT_MODE = 0x00,
    ESP_HIDD_BOOT_MODE = 0x01,
    ESP_HIDD_UNSUPPORTED_MODE = 0xff
} esp_hidd_protocol_mode_t;


/**
 * @brief HIDD characteristics for SDP report
 */
typedef struct {
    const char *name;
    const char *description;
    const char *provider;
    uint8_t subclass;
    uint8_t *desc_list;
    int desc_list_len;
} esp_hidd_app_param_t;

/**
 * @brief HIDD Quality of Service parameters
 */
typedef struct {
    uint8_t service_type;
    uint32_t token_rate;
    uint32_t token_bucket_size;
    uint32_t peak_bandwidth;
    uint32_t access_latency;
    uint32_t delay_variation;
} esp_hidd_qos_param_t;

/**
 * @brief HID device callback function events
 */
typedef enum {
    ESP_HIDD_INIT_EVT = 0,       /*!< When HID device is inited, the event comes */
    ESP_HIDD_DEINIT_EVT,         /*!< When HID device is deinited, the event comes */
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
 * @brief       HID device callback function type.
 * @param       event:      Event type
 * @param       param:      Point to callback parameter, currently is union type
 */
typedef void (esp_hd_cb_t)(esp_hidd_cb_event_t event, esp_hidd_cb_param_t *param);

/**
 * @brief       This function is called to init callbacks with HID device module.
 *
 * @param[in]   callback:   pointer to the init callback function.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_bt_hid_device_register_callback(esp_hd_cb_t callback);

/**
 * @brief       This function initializes HIDD. This function should be called after esp_bluedroid_enable and
 *              esp_blueroid_init success, and should be called after esp_bt_hid_device_register_callback.
 *              When the operation is complete the callback function will be called with ESP_HIDD_INIT_EVT.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_bt_hid_device_init(void);

/**
 * @brief       This function de-initializes HIDD interface. This function should be called after esp_bluedroid_enable() and
 *              esp_blueroid_init() success, and should be called after esp_bt_hid_device_init(). When the operation is complete the callback
 *              function will be called with ESP_HIDD_DEINIT_EVT.
 *
 * @return    - ESP_OK: success
 *            - other: failed
 */
esp_err_t esp_bt_hid_device_deinit(void);

/**
 * @brief     Registers HIDD parameters with SDP and sets l2cap Quality of Service. This function should be called after
 *            esp_bluedroid_enable and esp_blueroid_init success, and must be done after esp_bt_hid_device_init. When the operation is complete the callback
 *            function will be called with ESP_HIDD_REGISTER_APP_EVT.
 *
 * @param[in] app_param:  HIDD parameters
 * @param[in] in_qos:     incoming QoS parameters
 * @param[in] out_qos:    outgoing QoS parameters
 *
 * @return    - ESP_OK: success
 *            - other: failed
 */
esp_err_t esp_bt_hid_device_register_app(esp_hidd_app_param_t *app_param, esp_hidd_qos_param_t *in_qos,
                                         esp_hidd_qos_param_t *out_qos);

/**
 * @brief   Removes HIDD parameters from SDP and resets l2cap Quality of Service. This function should be called after esp_bluedroid_enable and
 *          esp_blueroid_init success, and should be called after esp_bt_hid_device_init. When the operation is complete the callback
 *          function will be called with ESP_HIDD_UNREGISTER_APP_EVT.
 *
 * @return  - ESP_OK: success
 *          - other: failed
 */
esp_err_t esp_bt_hid_device_unregister_app(void);

/**
 * @brief     This function connects HIDD interface to connected bluetooth device, if not done already. When the operation is complete the callback
 *            function will be called with ESP_HIDD_OPEN_EVT.
 *
 * @param[in] bd_addr:      Remote host bluetooth device address.
 *
 * @return
 *            - ESP_OK: success
 *            - other: failed
 */
esp_err_t esp_bt_hid_device_connect(esp_bd_addr_t bd_addr);

/**
 * @brief     This function disconnects HIDD interface. When the operation is complete the callback
 *            function will be called with ESP_HIDD_CLOSE_EVT.
 *
 * @return
 *            - ESP_OK: success
 *            - other: failed
 */
esp_err_t esp_bt_hid_device_disconnect(void);

/**
 * @brief     Send HIDD report. When the operation is complete the callback
 *            function will be called with ESP_HIDD_SEND_REPORT_EVT.
 *
 * @param[in] type:   type of report
 * @param[in] id:     report id as defined by descriptor
 * @param[in] len:    length of report
 * @param[in] data:   report data
 *
 * @return
 *            - ESP_OK: success
 *            - other: failed
 */
esp_err_t esp_bt_hid_device_send_report(esp_hidd_report_type_t type, uint8_t id, uint16_t len, uint8_t *data);

/**
 * @brief     Sends HIDD handshake with error info for invalid set_report. When the operation is complete the callback
 *            function will be called with ESP_HIDD_REPORT_ERR_EVT.
 *
 * @param[in] error: type of error
 *
 * @return    - ESP_OK: success
 *            - other: failed
 */
esp_err_t esp_bt_hid_device_report_error(esp_hidd_handshake_error_t error);

/**
 * @brief     Unplug virtual cable of HIDD. When the operation is complete the callback
 *            function will be called with ESP_HIDD_VC_UNPLUG_EVT.
 *
 * @return    - ESP_OK: success
 *            - other: failed
 */
esp_err_t esp_bt_hid_device_virtual_cable_unplug(void);

#ifdef __cplusplus
}
#endif

#endif
