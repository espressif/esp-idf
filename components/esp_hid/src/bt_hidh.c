/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bt_hidh.h"
#if CONFIG_BT_HID_HOST_ENABLED
#include "esp_private/esp_hidh_private.h"
#include <string.h>
#include <stdbool.h>
#include "esp_check.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "osi/fixed_queue.h"
#include "string.h"
#include "esp_hidh_api.h"

static const char *TAG = "BT_HIDH";

// element of connection queue
typedef struct {
    esp_hidh_dev_t* dev;
} conn_item_t;

typedef struct {
    fixed_queue_t *connection_queue; /* Queue of connection */
    esp_event_loop_handle_t event_loop_handle;
} hidh_local_param_t;

static hidh_local_param_t hidh_local_param;
#define TRANS_TO 1000000 // us
#define is_init() (hidh_local_param.event_loop_handle != NULL)

#define get_protocol_mode(mode) (mode) ? "REPORT" : "BOOT"
static const char *s_esp_hh_evt_names[] = {"INIT", "DEINIT", "OPEN", "CLOSE", "GET_RPT", "SET_RPT", "GET_PROTO", "SET_PROTO", "GET_IDLE", "SET_IDLE", "GET_DSCP", "ADD_DEV", "RMV_DEV", "VC_UNPLUG", "DATA", "DATA_IND", "SET_INFO"};
static const char *s_esp_hh_status_names[] = {"OK",
                                              "HS_HID_NOT_READY",
                                              "HS_INVALID_RPT_ID",
                                              "HS_TRANS_NOT_SPT",
                                              "HS_INVALID_PARAM",
                                              "HS_ERROR",
                                              "ERR",
                                              "ERR_SDP",
                                              "ERR_PROTO",
                                              "ERR_DB_FULL",
                                              "ERR_TOD_UNSPT",
                                              "ERR_NO_RES",
                                              "ERR_AUTH_FAILED",
                                              "ERR_HDL",
                                              "ERR_SEC",
                                              "BUSY",
                                              "NO_DATA",
                                              "NEED_INIT",
                                              "NEED_DEINIT",
                                              "NO_CONNECTION"
                                             };

static esp_hidh_dev_t *hidh_dev_ctor(esp_bd_addr_t bda);

static char *get_trans_type_str(esp_hid_trans_type_t trans_type)
{
    switch (trans_type) {
    case ESP_HID_TRANS_HANDSHAKE:
        return "TRANS_HANDSHAKE";
    case ESP_HID_TRANS_CONTROL:
        return "TRANS_CONTROL";
    case ESP_HID_TRANS_GET_REPORT:
        return "TRANS_GET_REPORT";
    case ESP_HID_TRANS_SET_REPORT:
        return "TRANS_SET_REPORT";
    case ESP_HID_TRANS_GET_PROTOCOL:
        return "TRANS_GET_PROTOCOL";
    case ESP_HID_TRANS_SET_PROTOCOL:
        return "TRANS_SET_PROTOCOL";
    case ESP_HID_TRANS_GET_IDLE:
        return "TRANS_GET_IDLE";
    case ESP_HID_TRANS_SET_IDLE:
        return "TRANS_SET_IDLE";
    case ESP_HID_TRANS_DATA:
        return "TRANS_DATA";
    case ESP_HID_TRANS_DATAC:
        return "TRANS_DATAC";
    case ESP_HID_TRANS_MAX:
        return "TRANS_MAX";
    default:
        return "UNKOWN";
    }
}

static esp_err_t bt_hidh_get_status(esp_hidh_status_t status)
{
    esp_err_t ret = ESP_OK;
    switch (status) {
    case ESP_HIDH_OK:
        ret = ESP_OK;
        break;
    case ESP_HIDH_ERR_NO_RES:
        ret = ESP_ERR_NO_MEM;
        break;
    default:
        ret = ESP_FAIL;
        break;
    }
    return ret;
}

static void utl_freebuf(void **p)
{
    if (*p != NULL) {
        free(*p);
        *p = NULL;
    }
}

static void transaction_timeout_handler(void *arg)
{
    esp_hidh_dev_t *dev = (esp_hidh_dev_t *)arg;
    if (dev != NULL && esp_hidh_dev_exists(dev)) {
        ESP_LOGW(TAG, "transaction timeout!");
        esp_hidh_dev_lock(dev);
        dev->trans_type = ESP_HID_TRANS_MAX;
        dev->report_id = 0;
        dev->report_type = 0;
        esp_hidh_dev_unlock(dev);
    }
}

static inline void set_trans(esp_hidh_dev_t *dev, esp_hid_trans_type_t trans_type)
{
    dev->trans_type = trans_type;
    if (dev->trans_timer == NULL) {
        esp_timer_create_args_t config = {
            .callback = &transaction_timeout_handler,
            .arg = (void *)dev,
            .name = "hid_trans"
        };
        if (esp_timer_create(&config, &dev->trans_timer) != ESP_OK) {
            ESP_LOGE(TAG, "create trans timer failed! trans:%s", get_trans_type_str(trans_type));
            return;
        }
    }
    if (!esp_timer_is_active(dev->trans_timer) && esp_timer_start_once(dev->trans_timer, TRANS_TO) != ESP_OK) {
        ESP_LOGE(TAG, "set trans timer failed! trans:%s", get_trans_type_str(trans_type));
    }
}

static inline void reset_trans(esp_hidh_dev_t *dev)
{
    esp_hidh_dev_lock(dev);
    dev->trans_type = ESP_HID_TRANS_MAX;
    dev->report_id = 0;
    dev->report_type = 0;
    if (dev->trans_timer) {
        esp_timer_stop(dev->trans_timer);
    }
    esp_hidh_dev_unlock(dev);
}

static inline bool is_trans_done(esp_hidh_dev_t *dev)
{
    bool ret = (dev->trans_type == ESP_HID_TRANS_MAX);
    return ret;
}

static void free_local_param(void)
{
    if (hidh_local_param.connection_queue) {
        fixed_queue_free(hidh_local_param.connection_queue, free);
    }
}

static void open_failed_cb(esp_hidh_dev_t *dev, esp_hidh_status_t status, esp_hidh_event_data_t *p,
                           size_t event_data_size)
{
    p->open.status = bt_hidh_get_status(status);
    p->open.dev = dev;
    if (dev != NULL) {
        esp_hidh_dev_lock(dev);
        if (dev->connected) {
            esp_bt_hid_host_disconnect(dev->addr.bda);
        } else {
            dev->in_use = false;
        }
        esp_hidh_dev_unlock(dev);
    }
    esp_event_post_to(hidh_local_param.event_loop_handle, ESP_HIDH_EVENTS, ESP_HIDH_OPEN_EVENT, p, event_data_size,
                      portMAX_DELAY);
}

static void esp_hh_cb(esp_hidh_cb_event_t event, esp_hidh_cb_param_t *param)
{
    conn_item_t *conn_item = NULL;
    esp_hidh_dev_t *dev = NULL;
    esp_hidh_dev_report_t *report = NULL;
    bool has_report_id = false;
    size_t data_len = 0;
    uint8_t *p_data = NULL;
    esp_hidh_event_data_t p = {0};
    esp_hidh_event_data_t *p_param = NULL;
    size_t event_data_size = sizeof(esp_hidh_event_data_t);

    switch (event) {
    case ESP_HIDH_INIT_EVT: {
        p.start.status = bt_hidh_get_status(param->init.status);
        esp_event_post_to(hidh_local_param.event_loop_handle, ESP_HIDH_EVENTS, ESP_HIDH_START_EVENT, &p,
                          event_data_size, portMAX_DELAY);
        if (param->init.status != ESP_HIDH_OK) {
            ESP_LOGE(TAG, "ENABLE ERROR: %s", s_esp_hh_status_names[param->init.status]);
            free_local_param();
        }
        break;
    }
    case ESP_HIDH_DEINIT_EVT: {
        p.stop.status = bt_hidh_get_status(param->deinit.status);
        esp_event_post_to(hidh_local_param.event_loop_handle, ESP_HIDH_EVENTS, ESP_HIDH_STOP_EVENT, &p,
                          event_data_size, portMAX_DELAY);
        if (param->deinit.status != ESP_HIDH_OK) {
            ESP_LOGE(TAG, "DISABLE ERROR: %s", s_esp_hh_status_names[param->deinit.status]);
        } else {
            free_local_param();
        }
        break;
    }
    case ESP_HIDH_OPEN_EVT: {
        if (param->open.conn_status == ESP_HIDH_CONN_STATE_CONNECTING) {
            // ignore this conn_status
            break;
        }

        do {
            dev = esp_hidh_dev_get_by_bda(param->open.bd_addr);
            if (dev == NULL) {
                if (param->open.is_orig) {
                    ESP_LOGE(TAG, "OPEN ERROR: Device Not Found");
                    param->open.status = ESP_HIDH_NO_CONNECTION;
                    break;
                } else {
                    ESP_LOGD(TAG, "incoming device connect");
                    if ((dev = hidh_dev_ctor(param->open.bd_addr)) == NULL) {
                        ESP_LOGE(TAG, "%s create device failed!", __func__);
                        param->open.status = ESP_HIDH_ERR_NO_RES;
                        break;
                    }
                    esp_hidh_dev_lock(dev);
                    dev->opened = false; // not opened by ourself
                    dev->is_orig = false;
                    esp_hidh_dev_unlock(dev);
                }
            }

            if (param->open.status != ESP_HIDH_OK) {
                break;
            }
            esp_hidh_dev_lock(dev);
            dev->connected = true;
            dev->bt.handle = param->open.handle;
            esp_hidh_dev_unlock(dev);
            conn_item = malloc(sizeof(conn_item_t));
            if (conn_item == NULL) {
                ESP_LOGE(TAG, "conn_item malloc failed!");
                param->open.status = ESP_HIDH_ERR_NO_RES;
                break;
            }
            conn_item->dev = dev;
            bool ret = fixed_queue_enqueue(hidh_local_param.connection_queue, conn_item, FIXED_QUEUE_MAX_TIMEOUT);
            assert(ret == true);
        } while (0);

        if (param->open.status != ESP_HIDH_OK) {
            ESP_LOGE(TAG, "OPEN ERROR: %s", s_esp_hh_status_names[param->open.status]);
            open_failed_cb(dev, param->open.status, &p, event_data_size);
        }

        if (dev != NULL) {
            esp_hidh_dev_lock(dev);
            dev->status = param->open.status;
            esp_hidh_dev_unlock(dev);
        }
        break;
    }
    case ESP_HIDH_GET_DSCP_EVT: {
        do {
            ESP_LOGV(TAG, "DESCRIPTOR: PID: 0x%04x, VID: 0x%04x, VERSION: 0x%04x, REPORT_LEN: %u",
                     param->dscp.product_id, param->dscp.vendor_id, param->dscp.version, param->dscp.dl_len);
            if ((conn_item = (conn_item_t *)fixed_queue_dequeue(hidh_local_param.connection_queue,
                                                                FIXED_QUEUE_MAX_TIMEOUT)) == NULL) {
                ESP_LOGE(TAG, "No pending connect device!");
                param->dscp.status = ESP_HIDH_NO_CONNECTION;
                break;
            }
            dev = conn_item->dev;
            utl_freebuf((void **)&conn_item);
            // in case the dev has been freed
            if (!esp_hidh_dev_exists(dev)) {
                ESP_LOGE(TAG, "Device Not Found");
                dev = NULL;
                param->dscp.status = ESP_HIDH_NO_CONNECTION;
                break;
            }
            // check if connected
            esp_hidh_dev_lock(dev);
            if (!dev->connected) {
                esp_hidh_dev_unlock(dev);
                ESP_LOGE(TAG, "Connection has been released!");
                param->dscp.status = ESP_HIDH_NO_CONNECTION;
                break;
            }
            // check if get descriptor failed
            if (param->dscp.status != ESP_HIDH_OK) {
                esp_hidh_dev_unlock(dev);
                ESP_LOGE(TAG, "GET_DSCP ERROR: %s", s_esp_hh_status_names[param->dscp.status]);
                break;
            }
            dev->added = param->dscp.added;
            dev->config.product_id = param->dscp.product_id;
            dev->config.vendor_id = param->dscp.vendor_id;
            dev->config.version = param->dscp.version;

            dev->config.report_maps_len = 1;
            dev->config.report_maps =
                (esp_hid_raw_report_map_t *)malloc(dev->config.report_maps_len * sizeof(esp_hid_raw_report_map_t));
            if (dev->config.report_maps == NULL) {
                esp_hidh_dev_unlock(dev);
                ESP_LOGE(TAG, "malloc report maps failed");
                param->dscp.status = ESP_HIDH_ERR_NO_RES;
                break;
            }

            dev->config.report_maps[0].data = (uint8_t *)malloc(param->dscp.dl_len);
            if (dev->config.report_maps[0].data == NULL) {
                ESP_LOGE(TAG, "Malloc Report Map Failed");
                param->dscp.status = ESP_HIDH_ERR_NO_RES;
            } else {
                dev->config.report_maps[0].len = param->dscp.dl_len;
                memcpy((uint8_t *)dev->config.report_maps[0].data, param->dscp.dsc_list,
                       dev->config.report_maps[0].len);
                // generate reports

                if (dev->config.report_maps[0].len && dev->config.report_maps[0].data) {
                    esp_hid_report_map_t *map;
                    esp_hidh_dev_report_t *report;
                    esp_hid_report_item_t *r;
                    map = esp_hid_parse_report_map(dev->config.report_maps[0].data, dev->config.report_maps[0].len);
                    if (map) {
                        if (dev->usage == 0) {
                            dev->usage = map->usage;
                        }
                        dev->reports = NULL;
                        for (uint8_t i = 0; i < map->reports_len; i++) {
                            r = &map->reports[i];
                            report = (esp_hidh_dev_report_t *)malloc(sizeof(esp_hidh_dev_report_t));
                            if (report == NULL) {
                                ESP_LOGE(TAG, "Malloc Report Failed");
                                param->dscp.status = ESP_HIDH_ERR_NO_RES;
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
                        param->dscp.status = ESP_HIDH_ERR;
                    }
                }
            }
            esp_hidh_dev_unlock(dev);
        } while (0);

        if (param->dscp.status != ESP_HIDH_OK) {
            open_failed_cb(dev, param->dscp.status, &p, event_data_size);
        }

        if (dev != NULL) {
            esp_hidh_dev_lock(dev);
            dev->status = param->dscp.status;
            // if has been added by lower layer, tell up layer
            if (dev->status == ESP_HIDH_OK && dev->connected && dev->added) {
                p.open.status = bt_hidh_get_status(ESP_HIDH_OK);
                p.open.dev = dev;
                esp_hidh_dev_unlock(dev);
                esp_event_post_to(hidh_local_param.event_loop_handle, ESP_HIDH_EVENTS, ESP_HIDH_OPEN_EVENT, &p,
                                  event_data_size, portMAX_DELAY);
            } else {
                esp_hidh_dev_unlock(dev);
            }
        }
        break;
    }
    case ESP_HIDH_ADD_DEV_EVT: {
        ESP_LOGV(TAG, "ADD_DEV: BDA: " ESP_BD_ADDR_STR ", handle: %d, status: %s",
                 ESP_BD_ADDR_HEX(param->add_dev.bd_addr), param->add_dev.handle,
                 s_esp_hh_status_names[param->add_dev.status]);
        do {
            dev = esp_hidh_dev_get_by_handle(param->add_dev.handle);
            if (dev == NULL) {
                ESP_LOGE(TAG, "Device Not Found");
                param->add_dev.status = ESP_HIDH_NO_CONNECTION;
                break;
            }
            esp_hidh_dev_lock(dev);
            dev->added = param->add_dev.status == ESP_HIDH_OK ? true : false;
            esp_hidh_dev_unlock(dev);
        } while (0);

        if (param->add_dev.status != ESP_HIDH_OK) {
            ESP_LOGE(TAG, "ADD_DEV ERROR: %s", s_esp_hh_status_names[param->add_dev.status]);
            open_failed_cb(dev, param->add_dev.status, &p, event_data_size);
        }
        if (dev != NULL) {
            esp_hidh_dev_lock(dev);
            dev->status = param->add_dev.status;
            if (dev->status == ESP_HIDH_OK && dev->connected && dev->added) {
                p.open.status = bt_hidh_get_status(ESP_HIDH_OK);
                p.open.dev = dev;
                esp_hidh_dev_unlock(dev);
                esp_event_post_to(hidh_local_param.event_loop_handle, ESP_HIDH_EVENTS, ESP_HIDH_OPEN_EVENT, &p,
                                  event_data_size, portMAX_DELAY);
            } else {
                esp_hidh_dev_unlock(dev);
            }
        }
        break;
    }
    case ESP_HIDH_CLOSE_EVT: {
        if (param->close.conn_status == ESP_HIDH_CONN_STATE_DISCONNECTING) {
            // ignore this conn_status
            break;
        }
        ESP_LOGV(TAG, "CLOSE: handle: %d, status: %s", param->close.handle, s_esp_hh_status_names[param->close.status]);
        do {
            dev = esp_hidh_dev_get_by_handle(param->close.handle);
            if (dev == NULL) {
                ESP_LOGE(TAG, "Device Not Found");
                param->close.status = ESP_HIDH_NO_CONNECTION;
                break;
            }
            esp_hidh_dev_lock(dev);
            dev->status = param->close.status;
            if (dev->connected) {
                dev->connected = false;
            }
            // free the device in the wrapper event handler
            dev->in_use = false;
            esp_hidh_dev_unlock(dev);
        } while (0);

        if (param->close.status != ESP_HIDH_OK) {
            ESP_LOGE(TAG, "CLOSE ERROR: %s", s_esp_hh_status_names[param->close.status]);
        }
        p.close.dev = dev;
        p.close.status = bt_hidh_get_status(param->close.status);
        esp_event_post_to(hidh_local_param.event_loop_handle, ESP_HIDH_EVENTS, ESP_HIDH_CLOSE_EVENT, &p,
                          event_data_size, portMAX_DELAY);

        break;
    }
    case ESP_HIDH_SET_RPT_EVT: {
        if (param->set_rpt.status != ESP_HIDH_OK) {
            ESP_LOGE(TAG, "SET_RPT ERROR: handle: %d, status: %s", param->set_rpt.handle,
                     s_esp_hh_status_names[param->set_rpt.status]);
        }
        dev = esp_hidh_dev_get_by_handle(param->set_rpt.handle);
        if (dev == NULL) {
            ESP_LOGE(TAG, "SET_RPT ERROR: Device Not Found");
            break;
        }
        esp_hidh_dev_lock(dev);
        dev->status = param->set_rpt.status;
        p.feature.dev = dev;
        esp_hidh_dev_unlock(dev);
        p.feature.status = bt_hidh_get_status(param->set_rpt.status);
        p.feature.trans_type = ESP_HID_TRANS_SET_REPORT;
        esp_event_post_to(hidh_local_param.event_loop_handle, ESP_HIDH_EVENTS, ESP_HIDH_FEATURE_EVENT, &p,
                          event_data_size, portMAX_DELAY);
        reset_trans(dev);
        break;
    }
    case ESP_HIDH_GET_RPT_EVT: {
        if (param->get_rpt.status != ESP_HIDH_OK) {
            ESP_LOGE(TAG, "GET_RPT ERROR: handle: %d, status: %s", param->get_rpt.handle,
                     s_esp_hh_status_names[param->get_rpt.status]);
        } else if (param->get_rpt.len > 0 && param->get_rpt.data) {
            event_data_size += param->get_rpt.len;
        }
        dev = esp_hidh_dev_get_by_handle(param->get_rpt.handle);
        if (dev == NULL) {
            ESP_LOGE(TAG, "GET_RPT ERROR: Device Not Found");
            break;
        }
        esp_hidh_dev_lock(dev);
        dev->status = param->get_rpt.status;
        if ((p_param = (esp_hidh_event_data_t *)malloc(event_data_size)) != NULL) {
            memset(p_param, 0, event_data_size);
            p_param->feature.dev = dev;
            p_param->feature.status = bt_hidh_get_status(param->get_rpt.status);
            p_param->feature.trans_type = ESP_HID_TRANS_GET_REPORT;
            if (param->get_rpt.status == ESP_HIDH_OK && param->get_rpt.len > 0 && param->get_rpt.data) {
                if (dev->report_id) {
                    data_len = param->get_rpt.len - 1;
                    p_data = (uint8_t *)param->get_rpt.data + 1;
                } else {
                    data_len = param->get_rpt.len;
                    p_data = (uint8_t *)param->get_rpt.data;
                }
                memcpy(((uint8_t *)p_param) + sizeof(esp_hidh_event_data_t), p_data, data_len);
                p_param->feature.length = data_len;
                p_param->feature.data = p_data;
                p_param->feature.report_id = dev->report_id;
                esp_hidh_dev_unlock(dev);
            }
            esp_event_post_to(hidh_local_param.event_loop_handle, ESP_HIDH_EVENTS, ESP_HIDH_FEATURE_EVENT, p_param,
                              event_data_size, portMAX_DELAY);
        } else {
            esp_hidh_dev_unlock(dev);
            ESP_LOGE(TAG, "GET_RPT ERROR: malloc event data failed!");
        }
        reset_trans(dev);
        break;
    }
    case ESP_HIDH_GET_IDLE_EVT: {
        if (param->get_idle.status != ESP_HIDH_OK) {
            ESP_LOGE(TAG, "GET_IDLE ERROR: handle: %d, status: %s", param->get_idle.handle,
                     s_esp_hh_status_names[param->get_idle.status]);
        } else {
            event_data_size += 1;
        }
        dev = esp_hidh_dev_get_by_handle(param->get_idle.handle);
        if (dev == NULL) {
            ESP_LOGE(TAG, "GET_IDLE ERROR: Device Not Found");
            break;
        }
        esp_hidh_dev_lock(dev);
        dev->status = param->get_idle.status;
        if ((p_param = (esp_hidh_event_data_t *)malloc(event_data_size)) != NULL) {
            memset(p_param, 0, event_data_size);
            p_param->feature.dev = dev;
            p_param->feature.status = bt_hidh_get_status(param->get_idle.status);
            p_param->feature.trans_type = ESP_HID_TRANS_GET_IDLE;
            if (param->get_idle.status == ESP_HIDH_OK) {
                *(((uint8_t *)p_param) + sizeof(esp_hidh_event_data_t)) = param->get_idle.idle_rate;
                p_param->feature.length = 1;
                p_param->feature.data = ((uint8_t *)p_param) + sizeof(esp_hidh_event_data_t);
            }
            esp_hidh_dev_unlock(dev);
            esp_event_post_to(hidh_local_param.event_loop_handle, ESP_HIDH_EVENTS, ESP_HIDH_FEATURE_EVENT, p_param,
                              event_data_size, portMAX_DELAY);
        } else {
            esp_hidh_dev_unlock(dev);
            ESP_LOGE(TAG, "GET_IDLE ERROR: malloc event data failed!");
        }
        reset_trans(dev);
        break;
    }
    case ESP_HIDH_SET_IDLE_EVT: {
        if (param->set_idle.status != ESP_HIDH_OK) {
            ESP_LOGE(TAG, "SET_IDLE ERROR: handle: %d, status: %s", param->set_idle.handle,
                     s_esp_hh_status_names[param->set_idle.status]);
        }
        dev = esp_hidh_dev_get_by_handle(param->set_idle.handle);
        if (dev == NULL) {
            ESP_LOGE(TAG, "SET_IDLE ERROR: Device Not Found");
            break;
        }
        esp_hidh_dev_lock(dev);
        dev->status = param->set_idle.status;
        p.feature.dev = dev;
        esp_hidh_dev_unlock(dev);
        p.feature.status = bt_hidh_get_status(param->set_idle.status);
        p.feature.trans_type = ESP_HID_TRANS_SET_IDLE;
        esp_event_post_to(hidh_local_param.event_loop_handle, ESP_HIDH_EVENTS, ESP_HIDH_FEATURE_EVENT, &p,
                          event_data_size, portMAX_DELAY);
        reset_trans(dev);
        break;
    }
    case ESP_HIDH_GET_PROTO_EVT: {
        if (param->get_proto.status != ESP_HIDH_OK) {
            ESP_LOGE(TAG, "GET_PROTO ERROR: handle: %d, status: %s", param->get_proto.handle,
                     s_esp_hh_status_names[param->get_proto.status]);
        } else {
            event_data_size += 1;
        }
        dev = esp_hidh_dev_get_by_handle(param->get_proto.handle);
        if (dev == NULL) {
            ESP_LOGE(TAG, "GET_PROTO ERROR: Device Not Found");
            break;
        }
        esp_hidh_dev_lock(dev);
        dev->status = param->get_proto.status;
        if ((p_param = (esp_hidh_event_data_t *)malloc(event_data_size)) != NULL) {
            memset(p_param, 0, event_data_size);
            p_param->feature.dev = dev;
            p_param->feature.status = bt_hidh_get_status(param->get_proto.status);
            p_param->feature.trans_type = ESP_HID_TRANS_GET_PROTOCOL;
            if (param->get_proto.status == ESP_HIDH_OK) {
                dev->protocol_mode = param->get_proto.proto_mode; // update the device protocol mode
                *(((uint8_t *)p_param) + sizeof(esp_hidh_event_data_t)) = param->get_proto.proto_mode;
                p_param->feature.length = 1;
                p_param->feature.data = ((uint8_t *)p_param) + sizeof(esp_hidh_event_data_t);
            }
            esp_hidh_dev_unlock(dev);
            esp_event_post_to(hidh_local_param.event_loop_handle, ESP_HIDH_EVENTS, ESP_HIDH_FEATURE_EVENT, p_param,
                              event_data_size, portMAX_DELAY);
        } else {
            esp_hidh_dev_unlock(dev);
            ESP_LOGE(TAG, "GET_PROTO ERROR: malloc event data failed!");
        }
        reset_trans(dev);
        break;
    }
    case ESP_HIDH_SET_PROTO_EVT: {
        if (param->set_proto.status != ESP_HIDH_OK) {
            ESP_LOGE(TAG, "SET_PROTO ERROR: handle: %d, status: %s", param->set_proto.handle,
                     s_esp_hh_status_names[param->set_proto.status]);
        }
        dev = esp_hidh_dev_get_by_handle(param->set_proto.handle);
        if (dev == NULL) {
            ESP_LOGE(TAG, "Device Not Found");
            break;
        }
        esp_hidh_dev_lock(dev);
        dev->status = param->set_proto.status;
        p.feature.dev = dev;
        esp_hidh_dev_unlock(dev);
        p.feature.status = bt_hidh_get_status(param->set_proto.status);
        p.feature.trans_type = ESP_HID_TRANS_SET_PROTOCOL;
        esp_event_post_to(hidh_local_param.event_loop_handle, ESP_HIDH_EVENTS, ESP_HIDH_FEATURE_EVENT, &p,
                          event_data_size, portMAX_DELAY);
        reset_trans(dev);
        break;
    }
    case ESP_HIDH_DATA_IND_EVT: {
        esp_hid_usage_t _usage;
        if (param->data_ind.status != ESP_HIDH_OK) {
            ESP_LOGE(TAG, "DATA_IND ERROR: handle: %d, status: %s", param->data_ind.handle,
                     s_esp_hh_status_names[param->data_ind.status]);
        }
        dev = esp_hidh_dev_get_by_handle(param->data_ind.handle);
        if (dev == NULL) {
            ESP_LOGE(TAG, "Device Not Found: handle %u", param->data_ind.handle);
            break;
        }

        if (param->data_ind.len > 0 && param->data_ind.data != NULL) {
            esp_hidh_dev_lock(dev);
            event_data_size += param->data_ind.len;
            if (param->data_ind.proto_mode == ESP_HID_PROTOCOL_MODE_BOOT) {
                /**
                 * first data shall have report_id, according to HID_SPEC_V10
                 * | Device   | Report ID | Report Size |
                 * --------------------------------------
                 * | Keyboard | 1         | 9 Bytes     |
                 * | Mouse    | 2         | 4 Bytes     |
                 * | Reserved | 0, 3-255  | N/A         |
                 */
                if (param->data_ind.len == 9 && *(param->data_ind.data) == 1) {
                    has_report_id = true;
                    _usage = ESP_HID_USAGE_KEYBOARD;
                } else if (param->data_ind.len == 4 && *(param->data_ind.data) == 2) {
                    has_report_id = true;
                    _usage = ESP_HID_USAGE_MOUSE;
                } else {
                    esp_hidh_dev_unlock(dev);
                    ESP_LOGE(TAG, "Invalid Boot Report format, rpt_len:%d, rpt_id:%d!", param->data_ind.len,
                             *(param->data_ind.data));
                    break;
                }
            } else {
                report = esp_hidh_dev_get_input_report_by_proto_and_data(
                             dev, ESP_HID_PROTOCOL_MODE_REPORT, param->data_ind.len, param->data_ind.data, &has_report_id);
                if (report == NULL) {
                    esp_hidh_dev_unlock(dev);
                    ESP_LOGE(TAG, "Not find report handle: %d mode: %s", param->data_ind.handle,
                             param->data_ind.proto_mode == ESP_HID_PROTOCOL_MODE_REPORT ? "REPORT" : "BOOT");
                    break;
                }
                _usage = report->usage;
            }

            if ((p_param = (esp_hidh_event_data_t *)malloc(event_data_size)) == NULL) {
                esp_hidh_dev_unlock(dev);
                ESP_LOGE(TAG, "DATA_IND ERROR: malloc event data failed!");
                break;
            }
            memset(p_param, 0, event_data_size);
            p_param->input.dev = dev;
            p_param->input.usage = _usage;
            if (has_report_id) {
                data_len = param->data_ind.len - 1;
                p_data = (uint8_t *)param->data_ind.data + 1;
                p_param->input.report_id = *(uint8_t *)param->data_ind.data;
            } else {
                data_len = param->data_ind.len;
                p_data = (uint8_t *)param->data_ind.data;
                p_param->input.report_id = report->report_id;
            }
            memcpy(((uint8_t *)p_param) + sizeof(esp_hidh_event_data_t), p_data, data_len);
            p_param->input.length = data_len;
            p_param->input.data = p_data;
            esp_hidh_dev_unlock(dev);
            esp_event_post_to(hidh_local_param.event_loop_handle, ESP_HIDH_EVENTS, ESP_HIDH_INPUT_EVENT, p_param,
                              event_data_size, portMAX_DELAY);
        }
        break;
    }
    case ESP_HIDH_DATA_EVT:
        break;
    default:
        ESP_LOGV(TAG, "BTA_HH EVENT: %s", s_esp_hh_evt_names[event]);
        break;
    }

    if (p_param) {
        free(p_param);
        p_param = NULL;
    }
}

/*
 * Public Functions
 * */

static esp_err_t esp_bt_hidh_dev_close(esp_hidh_dev_t *dev)
{
    esp_err_t ret = ESP_OK;
    do {
        if (dev == NULL) {
            ret = ESP_ERR_INVALID_ARG;
            break;
        }
        if (!dev->connected) {
            ESP_LOGW(TAG, "%s hdl:0x%02x not connected", __func__, dev->bt.handle);
            ret = ESP_ERR_INVALID_STATE;
            break;
        }
        ret = esp_bt_hid_host_disconnect(dev->addr.bda);
    } while (0);
    return ret;
}

static esp_err_t esp_bt_hidh_dev_report_write(esp_hidh_dev_t *dev, size_t map_index, size_t report_id,
                                              int report_type, uint8_t *data, size_t len)
{
    esp_err_t ret = ESP_OK;
    uint8_t *p_data = NULL;
    do {
        esp_hidh_dev_report_t *report =
            esp_hidh_dev_get_report_by_id_type_proto(dev, map_index, report_id, report_type, dev->protocol_mode);
        if (!report) {
            ESP_LOGE(TAG, "mode:%s report:%s id:%d not found", get_protocol_mode(dev->protocol_mode),
                     esp_hid_report_type_str(report_type), report_id);
            ret = ESP_FAIL;
            break;
        }
        if (len > report->value_len) {
            ESP_LOGE(TAG, "%s report %d takes maximum %d bytes. you have provided %d",
                     esp_hid_report_type_str(report_type), report_id, report->value_len, len);
            ret = ESP_FAIL;
            break;
        }

        if (report_type != ESP_HID_REPORT_TYPE_OUTPUT) {
            ESP_LOGE(TAG,
                     "Only OUTPUT type data can be send on interrupt channel.\n" \
                     "You have provided %s, try Set_Report!",
                     esp_hid_report_type_str(report_type));
            ret = ESP_FAIL;
            break;
        }

        if (report_id) {
            if ((p_data = malloc(len + 1)) == NULL) {
                ESP_LOGE(TAG, "%s malloc failed!", __func__);
                ret = ESP_FAIL;
                break;
            }
            *p_data = report_id;
            memcpy(p_data + 1, data, len);
            data = p_data;
            len = len + 1;
        }

        ret = esp_bt_hid_host_send_data(dev->addr.bda, data, len);
        if (p_data) {
            free(p_data);
        }
    } while (0);

    return ret;
}

static esp_err_t esp_bt_hidh_dev_set_report(esp_hidh_dev_t *dev, size_t map_index, size_t report_id,
                                            int report_type, uint8_t *data, size_t len)
{
    esp_err_t ret = ESP_OK;
    uint8_t *p_data = NULL;
    esp_hidh_dev_report_t *report = NULL;
    do {
        if (!is_trans_done(dev)) {
            ESP_LOGE(TAG, "Pending previous tansaction %s done, try later!", get_trans_type_str(dev->trans_type));
            ret = ESP_FAIL;
            break;
        }
        report = esp_hidh_dev_get_report_by_id_type_proto(dev, map_index, report_id, report_type, dev->protocol_mode);
        if (!report) {
            ESP_LOGE(TAG, "mode:%s report:%s id:%d not found", get_protocol_mode(dev->protocol_mode),
                     esp_hid_report_type_str(report_type), report_id);
            ret = ESP_FAIL;
            break;
        }
        if (len > report->value_len) {
            ESP_LOGE(TAG, "%s report %d takes maximum %d bytes. you have provided %d",
                     esp_hid_report_type_str(report_type), report_id, report->value_len, len);
            ret = ESP_FAIL;
            break;
        }

        if (report_id) {
            if ((p_data = malloc(len + 1)) == NULL) {
                ESP_LOGE(TAG, "%s malloc failed!", __func__);
                ret = ESP_FAIL;
                break;
            }
            *p_data = report_id;
            memcpy(p_data + 1, data, len);
            data = p_data;
            len = len + 1;
        }

        ret = esp_bt_hid_host_set_report(dev->addr.bda, report_type, data, len);
        if (p_data) {
            free(p_data);
        }

        if (ret == ESP_OK) {
            set_trans(dev, ESP_HID_TRANS_SET_REPORT);
        }
    } while (0);

    return ret;
}

static esp_err_t esp_bt_hidh_dev_report_read(esp_hidh_dev_t *dev, size_t map_index, size_t report_id, int report_type,
                                             size_t max_length, uint8_t *value, size_t *value_len)
{
    esp_err_t ret = ESP_OK;
    esp_hidh_dev_report_t *report = NULL;
    do {
        if (!is_trans_done(dev)) {
            ESP_LOGE(TAG, "Pending previous tansaction %s done, try later!", get_trans_type_str(dev->trans_type));
            ret = ESP_FAIL;
            break;
        }
        report = esp_hidh_dev_get_report_by_id_type_proto(dev, map_index, report_id, report_type, dev->protocol_mode);
        if (!report) {
            ESP_LOGE(TAG, "mode:%s report:%s id:%d not found", get_protocol_mode(dev->protocol_mode),
                     esp_hid_report_type_str(report_type), report_id);
            ret = ESP_FAIL;
            break;
        }
        ret = esp_bt_hid_host_get_report(dev->addr.bda, report_type, report_id, max_length);
        if (ret == ESP_OK) {
            dev->trans_type = ESP_HID_TRANS_GET_REPORT;
            dev->report_id = report_id;
            dev->report_type = report_type;
        }
    } while (0);
    return ret;
}

static esp_err_t esp_bt_hidh_dev_get_idle(esp_hidh_dev_t *dev)
{
    esp_err_t ret = ESP_OK;
    do {
        if (!is_trans_done(dev)) {
            ESP_LOGE(TAG, "Pending previous tansaction %s done, try later!", get_trans_type_str(dev->trans_type));
            ret = ESP_FAIL;
            break;
        }
        if (!dev->connected) {
            ESP_LOGW(TAG, "%s hdl:0x%02x not connected", __func__, dev->bt.handle);
            ret = ESP_ERR_INVALID_STATE;
            break;
        }
        ret = esp_bt_hid_host_get_idle(dev->addr.bda);
        if (ret == ESP_OK) {
            set_trans(dev, ESP_HID_TRANS_GET_IDLE);
        }
    } while (0);

    return ret;
}

static esp_err_t esp_bt_hidh_dev_set_idle(esp_hidh_dev_t *dev, uint8_t idle_time)
{
    esp_err_t ret = ESP_OK;
    do {
        if (!is_trans_done(dev)) {
            ESP_LOGE(TAG, "Pending previous tansaction %s done, try later!", get_trans_type_str(dev->trans_type));
            ret = ESP_FAIL;
            break;
        }
        if (!dev->connected) {
            ESP_LOGW(TAG, "%s hdl:0x%02x not connected", __func__, dev->bt.handle);
            ret = ESP_ERR_INVALID_STATE;
            break;
        }
        ret = esp_bt_hid_host_set_idle(dev->addr.bda, idle_time);
        if (ret == ESP_OK) {
            set_trans(dev, ESP_HID_TRANS_SET_IDLE);
        }
    } while (0);

    return ret;
}

static esp_err_t esp_bt_hidh_dev_get_protocol(esp_hidh_dev_t *dev)
{
    esp_err_t ret = ESP_OK;
    do {
        if (!is_trans_done(dev)) {
            ESP_LOGE(TAG, "Pending previous tansaction %s done, try later!", get_trans_type_str(dev->trans_type));
            ret = ESP_FAIL;
            break;
        }
        if (!dev->connected) {
            ESP_LOGW(TAG, "%s hdl:0x%02x not connected", __func__, dev->bt.handle);
            ret = ESP_ERR_INVALID_STATE;
            break;
        }
        ret = esp_bt_hid_host_get_protocol(dev->addr.bda);
        if (ret == ESP_OK) {
            set_trans(dev, ESP_HID_TRANS_GET_PROTOCOL);
        }
    } while (0);

    return ret;
}

static esp_err_t esp_bt_hidh_dev_set_protocol(esp_hidh_dev_t *dev, uint8_t protocol_mode)
{
    esp_err_t ret = ESP_OK;

    do {
        if (!is_trans_done(dev)) {
            ESP_LOGE(TAG, "Pending previous tansaction %s done, try later!", get_trans_type_str(dev->trans_type));
            ret = ESP_FAIL;
            break;
        }
        if (!dev->connected) {
            ESP_LOGW(TAG, "%s hdl:0x%02x not connected", __func__, dev->bt.handle);
            ret = ESP_ERR_INVALID_STATE;
            break;
        }
        ret = esp_bt_hid_host_set_protocol(dev->addr.bda, protocol_mode);
        if (ret == ESP_OK) {
            set_trans(dev, ESP_HID_TRANS_SET_PROTOCOL);
        }
    } while (0);

    return ret;
}

static void esp_bt_hidh_dev_dump(esp_hidh_dev_t *dev, FILE *fp)
{
    fprintf(fp, "BDA:" ESP_BD_ADDR_STR ", Status: %s, Connected: %s, Handle: %d, Usage: %s\n", ESP_BD_ADDR_HEX(dev->addr.bda), s_esp_hh_status_names[dev->status], dev->connected ? "YES" : "NO", dev->bt.handle, esp_hid_usage_str(dev->usage));
    fprintf(fp, "Name: %s, Manufacturer: %s, Serial Number: %s\n", dev->config.device_name ? dev->config.device_name : "", dev->config.manufacturer_name ? dev->config.manufacturer_name : "", dev->config.serial_number ? dev->config.serial_number : "");
    fprintf(fp, "PID: 0x%04x, VID: 0x%04x, VERSION: 0x%04x\n", dev->config.product_id, dev->config.vendor_id, dev->config.version);
    fprintf(fp, "Report Map Length: %d\n", dev->config.report_maps[0].len);
    esp_hidh_dev_report_t *report = dev->reports;
    while (report) {
        fprintf(fp, "  %8s %7s %6s, ID: %3u, Length: %3u\n",
                esp_hid_usage_str(report->usage), esp_hid_report_type_str(report->report_type), get_protocol_mode(report->protocol_mode),
                report->report_id, report->value_len);
        report = report->next;
    }
}

esp_err_t esp_bt_hidh_init(const esp_hidh_config_t *config)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(config, ESP_ERR_INVALID_ARG, TAG, "Config is NULL");

    hidh_local_param.connection_queue = fixed_queue_new(QUEUE_SIZE_MAX);
    ESP_RETURN_ON_FALSE(hidh_local_param.connection_queue, ESP_ERR_NO_MEM, TAG, "Alloc failed");
    hidh_local_param.event_loop_handle = esp_hidh_get_event_loop();

    ESP_GOTO_ON_ERROR(
        esp_bt_hid_host_register_callback(esp_hh_cb),
        bt_hid_fail, TAG, "BT HID register failed");
    ESP_GOTO_ON_ERROR(
        esp_bt_hid_host_init(),
        bt_hid_fail, TAG, "BT HID register failed");
    return ret;

bt_hid_fail:
    free_local_param();
    return ret;
}

esp_err_t esp_bt_hidh_deinit(void)
{
    return esp_bt_hid_host_deinit();
}

static esp_hidh_dev_t *hidh_dev_ctor(esp_bd_addr_t bda)
{
    esp_hidh_dev_t *dev = NULL;
    dev = esp_hidh_dev_malloc();
    if (dev == NULL) {
        return NULL;
    }
    dev->in_use = true;
    dev->transport = ESP_HID_TRANSPORT_BT;
    dev->trans_type = ESP_HID_TRANS_MAX;
    dev->trans_timer = NULL;
    dev->protocol_mode = ESP_HID_PROTOCOL_MODE_REPORT; // device default protocol mode
    dev->connected = false;
    dev->opened = true;
    dev->added = false;
    dev->is_orig = true;
    dev->reports = NULL;
    dev->reports_len = 0;
    dev->tmp = NULL;
    dev->tmp_len = 0;
    memcpy(dev->addr.bda, bda, sizeof(esp_bd_addr_t));
    dev->bt.handle = 0xff;

    dev->close = esp_bt_hidh_dev_close;
    dev->report_write = esp_bt_hidh_dev_report_write;
    dev->report_read = esp_bt_hidh_dev_report_read;
    dev->set_report = esp_bt_hidh_dev_set_report;
    dev->get_idle = esp_bt_hidh_dev_get_idle;
    dev->set_idle = esp_bt_hidh_dev_set_idle;
    dev->get_protocol = esp_bt_hidh_dev_get_protocol;
    dev->set_protocol = esp_bt_hidh_dev_set_protocol;
    dev->dump = esp_bt_hidh_dev_dump;

    return dev;
}

esp_hidh_dev_t *esp_bt_hidh_dev_open(esp_bd_addr_t bda)
{
    esp_hidh_dev_t *dev = esp_hidh_dev_get_by_bda(bda);
    if (dev == NULL) {
        if ((dev = hidh_dev_ctor(bda)) == NULL) {
            ESP_LOGE(TAG, "%s create device failed!", __func__);
            return NULL;
        }
    } else {
        ESP_LOGW(TAG, "device has opened, connected: %d", dev->connected);
    }

    if (!dev->connected) {
        esp_bt_hid_host_connect(dev->addr.bda);
    }
    return dev;
}

#endif /* CONFIG_BT_HID_HOST_ENABLED */
