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
 * @brief Set parameters of duplicate list (OCF: 0x010D)
 *
 * @note The init function is `advFilter_stack_eanbleDupExcListCmd(true)`
 */
#define ESP_BT_VS_SET_DUP_LIST_PARAMS_OCF                           (0x010d)
/**
* @brief Set duplicate list cmd parameters
*/
struct bt_hci_vs_ble_set_dup_params {
    uint8_t dup_mode;           /*!< Duplicate mode */
    uint8_t dup_ad_type;        /*!< Duplicate data type */
    uint16_t ring_list_max_num; /*!< Duplicate list size */
};

/**
 * @brief Enable/disable duplicate and exception list (OCF: 0x010E)
 *
 * @note The init function is `advFilter_stack_eanbleDupExcListCmd(true)`
 */
#define ESP_BT_VS_ENABLE_DUP_EXC_LIST_OCF                           (0x010e)
/**
* @brief Enable duplicate and exception list cmd parameters
*/
struct bt_hci_vs_ble_enable_dup_exc {
    uint8_t enable;             /*!< Enable or disable */
    uint8_t ring_list_reset;    /*!< Reset list */
};

/**
 * @brief Enable optimization of multiple connections (OCF: 0x010F)
 *
 * @note The init function is `arr_stack_enableMultiConnVsCmd(true)`
 */
#define ESP_BT_VS_ENABLE_ARRANGEMENT_OCF                            (0x010f)
/**
* @brief Multiple connections optimization cmd parameters
*/
struct bt_hci_vs_ble_enable_arrangement {
    uint32_t common_factor;     /*!< The greatest common factor of connection interval */
    uint8_t enable;             /*!< Enable or disable */
};

/**
 * @brief Set scheduling length for a certain role (OCF: 0x0110)
 *
 * @note The init function is `arr_stack_enableMultiConnVsCmd(true)`
 */
#define ESP_BT_VS_SET_SCHED_ROLE_LEN_OCF                            (0x0110)
/**
* @brief Scheduling length cmd parameters
*/
struct bt_hci_vs_ble_set_sched_role_len {
    uint8_t role;   /*!< BLE role; 0: central; 1: peripheral */
    uint32_t len;   /*!< Length is us */
};

/**
 * @brief Set RSSI threshold for automatic power control (OCF: 0x0111)
 *
 * @note The init function is `pcl_stack_enableSetRssiThreshVsCmd(true)`
 */
#define ESP_BT_VS_SET_PCL_RSSI_THRESH_OCF                           (0x0111)
/**
* @brief PCL RSSI threshold cmd parameters
*/
struct bt_hci_vs_ble_set_pcl_rssi_thresh {
    uint16_t conn_handle;           /*!< Connection handle */
    uint8_t rssi_thresh_min_1M;     /*!< Lower limit for 1M */
    uint8_t rssi_thresh_max_1M;     /*!< Upper limit for 1M */
    uint8_t rssi_thresh_min_2M;     /*!< Lower limit for 2M */
    uint8_t rssi_thresh_max_2M;     /*!< Upper limit for 2M */
    uint8_t rssi_thresh_min_s2coded;    /*!< Lower limit for s2 coded */
    uint8_t rssi_thresh_max_s2coded;    /*!< Upper limit for s2 coded */
    uint8_t rssi_thresh_min_s8coded;    /*!< Lower limit for s8 coded */
    uint8_t rssi_thresh_max_s8coded;    /*!< Upper limit for s8 coded */
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

/**
 * @brief Set parameters of controller logs (OCF: 0x0114)
 *
 * @note The init function is `log_stack_enableLogsRelatedVsCmd(true)`
 */
#define ESP_BT_VS_SET_LOG_PARAMS_OCF                                (0x0114)
/**
* @brief Controller logs cmd parameters
*/
struct bt_hci_vs_ble_log_params {
    uint8_t type;           /*!< Operation type */
    uint32_t output_enable; /*!< Enable/disable output */
    uint8_t buffer_optoin   /*!< Select log buffers */
};

/**
 * @brief Set BLE vendor events mask (OCF: 0x0116)
 *
 * @note The init function is `hci_stack_enableSetVsEvtMaskVsCmd(true)`
 */
#define ESP_BT_VS_SET_LE_VENDOR_EVTS_MASK_OCF                       (0x0116)
/**
* @brief Set BLE vendor events mask cmd parameters
*/
struct bt_hci_vs_ble_set_vs_evts_mask {
    uint32_t evt_masks; /*!< BLE vendor events Mask */
};

// @brief HCI VS Events for Espressif's Bluetooth Host
//
// @note The following HCI VS events are exclusively for Espressif's Bluetooth Host (ESP-Bluedroid Host or ESP-NimBLE Host).
//       If you are using a non-ESP host or HCI UART, these events will remain disabled unless the initialization function is explicitly called from the application.
//       Note, these init functions as well as these additional HCI VS events are intended for Espressif's Bluetooth Host use only.
//       Application developers **should not** call the init functions in their applications.
//

/**
 * @brief BLE Scan/Connect Request, Aux Connect Response received event (EVTCODE: 0xFF, SUBCODE: 0xC0)
 *
 * @note The init function is `adv_stack_enableScanReqRxdVsEvent(true)`
 */
#define ESP_BT_VS_LE_CONN_SCAN_REQ_RXED_EVT_SUBCODE                 (0xC0)
/**
* @brief BLE Scan/Connect Request, Aux Connect Response received event parameters
*/
struct bt_hci_vs_le_conn_scan_req_rxed_evt {
    uint8_t evt_type;       /*!< Event type; 0: SCAN_REQ; 1: CONN_IND */
    uint8_t handle;         /*!< Advertisement handle */
    uint8_t peer_addr_type; /*!< Peer address type */
    uint8_t peer_addr[6];   /*!< Peer address */
};

/**
 * @brief BLE Channel Map Update Completion event (EVTCODE: 0xFF, SUBCODE: 0xC1)
 *
 * @note The init function is `conn_stack_enableChanMapUpdCompVsEvent(true)`
 */
#define ESP_BT_VS_LE_CHAN_UPDATE_COMP_EVT_SUBCODE                   (0xC1)
/**
* @brief BLE Channel Map Update Completion event parameters
*/
struct bt_hci_vs_le_chan_update_comp_evt {
    uint8_t status;     /*!< Controller error code */
    uint16_t handle;    /*!< Connection handle */
    uint8_t ch_map[5];  /*!< Updated channel map */
};

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
    #define ESP_BT_VS_CFG_TEST_IGNORE_WL_FOR_DIR_ADV_SUBCMD         (0X0c)
    #define ESP_BT_VS_CFG_TEST_GET_ADV_RXED_RSSI_SUBCMD             (0X0d)
    #define ESP_BT_VS_CFG_TEST_ENABLE_CCA_SUBCMD                    (0X0e)
    #define ESP_BT_VS_CFG_TEST_SET_CCA_WIN_SUBCMD                   (0X0f)
    #define ESP_BT_VS_CFG_TEST_READ_CCA_DATA_SUBCM                  (0X10)
    #define ESP_BT_VS_CFG_TEST_CLEAR_RAND_ADDR_SUBCMD               (0X11)
    #define ESP_BT_VS_CFG_TEST_GET_MAX_TXPWR_SUBCMD                 (0X12)
    #define ESP_BT_VS_CFG_TEST_GET_TXPWR_RANGE_SUBCMD               (0X13)
    #define ESP_BT_VS_CFG_TEST_SET_SCAN_AA_SUBCMD                   (0X14)
    #define ESP_BT_VS_CFG_TEST_SET_ADV_AA_SUBCMD                    (0X15)
    #define ESP_BT_VS_CFG_TEST_SET_SCAN_CHAN_SUBCMD                 (0X16)
    #define ESP_BT_VS_CFG_TEST_SKIP_LIGHT_SLEEP_CHECK_SUBCMD        (0X17)
    #define ESP_BT_VS_CFG_TEST_SET_WAKEUP_OVERHEAD_SUBCMD           (0X18)
    #define ESP_BT_VS_CFG_TEST_GET_ADV_MIN_ITVL_SUBCMD              (0X19)
    #define ESP_BT_VS_CFG_TEST_GET_CTRL_STATUS_SUBCMD               (0X1a)
    #define ESP_BT_VS_CFG_TEST_SET_CONN_PHY_TXPWR_SUBCMD            (0X1b)
    #define ESP_BT_VS_CFG_TEST_GET_CONN_PHY_TXPWR_SUBCMD            (0X1c)
    #define ESP_BT_VS_CFG_TEST_GET_RXBUF_EMPTY_CNT_SUBCMD           (0X1d)
    #define ESP_BT_VS_CFG_TEST_RESTART_SUBCMD                       (0X1e)
    #define ESP_BT_VS_CFG_TEST_ENABLE_RECODE_RX_STATE_SUBCMD        (0X1f)
    #define ESP_BT_VS_CFG_TEST_GET_RECODE_CNT_SUBCMD                (0X20)
    #define ESP_BT_VS_CFG_TEST_CLR_RECODE_CNT_SUBCMD                (0X21)
    #define ESP_BT_VS_CFG_TEST_GET_CTRL_COMPILE_VER_SUBCMD          (0X24)
    #define ESP_BT_VS_CFG_TEST_SET_AUX_ADV_OFFSET_SUBCMD            (0X25)
    #define ESP_BT_VS_CFG_TEST_INIT_FLEXIBLE_MODE_SUBCMD            (0X26)
    #define ESP_BT_VS_CFG_TEST_ENABLE_FLEXIBLE_MODE_SUBCMD          (0X27)
    #define ESP_BT_VS_CFG_TEST_SET_FLEXIBLE_CONN_ERR_SUBCMD         (0X28)
    #define ESP_BT_VS_CFG_TEST_SET_FLEXIBLE_ADV_ERR_SUBCMD          (0X29)
    #define ESP_BT_VS_CFG_TEST_SET_FLEXIBLE_SCAN_ERR_SUBCMD         (0X2a)
    #define ESP_BT_VS_CFG_TEST_GET_TXED_CRCERR_SUBCMD               (0X2c)
    #define ESP_BT_VS_CFG_TEST_GET_BACKOFF_UPLIMIT_SUBCMD           (0X2d)
    #define ESP_BT_VS_CFG_TEST_GET_RXED_ADV_ADI_SUBCMD              (0X2f)
    #define ESP_BT_VS_CFG_TEST_SET_SCH_RAND_MODE_SUBCMD             (0X30)
    #define ESP_BT_VS_CFG_TEST_SET_RX_SENS_THRESH_SUBCMD            (0X31)
    #define ESP_BT_VS_CFG_TEST_CHECK_MSYS_BUF_SUBCMD                (0X32)
    #define ESP_BT_VS_CFG_TEST_UPDATE_BLE_TIMER_SUBCMD              (0X33)
    #define ESP_BT_VS_CFG_TEST_UPDATE_BLE_RTC_SUBCMD                (0X34)
    #define ESP_BT_VS_CFG_TEST_SET_LOCKED_MEM_NUM_SUBCMD            (0X35)
    #define ESP_BT_VS_CFG_TEST_ALLOW_MEM_ALLOC_SUBCMD               (0X36)
    #define ESP_BT_VS_CFG_TEST_SET_SCH_RAND_INFO_PTR_SUBCMD         (0X37)
    #define ESP_BT_VS_CFG_TEST_SET_DIAG_IO_SUBCMD                   (0X38)
    #define ESP_BT_VS_CFG_TEST_SET_AGC_MAX_GAIN_SUBCMD              (0X39)
    #define ESP_BT_VS_CFG_TEST_ENABLE_CHAN_ASSESS_SUBCMD            (0X40)
    #define ESP_BT_VS_CFG_TEST_SET_BACKOFF_UPLIMIT_SUBCMD           (0X41)
    #define ESP_BT_VS_CFG_TEST_SET_CONN_TOP_PRIO_RESV_THRESH_SUBCMD (0X42)
    #define ESP_BT_VS_CFG_TEST_SET_TEST_EVT_MSK_SUBCMD              (0X43)
    #define ESP_BT_VS_CFG_TEST_GET_WAKEUP_TIMEOUT_SUBCMD            (0X45)
    #define ESP_BT_VS_CFG_TEST_RELATED_SUBCMD_MAX                   (0Xff)

//
// @brief HCI VS Events for Espressif's Internal-Use Debugging
//
//  @note The following HCI VS debugging events are implemented in Bluetooth controller pre-compiled libraries.
//    These events are not linked into the application binary, unless the function `esp_ble_internalTestFeaturesEnable(true)`is called from the application.
//    Application developers **should not** call `esp_ble_internalTestFeaturesEnable(true)` in their applications.
//

#define ESP_BT_VS_LE_RUNNING_STATUS_EVT_SUBCODE                     (0xC3)


#ifdef __cplusplus
}
#endif
