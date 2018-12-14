// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file PHY init parameters and API
 */


/**
 * @brief Structure holding PHY init parameters
 */
typedef struct {
	uint8_t params[128];                    /*!< opaque PHY initialization parameters */
} esp_phy_init_data_t;

/**
 * @brief Opaque PHY calibration data
 */
typedef struct {
    uint8_t version[4];                     /*!< PHY version */
    uint8_t mac[6];                         /*!< The MAC address of the station */
    uint8_t opaque[1894];                   /*!< calibration data */
} esp_phy_calibration_data_t;

typedef enum {
    PHY_RF_CAL_PARTIAL = 0x00000000,        /*!< Do part of RF calibration. This should be used after power-on reset. */
    PHY_RF_CAL_NONE    = 0x00000001,        /*!< Don't do any RF calibration. This mode is only suggested to be used after deep sleep reset. */
    PHY_RF_CAL_FULL    = 0x00000002         /*!< Do full RF calibration. Produces best results, but also consumes a lot of time and current. Suggested to be used once. */
} esp_phy_calibration_mode_t;


/**
 * @brief Modules for modem sleep 
 */
typedef enum{
    MODEM_BLE_MODULE,              //!< BLE controller used
    MODEM_CLASSIC_BT_MODULE,       //!< Classic BT controller used
    MODEM_WIFI_STATION_MODULE,     //!< Wi-Fi Station used
    MODEM_WIFI_SOFTAP_MODULE,      //!< Wi-Fi SoftAP used
    MODEM_WIFI_SNIFFER_MODULE,     //!< Wi-Fi Sniffer used
    MODEM_WIFI_NULL_MODULE,        //!< Wi-Fi Null mode used
    MODEM_USER_MODULE,             //!< User used
    MODEM_MODULE_COUNT             //!< Number of items
}modem_sleep_module_t;

/**
 * @brief Module WIFI mask for medem sleep
 */
#define MODEM_BT_MASK   ((1<<MODEM_BLE_MODULE)          |   \
                         (1<<MODEM_CLASSIC_BT_MODULE))

/**
 * @brief Module WIFI mask for medem sleep
 */
#define MODEM_WIFI_MASK ((1<<MODEM_WIFI_STATION_MODULE) |   \
                         (1<<MODEM_WIFI_SOFTAP_MODULE)  |   \
                         (1<<MODEM_WIFI_SNIFFER_MODULE) |   \
                         (1<<MODEM_WIFI_NULL_MODULE))

/**
 * @brief Modules needing to call phy_rf_init
 */
typedef enum{
    PHY_BT_MODULE,          //!< Bluetooth used
    PHY_WIFI_MODULE,        //!< Wi-Fi used
    PHY_MODEM_MODULE,       //!< Modem sleep used
    PHY_MODULE_COUNT        //!< Number of items
}phy_rf_module_t;

/**
 * @brief Get PHY init data
 *
 * If "Use a partition to store PHY init data" option is set in menuconfig,
 * This function will load PHY init data from a partition. Otherwise,
 * PHY init data will be compiled into the application itself, and this function
 * will return a pointer to PHY init data located in read-only memory (DROM).
 *
 * If "Use a partition to store PHY init data" option is enabled, this function
 * may return NULL if the data loaded from flash is not valid.
 *
 * @note Call esp_phy_release_init_data to release the pointer obtained using
 * this function after the call to esp_wifi_init.
 *
 * @return pointer to PHY init data structure
 */
const esp_phy_init_data_t* esp_phy_get_init_data();

/**
 * @brief Release PHY init data
 * @param data  pointer to PHY init data structure obtained from
 *              esp_phy_get_init_data function
 */
void esp_phy_release_init_data(const esp_phy_init_data_t* data);

/**
 * @brief Function called by esp_phy_init to load PHY calibration data
 *
 * This is a convenience function which can be used to load PHY calibration
 * data from NVS. Data can be stored to NVS using esp_phy_store_cal_data_to_nvs
 * function.
 *
 * If calibration data is not present in the NVS, or
 * data is not valid (was obtained for a chip with a different MAC address,
 * or obtained for a different version of software), this function will
 * return an error.
 *
 * If "Initialize PHY in startup code" option is set in menuconfig, this
 * function will be used to load calibration data. To provide a different
 * mechanism for loading calibration data, disable
 * "Initialize PHY in startup code" option in menuconfig and call esp_phy_init
 * function from the application. For an example usage of esp_phy_init and
 * this function, see esp_phy_store_cal_data_to_nvs function in cpu_start.c
 *
 * @param out_cal_data pointer to calibration data structure to be filled with
 *                     loaded data.
 * @return ESP_OK on success
 */
esp_err_t esp_phy_load_cal_data_from_nvs(esp_phy_calibration_data_t* out_cal_data);

/**
 * @brief Function called by esp_phy_init to store PHY calibration data
 *
 * This is a convenience function which can be used to store PHY calibration
 * data to the NVS. Calibration data is returned by esp_phy_init function.
 * Data saved using this function to the NVS can later be loaded using
 * esp_phy_store_cal_data_to_nvs function.
 *
 * If "Initialize PHY in startup code" option is set in menuconfig, this
 * function will be used to store calibration data. To provide a different
 * mechanism for storing calibration data, disable
 * "Initialize PHY in startup code" option in menuconfig and call esp_phy_init
 * function from the application.
 *
 * @param cal_data pointer to calibration data which has to be saved.
 * @return ESP_OK on success
 */
esp_err_t esp_phy_store_cal_data_to_nvs(const esp_phy_calibration_data_t* cal_data);

/**
 * @brief Initialize PHY and RF module
 *
 * PHY and RF module should be initialized in order to use WiFi or BT.
 * Now PHY and RF initializing job is done automatically when start WiFi or BT. Users should not
 * call this API in their application.
 *
 * @param init_data  PHY parameters. Default set of parameters can
 *                   be obtained by calling esp_phy_get_default_init_data
 *                   function.
 * @param mode  Calibration mode (Full, partial, or no calibration)
 * @param[inout] calibration_data
 * @return ESP_OK on success.
 * @return ESP_FAIL on fail.
 */
esp_err_t esp_phy_rf_init(const esp_phy_init_data_t* init_data,esp_phy_calibration_mode_t mode, 
        esp_phy_calibration_data_t* calibration_data, phy_rf_module_t module);

/**
 * @brief De-initialize PHY and RF module
 *
 * PHY module should be de-initialized in order to shutdown WiFi or BT.
 * Now PHY and RF de-initializing job is done automatically when stop WiFi or BT. Users should not
 * call this API in their application.
 *
 * @return ESP_OK on success.
 */
esp_err_t esp_phy_rf_deinit(phy_rf_module_t module);

/**
 * @brief Load calibration data from NVS and initialize PHY and RF module
 */
void esp_phy_load_cal_and_init(phy_rf_module_t module);

/**
 * @brief Module requires to enter modem sleep
 */
esp_err_t esp_modem_sleep_enter(modem_sleep_module_t module);

/**
 * @brief Module requires to exit modem sleep
 */
esp_err_t esp_modem_sleep_exit(modem_sleep_module_t module);

/**
 * @brief Register module to make it be able to require to enter/exit modem sleep
 *        Although the module has no sleep function, as long as the module use RF,
 *        it must call esp_modem_sleep_regsiter. Otherwise, other modules with sleep
 *        function will disable RF without checking the module which doesn't call
 *        esp_modem_sleep_regsiter.
 */
esp_err_t esp_modem_sleep_register(modem_sleep_module_t module);

/**
 * @brief De-register module from modem sleep list 
 */
esp_err_t esp_modem_sleep_deregister(modem_sleep_module_t module);

/**
 * @brief            Get the time stamp when PHY/RF was switched on
 * @return           return 0 if PHY/RF is never switched on. Otherwise return time in
 *                   microsecond since boot when phy/rf was last switched on
*/
int64_t esp_phy_rf_get_on_ts(void);
#ifdef __cplusplus
}
#endif

