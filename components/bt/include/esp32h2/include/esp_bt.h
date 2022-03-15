/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_BT_H__
#define __ESP_BT_H__

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "sdkconfig.h"
#include "esp_task.h"

#include "nimble/nimble_npl.h"
#include "syscfg/syscfg.h"
#include "esp_nimble_cfg.h"
#include "nimble/ble.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Bluetooth mode for controller enable/disable
 */

typedef enum {
    ESP_BT_MODE_IDLE       = 0x00,   /*!< Bluetooth is not running */
    ESP_BT_MODE_BLE        = 0x01,   /*!< Run BLE mode */
    ESP_BT_MODE_CLASSIC_BT = 0x02,   /*!< Run Classic BT mode */
    ESP_BT_MODE_BTDM       = 0x03,   /*!< Run dual mode */
} esp_bt_mode_t;

/**
 * @brief Bluetooth controller enable/disable/initialised/de-initialised status
 */

typedef enum {
    ESP_BT_CONTROLLER_STATUS_IDLE = 0,   /*!< Controller is in idle state */
    ESP_BT_CONTROLLER_STATUS_INITED,     /*!< Controller is in initialising state */
    ESP_BT_CONTROLLER_STATUS_ENABLED,    /*!< Controller is in enabled state */
    ESP_BT_CONTROLLER_STATUS_NUM,        /*!< Controller is in disabled state */
} esp_bt_controller_status_t;

#define CONFIG_VERSION  0x02109228
#define CONFIG_MAGIC    0x5A5AA5A5

/**
 * @brief Controller config options, depend on config mask.
 *        Config mask indicate which functions enabled, this means
 *        some options or parameters of some functions enabled by config mask.
 */

struct esp_bt_controller_config_t {
    uint32_t config_version;
    uint16_t ble_ll_resolv_list_size;
    uint16_t ble_hci_evt_hi_buf_count;
    uint16_t ble_hci_evt_lo_buf_count;
    uint8_t ble_ll_sync_list_cnt;
    uint8_t ble_ll_sync_cnt;
    uint16_t ble_ll_rsp_dup_list_count;
    uint16_t ble_ll_adv_dup_list_count;
    uint8_t ble_ll_tx_pwr_dbm;
    uint64_t rtc_freq;
    uint16_t ble_ll_sca;
    uint8_t ble_ll_scan_phy_number;
    uint16_t ble_ll_conn_def_auth_pyld_tmo;
    uint8_t ble_ll_jitter_usecs;
    uint16_t ble_ll_sched_max_adv_pdu_usecs;
    uint16_t ble_ll_sched_direct_adv_max_usecs;
    uint16_t ble_ll_sched_adv_max_usecs;
    uint16_t ble_scan_rsp_data_max_len;
    uint8_t ble_ll_cfg_num_hci_cmd_pkts;
    uint32_t ble_ll_ctrl_proc_timeout_ms;
    uint16_t nimble_max_connections;
    uint8_t ble_whitelist_size;
    uint16_t ble_acl_buf_size;
    uint16_t ble_acl_buf_count;
    uint16_t ble_hci_evt_buf_size;
    uint16_t ble_multi_adv_instances;
    uint16_t ble_ext_adv_max_size;
    uint16_t controller_task_stack_size;
    uint8_t controller_task_prio;
    uint8_t controller_run_cpu;
    uint8_t enable_qa_test;
    uint8_t enable_bqb_test;
    uint8_t enable_uart_hci;
    uint8_t ble_hci_uart_port;
    uint32_t ble_hci_uart_baud;
    uint8_t ble_hci_uart_data_bits;
    uint8_t ble_hci_uart_stop_bits;
    uint8_t ble_hci_uart_flow_ctrl;
    uint8_t ble_hci_uart_uart_parity;
    uint8_t enable_tx_cca;
    uint8_t cca_rssi_thresh;
    uint8_t sleep_en;
    uint8_t coex_phy_coded_tx_rx_time_limit;
    uint32_t config_magic;
};

#ifdef CONFIG_BT_NIMBLE_RUN_BQB_TEST
#define RUN_BQB_TEST CONFIG_BT_NIMBLE_RUN_BQB_TEST
#else
#define RUN_BQB_TEST 0
#endif

#ifdef CONFIG_BT_NIMBLE_RUN_QA_TEST
#define RUN_QA_TEST CONFIG_BT_NIMBLE_RUN_QA_TEST
#else
#define RUN_QA_TEST 0
#endif

#ifdef CONFIG_BT_NIMBLE_CONTROL_USE_UART_HCI
#define HCI_UART_EN CONFIG_BT_NIMBLE_CONTROL_USE_UART_HCI
#else
#define HCI_UART_EN 0 // hci ram mode
#endif

#ifdef CONFIG_BT_NIMBLE_SLEEP_ENABLE
#define NIMBLE_SLEEP_ENABLE CONFIG_BT_NIMBLE_SLEEP_ENABLE
#else
#define NIMBLE_SLEEP_ENABLE  0
#endif

#define BT_CONTROLLER_INIT_CONFIG_DEFAULT() {                                           \
    .config_version = CONFIG_VERSION,                                                   \
    .ble_ll_resolv_list_size = MYNEWT_VAL(BLE_LL_RESOLV_LIST_SIZE),                     \
    .ble_hci_evt_hi_buf_count = MYNEWT_VAL(BLE_HCI_EVT_HI_BUF_COUNT),                   \
    .ble_hci_evt_lo_buf_count = MYNEWT_VAL(BLE_HCI_EVT_LO_BUF_COUNT),                   \
    .ble_ll_sync_list_cnt = BLE_LL_SYNC_LIST_CNT_N,                                     \
    .ble_ll_sync_cnt = BLE_LL_SYNC_CNT_N,                                               \
    .ble_ll_rsp_dup_list_count = CONFIG_BT_NIMBLE_LL_DUP_SCAN_LIST_COUNT,               \
    .ble_ll_adv_dup_list_count = CONFIG_BT_NIMBLE_LL_DUP_SCAN_LIST_COUNT,               \
    .ble_ll_tx_pwr_dbm = BLE_LL_TX_PWR_DBM_N,                                           \
    .rtc_freq = RTC_FREQ_N,                                                             \
    .ble_ll_sca = MYNEWT_VAL(BLE_LL_SCA),                                               \
    .ble_ll_scan_phy_number = BLE_LL_SCAN_PHY_NUMBER_N,                                 \
    .ble_ll_conn_def_auth_pyld_tmo = BLE_LL_CONN_DEF_AUTH_PYLD_TMO_N,                   \
    .ble_ll_jitter_usecs = BLE_LL_JITTER_USECS_N,                                       \
    .ble_ll_sched_max_adv_pdu_usecs = BLE_LL_SCHED_MAX_ADV_PDU_USECS_N,                 \
    .ble_ll_sched_direct_adv_max_usecs = BLE_LL_SCHED_DIRECT_ADV_MAX_USECS_N,           \
    .ble_ll_sched_adv_max_usecs = BLE_LL_SCHED_ADV_MAX_USECS_N,                         \
    .ble_scan_rsp_data_max_len = BLE_SCAN_RSP_DATA_MAX_LEN_N,                           \
    .ble_ll_cfg_num_hci_cmd_pkts = BLE_LL_CFG_NUM_HCI_CMD_PKTS_N,                       \
    .ble_ll_ctrl_proc_timeout_ms = BLE_LL_CTRL_PROC_TIMEOUT_MS_N,                       \
    .nimble_max_connections = MYNEWT_VAL(BLE_MAX_CONNECTIONS),                          \
    .ble_whitelist_size = MYNEWT_VAL(BLE_LL_WHITELIST_SIZE),                            \
    .ble_acl_buf_size = MYNEWT_VAL(BLE_ACL_BUF_SIZE),                                   \
    .ble_acl_buf_count = MYNEWT_VAL(BLE_ACL_BUF_COUNT),                                 \
    .ble_hci_evt_buf_size = MYNEWT_VAL(BLE_HCI_EVT_BUF_SIZE),                           \
    .ble_multi_adv_instances = MYNEWT_VAL(BLE_MULTI_ADV_INSTANCES),                     \
    .ble_ext_adv_max_size = MYNEWT_VAL(BLE_EXT_ADV_MAX_SIZE),                           \
    .controller_task_stack_size = NIMBLE_LL_STACK_SIZE,                                 \
    .controller_task_prio       = CONFIG_BT_NIMBLE_CONTROLLER_TASK_PRIORITY,            \
    .controller_run_cpu         = 0,                                                    \
    .enable_qa_test             = RUN_QA_TEST,                                  \
    .enable_bqb_test            = RUN_BQB_TEST,                                     \
    .enable_uart_hci            = HCI_UART_EN,                                          \
    .ble_hci_uart_port          = MYNEWT_VAL(BLE_HCI_UART_PORT),                        \
    .ble_hci_uart_baud          = MYNEWT_VAL(BLE_HCI_UART_BAUD),                    \
    .ble_hci_uart_data_bits     = MYNEWT_VAL(BLE_HCI_UART_DATA_BITS),                   \
    .ble_hci_uart_stop_bits     = MYNEWT_VAL(BLE_HCI_UART_STOP_BITS),                   \
    .ble_hci_uart_flow_ctrl     = MYNEWT_VAL(BLE_HCI_UART_FLOW_CTRL),                   \
    .ble_hci_uart_uart_parity   = MYNEWT_VAL(BLE_HCI_UART_PARITY),                      \
    .enable_tx_cca              = MYNEWT_VAL(BLE_TX_CCA_ENABLED),                       \
    .cca_rssi_thresh            = 256 - MYNEWT_VAL(BLE_CCA_RSSI_THRESH),                \
    .sleep_en                   = NIMBLE_SLEEP_ENABLE,                                  \
    .coex_phy_coded_tx_rx_time_limit = CONFIG_BT_NIMBLE_COEX_PHY_CODED_TX_RX_TLIM_EFF,  \
    .config_magic = CONFIG_MAGIC,                                                       \
};

esp_err_t esp_bt_controller_init(struct esp_bt_controller_config_t *cfg);
esp_err_t esp_bt_controller_deinit(void);
esp_err_t esp_bt_controller_enable(esp_bt_mode_t mode);
esp_err_t esp_bt_controller_disable(void);
esp_err_t esp_bt_mem_release(esp_bt_mode_t mode);
/* Returns random static address or -1 if not present */
extern int esp_ble_hw_get_static_addr(ble_addr_t *addr);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_BT_H__ */
