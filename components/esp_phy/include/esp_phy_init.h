/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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

#if CONFIG_ESP_PHY_MULTIPLE_INIT_DATA_BIN
/**
 * @brief PHY init data type
 */
typedef enum {
    ESP_PHY_INIT_DATA_TYPE_DEFAULT = 0,
    ESP_PHY_INIT_DATA_TYPE_SRRC,
    ESP_PHY_INIT_DATA_TYPE_FCC,
    ESP_PHY_INIT_DATA_TYPE_CE,
    ESP_PHY_INIT_DATA_TYPE_NCC,
    ESP_PHY_INIT_DATA_TYPE_KCC,
    ESP_PHY_INIT_DATA_TYPE_MIC,
    ESP_PHY_INIT_DATA_TYPE_IC,
    ESP_PHY_INIT_DATA_TYPE_ACMA,
    ESP_PHY_INIT_DATA_TYPE_ANATEL,
    ESP_PHY_INIT_DATA_TYPE_ISED,
    ESP_PHY_INIT_DATA_TYPE_WPC,
    ESP_PHY_INIT_DATA_TYPE_OFCA,
    ESP_PHY_INIT_DATA_TYPE_IFETEL,
    ESP_PHY_INIT_DATA_TYPE_RCM,
    ESP_PHY_INIT_DATA_TYPE_NUMBER,
} phy_init_data_type_t;
#endif

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
const esp_phy_init_data_t* esp_phy_get_init_data(void);

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
 * @brief Erase PHY calibration data which is stored in the NVS
 *
 * This is a function which can be used to trigger full calibration as a last-resort remedy
 * if partial calibration is used. It can be called in the application based on some conditions
 * (e.g. an option provided in some diagnostic mode).
 *
 * @return ESP_OK on success
 * @return others on fail. Please refer to NVS API return value error number.
 */
esp_err_t esp_phy_erase_cal_data_in_nvs(void);

/**
 * @brief Enable PHY and RF module
 *
 * PHY and RF module should be enabled in order to use WiFi or BT.
 * Now PHY and RF enabling job is done automatically when start WiFi or BT. Users should not
 * call this API in their application.
 *
 */
void esp_phy_enable(void);

/**
 * @brief Disable PHY and RF module
 *
 * PHY module should be disabled in order to shutdown WiFi or BT.
 * Now PHY and RF disabling job is done automatically when stop WiFi or BT. Users should not
 * call this API in their application.
 *
 */
void esp_phy_disable(void);

/**
 * @brief Load calibration data from NVS and initialize PHY and RF module
 */
void esp_phy_load_cal_and_init(void);

#if CONFIG_MAC_BB_PD
/**
 * @brief Initialize backup memory for MAC and Baseband power up/down
 */
void esp_mac_bb_pd_mem_init(void);

/**
 * @brief Power up MAC and Baseband
 */
void esp_mac_bb_power_up(void);

/**
 * @brief Power down MAC and Baseband
 */
void esp_mac_bb_power_down(void);
#endif

/**
 * @brief Enable WiFi/BT common clock
 *
 */
void esp_phy_common_clock_enable(void);

/**
 * @brief Disable WiFi/BT common clock
 *
 */
void esp_phy_common_clock_disable(void);

/**
 * @brief            Get the time stamp when PHY/RF was switched on
 * @return           return 0 if PHY/RF is never switched on. Otherwise return time in
 *                   microsecond since boot when phy/rf was last switched on
*/
int64_t esp_phy_rf_get_on_ts(void);

/**
 * @brief Update the corresponding PHY init type according to the country code of Wi-Fi.
 */
esp_err_t esp_phy_update_country_info(const char *country);


#if CONFIG_ESP_PHY_MULTIPLE_INIT_DATA_BIN
/**
 * @brief Apply PHY init bin to PHY
 * @return ESP_OK on success.
 * @return ESP_FAIL on fail.
 */
esp_err_t esp_phy_apply_phy_init_data(uint8_t *init_data);
#endif

/**
 * @brief Get PHY lib version
 * @return PHY lib version.
 */
char * get_phy_version_str(void);

#ifdef __cplusplus
}
#endif
