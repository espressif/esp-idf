// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_hid_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_hidd_transport.h"

ESP_EVENT_DECLARE_BASE(ESP_HIDD_EVENTS);        // Declare the event base for HID device

/**
 * @brief HIDD callback events enum
 */
typedef enum {
    ESP_HIDD_ANY_EVENT = ESP_EVENT_ANY_ID,      /*!< HID device any event */
    ESP_HIDD_START_EVENT = 0,                   /*!< HID device stack started */
    ESP_HIDD_CONNECT_EVENT,                     /*!< HID device connected */
    ESP_HIDD_PROTOCOL_MODE_EVENT,               /*!< HID device protocol mode change */
    ESP_HIDD_CONTROL_EVENT,                     /*!< HID device control request */
    ESP_HIDD_OUTPUT_EVENT,                      /*!< HID device output report event */
    ESP_HIDD_FEATURE_EVENT,                     /*!< HID device feature report event */
    ESP_HIDD_DISCONNECT_EVENT,                  /*!< HID device disconnected */
    ESP_HIDD_STOP_EVENT,                        /*!< HID device stack stopped */
    ESP_HIDD_MAX_EVENT,                         /*!< HID events end marker */
} esp_hidd_event_t;

/**
 * @brief HIDD structure forward declaration
 */
struct esp_hidd_dev_s;
typedef struct esp_hidd_dev_s esp_hidd_dev_t;

/**
 * @brief HIDD callback parameters union
 */
typedef union {
    /**
     * @brief ESP_HIDD_CONNECT_EVENT
     */
    struct {
        esp_hidd_dev_t *dev;                    /*!< HID device structure */
    } connect;                                  /*!< HID callback param of ESP_HIDD_CONNECT_EVENT */

    /**
     * @brief ESP_HIDD_DISCONNECT_EVENT
     */
    struct {
        esp_hidd_dev_t *dev;                    /*!< HID device structure */
        int reason;                             /*!< Indicate the reason of disconnection */
    } disconnect;                               /*!< HID callback param of ESP_HIDD_DISCONNECT_EVENT */

    /**
     * @brief ESP_HIDD_OUTPUT_EVENT
     */
    struct {
        esp_hidd_dev_t *dev;                    /*!< HID device structure */
        esp_hid_usage_t usage;                  /*!< HID report usage */
        uint16_t report_id;                     /*!< HID report index */
        uint16_t length;                        /*!< data length */
        uint8_t  *data;                         /*!< The pointer to the data */
        uint8_t map_index;                      /*!< HID config report map index */
    } output;                                   /*!< HID callback param of ESP_HIDD_OUTPUT_EVENT */

    /**
     * @brief ESP_HIDD_FEATURE_EVENT
     */
    struct {
        esp_hidd_dev_t *dev;                    /*!< HID device structure */
        esp_hid_usage_t usage;                  /*!< HID report usage */
        uint16_t report_id;                     /*!< HID report index */
        uint16_t length;                        /*!< data length */
        uint8_t  *data;                         /*!< The pointer to the data */
        uint8_t map_index;                      /*!< HID config report map index */
    } feature;                                  /*!< HID callback param of ESP_HIDD_FEATURE_EVENT */

    /**
     * @brief ESP_HIDD_PROTOCOL_MODE_EVENT
     */
    struct {
        esp_hidd_dev_t *dev;                    /*!< HID device structure */
        uint8_t protocol_mode;                  /*!< HID Protocol Mode */
        uint8_t map_index;                      /*!< HID config report map index */
    } protocol_mode;                            /*!< HID callback param of ESP_HIDD_PROTOCOL_MODE_EVENT */

    /**
     * @brief ESP_HIDD_CONTROL_EVENT
     */
    struct {
        esp_hidd_dev_t *dev;                    /*!< HID device structure */
        uint8_t control;                        /*!< HID Control Point */
        uint8_t map_index;                      /*!< HID config report map index */
    } control;                                  /*!< HID callback param of ESP_HIDD_CONTROL_EVENT */

} esp_hidd_event_data_t;

/**
 * @brief Init HID Device
 * @param       config   : configuration for the device
 * @param       transport: protocol that the device will use (ESP_HID_TRANSPORT_BLE/BT/USB)
 * @param       callback : function to call when events for this device are generated.
 *                         Can be NULL, but will miss the START event.
 * @param[out]  dev      : location to return the pointer to the device structure
 *
 * @return: ESP_OK on success
 */
esp_err_t esp_hidd_dev_init(const esp_hid_device_config_t *config, esp_hid_transport_t transport, esp_event_handler_t callback, esp_hidd_dev_t **dev);

/**
 * @brief Deinit HID Device
 * @param dev : pointer to the device to deinit
 *
 * @return: ESP_OK on success
 */
esp_err_t esp_hidd_dev_deinit(esp_hidd_dev_t *dev);

/**
 * @brief Get the HID Device Transport
 * @param dev : pointer to the HID Device
 *
 * @return: the transport of the connected device or ESP_HID_TRANSPORT_MAX
 */
esp_hid_transport_t esp_hidd_dev_transport_get(esp_hidd_dev_t *dev);

/**
 * @brief Check if HID Device is connected
 * @param dev : pointer to the device
 *
 * @return: true if the device is connected
 */
bool esp_hidd_dev_connected(esp_hidd_dev_t *dev);

/**
 * @brief Set the battery level reported by the HID Device
 * @param dev   : pointer to the device
 * @param level : battery level (0-100)
 *
 * @return: ESP_OK on success
 */
esp_err_t esp_hidd_dev_battery_set(esp_hidd_dev_t *dev, uint8_t level);

/**
 * @brief Send an INPUT report to the host
 * @param dev       : pointer to the device
 * @param map_index : index of the device report map in the init config
 * @param report_id : id of the HID INPUT report
 * @param data      : pointer to the data to send
 * @param length    : length of the data to send
 *
 * @return: ESP_OK on success
 */
esp_err_t esp_hidd_dev_input_set(esp_hidd_dev_t *dev, size_t map_index, size_t report_id, uint8_t *data, size_t length);

/**
 * @brief Send a FEATURE report to the host
 * @param dev       : pointer to the device
 * @param map_index : index of the device report map in the init config
 * @param report_id : id of the HID FEATURE report
 * @param data      : pointer to the data to send
 * @param length    : length of the data to send
 *
 * @return: ESP_OK on success
 */
esp_err_t esp_hidd_dev_feature_set(esp_hidd_dev_t *dev, size_t map_index, size_t report_id, uint8_t *data, size_t length);

/**
 * @brief Register function to listen for device events
 * @param dev       : pointer to the device
 * @param callback  : event handler function
 * @param event     : event to listen for (ESP_HIDD_ANY_EVENT for all)
 *
 * @return: ESP_OK on success
 */
esp_err_t esp_hidd_dev_event_handler_register(esp_hidd_dev_t *dev, esp_event_handler_t callback, esp_hidd_event_t event);

/**
 * @brief Unregister function that is listening for device events
 * @param dev       : pointer to the device
 * @param callback  : event handler function
 * @param event     : event that is listening for (ESP_HIDD_ANY_EVENT for all)
 *
 * @return: ESP_OK on success
 */
esp_err_t esp_hidd_dev_event_handler_unregister(esp_hidd_dev_t *dev, esp_event_handler_t callback, esp_hidd_event_t event);

#ifdef __cplusplus
}
#endif
