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

#include "sys/queue.h"
#include "esp_hidh_private.h"
#include "bt_hidh.h"
#include "ble_hidh.h"
#include <string.h>
#include <stdbool.h>
#include "esp_event_base.h"

ESP_EVENT_DEFINE_BASE(ESP_HIDH_EVENTS);
#define ESP_HIDH_DELAY_FREE_TO 100000 // us

static const char *TAG = "ESP_HIDH";

static esp_hidh_dev_head_t s_esp_hidh_devices;
static esp_timer_handle_t s_esp_hidh_timer;
static xSemaphoreHandle s_esp_hidh_devices_semaphore = NULL;

static void esp_hidh_dev_delay_free(void *arg);

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


/*
 * Public Functions
 * */

bool esp_hidh_dev_exists(esp_hidh_dev_t *dev)
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

    TAILQ_INIT(&s_esp_hidh_devices);

    esp_timer_create_args_t timer_config = {
        .callback = &esp_hidh_dev_delay_free,
        .arg = NULL,
        .name = "hidh_timer"
    };

    if ((err = esp_timer_create(&timer_config, &s_esp_hidh_timer)) != ESP_OK) {
        ESP_LOGE(TAG, "%s create timer failed!", __func__);
        return err;
    }

    s_esp_hidh_devices_semaphore = xSemaphoreCreateMutex();
    if (s_esp_hidh_devices_semaphore == NULL) {
        ESP_LOGE(TAG, "xSemaphoreCreateMutex failed!");
        return err;
    }
    // unlock_devices();
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

    if (err != ESP_OK) {
        vSemaphoreDelete(s_esp_hidh_devices_semaphore);
        s_esp_hidh_devices_semaphore = NULL;
        esp_timer_delete(s_esp_hidh_timer);
        s_esp_hidh_timer = NULL;
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

    if (esp_timer_is_active(s_esp_hidh_timer)) {
        ESP_LOGE(TAG, "Busy, try again later!");
        return ESP_ERR_NOT_FINISHED;
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
        esp_timer_delete(s_esp_hidh_timer);
        s_esp_hidh_timer = NULL;
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
    esp_err_t ret = ESP_OK;
    if (esp_hidh_dev_exists(dev)) {
        esp_hidh_dev_lock(dev);
        ret = dev->close(dev);
        esp_hidh_dev_unlock(dev);
    } else {
        ret = ESP_FAIL;
    }
    return ret;
}

void esp_hidh_dev_dump(esp_hidh_dev_t *dev, FILE *fp)
{
    if (esp_hidh_dev_exists(dev)) {
        esp_hidh_dev_lock(dev);
        dev->dump(dev, fp);
        esp_hidh_dev_unlock(dev);
    }
}

esp_err_t esp_hidh_dev_output_set(esp_hidh_dev_t *dev, size_t map_index, size_t report_id, uint8_t *value, size_t value_len)
{
    esp_err_t ret = ESP_OK;
    if (esp_hidh_dev_exists(dev)) {
        esp_hidh_dev_lock(dev);
        ret = dev->report_write(dev, map_index, report_id, ESP_HID_REPORT_TYPE_OUTPUT, value, value_len);
        esp_hidh_dev_unlock(dev);
    } else {
        ret = ESP_FAIL;
    }
    return ret;
}

esp_err_t esp_hidh_dev_feature_set(esp_hidh_dev_t *dev, size_t map_index, size_t report_id, uint8_t *value, size_t value_len)
{
    esp_err_t ret = ESP_OK;
    if (esp_hidh_dev_exists(dev)) {
        esp_hidh_dev_lock(dev);
        ret = dev->report_write(dev, map_index, report_id, ESP_HID_REPORT_TYPE_FEATURE, value, value_len);
        esp_hidh_dev_unlock(dev);
    } else {
        ret = ESP_FAIL;
    }
    return ret;
}

esp_err_t esp_hidh_dev_feature_get(esp_hidh_dev_t *dev, size_t map_index, size_t report_id, size_t max_length, uint8_t *value, size_t *value_len)
{
    esp_err_t ret = ESP_OK;
    if (esp_hidh_dev_exists(dev)) {
        esp_hidh_dev_lock(dev);
        ret = dev->report_read(dev, map_index, report_id, ESP_HID_REPORT_TYPE_FEATURE, max_length, value, value_len);
        esp_hidh_dev_unlock(dev);
    } else {
        ret = ESP_FAIL;
    }
    return ret;
}

esp_err_t esp_hidh_dev_set_report(esp_hidh_dev_t *dev, size_t map_index, size_t report_id, int report_type, uint8_t *data, size_t length)
{
    esp_err_t ret = ESP_OK;
    if (esp_hidh_dev_exists(dev)) {
        esp_hidh_dev_lock(dev);
        if (dev->set_report) {
            ret = dev->set_report(dev, map_index, report_id, report_type, data, length);
        } else {
            ret = ESP_ERR_NOT_SUPPORTED;
        }
        esp_hidh_dev_unlock(dev);
    } else {
        ret = ESP_FAIL;
    }
    return ret;
}

esp_err_t esp_hidh_dev_get_report(esp_hidh_dev_t *dev, size_t map_index, size_t report_id, int report_type,
                                  size_t max_len)
{
    esp_err_t ret = ESP_OK;
    if (esp_hidh_dev_exists(dev)) {
        esp_hidh_dev_lock(dev);
        ret = dev->report_read(dev, map_index, report_id, report_type, max_len, NULL, NULL);
        esp_hidh_dev_unlock(dev);
    } else {
        ret = ESP_FAIL;
    }
    return ret;
}

esp_err_t esp_hidh_dev_get_idle(esp_hidh_dev_t *dev)
{
    esp_err_t ret = ESP_OK;
    if (esp_hidh_dev_exists(dev)) {
        esp_hidh_dev_lock(dev);
        if (dev->get_idle) {
            ret = dev->get_idle(dev);
        } else {
            ret = ESP_ERR_NOT_SUPPORTED;
        }
        esp_hidh_dev_unlock(dev);
    } else {
        ret = ESP_FAIL;
    }
    return ret;
}

esp_err_t esp_hidh_dev_set_idle(esp_hidh_dev_t *dev, uint8_t idle_time)
{
    esp_err_t ret = ESP_OK;
    if (esp_hidh_dev_exists(dev)) {
        esp_hidh_dev_lock(dev);
        if (dev->set_idle) {
            ret = dev->set_idle(dev, idle_time);
        } else {
            ret = ESP_ERR_NOT_SUPPORTED;
        }
        esp_hidh_dev_unlock(dev);
    } else {
        ret = ESP_FAIL;
    }
    return ret;
}

esp_err_t esp_hidh_dev_get_protocol(esp_hidh_dev_t *dev)
{
    esp_err_t ret = ESP_OK;
    if (esp_hidh_dev_exists(dev)) {
        esp_hidh_dev_lock(dev);
        if (dev->get_protocol) {
            ret = dev->get_protocol(dev);
        } else {
            ret = ESP_ERR_NOT_SUPPORTED;
        }
        esp_hidh_dev_unlock(dev);
    } else {
        ret = ESP_FAIL;
    }
    return ret;
}

esp_err_t esp_hidh_dev_set_protocol(esp_hidh_dev_t *dev, uint8_t protocol_mode)
{
    esp_err_t ret = ESP_OK;
    if (esp_hidh_dev_exists(dev)) {
        esp_hidh_dev_lock(dev);
        if (dev->set_protocol) {
            ret = dev->set_protocol(dev, protocol_mode);
        } else {
            ret = ESP_ERR_NOT_SUPPORTED;
        }
        esp_hidh_dev_unlock(dev);
    } else {
        ret = ESP_FAIL;
    }
    return ret;
}

const uint8_t *esp_hidh_dev_bda_get(esp_hidh_dev_t *dev)
{
    uint8_t *ret = NULL;
#if CONFIG_BLUEDROID_ENABLED
    if (esp_hidh_dev_exists(dev)) {
        esp_hidh_dev_lock(dev);
        ret = dev->bda;
        esp_hidh_dev_unlock(dev);
    }
#endif /* CONFIG_BLUEDROID_ENABLED */
    return ret;
}

esp_hid_transport_t esp_hidh_dev_transport_get(esp_hidh_dev_t *dev)
{
    esp_hid_transport_t ret = ESP_HID_TRANSPORT_MAX;
    if (esp_hidh_dev_exists(dev)) {
        esp_hidh_dev_lock(dev);
        ret = dev->transport;
        esp_hidh_dev_unlock(dev);
    }
    return ret;
}

const esp_hid_device_config_t *esp_hidh_dev_config_get(esp_hidh_dev_t *dev)
{
    esp_hid_device_config_t *ret = NULL;
    if (esp_hidh_dev_exists(dev)) {
        esp_hidh_dev_lock(dev);
        ret = &dev->config;
        esp_hidh_dev_unlock(dev);
    }
    return ret;
}

const char *esp_hidh_dev_name_get(esp_hidh_dev_t *dev)
{
    const char * ret = NULL;
    if (esp_hidh_dev_exists(dev)) {
        esp_hidh_dev_lock(dev);
        ret = dev->config.device_name ? dev->config.device_name : "";
        esp_hidh_dev_unlock(dev);
    }
    return ret;
}

const char *esp_hidh_dev_manufacturer_get(esp_hidh_dev_t *dev)
{
    const char *ret = NULL;
    if (esp_hidh_dev_exists(dev)) {
        esp_hidh_dev_lock(dev);
        ret = dev->config.manufacturer_name ? dev->config.manufacturer_name : "";
        esp_hidh_dev_unlock(dev);
    }
    return ret;
}

const char *esp_hidh_dev_serial_get(esp_hidh_dev_t *dev)
{
    const char *ret = NULL;
    if (esp_hidh_dev_exists(dev)) {
        esp_hidh_dev_lock(dev);
        ret = dev->config.serial_number ? dev->config.serial_number : "";
        esp_hidh_dev_unlock(dev);
    }
    return ret;
}

uint16_t esp_hidh_dev_vendor_id_get(esp_hidh_dev_t *dev)
{
    uint16_t ret = 0;
    if (esp_hidh_dev_exists(dev)) {
        esp_hidh_dev_lock(dev);
        ret = dev->config.vendor_id;
        esp_hidh_dev_unlock(dev);
    }
    return ret;
}

uint16_t esp_hidh_dev_product_id_get(esp_hidh_dev_t *dev)
{
    uint16_t ret = 0;
    if (esp_hidh_dev_exists(dev)) {
        esp_hidh_dev_lock(dev);
        ret = dev->config.product_id;
        esp_hidh_dev_unlock(dev);
    }
    return ret;
}

uint16_t esp_hidh_dev_version_get(esp_hidh_dev_t *dev)
{
    uint16_t ret = 0;
    if (!esp_hidh_dev_exists(dev)) {
        esp_hidh_dev_lock(dev);
        ret = dev->config.version;
        esp_hidh_dev_unlock(dev);
    }
    return ret;
}

esp_hid_usage_t esp_hidh_dev_usage_get(esp_hidh_dev_t *dev)
{
    esp_hid_usage_t ret = ESP_HID_USAGE_GENERIC;
    if (esp_hidh_dev_exists(dev)) {
        esp_hidh_dev_lock(dev);
        ret = dev->usage;
        esp_hidh_dev_unlock(dev);
    }
    return ret;
}

esp_err_t esp_hidh_dev_reports_get(esp_hidh_dev_t *dev, size_t *num_reports, esp_hid_report_item_t **reports)
{
    esp_err_t ret = 0;
    esp_hid_report_item_t *r = NULL;

    if (!esp_hidh_dev_exists(dev)) {
        return ESP_FAIL;
    }

    esp_hidh_dev_lock(dev);
    do {
        r = (esp_hid_report_item_t *)malloc(sizeof(esp_hid_report_item_t) * dev->reports_len);
        if (r == NULL) {
            ret = ESP_FAIL;
            break;
        }

        esp_hidh_dev_report_t *dr = dev->reports;
        for (uint8_t i = 0; i < dev->reports_len; i++) {
            if (dr == NULL) {
                // error
                free(r);
                ret = ESP_FAIL;
                goto error_;
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
    } while (0);
error_:;
    esp_hidh_dev_unlock(dev);

    return ret;
}

esp_err_t esp_hidh_dev_report_maps_get(esp_hidh_dev_t *dev, size_t *num_maps, esp_hid_raw_report_map_t **maps)
{
    if (!esp_hidh_dev_exists(dev)) {
        return ESP_FAIL;
    }
    esp_hidh_dev_lock(dev);
    *num_maps = dev->config.report_maps_len;
    *maps = dev->config.report_maps;
    esp_hidh_dev_unlock(dev);
    return ESP_OK;
}


/*
 * Private Functions
 * */

/**
 * `lock_devices()` only protect the devices list, this mutex protect the single deivce instance.
 */
inline void esp_hidh_dev_lock(esp_hidh_dev_t *dev)
{
    if (dev && dev->mutex != NULL) {
        xSemaphoreTake(dev->mutex, portMAX_DELAY);
    }
}

inline void esp_hidh_dev_unlock(esp_hidh_dev_t *dev)
{
    if (dev && dev->mutex != NULL) {
        xSemaphoreGive(dev->mutex);
    }
}

inline void esp_hidh_dev_wait(esp_hidh_dev_t *dev)
{
    if (dev && dev->semaphore != NULL) {
        xSemaphoreTake(dev->semaphore, portMAX_DELAY);
    }
}

inline void esp_hidh_dev_send(esp_hidh_dev_t *dev)
{
    if (dev && dev->semaphore != NULL) {
        xSemaphoreGive(dev->semaphore);
    }
}

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

esp_hidh_dev_report_t *esp_hidh_dev_get_report_by_id_type_proto(esp_hidh_dev_t *dev, size_t map_index, size_t report_id, int report_type, uint8_t protocol_mode)
{
    esp_hidh_dev_report_t *r = dev->reports;
    while (r) {
        if (r->map_index == map_index && r->report_type == report_type && r->report_id == report_id &&
            r->protocol_mode == protocol_mode) {
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

esp_hidh_dev_report_t *esp_hidh_dev_get_input_report_by_len_and_proto(esp_hidh_dev_t *dev, size_t len, int protocol_mode)
{
    esp_hidh_dev_report_t *r = dev->reports;
    while (r) {
        if (r->value_len == len && (r->report_type & 1) && r->protocol_mode == protocol_mode) {
            return r;
        }
        r = r->next;
    }
    return NULL;
}

/**
 * If no Report ID item tags are present in the Report descriptor, it
 * can be assumed that only one Input, Output, and Feature report structure exists
 * and together they represent all of the device’s data.
 */
esp_hidh_dev_report_t *esp_hidh_dev_get_input_report_by_proto_and_data(esp_hidh_dev_t *dev, int protocol_mode,
                                                                       size_t len, const uint8_t *data, bool *has_report_id)
{
    esp_hidh_dev_report_t *r = dev->reports;
    *has_report_id = false;
    // first, assume data not include report id
    while (r) {
        if (r->value_len == len && r->report_id == 0 && (r->report_type & 1) &&
            r->protocol_mode == protocol_mode) {
            *has_report_id = false;
            break;
        }
        r = r->next;
    }
    // indicate data include report id
    if (r == NULL) {
        if (*data == 0) {
            ESP_LOGE(TAG, "data not include report id!");
            *has_report_id = false;
            return NULL;
        }
        r = dev->reports;
        while (r) {
            if (r->value_len == len + 1 && r->report_id == *data && (r->report_type & 1) &&
                r->protocol_mode == protocol_mode) {
                *has_report_id = true;
                break;
            }
            r = r->next;
        }
    }
    return r;
}

static void esp_hidh_dev_resources_free(esp_hidh_dev_t *dev)
{
    esp_hidh_dev_lock(dev);
    if (dev->semaphore) {
        vSemaphoreDelete(dev->semaphore);
    }
    if (dev->trans_timer) {
        esp_timer_stop(dev->trans_timer);
        esp_timer_delete(dev->trans_timer);
        dev->trans_timer = NULL;
    }
    free((void *)dev->config.device_name);
    free((void *)dev->config.manufacturer_name);
    free((void *)dev->config.serial_number);
    for (uint8_t d = 0; d < dev->config.report_maps_len; d++) {
        free((void *)dev->config.report_maps[d].data);
    }
    free((void *)dev->config.report_maps);
    esp_hidh_dev_report_t *r;
    while (dev->reports) {
        r = dev->reports;
        dev->reports = dev->reports->next;
        free(r);
    }
    esp_hidh_dev_unlock(dev);
    if (dev->mutex) {
        vSemaphoreDelete(dev->mutex);
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

    dev->mutex = xSemaphoreCreateMutex();
    if (dev->mutex == NULL) {
        ESP_LOGE(TAG, "malloc mutex failed");
        esp_hidh_dev_resources_free(dev);
        return NULL;
    }

    lock_devices();
    TAILQ_INSERT_TAIL(&s_esp_hidh_devices, dev, devices);
    unlock_devices();

    return dev;
}

/**
 * The `dev` is allocated by the internal function, and it should also be freed by the internal function. So, when the
 * user call this function, it will do nothing.
 */
esp_err_t esp_hidh_dev_free(esp_hidh_dev_t *dev)
{
    return ESP_OK;
}

esp_err_t esp_hidh_dev_free_inner(esp_hidh_dev_t *dev)
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

static void esp_hidh_dev_delay_free(void *arg)
{
    esp_hidh_dev_t *d = NULL;
    esp_hidh_dev_t *next = NULL;
    lock_devices();
    TAILQ_FOREACH_SAFE(d, &s_esp_hidh_devices, devices, next) {
        if (!d->in_use) {
            TAILQ_REMOVE(&s_esp_hidh_devices, d, devices);
            esp_hidh_dev_resources_free(d);
        }
    }
    unlock_devices();
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

esp_hidh_dev_t *esp_hidh_dev_get_by_handle(uint8_t handle)
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

/**
 * The deep copy data append the end of the esp_hidh_event_data_t, move the data pointer to the correct address. This is
 * a workaround way, it's better to use flexiable array in the interface.
 */
void esp_hidh_process_event_data_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id,
                                         void *event_data)
{
    esp_hidh_event_t event = (esp_hidh_event_t)event_id;
    esp_hidh_event_data_t *param = (esp_hidh_event_data_t *)event_data;

    switch (event) {
    case ESP_HIDH_INPUT_EVENT:
        if (param->input.length && param->input.data) {
            param->input.data = (uint8_t *)param + sizeof(esp_hidh_event_data_t);
        }
        break;
    case ESP_HIDH_FEATURE_EVENT:
        if (param->feature.length && param->feature.data) {
            param->feature.data = (uint8_t *)param + sizeof(esp_hidh_event_data_t);
        }
        break;
    case ESP_HIDH_OPEN_EVENT:
        if (param->open.status != ESP_OK) {
            if (s_esp_hidh_timer && !esp_timer_is_active(s_esp_hidh_timer) &&
                esp_timer_start_once(s_esp_hidh_timer, ESP_HIDH_DELAY_FREE_TO) != ESP_OK) {
                ESP_LOGE(TAG, "%s set hidh timer failed!", __func__);
            }
        }
        break;
    case ESP_HIDH_CLOSE_EVENT:
        if (s_esp_hidh_timer && !esp_timer_is_active(s_esp_hidh_timer) &&
            esp_timer_start_once(s_esp_hidh_timer, ESP_HIDH_DELAY_FREE_TO) != ESP_OK) {
            ESP_LOGE(TAG, "%s set hidh timer failed!", __func__);
        }
        break;
    default:
        break;
    }
}
#endif /* CONFIG_BLUEDROID_ENABLED */
