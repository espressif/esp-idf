/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sys/queue.h"
#include "esp_hidh_private.h"
#include "bt_hidh.h"
#include "ble_hidh.h"
#include <string.h>
#include <stdbool.h>
#include "esp_event_base.h"

ESP_EVENT_DEFINE_BASE(ESP_HIDH_EVENTS);

static const char *TAG = "ESP_HIDH";

static esp_hidh_dev_head_t s_esp_hidh_devices;

static xSemaphoreHandle s_esp_hidh_devices_semaphore = NULL;

static inline void lock_devices(void)
{
    if (s_esp_hidh_devices_semaphore != NULL) {
        xSemaphoreTake(s_esp_hidh_devices_semaphore, portMAX_DELAY);
    }
}

static inline void unlock_devices(void)
{
    if (s_esp_hidh_devices_semaphore != NULL) {
        xSemaphoreGive(s_esp_hidh_devices_semaphore);
    }
}

static bool esp_hidh_dev_exists(esp_hidh_dev_t *dev)
{
    if (dev == NULL) {
        return false;
    }
    esp_hidh_dev_t * d = NULL;
    lock_devices();
    TAILQ_FOREACH(d, &s_esp_hidh_devices, devices) {
        if (d == dev) {
            unlock_devices();
            return true;
        }
    }
    unlock_devices();
    return false;
}

/*
 * Public Functions
 * */

esp_err_t esp_hidh_init(const esp_hidh_config_t *config)
{
    esp_err_t err = ESP_FAIL;
    if (config == NULL) {
        ESP_LOGE(TAG, "Config is NULL");
        return err;
    }

    if (s_esp_hidh_devices_semaphore != NULL) {
        ESP_LOGE(TAG, "Already initialized");
        return err;
    }

    s_esp_hidh_devices_semaphore = xSemaphoreCreateBinary();
    if (s_esp_hidh_devices_semaphore == NULL) {
        ESP_LOGE(TAG, "xSemaphoreCreateMutex failed!");
        return err;
    }

    err = ESP_OK;

#if CONFIG_BT_HID_HOST_ENABLED
    if (err == ESP_OK) {
        err = esp_bt_hidh_init(config);
    }
#endif /* CONFIG_BT_HID_HOST_ENABLED */

#if CONFIG_GATTC_ENABLE
    if (err == ESP_OK) {
        err = esp_ble_hidh_init(config);
    }
#endif /* CONFIG_GATTC_ENABLE */

    if (err == ESP_OK) {
        TAILQ_INIT(&s_esp_hidh_devices);
        unlock_devices();
    } else {
        vSemaphoreDelete(s_esp_hidh_devices_semaphore);
        s_esp_hidh_devices_semaphore = NULL;
    }

    return err;
}

esp_err_t esp_hidh_deinit(void)
{
    esp_err_t err = ESP_FAIL;
    if (s_esp_hidh_devices_semaphore == NULL) {
        ESP_LOGE(TAG, "Already uninitialized");
        return err;
    }

    if (!TAILQ_EMPTY(&s_esp_hidh_devices)) {
        ESP_LOGE(TAG, "Please disconnect all devices first!");
        return err;
    }

    err = ESP_OK;

#if CONFIG_BT_HID_HOST_ENABLED
    if (err == ESP_OK) {
        err = esp_bt_hidh_deinit();
    }
#endif /* CONFIG_BT_HID_HOST_ENABLED */

#if CONFIG_GATTC_ENABLE
    if (err == ESP_OK) {
        err = esp_ble_hidh_deinit();
    }
#endif /* CONFIG_GATTC_ENABLE */

    if (err == ESP_OK) {
        TAILQ_INIT(&s_esp_hidh_devices);
        vSemaphoreDelete(s_esp_hidh_devices_semaphore);
        s_esp_hidh_devices_semaphore = NULL;
    }
    return err;
}

#if CONFIG_BLUEDROID_ENABLED
esp_hidh_dev_t *esp_hidh_dev_open(esp_bd_addr_t bda, esp_hid_transport_t transport, uint8_t remote_addr_type)
{
    if (esp_hidh_dev_get_by_bda(bda) != NULL) {
        ESP_LOGE(TAG, "Already Connected");
        return NULL;
    }
    esp_hidh_dev_t *dev = NULL;
#if CONFIG_GATTC_ENABLE
    if (transport == ESP_HID_TRANSPORT_BLE) {
        dev = esp_ble_hidh_dev_open(bda, (esp_ble_addr_type_t)remote_addr_type);
    }
#endif /* CONFIG_GATTC_ENABLE */
#if CONFIG_BT_HID_HOST_ENABLED
    if (transport == ESP_HID_TRANSPORT_BT) {
        dev = esp_bt_hidh_dev_open(bda);
    }
#endif /* CONFIG_BT_HID_HOST_ENABLED */
    return dev;
}
#endif /* CONFIG_BLUEDROID_ENABLED */

esp_err_t esp_hidh_dev_close(esp_hidh_dev_t *dev)
{
    if (!esp_hidh_dev_exists(dev)) {
        return ESP_FAIL;
    }
    return dev->close(dev);
}

void esp_hidh_dev_dump(esp_hidh_dev_t *dev, FILE *fp)
{
    if (!esp_hidh_dev_exists(dev)) {
        return;
    }
    dev->dump(dev, fp);
}

esp_err_t esp_hidh_dev_output_set(esp_hidh_dev_t *dev, size_t map_index, size_t report_id, uint8_t *value, size_t value_len)
{
    if (!esp_hidh_dev_exists(dev)) {
        return ESP_FAIL;
    }
    return dev->report_write(dev, map_index, report_id, ESP_HID_REPORT_TYPE_OUTPUT, value, value_len);
}

esp_err_t esp_hidh_dev_feature_set(esp_hidh_dev_t *dev, size_t map_index, size_t report_id, uint8_t *value, size_t value_len)
{
    if (!esp_hidh_dev_exists(dev)) {
        return ESP_FAIL;
    }
    return dev->report_write(dev, map_index, report_id, ESP_HID_REPORT_TYPE_FEATURE, value, value_len);
}

esp_err_t esp_hidh_dev_feature_get(esp_hidh_dev_t *dev, size_t map_index, size_t report_id, size_t max_length, uint8_t *value, size_t *value_len)
{
    if (!esp_hidh_dev_exists(dev)) {
        return ESP_FAIL;
    }
    return dev->report_read(dev, map_index, report_id, ESP_HID_REPORT_TYPE_FEATURE, max_length, value, value_len);
}

const uint8_t *esp_hidh_dev_bda_get(esp_hidh_dev_t *dev)
{
#if CONFIG_BLUEDROID_ENABLED
    if (esp_hidh_dev_exists(dev)) {
        return dev->bda;
    }
#endif /* CONFIG_BLUEDROID_ENABLED */
    return NULL;
}

esp_hid_transport_t esp_hidh_dev_transport_get(esp_hidh_dev_t *dev)
{
    if (!esp_hidh_dev_exists(dev)) {
        return ESP_HID_TRANSPORT_MAX;
    }
    return dev->transport;
}

const esp_hid_device_config_t *esp_hidh_dev_config_get(esp_hidh_dev_t *dev)
{
    if (!esp_hidh_dev_exists(dev)) {
        return NULL;
    }
    return &dev->config;
}

const char *esp_hidh_dev_name_get(esp_hidh_dev_t *dev)
{
    if (!esp_hidh_dev_exists(dev)) {
        return NULL;
    }
    return dev->config.device_name ? dev->config.device_name : "";
}

const char *esp_hidh_dev_manufacturer_get(esp_hidh_dev_t *dev)
{
    if (!esp_hidh_dev_exists(dev)) {
        return NULL;
    }
    return dev->config.manufacturer_name ? dev->config.manufacturer_name : "";
}

const char *esp_hidh_dev_serial_get(esp_hidh_dev_t *dev)
{
    if (!esp_hidh_dev_exists(dev)) {
        return NULL;
    }
    return dev->config.serial_number ? dev->config.serial_number : "";
}

uint16_t esp_hidh_dev_vendor_id_get(esp_hidh_dev_t *dev)
{
    if (!esp_hidh_dev_exists(dev)) {
        return 0;
    }
    return dev->config.vendor_id;
}

uint16_t esp_hidh_dev_product_id_get(esp_hidh_dev_t *dev)
{
    if (!esp_hidh_dev_exists(dev)) {
        return 0;
    }
    return dev->config.product_id;
}

uint16_t esp_hidh_dev_version_get(esp_hidh_dev_t *dev)
{
    if (!esp_hidh_dev_exists(dev)) {
        return 0;
    }
    return dev->config.version;
}

esp_hid_usage_t esp_hidh_dev_usage_get(esp_hidh_dev_t *dev)
{
    if (!esp_hidh_dev_exists(dev)) {
        return ESP_HID_USAGE_GENERIC;
    }
    return dev->usage;
}

esp_err_t esp_hidh_dev_reports_get(esp_hidh_dev_t *dev, size_t *num_reports, esp_hid_report_item_t **reports)
{
    if (!esp_hidh_dev_exists(dev)) {
        return ESP_FAIL;
    }

    esp_hid_report_item_t *r = (esp_hid_report_item_t *)malloc(sizeof(esp_hid_report_item_t) * dev->reports_len);
    if (r == NULL) {
        return ESP_FAIL;
    }

    esp_hidh_dev_report_t *dr = dev->reports;
    for (uint8_t i = 0; i < dev->reports_len; i++) {
        if (dr == NULL) {
            //error
            free(r);
            return ESP_FAIL;
        }
        r[i].map_index = dr->map_index;
        r[i].protocol_mode = dr->protocol_mode;
        r[i].usage = dr->usage;
        r[i].report_id = dr->report_id;
        r[i].report_type = dr->report_type;
        r[i].value_len = dr->value_len;

        dr = dr->next;
    }
    *reports = r;
    *num_reports = dev->reports_len;
    return ESP_OK;
}

esp_err_t esp_hidh_dev_report_maps_get(esp_hidh_dev_t *dev, size_t *num_maps, esp_hid_raw_report_map_t **maps)
{
    if (!esp_hidh_dev_exists(dev)) {
        return ESP_FAIL;
    }
    *num_maps = dev->config.report_maps_len;
    *maps = dev->config.report_maps;
    return ESP_OK;
}


/*
 * Private Functions
 * */

esp_hidh_dev_report_t *esp_hidh_dev_get_report_by_handle(esp_hidh_dev_t *dev, uint16_t handle)
{
    esp_hidh_dev_report_t *r = dev->reports;
    while (r) {
        if (r->handle == handle) {
            return r;
        }
        r = r->next;
    }
    return NULL;
}

esp_hidh_dev_report_t *esp_hidh_dev_get_report_by_id_and_type(esp_hidh_dev_t *dev, size_t map_index, size_t report_id, int report_type)
{
    esp_hidh_dev_report_t *r = dev->reports;
    while (r) {
        if (r->map_index == map_index && r->report_id == report_id && r->report_type == report_type && r->protocol_mode == ESP_HID_PROTOCOL_MODE_REPORT) {
            return r;
        }
        r = r->next;
    }
    return NULL;
}

esp_hidh_dev_report_t *esp_hidh_dev_get_input_report_by_id_and_proto(esp_hidh_dev_t *dev, size_t report_id, int protocol_mode)
{
    esp_hidh_dev_report_t *r = dev->reports;
    while (r) {
        if (r->report_id == report_id && (r->report_type & 1) && r->protocol_mode == protocol_mode) {
            return r;
        }
        r = r->next;
    }
    return NULL;
}

static void esp_hidh_dev_resources_free(esp_hidh_dev_t *dev)
{
    if (dev->semaphore) {
        vSemaphoreDelete(dev->semaphore);
    }
    free((void *)dev->config.device_name);
    free((void *)dev->config.manufacturer_name);
    free((void *)dev->config.serial_number);
    for (uint8_t d = 0; d < dev->config.report_maps_len; d++) {
        /* data of report map maybe is NULL */
        if (dev->config.report_maps[d].data) {
            free((void *)dev->config.report_maps[d].data);
        }
    }
    free((void *)dev->config.report_maps);
    esp_hidh_dev_report_t *r;
    while (dev->reports) {
        r = dev->reports;
        dev->reports = dev->reports->next;
        free(r);
    }
    free(dev);
}

esp_hidh_dev_t *esp_hidh_dev_malloc()
{
    esp_hidh_dev_t *dev = (esp_hidh_dev_t *)calloc(1, sizeof(esp_hidh_dev_t));
    if (dev == NULL) {
        ESP_LOGE(TAG, "malloc esp_hidh_dev_t failed");
        return NULL;
    }

    dev->semaphore = xSemaphoreCreateBinary();
    if (dev->semaphore == NULL) {
        ESP_LOGE(TAG, "malloc semaphore failed");
        esp_hidh_dev_resources_free(dev);
        return NULL;
    }

    lock_devices();
    TAILQ_INSERT_TAIL(&s_esp_hidh_devices, dev, devices);
    unlock_devices();

    return dev;
}

esp_err_t esp_hidh_dev_free(esp_hidh_dev_t *dev)
{
    esp_err_t ret = ESP_FAIL;

    if (dev == NULL) {
        return ret;
    }

    esp_hidh_dev_t *d = NULL;
    esp_hidh_dev_t *next = NULL;
    lock_devices();
    TAILQ_FOREACH_SAFE(d, &s_esp_hidh_devices, devices, next) {
        if (d == dev) {
            TAILQ_REMOVE(&s_esp_hidh_devices, d, devices);
            esp_hidh_dev_resources_free(d);
            ret = ESP_OK;
            break;
        }
    }
    unlock_devices();
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "device not found");
    } else {
        ESP_LOGD(TAG, "device removed");
    }
    return ret;
}

#if CONFIG_BLUEDROID_ENABLED
esp_hidh_dev_t *esp_hidh_dev_get_by_bda(esp_bd_addr_t bda)
{
    esp_hidh_dev_t * d = NULL;
    lock_devices();
    TAILQ_FOREACH(d, &s_esp_hidh_devices, devices) {
        if (memcmp(bda, d->bda, sizeof(esp_bd_addr_t)) == 0) {
            unlock_devices();
            return d;
        }
    }
    unlock_devices();
    return NULL;
}

esp_hidh_dev_t *esp_hidh_dev_get_by_handle(int handle)
{
#if CONFIG_BT_HID_HOST_ENABLED
    esp_hidh_dev_t * d = NULL;
    lock_devices();
    TAILQ_FOREACH(d, &s_esp_hidh_devices, devices) {
        if (d->transport == ESP_HID_TRANSPORT_BT && d->bt.handle == handle) {
            unlock_devices();
            return d;
        }
    }
    unlock_devices();
#endif /* CONFIG_BT_HID_HOST_ENABLED */
    return NULL;
}

esp_hidh_dev_t *esp_hidh_dev_get_by_conn_id(uint16_t conn_id)
{
#if CONFIG_GATTC_ENABLE
    esp_hidh_dev_t * d = NULL;
    lock_devices();
    TAILQ_FOREACH(d, &s_esp_hidh_devices, devices) {
        if (d->transport == ESP_HID_TRANSPORT_BLE && d->ble.conn_id == conn_id) {
            unlock_devices();
            return d;
        }
    }
    unlock_devices();
#endif /* CONFIG_GATTC_ENABLE */
    return NULL;
}
#endif /* CONFIG_BLUEDROID_ENABLED */
