/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_hidd.h"
#include "esp_hidd_private.h"
#include "esp_event_base.h"

#if CONFIG_GATTS_ENABLE || CONFIG_BT_NIMBLE_ENABLED
#include "ble_hidd.h"
#endif /* CONFIG_GATTS_ENABLE */

#if CONFIG_BT_HID_DEVICE_ENABLED
#include "bt_hidd.h"
#endif /* CONFIG_BT_HID_DEVICE_ENABLED */

ESP_EVENT_DEFINE_BASE(ESP_HIDD_EVENTS);

esp_err_t esp_hidd_dev_init(const esp_hid_device_config_t *config, esp_hid_transport_t transport, esp_event_handler_t callback, esp_hidd_dev_t **dev_out)
{
    esp_err_t ret = ESP_OK;
    esp_hidd_dev_t *dev = (esp_hidd_dev_t *)calloc(1, sizeof(esp_hidd_dev_t));
    if (dev == NULL) {
        return ESP_FAIL;
    }

    switch (transport) {
#if CONFIG_GATTS_ENABLE || CONFIG_BT_NIMBLE_ENABLED
    case ESP_HID_TRANSPORT_BLE:
        ret = esp_ble_hidd_dev_init(dev, config, callback);
        break;
#endif /* CONFIG_GATTS_ENABLE */
#if CONFIG_BT_HID_DEVICE_ENABLED
    case ESP_HID_TRANSPORT_BT:
        ret = esp_bt_hidd_dev_init(dev, config, callback);
        break;
#endif /* CONFIG_BT_HID_DEVICE_ENABLED */
    default:
        ret = ESP_FAIL;
        break;
    }

    if (ret != ESP_OK) {
        free(dev);
        return ret;
    }
    dev->transport = transport;
    *dev_out = dev;
    return ret;
}

esp_err_t esp_hidd_dev_deinit(esp_hidd_dev_t *dev)
{
    if (dev == NULL) {
        return ESP_FAIL;
    }
    esp_err_t ret = dev->deinit(dev->dev);
    if (ret != ESP_OK) {
        return ret;
    }
    free(dev);
    return ret;
}

esp_hid_transport_t esp_hidd_dev_transport_get(esp_hidd_dev_t *dev)
{
    if (dev == NULL) {
        return ESP_HID_TRANSPORT_MAX;
    }
    return dev->transport;
}

bool esp_hidd_dev_connected(esp_hidd_dev_t *dev)
{
    if (dev == NULL) {
        return false;
    }
    return dev->connected(dev->dev);
}

esp_err_t esp_hidd_dev_battery_set(esp_hidd_dev_t *dev, uint8_t level)
{
    if (dev == NULL) {
        return ESP_FAIL;
    }
    return dev->battery_set(dev->dev, level);
}

esp_err_t esp_hidd_dev_input_set(esp_hidd_dev_t *dev, size_t map_index, size_t report_id, uint8_t *data, size_t length)
{
    if (dev == NULL) {
        return ESP_FAIL;
    }
    return dev->input_set(dev->dev, map_index, report_id, data, length);
}

esp_err_t esp_hidd_dev_feature_set(esp_hidd_dev_t *dev, size_t map_index, size_t report_id, uint8_t *data, size_t length)
{
    if (dev == NULL) {
        return ESP_FAIL;
    }
    return dev->feature_set(dev->dev, map_index, report_id, data, length);
}

esp_err_t esp_hidd_dev_event_handler_register(esp_hidd_dev_t *dev, esp_event_handler_t callback, esp_hidd_event_t event)
{
    if (dev == NULL) {
        return ESP_FAIL;
    }
    return dev->event_handler_register(dev->dev, callback, event);
}

esp_err_t esp_hidd_dev_event_handler_unregister(esp_hidd_dev_t *dev, esp_event_handler_t callback, esp_hidd_event_t event)
{
    if (dev == NULL) {
        return ESP_FAIL;
    }
    return dev->event_handler_unregister(dev->dev, callback, event);
}

/**
 * The deep copy data append the end of the esp_hidd_event_data_t, move the data pointer to the correct address. This is
 * a workaround way, it's better to use flexiable array in the interface.
 */
void esp_hidd_process_event_data_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id,
                                         void *event_data)
{
    esp_hidd_event_t event = (esp_hidd_event_t)event_id;
    esp_hidd_event_data_t *param = (esp_hidd_event_data_t *)event_data;

    switch (event) {
    case ESP_HIDD_OUTPUT_EVENT:
        if (param->output.length && param->output.data) {
            param->output.data = (uint8_t *)param + sizeof(esp_hidd_event_data_t);
        }
        break;
    case ESP_HIDD_FEATURE_EVENT:
        if (param->feature.length && param->feature.data) {
            param->feature.data = (uint8_t *)param + sizeof(esp_hidd_event_data_t);
        }
        break;
    default:
        break;
    }
}
