/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "bt_hidd.h"

#if CONFIG_BT_HID_DEVICE_ENABLED
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_hidd.h"
#include "esp_hidd_api.h"
#include "esp_private/esp_hidd_private.h"
#include "esp_log.h"
#include "osi/mutex.h"
#include "string.h"

/* Values for service_type */
#define NO_TRAFFIC      0
#define BEST_EFFORT     1
#define GUARANTEED      2

static const char *TAG = "BT_HIDD";

typedef struct {
    esp_hid_raw_report_map_t    reports_map;
    uint8_t                     reports_len;
    hidd_report_item_t          *reports;
} hidd_dev_map_t;

typedef struct {
    esp_hidd_dev_t             *dev;
    esp_event_loop_handle_t     event_loop_handle;
    esp_hid_device_config_t     config;
    uint16_t                    appearance;
    bool                        registered;
    bool                        connected;
    esp_bd_addr_t               remote_bda;
    uint8_t                     bat_level;  // 0 - 100 - battery percentage
    uint8_t                     control;    // 0x00 suspend, 0x01 suspend off
    uint8_t                     protocol_mode;   // 0x00 boot, 0x01 report
    hidd_dev_map_t             *devices;
    uint8_t                     devices_len;
} esp_bt_hidd_dev_t;

typedef struct {
    osi_mutex_t mutex;
    esp_bt_hidd_dev_t *dev;
    esp_hidd_app_param_t app_param;
    esp_hidd_qos_param_t in_qos;
    esp_hidd_qos_param_t out_qos;
} hidd_param_t;

static hidd_param_t s_hidd_param = {0};
#define is_init() (s_hidd_param.dev != NULL)
#define UNUSED(x) (void)(x)

static esp_err_t bt_hidd_get_status(esp_hidd_status_t status)
{
    esp_err_t ret = ESP_OK;
    switch (status) {
    case ESP_HIDD_SUCCESS:
        ret = ESP_OK;
        break;
    case ESP_HIDD_NO_RES:
        ret = ESP_ERR_NO_MEM;
        break;
    default:
        ret = ESP_FAIL;
        break;
    }
    return ret;
}

static esp_err_t bt_hidd_init_config(esp_bt_hidd_dev_t *dev, const esp_hid_device_config_t *config)
{
    if (config->report_maps == NULL || config->report_maps_len == 0 || config->report_maps_len > 1) {
        return ESP_ERR_INVALID_ARG;
    }
    memset((uint8_t *)(&dev->config), 0, sizeof(esp_hid_device_config_t));
    dev->config.vendor_id = config->vendor_id;
    dev->config.product_id = config->product_id;
    dev->config.version = config->version;
    if (config->device_name != NULL) {
        dev->config.device_name = strdup(config->device_name);
    }
    if (config->manufacturer_name != NULL) {
        dev->config.manufacturer_name = strdup(config->manufacturer_name);
    }
    if (config->serial_number != NULL) {
        dev->config.serial_number = strdup(config->serial_number);
    }
    dev->appearance = ESP_HID_APPEARANCE_GENERIC;

    if (config->report_maps_len) {
        dev->devices = (hidd_dev_map_t *)malloc(config->report_maps_len * sizeof(hidd_dev_map_t));
        if (dev->devices == NULL) {
            ESP_LOGE(TAG, "devices malloc(%d) failed", config->report_maps_len);
            return ESP_ERR_NO_MEM;
        }
        memset(dev->devices, 0, config->report_maps_len * sizeof(hidd_dev_map_t));
        dev->devices_len = config->report_maps_len;
        for (uint8_t d = 0; d < dev->devices_len; d++) {

            //raw report map
            uint8_t *map = (uint8_t *)malloc(config->report_maps[d].len);
            if (map == NULL) {
                ESP_LOGE(TAG, "report map malloc(%d) failed", config->report_maps[d].len);
                return ESP_ERR_NO_MEM;
            }
            memcpy(map, config->report_maps[d].data, config->report_maps[d].len);

            dev->devices[d].reports_map.data = (const uint8_t *)map;
            dev->devices[d].reports_map.len = config->report_maps[d].len;

            esp_hid_report_map_t *rmap = esp_hid_parse_report_map(config->report_maps[d].data, config->report_maps[d].len);
            if (rmap == NULL) {
                ESP_LOGE(TAG, "hid_parse_report_map[%d](%d) failed", d, config->report_maps[d].len);
                return ESP_FAIL;
            }
            dev->appearance = rmap->appearance;
            dev->devices[d].reports_len = rmap->reports_len;
            dev->devices[d].reports = (hidd_report_item_t *)malloc(rmap->reports_len * sizeof(hidd_report_item_t));
            if (dev->devices[d].reports == NULL) {
                ESP_LOGE(TAG, "reports malloc(%d) failed", rmap->reports_len * sizeof(hidd_report_item_t));
                free(rmap);
                return ESP_ERR_NO_MEM;
            }
            for (uint8_t r = 0; r < rmap->reports_len; r++) {
                dev->devices[d].reports[r].map_index = d;
                dev->devices[d].reports[r].report_id = rmap->reports[r].report_id;
                dev->devices[d].reports[r].protocol_mode = rmap->reports[r].protocol_mode;
                dev->devices[d].reports[r].report_type = rmap->reports[r].report_type;
                dev->devices[d].reports[r].usage = rmap->reports[r].usage;
                dev->devices[d].reports[r].value_len = rmap->reports[r].value_len;
            }
            free(rmap->reports);
            free(rmap);
        }
    }

    return ESP_OK;
}

static hidd_report_item_t *get_report_by_idx_id_type(esp_bt_hidd_dev_t *dev, size_t index, uint8_t id, uint8_t type)
{
    hidd_report_item_t *rpt = NULL;
    if (index >= dev->devices_len) {
        ESP_LOGE(TAG, "index out of range[0-%d]", dev->devices_len - 1);
        return NULL;
    }
    for (uint8_t i = 0; i < dev->devices[index].reports_len; i++) {
        rpt = &dev->devices[index].reports[i];
        if (rpt->report_id == id && rpt->report_type == type && rpt->protocol_mode == dev->protocol_mode) {
            return rpt;
        }
    }
    return NULL;
}

static hidd_report_item_t *get_report_by_id_and_type(esp_bt_hidd_dev_t *dev, uint8_t id, uint8_t type, uint8_t *index)
{
    hidd_report_item_t *rpt = NULL;
    for (uint8_t idx = 0; idx < dev->devices_len; idx++) {
        for (uint8_t i = 0; i < dev->devices[idx].reports_len; i++) {
            rpt = &dev->devices[idx].reports[i];
            if (rpt->report_id == id && rpt->report_type == type && rpt->protocol_mode == dev->protocol_mode) {
                if (index) {
                    *index = idx;
                }
                return rpt;
            }
        }
    }
    return NULL;
}

static esp_err_t bt_hid_free_config(esp_bt_hidd_dev_t *dev)
{
    for (uint8_t d = 0; d < dev->devices_len; d++) {
        free((void *)dev->devices[d].reports);
        free((void *)dev->devices[d].reports_map.data);
    }

    free((void *)dev->devices);
    free((void *)dev->config.device_name);
    free((void *)dev->config.manufacturer_name);
    free((void *)dev->config.serial_number);
    if (dev->event_loop_handle != NULL) {
        esp_event_loop_delete(dev->event_loop_handle);
        dev->event_loop_handle = NULL;
    }
    return ESP_OK;
}

static void bt_hidd_dev_free(void)
{
    if (s_hidd_param.dev) {
        osi_mutex_lock(&s_hidd_param.mutex, OSI_MUTEX_MAX_TIMEOUT);
        bt_hid_free_config(s_hidd_param.dev);
        free(s_hidd_param.dev);
        s_hidd_param.dev = NULL;
        osi_mutex_unlock(&s_hidd_param.mutex);
        osi_mutex_free(&s_hidd_param.mutex);
    }
}

static void build_default_in_qos(esp_hidd_qos_param_t *in_qos, uint32_t value_len)
{

    if (value_len > 0) {
        in_qos->service_type = GUARANTEED;
        in_qos->token_rate = value_len * 100;
        in_qos->token_bucket_size = value_len;
        in_qos->peak_bandwidth = value_len * 100;
        in_qos->access_latency = 10;
        in_qos->delay_variation = 10;
    } else {
        memset(in_qos, 0, sizeof(esp_hidd_qos_param_t));
    }
}

static void build_default_out_qos(esp_hidd_qos_param_t *out_qos, uint32_t value_len)
{
    if (value_len > 0) {
        out_qos->service_type = GUARANTEED;
        out_qos->token_rate = value_len * 100;
        out_qos->token_bucket_size = value_len;
        out_qos->peak_bandwidth = value_len * 100;
        out_qos->access_latency = 10;
        out_qos->delay_variation = 10;
    } else {
        memset(out_qos, 0, sizeof(esp_hidd_qos_param_t));
    }
}

static uint8_t get_subclass_by_appearance(uint16_t appearance)
{
    uint8_t ret = ESP_HID_CLASS_UNKNOWN;
    switch (appearance) {
    case ESP_HID_APPEARANCE_KEYBOARD:
        ret = ESP_HID_CLASS_KBD;
        break;
    case ESP_HID_APPEARANCE_MOUSE:
        ret = ESP_HID_CLASS_MIC;
        break;
    case ESP_HID_APPEARANCE_JOYSTICK:
        ret = ESP_HID_CLASS_JOS;
        break;
    case ESP_HID_APPEARANCE_GAMEPAD:
        ret = ESP_HID_CLASS_GPD;
        break;
    default:
        ret = ESP_HID_CLASS_UNKNOWN;
        break;
    }
    return ret;
}

static uint32_t get_value_len_by_type_protocol(esp_bt_hidd_dev_t *dev, uint8_t report_type, uint8_t protocol_mode)
{
    uint32_t value_len = 0;
    hidd_report_item_t *rpt = NULL;
    for (uint8_t d = 0; d < dev->devices_len; d++) {
        for (uint8_t i = 0; i < dev->devices[d].reports_len; i++) {
            rpt = &dev->devices[d].reports[i];
            if (rpt->report_type == report_type && rpt->protocol_mode == dev->protocol_mode) {
                value_len += rpt->value_len;
            }
        }
    }
    return value_len;
}

static void bt_hidd_init_app(void)
{
    esp_hid_device_config_t *p_config = &s_hidd_param.dev->config;
    s_hidd_param.app_param.name = p_config->device_name;
    s_hidd_param.app_param.description = p_config->device_name;
    s_hidd_param.app_param.provider = p_config->manufacturer_name;
    s_hidd_param.app_param.subclass = get_subclass_by_appearance(s_hidd_param.dev->appearance);
    s_hidd_param.app_param.desc_list = (uint8_t *)s_hidd_param.dev->devices[0].reports_map.data;
    s_hidd_param.app_param.desc_list_len = s_hidd_param.dev->devices[0].reports_map.len;
}

static void bt_hidd_init_qos(void)
{
    uint32_t value_len = 0;
    value_len =
        get_value_len_by_type_protocol(s_hidd_param.dev, ESP_HID_REPORT_TYPE_INPUT, s_hidd_param.dev->protocol_mode);
    build_default_in_qos(&s_hidd_param.in_qos, value_len);

    value_len =
        get_value_len_by_type_protocol(s_hidd_param.dev, ESP_HID_REPORT_TYPE_INPUT, s_hidd_param.dev->protocol_mode);
    build_default_out_qos(&s_hidd_param.out_qos, value_len);
}

static bool esp_bt_hidd_dev_connected(void *devp)
{
    esp_bt_hidd_dev_t *dev = (esp_bt_hidd_dev_t *)devp;
    bool ret = true;
    osi_mutex_lock(&s_hidd_param.mutex, OSI_MUTEX_MAX_TIMEOUT);
    do {
        if (!is_init()) {
            ESP_LOGE(TAG, "HID device profile is uninit");
            ret = false;
            break;
        }

        if (s_hidd_param.dev != dev) {
            ESP_LOGE(TAG, "Wrong HID device provided");
            ret = false;
            break;
        }
    } while (0);
    if (ret) {
        ret = dev->connected;
    }
    osi_mutex_unlock(&s_hidd_param.mutex);
    return ret;
}

static esp_err_t esp_bt_hidd_dev_deinit(void *devp)
{
    esp_bt_hidd_dev_t *dev = (esp_bt_hidd_dev_t *)devp;
    esp_err_t ret = ESP_OK;
    osi_mutex_lock(&s_hidd_param.mutex, OSI_MUTEX_MAX_TIMEOUT);
    do {
        if (!is_init()) {
            osi_mutex_unlock(&s_hidd_param.mutex);
            ESP_LOGE(TAG, "HID device profile already uninitialized");
            return ESP_OK;
        }

        if (s_hidd_param.dev != dev) {
            ESP_LOGE(TAG, "Wrong HID device provided");
            ret = ESP_FAIL;
        }
    } while (0);
    osi_mutex_unlock(&s_hidd_param.mutex);

    if (ret == ESP_OK) {
        ret = esp_bt_hid_device_deinit();
    }
    return ret;
}

static esp_err_t esp_bt_hidd_dev_disconnect(void *devp)
{
    esp_bt_hidd_dev_t *dev = (esp_bt_hidd_dev_t *)devp;
    esp_err_t ret = ESP_OK;
    osi_mutex_lock(&s_hidd_param.mutex, OSI_MUTEX_MAX_TIMEOUT);
    do {
        if (!is_init()) {
            ESP_LOGE(TAG, "HID device is uninit");
            ret = ESP_FAIL;
            break;
        }

        if (s_hidd_param.dev != dev) {
            ESP_LOGE(TAG, "Wrong HID device provided");
            ret = ESP_FAIL;
            break;
        }

        if (!dev->connected) {
            osi_mutex_unlock(&s_hidd_param.mutex);
            ESP_LOGW(TAG, "already disconnected");
            return ESP_OK;
        }
    } while (0);
    osi_mutex_unlock(&s_hidd_param.mutex);

    if (ret == ESP_OK) {
        ret = esp_bt_hid_device_disconnect();
    }
    return ret;
}

static esp_err_t esp_bt_hidd_dev_battery_set(void *devp, uint8_t level)
{
    UNUSED(devp);
    UNUSED(level);
    ESP_LOGW(TAG, "Not implement yet!");
    return ESP_OK;
}

static esp_err_t esp_bt_hidd_dev_input_set(void *devp, size_t index, size_t id, uint8_t *data, size_t length)
{
    hidd_report_item_t *p_rpt;
    esp_bt_hidd_dev_t *dev = (esp_bt_hidd_dev_t *)devp;
    esp_err_t ret = ESP_OK;
    osi_mutex_lock(&s_hidd_param.mutex, OSI_MUTEX_MAX_TIMEOUT);
    do {
        if (!is_init()) {
            ESP_LOGE(TAG, "HID device is uninit");
            ret = ESP_FAIL;
            break;
        }

        if (s_hidd_param.dev != dev) {
            ESP_LOGE(TAG, "Wrong HID device provided");
            ret = ESP_FAIL;
            break;
        }

        if (!dev->connected) {
            ESP_LOGE(TAG, "HID device not connected!");
            ret = ESP_FAIL;
            break;
        }

        p_rpt = get_report_by_idx_id_type(dev, index, id, ESP_HID_REPORT_TYPE_INPUT);
        if (p_rpt == NULL) {
            ESP_LOGE(TAG, "HID device not connected!");
            ret = ESP_FAIL;
            break;
        }

        if (length > p_rpt->value_len) {
            ESP_LOGE(TAG, "Data size over %d!", p_rpt->value_len);
            ret = ESP_FAIL;
            break;
        }
    } while (0);
    osi_mutex_unlock(&s_hidd_param.mutex);

    if (ret == ESP_OK) {
        ret = esp_bt_hid_device_send_report(ESP_HIDD_REPORT_TYPE_INTRDATA, id, length, data);
    }
    return ret;
}

static esp_err_t esp_bt_hidd_dev_feature_set(void *devp, size_t index, size_t id, uint8_t *data, size_t length)
{
    hidd_report_item_t *p_rpt;
    esp_bt_hidd_dev_t *dev = (esp_bt_hidd_dev_t *)devp;
    esp_err_t ret = ESP_OK;
    osi_mutex_lock(&s_hidd_param.mutex, OSI_MUTEX_MAX_TIMEOUT);
    do {
        if (!is_init()) {
            ESP_LOGE(TAG, "HID device is uninit");
            ret = ESP_FAIL;
            break;
        }

        if (s_hidd_param.dev != dev) {
            ESP_LOGE(TAG, "Wrong HID device provided");
            ret = ESP_FAIL;
            break;
        }

        if (!dev->connected) {
            ESP_LOGE(TAG, "HID device not connected!");
            ret = ESP_FAIL;
            break;
        }

        p_rpt = get_report_by_idx_id_type(dev, index, id, ESP_HID_REPORT_TYPE_FEATURE);
        if (p_rpt == NULL) {
            ESP_LOGE(TAG, "HID device not connected!");
            ret = ESP_FAIL;
            break;
        }

        if (length > p_rpt->value_len) {
            ESP_LOGE(TAG, "Data size over %d!", p_rpt->value_len);
            ret = ESP_FAIL;
            break;
        }
    } while (0);
    osi_mutex_unlock(&s_hidd_param.mutex);

    if (ret == ESP_OK) {
        ret = esp_bt_hid_device_send_report(ESP_HID_REPORT_TYPE_FEATURE, id, length, data);
    }
    return ret;
}

static esp_err_t esp_bt_hidd_dev_event_handler_register(void *devp, esp_event_handler_t callback, esp_hidd_event_t event)
{
    esp_bt_hidd_dev_t *dev = (esp_bt_hidd_dev_t *)devp;
    esp_err_t ret = ESP_OK;
    osi_mutex_lock(&s_hidd_param.mutex, OSI_MUTEX_MAX_TIMEOUT);
    do {
        if (!is_init()) {
            ESP_LOGE(TAG, "HID device is uninit");
            ret = ESP_FAIL;
            break;
        }

        if (s_hidd_param.dev != dev) {
            ESP_LOGE(TAG, "Wrong HID device provided");
            ret = ESP_FAIL;
            break;
        }

        ret = esp_event_handler_register_with(dev->event_loop_handle, ESP_HIDD_EVENTS, event, callback, dev->dev);
    } while (0);
    osi_mutex_unlock(&s_hidd_param.mutex);
    return ret;
}

static esp_err_t esp_bt_hidd_dev_event_handler_unregister(void *devp, esp_event_handler_t callback, esp_hidd_event_t event)
{
    esp_bt_hidd_dev_t *dev = (esp_bt_hidd_dev_t *)devp;
    esp_err_t ret = ESP_OK;
    osi_mutex_lock(&s_hidd_param.mutex, OSI_MUTEX_MAX_TIMEOUT);
    do {
        if (!is_init()) {
            ESP_LOGE(TAG, "HID device is uninit");
            ret = ESP_FAIL;
            break;
        }

        if (s_hidd_param.dev != dev) {
            ESP_LOGE(TAG, "Wrong HID device provided");
            ret = ESP_FAIL;
            break;
        }
        ret = esp_event_handler_unregister_with(dev->event_loop_handle, ESP_HIDD_EVENTS, event, callback);
    } while (0);
    osi_mutex_unlock(&s_hidd_param.mutex);
    return ret;
}

void bt_hidd_cb(esp_hidd_cb_event_t event, esp_hidd_cb_param_t *param)
{
    esp_hidd_event_data_t cb_param = {0};
    esp_hidd_event_data_t *p_cb_param = NULL;
    size_t event_data_size = 0;
    uint8_t map_index = 0;
    hidd_report_item_t *p_rpt = NULL;

    if (!is_init()) {
        ESP_LOGE(TAG, "HID device is uninit, event(%d)", event);
        return;
    }

    switch (event) {
    case ESP_HIDD_INIT_EVT: {
        if (param->init.status == ESP_HIDD_SUCCESS) {
            ESP_LOGD(TAG, "Setting hid parameters in_qos:%d, out_qos:%d", s_hidd_param.in_qos.token_bucket_size,
                     s_hidd_param.out_qos.token_bucket_size);
            esp_bt_hid_device_register_app(&s_hidd_param.app_param, &s_hidd_param.in_qos, &s_hidd_param.out_qos);
        } else {
            ESP_LOGE(TAG, "Init hidd failed (%d)!", param->init.status);
            cb_param.start.status = bt_hidd_get_status(param->init.status);
            esp_event_post_to(s_hidd_param.dev->event_loop_handle, ESP_HIDD_EVENTS, ESP_HIDD_START_EVENT, &cb_param,
                              sizeof(esp_hidd_event_data_t), portMAX_DELAY);

            bt_hidd_dev_free();
        }
        break;
    }
    case ESP_HIDD_DEINIT_EVT: {
        cb_param.stop.status = bt_hidd_get_status(param->deinit.status);
        esp_event_post_to(s_hidd_param.dev->event_loop_handle, ESP_HIDD_EVENTS, ESP_HIDD_STOP_EVENT, &cb_param,
                          sizeof(esp_hidd_event_data_t), portMAX_DELAY);
        if (param->deinit.status == ESP_HIDD_SUCCESS) {
            bt_hidd_dev_free();
        } else {
            ESP_LOGE(TAG, "Deinit hidd failed (%d)!", param->deinit.status);
        }
        break;
    }
    case ESP_HIDD_REGISTER_APP_EVT: {
        if (param->register_app.status == ESP_HIDD_SUCCESS) {
            ESP_LOGD(TAG, "Setting hid parameters success!");
            if (param->register_app.in_use) {
                ESP_LOGI(TAG, "Start virtual cable plug!");
                esp_bt_hid_device_connect(param->register_app.bd_addr);
            }

            osi_mutex_lock(&s_hidd_param.mutex, OSI_MUTEX_MAX_TIMEOUT);
            s_hidd_param.dev->registered = true;
            osi_mutex_unlock(&s_hidd_param.mutex);

            cb_param.start.status = bt_hidd_get_status(param->init.status);
            esp_event_post_to(s_hidd_param.dev->event_loop_handle, ESP_HIDD_EVENTS, ESP_HIDD_START_EVENT, &cb_param,
                              sizeof(esp_hidd_event_data_t), portMAX_DELAY);
        } else {
            ESP_LOGE(TAG, "Setting hid parameters failed (%d), now deint!", param->register_app.status);
            esp_bt_hid_device_deinit();
        }
        break;
    }
    case ESP_HIDD_UNREGISTER_APP_EVT: {
        break;
    }
    case ESP_HIDD_OPEN_EVT: {
        if (param->open.conn_status == ESP_HIDD_CONN_STATE_CONNECTING) {
            break;
        }
        if (param->open.status == ESP_HIDD_SUCCESS && param->open.conn_status == ESP_HIDD_CONN_STATE_CONNECTED) {
            ESP_LOGI(TAG, "Connected to %02x:%02x:%02x:%02x:%02x:%02x", param->open.bd_addr[0], param->open.bd_addr[1],
                     param->open.bd_addr[2], param->open.bd_addr[3], param->open.bd_addr[4], param->open.bd_addr[5]);
            osi_mutex_lock(&s_hidd_param.mutex, OSI_MUTEX_MAX_TIMEOUT);
            s_hidd_param.dev->connected = true;
            memcpy(s_hidd_param.dev->remote_bda, param->open.bd_addr, ESP_BD_ADDR_LEN);
            osi_mutex_unlock(&s_hidd_param.mutex);
        } else {
            ESP_LOGE(TAG, "Connect failed (%d)!", param->open.status);
        }
        cb_param.connect.status = bt_hidd_get_status(param->open.status);
        cb_param.connect.dev = s_hidd_param.dev->dev;
        esp_event_post_to(s_hidd_param.dev->event_loop_handle, ESP_HIDD_EVENTS, ESP_HIDD_CONNECT_EVENT, &cb_param,
                          sizeof(esp_hidd_event_data_t), portMAX_DELAY);
        break;
    }
    case ESP_HIDD_CLOSE_EVT: {
        if (param->close.conn_status == ESP_HIDD_CONN_STATE_DISCONNECTING) {
            break;
        }
        if (param->close.status == ESP_HIDD_SUCCESS && param->close.conn_status == ESP_HIDD_CONN_STATE_DISCONNECTED) {
            osi_mutex_lock(&s_hidd_param.mutex, OSI_MUTEX_MAX_TIMEOUT);
            s_hidd_param.dev->connected = false;
            memset(s_hidd_param.dev->remote_bda, 0, ESP_BD_ADDR_LEN);
            osi_mutex_unlock(&s_hidd_param.mutex);
        } else {
            ESP_LOGE(TAG, "Disconnect failed (%d)!", param->close.status);
        }
        cb_param.disconnect.status = bt_hidd_get_status(param->close.status);
        cb_param.disconnect.dev = s_hidd_param.dev->dev;
        cb_param.disconnect.reason = param->close.status;
        esp_event_post_to(s_hidd_param.dev->event_loop_handle, ESP_HIDD_EVENTS, ESP_HIDD_DISCONNECT_EVENT, &cb_param,
                          sizeof(esp_hidd_event_data_t), portMAX_DELAY);
        break;
    }
    case ESP_HIDD_SEND_REPORT_EVT:
        break;
    case ESP_HIDD_REPORT_ERR_EVT:
        break;
    case ESP_HIDD_GET_REPORT_EVT: {
        uint8_t *data_ptr = NULL;
        p_rpt = get_report_by_id_and_type(s_hidd_param.dev, param->get_report.report_id, param->get_report.report_type,
                                          &map_index);
        if (p_rpt == NULL) {
            ESP_LOGE(TAG, "Can not find report!");
            esp_bt_hid_device_report_error(ESP_HID_PAR_HANDSHAKE_RSP_ERR_INVALID_REP_ID);
            break;
        }
        if (param->get_report.buffer_size > p_rpt->value_len) {
            ESP_LOGE(TAG, "Data size over %d!", p_rpt->value_len);
            esp_bt_hid_device_report_error(ESP_HID_PAR_HANDSHAKE_RSP_ERR_INVALID_PARAM);
            break;
        }

        event_data_size = sizeof(esp_hidd_event_data_t);
        if (param->get_report.buffer_size) {
            event_data_size += 2;
        }

        if ((p_cb_param = (esp_hidd_event_data_t *)malloc(event_data_size)) == NULL) {
            ESP_LOGE(TAG, "%s malloc event data failed!", __func__);
            break;
        }
        memset(p_cb_param, 0, event_data_size);
        p_cb_param->feature.dev = s_hidd_param.dev->dev;
        p_cb_param->feature.trans_type = ESP_HID_TRANS_GET_REPORT;
        p_cb_param->feature.report_type = param->get_report.report_type;
        p_cb_param->feature.report_id = p_rpt->report_id;
        p_cb_param->feature.usage = p_rpt->usage;
        p_cb_param->feature.length = param->get_report.buffer_size ? 2 : 0;
        p_cb_param->feature.data = ((uint8_t *)p_cb_param) + sizeof(esp_hidd_event_data_t);
        p_cb_param->feature.map_index = map_index;
        if (param->get_report.buffer_size) {
            data_ptr = ((uint8_t *)p_cb_param) + sizeof(esp_hidd_event_data_t);
            *data_ptr++ = (uint8_t)param->get_report.buffer_size;
            *data_ptr++ = (uint8_t)(param->get_report.buffer_size >> 8);
        }
        esp_event_post_to(s_hidd_param.dev->event_loop_handle, ESP_HIDD_EVENTS, ESP_HIDD_FEATURE_EVENT, p_cb_param,
                          event_data_size, portMAX_DELAY);
        break;
    }
    case ESP_HIDD_SET_REPORT_EVT: {
        p_rpt = get_report_by_id_and_type(s_hidd_param.dev, param->set_report.report_id, param->set_report.report_type,
                                          &map_index);
        if (p_rpt == NULL) {
            ESP_LOGE(TAG, "Can not find report!");
            esp_bt_hid_device_report_error(ESP_HID_PAR_HANDSHAKE_RSP_ERR_INVALID_REP_ID);
            break;
        }
        if (param->set_report.len > p_rpt->value_len) {
            ESP_LOGE(TAG, "Data size over %d!", p_rpt->value_len);
            esp_bt_hid_device_report_error(ESP_HID_PAR_HANDSHAKE_RSP_ERR_INVALID_PARAM);
            break;
        }

        event_data_size = sizeof(esp_hidd_event_data_t);
        if (param->set_report.len && param->set_report.data) {
            event_data_size += param->set_report.len;
        }

        if ((p_cb_param = (esp_hidd_event_data_t *)malloc(event_data_size)) == NULL) {
            ESP_LOGE(TAG, "%s malloc event data failed!", __func__);
            break;
        }
        memset(p_cb_param, 0, event_data_size);
        p_cb_param->feature.dev = s_hidd_param.dev->dev;
        p_cb_param->feature.trans_type = ESP_HID_TRANS_SET_REPORT;
        p_cb_param->feature.report_type = param->set_report.report_type;
        p_cb_param->feature.report_id = p_rpt->report_id;
        p_cb_param->feature.usage = p_rpt->usage;
        p_cb_param->feature.length = param->set_report.len;
        p_cb_param->feature.data = param->set_report.data;
        p_cb_param->feature.map_index = map_index;
        if (param->set_report.len && param->set_report.data) {
            memcpy(((uint8_t *)p_cb_param) + sizeof(esp_hidd_event_data_t), param->set_report.data,
                   param->set_report.len);
        }
        esp_event_post_to(s_hidd_param.dev->event_loop_handle, ESP_HIDD_EVENTS, ESP_HIDD_FEATURE_EVENT, p_cb_param,
                          event_data_size, portMAX_DELAY);
        break;
    }
    case ESP_HIDD_SET_PROTOCOL_EVT: {
        if (param->set_protocol.protocol_mode != ESP_HIDD_UNSUPPORTED_MODE) {
            osi_mutex_lock(&s_hidd_param.mutex, OSI_MUTEX_MAX_TIMEOUT);
            s_hidd_param.dev->protocol_mode =
                param->set_protocol.protocol_mode ? ESP_HID_PROTOCOL_MODE_BOOT : ESP_HID_PROTOCOL_MODE_REPORT;
            osi_mutex_unlock(&s_hidd_param.mutex);
            cb_param.protocol_mode.dev = s_hidd_param.dev->dev;
            cb_param.protocol_mode.protocol_mode = s_hidd_param.dev->protocol_mode;
            cb_param.protocol_mode.map_index = 0;
            esp_event_post_to(s_hidd_param.dev->event_loop_handle, ESP_HIDD_EVENTS, ESP_HIDD_PROTOCOL_MODE_EVENT,
                              &cb_param, sizeof(esp_hidd_event_data_t), portMAX_DELAY);
        } else {
            ESP_LOGE(TAG, "Unsupported protocol mode!");
            break;
        }
        break;
    }
    case ESP_HIDD_INTR_DATA_EVT: {
        p_rpt = get_report_by_id_and_type(s_hidd_param.dev, param->intr_data.report_id, ESP_HID_REPORT_TYPE_OUTPUT,
                                          &map_index);
        if (p_rpt == NULL) {
            ESP_LOGE(TAG, "Can not find report!");
            break;
        }

        event_data_size = sizeof(esp_hidd_event_data_t);
        if (param->intr_data.len && param->intr_data.data) {
            event_data_size += param->intr_data.len;
        }

        if ((p_cb_param = (esp_hidd_event_data_t *)malloc(event_data_size)) == NULL) {
            ESP_LOGE(TAG, "%s malloc event data failed!", __func__);
            break;
        }
        memset(p_cb_param, 0, event_data_size);
        p_cb_param->output.dev = s_hidd_param.dev->dev;
        p_cb_param->output.report_id = p_rpt->report_id;
        p_cb_param->output.usage = p_rpt->usage;
        p_cb_param->output.length = param->intr_data.len;
        p_cb_param->output.data = param->intr_data.data;
        p_cb_param->output.map_index = map_index;
        if (param->intr_data.len && param->intr_data.data) {
            memcpy(((uint8_t *)p_cb_param) + sizeof(esp_hidd_event_data_t), param->intr_data.data,
                   param->intr_data.len);
        }
        esp_event_post_to(s_hidd_param.dev->event_loop_handle, ESP_HIDD_EVENTS, ESP_HIDD_OUTPUT_EVENT, p_cb_param,
                          event_data_size, portMAX_DELAY);
        break;
    }
    default:
        break;
    }

    if (p_cb_param) {
        free(p_cb_param);
        p_cb_param = NULL;
    }
}

esp_err_t esp_bt_hidd_dev_init(esp_hidd_dev_t *dev_p, const esp_hid_device_config_t *config, esp_event_handler_t callback)
{
    esp_err_t ret = ESP_OK;
    if (dev_p == NULL || config == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (is_init()) {
        ESP_LOGE(TAG, "HID device profile already initialized");
        return ESP_FAIL;
    }

    osi_mutex_new(&s_hidd_param.mutex);
    if (s_hidd_param.mutex == NULL) {
        ESP_LOGE(TAG, "HID device mutex could not be allocated");
        return ESP_ERR_NO_MEM;
    }

    s_hidd_param.dev = (esp_bt_hidd_dev_t *)calloc(1, sizeof(esp_bt_hidd_dev_t));
    if (s_hidd_param.dev == NULL) {
        ESP_LOGE(TAG, "HID device could not be allocated");
        return ESP_ERR_NO_MEM;
    }

    //[1] Reset the hid device target environment
    s_hidd_param.dev->connected = false;
    s_hidd_param.dev->registered = false;
    s_hidd_param.dev->bat_level = 100;
    s_hidd_param.dev->control = ESP_HID_CONTROL_EXIT_SUSPEND;
    s_hidd_param.dev->protocol_mode = ESP_HID_PROTOCOL_MODE_REPORT;
    s_hidd_param.dev->event_loop_handle = NULL;
    s_hidd_param.dev->dev = dev_p;

    esp_event_loop_args_t event_task_args = {
        .queue_size = 5,
        .task_name = "bt_hidd_events",
        .task_priority = uxTaskPriorityGet(NULL),
        .task_stack_size = 2048,
        .task_core_id = tskNO_AFFINITY
    };
    ret = esp_event_loop_create(&event_task_args, &s_hidd_param.dev->event_loop_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "HID device event loop could not be created");
        bt_hidd_dev_free();
        return ret;
    }

    //[2] parse hid descriptor
    ret = bt_hidd_init_config(s_hidd_param.dev, config);
    if (ret != ESP_OK) {
        bt_hidd_dev_free();
        return ret;
    }

    //[3] configure hidd app param and qos param
    bt_hidd_init_app();
    bt_hidd_init_qos();

    //[4] implement the interface
    dev_p->dev = s_hidd_param.dev;
    dev_p->connected = esp_bt_hidd_dev_connected;
    dev_p->disconnect = esp_bt_hidd_dev_disconnect;
    dev_p->deinit = esp_bt_hidd_dev_deinit;
    dev_p->battery_set = esp_bt_hidd_dev_battery_set;
    dev_p->input_set = esp_bt_hidd_dev_input_set;
    dev_p->feature_set = esp_bt_hidd_dev_feature_set;
    dev_p->event_handler_register = esp_bt_hidd_dev_event_handler_register;
    dev_p->event_handler_unregister = esp_bt_hidd_dev_event_handler_unregister;

    ret = esp_bt_hidd_dev_event_handler_register(s_hidd_param.dev, esp_hidd_process_event_data_handler, ESP_EVENT_ANY_ID);
    if (ret != ESP_OK) {
        bt_hidd_dev_free();
        return ret;
    }

    if (callback != NULL) {
        ret = esp_bt_hidd_dev_event_handler_register(s_hidd_param.dev, callback, ESP_EVENT_ANY_ID);
        if (ret != ESP_OK) {
            bt_hidd_dev_free();
            return ret;
        }
    }

    ret = esp_bt_hid_device_register_callback(bt_hidd_cb);
    ret |= esp_bt_hid_device_init();
    if (ret != ESP_OK) {
        bt_hidd_dev_free();
        return ret;
    }

    return ret;
}
#endif /* CONFIG_BT_HID_DEVICE_ENABLED */
