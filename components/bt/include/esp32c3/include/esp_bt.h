/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
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
#include "esp_assert.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief Internal use only
*
* @note Please do not modify this value
*/
#define ESP_BT_CTRL_CONFIG_MAGIC_VAL    0x5A5AA5A5

/**
* @brief Internal use only
*
* @note Please do not modify this value
*/
#define ESP_BT_CTRL_CONFIG_VERSION      0x02509280

/**
* @brief Internal use only
*
* @note Please do not modify this value
*/
#define ESP_BT_HCI_TL_MAGIC_VALUE   0xfadebead

/**
* @brief Internal use only
*
* @note Please do not modify this value
*/
#define ESP_BT_HCI_TL_VERSION       0x00010000

/**
 * @brief Bluetooth Controller mode
 */
typedef enum {
    ESP_BT_MODE_IDLE       = 0x00,   /*!< Bluetooth is not operating. */
    ESP_BT_MODE_BLE        = 0x01,   /*!< Bluetooth is operating in BLE mode. */
    ESP_BT_MODE_CLASSIC_BT = 0x02,   /*!< Unsupported mode */
    ESP_BT_MODE_BTDM       = 0x03,   /*!< Unsupported mode */
} esp_bt_mode_t;

/**
 * @brief BLE Controller HCI transport layer type
 */
typedef enum {
    ESP_BT_CTRL_HCI_TL_UART = 0,      /*!< HCI UART h4 transport layer */
    ESP_BT_CTRL_HCI_TL_VHCI = 1,      /*!< Virtual HCI interface */
} esp_bt_ctrl_hci_tl_t;

/**
 * @brief BLE connection event length computation type
 */
typedef enum {
    ESP_BLE_CE_LEN_TYPE_ORIG = 0,      /*!< original */
    ESP_BLE_CE_LEN_TYPE_CE   = 1,      /*!< use `CE_LEN` parameter from HCI commands */
    ESP_BLE_CE_LEN_TYPE_SD   = 1,      /*!< Espressif vendor defined */
} esp_ble_ce_len_t;

/**
 * @brief Bluetooth modem sleep mode
 */
typedef enum {
    ESP_BT_SLEEP_MODE_NONE = 0,      /*!< Disable modem sleep */
    ESP_BT_SLEEP_MODE_1    = 1,      /*!< Enable modem sleep */
} esp_bt_sleep_mode_t;

/**
 * @brief Bluetooth modem sleep clock source
 *
 * @note If the modem sleep mode is enabled, `ESP_BT_SLEEP_CLOCK_MAIN_XTAL` is the default option and
 *      `ESP_BT_SLEEP_CLOCK_NONE` will become an invalid option.
 */

typedef enum {
    ESP_BT_SLEEP_CLOCK_NONE            = 0,   /*!< Sleep clock not configured */
    ESP_BT_SLEEP_CLOCK_MAIN_XTAL       = 1,   /*!< SoC main crystal */
    ESP_BT_SLEEP_CLOCK_EXT_32K_XTAL    = 2,   /*!< External 32.768kHz crystal */
    ESP_BT_SLEEP_CLOCK_RTC_SLOW        = 3,   /*!< Internal 136kHz RC oscillator */
    ESP_BT_SLEEP_CLOCK_FPGA_32K        = 4,   /*!< Hardwired 32KHz clock temporarily used for FPGA */
} esp_bt_sleep_clock_t;

/**
 * @brief Bluetooth antenna index
 */
enum {
    ESP_BT_ANT_IDX_0 = 0,               /*!< Antenna NO 0  */
    ESP_BT_ANT_IDX_1 = 1,               /*!< Antenna NO 1 */
};

/**
 * @brief Enable / disable the maximum TX/RX time limit for Coded-PHY connections in coexistence with Wi-Fi scenarios
 */
enum {
    ESP_BT_COEX_PHY_CODED_TX_RX_TIME_LIMIT_FORCE_DISABLE = 0,    /*!< Disable the limit */
    ESP_BT_COEX_PHY_CODED_TX_RX_TIME_LIMIT_FORCE_ENABLE,         /*!< Enable the limit */
};

#define ESP_BT_HCI_TL_STATUS_OK            (0)   /*!< HCI_TL TX/RX operation status OK */

/**
 * @brief Callback function for HCI Transport Layer send/receive operations
 */
typedef void (* esp_bt_hci_tl_callback_t) (void *arg, uint8_t status);

#ifdef CONFIG_BT_ENABLED

#define BT_CTRL_BLE_MAX_ACT_LIMIT           10  //Maximum BLE activity limitation
#define SLAVE_CE_LEN_MIN_DEFAULT             5

#ifdef CONFIG_BT_CTRL_SCAN_DUPL_TYPE
#define SCAN_DUPLICATE_TYPE_VALUE  CONFIG_BT_CTRL_SCAN_DUPL_TYPE
#else
#define SCAN_DUPLICATE_TYPE_VALUE  0
#endif

/* normal adv cache size */
#ifdef CONFIG_BT_CTRL_SCAN_DUPL_CACHE_SIZE
#define NORMAL_SCAN_DUPLICATE_CACHE_SIZE            CONFIG_BT_CTRL_SCAN_DUPL_CACHE_SIZE
#else
#define NORMAL_SCAN_DUPLICATE_CACHE_SIZE            20
#endif

#ifndef CONFIG_BT_CTRL_BLE_MESH_SCAN_DUPL_EN
#define CONFIG_BT_CTRL_BLE_MESH_SCAN_DUPL_EN FALSE
#endif

#define SCAN_DUPLICATE_MODE_NORMAL_ADV_ONLY         0
#define SCAN_DUPLICATE_MODE_NORMAL_ADV_MESH_ADV     1

#if CONFIG_BT_CTRL_BLE_MESH_SCAN_DUPL_EN
    #define SCAN_DUPLICATE_MODE                     SCAN_DUPLICATE_MODE_NORMAL_ADV_MESH_ADV
    #ifdef CONFIG_BT_CTRL_MESH_DUPL_SCAN_CACHE_SIZE
    #define MESH_DUPLICATE_SCAN_CACHE_SIZE          CONFIG_BT_CTRL_MESH_DUPL_SCAN_CACHE_SIZE
    #else
    #define MESH_DUPLICATE_SCAN_CACHE_SIZE          50
    #endif
#else
    #define SCAN_DUPLICATE_MODE SCAN_DUPLICATE_MODE_NORMAL_ADV_ONLY
    #define MESH_DUPLICATE_SCAN_CACHE_SIZE          0
#endif

#ifndef CONFIG_BT_CTRL_DUPL_SCAN_CACHE_REFRESH_PERIOD
#define DUPL_SCAN_CACHE_REFRESH_PERIOD 0
#else
#define DUPL_SCAN_CACHE_REFRESH_PERIOD CONFIG_BT_CTRL_DUPL_SCAN_CACHE_REFRESH_PERIOD
#endif

#ifdef CONFIG_BT_CTRL_SCAN_BACKOFF_UPPERLIMITMAX
#define BT_CTRL_SCAN_BACKOFF_UPPERLIMITMAX CONFIG_BT_CTRL_SCAN_BACKOFF_UPPERLIMITMAX
#else
#define BT_CTRL_SCAN_BACKOFF_UPPERLIMITMAX  0
#endif

#ifdef CONFIG_BT_CTRL_AGC_RECORRECT_EN
#define BT_CTRL_AGC_RECORRECT_EN  CONFIG_BT_CTRL_AGC_RECORRECT_EN
// ESP32-S3
#if CONFIG_IDF_TARGET_ESP32S3
#define BT_CTRL_AGC_RECORRECT_NEW       1
#else
//Check if chip target is ESP32-C3 101
#if CONFIG_ESP32C3_REV_MIN_FULL >= 101
#define BT_CTRL_AGC_RECORRECT_NEW       1
#else
#define BT_CTRL_AGC_RECORRECT_NEW       0
#endif // CONFIG_ESP32C3_REV_MIN_FULL >= 101
#endif // CONFIG_IDF_TARGET_ESP32S3

#else
#define BT_CTRL_AGC_RECORRECT_EN        0
#define BT_CTRL_AGC_RECORRECT_NEW       0
#endif

#ifdef CONFIG_BT_CTRL_CODED_AGC_RECORRECT_EN
#define BT_CTRL_CODED_AGC_RECORRECT  CONFIG_BT_CTRL_CODED_AGC_RECORRECT_EN
#else
#define BT_CTRL_CODED_AGC_RECORRECT        0
#endif

#if defined (CONFIG_BT_BLE_50_FEATURES_SUPPORTED) || defined (CONFIG_BT_NIMBLE_50_FEATURE_SUPPORT)
#ifdef CONFIG_BT_BLE_50_FEATURES_SUPPORTED
#define BT_CTRL_50_FEATURE_SUPPORT   (CONFIG_BT_BLE_50_FEATURES_SUPPORTED)
#endif // CONFIG_BT_BLE_50_FEATURES_SUPPORTED
#ifdef CONFIG_BT_NIMBLE_50_FEATURE_SUPPORT
#define BT_CTRL_50_FEATURE_SUPPORT   (CONFIG_BT_NIMBLE_50_FEATURE_SUPPORT)
#endif // CONFIG_BT_NIMBLE_50_FEATURE_SUPPORT
#else
#if defined (CONFIG_BT_BLUEDROID_ENABLED) || defined (CONFIG_BT_NIMBLE_ENABLED)
#define BT_CTRL_50_FEATURE_SUPPORT (0)
#else
#define BT_CTRL_50_FEATURE_SUPPORT (1)
#endif // (CONFIG_BT_BLUEDROID_ENABLED) || (CONFIG_BT_NIMBLE_ENABLED)
#endif // (CONFIG_BT_BLE_50_FEATURES_SUPPORTED) || (CONFIG_BT_NIMBLE_50_FEATURE_SUPPORT)

#if defined(CONFIG_BT_BLE_CCA_MODE)
#define BT_BLE_CCA_MODE (CONFIG_BT_BLE_CCA_MODE)
#else
#define BT_BLE_CCA_MODE (0)
#endif

#if defined(CONFIG_BT_BLE_ADV_DATA_LENGTH_ZERO_AUX)
#define BT_BLE_ADV_DATA_LENGTH_ZERO_AUX (CONFIG_BT_BLE_ADV_DATA_LENGTH_ZERO_AUX)
#else
#define BT_BLE_ADV_DATA_LENGTH_ZERO_AUX (0)
#endif

#if defined(CONFIG_BT_CTRL_CHAN_ASS_EN)
#define BT_CTRL_CHAN_ASS_EN (CONFIG_BT_CTRL_CHAN_ASS_EN)
#else
#define BT_CTRL_CHAN_ASS_EN (0)
#endif

#if defined(CONFIG_BT_CTRL_LE_PING_EN)
#define BT_CTRL_LE_PING_EN (CONFIG_BT_CTRL_LE_PING_EN)
#else
#define BT_CTRL_LE_PING_EN (0)
#endif

#define AGC_RECORRECT_EN       ((BT_CTRL_AGC_RECORRECT_EN << 0) | (BT_CTRL_CODED_AGC_RECORRECT <<1) | (BT_CTRL_AGC_RECORRECT_NEW << 2))

#define CFG_MASK_BIT_SCAN_DUPLICATE_OPTION    (1<<0)

#define CFG_MASK      CFG_MASK_BIT_SCAN_DUPLICATE_OPTION
#if CONFIG_IDF_TARGET_ESP32C3
#define BLE_HW_TARGET_CODE_CHIP_ECO0                      (0x01010000)
#else // CONFIG_IDF_TARGET_ESP32S3
#define BLE_HW_TARGET_CODE_CHIP_ECO0                      (0x02010000)
#endif

#ifdef CONFIG_BT_CTRL_BLE_LLCP_CONN_UPDATE
#define BT_CTRL_BLE_LLCP_CONN_UPDATE (1<<0)
#else
#define BT_CTRL_BLE_LLCP_CONN_UPDATE (0<<0)
#endif

#ifdef CONFIG_BT_CTRL_BLE_LLCP_CHAN_MAP_UPDATE
#define BT_CTRL_BLE_LLCP_CHAN_MAP_UPDATE (1<<1)
#else
#define BT_CTRL_BLE_LLCP_CHAN_MAP_UPDATE (0<<1)
#endif

#ifdef CONFIG_BT_CTRL_BLE_LLCP_PHY_UPDATE
#define BT_CTRL_BLE_LLCP_PHY_UPDATE (1<<2)
#else
#define BT_CTRL_BLE_LLCP_PHY_UPDATE (0<<2)
#endif

#define BT_CTRL_BLE_LLCP_DISC_FLAG (BT_CTRL_BLE_LLCP_CONN_UPDATE | BT_CTRL_BLE_LLCP_CHAN_MAP_UPDATE | BT_CTRL_BLE_LLCP_PHY_UPDATE)
#if defined(CONFIG_BT_CTRL_RUN_IN_FLASH_ONLY)
#define BT_CTRL_RUN_IN_FLASH_ONLY  CONFIG_BT_CTRL_RUN_IN_FLASH_ONLY
#else
#define BT_CTRL_RUN_IN_FLASH_ONLY  (0)
#endif



#if defined(CONFIG_BT_CTRL_DTM_ENABLE)
#define BT_CTRL_DTM_ENABLE  CONFIG_BT_CTRL_DTM_ENABLE
#else
#define BT_CTRL_DTM_ENABLE  (0)
#endif

#if defined(CONFIG_BT_CTRL_BLE_MASTER)
#define BT_CTRL_BLE_MASTER  CONFIG_BT_CTRL_BLE_MASTER
#else
#define BT_CTRL_BLE_MASTER  (0)
#endif

#if defined(CONFIG_BT_CTRL_BLE_TEST)
#define BT_CTRL_BLE_TEST  CONFIG_BT_CTRL_BLE_TEST
#else
#define BT_CTRL_BLE_TEST  (0)
#endif

#if defined (CONFIG_BT_CTRL_BLE_SECURITY_ENABLE)
#define BLE_SECURITY_ENABLE  (CONFIG_BT_CTRL_BLE_SECURITY_ENABLE)
#else
#define BLE_SECURITY_ENABLE  (0)
#endif

#if defined (CONFIG_BT_CTRL_BLE_SCAN)
#define BT_CTRL_BLE_SCAN    CONFIG_BT_CTRL_BLE_SCAN
#else
#define BT_CTRL_BLE_SCAN    (0)
#endif

#if defined (CONFIG_BT_CTRL_BLE_ADV)
#define BT_CTRL_BLE_ADV    CONFIG_BT_CTRL_BLE_ADV
#else
#define BT_CTRL_BLE_ADV    (0)
#endif

#ifdef CONFIG_BT_CTRL_CHECK_CONNECT_IND_ACCESS_ADDRESS
#define BLE_CTRL_CHECK_CONNECT_IND_ACCESS_ADDRESS_ENABLED CONFIG_BT_CTRL_CHECK_CONNECT_IND_ACCESS_ADDRESS
#else
#define BLE_CTRL_CHECK_CONNECT_IND_ACCESS_ADDRESS_ENABLED 0
#endif

#if defined(CONFIG_BT_CTRL_LE_LOG_EN)
#define BT_BLE_LOG_EN CONFIG_BT_CTRL_LE_LOG_EN
#else
#define BT_BLE_LOG_EN (0)
#endif

#if defined(CONFIG_BT_CTRL_LE_LOG_MODE_EN)
#define BLE_LOG_MODE_EN CONFIG_BT_CTRL_LE_LOG_MODE_EN
#else
#define BLE_LOG_MODE_EN (0)
#endif

#if defined(CONFIG_BT_CTRL_LE_LOG_LEVEL)
#define BLE_LOG_LEVEL CONFIG_BT_CTRL_LE_LOG_LEVEL
#else
#define BLE_LOG_LEVEL (0)
#endif

#define BT_CONTROLLER_INIT_CONFIG_DEFAULT() {                              \
    .magic = ESP_BT_CTRL_CONFIG_MAGIC_VAL,                                 \
    .version = ESP_BT_CTRL_CONFIG_VERSION,                                 \
    .controller_task_stack_size = ESP_TASK_BT_CONTROLLER_STACK,            \
    .controller_task_prio = ESP_TASK_BT_CONTROLLER_PRIO,                   \
    .controller_task_run_cpu = CONFIG_BT_CTRL_PINNED_TO_CORE,              \
    .bluetooth_mode = CONFIG_BT_CTRL_MODE_EFF,                             \
    .ble_max_act = CONFIG_BT_CTRL_BLE_MAX_ACT_EFF,                         \
    .sleep_mode = CONFIG_BT_CTRL_SLEEP_MODE_EFF,                           \
    .sleep_clock = CONFIG_BT_CTRL_SLEEP_CLOCK_EFF,                         \
    .ble_st_acl_tx_buf_nb = CONFIG_BT_CTRL_BLE_STATIC_ACL_TX_BUF_NB,       \
    .ble_hw_cca_check = CONFIG_BT_CTRL_HW_CCA_EFF,                         \
    .ble_adv_dup_filt_max = CONFIG_BT_CTRL_ADV_DUP_FILT_MAX,               \
    .coex_param_en = false,                                                \
    .ce_len_type = CONFIG_BT_CTRL_CE_LENGTH_TYPE_EFF,                      \
    .coex_use_hooks = false,                                               \
    .hci_tl_type = CONFIG_BT_CTRL_HCI_TL_EFF,                              \
    .hci_tl_funcs = NULL,                                                  \
    .txant_dft = CONFIG_BT_CTRL_TX_ANTENNA_INDEX_EFF,                      \
    .rxant_dft = CONFIG_BT_CTRL_RX_ANTENNA_INDEX_EFF,                      \
    .txpwr_dft = CONFIG_BT_CTRL_DFT_TX_POWER_LEVEL_EFF,                    \
    .cfg_mask = CFG_MASK,                                                  \
    .scan_duplicate_mode = SCAN_DUPLICATE_MODE,                            \
    .scan_duplicate_type = SCAN_DUPLICATE_TYPE_VALUE,                      \
    .normal_adv_size = NORMAL_SCAN_DUPLICATE_CACHE_SIZE,                   \
    .mesh_adv_size = MESH_DUPLICATE_SCAN_CACHE_SIZE,                       \
    .coex_phy_coded_tx_rx_time_limit = CONFIG_BT_CTRL_COEX_PHY_CODED_TX_RX_TLIM_EFF, \
    .hw_target_code = BLE_HW_TARGET_CODE_CHIP_ECO0,                        \
    .slave_ce_len_min = SLAVE_CE_LEN_MIN_DEFAULT,                          \
    .hw_recorrect_en = AGC_RECORRECT_EN,                                   \
    .cca_thresh = CONFIG_BT_CTRL_HW_CCA_VAL,                               \
    .scan_backoff_upperlimitmax = BT_CTRL_SCAN_BACKOFF_UPPERLIMITMAX,      \
    .dup_list_refresh_period = DUPL_SCAN_CACHE_REFRESH_PERIOD,             \
    .ble_50_feat_supp  = BT_CTRL_50_FEATURE_SUPPORT,                       \
    .ble_cca_mode = BT_BLE_CCA_MODE,                                       \
    .ble_data_lenth_zero_aux = BT_BLE_ADV_DATA_LENGTH_ZERO_AUX,            \
    .ble_chan_ass_en = BT_CTRL_CHAN_ASS_EN,                                \
    .ble_ping_en = BT_CTRL_LE_PING_EN,                                     \
    .ble_llcp_disc_flag = BT_CTRL_BLE_LLCP_DISC_FLAG,                      \
    .run_in_flash = BT_CTRL_RUN_IN_FLASH_ONLY,                             \
    .dtm_en = BT_CTRL_DTM_ENABLE,                                          \
    .enc_en = BLE_SECURITY_ENABLE,                                         \
    .qa_test = BT_CTRL_BLE_TEST,                                           \
    .connect_en = BT_CTRL_BLE_MASTER,                                      \
    .scan_en = BT_CTRL_BLE_SCAN,                                           \
    .ble_aa_check = BLE_CTRL_CHECK_CONNECT_IND_ACCESS_ADDRESS_ENABLED,     \
    .adv_en = BT_CTRL_BLE_ADV,                                             \
}

#else
#define BT_CONTROLLER_INIT_CONFIG_DEFAULT() {0}; ESP_STATIC_ASSERT(0, "please enable bluetooth in menuconfig to use esp_bt.h");
#endif

/**
 * @brief Controller HCI transport layer function structure
 *
 * @note This structure must be registered when HCI transport layer is UART
 */
typedef struct {
    uint32_t _magic;                        /*!< Magic number */
    uint32_t _version;                      /*!< Version number of the defined structure */
    uint32_t _reserved;                     /*!< Reserved for future use */
    int (* _open)(void);                    /*!< HCI transport layer open function */
    void (* _close)(void);                  /*!< HCI transport layer close function */
    void (* _finish_transfers)(void);       /*!< HCI transport layer finish transfers function */
    void (* _recv)(uint8_t *buf, uint32_t len, esp_bt_hci_tl_callback_t callback, void* arg); /*!< HCI transport layer receive function */
    void (* _send)(uint8_t *buf, uint32_t len, esp_bt_hci_tl_callback_t callback, void* arg); /*!< HCI transport layer send function */
    bool (* _flow_off)(void);               /*!< HCI transport layer flow off function */
    void (* _flow_on)(void);                /*!< HCI transport layer flow on function */
} esp_bt_hci_tl_t;

/**
 * @brief Bluetooth Controller config options
 * @note
 *      1. For parameters configurable through menuconfig, it is recommended to adjust them via the menuconfig interface. Please refer to menuconfig for details on the range and default values.
 *      2. It is not recommended to modify the values for parameters which are not configurable through menuconfig.
 */
typedef struct {
    uint32_t magic;                         /*!< Magic number */
    uint32_t version;                       /*!< version number of the defined structure */
    uint16_t controller_task_stack_size;    /*!< Bluetooth Controller task stack size in bytes */
    uint8_t controller_task_prio;           /*!< Bluetooth Controller task priority */
    uint8_t controller_task_run_cpu;        /*!< CPU number that Bluetooth Controller task runs on. Configurable in menuconfig.
                                                - 0 - CPU 0 (default)
                                                - 1 - CPU 1 */
    uint8_t bluetooth_mode;                 /*!< BLE mode */
    uint8_t ble_max_act;                    /*!< The maximum number of BLE instance. Configurable in menuconfig.
                                                - Range: 1 - 10
                                                - Default: 6 */
    uint8_t sleep_mode;                     /*!< Modem sleep mode. Configurable in menuconfig.
                                                - 0 - Disable (default)
                                                - 1 - Enable */
    uint8_t sleep_clock;                    /*!< Modem sleep clock source. Configurable in menuconfig. */
    uint8_t ble_st_acl_tx_buf_nb;           /*!< Static ACL TX buffer numbers. Configurable in menuconfig.
                                                - Range: 0 - 12
                                                - Default: 0 */
    uint8_t ble_hw_cca_check;               /*!< Deprecated */
    uint16_t ble_adv_dup_filt_max;          /*!< The maximum number of extended duplicate scan filter. Configurable in menuconfig.
                                                - Range: 1 - 500
                                                - Default: 30 */
    bool coex_param_en;                     /*!< Deprecated */
    uint8_t ce_len_type;                    /*!< Connection event length determination method. Configurable in menuconfig.
                                                - 0 - Original (default)
                                                - 1 - use `CE_LEN` parameter from HCI commands
                                                - 2 - Espressif vendor defined method */
    bool coex_use_hooks;                    /*!< Deprecated */
    uint8_t hci_tl_type;                    /*!< HCI transport layer type. Configurable in menuconfig.
                                                - 0 - URAT
                                                - 1 - Virtual HCI (default) */
    esp_bt_hci_tl_t *hci_tl_funcs;          /*!< HCI transport functions used. It must be set when `hci_tl_type` is UART. */
    uint8_t txant_dft;                      /*!< Default TX antenna. Configurable in menuconfig.
                                                - 0 - Antenna 0 (default)
                                                - 1 - Antenna 1 */
    uint8_t rxant_dft;                      /*!< Default RX antenna. Configurable in menuconfig.
                                                - 0 - Antenna 0 (default)
                                                - 1 - Antenna 1 */
    uint8_t txpwr_dft;                      /*!< Default TX power. Please refer to `esp_power_level_t` for supported power level. Configurable in menuconfig.
                                                - Default : `ESP_PWR_LVL_P9` +9 dBm. */
    uint32_t cfg_mask;                      /*!< Configuration mask to set specific options */
    uint8_t scan_duplicate_mode;            /*!< Scan duplicate filtering mode. Configurable in menuconfig.
                                                - 0 - Normal scan duplicate filtering mode (default)
                                                - 1 - Special scan duplicate filtering mode for BLE Mesh */
    uint8_t scan_duplicate_type;            /*!< Scan duplicate filtering type. If `scan_duplicate_mode` is set to 1, this parameter will be ignored. Configurable in menuconfig.
                                                - 0 - Filter scan duplicates by device address only (default)
                                                - 1 - Filter scan duplicates by advertising data only, even if they originate from different devices.
                                                - 2 - Filter scan duplicated by device address and advertising data. */
    uint16_t normal_adv_size;               /*!< Maximum number of devices in scan duplicate filtering list. Configurable in menuconfig.
                                                - Range: 10 - 1000
                                                - Default: 100 */
    uint16_t mesh_adv_size;                 /*!< Maximum number of Mesh advertising packets in scan duplicate filtering list. Configurable in menuconfig.
                                                - Range: 10 - 1000
                                                - Default: 100 */
    uint8_t coex_phy_coded_tx_rx_time_limit;  /*!< Enable / disable the maximum TX/RX time limit for Coded-PHY connections in coexistence with Wi-Fi scenarios. Configurable in menuconfig.
                                                - 0 - Disable (default)
                                                - 1 - Enable */
    uint32_t hw_target_code;                /*!< Hardware target. Internal use only. Please do not modify this value. */
    uint8_t slave_ce_len_min;               /*!< Slave minimum connection event length: 5 slots. Please do not modify this value. */
    uint8_t hw_recorrect_en;                /*!< Enable / disable uncoded phy / coded phy hardware re-correction. Configurable in menuconfig. */
    uint8_t cca_thresh;                     /*!< Absolute value of hardware-triggered CCA threshold. The CCA threshold is always negative.
                                                 If the channel assessment result exceeds the CCA threshold (e.g. -75 dBm), indicating the channel is busy,
                                                 the hardware will not transmit packets on that channel. Configurable in menuconfig.
                                                - Range: 20 dBm - 100 dBm
                                                - Default: 75 dBm */
    uint16_t scan_backoff_upperlimitmax;    /*!< Enable / disable active scan backoff. Configurable in menuconfig.
                                                - 0 - Disable (default)
                                                - 1 - Enable */
    uint16_t dup_list_refresh_period;       /*!< Scan duplicate filtering list refresh period in seconds.  Configurable in menuconfig
                                                - Range: 0 - 100 seconds
                                                - Default: 0 second */
    bool ble_50_feat_supp;                  /*!< True if BLE 5.0 features are enabled; false otherwise. This option depends on whether the Host enable the 5.0 features. */
    uint8_t ble_cca_mode;                   /*!< BLE CCA mode. Configurable in menuconfig
                                                - 0 - Disable (default)
                                                - 1 - Hardware-triggered CCA
                                                - 2 - Software-based CCA (experimental) */
    uint8_t ble_data_lenth_zero_aux;        /*!< Enable / disable auxiliary packets when the extended ADV data length is zero. Configurable in menuconfig.
                                                - 0 - Disable (default)
                                                - 1 - Enable */
    uint8_t ble_chan_ass_en;                /*!< Enable / disable BLE channel assessment. Configurable in menuconfig.
                                                - 0 - Disable
                                                - 1 - Enable (default) */
    uint8_t ble_ping_en;                    /*!< Enable / disable BLE ping procedure. Configurable in menuconfig.
                                                - 0 - Disable
                                                - 1 - Enable (default) */
    uint8_t ble_llcp_disc_flag;             /*!< Flag indicating whether the Controller disconnects after Instant Passed (0x28) error occurs. Configurable in menuconfig.
                                                - The Controller does not disconnect after Instant Passed (0x28) by default. */
    bool run_in_flash;                      /*!< True if the Controller code is in flash (flash model); false otherwise (default). Configurable in menuconfig. */
    bool dtm_en;                            /*!< True if the DTM feature is enabled; false otherwise (default). Configurable in menuconfig. */
    bool enc_en;                            /*!< True if the encryption feature is enabled (default); false otherwise. Configurable in menuconfig. */
    bool qa_test;                           /*!< True if the QA test feature is enabled; false otherwise (default). Configurable in menuconfig.*/
    bool connect_en;                        /*!< True if the connection feature is enabled (default); false otherwise. Configurable in menuconfig.*/
    bool scan_en;                           /*!< True if the scan feature is enabled (default); false otherwise. Configurable in menuconfig.*/
    bool ble_aa_check;                      /*!< True if adds a verification step for the Access Address within the CONNECT_IND PDU; false otherwise. Configurable in menuconfig */
    bool adv_en;                            /*!< True if the ADV feature is enabled (default); false otherwise. Configurable in menuconfig.*/
} esp_bt_controller_config_t;

/**
 * @brief Bluetooth Controller status
 */
typedef enum {
    ESP_BT_CONTROLLER_STATUS_IDLE = 0,  /*!< The Controller is not initialized or has been de-initialized. */
    ESP_BT_CONTROLLER_STATUS_INITED,    /*!< The Controller has been initialized, but not enabled or has been disabled. */
    ESP_BT_CONTROLLER_STATUS_ENABLED,   /*!< The Controller has been initialized and enabled. */
    ESP_BT_CONTROLLER_STATUS_NUM,       /*!< Number of Controller statuses */
} esp_bt_controller_status_t;

/**
 * @brief BLE TX power type
 *
 * This TX power type is used for the API `esp_ble_tx_power_set()` and `esp_ble_tx_power_get()`.
 *
 * @note
 *       1. The connection TX power can only be set after the connection is established.
 *           After disconnecting, the corresponding TX power will not be affected.
 *       2. `ESP_BLE_PWR_TYPE_DEFAULT` can be used to set the TX power for power types that have not been set before.
 *          It will not affect the TX power values which have been set for the ADV/SCAN/CONN0-8 power types.
 *       3. If none of power type is set, the system will use `ESP_PWR_LVL_P3` as default for all power types.
 */
typedef enum {
    ESP_BLE_PWR_TYPE_CONN_HDL0  = 0,            /*!< TX power for Connection state handle 0 */
    ESP_BLE_PWR_TYPE_CONN_HDL1  = 1,            /*!< TX power for Connection state handle 1 */
    ESP_BLE_PWR_TYPE_CONN_HDL2  = 2,            /*!< TX power for Connection state handle 2 */
    ESP_BLE_PWR_TYPE_CONN_HDL3  = 3,            /*!< TX power for Connection state handle 3 */
    ESP_BLE_PWR_TYPE_CONN_HDL4  = 4,            /*!< TX power for Connection state handle 4 */
    ESP_BLE_PWR_TYPE_CONN_HDL5  = 5,            /*!< TX power for Connection state handle 5 */
    ESP_BLE_PWR_TYPE_CONN_HDL6  = 6,            /*!< TX power for Connection state handle 6 */
    ESP_BLE_PWR_TYPE_CONN_HDL7  = 7,            /*!< TX power for Connection state handle 7 */
    ESP_BLE_PWR_TYPE_CONN_HDL8  = 8,            /*!< TX power for Connection state handle 8 */
    ESP_BLE_PWR_TYPE_ADV        = 9,            /*!< TX power for Advertising state*/
    ESP_BLE_PWR_TYPE_SCAN       = 10,           /*!< TX power for Scanning state */
    ESP_BLE_PWR_TYPE_DEFAULT    = 11,           /*!< TX power for states that have not been set before */
    ESP_BLE_PWR_TYPE_NUM        = 12,           /*!< Reserved  */
} esp_ble_power_type_t;

/**
 * @brief The enhanced type of which TX power, could set Advertising/Connection/Default and etc.
 *
 * This TX power type is used for the API `esp_ble_tx_power_set_enhanced()` and `esp_ble_tx_power_get_enhanced()`.
 *
 * @note
 *       1. The connection TX power can only be set after the connection is established.
 *           After disconnecting, the corresponding TX power will not be affected.
 *       2. `ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT` can be used to set the TX power for power types that have not been set before.
 *          It will not affect the TX power values which have been set for the ADV/SCAN/INIT/CONN power types.
 *       3. If none of power type is set, the system will use `ESP_PWR_LVL_P3` as default for all power types.
 */
typedef enum {
    ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT = 0,   /*!< TX power for states that have not been set before */
    ESP_BLE_ENHANCED_PWR_TYPE_ADV,           /*!< TX power for Advertising state */
    ESP_BLE_ENHANCED_PWR_TYPE_SCAN,          /*!< TX power for Scanning state */
    ESP_BLE_ENHANCED_PWR_TYPE_INIT,          /*!< TX power for Initiating state */
    ESP_BLE_ENHANCED_PWR_TYPE_CONN,          /*!< TX power for Connection state */
    ESP_BLE_ENHANCED_PWR_TYPE_MAX,           /*!< Reserved */
} esp_ble_enhanced_power_type_t;

/**
 * @brief Bluetooth TX power level (index). Each index corresponds to a specific power value in dBm.
 */
typedef enum {
    ESP_PWR_LVL_N24 = 0,              /*!< Corresponding to -24 dBm */
    ESP_PWR_LVL_N21 = 1,              /*!< Corresponding to -21 dBm */
    ESP_PWR_LVL_N18 = 2,              /*!< Corresponding to -18 dBm */
    ESP_PWR_LVL_N15 = 3,              /*!< Corresponding to -15 dBm */
    ESP_PWR_LVL_N12 = 4,              /*!< Corresponding to -12 dBm */
    ESP_PWR_LVL_N9  = 5,              /*!< Corresponding to -9 dBm */
    ESP_PWR_LVL_N6  = 6,              /*!< Corresponding to -6 dBm */
    ESP_PWR_LVL_N3  = 7,              /*!< Corresponding to -3 dBm */
    ESP_PWR_LVL_N0  = 8,              /*!< Corresponding to 0 dBm */
    ESP_PWR_LVL_P3  = 9,              /*!< Corresponding to +3 dBm */
    ESP_PWR_LVL_P6  = 10,             /*!< Corresponding to +6 dBm */
    ESP_PWR_LVL_P9  = 11,             /*!< Corresponding to +9 dBm */
    ESP_PWR_LVL_P12 = 12,             /*!< Corresponding to +12 dBm */
    ESP_PWR_LVL_P15 = 13,             /*!< Corresponding to +15 dBm */
    ESP_PWR_LVL_P18 = 14,             /*!< Corresponding to +18 dBm */
    ESP_PWR_LVL_P20 = 15,             /*!< Corresponding to +20 dBm */
    ESP_PWR_LVL_P21 = 15,             /*!< Deprecated */
    ESP_PWR_LVL_INVALID = 0xFF,       /*!< Indicates an invalid value */
} esp_power_level_t;

/**
 * @brief       Initialize the Bluetooth Controller to allocate tasks and other resources
 *
 * @note        This function should be called only once, before any other Bluetooth functions.
 *
 * @param[in]  cfg Initial Bluetooth Controller configuration
 *
 * @return
 *        - ESP_OK: Success
 *        - ESP_ERR_INVALID_STATE: Invalid Bluetooth Controller state
 *        - ESP_ERR_NOT_SUPPORTED: Invalid Bluetooth Controller mode
 *        - ESP_ERR_INVALID_ARG: Invalid arguments
 *        - ESP_ERR_NO_MEM: Out of memory
 *        - ESP_FAIL: Failure due to other reasons
 *
 */
esp_err_t esp_bt_controller_init(esp_bt_controller_config_t *cfg);

/**
 * @brief  De-initialize Bluetooth Controller to free resources and delete tasks
 *
 * @note
 *      1. You should make sure that the Controller is in idle state before de-initializing it.
 *      2. This function should be called only once, after any other Bluetooth functions.
 *
 * @return
 *      - ESP_OK: Success
 *      - ESP_ERR_INVALID_ARG: Invalid arguments
 *      - ESP_ERR_INVALID_STATE: Invalid Bluetooth Controller state
 *      - ESP_ERR_NO_MEM: Out of memory
 */
esp_err_t esp_bt_controller_deinit(void);

/**
 * @brief Enable Bluetooth Controller
 *
 * @note
 *       1. Bluetooth Controller cannot be enabled in `ESP_BT_CONTROLLER_STATUS_IDLE` status. It has to be initialized first.
 *       2. Due to a known issue, you cannot call `esp_bt_controller_enable()` for the second time
 *       to change the Controller mode dynamically. To change the Controller mode, call
 *       `esp_bt_controller_disable()` and then call `esp_bt_controller_enable()` with the new mode.
 *
 * @param[in] mode The Bluetooth Controller mode to enable
 *
 * For API compatibility, retain this argument. This mode must match the mode specified in the `cfg` of `esp_bt_controller_init()`.
 *
 * @return
 *          - ESP_OK: Success
 *          - ESP_ERR_INVALID_STATE: Invalid Bluetooth Controller state
 *          - ESP_ERR_INVALID_ARG: Invalid arguments
 */
esp_err_t esp_bt_controller_enable(esp_bt_mode_t mode);

/**
 * @brief  Disable Bluetooth Controller
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_ERR_INVALID_STATE: Invalid Bluetooth Controller state
 */
esp_err_t esp_bt_controller_disable(void);

/**
 * @brief  Get Bluetooth Controller status
 *
 * @return
 *      - ESP_BT_CONTROLLER_STATUS_IDLE:    The Controller is not initialized or has been de-initialized.
 *      - ESP_BT_CONTROLLER_STATUS_INITED:  The Controller has been initialized, but not enabled or has been disabled.
 *      - ESP_BT_CONTROLLER_STATUS_ENABLED: The Controller has been initialized and enabled.
 */
esp_bt_controller_status_t esp_bt_controller_get_status(void);

/**
 * @brief Release the Controller memory as per the mode
 *
 * This function releases the BSS, data and other sections of the Controller to heap. The total size is about 70 KB.
 *
 * @note
 *    1. This function is optional and should be called only if you want to free up memory for other components.
 *    2. This function should only be called when the Controller is in `ESP_BT_CONTROLLER_STATUS_IDLE` status.
 *    3. Once Bluetooth Controller memory is released, the process cannot be reversed. This means you cannot use the Bluetooth Controller mode that you have released using this function.
 *    4. If your firmware will upgrade the Bluetooth Controller mode later (such as from disabled to enabled), then do not call this function.
 *
 * If you never intend to use Bluetooth in a current boot-up cycle, calling `esp_bt_controller_mem_release(ESP_BT_MODE_BLE)` could release the BSS and data consumed by BLE Controller to heap.
 *
 *
 * @param[in] mode The Bluetooth Controller mode
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_ERR_INVALID_STATE: Invalid Bluetooth Controller state
 *       - ESP_ERR_NOT_FOUND: Requested resource not found
 */
esp_err_t esp_bt_controller_mem_release(esp_bt_mode_t mode);

/** @brief Release the Controller memory, BSS and data section of the BLE Host stack as per the mode
 *
 * @note
 *    1. This function is optional and should be called only if you want to free up memory for other components.
 *    2. This function should only be called when the Controller is in `ESP_BT_CONTROLLER_STATUS_IDLE` status.
 *    3. Once Bluetooth Controller memory is released, the process cannot be reversed. This means you cannot use the Bluetooth Controller mode that you have released using this function.
 *    4. If your firmware will upgrade the Bluetooth Controller mode later (such as from disabled to enabled), then do not call this function.
 *
 * This function first releases Controller memory by internally calling `esp_bt_controller_mem_release()`, then releases Host memory.
 *
 * If you never intend to use Bluetooth in a current boot-up cycle, calling `esp_bt_mem_release(ESP_BT_MODE_BLE)` could release the BSS and data consumed by BLE stack to heap.
 *
 * For example, if you only use Bluetooth for setting the Wi-Fi configuration, and do not use Bluetooth in the rest of the product operation,
 *  after receiving the Wi-Fi configuration, you can disable/de-init Bluetooth and release its memory.
 * Below is the sequence of APIs to be called for such scenarios:
 *
 *       esp_bluedroid_disable();
 *       esp_bluedroid_deinit();
 *       esp_bt_controller_disable();
 *       esp_bt_controller_deinit();
 *       esp_bt_mem_release(ESP_BT_MODE_BLE);
 *
 * @param[in] mode The Bluetooth Controller mode
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_ERR_INVALID_STATE: Invalid Bluetooth Controller state
 *       - ESP_ERR_NOT_FOUND: Requested resource not found
 */
esp_err_t esp_bt_mem_release(esp_bt_mode_t mode);

/**
 * @brief Enable Bluetooth modem sleep
 *
 * There are currently two options for Bluetooth modem sleep: ORIG mode and EVED mode. The latter is intended for BLE only.
 * The modem sleep mode could be configured in menuconfig.
 *
 * In ORIG mode, if there is no event to process, the Bluetooth Controller will periodically switch off some components and pause operation, then wake up according to the scheduled interval and resume work.
 * It can also wakeup earlier upon external request using function `esp_bt_controller_wakeup_request()`.
 *
 * @note  This function shall not be invoked before `esp_bt_controller_enable()`.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_ERR_INVALID_STATE: Invalid Bluetooth Controller state
 *       - ESP_ERR_NOT_SUPPORTED: Operation or feature not supported
 */
esp_err_t esp_bt_sleep_enable(void);

/**
 * @brief Disable Bluetooth modem sleep
 *
 * @note
 *      1. Bluetooth Controller will not be allowed to enter modem sleep after calling this function.
 *      2. In ORIG modem sleep mode, calling this function may not immediately wake up the Controller if it is currently dormant.
 *         In this case, `esp_bt_controller_wakeup_request()` can be used to shorten the wake-up time.
 *      3. This function shall not be invoked before `esp_bt_controller_enable()`.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_ERR_INVALID_STATE: Invalid Bluetooth Controller state
 *       - ESP_ERR_NOT_SUPPORTED: Operation or feature not supported
 */
esp_err_t esp_bt_sleep_disable(void);

/**
 * @brief Get the Bluetooth sleep clock source.
 *
 * @note This function shall not be invoked before `esp_bt_controller_init()`.
 *
 * @return  clock source used in Bluetooth low power mode
 */
esp_bt_sleep_clock_t esp_bt_get_lpclk_src(void);

/**
 * @brief Check if the Bluetooth Controller is currently in sleep mode when modem sleep is enabled.
 *
 * @note
 *      1. This function shall not be invoked before `esp_bt_controller_enable()`.
 *      2. This function is supposed to be used ORIG mode of modem sleep.
 *
 * @return
 *      - True if the Controller is in modem sleep state; false otherwise.
 */
bool esp_bt_controller_is_sleeping(void);

/**
 * @brief Request the Controller to wakeup from sleeping state during sleep mode
 *
 * Profiling shows that it takes several milliseconds to wakeup from modem sleep after this request.
 *
 * Generally it takes longer if 32kHz XTAL is used than the main XTAL, due to the lower frequency of the former as the bluetooth low power clock source.
 *
 * @note
 *      1. This function shall not be invoked before `esp_bt_controller_enable()`.
 *      2. This function is supposed to be used ORIG mode of modem sleep.
 *      3. After this request, the Bluetooth Controller can re-enter sleep as long as modem sleep remains enabled.
 */
void esp_bt_controller_wakeup_request(void);

/**
 * @brief  Set BLE TX power for the specified power type
 *
 * It is recommended to use `esp_ble_tx_power_set_enhanced` to set TX power for individual advertising and connection handle.
 *
 * @note Connection TX power should only be set after the connection is established.
 *
 * @param[in]  power_type The type of TX power. It could be Advertising, Connection, or Default.
 * @param[in]  power_level Power level (index) corresponding to the absolute value (dBm)
 *
 * @return
 *      - ESP_OK:   Success
 *      - ESP_ERR_NOT_SUPPORTED: Invalid TX power type
 *      - ESP_FAIL: Failure due to other reasons
 */
esp_err_t esp_ble_tx_power_set(esp_ble_power_type_t power_type, esp_power_level_t power_level);

/**
 * @brief  Get BLE TX power of the specified power type
 *
 * It is recommended to use `esp_ble_tx_power_get_enhanced` to get TX power of individual advertising and connection handle.
 *
 * @note
 *      1. Connection TX power should only be retrieved after the connection is established.
 *      2. If an invalid power type is provided, this API returns `ESP_PWR_LVL_INVALID`.
 *
 * @param[in]  power_type The type of TX power. It could be Advertising/Connection/Default and etc.
 *
 * @return
 *         - Power level
 *
 */
esp_power_level_t esp_ble_tx_power_get(esp_ble_power_type_t power_type);

/**
 * @brief  Set BLE TX power for the specified Advertising or Connection handle
 *
 * For the TX power type: `ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT`, `ESP_BLE_ENHANCED_PWR_TYPE_SCAN`, `ESP_BLE_ENHANCED_PWR_TYPE_INIT`,
 * this API will ignore the input handle number, and set 0 internally.
 *
 * For the TX power type: `ESP_BLE_ENHANCED_PWR_TYPE_ADV` and `ESP_BLE_ENHANCED_PWR_TYPE_CONN`，
 * this API will set the TX power for the target handle.
 *
 * @note
 *      1. Connection TX power should only be set after connection created.
 *
 * @param[in]  power_type  The type of TX power
 * @param[in]  handle      The handle of Advertising or Connection
 * @param[in]  power_level Power level (index) corresponding to absolute value (dBm)
 *
 * @return
 *        - ESP_OK: Success
 *        - ESP_ERR_NOT_SUPPORTED: Invalid TX power type
 *        - ESP_FAIL: Failure due to other reasons
 */
esp_err_t esp_ble_tx_power_set_enhanced(esp_ble_enhanced_power_type_t power_type, uint16_t handle, esp_power_level_t power_level);

/**
 * @brief  Get BLE TX power of the specified Advertising or Connection handle
 *
 * For the TX power type: `ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT`, `ESP_BLE_ENHANCED_PWR_TYPE_SCAN`, `ESP_BLE_ENHANCED_PWR_TYPE_INIT`,
 * this API will ignore the input handle number.
 *
 * For the TX power type: `ESP_BLE_ENHANCED_PWR_TYPE_ADV` and `ESP_BLE_ENHANCED_PWR_TYPE_CONN`，
 * this API will return the TX power of the target handle.
 *
 * @note
 *      1. Connection Tx power should only be get after connection created.
 *      2. If an invalid power type is provided, this API returns `ESP_PWR_LVL_INVALID`.
 *
 * @param[in]  power_type The type of TX power
 * @param[in]  handle     The handle of Advertising or Connection and the value 0 for other enhanced power types
 *
 * @return             Power level
 */
esp_power_level_t esp_ble_tx_power_get_enhanced(esp_ble_enhanced_power_type_t power_type, uint16_t handle);

/**
 * @brief Notify Bluetooth Controller task to process the event upon TX or RX done
 *
 * @note
 *     1. This function shall not be invoked before `esp_bt_controller_enable()`.
 *     2. This function can be called in both ISR and non-ISR context.
 *     3. This function ignored the passed `event` value currently
 */
int esp_bt_h4tl_eif_io_event_notify(int event);

/**
 * @brief Virtual HCI (VHCI) callback functions to notify the Host on the next operation
 */
typedef struct esp_vhci_host_callback {
    void (*notify_host_send_available)(void);               /*!< callback used to notify that the Host can send the HCI data to Controller */
    int (*notify_host_recv)(uint8_t *data, uint16_t len);   /*!< callback used to notify that the Controller has the HCI data to send to the Host */
} esp_vhci_host_callback_t;

/**
 * @brief Check whether the Controller is ready to receive the HCI data
 *
 * If the return value is True, the Host can send the HCI data to the Controller.
 *
 * @note This function should be called before each `esp_vhci_host_send_packet()`.
 *
 * @return
 *       True if the Controller is ready to receive the HCI data; false otherwise.
 */
bool esp_vhci_host_check_send_available(void);

/**
 * @brief Send the HCI data to the Controller
 *
 * @note
 *      1. This function shall not be called within a critical section or when the scheduler is suspended.
 *      2. This function should be called only if `esp_vhci_host_check_send_available` returns True.
 *
 * @param[in] data Pointer to the HCI data
 * @param[in] len The HCI data length
 */
void esp_vhci_host_send_packet(uint8_t *data, uint16_t len);

/**
 * @brief Register the VHCI callback functions defined in `esp_vhci_host_callback` structure.
 *
 * @param[in] callback `esp_vhci_host_callback` type variable
 *
 * @return
 *      - ESP_OK: Success
 *      - ESP_FAIL: Failure
 */
esp_err_t esp_vhci_host_register_callback(const esp_vhci_host_callback_t *callback);

/**
 * @brief Select buffers
*/
typedef enum {
    ESP_BLE_LOG_BUF_HCI         = 0x02,
    ESP_BLE_LOG_BUF_CONTROLLER  = 0x05,
} esp_ble_log_buf_t;

void esp_ble_controller_log_dump_all(bool output);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_BT_H__ */
