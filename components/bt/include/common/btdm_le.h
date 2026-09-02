/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "btdm_user_cfg.h"
#include "ble_user_cfg.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 ***************************************************************************************************
 * Local Defined Macros
 ***************************************************************************************************
 */
#define BLE_CONFIG_VERSION 0x20260810
#define BLE_CONFIG_MAGIC 0x5A5AA5A5

#if defined(CONFIG_BT_LE_ISO_SUPPORT)
#define DEFAULT_BT_LE_ISO_ENABLED 1
#if defined(CONFIG_BT_LE_ISO_FRA_UNSEG)
#define DEFAULT_BT_LE_ISO_FRA_UNSEG CONFIG_BT_LE_ISO_FRA_UNSEG
#else
#define DEFAULT_BT_LE_ISO_FRA_UNSEG (0)
#endif
#if defined(CONFIG_BT_LE_ISO_NSFC_EN)
#define DEFAULT_BT_LE_ISO_NSFC_EN CONFIG_BT_LE_ISO_NSFC_EN
#define DEFAULT_BT_LE_ISO_NSFC_NUM CONFIG_BT_LE_ISO_NSFC_NUM
#else
#define DEFAULT_BT_LE_ISO_NSFC_EN (0)
#define DEFAULT_BT_LE_ISO_NSFC_NUM (0)
#endif
#define DEFAULT_BT_LE_ISO_BUF_COUNT CONFIG_BT_LE_ISO_BUF_COUNT
#define DEFAULT_BT_LE_ISO_BUF_SIZE CONFIG_BT_LE_ISO_BUF_SIZE
#define DEFAULT_BT_LE_ISO_BIG CONFIG_BT_LE_ISO_BIG
#define DEFAULT_BT_LE_ISO_BIS CONFIG_BT_LE_ISO_BIS
#define DEFAULT_BT_LE_ISO_BIS_PER_BIG CONFIG_BT_LE_ISO_BIS_PER_BIG
#define DEFAULT_BT_LE_ISO_CIG CONFIG_BT_LE_ISO_CIG
#define DEFAULT_BT_LE_ISO_CIS CONFIG_BT_LE_ISO_CIS
#define DEFAULT_BT_LE_ISO_CIS_PER_CIG CONFIG_BT_LE_ISO_CIS_PER_CIG

#define DEFAULT_BT_LE_ISO_CONFIG                                                                   \
    &(esp_bt_ctrl_le_iso_config_t){                                                                \
        .iso_enabled = DEFAULT_BT_LE_ISO_ENABLED,                                                  \
        .iso_fra_unseg = DEFAULT_BT_LE_ISO_FRA_UNSEG,                                              \
        .iso_nsfc_en = DEFAULT_BT_LE_ISO_NSFC_EN,                                                  \
        .iso_nsfc_num = DEFAULT_BT_LE_ISO_NSFC_NUM,                                                \
        .iso_buf_count = DEFAULT_BT_LE_ISO_BUF_COUNT,                                              \
        .iso_buf_size = DEFAULT_BT_LE_ISO_BUF_SIZE,                                                \
        .iso_big_count = DEFAULT_BT_LE_ISO_BIG,                                                    \
        .iso_bis_count = DEFAULT_BT_LE_ISO_BIS,                                                    \
        .iso_bis_per_big = DEFAULT_BT_LE_ISO_BIS_PER_BIG,                                          \
        .iso_cig_count = DEFAULT_BT_LE_ISO_CIG,                                                    \
        .iso_cis_count = DEFAULT_BT_LE_ISO_CIS,                                                    \
        .iso_cis_per_cig = DEFAULT_BT_LE_ISO_CIS_PER_CIG,                                          \
    }
#else // defined(CONFIG_BT_LE_ISO_SUPPORT)
#define DEFAULT_BT_LE_ISO_ENABLED 0
#define DEFAULT_BT_LE_ISO_FRA_UNSEG 0
#define DEFAULT_BT_LE_ISO_NSFC_EN 0
#define DEFAULT_BT_LE_ISO_NSFC_NUM 0
#define DEFAULT_BT_LE_ISO_BUF_COUNT 0
#define DEFAULT_BT_LE_ISO_BUF_SIZE 0
#define DEFAULT_BT_LE_ISO_BIG 0
#define DEFAULT_BT_LE_ISO_BIS 0
#define DEFAULT_BT_LE_ISO_BIS_PER_BIG 0
#define DEFAULT_BT_LE_ISO_CIG 0
#define DEFAULT_BT_LE_ISO_CIS 0
#define DEFAULT_BT_LE_ISO_CIS_PER_CIG 0

#define DEFAULT_BT_LE_ISO_CONFIG NULL
#endif // defined(CONFIG_BT_LE_ISO_SUPPORT)

#if UC_BT_CTRL_LE_SUPPORT_MEM_RES
#define DEFAULT_BT_LE_MEM_RES_CONFIG                                                               \
        .enhanced_mem_resv = 0,                                                                    \
        .rxbuf_reserved = 0,
#else
#define DEFAULT_BT_LE_MEM_RES_CONFIG
#endif // UC_BT_CTRL_LE_SUPPORT_MEM_RES

#if UC_BT_CTRL_BLE_IS_ENABLE
#define _BT_CTRL_LE_INIT_CONFIG_DEFAULT()                                                          \
    {                                                                                              \
        .config_version = BLE_CONFIG_VERSION,                                                      \
        .ble_ll_resolv_list_size = CONFIG_BT_LE_LL_RESOLV_LIST_SIZE,                               \
        .ble_hci_evt_hi_buf_count = DEFAULT_BT_LE_HCI_EVT_HI_BUF_COUNT,                            \
        .ble_hci_evt_lo_buf_count = DEFAULT_BT_LE_HCI_EVT_LO_BUF_COUNT,                            \
        .ble_ll_sync_list_cnt = DEFAULT_BT_LE_MAX_PERIODIC_ADVERTISER_LIST,                        \
        .ble_ll_sync_cnt = DEFAULT_BT_LE_MAX_PERIODIC_SYNCS,                                       \
        .ble_ll_rsp_dup_list_count = CONFIG_BT_LE_LL_DUP_SCAN_LIST_COUNT,                          \
        .ble_ll_adv_dup_list_count = CONFIG_BT_LE_LL_DUP_SCAN_LIST_COUNT,                          \
        .ble_ll_tx_pwr_dbm = UC_BT_CTRL_LE_DFT_TX_POWER_LEVEL_DBM_EFF,                             \
        .rtc_freq = 32000,                                                                         \
        .ble_ll_sca = CONFIG_BT_LE_LL_SCA,                                                         \
        .ble_ll_scan_phy_number = BLE_LL_SCAN_PHY_NUMBER_N,                                        \
        .ble_ll_conn_def_auth_pyld_tmo = BLE_LL_CONN_DEF_AUTH_PYLD_TMO_N,                          \
        .ble_ll_jitter_usecs = BLE_LL_JITTER_USECS_N,                                              \
        .ble_ll_sched_max_adv_pdu_usecs = BLE_LL_SCHED_MAX_ADV_PDU_USECS_N,                        \
        .ble_ll_sched_direct_adv_max_usecs = BLE_LL_SCHED_DIRECT_ADV_MAX_USECS_N,                  \
        .ble_ll_sched_adv_max_usecs = BLE_LL_SCHED_ADV_MAX_USECS_N,                                \
        .ble_scan_rsp_data_max_len = DEFAULT_BT_LE_SCAN_RSP_DATA_MAX_LEN_N,                        \
        .ble_ll_cfg_num_hci_cmd_pkts = BLE_LL_CFG_NUM_HCI_CMD_PKTS_N,                              \
        .ble_ll_ctrl_proc_timeout_ms = BLE_LL_CTRL_PROC_TIMEOUT_MS_N,                              \
        .nimble_max_connections = DEFAULT_BT_LE_MAX_CONNECTIONS,                                   \
        .ble_whitelist_size = DEFAULT_BT_NIMBLE_WHITELIST_SIZE,                                    \
        .ble_acl_buf_size = DEFAULT_BT_LE_ACL_BUF_SIZE,                                            \
        .ble_acl_buf_count = DEFAULT_BT_LE_ACL_BUF_COUNT,                                          \
        .ble_hci_evt_buf_size = DEFAULT_BT_LE_HCI_EVT_BUF_SIZE,                                    \
        .ble_multi_adv_instances = DEFAULT_BT_LE_MAX_EXT_ADV_INSTANCES,                            \
        .ble_ext_adv_max_size = DEFAULT_BT_LE_EXT_ADV_MAX_SIZE,                                    \
        .controller_task_stack_size = UC_BT_CTRL_TASK_STACK_SIZE,                                  \
        .controller_task_prio = ESP_TASK_BT_CONTROLLER_PRIO,                                       \
        .controller_run_cpu = 0,                                                                   \
        .enable_qa_test = RUN_QA_TEST,                                                             \
        .enable_bqb_test = RUN_BQB_TEST,                                                           \
        .enable_tx_cca = DEFAULT_BT_LE_TX_CCA_ENABLED,                                             \
        .cca_rssi_thresh = 256 - DEFAULT_BT_LE_CCA_RSSI_THRESH,                                    \
        .sleep_en = UC_BT_CTRL_SLEEP_ENABLE,                                                       \
        .coex_phy_coded_tx_rx_time_limit = DEFAULT_BT_LE_COEX_PHY_CODED_TX_RX_TLIM_EFF,            \
        .dis_scan_backoff = NIMBLE_DISABLE_SCAN_BACKOFF,                                           \
        .ble_scan_classify_filter_enable = 1,                                                      \
        .main_xtal_freq = CONFIG_XTAL_FREQ,                                                        \
        .ignore_wl_for_direct_adv = 0,                                                             \
        .enable_pcl = UC_BT_CTRL_LE_POWER_CONTROL_ENABLED,                                         \
        .csa2_select = UC_BT_CTRL_LE_50_FEATURE_SUPPORT,                                           \
        .enable_csr = UC_BT_CTRL_LE_SUBRATE_ENABLED,                                               \
        .ble_aa_check = UC_BT_CTRL_LE_CHECK_CONNECT_IND_ACCESS_ADDRESS,                            \
        .ble_llcp_disc_flag = UC_BT_CTRL_LLCP_INSTANT_PASSED_DISC_FLAGS,                           \
        .scan_backoff_upperlimitmax = UC_BT_CTRL_LE_SCAN_BACKOFF_UPPERLIMITMAX,                    \
        .ble_chan_ass_en = UC_BT_CTRL_LE_CHAN_ASS_ENABLED,                                         \
        .ble_data_lenth_zero_aux = UC_BT_CTRL_LE_ADV_DATA_LENGTH_ZERO_AUX,                         \
        .vhci_enabled = UC_BT_CTRL_HCI_INTERFACE_USE_RAM,                                          \
        .ptr_check_enabled = UC_BT_CTRL_LE_PTR_CHECK_ENABLED,                                      \
        .ble_adv_tx_options = UC_BT_CTRL_LE_ADV_TX_OPTIONS,                                        \
        .skip_unnecessary_checks_en = 0,                                                           \
        .fast_conn_data_tx_en = UC_BT_CTRL_LE_FAST_CONN_DATA_TX_EN,                                \
        .ch39_txpwr = UC_BT_CTRL_LE_CH39_TX_PWR_DBM,                                               \
        .adv_rsv_cnt = UC_BT_CTRL_LE_ADV_SM_RESERVE_CNT,                                           \
        .conn_rsv_cnt = UC_BT_CTRL_LE_CONN_SM_RESERVE_CNT,                                         \
        .priority_level_cfg = UC_BT_CTRL_LE_SCHED_PRIO_LVL_CFG,                                    \
        .slv_fst_rx_lat_en = UC_BT_CTRL_LE_SLV_FAST_RX_CONN_DATA_EN,                               \
        .dl_itvl_phy_sync_en = UC_BT_CTRL_LE_DL_ITVL_PHY_SYNC_EN,                                  \
        .scan_allow_adi_filter = UC_BT_CTRL_LE_SCAN_ENH_ADI_FILTER,                                \
        DEFAULT_BT_LE_MEM_RES_CONFIG                                                               \
        .iso_config = DEFAULT_BT_LE_ISO_CONFIG,                                                    \
        .config_magic = BLE_CONFIG_MAGIC,                                                          \
    }
#else
#define _BT_CTRL_LE_INIT_CONFIG_DEFAULT() {0}
#endif // UC_BT_CTRL_BLE_IS_ENABLE

/*
 ***************************************************************************************************
 * Type Definitions
 ***************************************************************************************************
 */
typedef enum esp_power_level esp_power_level_t;
typedef enum esp_ble_power_type esp_ble_power_type_t;

/**
 * @brief The enhanced type of which tx power, could set Advertising/Connection/Default and etc.
 */
typedef enum {
    ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT = 0,
    ESP_BLE_ENHANCED_PWR_TYPE_ADV,
    ESP_BLE_ENHANCED_PWR_TYPE_SCAN,
    ESP_BLE_ENHANCED_PWR_TYPE_INIT,
    ESP_BLE_ENHANCED_PWR_TYPE_CONN,
    ESP_BLE_ENHANCED_PWR_TYPE_MAX,
} esp_ble_enhanced_power_type_t;

#if SOC_BLE_ISO_SUPPORTED
/**
 * @brief Bluetooth LE controller ISO configuration options
 */
typedef struct {
    bool iso_enabled;        /*!< Enable ISO support */
    bool iso_fra_unseg;      /*!< Enable ISO FRA unsegmented */
    bool iso_nsfc_en;        /*!< Enable ISO NSF-C */
    uint8_t iso_nsfc_num;    /*!< ISO NSF-C number */
    uint8_t iso_buf_count;   /*!< ISO buffer count */
    uint16_t iso_buf_size;   /*!< ISO buffer size */
    uint8_t iso_big_count;   /*!< ISO BIG count */
    uint16_t iso_bis_count;  /*!< ISO BIS count */
    uint8_t iso_bis_per_big; /*!< ISO BIS per BIG */
    uint8_t iso_cig_count;   /*!< ISO CIG count */
    uint16_t iso_cis_count;  /*!< ISO CIS count */
    uint8_t iso_cis_per_cig; /*!< ISO CIS per CIG */
} esp_bt_ctrl_le_iso_config_t;
#endif // SOC_BLE_ISO_SUPPORTED

/**
 * @brief Bluetooth LE controller configuration options
 */
typedef struct {
    uint32_t config_version;            /*!< Version number of the defined structure */
    uint16_t ble_ll_resolv_list_size;   /*!< Size of the resolvable private address list */
    uint16_t ble_hci_evt_hi_buf_count;  /*!< Count of high buffers for HCI events */
    uint16_t ble_hci_evt_lo_buf_count;  /*!< Count of low buffers for HCI events */
    uint8_t ble_ll_sync_list_cnt;       /*!< Number of synchronization lists */
    uint8_t ble_ll_sync_cnt;            /*!< Number of synchronizations */
    uint16_t ble_ll_rsp_dup_list_count; /*!< Count of duplicated lists for scan response packets */
    uint16_t ble_ll_adv_dup_list_count; /*!< Count of duplicated lists for advertising packets */
    uint8_t ble_ll_tx_pwr_dbm;          /*!< Tx power (transmit power) in dBm */
    uint64_t rtc_freq;                  /*!< Frequency of RTC (Real-Time Clock) */
    uint16_t ble_ll_sca;                /*!< Sleep Clock Accuracy (SCA) parameter */
    uint8_t ble_ll_scan_phy_number;     /*!< Number of PHYs supported for scanning */
    uint16_t
        ble_ll_conn_def_auth_pyld_tmo; /*!< Connection default authentication payload timeout */
    uint8_t ble_ll_jitter_usecs;       /*!< Jitter time in microseconds */
    uint16_t ble_ll_sched_max_adv_pdu_usecs; /*!< Maximum time in microseconds for advertising PDU
                                                scheduling */
    uint16_t ble_ll_sched_direct_adv_max_usecs; /*!< Maximum time in microseconds for directed
                                                   advertising scheduling */
    uint16_t
        ble_ll_sched_adv_max_usecs; /*!< Maximum time in microseconds for advertising scheduling */
    uint16_t ble_scan_rsp_data_max_len;   /*!< Maximum length of scan response data */
    uint8_t ble_ll_cfg_num_hci_cmd_pkts;  /*!< Number of HCI command packets that can be queued */
    uint32_t ble_ll_ctrl_proc_timeout_ms; /*!< Control processing timeout in milliseconds */
    uint16_t nimble_max_connections;      /*!< Maximum number of connections supported */
    uint8_t ble_whitelist_size;           /*!< Size of the white list */
    uint16_t ble_acl_buf_size;        /*!< Buffer size of ACL (Asynchronous Connection-Less) data */
    uint16_t ble_acl_buf_count;       /*!< Buffer count of ACL data */
    uint16_t ble_hci_evt_buf_size;    /*!< Buffer size for HCI event data */
    uint16_t ble_multi_adv_instances; /*!< Number of advertising instances */
    uint16_t ble_ext_adv_max_size;    /*!< Maximum size of extended advertising data */
    uint16_t
        controller_task_stack_size; /*!< Size of Bluetooth controller task stack, to be removed */
    uint8_t controller_task_prio;   /*!< Priority of the Bluetooth task, to be removed  */
    uint8_t controller_run_cpu; /*!< CPU number on which the Bluetooth controller task runs, to be
                                   removed */
    uint8_t enable_qa_test;     /*!< Enable for QA test */
    uint8_t enable_bqb_test;    /*!< Enable for BQB test */
    uint8_t enable_tx_cca;      /*!< Enable Clear Channel Assessment (CCA) when transmitting */
    uint8_t cca_rssi_thresh;    /*!< RSSI threshold for CCA */
    uint8_t sleep_en;           /*!< Enable sleep functionality */
    uint8_t coex_phy_coded_tx_rx_time_limit; /*!< Coexistence PHY coded TX and RX time limit */
    uint8_t dis_scan_backoff;                /*!< Disable scan backoff */
    uint8_t ble_scan_classify_filter_enable; /*!< Enable classification filter for BLE scan */
    uint8_t cca_drop_mode;                   /*!< CCA drop mode */
    int8_t cca_low_tx_pwr;                   /*!< Low TX power setting for CCA */
    uint8_t main_xtal_freq;                  /*!< Main crystal frequency */
    uint8_t ignore_wl_for_direct_adv;        /*!< Ignore the white list for directed advertising */
    uint8_t enable_pcl;                      /*!< Enable power control */
    uint8_t csa2_select;                     /*!< Select CSA#2*/
    uint8_t enable_csr;                      /*!< Enable CSR */
    uint8_t ble_aa_check; /*!< True if adds a verification step for the Access Address within the
                             CONNECT_IND PDU; false otherwise. Configurable in menuconfig */
    uint8_t ble_llcp_disc_flag; /*!< Flag indicating whether the Controller disconnects after
                                   Instant Passed (0x28) error occurs. Configurable in menuconfig.
                                    - The Controller does not disconnect after Instant Passed (0x28)
                                   by default. */
    uint16_t scan_backoff_upperlimitmax; /*!< The value of upperlimitmax is 2^n, The maximum value
                                            is 256 */
    uint8_t
        ble_chan_ass_en; /*!< Enable / disable BLE channel assessment. Configurable in menuconfig.
                             - 0 - Disable
                             - 1 - Enable (default) */
    uint8_t ble_data_lenth_zero_aux; /*!< Enable / disable auxiliary packets when the extended ADV
                                        data length is zero. Configurable in menuconfig.
                                         - 0 - Disable (default)
                                         - 1 - Enable */
    uint8_t vhci_enabled;            /*!< VHCI is enabled */
    uint8_t ptr_check_enabled;       /*!< Enable boundary check for internal memory. */
    uint8_t ble_adv_tx_options;      /*!< The options for Extended advertising sending. */
    uint8_t skip_unnecessary_checks_en; /*!< The option to skip non-fatal state checks and perform
                                           extra handling for fatal checks. */
    uint8_t fast_conn_data_tx_en;       /*!< The option for fast transmission of connection data
                                            - 0 - Disable
                                            - 1 - Enable (default) */
    int8_t ch39_txpwr;   /*!< BLE transmit power (in dBm) used for BLE advertising on channel 39. */
    uint8_t adv_rsv_cnt; /*!< BLE adv state machine reserve count number */
    uint8_t conn_rsv_cnt;       /*!< BLE conn state machine reserve count number */
    uint8_t priority_level_cfg; /*!< The option for priority level configuration */
    uint8_t
        slv_fst_rx_lat_en; /*!< The option for enabling slave fast PDU reception during latency. */
    uint8_t dl_itvl_phy_sync_en;   /*!< The option for automatically initiate the data length update
                                      when phy update or connect interval update. */
    uint8_t scan_allow_adi_filter; /*!< The option for ext scan to allow PDU with specific adi. */
#if UC_BT_CTRL_LE_SUPPORT_MEM_RES
    uint8_t enhanced_mem_resv; /*!< The option masks the BLE events with all reserved memory.*/
    uint8_t rxbuf_reserved;    /*!< The option reserve all Rxbuffer memory at initialization. */
#endif // UC_BT_CTRL_LE_SUPPORT_MEM_RES
#if SOC_BLE_ISO_SUPPORTED
    esp_bt_ctrl_le_iso_config_t *iso_config; /*!< The option for ISO configuration */
#endif // SOC_BLE_ISO_SUPPORTED
    uint32_t config_magic;                   /*!< Configuration magic value */
} esp_bt_ctrl_le_config_t;

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
 * @param  power_type : The enhanced type of which tx power, could set
 * Advertising/Connection/Default and etc
 * @param  handle : The handle of Advertising or Connection and the value 0 for other enhanced power
 * types.
 * @param  power_level: Power level(index) corresponding to absolute value(dbm)
 * @return              ESP_OK - success, other - failed
 */
esp_err_t esp_ble_tx_power_set_enhanced(esp_ble_enhanced_power_type_t power_type, uint16_t handle,
                                        esp_power_level_t power_level);

/**
 * @brief  ENHANCED API of Getting BLE TX power
 *         Connection Tx power should only be get after connection created.
 * @param  power_type : The enhanced type of which tx power, could set
 * Advertising/Connection/Default and etc
 * @param  handle : The handle of Advertising or Connection and the value 0 for other enhanced power
 * types.
 * @return             >= 0 - Power level, < 0 - Invalid
 */
esp_power_level_t esp_ble_tx_power_get_enhanced(esp_ble_enhanced_power_type_t power_type,
                                                uint16_t handle);

#ifdef __cplusplus
}
#endif
