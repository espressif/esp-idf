/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
*
* SPDX-License-Identifier: Apache-2.0
*/

#pragma once

#include "sdkconfig.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// @brief HCI VS Events for Espressif's Bluetooth Classic Host
//
// @note The following HCI VS events are exclusively for Espressif's Bluetooth Classic Host (ESP-Bluedroid Host).
//       If you are using a non-ESP host or HCI UART, these events will remain disabled unless enabling the feature in the menuconfig (CONFIG_BT_CTRL_BR_EDR_LEGACY_AUTH_VENDOR_EVT).
//       Note, these init functions as well as these additional HCI VS events are intended for Espressif's Bluetooth Host use only.
//       Application developers **should not** call the init functions in their applications.
//

/**
* @brief This event indicates legacy authentication is completed by remote device (EVTCODE: 0xFF, SUBCODE: 0x03)
*
* @note The init function is `bt_stack_enableSecCtrlVsCmd(true)`
*/
#define ESP_BT_VS_LEGACY_REM_AUTH_EVT_SUBCODE                       (0x03)
/**
* @brief legacy remote auth event parameters
*/
struct bt_hci_vs_legacy_rem_auth_evt {
    uint16_t conhdl; /*!< connection handle */
};

//
// @brief HCI VS Commands for Espressif's Internal-Use Debugging
//
// @note The following HCI VS debugging commands are implemented in Bluetooth Classic controller pre-compiled libraries.
//        These commands are not linked into the application binary, unless the corresponding initialization function is explicitly called from the application.
//        They are intended for Espressif's internal use only. Application developers **should not** call the corresponding initialization function in their applications.
//

/**
* @note The init function is `bt_stack_enablePktCtrlVsCmd(true)`
*/
#define ESP_BT_VS_WR_DM1_ENABLE_OCF                                 (0x0181)

/**
* @note The init function is `bt_stack_enableClkCtrlVsCmd(true)`
*/
#define ESP_BT_VS_CLK_UPDATE_OCF                                    (0x0183)

/**
* @note The init function is `bt_stack_enableAfhVsCmd(true)`
*/
#define ESP_BT_VS_SET_AFH_OCF                                       (0x0187)

/**
* @note The init function is `bt_stack_enableBasicVsCmd(true)`
*/
#define ESP_BT_VS_SET_EVT_MASK_OCF                                  (0x0188)

/**
* @note The init function is `bt_stack_enableAfhVsCmd(true)`
*/
#define ESP_BT_VS_SET_AFH_REPORTING_MODE_OCF                        (0x0189)

/**
* @note The init function is `bt_stack_enableAfhVsCmd(true)`
*/
#define ESP_BT_VS_MASK_RMT_CHANNEL_CLASSIFICATION_OCF               (0x018a)

/**
* @note The init function is `bt_stack_enableDTMVsCmd(true)`
*/
#define ESP_BT_VS_DTM_TX_TEST_START_OCF                             (0x018c)

/**
* @note The init function is `bt_stack_enableDTMVsCmd(true)`
*/
#define ESP_BT_VS_DTM_RX_TEST_START_OCF                             (0x018d)

/**
* @note The init function is `bt_stack_enableDTMVsCmd(true)`
*/
#define ESP_BT_VS_DTM_TX_TEST_END_OCF                               (0x018e)

/**
* @note The init function is `bt_stack_enableDTMVsCmd(true)`
*/
#define ESP_BT_VS_DTM_RX_TEST_END_OCF                               (0x018f)

/**
* @note The init function is `bt_stack_enableInternalDbgVsCmd(true)`
*/
#define ESP_BT_VS_PCA_OCF                                           (0x0190)

/**
* @note The init function is `bt_stack_enableInternalDbgVsCmd(true)`
*/
#define ESP_BT_VS_DISCARD_PING_OCF                                  (0x0191)

/**
* @note The init function is `bt_stack_enableInternalDbgVsCmd(true)`
*/
#define ESP_BT_VS_LMP_DBG_ENABLE_OCF                                (0x019a)

/**
* @note The init function is `bt_stack_enableInternalDbgVsCmd(true)`
*/
#define ESP_BT_VS_SEND_LMP_OCF                                      (0x019b)

/**
* @note The init function is `bt_stack_enableInternalDbgVsCmd(true)`
*/
#define ESP_BT_VS_LMP_DBG_SAVE_OCF                                  (0x019c)

//
// @brief HCI VS Events for Espressif's Internal-Use Debugging
//
// @note The following HCI VS debugging events are implemented in Bluetooth Classic controller pre-compiled libraries.
//        These events are not linked into the application binary, unless the corresponding initialization function is explicitly called from the application.
//        They are intended for Espressif's internal use only. Application developers **should not** call the corresponding initialization function in their applications.
//

/**
* @note The init function is `bt_stack_enableAfhVsCmd(true)`
*/
#define ESP_BT_VS_AFH_CHG_EVT_SUBCODE                               (0x05)

/**
* @note The init function is `bt_stack_enableAfhVsCmd(true)`
*/
#define ESP_BT_VS_CH_CLASSIFICATION_EVT_SUBCODE                     (0x06)

/**
* @note The init function is `bt_stack_enableAfhVsCmd(true)`
*/
#define ESP_BT_VS_CH_CLASSIFICATION_REPORTING_MODE_EVT_SUBCODE      (0x07)

/**
* @note The init function is `bt_stack_enableInternalDbgVsCmd(true)`
*/
#define ESP_BT_VS_LMP_RECEIVED_EVT_SUBCODE                          (0x0a)

/**
* @note The init function is `bt_stack_enableInternalDbgVsCmd(true)`
*/
#define ESP_BT_VS_LMP_TX_CFM_EVT_SUBCODE                            (0x0b)

/**
* @note The init function is `bt_stack_enableInternalDbgVsCmd(true)`
*/
#define ESP_BT_VS_LMP_DBG_ENTERED_EVT_SUBCODE                       (0x0c)

#ifdef __cplusplus
}
#endif
