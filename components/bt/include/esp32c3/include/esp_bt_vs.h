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


// @brief HCI VS Commands for Espressif's Bluetooth Host
//
// @note The following vendor-specific HCI commands are exclusively for Espressif's Bluetooth Host (ESP-Bluedroid Host or ESP-NimBLE Host).
//       If you are using a non-ESP host or HCI UART, these commands will remain disabled unless the initialization function is explicitly called from the application.
//       Note, these init functions as well as these additional HCI VS commands are intended for Espressif's Bluetooth Host use only.
//       Application developers **should not** call the init functions in their applications.
//

/**
 * @brief Test vendor HCI feature (OCF: 0x0081)
 *
 * The Controller returns the value in command.
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
 * @brief Config scanning duplicate exceptional list (OCF: 0x0108)
 *
 * @note The init function is `advFilter_stack_eanbleDupExcListCmd(true)`
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
 * @brief Update the number of advertising report in ADV flow control (OCF: 0x010A)
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
 * @brief Enable/disable channel selection algorithm #2 (OCF: 0x0112)
 *
 * @note The init function is `chanSel_stack_enableSetCsaVsCmd(true)`
 */
#define ESP_BT_VS_ENABLE_CSA2_OCF                                   (0x0112)
/**
* @brief Enable/disable channel selection algorithm #2 cmd parameters
*/
struct bt_hci_vs_ble_csa_enable {
    uint8_t csa2_select;  /*!< Select CSA2 */
};

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

//
// @brief HCI VS Commands for Espressif's Internal-Use Debugging
//
//  @note The following HCI VS debugging commands are implemented in Bluetooth controller pre-compiled libraries.
//        These commands are not linked into the application binary, unless the function `esp_ble_internalTestFeaturesEnable(true)`is called from the application.
//        They are intended for Espressif's internal use only. Application developers **should not** call `esp_ble_internalTestFeaturesEnable(true)` in their applications.
//

#define ESP_BT_VS_CFG_TEST_RELATED_OCF                              (0x0113)
    #define ESP_BT_VS_CFG_TEST_ENABLE_SUBCMD                        (0X00)
    #define ESP_BT_VS_CFG_TEST_ENABLE_ADV_DELAY_SUBCMD              (0X01)
    #define ESP_BT_VS_CFG_TEST_SET_PREF_CODED_SUBCMD                (0X02)
    #define ESP_BT_VS_CFG_TEST_SET_DEFAULT_PRIV_MODE_SUBCMD         (0X03)
    #define ESP_BT_VS_CFG_TEST_SET_SCAN_FOREVER_SUBCMD              (0X04)
    #define ESP_BT_VS_CFG_TEST_SET_EXPECTED_PEER_SUBCMD             (0X05)
    #define ESP_BT_VS_CFG_TEST_GET_ADV_TXED_CNT_SUBCMD              (0X06)
    #define ESP_BT_VS_CFG_TEST_GET_SCAN_RXED_CNT_SUBCMD             (0X07)
    #define ESP_BT_VS_CFG_TEST_SET_TXPWR_LVL_SUBCMD                 (0X08)
    #define ESP_BT_VS_CFG_TEST_GET_TXPWR_LVL_SUBCMD                 (0X09)
    #define ESP_BT_VS_CFG_TEST_SET_TXPWR_LVL_ENH_SUBCMD             (0X0a)
    #define ESP_BT_VS_CFG_TEST_GET_TXPWR_LVL_ENH_SUBCMD             (0X0b)
    #define ESP_BT_VS_CFG_TEST_ENABLE_CCA_SUBCMD                    (0X0e)
    #define ESP_BT_VS_CFG_TEST_CLEAR_RAND_ADDR_SUBCMD               (0X11)
    #define ESP_BT_VS_CFG_TEST_GET_MAX_TXPWR_SUBCMD                 (0X12)
    #define ESP_BT_VS_CFG_TEST_GET_TXPWR_RANGE_SUBCMD               (0X13)
    #define ESP_BT_VS_CFG_TEST_SET_SCAN_AA_SUBCMD                   (0X14)
    #define ESP_BT_VS_CFG_TEST_SET_ADV_AA_SUBCMD                    (0X15)
    #define ESP_BT_VS_CFG_TEST_SET_SCAN_CHAN_SUBCMD                 (0X16)
    #define ESP_BT_VS_CFG_TEST_GET_CTRL_STATUS_SUBCMD               (0X1a)
    #define ESP_BT_VS_CFG_TEST_GET_CTRL_COMPILE_VER_SUBCMD          (0X24)
    #define ESP_BT_VS_CFG_TEST_SET_AUX_ADV_OFFSET_SUBCMD            (0X25)
    #define ESP_BT_VS_CFG_TEST_SET_AUX_OFFSET_THRESHOLD_SUBCMD      (0X2b)
    #define ESP_BT_VS_CFG_TEST_SET_RX_SENS_THRESH_SUBCMD            (0X31)
    #define ESP_BT_VS_CFG_TEST_SET_AGC_MAX_GAIN_SUBCMD              (0X39)
    #define ESP_BT_VS_CFG_TEST_RELATED_SUBCMD_MAX                   (0Xff)

//
// @brief HCI VS Events for Espressif's Internal-Use Debugging
//
//  @note The following HCI VS debugging events are implemented in Bluetooth controller pre-compiled libraries.
//    These events are not linked into the application binary, unless the function `esp_ble_internalTestFeaturesEnable(true)`is called from the application.
//    Application developers **should not** call `esp_ble_internalTestFeaturesEnable(true)` in their applications.
//

#ifdef __cplusplus
}
#endif
