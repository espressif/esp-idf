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
#include "esp_bt_cfg.h"

#ifdef CONFIG_BT_LE_HCI_INTERFACE_USE_UART
#include "driver/uart.h"
#endif


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

/**
 * @brief BLE tx power type
 *        ESP_BLE_PWR_TYPE_CONN_HDL0-8: for each connection, and only be set after connection completed.
 *                                      when disconnect, the correspond TX power is not effected.
 *        ESP_BLE_PWR_TYPE_ADV : for advertising/scan response.
 *        ESP_BLE_PWR_TYPE_SCAN : for scan.
 *        ESP_BLE_PWR_TYPE_DEFAULT : if each connection's TX power is not set, it will use this default value.
 *                                   if neither in scan mode nor in adv mode, it will use this default value.
 *        If none of power type is set, system will use ESP_PWR_LVL_P3 as default for ADV/SCAN/CONN0-9.
 */
typedef enum {
    ESP_BLE_PWR_TYPE_CONN_HDL0  = 0,            /*!< For connection handle 0 */
    ESP_BLE_PWR_TYPE_CONN_HDL1  = 1,            /*!< For connection handle 1 */
    ESP_BLE_PWR_TYPE_CONN_HDL2  = 2,            /*!< For connection handle 2 */
    ESP_BLE_PWR_TYPE_CONN_HDL3  = 3,            /*!< For connection handle 3 */
    ESP_BLE_PWR_TYPE_CONN_HDL4  = 4,            /*!< For connection handle 4 */
    ESP_BLE_PWR_TYPE_CONN_HDL5  = 5,            /*!< For connection handle 5 */
    ESP_BLE_PWR_TYPE_CONN_HDL6  = 6,            /*!< For connection handle 6 */
    ESP_BLE_PWR_TYPE_CONN_HDL7  = 7,            /*!< For connection handle 7 */
    ESP_BLE_PWR_TYPE_CONN_HDL8  = 8,            /*!< For connection handle 8 */
    ESP_BLE_PWR_TYPE_ADV        = 9,            /*!< For advertising */
    ESP_BLE_PWR_TYPE_SCAN       = 10,           /*!< For scan */
    ESP_BLE_PWR_TYPE_DEFAULT    = 11,           /*!< For default, if not set other, it will use default value */
    ESP_BLE_PWR_TYPE_NUM        = 12,           /*!< TYPE numbers */
} esp_ble_power_type_t;

/**
 * @brief Bluetooth TX power level(index), it's just a index corresponding to power(dbm).
 */
typedef enum {
    ESP_PWR_LVL_N27 = 0,              /*!< Corresponding to -27dbm */
    ESP_PWR_LVL_N24 = 1,              /*!< Corresponding to -24dbm */
    ESP_PWR_LVL_N21 = 2,              /*!< Corresponding to -21dbm */
    ESP_PWR_LVL_N18 = 3,              /*!< Corresponding to -18dbm */
    ESP_PWR_LVL_N15 = 4,              /*!< Corresponding to -15dbm */
    ESP_PWR_LVL_N12 = 5,              /*!< Corresponding to -12dbm */
    ESP_PWR_LVL_N9  = 6,              /*!< Corresponding to  -9dbm */
    ESP_PWR_LVL_N6  = 7,              /*!< Corresponding to  -6dbm */
    ESP_PWR_LVL_N3  = 8,              /*!< Corresponding to  -3dbm */
    ESP_PWR_LVL_N0  = 9,              /*!< Corresponding to   0dbm */
    ESP_PWR_LVL_P3  = 10,             /*!< Corresponding to  +3dbm */
    ESP_PWR_LVL_P6  = 11,             /*!< Corresponding to  +6dbm */
    ESP_PWR_LVL_P9  = 12,             /*!< Corresponding to  +9dbm */
    ESP_PWR_LVL_P12 = 13,             /*!< Corresponding to  +12dbm */
    ESP_PWR_LVL_P15 = 14,             /*!< Corresponding to  +15dbm */
    ESP_PWR_LVL_P18 = 15,             /*!< Corresponding to  +18dbm */
    ESP_PWR_LVL_INVALID = 0xFF,         /*!< Indicates an invalid value */
} esp_power_level_t;

typedef enum {
    ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT = 0,
    ESP_BLE_ENHANCED_PWR_TYPE_ADV,
    ESP_BLE_ENHANCED_PWR_TYPE_SCAN,
    ESP_BLE_ENHANCED_PWR_TYPE_INIT,
    ESP_BLE_ENHANCED_PWR_TYPE_CONN,
    ESP_BLE_ENHANCED_PWR_TYPE_MAX,
} esp_ble_enhanced_power_type_t;

typedef struct {
    uint8_t type;
    uint8_t val[6];
} esp_ble_addr_t;

/**
 * @brief  Set BLE TX power
 *         Connection Tx power should only be set after connection created.
 * @param  power_type : The type of which tx power, could set Advertising/Connection/Default and etc
 * @param  power_level: Power level(index) corresponding to absolute value(dbm)
 * @return              ESP_OK - success, other - failed
 */
esp_err_t esp_ble_tx_power_set(esp_ble_power_type_t power_type, esp_power_level_t power_level);

/**
 * @brief  Get BLE TX power
 *         Connection Tx power should only be get after connection created.
 * @param  power_type : The type of which tx power, could set Advertising/Connection/Default and etc
 * @return             >= 0 - Power level, < 0 - Invalid
 */
esp_power_level_t esp_ble_tx_power_get(esp_ble_power_type_t power_type);


/**
 * @brief  ENHANCED API for Setting BLE TX power
 *         Connection Tx power should only be set after connection created.
 * @param  power_type : The enhanced type of which tx power, could set Advertising/Connection/Default and etc
 * @param  handle : The handle of Advertising or Connection and the value 0 for other enhanced power types.
 * @param  power_level: Power level(index) corresponding to absolute value(dbm)
 * @return              ESP_OK - success, other - failed
 */
esp_err_t esp_ble_tx_power_set_enhanced(esp_ble_enhanced_power_type_t power_type, uint16_t handle, esp_power_level_t power_level);

/**
 * @brief  ENHANCED API of Getting BLE TX power
 *         Connection Tx power should only be get after connection created.
 * @param  power_type : The enhanced type of which tx power, could set Advertising/Connection/Default and etc
 * @param  handle : The handle of Advertising or Connection and the value 0 for other enhanced power types.
 * @return             >= 0 - Power level, < 0 - Invalid
 */
esp_power_level_t esp_ble_tx_power_get_enhanced(esp_ble_enhanced_power_type_t power_type, uint16_t handle);

#define CONFIG_VERSION  0x20220824
#define CONFIG_MAGIC    0x5A5AA5A5

/**
 * @brief Controller config options, depend on config mask.
 *        Config mask indicate which functions enabled, this means
 *        some options or parameters of some functions enabled by config mask.
 */

typedef struct {
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
    uint8_t dis_scan_backoff;
    uint8_t ble_scan_classify_filter_enable;
    uint8_t cca_drop_mode;
    int8_t cca_low_tx_pwr;
    uint8_t main_xtal_freq;
    uint32_t config_magic;
} esp_bt_controller_config_t;


#define BT_CONTROLLER_INIT_CONFIG_DEFAULT() {                                           \
    .config_version = CONFIG_VERSION,                                                   \
    .ble_ll_resolv_list_size = CONFIG_BT_LE_LL_RESOLV_LIST_SIZE,                        \
    .ble_hci_evt_hi_buf_count = DEFAULT_BT_LE_HCI_EVT_HI_BUF_COUNT,                     \
    .ble_hci_evt_lo_buf_count = DEFAULT_BT_LE_HCI_EVT_LO_BUF_COUNT,                     \
    .ble_ll_sync_list_cnt = DEFAULT_BT_LE_MAX_PERIODIC_ADVERTISER_LIST,                 \
    .ble_ll_sync_cnt = DEFAULT_BT_LE_MAX_PERIODIC_SYNCS,                                \
    .ble_ll_rsp_dup_list_count = CONFIG_BT_LE_LL_DUP_SCAN_LIST_COUNT,                   \
    .ble_ll_adv_dup_list_count = CONFIG_BT_LE_LL_DUP_SCAN_LIST_COUNT,                   \
    .ble_ll_tx_pwr_dbm = BLE_LL_TX_PWR_DBM_N,                                           \
    .rtc_freq = RTC_FREQ_N,                                                             \
    .ble_ll_sca = CONFIG_BT_LE_LL_SCA,                                                  \
    .ble_ll_scan_phy_number = BLE_LL_SCAN_PHY_NUMBER_N,                                 \
    .ble_ll_conn_def_auth_pyld_tmo = BLE_LL_CONN_DEF_AUTH_PYLD_TMO_N,                   \
    .ble_ll_jitter_usecs = BLE_LL_JITTER_USECS_N,                                       \
    .ble_ll_sched_max_adv_pdu_usecs = BLE_LL_SCHED_MAX_ADV_PDU_USECS_N,                 \
    .ble_ll_sched_direct_adv_max_usecs = BLE_LL_SCHED_DIRECT_ADV_MAX_USECS_N,           \
    .ble_ll_sched_adv_max_usecs = BLE_LL_SCHED_ADV_MAX_USECS_N,                         \
    .ble_scan_rsp_data_max_len = DEFAULT_BT_LE_SCAN_RSP_DATA_MAX_LEN_N,                 \
    .ble_ll_cfg_num_hci_cmd_pkts = BLE_LL_CFG_NUM_HCI_CMD_PKTS_N,                       \
    .ble_ll_ctrl_proc_timeout_ms = BLE_LL_CTRL_PROC_TIMEOUT_MS_N,                       \
    .nimble_max_connections = DEFAULT_BT_LE_MAX_CONNECTIONS,                            \
    .ble_whitelist_size = DEFAULT_BT_NIMBLE_WHITELIST_SIZE,                             \
    .ble_acl_buf_size = DEFAULT_BT_LE_ACL_BUF_SIZE,                                     \
    .ble_acl_buf_count = DEFAULT_BT_LE_ACL_BUF_COUNT,                                   \
    .ble_hci_evt_buf_size = DEFAULT_BT_LE_HCI_EVT_BUF_SIZE,                             \
    .ble_multi_adv_instances = DEFAULT_BT_LE_MAX_EXT_ADV_INSTANCES,                     \
    .ble_ext_adv_max_size = DEFAULT_BT_LE_EXT_ADV_MAX_SIZE,                             \
    .controller_task_stack_size = NIMBLE_LL_STACK_SIZE,                                 \
    .controller_task_prio       = ESP_TASK_BT_CONTROLLER_PRIO,                          \
    .controller_run_cpu         = 0,                                                    \
    .enable_qa_test             = RUN_QA_TEST,                                          \
    .enable_bqb_test            = RUN_BQB_TEST,                                         \
    .enable_uart_hci            = HCI_UART_EN,                                          \
    .ble_hci_uart_port          = DEFAULT_BT_LE_HCI_UART_PORT,                          \
    .ble_hci_uart_baud          = DEFAULT_BT_LE_HCI_UART_BAUD,                          \
    .ble_hci_uart_data_bits     = DEFAULT_BT_LE_HCI_UART_DATA_BITS,                     \
    .ble_hci_uart_stop_bits     = DEFAULT_BT_LE_HCI_UART_STOP_BITS,                     \
    .ble_hci_uart_flow_ctrl     = DEFAULT_BT_LE_HCI_UART_FLOW_CTRL,                     \
    .ble_hci_uart_uart_parity   = DEFAULT_BT_LE_HCI_UART_PARITY,                        \
    .enable_tx_cca              = DEFAULT_BT_LE_TX_CCA_ENABLED,                         \
    .cca_rssi_thresh            = 256 - DEFAULT_BT_LE_CCA_RSSI_THRESH,                  \
    .cca_drop_mode              = 0,                                                    \
    .cca_low_tx_pwr             = 0,                                                    \
    .sleep_en                   = NIMBLE_SLEEP_ENABLE,                                  \
    .coex_phy_coded_tx_rx_time_limit = DEFAULT_BT_LE_COEX_PHY_CODED_TX_RX_TLIM_EFF,     \
    .ble_scan_classify_filter_enable         = 0,                                       \
    .main_xtal_freq             = CONFIG_XTAL_FREQ,                                     \
    .config_magic = CONFIG_MAGIC,                                                       \
}

esp_err_t esp_bt_controller_init(esp_bt_controller_config_t *cfg);

/**
 * @brief  Get BT controller is initialised/de-initialised/enabled/disabled
 * @return status value
 */
esp_bt_controller_status_t esp_bt_controller_get_status(void);
esp_power_level_t esp_ble_tx_power_get(esp_ble_power_type_t power_type);
esp_err_t esp_bt_controller_deinit(void);
esp_err_t esp_bt_controller_enable(esp_bt_mode_t mode);
esp_err_t esp_bt_controller_disable(void);

typedef struct esp_vhci_host_callback {
    void (*notify_host_send_available)(void);               /*!< callback used to notify that the host can send packet to controller */
    int (*notify_host_recv)(uint8_t *data, uint16_t len);   /*!< callback used to notify that the controller has a packet to send to the host*/
} esp_vhci_host_callback_t;

/** @brief esp_vhci_host_check_send_available
 *  used for check actively if the host can send packet to controller or not.
 *  @return true for ready to send, false means cannot send packet
 */
bool esp_vhci_host_check_send_available(void);

/** @brief esp_vhci_host_send_packet
 * host send packet to controller
 *
 * Should not call this function from within a critical section
 * or when the scheduler is suspended.
 *
 * @param data the packet point
 * @param len the packet length
 */
void esp_vhci_host_send_packet(uint8_t *data, uint16_t len);

/** @brief esp_vhci_host_register_callback
 * register the vhci reference callback
 * struct defined by vhci_host_callback structure.
 * @param callback esp_vhci_host_callback type variable
 * @return ESP_OK - success, ESP_FAIL - failed
 */
esp_err_t esp_vhci_host_register_callback(const esp_vhci_host_callback_t *callback);

/** @brief esp_bt_controller_mem_release
 * release the controller memory as per the mode
 *
 * This function releases the BSS, data and other sections of the controller to heap. The total size is about 70k bytes.
 *
 * esp_bt_controller_mem_release(mode) should be called only before esp_bt_controller_init()
 * or after esp_bt_controller_deinit().
 *
 * Note that once BT controller memory is released, the process cannot be reversed. It means you cannot use the bluetooth
 * mode which you have released by this function.
 *
 * If your firmware will later upgrade the Bluetooth controller mode (BLE -> BT Classic or disabled -> enabled)
 * then do not call this function.
 *
 * If the app calls esp_bt_controller_enable(ESP_BT_MODE_BLE) to use BLE only then it is safe to call
 * esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT) at initialization time to free unused BT Classic memory.
 *
 * If the mode is ESP_BT_MODE_BTDM, then it may be useful to call API esp_bt_mem_release(ESP_BT_MODE_BTDM) instead,
 * which internally calls esp_bt_controller_mem_release(ESP_BT_MODE_BTDM) and additionally releases the BSS and data
 * consumed by the BT/BLE host stack to heap. For more details about usage please refer to the documentation of
 * esp_bt_mem_release() function
 *
 * @param mode : the mode want to release memory
 * @return ESP_OK - success, other - failed
 */
esp_err_t esp_bt_controller_mem_release(esp_bt_mode_t mode);

/** @brief esp_bt_mem_release
 * release controller memory and BSS and data section of the BT/BLE host stack as per the mode
 *
 * This function first releases controller memory by internally calling esp_bt_controller_mem_release().
 * Additionally, if the mode is set to ESP_BT_MODE_BTDM, it also releases the BSS and data consumed by the BT/BLE host stack to heap
 *
 * Note that once BT memory is released, the process cannot be reversed. It means you cannot use the bluetooth
 * mode which you have released by this function.
 *
 * If your firmware will later upgrade the Bluetooth controller mode (BLE -> BT Classic or disabled -> enabled)
 * then do not call this function.
 *
 * If you never intend to use bluetooth in a current boot-up cycle, you can call esp_bt_mem_release(ESP_BT_MODE_BTDM)
 * before esp_bt_controller_init or after esp_bt_controller_deinit.
 *
 * For example, if a user only uses bluetooth for setting the WiFi configuration, and does not use bluetooth in the rest of the product operation".
 * In such cases, after receiving the WiFi configuration, you can disable/deinit bluetooth and release its memory.
 * Below is the sequence of APIs to be called for such scenarios:
 *
 *      esp_bluedroid_disable();
 *      esp_bluedroid_deinit();
 *      esp_bt_controller_disable();
 *      esp_bt_controller_deinit();
 *      esp_bt_mem_release(ESP_BT_MODE_BTDM);
 *
 * @param mode : the mode whose memory is to be released
 * @return ESP_OK - success, other - failed
 */
esp_err_t esp_bt_mem_release(esp_bt_mode_t mode);

/* Returns random static address or -1 if not present */
extern int esp_ble_hw_get_static_addr(esp_ble_addr_t *addr);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_BT_H__ */
