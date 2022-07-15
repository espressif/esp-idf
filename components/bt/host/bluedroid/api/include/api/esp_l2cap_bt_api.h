/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_L2CAP_BT_API_H__
#define __ESP_L2CAP_BT_API_H__

#include "esp_err.h"
#include "esp_bt_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief L2CAP operation success and failure codes
 */
typedef enum {
    ESP_BT_L2CAP_SUCCESS   = 0,          /*!< Successful operation. */
    ESP_BT_L2CAP_FAILURE,                /*!< Generic failure. */
    ESP_BT_L2CAP_BUSY,                   /*!< Temporarily can not handle this request. */
    ESP_BT_L2CAP_NO_RESOURCE,            /*!< No more resource */
    ESP_BT_L2CAP_NEED_INIT,              /*!< L2CAP module shall init first */
    ESP_BT_L2CAP_NEED_DEINIT,            /*!< L2CAP module shall deinit first */
    ESP_BT_L2CAP_NO_CONNECTION,          /*!< Connection may have been closed */
    ESP_BT_L2CAP_NO_SERVER,              /*!< No server */
} esp_bt_l2cap_status_t;

/**
 * @brief Security Setting Mask. Use these three mask mode:
 *        1. ESP_BT_L2CAP_SEC_NONE
 *        2. ESP_BT_L2CAP_SEC_AUTHENTICATE
 *        3. (ESP_BT_L2CAP_SEC_ENCRYPT|ESP_BT_L2CAP_SEC_AUTHENTICATE)
 */
#define ESP_BT_L2CAP_SEC_NONE            0x0000    /*!< No security */
#define ESP_BT_L2CAP_SEC_AUTHORIZE       0x0001    /*!< Authorization required */
#define ESP_BT_L2CAP_SEC_AUTHENTICATE    0x0012    /*!< Authentication required */
#define ESP_BT_L2CAP_SEC_ENCRYPT         0x0024    /*!< Encryption required */
typedef uint32_t esp_bt_l2cap_cntl_flags_t;

/**
 * @brief L2CAP callback function events
 */
typedef enum {
    ESP_BT_L2CAP_INIT_EVT                 = 0,      /*!< When L2CAP is initialized, the event comes */
    ESP_BT_L2CAP_UNINIT_EVT               = 1,      /*!< When L2CAP is deinitialized, the event comes */
    ESP_BT_L2CAP_OPEN_EVT                 = 16,     /*!< When L2CAP Client connection open, the event comes */
    ESP_BT_L2CAP_CLOSE_EVT                = 17,     /*!< When L2CAP connection closed, the event comes */
    ESP_BT_L2CAP_START_EVT                = 18,     /*!< When L2CAP server started, the event comes */
    ESP_BT_L2CAP_CL_INIT_EVT              = 19,     /*!< When L2CAP client initiated a connection, the event comes */
    ESP_BT_L2CAP_SRV_STOP_EVT             = 36,     /*!< When L2CAP server stopped, the event comes */
} esp_bt_l2cap_cb_event_t;

/**
 * @brief L2CAP callback parameters union
 */
typedef union {
    /**
     * @brief ESP_BT_L2CAP_INIT_EVT
     */
    struct l2cap_init_evt_param {
        esp_bt_l2cap_status_t  status;         /*!< status */
    } init;                                    /*!< L2CAP callback param of ESP_BT_L2CAP_INIT_EVT */

    /**
     * @brief ESP_BT_L2CAP_UNINIT_EVT
     */
    struct l2cap_uninit_evt_param {
        esp_bt_l2cap_status_t  status;         /*!< status */
    } uninit;                                  /*!< L2CAP callback param of ESP_BT_L2CAP_UNINIT_EVT */

    /**
     * @brief ESP_BT_L2CAP_OPEN_EVT
     */
    struct l2cap_open_evt_param {
        esp_bt_l2cap_status_t  status;         /*!< status */
        uint32_t               handle;         /*!< The connection handle */
        int                    fd;             /*!< File descriptor  */
        esp_bd_addr_t          rem_bda;        /*!< The peer address */
        int32_t                tx_mtu;         /*!< The transmit MTU */
    } open;                                    /*!< L2CAP callback param of ESP_BT_L2CAP_OPEN_EVT */

    /**
     * @brief ESP_BT_L2CAP_CLOSE_EVT
     */
    struct l2cap_close_evt_param {
        esp_bt_l2cap_status_t  status;         /*!< status */
        uint32_t               handle;         /*!< The connection handle */
        bool                   async;          /*!< FALSE, if local initiates disconnect */
    } close;                                   /*!< L2CAP callback param of ESP_BT_L2CAP_CLOSE_EVT */

    /**
     * @brief ESP_BT_L2CAP_START_EVT
     */
    struct l2cap_start_evt_param {
        esp_bt_l2cap_status_t  status;         /*!< status */
        uint32_t               handle;         /*!< The connection handle */
        uint8_t                sec_id;         /*!< security ID used by this server */
    } start;                                   /*!< L2CAP callback param of ESP_BT_L2CAP_START_EVT */

    /**
     * @brief ESP_BT_L2CAP_CL_INIT_EVT
     */
    struct l2cap_cl_init_evt_param {
        esp_bt_l2cap_status_t  status;         /*!< status */
        uint32_t               handle;         /*!< The connection handle */
        uint8_t                sec_id;         /*!< security ID used by this server */
    } cl_init;                                 /*!< L2CAP callback param of ESP_BT_L2CAP_CL_INIT_EVT */

    /**
     * @brief ESP_BT_L2CAP_SRV_STOP_EVT
     */
    struct l2cap_srv_stop_evt_param {
        esp_bt_l2cap_status_t  status;         /*!< status */
        uint8_t                psm;            /*!< local psm */
    } srv_stop;                                /*!< L2CAP callback param of ESP_BT_L2CAP_SRV_STOP_EVT */

} esp_bt_l2cap_cb_param_t;

/**
 * @brief       L2CAP callback function type.
 *
 * @param       event:      Event type
 * @param       param:      Point to callback parameter, currently is union type
 */
typedef void (* esp_bt_l2cap_cb_t)(esp_bt_l2cap_cb_event_t event, esp_bt_l2cap_cb_param_t *param);

/**
 * @brief       This function is called to init callbacks with L2CAP module.
 *
 * @param[in]   callback:   pointer to the init callback function.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_bt_l2cap_register_callback(esp_bt_l2cap_cb_t callback);

/**
 * @brief       This function is called to init L2CAP module.
 *              When the operation is completed, the callback function will be called with ESP_BT_L2CAP_INIT_EVT.
 *              This function should be called after esp_bluedroid_enable() completes successfully.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_bt_l2cap_init(void);

/**
 * @brief       This function is called to uninit l2cap module.
 *              The operation will close all active L2CAP connection first, then the callback function will be called
 *              with ESP_BT_L2CAP_CLOSE_EVT, and the number of ESP_BT_L2CAP_CLOSE_EVT is equal to the number of connection.
 *              When the operation is completed, the callback function will be called with ESP_BT_L2CAP_UNINIT_EVT.
 *              This function should be called after esp_bt_l2cap_init() completes successfully.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_bt_l2cap_deinit(void);

/**
 * @brief       This function makes an L2CAP connection to a remote BD Address.
 *              When the connection is initiated or failed to initiate, the callback is called with ESP_BT_L2CAP_CL_INIT_EVT.
 *              When the connection is established or failed, the callback is called with ESP_BT_L2CAP_OPEN_EVT.
 *              This function must be called after esp_bt_l2cap_init() successful and before esp_bt_l2cap_deinit().
 *
 * @param[in]   cntl_flag:    Lower 16-bit security settings mask.
 * @param[in]   remote_psm:   Remote device bluetooth Profile PSM.
 * @param[in]   peer_bd_addr: Remote device bluetooth device address.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_bt_l2cap_connect(esp_bt_l2cap_cntl_flags_t cntl_flag, uint16_t remote_psm, esp_bd_addr_t peer_bd_addr);

/**
 * @brief       This function create a L2CAP server and starts listening for an
 *              L2CAP connection request from a remote Bluetooth device.
 *              When the server is started successfully, the callback is called with ESP_BT_L2CAP_START_EVT.
 *              When the connection is established, the callback is called with ESP_BT_L2CAP_OPEN_EVT.
 *              This function must be called after esp_bt_l2cap_init() successful and before esp_bt_l2cap_deinit().
 *
 * @param[in]   cntl_flag:    Lower 16-bit security settings mask.
 * @param[in]   local_psm:    Dynamic PSM.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_bt_l2cap_start_srv(esp_bt_l2cap_cntl_flags_t cntl_flag, uint16_t local_psm);

/**
 * @brief       This function stops all L2CAP servers.
 *              The operation will close all active L2CAP connection first, then the callback function will be called
 *              with ESP_BT_L2CAP_CLOSE_EVT, and the number of ESP_BT_L2CAP_CLOSE_EVT is equal to the number of connection.
 *              When the operation is completed, the callback is called with ESP_BT_L2CAP_SRV_STOP_EVT.
 *              This function must be called after esp_bt_l2cap_init() successful and before esp_bt_l2cap_deinit().
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */

esp_err_t esp_bt_l2cap_stop_all_srv(void);

/**
 * @brief       This function stops a specific L2CAP server.
 *              The operation will close all active L2CAP connection first on the specific L2CAP server, then the callback function will
 *              be called with ESP_BT_L2CAP_CLOSE_EVT, and the number of ESP_BT_L2CAP_CLOSE_EVT is equal to the number of connection.
 *              When the operation is completed, the callback is called with ESP_BT_L2CAP_SRV_STOP_EVT.
 *              This function must be called after esp_bt_l2cap_init() successful and before esp_bt_l2cap_deinit().
 *
 * @param[in]   local_psm:    Dynamic PSM.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_bt_l2cap_stop_srv(uint16_t local_psm);

/**
 * @brief       This function is used to register VFS.
 *              Only supports write, read and close.
 *              This function must be called after esp_bt_l2cap_init() successful and before esp_bt_l2cap_deinit().
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_bt_l2cap_vfs_register(void);

/**
 * @brief       This function is used to unregister VFS.
 *              This function must be called after esp_bt_l2cap_init() successful and before esp_bt_l2cap_deinit().
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_bt_l2cap_vfs_unregister(void);

#ifdef __cplusplus
}
#endif

#endif ///__ESP_L2CAP_BT_API_H__
