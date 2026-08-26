/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_hid_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief HIDH structure forward declaration
 */
struct esp_hidh_dev_s;
typedef struct esp_hidh_dev_s esp_hidh_dev_t;

ESP_EVENT_DECLARE_BASE(ESP_HIDH_EVENTS);

/**
 * @brief HIDH callback events enum
 */
typedef enum {
    ESP_HIDH_ANY_EVENT = ESP_EVENT_ANY_ID,          /*!< HID device any event */
    ESP_HIDH_OPEN_EVENT = 0,                        /*!< HID device opened */
    ESP_HIDH_BATTERY_EVENT,                         /*!< HID device battery level changed */
    ESP_HIDH_INPUT_EVENT,                           /*!< Received HID device INPUT report */
    ESP_HIDH_FEATURE_EVENT,                         /*!< Received HID device FEATURE report */
    ESP_HIDH_CLOSE_EVENT,                           /*!< HID device closed */
    ESP_HIDH_START_EVENT,                           /*!< HID host stack started, used only for Classic Bluetooth */
    ESP_HIDH_STOP_EVENT,                            /*!< HID host stack stopped, used only for Classic Bluetooth */
    ESP_HIDH_CONN_REQUEST_EVENT,                    /*!< HID device requested connection, used only for USB */
    ESP_HIDH_MAX_EVENT,                             /*!< HID events end marker */
} esp_hidh_event_t;

/**
 * @brief HIDH callback parameters union
 */
typedef union {
    /**
     * @brief ESP_HIDH_START_EVENT
     * @note Used only for Classic Bluetooth.
     */
    struct {
        esp_err_t status;                           /*!< HID host operation status */
    } start;                                         /*!< HID callback param of ESP_HIDH_START_EVENT */

    /**
     * @brief ESP_HIDH_STOP_EVENT
     * @note Used only for Classic Bluetooth.
     */
    struct {
        esp_err_t status;                           /*!< HID host operation status */
    } stop;                                        /*!< HID callback param of ESP_HIDH_STOP_EVENT */

    /**
     * @brief ESP_HIDH_OPEN_EVENT
     */
    struct {
        esp_hidh_dev_t *dev;                        /*!< HID Remote bluetooth device */
        esp_err_t status;                           /*!< HID host operation status, used only for Classic Bluetooth */
    } open;                                         /*!< HID callback param of ESP_HIDH_OPEN_EVENT */

    /**
     * @brief ESP_HIDH_CLOSE_EVENT
     */
    struct {
        esp_hidh_dev_t *dev;                        /*!< HID Remote bluetooth device. */
        int reason;                                 /*!< Reason why the connection was closed. BLE Only */
        esp_err_t status;                           /*!< HID host operation status, used only for Classic Bluetooth */
    } close;                                        /*!< HID callback param of ESP_HIDH_CLOSE_EVENT */

    /**
     * @brief ESP_HIDH_BATTERY_EVENT
     */
    struct {
        esp_hidh_dev_t *dev;                        /*!< HID Remote bluetooth device */
        uint8_t level;                              /*!< Battery Level (0-100%) */
        esp_err_t status;                           /*!< HID host operation status */
    } battery;                                      /*!< HID callback param of ESP_HIDH_BATTERY_EVENT */

    /**
     * @brief ESP_HIDH_INPUT_EVENT
     */
    struct {
        esp_hidh_dev_t *dev;                        /*!< HID Remote bluetooth device */
        esp_hid_usage_t usage;                      /*!< HID report usage */
        uint16_t report_id;                         /*!< HID report index */
        uint16_t length;                            /*!< HID data length */
        uint8_t *data;                              /*!< The pointer to the HID data */
        uint8_t map_index;                          /*!< HID report map index */
    } input;                                        /*!< HID callback param of ESP_HIDH_INPUT_EVENT */

    /**
     * @brief ESP_HIDH_FEATURE_EVENT
     */
    struct {
        esp_hidh_dev_t *dev;                        /*!< HID Remote bluetooth device */
        esp_hid_usage_t usage;                      /*!< HID report usage */
        uint16_t report_id;                         /*!< HID report index */
        uint16_t length;                            /*!< HID data length */
        uint8_t *data;                              /*!< The pointer to the HID data */
        uint8_t map_index;                          /*!< HID report map index */
        esp_err_t status;                           /*!< HID host operation status, used only for Classic Bluetooth */
        esp_hid_trans_type_t trans_type;            /*!< HID host feature transaction type, used only for Classic Bluetooth */
    } feature;                                      /*!< HID callback param of ESP_HIDH_FEATURE_EVENT */

    /**
     * @brief ESP_HIDH_CONN_REQUEST_EVENT
     */
    struct {
        esp_hid_transport_t trans_type;        /*!< Transport type, currently only USB */
        esp_hid_address_t address;             /*!< Address of the device */
        const void *dev_info;                  /*!< Information about the device that requests connection. usb_intf_desc_t* for USB */
    } conn_request;

} esp_hidh_event_data_t;

typedef struct {
    esp_event_handler_t callback;
    uint16_t event_stack_size;
    void *callback_arg;
} esp_hidh_config_t;

/**
 * @brief Initialize the HID Host component
 * @param config           : pointer to esp_hidh_config_t structure
 *
 * @return: ESP_OK on success
 */
esp_err_t esp_hidh_init(const esp_hidh_config_t *config);

/**
 * @brief De-initialize the HID Host component
 *
 * @return: ESP_OK on success
 */
esp_err_t esp_hidh_deinit(void);

/**
 * @brief Close HID Device
 * @param dev : pointer to the device
 *
 * @return: ESP_OK on success
 */
esp_err_t esp_hidh_dev_close(esp_hidh_dev_t *dev);

/**
 * @brief Free HID Device Memory
 *        This function MUST be called when handling ESP_HIDH_CLOSE_EVENT
 *        Only then all memory used for the device will be freed.
 * @param dev : pointer to the device
 *
 * @return: ESP_OK on success
 */
esp_err_t esp_hidh_dev_free(esp_hidh_dev_t *dev);

/**
 * @brief Check if the device still exists.
 * @param dev : pointer to the device
 *
 * @return: true if exists
 */
bool esp_hidh_dev_exists(esp_hidh_dev_t *dev);

/**
 * @brief Send an OUTPUT report to the device
 * @param dev       : pointer to the device
 * @param map_index : index of the device report map
 * @param report_id : id of the HID OUTPUT report
 * @param data      : pointer to the data to send
 * @param length    : length of the data to send
 *
 * @return: ESP_OK on success
 */
esp_err_t esp_hidh_dev_output_set(esp_hidh_dev_t *dev, size_t map_index, size_t report_id, uint8_t *data, size_t length);

/**
 * @brief Send a FEATURE report to the device
 * @param dev       : pointer to the device
 * @param map_index : index of the device report map
 * @param report_id : id of the HID FEATURE report
 * @param data      : pointer to the data to send
 * @param length    : length of the data to send
 *
 * @return: ESP_OK on success
 */
esp_err_t esp_hidh_dev_feature_set(esp_hidh_dev_t *dev, size_t map_index, size_t report_id, uint8_t *data, size_t length);

/**
 * @brief Get the value a FEATURE report from the device
 * @param dev       : pointer to the device
 * @param map_index : index of the device report map
 * @param report_id : id of the HID FEATURE report
 * @param max_len   : size of the buffer that will hold the data
 * @param data      : pointer to the data buffer
 * @param length    : pointer to the value that will be set to the number of bytes received
 *
 * @return: ESP_OK on success
 */
esp_err_t esp_hidh_dev_feature_get(esp_hidh_dev_t *dev, size_t map_index, size_t report_id, size_t max_len, uint8_t *data, size_t *length);

/**
 * @brief Set_Report command.
 * @note For now, this function used only for Classic Bluetooth.
 *
 * @param dev           : pointer to the device
 * @param map_index     : index of the device report map
 * @param report_id     : id of the HID FEATURE report
 * @param report_type   : report type, defines in `esp_hid_common.h`
 * @param data          : pointer to the data to send
 * @param length        : length of the data to send
 *
 * @return: ESP_OK on success
 */
esp_err_t esp_hidh_dev_set_report(esp_hidh_dev_t *dev, size_t map_index, size_t report_id, int report_type,
                                  uint8_t *data, size_t length);

/**
 * @brief Get_Report command.
 * @note For now, this function used only for Classic Bluetooth.
 *
 * @param dev           : pointer to the device
 * @param map_index     : index of the device report map
 * @param report_id     : id of the HID FEATURE report
 * @param report_type   : report type, defines in `esp_hid_common.h`
 * @param max_len       : size of the buffer that will hold the data
 *
 * @return: ESP_OK on success
 */
esp_err_t esp_hidh_dev_get_report(esp_hidh_dev_t *dev, size_t map_index, size_t report_id, int report_type,
                                  size_t max_len);

/**
 * @brief Get_Idle Command.
 * @note For now, this function used only for Classic Bluetooth.
 *
 * @param dev               : pointer to the device
 *
 * @return: ESP_OK on success
 */
esp_err_t esp_hidh_dev_get_idle(esp_hidh_dev_t *dev);

/**
 * @brief Set_Idle Command.
 * @note For now, this function used only for Classic Bluetooth.
 *
 * @param dev           : pointer to the device
 * @param idle_time     : idle_time
 *
 * @return: ESP_OK on success
 */
esp_err_t esp_hidh_dev_set_idle(esp_hidh_dev_t *dev, uint8_t idle_time);

/**
 * @brief Get_Protocol Command.
 * @note For now, this function used only for Classic Bluetooth.
 *
 * @param dev               : pointer to the device
 *
 * @return: ESP_OK on success
 */
esp_err_t esp_hidh_dev_get_protocol(esp_hidh_dev_t *dev);

/**
 * @brief Set_Protocol Command.
 * @note For now, this function used only for Classic Bluetooth.
 *
 * @param dev           : pointer to the device
 * @param protocol_mode : protocol_mode
 *
 * @return: ESP_OK on success
 */
esp_err_t esp_hidh_dev_set_protocol(esp_hidh_dev_t *dev, uint8_t protocol_mode);

/**
 * @brief Dump the properties of HID Device to UART
 * @param dev : pointer to the HID Device
 * @param fp  : pointer to the output file
 */
void esp_hidh_dev_dump(esp_hidh_dev_t *dev, FILE *fp);

/**
 * @brief Get the BT Device Address of a HID Device
 * @param dev : pointer to the HID Device
 *
 * @return: pointer to the BDA byte array or NULL
 */
const uint8_t *esp_hidh_dev_bda_get(esp_hidh_dev_t *dev);

/**
 * @brief Get the HID Device Transport
 * @param dev : pointer to the HID Device
 *
 * @return: the transport of the connected device or ESP_HID_TRANSPORT_MAX
 */
esp_hid_transport_t esp_hidh_dev_transport_get(esp_hidh_dev_t *dev);

/**
 * @brief Get the HID Device Cofiguration
 * @param dev : pointer to the HID Device
 *
 * @return: pointer to the config structure or NULL
 */
const esp_hid_device_config_t *esp_hidh_dev_config_get(esp_hidh_dev_t *dev);

/**
 * @brief Get the name of a HID Device
 * @param dev : pointer to the HID Device
 *
 * @return: pointer to the character array or NULL
 */
const char *esp_hidh_dev_name_get(esp_hidh_dev_t *dev);

/**
 * @brief Get the manufacturer of a HID Device
 * @param dev : pointer to the HID Device
 *
 * @return: pointer to the character array
 */
const char *esp_hidh_dev_manufacturer_get(esp_hidh_dev_t *dev);

/**
 * @brief Get the serial number of a HID Device
 * @param dev : pointer to the HID Device
 *
 * @return: pointer to the character array or NULL
 */
const char *esp_hidh_dev_serial_get(esp_hidh_dev_t *dev);

/**
 * @brief Get the VID of a HID Device
 * @param dev : pointer to the HID Device
 *
 * @return: the VID value
 */
uint16_t esp_hidh_dev_vendor_id_get(esp_hidh_dev_t *dev);

/**
 * @brief Get the PID of a HID Device
 * @param dev : pointer to the HID Device
 *
 * @return: the PID value
 */
uint16_t esp_hidh_dev_product_id_get(esp_hidh_dev_t *dev);

/**
 * @brief Get the version HID Device
 * @param dev : pointer to the HID Device
 *
 * @return: the version value
 */
uint16_t esp_hidh_dev_version_get(esp_hidh_dev_t *dev);

/**
 * @brief Get the appearance of BLE HID Device
 * @param dev : pointer to the BLE HID Device
 *
 * @return: the appearance value
 */
uint16_t esp_hidh_dev_appearance_get(esp_hidh_dev_t *dev); //BLE Only

/**
 * @brief Get the calculated HID Device usage type
 * @param dev : pointer to the HID Device
 *
 * @return: the hid usage type
 */
esp_hid_usage_t esp_hidh_dev_usage_get(esp_hidh_dev_t *dev);

/**
 * @brief Get an array of all reports found on a device
 * @param dev           : pointer to the device
 * @param num_reports   : pointer to the value that will be set to the number of reports
 * @param reports       : location to set to the pointer of the reports array
 *
 * @return: ESP_OK on success
 */
esp_err_t esp_hidh_dev_reports_get(esp_hidh_dev_t *dev, size_t *num_reports, esp_hid_report_item_t **reports);

/**
 * @brief Get an array of the report maps found on a device
 * @param dev        : pointer to the device
 * @param num_maps   : pointer to the value that will be set to the number of report maps found
 * @param maps       : location to set to the pointer of the report maps array
 *
 * @return: ESP_OK on success
 */
esp_err_t esp_hidh_dev_report_maps_get(esp_hidh_dev_t *dev, size_t *num_maps, esp_hid_raw_report_map_t **maps);

#include "esp_hidh_transport.h"

#ifdef __cplusplus
}
#endif
