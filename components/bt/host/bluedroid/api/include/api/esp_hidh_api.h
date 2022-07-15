/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: Blake Felt
 */

#ifndef __ESP_HIDH_API_H__
#define __ESP_HIDH_API_H__

#include "esp_bt_defs.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BTHH_MAX_DSC_LEN 884

/**
 * @brief HID host connection state
 */
typedef enum {
    ESP_HIDH_CONN_STATE_CONNECTED = 0,           /*!< connected state */
    ESP_HIDH_CONN_STATE_CONNECTING,              /*!< connecting state */
    ESP_HIDH_CONN_STATE_DISCONNECTED,            /*!< disconnected state */
    ESP_HIDH_CONN_STATE_DISCONNECTING,           /*!< disconnecting state */
    ESP_HIDH_CONN_STATE_UNKNOWN                  /*!< unknown state(initial state) */
} esp_hidh_connection_state_t;

typedef enum {
    ESP_HIDH_OK,
    ESP_HIDH_HS_HID_NOT_READY,  /*!< handshake error : device not ready */
    ESP_HIDH_HS_INVALID_RPT_ID, /*!< handshake error : invalid report ID */
    ESP_HIDH_HS_TRANS_NOT_SPT,  /*!< handshake error : transaction not spt */
    ESP_HIDH_HS_INVALID_PARAM,  /*!< handshake error : invalid paremeter */
    ESP_HIDH_HS_ERROR,          /*!< handshake error : unspecified HS error */
    ESP_HIDH_ERR,               /*!< general ESP HH error */
    ESP_HIDH_ERR_SDP,           /*!< SDP error */
    ESP_HIDH_ERR_PROTO,         /*!< SET_Protocol error,
                                  only used in ESP_HIDH_OPEN_EVT callback */

    ESP_HIDH_ERR_DB_FULL,       /*!< device database full error, used in
                                     ESP_HIDH_OPEN_EVT/ESP_HIDH_ADD_DEV_EVT */
    ESP_HIDH_ERR_TOD_UNSPT,     /*!< type of device not supported */
    ESP_HIDH_ERR_NO_RES,        /*!< out of system resources */
    ESP_HIDH_ERR_AUTH_FAILED,   /*!< authentication fail */
    ESP_HIDH_ERR_HDL,           /*!< connection handle error */
    ESP_HIDH_ERR_SEC,           /*!< encryption error */
    // self_defined
    ESP_HIDH_BUSY,              /*!< Temporarily can not handle this request. */
    ESP_HIDH_NO_DATA,           /*!< No data. */
    ESP_HIDH_NEED_INIT,         /*!< HIDH module shall init first */
    ESP_HIDH_NEED_DEINIT,       /*!< HIDH module shall deinit first */
    ESP_HIDH_NO_CONNECTION,     /*!< connection may have been closed */
} esp_hidh_status_t;

/**
 * @brief HID host protocol modes
 */
typedef enum {
    ESP_HIDH_BOOT_MODE = 0x00,       /*!< boot protocol mode */
    ESP_HIDH_REPORT_MODE = 0x01,     /*!< report protocol mode */
    ESP_HIDH_UNSUPPORTED_MODE = 0xff /*!< unsupported protocol mode */
} esp_hidh_protocol_mode_t;

/**
 * @brief HID host report types
 */
typedef enum {
    ESP_HIDH_REPORT_TYPE_OTHER = 0, /*!< unsupported report type */
    ESP_HIDH_REPORT_TYPE_INPUT,     /*!< input report type */
    ESP_HIDH_REPORT_TYPE_OUTPUT,    /*!< output report type */
    ESP_HIDH_REPORT_TYPE_FEATURE,   /*!< feature report type */
} esp_hidh_report_type_t;

/**
 * @brief HID host callback function events
 */
typedef enum {
    ESP_HIDH_INIT_EVT = 0,  /*!< When HID host is initialized, the event comes */
    ESP_HIDH_DEINIT_EVT,    /*!< When HID host is deinitialized, the event comes */
    ESP_HIDH_OPEN_EVT,      /*!< When HID host connection opened, the event comes */
    ESP_HIDH_CLOSE_EVT,     /*!< When HID host connection closed, the event comes */
    ESP_HIDH_GET_RPT_EVT,   /*!< When Get_Report command is called, the event comes */
    ESP_HIDH_SET_RPT_EVT,   /*!< When Set_Report command is called, the event comes */
    ESP_HIDH_GET_PROTO_EVT, /*!< When Get_Protocol command is called, the event comes */
    ESP_HIDH_SET_PROTO_EVT, /*!< When Set_Protocol command is called, the event comes */
    ESP_HIDH_GET_IDLE_EVT,  /*!< When Get_Idle command is called, the event comes */
    ESP_HIDH_SET_IDLE_EVT,  /*!< When Set_Idle command is called, the event comes */
    ESP_HIDH_GET_DSCP_EVT,  /*!< When HIDH is initialized, the event comes */
    ESP_HIDH_ADD_DEV_EVT,   /*!< When a device is added, the event comes */
    ESP_HIDH_RMV_DEV_EVT,   /*!< When a device is removed, the event comes */
    ESP_HIDH_VC_UNPLUG_EVT, /*!< When virtually unplugged, the event comes */
    ESP_HIDH_DATA_EVT,      /*!< When send data on interrupt channel, the event comes */
    ESP_HIDH_DATA_IND_EVT,  /*!< When receive data on interrupt channel, the event comes */
    ESP_HIDH_SET_INFO_EVT   /*!< When set the HID device descriptor, the event comes */
} esp_hidh_cb_event_t;

typedef struct {
    int attr_mask;
    uint8_t sub_class;
    uint8_t app_id;
    int vendor_id;
    int product_id;
    int version;
    uint8_t ctry_code;
    int dl_len;
    uint8_t dsc_list[BTHH_MAX_DSC_LEN];
} esp_hidh_hid_info_t;

/**
 * @brief HID host callback parameters union
 */
typedef union {
    /**
     * @brief ESP_HIDH_INIT_EVT
     */
    struct hidh_init_evt_param {
        esp_hidh_status_t status; /*!< status */
    } init;                       /*!< HIDH callback param of ESP_HIDH_INIT_EVT */

    /**
     * @brief ESP_HIDH_DEINIT_EVT
     */
    struct hidh_uninit_evt_param {
        esp_hidh_status_t status; /*!< status */
    } deinit;                     /*!< HIDH callback param of ESP_HIDH_DEINIT_EVT */

    /**
     * @brief ESP_HIDH_OPEN_EVT
     */
    struct hidh_open_evt_param {
        esp_hidh_status_t status;                /*!< operation status         */
        esp_hidh_connection_state_t conn_status; /*!< connection status        */
        bool is_orig;                            /*!< indicate if host intiate the connection        */
        uint8_t handle;                          /*!< device handle            */
        esp_bd_addr_t bd_addr;                   /*!< device address           */
    } open;                                      /*!< HIDH callback param of ESP_HIDH_OPEN_EVT */

    /**
     * @brief ESP_HIDH_CLOSE_EVT
     */
    struct hidh_close_evt_param {
        esp_hidh_status_t status;                /*!< operation status         */
        uint8_t reason;                          /*!< lower layer failed reason(ref hiddefs.h)       */
        esp_hidh_connection_state_t conn_status; /*!< connection status        */
        uint8_t handle;                          /*!< device handle            */
    } close;                                     /*!< HIDH callback param of ESP_HIDH_CLOSE_EVT */

    /**
     * @brief ESP_HIDH_VC_UNPLUG_EVT
     */
    struct hidh_unplug_evt_param {
        esp_hidh_status_t status;                /*!< operation status         */
        esp_hidh_connection_state_t conn_status; /*!< connection status        */
        uint8_t handle;                          /*!< device handle            */
    } unplug;                                    /*!< HIDH callback param of ESP_HIDH_VC_UNPLUG_EVT */

    /**
     * @brief ESP_HIDH_GET_PROTO_EVT
     */
    struct hidh_get_proto_evt_param {
        esp_hidh_status_t status;            /*!< operation status         */
        uint8_t handle;                      /*!< device handle            */
        esp_hidh_protocol_mode_t proto_mode; /*!< protocol mode            */
    } get_proto;                             /*!< HIDH callback param of ESP_HIDH_GET_PROTO_EVT */

    /**
     * @brief ESP_HIDH_SET_PROTO_EVT
     */
    struct hidh_set_proto_evt_param {
        esp_hidh_status_t status; /*!< operation status         */
        uint8_t handle;           /*!< device handle            */
    } set_proto;                  /*!< HIDH callback param of ESP_HIDH_SET_PROTO_EVT */

    /**
     * @brief ESP_HIDH_GET_RPT_EVT
     */
    struct hidh_get_rpt_evt_param {
        esp_hidh_status_t status; /*!< operation status         */
        uint8_t handle;           /*!< device handle            */
        uint16_t len;             /*!< data length              */
        uint8_t *data;            /*!< data pointer             */
    } get_rpt;                    /*!< HIDH callback param of ESP_HIDH_GET_RPT_EVT */

    /**
     * @brief ESP_HIDH_SET_RPT_EVT
     */
    struct hidh_set_rpt_evt_param {
        esp_hidh_status_t status; /*!< operation status         */
        uint8_t handle;           /*!< device handle            */
    } set_rpt;                    /*!< HIDH callback param of ESP_HIDH_SET_RPT_EVT */

    /**
     * @brief ESP_HIDH_DATA_EVT
     */
    struct hidh_send_data_evt_param {
        esp_hidh_status_t status; /*!< operation status         */
        uint8_t handle;           /*!< device handle            */
        uint8_t reason;           /*!< lower layer failed reason(ref hiddefs.h)       */
    } send_data;                  /*!< HIDH callback param of ESP_HIDH_DATA_EVT */

    /**
     * @brief ESP_HIDH_GET_IDLE_EVT
     */
    struct hidh_get_idle_evt_param {
        esp_hidh_status_t status; /*!< operation status         */
        uint8_t handle;           /*!< device handle            */
        uint8_t idle_rate;        /*!< idle rate                */
    } get_idle;                   /*!< HIDH callback param of ESP_HIDH_GET_IDLE_EVT */

    /**
     * @brief ESP_HIDH_SET_IDLE_EVT
     */
    struct hidh_set_idle_evt_param {
        esp_hidh_status_t status; /*!< operation status         */
        uint8_t handle;           /*!< device handle            */
    } set_idle;                   /*!< HIDH callback param of ESP_HIDH_SET_IDLE_EVT */

    /**
     * @brief ESP_HIDH_DATA_IND_EVT
     */
    struct hidh_data_ind_evt_param {
        esp_hidh_status_t status;            /*!< operation status         */
        uint8_t handle;                      /*!< device handle            */
        esp_hidh_protocol_mode_t proto_mode; /*!< protocol mode            */
        uint16_t len;                        /*!< data length              */
        uint8_t *data;                       /*!< data pointer             */
    } data_ind;                              /*!< HIDH callback param of ESP_HIDH_DATA_IND_EVT */

    /**
     * @brief ESP_HIDH_ADD_DEV_EVT
     */
    struct hidh_add_dev_evt_param {
        esp_hidh_status_t status; /*!< operation status         */
        uint8_t handle;           /*!< device handle            */
        esp_bd_addr_t bd_addr;    /*!< device address           */
    } add_dev;                    /*!< HIDH callback param of ESP_HIDH_ADD_DEV_EVT */

    /**
     * @brief ESP_HIDH_RMV_DEV_EVT
     */
    struct hidh_rmv_dev_evt_param {
        esp_hidh_status_t status; /*!< operation status         */
        uint8_t handle;           /*!< device handle            */
        esp_bd_addr_t bd_addr;    /*!< device address           */
    } rmv_dev;                    /*!< HIDH callback param of ESP_HIDH_RMV_DEV_EVT */

    /**
     * @brief ESP_HIDH_GET_DSCP_EVT
     */
    struct hidh_get_dscp_evt_param {
        esp_hidh_status_t status; /*!< operation status         */
        uint8_t handle;           /*!< device handle            */
        bool added;               /*!< Indicate if added        */
        uint16_t vendor_id;       /*!< Vendor ID */
        uint16_t product_id;      /*!< Product ID */
        uint16_t version;         /*!< Version */
        uint16_t ssr_max_latency; /*!< SSR max latency */
        uint16_t ssr_min_tout;    /*!< SSR min timeout */
        uint8_t ctry_code;        /*!< Country Code */
        uint16_t dl_len;          /*!< Device descriptor length */
        uint8_t *dsc_list;        /*!< Device descriptor pointer */
    } dscp;                       /*!< HIDH callback param of ESP_HIDH_GET_DSCP_EVT */

    /**
     * @brief ESP_HIDH_SET_INFO_EVT
     */
    struct hidh_set_info_evt_param {
        esp_hidh_status_t status; /*!< operation status         */
        uint8_t handle;           /*!< device handle            */
        esp_bd_addr_t bd_addr;    /*!< device address           */
    } set_info;                   /*!< HIDH callback param of ESP_HIDH_SET_INFO_EVT */
} esp_hidh_cb_param_t;

/**
 * @brief       HID host callback function type
 * @param       event:      Event type
 * @param       param:      Point to callback parameter, currently is union type
 */
typedef void (*esp_hh_cb_t)(esp_hidh_cb_event_t event, esp_hidh_cb_param_t *param);

/**
 * @brief       This function is called to init callbacks with HID host module.
 *
 * @param[in]   callback:   pointer to the init callback function.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_bt_hid_host_register_callback(esp_hh_cb_t callback);

/**
 * @brief       This function initializes HID host. This function should be called after esp_bluedroid_enable() and
 *              esp_bluedroid_init() success, and should be called after esp_bt_hid_host_register_callback().
 *              When the operation is complete the callback function will be called with ESP_HIDH_INIT_EVT.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_bt_hid_host_init(void);

/**
 * @brief       Closes the interface. This function should be called after esp_bluedroid_enable() and
 *              esp_bluedroid_init() success, and should be called after esp_bt_hid_host_init().
 *              When the operation is complete the callback function will be called with ESP_HIDH_DEINIT_EVT.
 *
 * @return      - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_bt_hid_host_deinit(void);

/**
 * @brief       Connect to hid device. When the operation is complete the callback
 *              function will be called with ESP_HIDH_OPEN_EVT.
 *
 * @param[in]   bd_addr:  Remote device bluetooth device address.
 *
 * @return      - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_bt_hid_host_connect(esp_bd_addr_t bd_addr);

/**
 * @brief       Disconnect from hid device. When the operation is complete the callback
 *              function will be called with ESP_HIDH_CLOSE_EVT.
 *
 * @param[in]   bd_addr:  Remote device bluetooth device address.
 *
 * @return      - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_bt_hid_host_disconnect(esp_bd_addr_t bd_addr);

/**
 * @brief       Virtual UnPlug (VUP) the specified HID device. When the operation is complete the callback
 *              function will be called with ESP_HIDH_VC_UNPLUG_EVT.
 *
 * @param[in]   bd_addr:  Remote device bluetooth device address.
 *
 * @return      - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_bt_hid_host_virtual_cable_unplug(esp_bd_addr_t bd_addr);

/**
 * @brief       Set the HID device descriptor for the specified HID device. When the operation is complete the callback
 *              function will be called with ESP_HIDH_SET_INFO_EVT.
 *
 * @param[in]   bd_addr:  Remote device bluetooth device address.
 * @param[in]   hid_info:  HID device descriptor structure.
 *
 * @return      - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_bt_hid_host_set_info(esp_bd_addr_t bd_addr, esp_hidh_hid_info_t *hid_info);

/**
 * @brief       Get the HID proto mode. When the operation is complete the callback
 *              function will be called with ESP_HIDH_GET_PROTO_EVT.
 *
 * @param[in]   bd_addr:  Remote device bluetooth device address.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_bt_hid_host_get_protocol(esp_bd_addr_t bd_addr);

/**
 * @brief       Set the HID proto mode. When the operation is complete the callback
 *              function will be called with ESP_HIDH_SET_PROTO_EVT.
 *
 * @param[in]   bd_addr:  Remote device bluetooth device address.
 * @param[in]   protocol_mode:  Protocol mode type.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_bt_hid_host_set_protocol(esp_bd_addr_t bd_addr, esp_hidh_protocol_mode_t protocol_mode);

/**
 * @brief       Get the HID Idle Time. When the operation is complete the callback
 *              function will be called with ESP_HIDH_GET_IDLE_EVT.
 *
 * @param[in]   bd_addr:  Remote device bluetooth device address.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_bt_hid_host_get_idle(esp_bd_addr_t bd_addr);

/**
 * @brief       Set the HID Idle Time. When the operation is complete the callback
 *              function will be called with ESP_HIDH_SET_IDLE_EVT.
 *
 * @param[in]   bd_addr:  Remote device bluetooth device address.
 * @param[in]   idle_time:  Idle time rate
 *
 * @return    - ESP_OK: success
 *            - other: failed
 */
esp_err_t esp_bt_hid_host_set_idle(esp_bd_addr_t bd_addr, uint16_t idle_time);

/**
 * @brief       Send a GET_REPORT to HID device. When the operation is complete the callback
 *              function will be called with ESP_HIDH_GET_RPT_EVT.
 *
 * @param[in]   bd_addr:  Remote device bluetooth device address.
 * @param[in]   report_type:  Report type
 * @param[in]   report_id:  Report id
 * @param[in]   buffer_size:  Buffer size
 *
 * @return      - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_bt_hid_host_get_report(esp_bd_addr_t bd_addr, esp_hidh_report_type_t report_type, uint8_t report_id,
                                     int buffer_size);

/**
 * @brief       Send a SET_REPORT to HID device. When the operation is complete the callback
 *              function will be called with ESP_HIDH_SET_RPT_EVT.
 *
 * @param[in]   bd_addr:  Remote device bluetooth device address.
 * @param[in]   report_type:  Report type
 * @param[in]   report:  Report data pointer
 * @param[in]   len:  Report data length
 *
 * @return      - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_bt_hid_host_set_report(esp_bd_addr_t bd_addr, esp_hidh_report_type_t report_type, uint8_t *report,
                                     size_t len);

/**
 * @brief       Send data to HID device. When the operation is complete the callback
 *              function will be called with ESP_HIDH_DATA_EVT.
 *
 * @param[in]   bd_addr:  Remote device bluetooth device address.
 * @param[in]   data:  Data pointer
 * @param[in]   len:  Data length
 *
 * @return      - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_bt_hid_host_send_data(esp_bd_addr_t bd_addr, uint8_t *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif
