/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "ble_hidh.h"
#if CONFIG_GATTC_ENABLE
#include "esp_private/esp_hidh_private.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"

#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_gap_ble_api.h"
#include "esp_hid_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

static const char *TAG = "BLE_HIDH";

static const char *s_gattc_evt_names[] = {"REG", "UNREG", "OPEN", "READ_CHAR", "WRITE_CHAR", "CLOSE", "SEARCH_CMPL", "SEARCH_RES", "READ_DESCR", "WRITE_DESCR", "NOTIFY", "PREP_WRITE", "EXEC", "ACL", "CANCEL_OPEN", "SRVC_CHG", "", "ENC_CMPL_CB", "CFG_MTU", "ADV_DATA", "MULT_ADV_ENB", "MULT_ADV_UPD", "MULT_ADV_DATA", "MULT_ADV_DIS", "CONGEST", "BTH_SCAN_ENB", "BTH_SCAN_CFG", "BTH_SCAN_RD", "BTH_SCAN_THR", "BTH_SCAN_PARAM", "BTH_SCAN_DIS", "SCAN_FLT_CFG", "SCAN_FLT_PARAM", "SCAN_FLT_STATUS", "ADV_VSC", "", "", "", "REG_FOR_NOTIFY", "UNREG_FOR_NOTIFY", "CONNECT", "DISCONNECT", "READ_MULTIPLE", "QUEUE_FULL", "SET_ASSOC", "GET_ADDR_LIST", "DIS_SRVC_CMPL"};

const char *gattc_evt_str(uint8_t event)
{
    if (event >= (sizeof(s_gattc_evt_names) / sizeof(*s_gattc_evt_names))) {
        return "UNKNOWN";
    }
    return s_gattc_evt_names[event];
}

static SemaphoreHandle_t s_ble_hidh_cb_semaphore = NULL;

static inline void WAIT_CB(void)
{
    xSemaphoreTake(s_ble_hidh_cb_semaphore, portMAX_DELAY);
}

static inline void SEND_CB(void)
{
    xSemaphoreGive(s_ble_hidh_cb_semaphore);
}

static esp_event_loop_handle_t event_loop_handle;
static uint8_t *s_read_data_val = NULL;
static uint16_t s_read_data_len = 0;
static esp_gatt_status_t s_read_status = ESP_GATT_OK;

static esp_gatt_status_t read_char(esp_gatt_if_t gattc_if, uint16_t conn_id, uint16_t handle, esp_gatt_auth_req_t auth_req, uint8_t **out, uint16_t *out_len)
{
    s_read_data_val = NULL;
    s_read_data_len = 0;
    if (esp_ble_gattc_read_char(gattc_if, conn_id, handle, auth_req) != ESP_OK) {
        ESP_LOGE(TAG, "read_char failed");
        return ESP_GATT_ERROR;
    }
    WAIT_CB();
    if (s_read_status == ESP_GATT_OK) {
        *out = s_read_data_val;
        *out_len = s_read_data_len;
    }
    return s_read_status;
}

static esp_gatt_status_t read_descr(esp_gatt_if_t gattc_if, uint16_t conn_id, uint16_t handle, esp_gatt_auth_req_t auth_req, uint8_t **out, uint16_t *out_len)
{
    s_read_data_val = NULL;
    s_read_data_len = 0;
    if (esp_ble_gattc_read_char_descr(gattc_if, conn_id, handle, auth_req) != ESP_OK) {
        ESP_LOGE(TAG, "esp_ble_gattc_read_char failed");
        return ESP_GATT_ERROR;
    }
    WAIT_CB();
    if (s_read_status == ESP_GATT_OK) {
        *out = s_read_data_val;
        *out_len = s_read_data_len;
    }
    return s_read_status;
}

static void read_device_services(esp_gatt_if_t gattc_if, esp_hidh_dev_t *dev)
{
    uint16_t suuid, cuuid, duuid;
    uint16_t chandle, dhandle;
    esp_hidh_dev_report_t *report = NULL;
    uint8_t *rdata = 0;
    uint16_t rlen = 0;
    esp_hid_report_item_t *r;
    esp_hid_report_map_t *map;

    esp_gattc_service_elem_t service_result[10];
    uint16_t dcount = 10;
    uint8_t hidindex = 0;
    if (esp_ble_gattc_get_service(gattc_if, dev->ble.conn_id, NULL, service_result, &dcount, 0) == ESP_OK) {
        ESP_LOGD(TAG, "Found %u HID Services", dev->config.report_maps_len);
        dev->config.report_maps = (esp_hid_raw_report_map_t *)malloc(dev->config.report_maps_len * sizeof(esp_hid_raw_report_map_t));
        if (dev->config.report_maps == NULL) {
            ESP_LOGE(TAG, "malloc report maps failed");
            return;
        }
        /* read characteristic value may failed, so we should init report maps */
        memset(dev->config.report_maps, 0, dev->config.report_maps_len * sizeof(esp_hid_raw_report_map_t));

        for (uint16_t s = 0; s < dcount; s++) {
            suuid = service_result[s].uuid.uuid.uuid16;
            ESP_LOGV(TAG, "SRV(%d) %s start_handle %d, end_handle %d, uuid: 0x%04x", s, service_result[s].is_primary ? " PRIMARY" : "", service_result[s].start_handle, service_result[s].end_handle, suuid);

            if (suuid != ESP_GATT_UUID_BATTERY_SERVICE_SVC
                    && suuid != ESP_GATT_UUID_DEVICE_INFO_SVC
                    && suuid != ESP_GATT_UUID_HID_SVC
                    && suuid != 0x1800) {//device name?
                continue;
            }

            esp_gattc_char_elem_t char_result[20];
            uint16_t ccount = 20;
            if (esp_ble_gattc_get_all_char(gattc_if, dev->ble.conn_id, service_result[s].start_handle, service_result[s].end_handle, char_result, &ccount, 0) == ESP_OK) {
                for (uint16_t c = 0; c < ccount; c++) {
                    cuuid = char_result[c].uuid.uuid.uuid16;
                    chandle = char_result[c].char_handle;
                    ESP_LOGV(TAG, "  CHAR:(%d), handle: %d, perm: 0x%02x, uuid: 0x%04x", c + 1, chandle, char_result[c].properties, cuuid);

                    if (suuid == 0x1800) {
                        if (dev->config.device_name == NULL && cuuid == 0x2a00 && (char_result[c].properties & ESP_GATT_CHAR_PROP_BIT_READ) != 0) {
                            if (read_char(gattc_if, dev->ble.conn_id, chandle, ESP_GATT_AUTH_REQ_NO_MITM, &rdata, &rlen) == ESP_GATT_OK && rlen) {
                                dev->config.device_name = (const char *)rdata;
                            }
                        } else {
                            continue;
                        }
                    } else if (suuid == ESP_GATT_UUID_BATTERY_SERVICE_SVC) {
                        if (cuuid == ESP_GATT_UUID_BATTERY_LEVEL && (char_result[c].properties & ESP_GATT_CHAR_PROP_BIT_READ) != 0) {
                            dev->ble.battery_handle = chandle;
                        } else {
                            continue;
                        }
                    } else if (suuid == ESP_GATT_UUID_DEVICE_INFO_SVC) {
                        if (char_result[c].properties & ESP_GATT_CHAR_PROP_BIT_READ) {
                            if (cuuid == ESP_GATT_UUID_PNP_ID) {
                                if (read_char(gattc_if, dev->ble.conn_id, chandle, ESP_GATT_AUTH_REQ_NO_MITM, &rdata, &rlen) == ESP_GATT_OK && rlen == 7) {
                                    dev->config.vendor_id = *((uint16_t *)&rdata[1]);
                                    dev->config.product_id = *((uint16_t *)&rdata[3]);
                                    dev->config.version = *((uint16_t *)&rdata[5]);
                                }
                                free(rdata);
                            } else if (cuuid == ESP_GATT_UUID_MANU_NAME) {
                                if (read_char(gattc_if, dev->ble.conn_id, chandle, ESP_GATT_AUTH_REQ_NO_MITM, &rdata, &rlen) == ESP_GATT_OK && rlen) {
                                    dev->config.manufacturer_name = (const char *)rdata;
                                }
                            } else if (cuuid == ESP_GATT_UUID_SERIAL_NUMBER_STR) {
                                if (read_char(gattc_if, dev->ble.conn_id, chandle, ESP_GATT_AUTH_REQ_NO_MITM, &rdata, &rlen) == ESP_GATT_OK && rlen) {
                                    dev->config.serial_number = (const char *)rdata;
                                }
                            }
                        }
                        continue;
                    } else {
                        if (cuuid == ESP_GATT_UUID_HID_REPORT_MAP) {
                            if (char_result[c].properties & ESP_GATT_CHAR_PROP_BIT_READ) {
                                if (read_char(gattc_if, dev->ble.conn_id, chandle, ESP_GATT_AUTH_REQ_NO_MITM, &rdata, &rlen) == ESP_GATT_OK && rlen) {
                                    dev->config.report_maps[hidindex].data = (const uint8_t *)rdata;
                                    dev->config.report_maps[hidindex].len = rlen;
                                }
                            }
                            continue;
                        } else if (cuuid == ESP_GATT_UUID_HID_BT_KB_INPUT || cuuid == ESP_GATT_UUID_HID_BT_KB_OUTPUT || cuuid == ESP_GATT_UUID_HID_BT_MOUSE_INPUT || cuuid == ESP_GATT_UUID_HID_REPORT) {
                            report = (esp_hidh_dev_report_t *)malloc(sizeof(esp_hidh_dev_report_t));
                            if (report == NULL) {
                                ESP_LOGE(TAG, "malloc esp_hidh_dev_report_t failed");
                                return;
                            }
                            report->next = NULL;
                            report->permissions = char_result[c].properties;
                            report->handle = chandle;
                            report->ccc_handle = 0;
                            report->report_id = 0;
                            report->map_index = hidindex;
                            if (cuuid == ESP_GATT_UUID_HID_BT_KB_INPUT) {
                                report->protocol_mode = ESP_HID_PROTOCOL_MODE_BOOT;
                                report->report_type = ESP_HID_REPORT_TYPE_INPUT;
                                report->usage = ESP_HID_USAGE_KEYBOARD;
                                report->value_len = 8;
                            } else if (cuuid == ESP_GATT_UUID_HID_BT_KB_OUTPUT) {
                                report->protocol_mode = ESP_HID_PROTOCOL_MODE_BOOT;
                                report->report_type = ESP_HID_REPORT_TYPE_OUTPUT;
                                report->usage = ESP_HID_USAGE_KEYBOARD;
                                report->value_len = 8;
                            } else if (cuuid == ESP_GATT_UUID_HID_BT_MOUSE_INPUT) {
                                report->protocol_mode = ESP_HID_PROTOCOL_MODE_BOOT;
                                report->report_type = ESP_HID_REPORT_TYPE_INPUT;
                                report->usage = ESP_HID_USAGE_MOUSE;
                                report->value_len = 8;
                            } else {
                                report->protocol_mode = ESP_HID_PROTOCOL_MODE_REPORT;
                                report->report_type = 0;
                                report->usage = ESP_HID_USAGE_GENERIC;
                                report->value_len = 0;
                            }
                        } else {
                            continue;
                        }
                    }
                    esp_gattc_descr_elem_t descr_result[20];
                    uint16_t dcount = 20;
                    if (esp_ble_gattc_get_all_descr(gattc_if, dev->ble.conn_id, char_result[c].char_handle, descr_result, &dcount, 0) == ESP_OK) {
                        for (uint16_t d = 0; d < dcount; d++) {
                            duuid = descr_result[d].uuid.uuid.uuid16;
                            dhandle = descr_result[d].handle;
                            ESP_LOGV(TAG, "    DESCR:(%d), handle: %d, uuid: 0x%04x", d + 1, dhandle, duuid);

                            if (suuid == ESP_GATT_UUID_BATTERY_SERVICE_SVC) {
                                if (duuid == ESP_GATT_UUID_CHAR_CLIENT_CONFIG && (char_result[c].properties & ESP_GATT_CHAR_PROP_BIT_NOTIFY) != 0) {
                                    dev->ble.battery_ccc_handle = dhandle;
                                }
                            } else if (suuid == ESP_GATT_UUID_HID_SVC && report != NULL) {
                                if (duuid == ESP_GATT_UUID_CHAR_CLIENT_CONFIG && (report->permissions & ESP_GATT_CHAR_PROP_BIT_NOTIFY) != 0) {
                                    report->ccc_handle = dhandle;
                                } else if (duuid == ESP_GATT_UUID_RPT_REF_DESCR) {
                                    if (read_descr(gattc_if, dev->ble.conn_id, dhandle, ESP_GATT_AUTH_REQ_NO_MITM, &rdata, &rlen) == ESP_GATT_OK && rlen) {
                                        report->report_id = rdata[0];
                                        report->report_type = rdata[1];
                                        free(rdata);
                                    }
                                }
                            }
                        }
                    }
                    if (suuid == ESP_GATT_UUID_HID_SVC && report != NULL) {
                        report->next = dev->reports;
                        dev->reports = report;
                        dev->reports_len++;
                    }
                }
                if (suuid == ESP_GATT_UUID_HID_SVC) {
                    hidindex++;
                }
            }
        }

        for (uint8_t d = 0; d < dev->config.report_maps_len; d++) {
            if (dev->reports_len && dev->config.report_maps[d].len) {
                map = esp_hid_parse_report_map(dev->config.report_maps[d].data, dev->config.report_maps[d].len);
                if (map) {
                    if (dev->ble.appearance == 0) {
                        dev->ble.appearance = map->appearance;
                    }
                    report = dev->reports;
                    while (report) {
                        if (report->map_index == d) {
                            for (uint8_t i = 0; i < map->reports_len; i++) {
                                r = &map->reports[i];
                                if (report->protocol_mode == ESP_HID_PROTOCOL_MODE_BOOT
                                        && report->protocol_mode == r->protocol_mode
                                        && report->report_type == r->report_type
                                        && report->usage == r->usage) {
                                    report->report_id = r->report_id;
                                    report->value_len = r->value_len;
                                } else if (report->protocol_mode == r->protocol_mode
                                           && report->report_type == r->report_type
                                           && report->report_id == r->report_id) {
                                    report->usage = r->usage;
                                    report->value_len = r->value_len;
                                }
                            }
                        }
                        report = report->next;
                    }
                    free(map->reports);
                    free(map);
                    map = NULL;
                }
            }
        }
    }
}

static void register_for_notify(esp_gatt_if_t gattc_if, esp_bd_addr_t bda, uint16_t handle)
{
    esp_ble_gattc_register_for_notify(gattc_if, bda, handle);
    WAIT_CB();
}

static void write_char_descr(esp_gatt_if_t gattc_if, uint16_t conn_id, uint16_t handle, uint16_t value_len, uint8_t *value, esp_gatt_write_type_t write_type, esp_gatt_auth_req_t auth_req)
{
    esp_ble_gattc_write_char_descr(gattc_if, conn_id, handle, value_len, value, write_type, auth_req);
    WAIT_CB();
}

static void attach_report_listeners(esp_gatt_if_t gattc_if, esp_hidh_dev_t *dev)
{
    if (dev == NULL) {
        return;
    }
    uint16_t ccc_data = 1;
    esp_hidh_dev_report_t *report = dev->reports;

    //subscribe to battery notifications
    if (dev->ble.battery_handle) {
        register_for_notify(gattc_if, dev->addr.bda, dev->ble.battery_handle);
        if (dev->ble.battery_ccc_handle) {
            //Write CCC descr to enable notifications
            write_char_descr(gattc_if, dev->ble.conn_id, dev->ble.battery_ccc_handle, 2, (uint8_t *)&ccc_data, ESP_GATT_WRITE_TYPE_NO_RSP, ESP_GATT_AUTH_REQ_NO_MITM);
        }
    }

    while (report) {
        //subscribe to notifications
        if ((report->permissions & ESP_GATT_CHAR_PROP_BIT_NOTIFY) != 0 && report->protocol_mode == ESP_HID_PROTOCOL_MODE_REPORT) {
            register_for_notify(gattc_if, dev->addr.bda, report->handle);
            if (report->ccc_handle) {
                //Write CCC descr to enable notifications
                write_char_descr(gattc_if, dev->ble.conn_id, report->ccc_handle, 2, (uint8_t *)&ccc_data, ESP_GATT_WRITE_TYPE_NO_RSP, ESP_GATT_AUTH_REQ_NO_MITM);
            }
        }
        report = report->next;
    }
}

static esp_gatt_if_t hid_gattc_if = 0;

void esp_hidh_gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    esp_ble_gattc_cb_param_t *p_data = param;
    esp_hidh_dev_t *dev = NULL;
    esp_hidh_dev_report_t *report = NULL;

    switch (event) {
    case ESP_GATTC_REG_EVT:
        if (param->reg.status == ESP_GATT_OK) {
            hid_gattc_if = gattc_if;
        } else {
            ESP_LOGE(TAG, "Reg app failed, app_id %04x, status 0x%x", param->reg.app_id,  param->reg.status);
            return;
        }
        SEND_CB();
        break;

    case ESP_GATTC_OPEN_EVT:
        ESP_LOGV(TAG, "OPEN bda " ESP_BD_ADDR_STR ", conn_id %d, status 0x%x, mtu %d", ESP_BD_ADDR_HEX(p_data->open.remote_bda), p_data->open.conn_id, p_data->open.status, p_data->open.mtu);
        dev = esp_hidh_dev_get_by_bda(p_data->open.remote_bda);
        if (!dev) {
            ESP_LOGE(TAG, "OPEN received for unknown device");
            break;
        }
        if (p_data->open.status != 0) {
            //error
            ESP_LOGE(TAG, "OPEN failed: 0x%x", p_data->open.status);
            dev->status = p_data->open.status;//ESP_GATT_CONN_FAIL_ESTABLISH;
            dev->ble.conn_id = -1;
            SEND_CB();//return from open
        } else {
            dev->status = ESP_GATT_NOT_FOUND;//set to not found and clear if HID service is found
            dev->ble.conn_id = p_data->open.conn_id;
            esp_ble_gattc_search_service(gattc_if, dev->ble.conn_id, NULL);
        }
        break;

    case ESP_GATTC_SEARCH_RES_EVT:
        dev = esp_hidh_dev_get_by_conn_id(p_data->search_res.conn_id);
        if (!dev) {
            ESP_LOGE(TAG, "SEARCH_RES received for unknown device");
            break;
        }
        if (p_data->search_res.srvc_id.uuid.uuid.uuid16 == ESP_GATT_UUID_HID_SVC) {
            dev->status = ESP_GATT_OK;
            dev->config.report_maps_len++;
            ESP_LOGV(TAG, "SEARCH_RES HID Service was found");
        }
        break;

    case ESP_GATTC_SEARCH_CMPL_EVT:
        dev = esp_hidh_dev_get_by_conn_id(p_data->search_cmpl.conn_id);
        if (!dev) {
            ESP_LOGE(TAG, "SEARCH_CMPL received for unknown device");
            break;
        }
        if (dev->status == ESP_GATT_NOT_FOUND) {
            //service not found
            ESP_LOGE(TAG, "SEARCH_CMPL HID Service was not found on the device");
            dev->status = ESP_GATT_CONN_NONE;
        } else if (p_data->search_cmpl.status) {
            //error
            dev->status = p_data->search_cmpl.status;
        }
        if (dev->status) {
            esp_ble_gattc_close(gattc_if, dev->ble.conn_id);
            dev->ble.conn_id = -1;
        }
        dev->connected = true;
        SEND_CB();//return from open
        break;

    case ESP_GATTC_READ_CHAR_EVT:
    case ESP_GATTC_READ_DESCR_EVT: {
        dev = esp_hidh_dev_get_by_conn_id(p_data->read.conn_id);
        if (!dev) {
            ESP_LOGE(TAG, "READ received for unknown device");
            break;
        }
        dev->status = p_data->read.status;
        s_read_status = p_data->read.status;
        s_read_data_len = 0;
        s_read_data_val = NULL;
        if (s_read_status == 0 && p_data->read.value_len > 0) {
            s_read_data_len = p_data->read.value_len;
            s_read_data_val = (uint8_t *)malloc(s_read_data_len + 1);
            if (s_read_data_val) {
                memcpy(s_read_data_val, p_data->read.value, s_read_data_len);
                s_read_data_val[s_read_data_len] = 0;
            }
        }
        SEND_CB();
        break;
    }

    case ESP_GATTC_WRITE_DESCR_EVT: {
        dev = esp_hidh_dev_get_by_conn_id(p_data->write.conn_id);
        if (!dev) {
            ESP_LOGE(TAG, "WRITE_DESCR received for unknown device");
            break;
        }
        dev->status = p_data->write.status;
        SEND_CB();
        break;
    }

    case ESP_GATTC_WRITE_CHAR_EVT: {
        dev = esp_hidh_dev_get_by_conn_id(p_data->write.conn_id);
        if (!dev) {
            ESP_LOGE(TAG, "WRITE_CHAR received for unknown device");
            break;
        }
        dev->status = p_data->write.status;
        if (p_data->write.status) {
            ESP_LOGE(TAG, "WRITE_CHAR: conn_id %d, handle %d, status 0x%x", p_data->write.conn_id, p_data->write.handle, p_data->write.status);
        }
        break;
    }

    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
        SEND_CB();
        break;
    }

    case ESP_GATTC_DISCONNECT_EVT: {
        ESP_LOGV(TAG, "DISCONNECT: bda " ESP_BD_ADDR_STR ", conn_id %u, reason 0x%x", ESP_BD_ADDR_HEX(p_data->disconnect.remote_bda), p_data->disconnect.conn_id, p_data->disconnect.reason);
        break;
    }

    case ESP_GATTC_NOTIFY_EVT: {
        dev = esp_hidh_dev_get_by_conn_id(p_data->notify.conn_id);
        if (!dev) {
            ESP_LOGE(TAG, "NOTIFY received for unknown device");
            break;
        }
        if (event_loop_handle) {
            esp_hidh_event_data_t p = {0};
            if (p_data->notify.handle == dev->ble.battery_handle) {
                p.battery.dev = dev;
                p.battery.level = p_data->notify.value[0];
                esp_event_post_to(event_loop_handle, ESP_HIDH_EVENTS, ESP_HIDH_BATTERY_EVENT, &p, sizeof(esp_hidh_event_data_t), portMAX_DELAY);
            } else {
                report = esp_hidh_dev_get_report_by_handle(dev, p_data->notify.handle);
                if (report) {
                    esp_hidh_event_data_t *p_param = NULL;
                    size_t event_data_size = sizeof(esp_hidh_event_data_t);

                    if (p_data->notify.value_len && p_data->notify.value) {
                        event_data_size += p_data->notify.value_len;
                    }

                    if ((p_param = (esp_hidh_event_data_t *)malloc(event_data_size)) == NULL) {
                        ESP_LOGE(TAG, "%s malloc event data failed!", __func__);
                        break;
                    }
                    memset(p_param, 0, event_data_size);
                    if (p_data->notify.value_len && p_data->notify.value) {
                        memcpy(((uint8_t *)p_param) + sizeof(esp_hidh_event_data_t), p_data->notify.value,
                               p_data->notify.value_len);
                    }

                    if (report->report_type == ESP_HID_REPORT_TYPE_FEATURE) {
                        p_param->feature.dev = dev;
                        p_param->feature.map_index = report->map_index;
                        p_param->feature.report_id = report->report_id;
                        p_param->feature.usage = report->usage;
                        p_param->feature.length = p_data->notify.value_len;
                        p_param->feature.data = p_data->notify.value;
                        esp_event_post_to(event_loop_handle, ESP_HIDH_EVENTS, ESP_HIDH_FEATURE_EVENT, p_param, event_data_size, portMAX_DELAY);
                    } else {
                        p_param->input.dev = dev;
                        p_param->input.map_index = report->map_index;
                        p_param->input.report_id = report->report_id;
                        p_param->input.usage = report->usage;
                        p_param->input.length = p_data->notify.value_len;
                        p_param->input.data = p_data->notify.value;
                        esp_event_post_to(event_loop_handle, ESP_HIDH_EVENTS, ESP_HIDH_INPUT_EVENT, p_param, event_data_size, portMAX_DELAY);
                    }

                    if (p_param) {
                        free(p_param);
                        p_param = NULL;
                    }
                }
            }
        }
        break;
    }

    case ESP_GATTC_CLOSE_EVT: {
        ESP_LOGV(TAG, "CLOSE bda " ESP_BD_ADDR_STR ", conn_id %d, status 0x%x, reason 0x%x", ESP_BD_ADDR_HEX(p_data->close.remote_bda), p_data->close.conn_id, p_data->close.status, p_data->close.reason);
        dev = esp_hidh_dev_get_by_bda(p_data->open.remote_bda);
        if (!dev) {
            ESP_LOGE(TAG, "CLOSE received for unknown device");
            break;
        }
        if (!dev->connected) {
            dev->status = p_data->close.reason;
            dev->ble.conn_id = -1;
            SEND_CB();//return from open
        } else {
            dev->connected = false;
            dev->status = p_data->close.status;
            // free the device in the wrapper event handler
            dev->in_use = false;
            if (event_loop_handle) {
                esp_hidh_event_data_t p = {0};
                p.close.dev = dev;
                p.close.reason = p_data->close.reason;
                p.close.status = ESP_OK;
                esp_event_post_to(event_loop_handle, ESP_HIDH_EVENTS, ESP_HIDH_CLOSE_EVENT, &p, sizeof(esp_hidh_event_data_t), portMAX_DELAY);
            } else {
                esp_hidh_dev_free_inner(dev);
            }
        }
        break;
    }

    default:
        ESP_LOGV(TAG, "GATTC EVENT %s", gattc_evt_str(event));
        break;
    }
}

/*
 * Public Functions
 * */

static esp_err_t esp_ble_hidh_dev_close(esp_hidh_dev_t *dev)
{
    return esp_ble_gattc_close(hid_gattc_if, dev->ble.conn_id);
}

static esp_err_t esp_ble_hidh_dev_report_write(esp_hidh_dev_t *dev, size_t map_index, size_t report_id, int report_type, uint8_t *value, size_t value_len)
{
    esp_hidh_dev_report_t *report = esp_hidh_dev_get_report_by_id_and_type(dev, map_index, report_id, report_type);
    if (!report) {
        ESP_LOGE(TAG, "%s report %d not found", esp_hid_report_type_str(report_type), report_id);
        return ESP_FAIL;
    }
    if (value_len > report->value_len) {
        ESP_LOGE(TAG, "%s report %d takes maximum %d bytes. you have provided %d", esp_hid_report_type_str(report_type), report_id, report->value_len, value_len);
        return ESP_FAIL;
    }
    return esp_ble_gattc_write_char(hid_gattc_if, dev->ble.conn_id, report->handle, value_len, value, ESP_GATT_WRITE_TYPE_RSP, ESP_GATT_AUTH_REQ_NO_MITM);
}

static esp_err_t esp_ble_hidh_dev_report_read(esp_hidh_dev_t *dev, size_t map_index, size_t report_id, int report_type, size_t max_length, uint8_t *value, size_t *value_len)
{
    esp_hidh_dev_report_t *report = esp_hidh_dev_get_report_by_id_and_type(dev, map_index, report_id, report_type);
    if (!report) {
        ESP_LOGE(TAG, "%s report %d not found", esp_hid_report_type_str(report_type), report_id);
        return ESP_FAIL;
    }
    uint16_t len = max_length;
    uint8_t *v = NULL;
    esp_gatt_status_t s = read_char(hid_gattc_if, dev->ble.conn_id, report->handle, ESP_GATT_AUTH_REQ_NO_MITM, &v, &len);
    if (s == ESP_GATT_OK) {
        if (len > max_length) {
            len = max_length;
        }
        *value_len = len;
        memcpy(value, v, len);
        return ESP_OK;
    }
    ESP_LOGE(TAG, "%s report %d read failed: 0x%x", esp_hid_report_type_str(report_type), report_id, s);
    return ESP_FAIL;
}

static void esp_ble_hidh_dev_dump(esp_hidh_dev_t *dev, FILE *fp)
{
    fprintf(fp, "BDA:" ESP_BD_ADDR_STR ", Appearance: 0x%04x, Connection ID: %d\n", ESP_BD_ADDR_HEX(dev->addr.bda), dev->ble.appearance, dev->ble.conn_id);
    fprintf(fp, "Name: %s, Manufacturer: %s, Serial Number: %s\n", dev->config.device_name ? dev->config.device_name : "", dev->config.manufacturer_name ? dev->config.manufacturer_name : "", dev->config.serial_number ? dev->config.serial_number : "");
    fprintf(fp, "PID: 0x%04x, VID: 0x%04x, VERSION: 0x%04x\n", dev->config.product_id, dev->config.vendor_id, dev->config.version);
    fprintf(fp, "Battery: Handle: %u, CCC Handle: %u\n", dev->ble.battery_handle, dev->ble.battery_ccc_handle);
    fprintf(fp, "Report Maps: %d\n", dev->config.report_maps_len);
    for (uint8_t d = 0; d < dev->config.report_maps_len; d++) {
        fprintf(fp, "  Report Map Length: %d\n", dev->config.report_maps[d].len);
        esp_hidh_dev_report_t *report = dev->reports;
        while (report) {
            if (report->map_index == d) {
                fprintf(fp, "    %8s %7s %6s, ID: %2u, Length: %3u, Permissions: 0x%02x, Handle: %3u, CCC Handle: %3u\n",
                        esp_hid_usage_str(report->usage), esp_hid_report_type_str(report->report_type), esp_hid_protocol_mode_str(report->protocol_mode),
                        report->report_id, report->value_len, report->permissions, report->handle, report->ccc_handle);
            }
            report = report->next;
        }
    }

}

esp_err_t esp_ble_hidh_init(const esp_hidh_config_t *config)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(config, ESP_ERR_INVALID_ARG, TAG, "Config is NULL");
    ESP_RETURN_ON_FALSE(!s_ble_hidh_cb_semaphore, ESP_ERR_INVALID_STATE, TAG, "Already initialized");

    event_loop_handle = esp_hidh_get_event_loop();
    s_ble_hidh_cb_semaphore = xSemaphoreCreateBinary();
    ESP_RETURN_ON_FALSE(s_ble_hidh_cb_semaphore,
                        ESP_ERR_NO_MEM, TAG, "Allocation failed");

    ESP_GOTO_ON_ERROR(
        esp_ble_gattc_app_register(0),
        gattc_fail, TAG, "esp_ble_gattc_app_register failed!");
    WAIT_CB();
    return ret;

gattc_fail:
    if (s_ble_hidh_cb_semaphore) {
        vSemaphoreDelete(s_ble_hidh_cb_semaphore);
        s_ble_hidh_cb_semaphore = NULL;
    }
    return ret;
}

esp_err_t esp_ble_hidh_deinit(void)
{
    ESP_RETURN_ON_FALSE(s_ble_hidh_cb_semaphore, ESP_ERR_INVALID_STATE, TAG, "Already deinitialized");
    ESP_RETURN_ON_ERROR(
        esp_ble_gattc_app_unregister(hid_gattc_if),
        TAG, "App Unregister Failed");

    if (s_ble_hidh_cb_semaphore) {
        vSemaphoreDelete(s_ble_hidh_cb_semaphore);
        s_ble_hidh_cb_semaphore = NULL;
    }

    return ESP_OK;

}

esp_hidh_dev_t *esp_ble_hidh_dev_open(esp_bd_addr_t bda, esp_ble_addr_type_t address_type)
{
    esp_err_t ret;

    esp_hidh_dev_t *dev = esp_hidh_dev_malloc();
    if (dev == NULL) {
        ESP_LOGE(TAG, "malloc esp_hidh_dev_t failed");
        return NULL;
    }

    dev->in_use = true;
    dev->transport = ESP_HID_TRANSPORT_BLE;
    memcpy(dev->addr.bda, bda, sizeof(esp_bd_addr_t));
    dev->ble.address_type = address_type;
    dev->ble.appearance = ESP_HID_APPEARANCE_GENERIC;

    esp_ble_gatt_creat_conn_params_t creat_conn_params = {0};
    memcpy(&creat_conn_params.remote_bda, dev->addr.bda, ESP_BD_ADDR_LEN);
    creat_conn_params.remote_addr_type = dev->ble.address_type;
    creat_conn_params.own_addr_type = BLE_ADDR_TYPE_PUBLIC;
    creat_conn_params.is_direct = true;
    creat_conn_params.is_aux = false;
    creat_conn_params.phy_mask = 0x0;
    ret = esp_ble_gattc_enh_open(hid_gattc_if,
                                 &creat_conn_params);
    if (ret) {
        esp_hidh_dev_free_inner(dev);
        ESP_LOGE(TAG, "esp_ble_gattc_enh_open failed: %d", ret);
        return NULL;
    }
    WAIT_CB();
    if (dev->ble.conn_id < 0) {
        ret = dev->status;
        ESP_LOGE(TAG, "dev open failed! status: 0x%x", dev->status);
        esp_hidh_dev_free_inner(dev);
        return NULL;
    }

    dev->close = esp_ble_hidh_dev_close;
    dev->report_write = esp_ble_hidh_dev_report_write;
    dev->report_read = esp_ble_hidh_dev_report_read;
    dev->dump = esp_ble_hidh_dev_dump;

    read_device_services(hid_gattc_if, dev);

    if (event_loop_handle) {
        esp_hidh_event_data_t p = {0};
        p.open.status = ESP_OK;
        p.open.dev = dev;
        esp_event_post_to(event_loop_handle, ESP_HIDH_EVENTS, ESP_HIDH_OPEN_EVENT, &p, sizeof(esp_hidh_event_data_t), portMAX_DELAY);
    }

    attach_report_listeners(hid_gattc_if, dev);
    return dev;
}

#endif /* CONFIG_GATTC_ENABLE */
