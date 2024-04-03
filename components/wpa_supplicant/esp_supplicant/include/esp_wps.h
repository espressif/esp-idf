/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_WPS_H__
#define __ESP_WPS_H__

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_wifi_crypto_types.h"
#include "esp_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup WiFi_APIs WiFi Related APIs
  * @brief WiFi APIs
  */

/** @addtogroup WiFi_APIs
  * @{
  */

/** @addtogroup WPS_APIs
  * @{
  */

#define ESP_ERR_WIFI_REGISTRAR   (ESP_ERR_WIFI_BASE + 51)  /*!< WPS registrar is not supported */
#define ESP_ERR_WIFI_WPS_TYPE    (ESP_ERR_WIFI_BASE + 52)  /*!< WPS type error */
#define ESP_ERR_WIFI_WPS_SM      (ESP_ERR_WIFI_BASE + 53)  /*!< WPS state machine is not initialized */

/**
 * @brief Enumeration of WPS (Wi-Fi Protected Setup) types.
 */
typedef enum wps_type {
    WPS_TYPE_DISABLE = 0,   /**< WPS is disabled */
    WPS_TYPE_PBC,           /**< WPS Push Button Configuration method */
    WPS_TYPE_PIN,           /**< WPS PIN (Personal Identification Number) method */
    WPS_TYPE_MAX            /**< Maximum value for WPS type enumeration */
} wps_type_t;

#define WPS_MAX_MANUFACTURER_LEN 65  /**< Maximum length of the manufacturer name in WPS information */
#define WPS_MAX_MODEL_NUMBER_LEN 33  /**< Maximum length of the model number in WPS information */
#define WPS_MAX_MODEL_NAME_LEN   33  /**< Maximum length of the model name in WPS information */
#define WPS_MAX_DEVICE_NAME_LEN  33  /**< Maximum length of the device name in WPS information */

/**
 * @brief Structure representing WPS factory information for ESP device.
 *
 * This structure holds various strings representing factory information for a device, such as the manufacturer,
 * model number, model name, and device name. Each string is a null-terminated character array. If any of the
 * strings are empty, the default values are used.
 */
typedef struct {
    char manufacturer[WPS_MAX_MANUFACTURER_LEN]; /*!< Manufacturer of the device. If empty, the default manufacturer is used. */
    char model_number[WPS_MAX_MODEL_NUMBER_LEN]; /*!< Model number of the device. If empty, the default model number is used. */
    char model_name[WPS_MAX_MODEL_NAME_LEN];     /*!< Model name of the device. If empty, the default model name is used. */
    char device_name[WPS_MAX_DEVICE_NAME_LEN];   /*!< Device name. If empty, the default device name is used. */
} wps_factory_information_t;

#define PIN_LEN 9 /*!< The length of the WPS PIN (Personal Identification Number). */
/**
 * @brief Structure representing configuration settings for WPS (Wi-Fi Protected Setup).
 *
 * This structure encapsulates various configuration settings for WPS, including the WPS type (PBC or PIN),
 * factory information that will be shown in the WPS Information Element (IE), and a PIN if the WPS type is
 * set to PIN.
 */
typedef struct {
    wps_type_t wps_type;  /*!< The type of WPS to be used (PBC or PIN). */
    wps_factory_information_t factory_info; /*!< Factory information to be shown in the WPS Information Element (IE). Vendor can choose to display their own information. */
    char pin[PIN_LEN];   /*!< WPS PIN (Personal Identification Number) used when wps_type is set to WPS_TYPE_PIN. */
} esp_wps_config_t;

/**
 * @def WPS_CONFIG_INIT_DEFAULT(type)
 * @brief Initialize a default WPS configuration structure with specified WPS type.
 *
 * This macro initializes a `esp_wps_config_t` structure with default values for the specified WPS type.
 * It sets the WPS type, factory information (including default manufacturer, model number, model name, and device name),
 * and a default PIN value if applicable.
 *
 * @param type The WPS type to be used (PBC or PIN).
 * @return An initialized `esp_wps_config_t` structure with the specified WPS type and default values.
 */
#define WPS_CONFIG_INIT_DEFAULT(type) { \
    .wps_type = type, \
    .factory_info = {   \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_STR(manufacturer, "ESPRESSIF")  \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_STR(model_number, CONFIG_IDF_TARGET)  \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_STR(model_name, "ESPRESSIF IOT")  \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_STR(device_name, "ESP DEVICE")  \
    },  \
    ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_STR(pin, "00000000") \
}

/**
  * @brief     Enable Wi-Fi WPS function.
  *
  * @param     config : WPS config to be used in connection
  *
  * @return
  *          - ESP_OK : succeed
  *          - ESP_ERR_WIFI_WPS_TYPE : wps type is invalid
  *          - ESP_ERR_WIFI_WPS_MODE : wifi is not in station mode or sniffer mode is on
  *          - ESP_FAIL : wps initialization fails
  */
esp_err_t esp_wifi_wps_enable(const esp_wps_config_t *config);

/**
  * @brief  Disable Wi-Fi WPS function and release resource it taken.
  *
  * @return
  *          - ESP_OK : succeed
  *          - ESP_ERR_WIFI_WPS_MODE : wifi is not in station mode or sniffer mode is on
  */
esp_err_t esp_wifi_wps_disable(void);

/**
  * @brief     Start WPS session.
  *
  * @attention WPS can only be used when station is enabled. WPS needs to be enabled first for using this API.
  *
  * @param     timeout_ms : deprecated: This argument's value will have not effect in functionality of API.
  *                         The argument will be removed in future.
  *                         The app should start WPS and register for WIFI events to get the status.
  *                         WPS status is updated through WPS events. See wifi_event_t enum for more info.
  *
  * @return
  *          - ESP_OK : succeed
  *          - ESP_ERR_WIFI_WPS_TYPE : wps type is invalid
  *          - ESP_ERR_WIFI_WPS_MODE : wifi is not in station mode or sniffer mode is on
  *          - ESP_ERR_WIFI_WPS_SM : wps state machine is not initialized
  *          - ESP_FAIL : wps initialization fails
  */
esp_err_t esp_wifi_wps_start(int timeout_ms);

/**
  * @brief     Enable Wi-Fi AP WPS function.
  *
  * @attention WPS can only be used when softAP is enabled.
  *
  * @param     config: wps configuration to be used.
  *
  * @return
  *          - ESP_OK : succeed
  *          - ESP_ERR_WIFI_WPS_TYPE : wps type is invalid
  *          - ESP_ERR_WIFI_WPS_MODE : wifi is not in station mode or sniffer mode is on
  *          - ESP_FAIL : wps initialization fails
  */
esp_err_t esp_wifi_ap_wps_enable(const esp_wps_config_t *config);

/**
  * @brief  Disable Wi-Fi SoftAP WPS function and release resource it taken.
  *
  * @return
  *          - ESP_OK : succeed
  *          - ESP_ERR_WIFI_WPS_MODE : wifi is not in station mode or sniffer mode is on
  */
esp_err_t esp_wifi_ap_wps_disable(void);

/**
  * @brief     WPS starts to work.
  *
  * @attention WPS can only be used when softAP is enabled.
  *
  * @param  pin : Pin to be used in case of WPS mode is pin.
  *               If Pin is not provided, device will use the pin generated/provided
  *               during esp_wifi_ap_wps_enable() and reported in WIFI_EVENT_AP_WPS_RG_PIN
  *
  * @return
  *          - ESP_OK : succeed
  *          - ESP_ERR_WIFI_WPS_TYPE : wps type is invalid
  *          - ESP_ERR_WIFI_WPS_MODE : wifi is not in station mode or sniffer mode is on
  *          - ESP_ERR_WIFI_WPS_SM : wps state machine is not initialized
  *          - ESP_FAIL : wps initialization fails
  */
esp_err_t esp_wifi_ap_wps_start(const unsigned char *pin);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __ESP_WPS_H__ */
