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

#include "bt_hidh.h"
#if CONFIG_BT_HID_HOST_ENABLED
#include "esp_hidh_private.h"
#include <string.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_bt_hh_api.h"

static const char *TAG = "BT_HIDH";

static esp_event_loop_handle_t event_loop_handle;

static const char *s_bta_hh_evt_names[] = {"ENABLE", "DISABLE", "OPEN", "CLOSE", "GET_RPT", "SET_RPT", "GET_PROTO", "SET_PROTO", "GET_IDLE", "SET_IDLE", "GET_DSCP", "ADD_DEV", "RMV_DEV", "VC_UNPLUG", "DATA", "API_ERR", "UPDATE_SCPP"};
static const char *s_bta_hh_status_names[] = {"OK", "HS_HID_NOT_READY", "HS_INVALID_RPT_ID", "HS_TRANS_NOT_SPT", "HS_INVALID_PARAM", "HS_ERROR", "ERR", "ERR_SDP", "ERR_PROTO", "ERR_DB_FULL", "ERR_TOD_UNSPT", "ERR_NO_RES", "ERR_AUTH_FAILED", "ERR_HDL", "ERR_SEC"};

static inline void WAIT_DEV(esp_hidh_dev_t *dev)
{
    xSemaphoreTake(dev->semaphore, portMAX_DELAY);
}

static inline void SEND_DEV(esp_hidh_dev_t *dev)
{
    xSemaphoreGive(dev->semaphore);
}

static void bta_hh_cb(tBTA_HH_EVT event, tBTA_HH *p_data)
{
    static esp_hidh_dev_t *descr_dev = NULL;
    esp_hidh_dev_t *dev = NULL;
    switch (event) {
    case BTA_HH_ENABLE_EVT: {
        if (p_data->status) {
            ESP_LOGE(TAG, "ENABLE ERROR: %s", s_bta_hh_status_names[p_data->status]);
        }
    } break;
    case BTA_HH_OPEN_EVT: {
        dev = esp_hidh_dev_get_by_handle(p_data->conn.handle);
        if (dev == NULL) {
            ESP_LOGE(TAG, "OPEN ERROR: Device Not Found");
            return;
        }
        dev->status = p_data->conn.status;
        memcpy(dev->bda, p_data->conn.bda, sizeof(esp_bd_addr_t));
        if (dev->status == BTA_HH_OK) {
            descr_dev = dev;
            BTA_HhGetDscpInfo(dev->bt.handle);
        } else {
            ESP_LOGE(TAG, "OPEN ERROR: %s", s_bta_hh_status_names[dev->status]);
            if (dev->opened) {
                SEND_DEV(dev);
            } else {
                esp_hidh_dev_free(dev);
            }
        }

    } break;
    case BTA_HH_GET_DSCP_EVT: {
        ESP_LOGV(TAG, "DESCRIPTOR: PID: 0x%04x, VID: 0x%04x, VERSION: 0x%04x, REPORT_LEN: %u", p_data->dscp_info.product_id, p_data->dscp_info.vendor_id, p_data->dscp_info.version, p_data->dscp_info.descriptor.dl_len);
        if (descr_dev == NULL) {
            ESP_LOGE(TAG, "Device Not Found");
            return;
        }
        dev = descr_dev;
        dev->config.product_id = p_data->dscp_info.product_id;
        dev->config.vendor_id = p_data->dscp_info.vendor_id;
        dev->config.version = p_data->dscp_info.version;


        dev->config.report_maps_len = 1;
        dev->config.report_maps = (esp_hid_raw_report_map_t *)malloc(dev->config.report_maps_len * sizeof(esp_hid_raw_report_map_t));
        if (dev->config.report_maps == NULL) {
            ESP_LOGE(TAG, "malloc report maps failed");
            return;
        }

        dev->config.report_maps[0].data = (uint8_t *)malloc(p_data->dscp_info.descriptor.dl_len);
        if (dev->config.report_maps[0].data == NULL) {
            ESP_LOGE(TAG, "Malloc Report Map Failed");
            dev->status = BTA_HH_ERR_NO_RES;
        } else {
            dev->config.report_maps[0].len = p_data->dscp_info.descriptor.dl_len;
            memcpy((uint8_t *)dev->config.report_maps[0].data, p_data->dscp_info.descriptor.dsc_list, dev->config.report_maps[0].len);
            //generate reports

            if (dev->config.report_maps[0].len && dev->config.report_maps[0].data) {
                esp_hid_report_map_t *map;
                esp_hidh_dev_report_t *report;
                esp_hid_report_item_t *r;
                map = esp_hid_parse_report_map(dev->config.report_maps[0].data, dev->config.report_maps[0].len);
                if (map) {
                    if (dev->usage == 0) {
                        dev->usage = map->usage;
                    }
                    dev->connected = true;
                    dev->reports = NULL;
                    for (uint8_t i = 0; i < map->reports_len; i++) {
                        r = &map->reports[i];
                        report = (esp_hidh_dev_report_t *)malloc(sizeof(esp_hidh_dev_report_t));
                        if (report == NULL) {
                            ESP_LOGE(TAG, "Malloc Report Failed");
                            dev->status = BTA_HH_ERR_NO_RES;
                            dev->connected = false;
                            break;
                        }
                        report->map_index = 0;
                        report->protocol_mode = r->protocol_mode;
                        report->report_type = r->report_type;
                        report->report_id = r->report_id;
                        report->value_len = r->value_len;
                        report->usage = r->usage;
                        report->next = dev->reports;
                        dev->reports = report;
                    }
                    dev->reports_len = map->reports_len;
                    free(map->reports);
                    free(map);
                    map = NULL;
                } else {
                    ESP_LOGE(TAG, "Parse Report Map Failed");
                    dev->status = BTA_HH_ERR;
                }
            }

        }
        descr_dev = NULL;
        if (dev->status == BTA_HH_OK) {
            BTA_HhAddDev(dev->bda, dev->bt.attr_mask, dev->bt.sub_class, dev->bt.app_id, p_data->dscp_info);
        } else {
            ESP_LOGE(TAG, "Read Report Map Failed, status: %s", s_bta_hh_status_names[dev->status]);
            if (dev->opened) {
                SEND_DEV(dev);
            } else {
                esp_hidh_dev_free(dev);
            }
        }
    } break;
    case BTA_HH_ADD_DEV_EVT: {
        ESP_LOGV(TAG, "ADD_DEV: BDA: " ESP_BD_ADDR_STR ", handle: %d, status: %s", ESP_BD_ADDR_HEX(p_data->dev_info.bda), p_data->dev_info.handle, s_bta_hh_status_names[p_data->dev_info.status]);
        dev = esp_hidh_dev_get_by_handle(p_data->conn.handle);
        if (dev == NULL) {
            ESP_LOGE(TAG, "Device Not Found");
            return;
        }
        dev->status = p_data->conn.status;
        if (dev->status == BTA_HH_OK) {
            esp_hidh_event_data_t p;
            p.open.dev = dev;
            esp_event_post_to(event_loop_handle, ESP_HIDH_EVENTS, ESP_HIDH_OPEN_EVENT, &p, sizeof(esp_hidh_event_data_t), portMAX_DELAY);
        } else {
            ESP_LOGE(TAG, "Device Add Failed, status: %s", s_bta_hh_status_names[dev->status]);
        }
        if (dev->opened) {
            SEND_DEV(dev);
        } else if (dev->status != BTA_HH_OK) {
            esp_hidh_dev_free(dev);
        }
    } break;
    case BTA_HH_CLOSE_EVT: {
        ESP_LOGV(TAG, "CLOSE: handle: %d, status: %s", p_data->dev_status.handle, s_bta_hh_status_names[p_data->dev_status.status]);
        dev = esp_hidh_dev_get_by_handle(p_data->dev_status.handle);
        if (dev == NULL) {
            ESP_LOGE(TAG, "Device Not Found");
            return;
        }
        dev->status = p_data->dev_status.status;
        esp_hidh_event_data_t p;
        p.close.dev = dev;
        p.close.reason = 0;
        esp_event_post_to(event_loop_handle, ESP_HIDH_EVENTS, ESP_HIDH_CLOSE_EVENT, &p, sizeof(esp_hidh_event_data_t), portMAX_DELAY);
    } break;
    case BTA_HH_SET_RPT_EVT: {
        dev = esp_hidh_dev_get_by_handle(p_data->dev_status.handle);
        if (dev == NULL) {
            ESP_LOGE(TAG, "SET_RPT ERROR: hDevice Not Found");
            return;
        }
        if (p_data->dev_status.status) {
            ESP_LOGE(TAG, "SET_RPT ERROR: handle: %d, status: %s", p_data->dev_status.handle, s_bta_hh_status_names[p_data->dev_status.status]);
        }
        dev->status = p_data->dev_status.status;
        SEND_DEV(dev);
    } break;
    case BTA_HH_GET_RPT_EVT: {
        dev = esp_hidh_dev_get_by_handle(p_data->hs_data.handle);
        if (dev == NULL) {
            ESP_LOGE(TAG, "Device Not Found");
            return;
        }
        if (p_data->hs_data.status) {
            ESP_LOGE(TAG, "GET_RPT ERROR: handle: %d, status: %s", p_data->hs_data.handle, s_bta_hh_status_names[p_data->hs_data.status]);
        }
        dev->status = p_data->hs_data.status;
        BT_HDR *rpt = p_data->hs_data.rsp_data.p_rpt_data;
        dev->tmp = rpt->data + rpt->offset;
        dev->tmp_len = rpt->len;
        SEND_DEV(dev);
    } break;
    default:
        ESP_LOGV(TAG, "BTA_HH EVENT: %s", s_bta_hh_evt_names[event]);
        break;
    }
}

/*
 * Public Functions
 * */

static esp_err_t esp_bt_hidh_dev_close(esp_hidh_dev_t *dev)
{
    BTA_HhClose(dev->bt.handle);
    return ESP_OK;
}

static esp_err_t esp_bt_hidh_dev_report_write(esp_hidh_dev_t *dev, size_t map_index, size_t report_id, int report_type, uint8_t *data, size_t len)
{
    esp_hidh_dev_report_t *report = esp_hidh_dev_get_report_by_id_and_type(dev, map_index, report_id, report_type);
    if (!report) {
        ESP_LOGE(TAG, "%s report %d not found", esp_hid_report_type_str(report_type), report_id);
        return ESP_FAIL;
    }
    if (len > report->value_len) {
        ESP_LOGE(TAG, "%s report %d takes maximum %d bytes. you have provided %d", esp_hid_report_type_str(report_type), report_id, report->value_len, len);
        return ESP_FAIL;
    }

    uint8_t *pbuf_data;
    BT_HDR *p_buf = (BT_HDR *)malloc((uint16_t) (len + 14 + sizeof(BT_HDR)));

    if (p_buf != NULL) {
        p_buf->len = len + 1;
        p_buf->offset = 14;

        pbuf_data = (uint8_t *) (p_buf + 1) + p_buf->offset;
        pbuf_data[0] = report_id;
        memcpy(pbuf_data + 1, data, len);

        if (report_type == ESP_HID_REPORT_TYPE_OUTPUT) {
            p_buf->layer_specific = BTA_HH_RPTT_OUTPUT;
            BTA_HhSendData(dev->bt.handle, dev->bda, p_buf);
        } else {
            BTA_HhSetReport(dev->bt.handle, report_type, p_buf);
            WAIT_DEV(dev);
        }
        if (dev->status) {
            ESP_LOGE(TAG, "Write %s: %s", esp_hid_report_type_str(report_type), s_bta_hh_status_names[dev->status]);
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}

static esp_err_t esp_bt_hidh_dev_report_read(esp_hidh_dev_t *dev, size_t map_index, size_t report_id, int report_type, size_t max_length, uint8_t *value, size_t *value_len)
{
    esp_hidh_dev_report_t *report = esp_hidh_dev_get_report_by_id_and_type(dev, map_index, report_id, report_type);
    if (!report) {
        ESP_LOGE(TAG, "%s report %d not found", esp_hid_report_type_str(report_type), report_id);
        return ESP_FAIL;
    }
    BTA_HhGetReport(dev->bt.handle, report_type, report_id, max_length);
    if (xSemaphoreTake(dev->semaphore, 500 / portTICK_PERIOD_MS) != pdTRUE) {
        ESP_LOGE(TAG, "Read Timeout %s", esp_hid_report_type_str(report_type));
        return ESP_FAIL;
    }
    if (dev->status) {
        ESP_LOGE(TAG, "Read %s: %s", esp_hid_report_type_str(report_type), s_bta_hh_status_names[dev->status]);
        return ESP_FAIL;
    }
    if (report_id) {
        dev->tmp++;
        dev->tmp_len--;
    }
    if (dev->tmp_len > max_length) {
        dev->tmp_len = max_length;
    }
    *value_len = dev->tmp_len;
    memcpy(value, dev->tmp, dev->tmp_len);
    return ESP_OK;
}

static void esp_bt_hidh_dev_dump(esp_hidh_dev_t *dev, FILE *fp)
{
    fprintf(fp, "BDA:" ESP_BD_ADDR_STR ", Status: %s, Connected: %s, Handle: %d, Usage: %s\n", ESP_BD_ADDR_HEX(dev->bda), s_bta_hh_status_names[dev->status], dev->connected ? "YES" : "NO", dev->bt.handle, esp_hid_usage_str(dev->usage));
    fprintf(fp, "Name: %s, Manufacturer: %s, Serial Number: %s\n", dev->config.device_name ? dev->config.device_name : "", dev->config.manufacturer_name ? dev->config.manufacturer_name : "", dev->config.serial_number ? dev->config.serial_number : "");
    fprintf(fp, "PID: 0x%04x, VID: 0x%04x, VERSION: 0x%04x\n", dev->config.product_id, dev->config.vendor_id, dev->config.version);
    fprintf(fp, "Report Map Length: %d\n", dev->config.report_maps[0].len);
    esp_hidh_dev_report_t *report = dev->reports;
    while (report) {
        fprintf(fp, "  %8s %7s %6s, ID: %3u, Length: %3u\n",
               esp_hid_usage_str(report->usage), esp_hid_report_type_str(report->report_type), esp_hid_protocol_mode_str(report->protocol_mode),
               report->report_id, report->value_len);
        report = report->next;
    }
}

esp_err_t esp_bt_hidh_init(const esp_hidh_config_t *config)
{
    if (config == NULL) {
        ESP_LOGE(TAG, "Config is NULL");
        return ESP_FAIL;
    }
    esp_event_loop_args_t event_task_args = {
        .queue_size = 5,
        .task_name = "esp_bt_hidh_events",
        .task_priority = uxTaskPriorityGet(NULL),
        .task_stack_size = 2048,
        .task_core_id = tskNO_AFFINITY
    };
    esp_err_t ret = esp_event_loop_create(&event_task_args, &event_loop_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_event_loop_create failed!");
        return ret;
    }
    esp_event_handler_register_with(event_loop_handle, ESP_HIDH_EVENTS, ESP_EVENT_ANY_ID, config->callback, NULL);
    BTA_HhEnable(0, bta_hh_cb);
    return ESP_OK;
}

esp_err_t esp_bt_hidh_deinit(void)
{
    if (event_loop_handle) {
        esp_event_loop_delete(event_loop_handle);
    }
    BTA_HhDisable();
    return ESP_OK;
}

esp_hidh_dev_t *esp_bt_hidh_dev_open(esp_bd_addr_t bda)
{
    esp_hidh_dev_t *dev = esp_hidh_dev_malloc();
    if (dev == NULL) {
        ESP_LOGE(TAG, "malloc esp_hidh_dev_t failed");
        return NULL;
    }

    dev->transport = ESP_HID_TRANSPORT_BT;
    memcpy(dev->bda, bda, sizeof(esp_bd_addr_t));
    dev->bt.handle = -1;

    dev->opened = true;
    BTA_HhOpen(dev->bda, 0, BTA_HH_PROTO_RPT_MODE, (BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT));
    WAIT_DEV(dev);
    if (dev->status != BTA_HH_OK) {
        esp_hidh_dev_free(dev);
        return NULL;
    }
    dev->close = esp_bt_hidh_dev_close;
    dev->report_write = esp_bt_hidh_dev_report_write;
    dev->report_read = esp_bt_hidh_dev_report_read;
    dev->dump = esp_bt_hidh_dev_dump;
    return dev;
}

/*
 * BlueDroid BT HIDH Stack Callbacks
 * */

/* This callback function is executed by BTA_HH when data is received on an interrupt channel. */
void bta_hh_co_data(uint8_t handle, uint8_t *p_rpt, uint16_t len, tBTA_HH_PROTO_MODE  mode, uint8_t sub_class, uint8_t country_code, esp_bd_addr_t bda, uint8_t app_id)
{
    if (len < 2) {
        ESP_LOGE(TAG, "Not Enough Data");
        return;
    }
    esp_hidh_dev_t *dev = NULL;
    esp_hidh_dev_report_t *report = NULL;
    dev = esp_hidh_dev_get_by_handle(handle);
    if (dev == NULL) {
        ESP_LOGE(TAG, "Device Not Found: handle %u", handle);
        return;
    }
    report = esp_hidh_dev_get_input_report_by_id_and_proto(dev, p_rpt[0], mode ? ESP_HID_PROTOCOL_MODE_BOOT : ESP_HID_PROTOCOL_MODE_REPORT);
    if (report == NULL) {
        ESP_LOGE(TAG, "Report Not Found: %d mode: %s", p_rpt[0], mode ? "BOOT" : "REPORT");
        return;
    }
    if (len != (report->value_len + 1)) {
        ESP_LOGW(TAG, "Wrong Data Len: %u != %u", len, (report->value_len + 1));
    }

    if (event_loop_handle) {
        esp_hidh_event_data_t p = {0};
        if (report->report_type == ESP_HID_REPORT_TYPE_FEATURE) {
            p.feature.dev = dev;
            p.feature.report_id = report->report_id;
            p.feature.usage = report->usage;
            p.feature.data = p_rpt + 1;
            p.feature.length = len - 1;
            esp_event_post_to(event_loop_handle, ESP_HIDH_EVENTS, ESP_HIDH_FEATURE_EVENT, &p, sizeof(esp_hidh_event_data_t), portMAX_DELAY);
        } else {
            p.input.dev = dev;
            p.input.report_id = report->report_id;
            p.input.usage = report->usage;
            p.input.data = p_rpt + 1;
            p.input.length = len - 1;
            esp_event_post_to(event_loop_handle, ESP_HIDH_EVENTS, ESP_HIDH_INPUT_EVENT, &p, sizeof(esp_hidh_event_data_t), portMAX_DELAY);
        }
    }
}

/* This callback function is executed by BTA_HH when connection is opened, and application may do some device specific initialization. */
void bta_hh_co_open(uint8_t handle, uint8_t sub_class, uint16_t attr_mask, uint8_t app_id)
{
    esp_hidh_dev_t *dev = NULL;
    dev = esp_hidh_dev_get_by_handle(-1);
    if (dev == NULL) {
        ESP_LOGI(TAG, "Device Not Found? It's probably a reconnect.");
        dev = esp_hidh_dev_malloc();
        if (dev == NULL) {
            ESP_LOGE(TAG, "DEV Malloc Failed");
            return;
        }
        dev->transport = ESP_HID_TRANSPORT_BT;
        dev->close = esp_bt_hidh_dev_close;
        dev->report_write = esp_bt_hidh_dev_report_write;
        dev->report_read = esp_bt_hidh_dev_report_read;
        dev->dump = esp_bt_hidh_dev_dump;
    }
    dev->bt.attr_mask = attr_mask;
    dev->bt.app_id = app_id;
    dev->bt.sub_class = sub_class;
    dev->bt.handle = handle;
}

/* This callback function is executed by BTA_HH when connection is closed, and device specific finalization may be needed. */
void bta_hh_co_close(uint8_t dev_handle, uint8_t app_id) {}

#endif /* CONFIG_BT_HID_HOST_ENABLED */
