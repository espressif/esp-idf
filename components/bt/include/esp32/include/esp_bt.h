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

#ifdef CONFIG_BT_ENABLED

#define SOC_MEM_BT_DATA_START               0x3ffae6e0
#define SOC_MEM_BT_DATA_END                 0x3ffaff10
#define SOC_MEM_BT_EM_START                 0x3ffb0000
#define SOC_MEM_BT_EM_END                   0x3ffb7cd8
#define SOC_MEM_BT_EM_BTDM0_START           0x3ffb0000
#define SOC_MEM_BT_EM_BTDM0_END             0x3ffb09a8
#define SOC_MEM_BT_EM_BLE_START             0x3ffb09a8
#define SOC_MEM_BT_EM_BLE_END               0x3ffb1ddc
#define SOC_MEM_BT_EM_BTDM1_START           0x3ffb1ddc
#define SOC_MEM_BT_EM_BTDM1_END             0x3ffb2730
#define SOC_MEM_BT_EM_BREDR_START           0x3ffb2730
#define SOC_MEM_BT_EM_BREDR_NO_SYNC_END     0x3ffb6388  //Not calculate with synchronize connection support
#define SOC_MEM_BT_EM_BREDR_END             0x3ffb7cd8  //Calculate with synchronize connection support
#define SOC_MEM_BT_EM_SYNC0_START           0x3ffb6388
#define SOC_MEM_BT_EM_SYNC0_END             0x3ffb6bf8
#define SOC_MEM_BT_EM_SYNC1_START           0x3ffb6bf8
#define SOC_MEM_BT_EM_SYNC1_END             0x3ffb7468
#define SOC_MEM_BT_EM_SYNC2_START           0x3ffb7468
#define SOC_MEM_BT_EM_SYNC2_END             0x3ffb7cd8
#define SOC_MEM_BT_BSS_START                0x3ffb8000
#define SOC_MEM_BT_BSS_END                  0x3ffb9a20
#define SOC_MEM_BT_MISC_START               0x3ffbdb28
#define SOC_MEM_BT_MISC_END                 0x3ffbdb5c

#define SOC_MEM_BT_EM_PER_SYNC_SIZE         0x870

#define SOC_MEM_BT_EM_BREDR_REAL_END        (SOC_MEM_BT_EM_BREDR_NO_SYNC_END + CONFIG_BTDM_CTRL_BR_EDR_MAX_SYNC_CONN_EFF * SOC_MEM_BT_EM_PER_SYNC_SIZE)

#endif //CONFIG_BT_ENABLED

/**
* @brief Internal use only
*
* @note Please do not modify this value
*/
#define ESP_BT_CONTROLLER_CONFIG_MAGIC_VAL  0x20250318

/**
 * @brief Bluetooth Controller mode
 */
typedef enum {
    ESP_BT_MODE_IDLE       = 0x00,   /*!< Bluetooth is not operating. */
    ESP_BT_MODE_BLE        = 0x01,   /*!< Bluetooth is operating in BLE mode. */
    ESP_BT_MODE_CLASSIC_BT = 0x02,   /*!< Bluetooth is operating in Classic Bluetooth mode. */
    ESP_BT_MODE_BTDM       = 0x03,   /*!< Bluetooth is operating in Dual mode. */
} esp_bt_mode_t;

/**
 * @brief BLE sleep clock accuracy (SCA)
 *
 * @note Currently only ESP_BLE_SCA_500PPM and ESP_BLE_SCA_250PPM are supported.
 */
typedef enum {
    ESP_BLE_SCA_500PPM     = 0,   /*!< BLE SCA at 500 ppm */
    ESP_BLE_SCA_250PPM,           /*!< BLE SCA at 250 ppm */
    ESP_BLE_SCA_150PPM,           /*!< BLE SCA at 150 ppm */
    ESP_BLE_SCA_100PPM,           /*!< BLE SCA at 100 ppm */
    ESP_BLE_SCA_75PPM,            /*!< BLE SCA at 75 ppm */
    ESP_BLE_SCA_50PPM,            /*!< BLE SCA at 50 ppm */
    ESP_BLE_SCA_30PPM,            /*!< BLE SCA at 30 ppm */
    ESP_BLE_SCA_20PPM,            /*!< BLE SCA at 20 ppm */
} esp_ble_sca_t;

#ifdef CONFIG_BT_ENABLED
/* While scanning, if the free memory value in controller is less than SCAN_SEND_ADV_RESERVED_SIZE,
the advertising packet will be discarded until the memory is restored. */
#define SCAN_SEND_ADV_RESERVED_SIZE        1000
/* enable controller log debug when the advertising packet gets lost */
#define CONTROLLER_ADV_LOST_DEBUG_BIT      (0<<0)

#ifdef CONFIG_BTDM_CTRL_HCI_UART_NO
#define BT_HCI_UART_NO_DEFAULT                      CONFIG_BTDM_CTRL_HCI_UART_NO
#else
#define BT_HCI_UART_NO_DEFAULT                      1
#endif /* BT_HCI_UART_NO_DEFAULT */

#ifdef CONFIG_BTDM_CTRL_HCI_UART_BAUDRATE
#define BT_HCI_UART_BAUDRATE_DEFAULT                CONFIG_BTDM_CTRL_HCI_UART_BAUDRATE
#else
#define BT_HCI_UART_BAUDRATE_DEFAULT                921600
#endif /* BT_HCI_UART_BAUDRATE_DEFAULT */

#ifdef CONFIG_BTDM_SCAN_DUPL_TYPE
#define SCAN_DUPLICATE_TYPE_VALUE  CONFIG_BTDM_SCAN_DUPL_TYPE
#else
#define SCAN_DUPLICATE_TYPE_VALUE  0
#endif

/* normal advertising cache size */
#ifdef CONFIG_BTDM_SCAN_DUPL_CACHE_SIZE
#define NORMAL_SCAN_DUPLICATE_CACHE_SIZE            CONFIG_BTDM_SCAN_DUPL_CACHE_SIZE
#else
#define NORMAL_SCAN_DUPLICATE_CACHE_SIZE            20
#endif

#ifndef CONFIG_BTDM_BLE_MESH_SCAN_DUPL_EN
#define CONFIG_BTDM_BLE_MESH_SCAN_DUPL_EN FALSE
#endif

#define SCAN_DUPLICATE_MODE_NORMAL_ADV_ONLY         0
#define SCAN_DUPLICATE_MODE_NORMAL_ADV_MESH_ADV     1

#if CONFIG_BTDM_BLE_MESH_SCAN_DUPL_EN
    #define SCAN_DUPLICATE_MODE                     SCAN_DUPLICATE_MODE_NORMAL_ADV_MESH_ADV
    #ifdef CONFIG_BTDM_MESH_DUPL_SCAN_CACHE_SIZE
    #define MESH_DUPLICATE_SCAN_CACHE_SIZE          CONFIG_BTDM_MESH_DUPL_SCAN_CACHE_SIZE
    #else
    #define MESH_DUPLICATE_SCAN_CACHE_SIZE          50
    #endif
#else
    #define SCAN_DUPLICATE_MODE SCAN_DUPLICATE_MODE_NORMAL_ADV_ONLY
    #define MESH_DUPLICATE_SCAN_CACHE_SIZE          0
#endif

#ifdef CONFIG_BTDM_SCAN_DUPL_CACHE_REFRESH_PERIOD
#define SCAN_DUPL_CACHE_REFRESH_PERIOD CONFIG_BTDM_SCAN_DUPL_CACHE_REFRESH_PERIOD
#else
#define SCAN_DUPL_CACHE_REFRESH_PERIOD              0
#endif

#if defined(CONFIG_BTDM_CTRL_MODE_BLE_ONLY)
#define BTDM_CONTROLLER_MODE_EFF                    ESP_BT_MODE_BLE
#elif defined(CONFIG_BTDM_CTRL_MODE_BR_EDR_ONLY)
#define BTDM_CONTROLLER_MODE_EFF                    ESP_BT_MODE_CLASSIC_BT
#else
#define BTDM_CONTROLLER_MODE_EFF                    ESP_BT_MODE_BTDM
#endif

#ifdef CONFIG_BTDM_CTRL_AUTO_LATENCY_EFF
#define BTDM_CTRL_AUTO_LATENCY_EFF CONFIG_BTDM_CTRL_AUTO_LATENCY_EFF
#else
#define BTDM_CTRL_AUTO_LATENCY_EFF false
#endif

#ifdef CONFIG_BTDM_CTRL_HLI
#define BTDM_CTRL_HLI CONFIG_BTDM_CTRL_HLI
#else
#define BTDM_CTRL_HLI false
#endif

#ifdef CONFIG_BTDM_CTRL_LEGACY_AUTH_VENDOR_EVT_EFF
#define BTDM_CTRL_LEGACY_AUTH_VENDOR_EVT_EFF CONFIG_BTDM_CTRL_LEGACY_AUTH_VENDOR_EVT_EFF
#else
#define BTDM_CTRL_LEGACY_AUTH_VENDOR_EVT_EFF false
#endif

#define BTDM_CONTROLLER_BLE_MAX_CONN_LIMIT          9   //Maximum BLE connection limitation
#define BTDM_CONTROLLER_BR_EDR_MAX_ACL_CONN_LIMIT   7   //Maximum ACL connection limitation
#define BTDM_CONTROLLER_BR_EDR_MAX_SYNC_CONN_LIMIT  3   //Maximum SCO/eSCO connection limitation

#define BTDM_CONTROLLER_SCO_DATA_PATH_HCI           0   // SCO data is routed to HCI
#define BTDM_CONTROLLER_SCO_DATA_PATH_PCM           1   // SCO data path is PCM

#ifdef CONFIG_BTDM_CTRL_SCAN_BACKOFF_UPPERLIMITMAX
#define BTDM_CTRL_SCAN_BACKOFF_UPPERLIMITMAX CONFIG_BTDM_CTRL_SCAN_BACKOFF_UPPERLIMITMAX
#else
#define BTDM_CTRL_SCAN_BACKOFF_UPPERLIMITMAX  0
#endif

#ifdef CONFIG_BTDM_BLE_LLCP_CONN_UPDATE
#define BTDM_BLE_LLCP_CONN_UPDATE (1<<0)
#else
#define BTDM_BLE_LLCP_CONN_UPDATE (0<<0)
#endif

#ifdef CONFIG_BTDM_BLE_LLCP_CHAN_MAP_UPDATE
#define BTDM_BLE_LLCP_CHAN_MAP_UPDATE (1<<1)
#else
#define BTDM_BLE_LLCP_CHAN_MAP_UPDATE (0<<1)
#endif

#define BTDM_BLE_LLCP_DISC_FLAG (BTDM_BLE_LLCP_CONN_UPDATE | BTDM_BLE_LLCP_CHAN_MAP_UPDATE)

#ifdef CONFIG_BTDM_CTRL_CHECK_CONNECT_IND_ACCESS_ADDRESS
#define BTDM_CTRL_CHECK_CONNECT_IND_ACCESS_ADDRESS_ENABLED CONFIG_BTDM_CTRL_CHECK_CONNECT_IND_ACCESS_ADDRESS
#else
#define BTDM_CTRL_CHECK_CONNECT_IND_ACCESS_ADDRESS_ENABLED 0
#endif

#if defined(CONFIG_BTDM_BLE_CHAN_ASS_EN)
#define BTDM_BLE_CHAN_ASS_EN (CONFIG_BTDM_BLE_CHAN_ASS_EN)
#else
#define BTDM_BLE_CHAN_ASS_EN (0)
#endif

#if CONFIG_BTDM_CTRL_CONTROLLER_DEBUG_MODE_1
#define BTDM_CTRL_CONTROLLER_DEBUG_MODE_1  (1 << 1)
#else
#define BTDM_CTRL_CONTROLLER_DEBUG_MODE_1  0
#endif

#ifndef BTDM_CTRL_CONTROLLER_DEBUG_FLAG
#define BTDM_CTRL_CONTROLLER_DEBUG_FLAG    (BTDM_CTRL_CONTROLLER_DEBUG_MODE_1 | CONTROLLER_ADV_LOST_DEBUG_BIT)
#endif

#if defined(CONFIG_BTDM_BLE_PING_EN)
#define BTDM_BLE_PING_EN (CONFIG_BTDM_BLE_PING_EN)
#else
#define BTDM_BLE_PING_EN (0)
#endif

/**
* @brief  Default Bluetooth Controller configuration
*/
#define BT_CONTROLLER_INIT_CONFIG_DEFAULT() {                              \
    .controller_task_stack_size = ESP_TASK_BT_CONTROLLER_STACK,            \
    .controller_task_prio = ESP_TASK_BT_CONTROLLER_PRIO,                   \
    .hci_uart_no = BT_HCI_UART_NO_DEFAULT,                                 \
    .hci_uart_baudrate = BT_HCI_UART_BAUDRATE_DEFAULT,                     \
    .scan_duplicate_mode = SCAN_DUPLICATE_MODE,                            \
    .scan_duplicate_type = SCAN_DUPLICATE_TYPE_VALUE,                      \
    .normal_adv_size = NORMAL_SCAN_DUPLICATE_CACHE_SIZE,                   \
    .mesh_adv_size = MESH_DUPLICATE_SCAN_CACHE_SIZE,                       \
    .send_adv_reserved_size = SCAN_SEND_ADV_RESERVED_SIZE,                 \
    .controller_debug_flag = BTDM_CTRL_CONTROLLER_DEBUG_FLAG,              \
    .mode = BTDM_CONTROLLER_MODE_EFF,                                      \
    .ble_max_conn = CONFIG_BTDM_CTRL_BLE_MAX_CONN_EFF,                     \
    .bt_max_acl_conn = CONFIG_BTDM_CTRL_BR_EDR_MAX_ACL_CONN_EFF,           \
    .bt_sco_datapath = CONFIG_BTDM_CTRL_BR_EDR_SCO_DATA_PATH_EFF,          \
    .auto_latency = BTDM_CTRL_AUTO_LATENCY_EFF,                            \
    .bt_legacy_auth_vs_evt = BTDM_CTRL_LEGACY_AUTH_VENDOR_EVT_EFF,         \
    .bt_max_sync_conn = CONFIG_BTDM_CTRL_BR_EDR_MAX_SYNC_CONN_EFF,         \
    .ble_sca = CONFIG_BTDM_BLE_SLEEP_CLOCK_ACCURACY_INDEX_EFF,             \
    .pcm_role = CONFIG_BTDM_CTRL_PCM_ROLE_EFF,                             \
    .pcm_polar = CONFIG_BTDM_CTRL_PCM_POLAR_EFF,                           \
    .pcm_fsyncshp = CONFIG_BTDM_CTRL_PCM_FSYNCSHP_EFF,                     \
    .hli = BTDM_CTRL_HLI,                                                  \
    .enc_key_sz_min = CONFIG_BTDM_CTRL_BR_EDR_MIN_ENC_KEY_SZ_DFT_EFF,      \
    .dup_list_refresh_period = SCAN_DUPL_CACHE_REFRESH_PERIOD,             \
    .ble_scan_backoff = BTDM_CTRL_SCAN_BACKOFF_UPPERLIMITMAX,              \
    .ble_llcp_disc_flag = BTDM_BLE_LLCP_DISC_FLAG,                         \
    .ble_aa_check = BTDM_CTRL_CHECK_CONNECT_IND_ACCESS_ADDRESS_ENABLED,    \
    .ble_chan_ass_en = BTDM_BLE_CHAN_ASS_EN,                               \
    .ble_ping_en = BTDM_BLE_PING_EN,                                       \
    .magic = ESP_BT_CONTROLLER_CONFIG_MAGIC_VAL,                           \
}

#else
/**
* @brief  Default Bluetooth Controller configuration
*/
#define BT_CONTROLLER_INIT_CONFIG_DEFAULT() {0}; ESP_STATIC_ASSERT(0, "please enable bluetooth in menuconfig to use esp_bt.h");
#endif

/**
 * @brief Bluetooth Controller config options
 * @note
 *      1. For parameters configurable through menuconfig, it is recommended to adjust them via the menuconfig interface. Please refer to menuconfig for details on the range and default values.
 *      2. It is not recommended to modify the values for parameters which are not configurable through menuconfig.
 */
typedef struct {
    uint16_t controller_task_stack_size;    /*!< Bluetooth Controller task stack size in bytes */
    uint8_t controller_task_prio;           /*!< Bluetooth Controller task priority */
    uint8_t hci_uart_no;                    /*!< UART number as HCI I/O interface. Configurable in menuconfig.
                                                - 1 - URAT 1 (default)
                                                - 2 - URAT 2 */
    uint32_t hci_uart_baudrate;             /*!<  UART baudrate. Configurable in menuconfig.
                                                - Range: 115200 - 921600
                                                - Default: 921600 */
    uint8_t scan_duplicate_mode;            /*!< Scan duplicate filtering mode. Configurable in menuconfig.
                                                - 0 - Normal scan duplicate filtering mode (default)
                                                - 1 - Special scan duplicate filtering mode for BLE Mesh */
    uint8_t scan_duplicate_type;            /*!< Scan duplicate filtering type. If `scan_duplicate_mode` is set to 1, this parameter will be ignored. Configurable in menuconfig.
                                                - 0 - Filter scan duplicates by device address only (default)
                                                - 1 - Filter scan duplicates by advertising data only, even if they originate from different devices.
                                                - 2 - Filter scan duplicated by device address and advertising data. */
    uint16_t normal_adv_size;               /*!< Maximum number of devices in scan duplicate filtering list. Configurable in menuconfig
                                                - Range: 10 - 1000
                                                - Default: 100 */
    uint16_t mesh_adv_size;                 /*!< Maximum number of Mesh advertising packets in scan duplicate filtering list. Configurable in menuconfig
                                                - Range: 10 - 1000
                                                - Default: 100 */
    uint16_t send_adv_reserved_size;        /*!< Controller minimum memory value in bytes. Internal use only */
    uint32_t  controller_debug_flag;        /*!< Controller debug log flag. Internal use only */
    uint8_t mode;                           /*!< Controller mode.  Configurable in menuconfig
                                                - 1 - BLE mode
                                                - 2 - Classic Bluetooth mode
                                                - 3 - Dual mode
                                                - 4 - Others: Invalid */
    uint8_t ble_max_conn;                   /*!< Maximum number of BLE connections. Configurable in menuconfig
                                                - Range: 1 - 9
                                                - Default: 3 */
    uint8_t bt_max_acl_conn;                /*!< Maximum number of BR/EDR ACL connections. Configurable in menuconfig
                                                - Range: 1 - 7
                                                - Default: 2 */
    uint8_t bt_sco_datapath;                /*!< SCO data path. Configurable in menuconfig
                                                - 0 - HCI module (default)
                                                - 1 - PCM module */
    bool auto_latency;                      /*!< True if BLE auto latency is enabled, used to enhance Classic Bluetooth performance in the Dual mode; false otherwise (default). Configurable in menuconfig */
    bool bt_legacy_auth_vs_evt;             /*!< True if BR/EDR Legacy Authentication Vendor Specific Event is enabled (default in the classic bluetooth or Dual mode), which is required to protect from BIAS attack; false otherwise. Configurable in menuconfig */
    uint8_t bt_max_sync_conn;               /*!< Maximum number of BR/EDR synchronous connections. Configurable in menuconfig
                                                - Range: 0 - 3
                                                - Default: 0 */
    uint8_t ble_sca;                        /*!< BLE low power crystal accuracy index. Configurable in menuconfig
                                                - 0 - `BTDM_BLE_DEFAULT_SCA_500PPM`
                                                - 1 - `BTDM_BLE_DEFAULT_SCA_250PPM` (default) */
    uint8_t pcm_role;                       /*!< PCM role. Configurable in menuconfig
                                                - 0 - PCM master (default)
                                                - 1 - PCM slave (default) */
    uint8_t pcm_polar;                      /*!< PCM polarity (falling clk edge & rising clk edge). Configurable in menuconfig
                                                - 0 - Falling Edge (default)
                                                - 1 - Rising Edge */
    uint8_t pcm_fsyncshp;                   /*!< Physical shape of the PCM Frame Synchronization signal. Configurable in menuconfig
                                                - 0 - Stereo Mode (default)
                                                - 1 - Mono Mode 1
                                                - 2 - Mono Mode 2 */
    bool hli;                               /*!< True if using high-level (level 4) interrupt (default); false otherwise. Configurable in menuconfig */
    uint8_t enc_key_sz_min;                 /*!< Minimum size of the encryption key
                                                - Range: 7 - 16
                                                - Default: 7 */
    uint16_t dup_list_refresh_period;       /*!< Scan duplicate filtering list refresh period in seconds. Configurable in menuconfig
                                                - Range: 0 - 100 seconds
                                                - Default: 0 second */
    bool ble_scan_backoff;                  /*!< True if BLE scan backoff is enabled; false otherwise (default). Configurable in menuconfig */
    uint8_t ble_llcp_disc_flag;             /*!< Flag indicating whether the Controller disconnects after Instant Passed (0x28) error occurs. Configurable in menuconfig.
                                                - The Controller does not disconnect after Instant Passed (0x28) by default. */
    bool ble_aa_check;                      /*!< True if adds a verification step for the Access Address within the `CONNECT_IND` PDU; false otherwise (default). Configurable in menuconfig */
    uint8_t ble_chan_ass_en;                /*!< True if BLE channel assessment is enabled (default), false otherwise. Configurable in menuconfig */
    uint8_t ble_ping_en;                    /*!< True if BLE ping procedure is enabled (default), false otherwise. Configurable in menuconfig */
    uint32_t magic;                         /*!< Magic number */
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
 * @note
 *       1. The connection TX power can only be set after the connection is established.
 *          After disconnecting, the corresponding TX power will not be affected.
 *       2. `ESP_BLE_PWR_TYPE_DEFAULT` can be used to set the TX power for power types that have not been set before.
 *          It will not affect the TX power values which have been set for the ADV/SCAN/CONN0-8 power types.
 *       3. If none of power type is set, the system will use `ESP_PWR_LVL_P3` as default for all power types.
 */
typedef enum {
    ESP_BLE_PWR_TYPE_CONN_HDL0  = 0,            /*!< TX power for connection handle 0 */
    ESP_BLE_PWR_TYPE_CONN_HDL1  = 1,            /*!< TX power for connection handle 1 */
    ESP_BLE_PWR_TYPE_CONN_HDL2  = 2,            /*!< TX power for connection handle 2 */
    ESP_BLE_PWR_TYPE_CONN_HDL3  = 3,            /*!< TX power for connection handle 3 */
    ESP_BLE_PWR_TYPE_CONN_HDL4  = 4,            /*!< TX power for connection handle 4 */
    ESP_BLE_PWR_TYPE_CONN_HDL5  = 5,            /*!< TX power for connection handle 5 */
    ESP_BLE_PWR_TYPE_CONN_HDL6  = 6,            /*!< TX power for connection handle 6 */
    ESP_BLE_PWR_TYPE_CONN_HDL7  = 7,            /*!< TX power for connection handle 7 */
    ESP_BLE_PWR_TYPE_CONN_HDL8  = 8,            /*!< TX power for connection handle 8 */
    ESP_BLE_PWR_TYPE_ADV        = 9,            /*!< TX power for advertising */
    ESP_BLE_PWR_TYPE_SCAN       = 10,           /*!< TX power for scan */
    ESP_BLE_PWR_TYPE_DEFAULT    = 11,           /*!< Default TX power type, which can be used to set the TX power for power types that have not been set before.*/
    ESP_BLE_PWR_TYPE_NUM        = 12,           /*!< Number of types */
} esp_ble_power_type_t;

/**
 * @brief Bluetooth TX power level (index). Each index corresponds to a specific power value in dBm.
 */
typedef enum {
    ESP_PWR_LVL_N12 = 0,                /*!< Corresponding to -12 dBm */
    ESP_PWR_LVL_N9  = 1,                /*!< Corresponding to -9 dBm */
    ESP_PWR_LVL_N6  = 2,                /*!< Corresponding to -6 dBm */
    ESP_PWR_LVL_N3  = 3,                /*!< Corresponding to -3 dBm */
    ESP_PWR_LVL_N0  = 4,                /*!< Corresponding to 0 dBm */
    ESP_PWR_LVL_P3  = 5,                /*!< Corresponding to +3 dBm */
    ESP_PWR_LVL_P6  = 6,                /*!< Corresponding to +6 dBm */
    ESP_PWR_LVL_P9  = 7,                /*!< Corresponding to +9 dBm */
    ESP_PWR_LVL_N14 = ESP_PWR_LVL_N12,  /*!< Backward compatibility! Setting to -14 dBm will actually result in -12 dBm */
    ESP_PWR_LVL_N11 = ESP_PWR_LVL_N9,   /*!< Backward compatibility! Setting to -11 dBm will actually result in -9 dBm */
    ESP_PWR_LVL_N8  = ESP_PWR_LVL_N6,   /*!< Backward compatibility! Setting to  -8 dBm will actually result in -6 dBm */
    ESP_PWR_LVL_N5  = ESP_PWR_LVL_N3,   /*!< Backward compatibility! Setting to  -5 dBm will actually result in -3 dBm */
    ESP_PWR_LVL_N2  = ESP_PWR_LVL_N0,   /*!< Backward compatibility! Setting to  -2 dBm will actually result in 0 dBm */
    ESP_PWR_LVL_P1  = ESP_PWR_LVL_P3,   /*!< Backward compatibility! Setting to  +1 dBm will actually result in +3 dBm */
    ESP_PWR_LVL_P4  = ESP_PWR_LVL_P6,   /*!< Backward compatibility! Setting to  +4 dBm will actually result in +6 dBm */
    ESP_PWR_LVL_P7  = ESP_PWR_LVL_P9,   /*!< Backward compatibility! Setting to  +7 dBm will actually result in +9 dBm */
} esp_power_level_t;

/**
 * @brief Bluetooth audio data transport path
 */
typedef enum {
    ESP_SCO_DATA_PATH_HCI = 0,            /*!< data over HCI transport */
    ESP_SCO_DATA_PATH_PCM = 1,            /*!< data over PCM interface */
} esp_sco_data_path_t;

/**
 * @brief Bluetooth sleep clock
 */
typedef enum {
    ESP_BT_SLEEP_CLOCK_NONE            = 0,   /*!< Sleep clock not configured */
    ESP_BT_SLEEP_CLOCK_MAIN_XTAL       = 1,   /*!< SoC main crystal */
    ESP_BT_SLEEP_CLOCK_EXT_32K_XTAL    = 2,   /*!< External 32.768kHz crystal/oscillator */
} esp_bt_sleep_clock_t;

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
 *        - ESP_ERR_INVALID_ARG: Invalid arguments
 *        - ESP_ERR_NO_MEM: Out of memory
 */
esp_err_t esp_bt_controller_init(esp_bt_controller_config_t *cfg);

/**
 * @brief  De-initialize Bluetooth Controller to free resources and delete tasks
 *
 * @note
 *      1. You should stop advertising and scanning, and disconnect all existing connections before de-initializing Bluetooth Controller.
 *      2. This function should be called after `esp_bt_controller_disable` if the Controller was enabled before.
 *      3. This function should be called only once, after any other Bluetooth functions.
 *
 * @return
 *      - ESP_OK: Success
 *      - ESP_ERR_INVALID_STATE: Invalid Bluetooth Controller state
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
 * @param[in] mode The Bluetooth Controller mode (BLE/Classic Bluetooth/BTDM) to enable
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
 *    4. If your firmware will upgrade the Bluetooth Controller mode later (such as switching from BLE to Classic Bluetooth or from disabled to enabled), then do not call this function.
 *
 * If you never intend to use Bluetooth in a current boot-up cycle, calling `esp_bt_controller_mem_release(ESP_BT_MODE_BTDM)` could release the BSS and data consumed by both Classic Bluetooth and BLE Controller to heap.
 *
 * If you intend to use BLE only, calling `esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT)` could release the BSS and data consumed by Classic Bluetooth Controller. You can then continue using BLE.
 *
 * If you intend to use Classic Bluetooth only, calling `esp_bt_controller_mem_release(ESP_BT_MODE_BLE)` could release the BSS and data consumed by BLE Controller. You can then continue using Classic Bluetooth.
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

/** @brief Release the Controller memory, BSS and data section of the Classic Bluetooth/BLE Host stack as per the mode
 *
 * @note
 *    1. This function is optional and should be called only if you want to free up memory for other components.
 *    2. This function should only be called when the Controller is in `ESP_BT_CONTROLLER_STATUS_IDLE` status.
 *    3. Once Bluetooth Controller memory is released, the process cannot be reversed. This means you cannot use the Bluetooth Controller mode that you have released using this function.
 *    4. If your firmware will upgrade the Bluetooth Controller mode later (such as switching from BLE to Classic Bluetooth or from disabled to enabled), then do not call this function.
 *
 * This function first releases Controller memory by internally calling `esp_bt_controller_mem_release()`, then releases Host memory.
 *
 * If you never intend to use Bluetooth in a current boot-up cycle, calling `esp_bt_mem_release(ESP_BT_MODE_BTDM)` could release the BSS and data consumed by both Classic Bluetooth and BLE stack to heap.
 *
 * If you intend to use BLE only, calling `esp_bt_mem_release(ESP_BT_MODE_CLASSIC_BT)` could release the BSS and data consumed by Classic Bluetooth. You can then continue using BLE.
 *
 * If you intend to use Classic Bluetooth only, calling `esp_bt_mem_release(ESP_BT_MODE_BLE)` could release the BSS and data consumed by BLE. You can then continue using Classic Bluetooth.
 *
 * For example, if you only use Bluetooth for setting the Wi-Fi configuration, and do not use Bluetooth in the rest of the product operation,
 *  after receiving the Wi-Fi configuration, you can disable/de-init Bluetooth and release its memory.
 * Below is the sequence of APIs to be called for such scenarios:
 *
 *       esp_bluedroid_disable();
 *       esp_bluedroid_deinit();
 *       esp_bt_controller_disable();
 *       esp_bt_controller_deinit();
 *       esp_bt_mem_release(ESP_BT_MODE_BTDM);
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
 *       - ESP_ERR_NOT_SUPPORTED: The modem sleep mode is not supported
 */
esp_err_t esp_bt_sleep_disable(void);

/**
 * @brief  Set BLE TX power
 *
 * @note Connection TX power should only be set after the connection is established.
 *
 * @param[in]  power_type The type of TX power. It could be Advertising, Connection, Default, etc.
 * @param[in]  power_level Power level (index) corresponding to the absolute value (dBm)
 *
 * @return
 *      - ESP_OK:   Success
 *      - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t esp_ble_tx_power_set(esp_ble_power_type_t power_type, esp_power_level_t power_level);

/**
 * @brief  Get BLE TX power
 *
 * @note Connection TX power should only be retrieved after the connection is established.
 *
 * @param[in]  power_type The type of TX power. It could be Advertising/Connection/Default and etc.
 *
 * @return
 *         - Power level
 *
 */
esp_power_level_t esp_ble_tx_power_get(esp_ble_power_type_t power_type);


/**
 * @brief  Set BLE TX power
 *
 * @note Connection TX power should only be set after the connection is established.
 *
 * @param[in]  power_type The type of TX power. It could be Advertising, Connection, Default, etc.
 * @param[in]  power_level Power level (index) corresponding to the absolute value (dBm)
 *
 * @return
 *      - ESP_OK:   Success
 *      - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t esp_ble_tx_power_set(esp_ble_power_type_t power_type, esp_power_level_t power_level);

/**
 * @brief Manually clear the BLE scan duplicate list
 *
 * @note
 *      1. This function name is incorrectly spelled, it will be fixed in release 5.x version.
 *      2. The scan duplicate list will be automatically cleared when the maximum amount of devices in the filter is reached.
 *         The amount of devices in the filter can be configured in menuconfig.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_ERR_INVALID_STATE: Invalid Bluetooth Controller state
 */
esp_err_t esp_ble_scan_duplicate_list_flush(void);
esp_err_t esp_ble_scan_dupilcate_list_flush(void);

/**
 * @brief  Set BR/EDR TX power
 *
 * BR/EDR power control will use the power within the range of minimum value and maximum value.
 * The power level will affect the global BR/EDR TX power for operations such as inquiry, page, and connection.
 *
 * @note
 *      1. Please call this function after `esp_bt_controller_enable()` and before any functions that cause RF transmission,
 *          such as performing discovery, profile initialization, and so on.
 *      2. For BR/EDR to use the new TX power for inquiry, call this function before starting an inquiry.
 *          If BR/EDR is already inquiring, restart the inquiry after calling this function.
 *
 * @param[in]  min_power_level The minimum power level. The default value is `ESP_PWR_LVL_N0`.
 * @param[in]  max_power_level The maximum power level. The default value is `ESP_PWR_LVL_P3`.
 *
 * @return
 *      - ESP_OK: Success
 *      - ESP_ERR_INVALID_ARG: Invalid argument
 *      - ESP_ERR_INVALID_STATE: Invalid Bluetooth Controller state
 */
esp_err_t esp_bredr_tx_power_set(esp_power_level_t min_power_level, esp_power_level_t max_power_level);

/**
 * @brief  Get BR/EDR TX power
 *
 * The corresponding power levels will be stored into the arguments.
 *
 * @param[out]  min_power_level Pointer to store the minimum power level
 * @param[out]  max_power_level The maximum power level
 *
 * @return
 *      - ESP_OK: Success
 *      - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t esp_bredr_tx_power_get(esp_power_level_t *min_power_level, esp_power_level_t *max_power_level);

/**
 * @brief  Set BR/EDR default SCO data path
 *
 * @note   This function should be called after the Controller is enabled, and before (e)SCO link is established.
 *
 * @param[in] data_path SCO data path
 *
 * @return
 *      - ESP_OK: Success
 *      - ESP_ERR_INVALID_STATE: Invalid Bluetooth Controller state
 */
esp_err_t esp_bredr_sco_datapath_set(esp_sco_data_path_t data_path);

/**
 * @brief Virtual HCI (VHCI) callback functions to notify the Host on the next operation
 */
typedef struct esp_vhci_host_callback {
    void (*notify_host_send_available)(void);               /*!< Callback to notify the Host that the Controller is ready to receive the packet */
    int (*notify_host_recv)(uint8_t *data, uint16_t len);   /*!< Callback to notify the Host that the Controller has a packet to send */
} esp_vhci_host_callback_t;

/**
 * @brief Check whether the Controller is ready to receive the packet
 *
 * If the return value is True, the Host can send the packet to the Controller.
 *
 * @note This function should be called before each `esp_vhci_host_send_packet()`.
 *
 * @return
 *       True if the Controller is ready to receive packets; false otherwise
 */
bool esp_vhci_host_check_send_available(void);

/**
 * @brief Send the packet to the Controller
 *
 * @note
 *      1. This function shall not be called within a critical section or when the scheduler is suspended.
 *      2. This function should be called only if `esp_vhci_host_check_send_available()` returns True.
 *
 * @param[in] data Pointer to the packet data
 * @param[in] len The packet length
 */
void esp_vhci_host_send_packet(uint8_t *data, uint16_t len);

/**
 * @brief Register the VHCI callback funations defined in `esp_vhci_host_callback` structure.
 *
 * @param[in] callback `esp_vhci_host_callback` type variable
 *
 * @return
 *      - ESP_OK: Success
 *      - ESP_FAIL: Failure
 */
esp_err_t esp_vhci_host_register_callback(const esp_vhci_host_callback_t *callback);

/**
 * @brief Get the Bluetooth module sleep clock source.
 *
 * @note This function should be called after `esp_bt_controller_init()`
 *
 * @return
 *      - Clock source used in Bluetooth low power mode
 */
esp_bt_sleep_clock_t esp_bt_get_lpclk_src(void);

/**
 * @brief Set the Bluetooth module sleep clock source.
 *
 * @note This function should be called before `esp_bt_controller_init()`
 *
 * @param[in] lpclk Bluetooth sleep clock source
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t esp_bt_set_lpclk_src(esp_bt_sleep_clock_t lpclk);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_BT_H__ */
