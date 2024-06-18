/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
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
* @note Please do not modify this value.
*/
#define ESP_BT_CONTROLLER_CONFIG_MAGIC_VAL  0x20240315

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
 * @brief BLE sleep clock accuracy(SCA)
 *
 * @note Currently only ESP_BLE_SCA_500PPM and ESP_BLE_SCA_250PPM are supported
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
the adv packet will be discarded until the memory is restored. */
#define SCAN_SEND_ADV_RESERVED_SIZE        1000
/* enable controller log debug when adv lost */
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

/* normal adv cache size */
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
    .controller_debug_flag = CONTROLLER_ADV_LOST_DEBUG_BIT,                \
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
    .hli = BTDM_CTRL_HLI,                                                  \
    .dup_list_refresh_period = SCAN_DUPL_CACHE_REFRESH_PERIOD,             \
    .ble_scan_backoff = BTDM_CTRL_SCAN_BACKOFF_UPPERLIMITMAX,              \
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
 *     1. The following parameters can be configured at runtime when calling esp_bt_controller_init():
 *
 *      `controller_task_stack_size`, `controller_task_prio`, `hci_uart_no`, `hci_uart_baudrate`
 *      `scan_duplicate_mode`, `scan_duplicate_type`, `normal_adv_size`, `mesh_adv_size`, `send_adv_reserved_size`,
 *      `controller_debug_flag`, `mode`, `ble_max_conn`, `bt_max_acl_conn`, `bt_sco_datapath`, `auto_latency`,
 *      `bt_legacy_auth_vs_evt`
 *
 *     2. The following parameters cannot be configured at runtime when calling `esp_bt_controller_init()`.
 *         They will be overwritten with values in menuconfig or from a macro:
 *
 *         `bt_max_sync_conn`, `ble_sca`, `pcm_role`, `pcm_polar`, `hli`, `dup_list_refresh_period`, `ble_scan_backoff`,
 *         `magic`.
 */
typedef struct {
    /*
     * Following parameters can be configured runtime, when call esp_bt_controller_init()
     */
    uint16_t controller_task_stack_size;    /*!< Bluetooth Controller task stack size in bytes */
    uint8_t controller_task_prio;           /*!< Bluetooth Controller task priority */
    uint8_t hci_uart_no;                    /*!< If use UART1/2 as HCI IO interface, indicate UART number */
    uint32_t hci_uart_baudrate;             /*!< If use UART1/2 as HCI IO interface, indicate UART baudrate */
    uint8_t scan_duplicate_mode;            /*!< Scan duplicate filtering mode */
    uint8_t scan_duplicate_type;            /*!< Scan duplicate filtering type */
    uint16_t normal_adv_size;               /*!< Scan duplicate filtering list size with normal ADV */
    uint16_t mesh_adv_size;                 /*!< Scan duplicate filtering list size with mesh ADV */
    uint16_t send_adv_reserved_size;        /*!< Controller minimum memory value*/
    uint32_t  controller_debug_flag;        /*!< Controller debug log flag */
    uint8_t mode;                           /*!< Controller mode: BLE mode (1), Classic Bluetooth mode (2) or Dual mode (3) */
    uint8_t ble_max_conn;                   /*!< BLE maximum connection numbers */
    uint8_t bt_max_acl_conn;                /*!< BR/EDR maximum ACL connection numbers */
    uint8_t bt_sco_datapath;                /*!< SCO data path, i.e. HCI or PCM module */
    bool auto_latency;                      /*!< BLE auto latency, used to enhance classic BT performance */
    bool bt_legacy_auth_vs_evt;             /*!< BR/EDR Legacy auth complete event required to  protect from BIAS attack */
    /*
     * Following parameters can not be configured runtime when call esp_bt_controller_init()
     * It will be overwrite with a constant value which in menuconfig or from a macro.
     * So, do not modify the value when esp_bt_controller_init()
     */
    uint8_t bt_max_sync_conn;               /*!< BR/EDR maximum ACL connection numbers. Effective in menuconfig */
    uint8_t ble_sca;                        /*!< BLE low power crystal accuracy index */
    uint8_t pcm_role;                       /*!< PCM role (master & slave)*/
    uint8_t pcm_polar;                      /*!< PCM polar trig (falling clk edge & rising clk edge) */
    bool hli;                               /*!< Using high level interrupt or not */
    uint16_t dup_list_refresh_period;       /*!< Scan duplicate filtering list refresh period */
    bool ble_scan_backoff;                  /*!< BLE scan backoff */
    uint32_t magic;                         /*!< Magic number */
} esp_bt_controller_config_t;

/**
 * @brief Bluetooth Controller status
 */
typedef enum {
    ESP_BT_CONTROLLER_STATUS_IDLE = 0,  /*!< The Controller is not initialized or has been de-initialized.*/
    ESP_BT_CONTROLLER_STATUS_INITED,    /*!< The Controller has been initialized. But it is not enabled or has been disabled. */
    ESP_BT_CONTROLLER_STATUS_ENABLED,   /*!< The Controller has been initialized and enabled.  */
    ESP_BT_CONTROLLER_STATUS_NUM,       /*!< Controller status numbers */
} esp_bt_controller_status_t;

/**
 * @brief BLE TX power type
 * @note
 *       1. The connection TX power can only be set after the connection is established.
 *          After disconnecting, the corresponding TX power will not be affected.
 *       2. `ESP_BLE_PWR_TYPE_DEFAULT` can be used to set the TX power for power types that have not been set before.
 *          It will not affect the TX power values which have been set for the following CONN0-8/ADV/SCAN power types.
 *       3. If none of power type is set, the system will use `ESP_PWR_LVL_P3` as default for ADV/SCAN/CONN0-8.
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
    ESP_BLE_PWR_TYPE_NUM        = 12,           /*!< TYPE numbers */
} esp_ble_power_type_t;

/**
 * @brief Bluetooth TX power level (index). Each index corresponds to a specific power value in dBm.
 */
typedef enum {
    ESP_PWR_LVL_N12 = 0,                /*!< Corresponding to -12 dBm */
    ESP_PWR_LVL_N9  = 1,                /*!< Corresponding to  -9 dBm */
    ESP_PWR_LVL_N6  = 2,                /*!< Corresponding to  -6 dBm */
    ESP_PWR_LVL_N3  = 3,                /*!< Corresponding to  -3 dBm */
    ESP_PWR_LVL_N0  = 4,                /*!< Corresponding to   0 dBm */
    ESP_PWR_LVL_P3  = 5,                /*!< Corresponding to  +3 dBm */
    ESP_PWR_LVL_P6  = 6,                /*!< Corresponding to  +6 dBm */
    ESP_PWR_LVL_P9  = 7,                /*!< Corresponding to  +9 dBm */
    ESP_PWR_LVL_N14 = ESP_PWR_LVL_N12,  /*!< Backward compatibility! Setting to -14 dBm will actually result in -12 dBm */
    ESP_PWR_LVL_N11 = ESP_PWR_LVL_N9,   /*!< Backward compatibility! Setting to -11 dBm will actually result in  -9 dBm */
    ESP_PWR_LVL_N8  = ESP_PWR_LVL_N6,   /*!< Backward compatibility! Setting to  -8 dBm will actually result in  -6 dBm */
    ESP_PWR_LVL_N5  = ESP_PWR_LVL_N3,   /*!< Backward compatibility! Setting to  -5 dBm will actually result in  -3 dBm */
    ESP_PWR_LVL_N2  = ESP_PWR_LVL_N0,   /*!< Backward compatibility! Setting to  -2 dBm will actually result in   0 dBm */
    ESP_PWR_LVL_P1  = ESP_PWR_LVL_P3,   /*!< Backward compatibility! Setting to  +1 dBm will actually result in  +3 dBm */
    ESP_PWR_LVL_P4  = ESP_PWR_LVL_P6,   /*!< Backward compatibility! Setting to  +4 dBm will actually result in  +6 dBm */
    ESP_PWR_LVL_P7  = ESP_PWR_LVL_P9,   /*!< Backward compatibility! Setting to  +7 dBm will actually result in  +9 dBm */
} esp_power_level_t;

/**
 * @brief Bluetooth audio data transport path
 */
typedef enum {
    ESP_SCO_DATA_PATH_HCI = 0,            /*!< data over HCI transport */
    ESP_SCO_DATA_PATH_PCM = 1,            /*!< data over PCM interface */
} esp_sco_data_path_t;

/**
 * @brief  Set BLE TX power
 *
 * @note   Connection TX power should only be set after the connection is established.
 *
 * @param[in]  power_type The type of TX power. It could be Advertising/Connection/Default and etc.
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
 * @note    Connection TX power should only be get after the connection is established.
 *
 * @param[in]  power_type The type of TX power. It could be Advertising/Connection/Default and etc.
 *
 * @return
 *         - Power level
 *
 */
esp_power_level_t esp_ble_tx_power_get(esp_ble_power_type_t power_type);

/**
 * @brief  Set BR/EDR TX power
 *
 * BR/EDR power control will use the power within the range of minimum value and maximum value.
 * The power level will affect the global BR/EDR TX power for operations such as inquiry, page, and connection.
 *
 * @note
 *      1. Please call the function after `esp_bt_controller_enable()` and before any function that causes RF transmission,
 *          such as performing discovery, profile initialization, and so on.
 *      2. For BR/EDR to use the new TX power for inquiry, call this function before starting an inquiry.
 *          If BR/EDR is already inquiring, restart the inquiry after calling this function.
 *      3. The default minimum power level is `ESP_PWR_LVL_N0`, and the maximum power level is `ESP_PWR_LVL_P3`.
 *
 * @param[in]  min_power_level The minimum power level
 * @param[in]  max_power_level The maximum power level
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
 * The corresponding power level will be stored into the arguments.
 *
 * @param[out]  min_power_level The minimum power level
 * @param[out]  max_power_level  The maximum power level
 *
 * @return
 *      - ESP_OK: Success
 *      - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t esp_bredr_tx_power_get(esp_power_level_t *min_power_level, esp_power_level_t *max_power_level);

/**
 * @brief  Set default SCO data path
 *
 * @note   This function should be called after the Controller is enabled, and before (e)SCO link is established
 *
 * @param[in] data_path SCO data path
 *
 * @return
 *      - ESP_OK: Success
 *      - ESP_ERR_INVALID_STATE: Invalid Bluetooth Controller state
 */
esp_err_t esp_bredr_sco_datapath_set(esp_sco_data_path_t data_path);

/**
 * @brief       Initialize the Bluetooth Controller to allocate tasks and other resources
 *
 * @note        This function should be called only once, before any other Bluetooth functions.
 *
 * @param[in]  cfg Initial Bluetooth Controller configuration.
 *
 * @return
 *        - ESP_OK: Success
 *        - ESP_ERR_INVALID_STATE: Invalid Bluetooth Controller state
 */
esp_err_t esp_bt_controller_init(esp_bt_controller_config_t *cfg);

/**
 * @brief  De-initialize Bluetooth Controller to free resources and delete tasks
 *
 * @note
 *      1. You should stop advertising and scanning, as well as disconnect all existing connections before de-initializing Bluetooth Controller.
 *      2. This function should be called only once, after any other Bluetooth functions.
 *
 * @return
 *      - ESP_OK: Success
 *      - ESP_ERR_INVALID_ARG: Invalid argument
 *      - ESP_ERR_INVALID_STATE: Invalid Bluetooth Controller state
 *      - ESP_ERR_NO_MEM: Out of memory
 */
esp_err_t esp_bt_controller_deinit(void);

/**
 * @brief Enable Bluetooth Controller
 *
 * @note
 *       1. Bluetooth Controller cannot be enabled in `ESP_BT_CONTROLLER_STATUS_IDLE` status. It have to be initialized first.
 *       2. Due to a known issue, you cannot call `esp_bt_controller_enable()` a second time
 *       to change the Controller mode dynamically. To change the Controller mode, call
 *       `esp_bt_controller_disable()` and then call `esp_bt_controller_enable()` with the new mode.
 *
 * @param[in] mode the Bluetooth Controller mode (BLE/Classic BT/BTDM) to enable
 *
 * For API compatibility, retain this argument. This mode must match the mode specified in the `cfg` of `esp_bt_controller_init()`.
 *
 * @return
 *          - ESP_OK: Success
 *          - ESP_ERR_INVALID_STATE: Invalid Bluetooth Controller state
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
 *      - ESP_BT_CONTROLLER_STATUS_INITED:  The Controller has been initialized. But it is not enabled or has been disabled.
 *      - ESP_BT_CONTROLLER_STATUS_ENABLED: The Controller has been initialized and enabled.
 */
esp_bt_controller_status_t esp_bt_controller_get_status(void);

/**
 * @brief Vendor HCI (VHCI) callback functions to notify the Host on the next operation
 */
typedef struct esp_vhci_host_callback {
    void (*notify_host_send_available)(void);               /*!< Callback to notify the Host that the Controller is ready to receive the packet */
    int (*notify_host_recv)(uint8_t *data, uint16_t len);   /*!< Callback to notify the Host that the Controller has a packet to send */
} esp_vhci_host_callback_t;

/**
 * @brief Check whether the Controller is ready to receive the packet
 *
 *  If the return value is True, the Host can send the packet to the Controller.
 *
 *  @note This function should be called before each `esp_vhci_host_send_packet()`.
 *
 *  @return
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
 * @param[in] data the packet point
 * @param[in] len the packet length
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
 * @brief Release the Controller memory as per the mode
 *
 * This function releases the BSS, data and other sections of the Controller to heap. The total size is about 70k bytes.
 *
 * If the app calls `esp_bt_controller_enable(ESP_BT_MODE_BLE)` to use BLE only,
 * then it is safe to call `esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT)` at initialization time to free unused Classic Bluetooth  memory.
 *
 * If the mode is `ESP_BT_MODE_BTDM`, then it may be useful to call API `esp_bt_mem_release(ESP_BT_MODE_BTDM)` instead,
 * which internally calls `esp_bt_controller_mem_release(ESP_BT_MODE_BTDM)` and additionally releases the BSS and data
 * consumed by the BT/BLE Host stack to heap. For more details about usage please refer to the documentation of `esp_bt_mem_release()` function
 *
 * @note
 *      1. This function should be called only before `esp_bt_controller_init()` or after `esp_bt_controller_deinit()`.
 *      2. Once Bluetooth Controller memory is released, the process cannot be reversed. This means you cannot use the Bluetooth Controller mode that you have released using this function.
 *      3. If your firmware will upgrade the Bluetooth Controller mode later (such as switching from BLE to Classic Bluetooth or from disabled to enabled), then do not call this function.
 *
 * @param[in] mode the Bluetooth Controller mode
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_ERR_INVALID_STATE: Invalid Bluetooth Controller state
 *       - ESP_ERR_NOT_FOUND: Requested resource not found
 */
esp_err_t esp_bt_controller_mem_release(esp_bt_mode_t mode);

/** @brief Release Controller memory, BSS and data section of the BT/BLE Host stack as per the mode
 *
 * This function first releases Controller memory by internally calling `esp_bt_controller_mem_release()`.
 * Additionally, if the mode is set to `ESP_BT_MODE_BTDM`, it also releases the BSS and data consumed by the BT/BLE Host stack to heap
 *
 * If you never intend to use Bluetooth in a current boot-up cycle, you can call `esp_bt_mem_release(ESP_BT_MODE_BTDM)`
 * before `esp_bt_controller_init()` or after `esp_bt_controller_deinit()`.
 *
 * For example, if a user only uses Bluetooth for setting the WiFi configuration, and does not use Bluetooth in the rest of the product operation.
 * In such cases, after receiving the WiFi configuration, you can disable/de-init Bluetooth and release its memory.
 * Below is the sequence of APIs to be called for such scenarios:
 *
 *       esp_bluedroid_disable();
 *       esp_bluedroid_deinit();
 *       esp_bt_controller_disable();
 *       esp_bt_controller_deinit();
 *       esp_bt_mem_release(ESP_BT_MODE_BTDM);
 *
 * @note
 *    1.  Once Bluetooth Controller memory is released, the process cannot be reversed. This means you cannot use the Bluetooth Controller mode that you have released using this function.
 *    2.  If your firmware will upgrade the Bluetooth Controller mode later (such as switching from BLE to Classic Bluetooth or from disabled to enabled), then do not call this function.
 *    3.  In case of NimBLE Host, to release BSS and data memory to heap, the mode needs to be set to `ESP_BT_MODE_BTDM` as the Controller is in Dual mode.
 *
 * @param[in] mode the Bluetooth Controller mode
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
 * There are currently two options for Bluetooth modem sleep: ORIG mode and EVED mode, the latter being intended for BLE only.
 * The modem sleep mode could be configured in menuconfig.
 *
 * In ORIG mode, if there is no event to process, the Bluetooth Controller will periodically switch off some components and pause operation, then wake up according to the scheduled interval and resume work.
 * It can also wakeup earlier upon external request using function `esp_bt_controller_wakeup_request()`.
 *
 * @note  This function shall not be invoked before `esp_bt_controller_enable()`
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
 * @brief Manually clear scan duplicate list
 *
 * @note
 *      1. This function name is incorrectly spelled, it will be fixed in release 5.x version.
 *      2. The scan duplicate list will be automatically cleared when the maximum amount of device in the filter is reached.
 *         The amount of device in the filter can be configured in menuconfig.
 *
 * @return
 *       - ESP_OK: Success
 *       - ESP_ERR_INVALID_STATE: Invalid Bluetooth Controller state
 */
esp_err_t esp_ble_scan_dupilcate_list_flush(void);

/**
 * @brief Power on BT Wi-Fi power domain
 *
 * @note This function is not recommended to use due to potential risk.
 */
void esp_wifi_bt_power_domain_on(void);

/**
 * @brief Power off BT Wi-Fi power domain
 *
 * @note This function is not recommended to use due to potential risk.
 */
void esp_wifi_bt_power_domain_off(void);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_BT_H__ */
