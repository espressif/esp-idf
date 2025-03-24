/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// @brief HCI VS Commands for Espressif's Bluetooth Host
//
// @note The following vendor-specific HCI commands are exclusively for Espressif's Bluetooth Host (Bluedroid Host or NimBLE Host).
//       If you are using a different host or HCI UART, these commands will not be enabled, unless the init function is called from the application.
//       Note, these functions as well as these additional vendor-specific HCI commands are intended for Espressif's Bluetooth Host use only.
//       Application developers should not call the init functions in their applications.
//

/**
 * @brief Test vendor HCI feature (OCF: 0x0081)
 *
 * The Controller return the value in command
 *
 * @note The init function is `bt_stack_enableEchoVsCmd(true)`
 */
#define ESP_BT_VS_COMMON_ECHO_OCF                                   (0x0081)

/**
* @brief echo test cmd parameters
*/
struct bt_hci_vs_common_echo {
    uint8_t echo; /*!< echo data */
};

/**
 * @brief Set/Clear coexistence status (OCF: 0x0082)
 *
 * @note The init function is `bt_stack_enableCoexVsCmd(true)`
 */
#define ESP_BT_VS_SET_COEX_STATUS_OCF                               (0x0082)
/**
* @brief Set/Clear coexistence status cmd parameters
*/
struct bt_hci_vs_set_coex_status {
    uint8_t type;  /*!< WIFI, BLE or BT */
    uint8_t op;    /*!< clear or set status */
    uint8_t status; /*!< WIFI, BLE or BT status */
};

/**
 * @brief Config scanning duplicate exceptional list (OCF: 0x0108)
 *
 * @note The init function is `advFilter_stack_enableDupExcListCmd(true)`
 */
#define ESP_BT_VS_CONFIG_DUP_EXC_LIST_OCF                           (0x0108)
/**
* @brief Update exception list cmd parameters
*/
struct bt_hci_vs_update_exc_list {
    uint8_t subcode;    /*!< Add, remove or clear exception list */
    uint32_t type;      /*!< device type */
    uint8_t device_info[6]; /*!< device information */
};

/**
 * @brief Enable/disable advertising report flow control (OCF: 0x0109)
 *
 * @note The init function is `scan_stack_enableAdvFlowCtrlVsCmd(true)`
 */
#define ESP_BT_VS_SET_ADV_REPORT_FLOW_CTRL_OCF                      (0x0109)

/**
* @brief Init ADV flow control cmd parameters
*/
struct bt_hci_vs_init_adv_flow_ctrl {
    uint8_t enable; /*!< Enable ADV flow control */
    uint16_t num;   /*!< ADV buffer maximum value */
    uint16_t adv_lost_threshold; /*!< ADV lost event triggered threshold */
};

/**
 * @brief Update the number of advertising report flow control (OCF: 0x010A)
 *
 * @note The init function is `scan_stack_enableAdvFlowCtrlVsCmd(true)`
 */
#define ESP_BT_VS_UPD_ADV_REPORT_FLOW_CTRL_NUM_OCF                  (0x010a)
/**
* @brief Update ADV flow control cmd parameters
*/
struct bt_hci_vs_update_adv_flow_ctrl {
    uint16_t num; /*!< The number of ADV report processed */
};

/**
 * @brief Clear legacy advertising (same as HCI_LE_Clear_Advertising_Sets) (OCF: 0x010C)
 *
 * @note The init function is `adv_stack_enableClearLegacyAdvVsCmd(true)`
 */
#define ESP_BT_VS_CLR_LEGACY_ADV_OCF                                (0x010c)
/**
* @brief Clear legacy ADV cmd parameters
*/
struct bt_hci_vs_ble_clr_legacy_adv {
    // no parameters
};

/**
 * @brief Set Classic Bluetooth minimum encryption key size (OCF: 0x0182)
 *
 * @note The init function is `bt_stack_enableSecCtrlVsCmd(true)`
 */
#define ESP_BT_VS_SET_MIN_ENC_KEY_SIZE_OCF                          (0x0182)
/**
* @brief Set bt minimum encryption key size cmd parameters
*/
struct bt_hci_vs_set_min_enc_key_size {
    uint8_t size; /*!< the minimum encryption key size */
};

//
// @brief HCI VS Events for Espressif's Bluetooth Host
//
// @note The following HCI VS events are exclusively for Espressif's Bluetooth Host (ESP-Bluedroid Host or ESP-NimBLE Host).
//       If you are using a non-ESP host or HCI UART, these events will remain disabled unless the initialization function is explicitly called from the application.
//       Note, these init functions as well as these additional HCI VS events are intended for Espressif's Bluetooth Host use only.
//       Application developers **should not** call the init functions in their applications.
//

/**
 * @brief BLE advertising report lost event for flow control (EVTCODE: 0x3E, SUBCODE: 0xF0)
 *
 * @note The init function is `scan_stack_enableAdvFlowCtrlVsCmd(true)`
 */
#define ESP_BT_VS_LE_ADV_LOST_EVT_SUBCODE                           (0xF0)
/**
* @brief ADV lost event parameters
*/
struct bt_hci_vs_le_adv_lost_evt {
    uint32_t nb_lost; /*!< The number of ADV report discarded */
};

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
// @note The following HCI VS debugging commands are implemented in Bluetooth controller pre-compiled libraries.
//       These commands are not linked into the application binary, unless the specific enabling function is called from the application.
//       They are intended for Espressif's internal use only. Application developers **should not** call the specific enabling function in their applications.
//

//
// @brief OCF for vendor specific BLE internal test command
//
// @note The init function is `esp_ble_internalTestFeaturesEnable(true)`
//
#define ESP_BT_VS_CFG_TEST_RELATED_OCF                              (0x0113)
    #define ESP_BT_VS_CFG_TEST_ENABLE_SUBCMD                        (0X00)
    #define ESP_BT_VS_CFG_TEST_ENABLE_ADV_DELAY_SUBCMD              (0X01)
    #define ESP_BT_VS_CFG_TEST_SET_SCAN_FOREVER_SUBCMD              (0X04)
    #define ESP_BT_VS_CFG_TEST_SET_EXPECTED_PEER_SUBCMD             (0X05)
    #define ESP_BT_VS_CFG_TEST_GET_ADV_TXED_CNT_SUBCMD              (0X06)
    #define ESP_BT_VS_CFG_TEST_GET_SCAN_RXED_CNT_SUBCMD             (0X07)
    #define ESP_BT_VS_CFG_TEST_SET_TXPWR_LVL_SUBCMD                 (0X08)
    #define ESP_BT_VS_CFG_TEST_GET_TXPWR_LVL_SUBCMD                 (0X09)
    #define ESP_BT_VS_CFG_TEST_CLEAR_RAND_ADDR_SUBCMD               (0X11)
    #define ESP_BT_VS_CFG_TEST_GET_MAX_TXPWR_SUBCMD                 (0X12)
    #define ESP_BT_VS_CFG_TEST_GET_TXPWR_RANGE_SUBCMD               (0X13)
    #define ESP_BT_VS_CFG_TEST_SET_SCAN_AA_SUBCMD                   (0X14)
    #define ESP_BT_VS_CFG_TEST_SET_ADV_AA_SUBCMD                    (0X15)
    #define ESP_BT_VS_CFG_TEST_SET_SCAN_CHAN_SUBCMD                 (0X16)
    #define ESP_BT_VS_CFG_TEST_GET_CTRL_STATUS_SUBCMD               (0X1a)
    #define ESP_BT_VS_CFG_TEST_GET_CTRL_COMPILE_VER_SUBCMD          (0X24)
    #define ESP_BT_VS_CFG_TEST_RELATED_SUBCMD_MAX                   (0Xff)

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
 * @note The init function is `bt_stack_enableRateCtrlVsCmd(true)`
 */
#define ESP_BT_VS_WR_AUTO_RATE_INIT_OCF                             (0x018b)

//
// @brief HCI VS Events for Espressif's Internal-Use Debugging
//
// @note The following HCI VS debugging events are implemented in Bluetooth controller pre-compiled libraries.
//       These events are not linked into the application binary, unless the specific enabling function is called from the application.
//       Application developers **should not** call the specific enabling function in their applications.
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

#ifdef __cplusplus
}
#endif
