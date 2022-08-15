/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
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

#define ESP_BT_CONTROLLER_CONFIG_MAGIC_VAL  0x20200622

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
 * @brief BLE sleep clock accuracy(SCA), values for ble_sca field in esp_bt_controller_config_t,
 *        currently only ESP_BLE_SCA_500PPM and ESP_BLE_SCA_250PPM are supported
 */
enum {
    ESP_BLE_SCA_500PPM     = 0,   /*!< BLE SCA at 500ppm */
    ESP_BLE_SCA_250PPM,           /*!< BLE SCA at 250ppm */
    ESP_BLE_SCA_150PPM,           /*!< BLE SCA at 150ppm */
    ESP_BLE_SCA_100PPM,           /*!< BLE SCA at 100ppm */
    ESP_BLE_SCA_75PPM,            /*!< BLE SCA at 75ppm */
    ESP_BLE_SCA_50PPM,            /*!< BLE SCA at 50ppm */
    ESP_BLE_SCA_30PPM,            /*!< BLE SCA at 30ppm */
    ESP_BLE_SCA_20PPM,            /*!< BLE SCA at 20ppm */
};

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
    .magic = ESP_BT_CONTROLLER_CONFIG_MAGIC_VAL,                           \
}

#else
#define BT_CONTROLLER_INIT_CONFIG_DEFAULT() {0}; _Static_assert(0, "please enable bluetooth in menuconfig to use bt.h");
#endif

/**
 * @brief Controller config options, depend on config mask.
 *        Config mask indicate which functions enabled, this means
 *        some options or parameters of some functions enabled by config mask.
 */
typedef struct {
    /*
     * Following parameters can be configured runtime, when call esp_bt_controller_init()
     */
    uint16_t controller_task_stack_size;    /*!< Bluetooth controller task stack size */
    uint8_t controller_task_prio;           /*!< Bluetooth controller task priority */
    uint8_t hci_uart_no;                    /*!< If use UART1/2 as HCI IO interface, indicate UART number */
    uint32_t hci_uart_baudrate;             /*!< If use UART1/2 as HCI IO interface, indicate UART baudrate */
    uint8_t scan_duplicate_mode;            /*!< scan duplicate mode */
    uint8_t scan_duplicate_type;            /*!< scan duplicate type */
    uint16_t normal_adv_size;               /*!< Normal adv size for scan duplicate */
    uint16_t mesh_adv_size;                 /*!< Mesh adv size for scan duplicate */
    uint16_t send_adv_reserved_size;        /*!< Controller minimum memory value */
    uint32_t  controller_debug_flag;        /*!< Controller debug log flag */
    uint8_t mode;                           /*!< Controller mode: BR/EDR, BLE or Dual Mode */
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
    uint32_t magic;                         /*!< Magic number */
} esp_bt_controller_config_t;

/**
 * @brief Bluetooth controller enable/disable/initialised/de-initialised status
 */
typedef enum {
    ESP_BT_CONTROLLER_STATUS_IDLE = 0,
    ESP_BT_CONTROLLER_STATUS_INITED,
    ESP_BT_CONTROLLER_STATUS_ENABLED,
    ESP_BT_CONTROLLER_STATUS_NUM,
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
    ESP_PWR_LVL_N12 = 0,                /*!< Corresponding to -12dbm */
    ESP_PWR_LVL_N9  = 1,                /*!< Corresponding to  -9dbm */
    ESP_PWR_LVL_N6  = 2,                /*!< Corresponding to  -6dbm */
    ESP_PWR_LVL_N3  = 3,                /*!< Corresponding to  -3dbm */
    ESP_PWR_LVL_N0  = 4,                /*!< Corresponding to   0dbm */
    ESP_PWR_LVL_P3  = 5,                /*!< Corresponding to  +3dbm */
    ESP_PWR_LVL_P6  = 6,                /*!< Corresponding to  +6dbm */
    ESP_PWR_LVL_P9  = 7,                /*!< Corresponding to  +9dbm */
    ESP_PWR_LVL_N14 = ESP_PWR_LVL_N12,  /*!< Backward compatibility! Setting to -14dbm will actually result to -12dbm */
    ESP_PWR_LVL_N11 = ESP_PWR_LVL_N9,   /*!< Backward compatibility! Setting to -11dbm will actually result to  -9dbm */
    ESP_PWR_LVL_N8  = ESP_PWR_LVL_N6,   /*!< Backward compatibility! Setting to  -8dbm will actually result to  -6dbm */
    ESP_PWR_LVL_N5  = ESP_PWR_LVL_N3,   /*!< Backward compatibility! Setting to  -5dbm will actually result to  -3dbm */
    ESP_PWR_LVL_N2  = ESP_PWR_LVL_N0,   /*!< Backward compatibility! Setting to  -2dbm will actually result to   0dbm */
    ESP_PWR_LVL_P1  = ESP_PWR_LVL_P3,   /*!< Backward compatibility! Setting to  +1dbm will actually result to  +3dbm */
    ESP_PWR_LVL_P4  = ESP_PWR_LVL_P6,   /*!< Backward compatibility! Setting to  +4dbm will actually result to  +6dbm */
    ESP_PWR_LVL_P7  = ESP_PWR_LVL_P9,   /*!< Backward compatibility! Setting to  +7dbm will actually result to  +9dbm */
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
 * @brief  Set BR/EDR TX power
 *         BR/EDR power control will use the power in range of minimum value and maximum value.
 *         The power level will effect the global BR/EDR TX power, such inquire, page, connection and so on.
 *         Please call the function after esp_bt_controller_enable and before any function which cause RF do TX.
 *         So you can call the function before doing discovery, profile init and so on.
 *         For example, if you want BR/EDR use the new TX power to do inquire, you should call
 *         this function before inquire. Another word, If call this function when BR/EDR is in inquire(ING),
 *         please do inquire again after call this function.
 *         Default minimum power level is ESP_PWR_LVL_N0, and maximum power level is ESP_PWR_LVL_P3.
 * @param  min_power_level: The minimum power level
 * @param  max_power_level: The maximum power level
 * @return              ESP_OK - success, other - failed
 */
esp_err_t esp_bredr_tx_power_set(esp_power_level_t min_power_level, esp_power_level_t max_power_level);

/**
 * @brief  Get BR/EDR TX power
 *         If the argument is not NULL, then store the corresponding value.
 * @param  min_power_level: The minimum power level
 * @param  max_power_level: The maximum power level
 * @return              ESP_OK - success, other - failed
 */
esp_err_t esp_bredr_tx_power_get(esp_power_level_t *min_power_level, esp_power_level_t *max_power_level);

/**
 * @brief  Set default SCO data path
 *         Should be called after controller is enabled, and before (e)SCO link is established
 * @param  data_path: SCO data path
 * @return              ESP_OK - success, other - failed
 */
esp_err_t esp_bredr_sco_datapath_set(esp_sco_data_path_t data_path);

/**
 * @brief       Initialize BT controller to allocate task and other resource.
 *              This function should be called only once, before any other BT functions are called.
 * @param  cfg: Initial configuration of BT controller. Different from previous version, there's a mode and some
 *              connection configuration in "cfg" to configure controller work mode and allocate the resource which is needed.
 * @return      ESP_OK - success, other - failed
 */
esp_err_t esp_bt_controller_init(esp_bt_controller_config_t *cfg);

/**
 * @brief  De-initialize BT controller to free resource and delete task.
 *         You should stop advertising and scanning, as well as
 *         disconnect all existing connections before de-initializing BT controller.
 *
 * This function should be called only once, after any other BT functions are called.
 * @return  ESP_OK - success, other - failed
 */
esp_err_t esp_bt_controller_deinit(void);

/**
 * @brief Enable BT controller.
 *               Due to a known issue, you cannot call esp_bt_controller_enable() a second time
 *               to change the controller mode dynamically. To change controller mode, call
 *               esp_bt_controller_disable() and then call esp_bt_controller_enable() with the new mode.
 * @param mode : the mode(BLE/BT/BTDM) to enable. For compatible of API, retain this argument. This mode must be
 *               equal as the mode in "cfg" of esp_bt_controller_init().
 * @return       ESP_OK - success, other - failed
 */
esp_err_t esp_bt_controller_enable(esp_bt_mode_t mode);

/**
 * @brief  Disable BT controller
 * @return       ESP_OK - success, other - failed
 */
esp_err_t esp_bt_controller_disable(void);

/**
 * @brief  Get BT controller is initialised/de-initialised/enabled/disabled
 * @return status value
 */
esp_bt_controller_status_t esp_bt_controller_get_status(void);

/** @brief esp_vhci_host_callback
 *  used for vhci call host function to notify what host need to do
 */
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
 * @note In case of NimBLE host, to release BSS and data memory to heap, the mode needs to be
 * set to ESP_BT_MODE_BTDM as controller is dual mode.
 * @param mode : the mode whose memory is to be released
 * @return ESP_OK - success, other - failed
 */
esp_err_t esp_bt_mem_release(esp_bt_mode_t mode);

/**
 * @brief enable bluetooth to enter modem sleep
 *
 * Note that this function shall not be invoked before esp_bt_controller_enable()
 *
 * There are currently two options for bluetooth modem sleep, one is ORIG mode, and another is EVED Mode. EVED Mode is intended for BLE only.
 *
 * For ORIG mode:
 * Bluetooth modem sleep is enabled in controller start up by default if CONFIG_CTRL_BTDM_MODEM_SLEEP is set and "ORIG mode" is selected. In ORIG modem sleep mode, bluetooth controller will switch off some components and pause to work every now and then, if there is no event to process; and wakeup according to the scheduled interval and resume the work. It can also wakeup earlier upon external request using function "esp_bt_controller_wakeup_request".
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 */
esp_err_t esp_bt_sleep_enable(void);


/**
 * @brief disable bluetooth modem sleep
 *
 * Note that this function shall not be invoked before esp_bt_controller_enable()
 *
 * If esp_bt_sleep_disable() is called, bluetooth controller will not be allowed to enter modem sleep;
 *
 * If ORIG modem sleep mode is in use, if this function is called, bluetooth controller may not immediately wake up if it is dormant then.
 * In this case, esp_bt_controller_wakeup_request() can be used to shorten the time for wakeup.
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 */
esp_err_t esp_bt_sleep_disable(void);

/**
 * @brief Manually clear scan duplicate list
 *
 * Note that scan duplicate list will be automatically cleared when the maximum amount of device in the filter is reached
 * the amount of device in the filter can be configured in menuconfig.
 *
 * @note This function name is incorrectly spelled, it will be fixed in release 5.x version.
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 */
esp_err_t esp_ble_scan_dupilcate_list_flush(void);

/**
 * @brief bt Wi-Fi power domain power on
 */
void esp_wifi_bt_power_domain_on(void);

/**
 * @brief bt Wi-Fi power domain power off
 */
void esp_wifi_bt_power_domain_off(void);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_BT_H__ */
