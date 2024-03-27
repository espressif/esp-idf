/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdbool.h>

#include "ble_hidd.h"
#if CONFIG_GATTS_ENABLE

#include "esp_private/esp_hidd_private.h"
#include "esp_log.h"
#include "esp_bt_main.h"
#include "esp_bt_defs.h"
#include "esp_gatts_api.h"
#include "esp_gatt_defs.h"
#include "esp_gap_ble_api.h"

static const char *TAG = "BLE_HIDD";

/// Maximal length of Report Char. Value
#define HIDD_LE_REPORT_MAX_LEN                (255)

/// Maximal length of Report Map Char. Value
#define HIDD_LE_REPORT_MAP_MAX_LEN            (512)

/// Length of Boot Report Char. Value Maximal Length
#define HIDD_LE_BOOT_REPORT_MAX_LEN           (8)

typedef hidd_report_item_t hidd_le_report_item_t;

/*
 * UUIDs
 * */

//the uuid definition
static const uint16_t s_primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t s_include_service_uuid = ESP_GATT_UUID_INCLUDE_SERVICE;
static const uint16_t s_character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static const uint16_t s_character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;

//the property definition
static const uint8_t s_char_prop_read = ESP_GATT_CHAR_PROP_BIT_READ;
//static const uint8_t s_char_prop_write = ESP_GATT_CHAR_PROP_BIT_WRITE;
static const uint8_t s_char_prop_write_nr = ESP_GATT_CHAR_PROP_BIT_WRITE_NR;
static const uint8_t s_char_prop_read_notify = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
static const uint8_t s_char_prop_read_write = ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ;
static const uint8_t s_char_prop_read_write_nr = ESP_GATT_CHAR_PROP_BIT_WRITE_NR | ESP_GATT_CHAR_PROP_BIT_READ;
static const uint8_t s_char_prop_read_write_write_nr = ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_WRITE_NR | ESP_GATT_CHAR_PROP_BIT_READ;
//static const uint8_t s_char_prop_read_write_notify = ESP_GATT_CHAR_PROP_BIT_READ|ESP_GATT_CHAR_PROP_BIT_WRITE|ESP_GATT_CHAR_PROP_BIT_NOTIFY;

// Service UUIDs
static const uint16_t s_bat_svc = ESP_GATT_UUID_BATTERY_SERVICE_SVC;
static const uint16_t s_device_info_svc = ESP_GATT_UUID_DEVICE_INFO_SVC;
static const uint16_t s_hid_le_svc = ESP_GATT_UUID_HID_SVC;

// Battery UUIDs
static const uint16_t s_bat_level_uuid = ESP_GATT_UUID_BATTERY_LEVEL;
static const uint16_t s_bat_char_pres_format_uuid = ESP_GATT_UUID_CHAR_PRESENT_FORMAT;

// Device Info UUIDs
static const uint16_t s_device_info_pnp_id_uuid = ESP_GATT_UUID_PNP_ID;
static const uint16_t s_device_info_manufacturer_uuid = ESP_GATT_UUID_MANU_NAME;
static const uint16_t s_device_info_serial_num_uuid = ESP_GATT_UUID_SERIAL_NUMBER_STR;

// HID General UUIDs
static const uint16_t s_hid_report_map_uuid    = ESP_GATT_UUID_HID_REPORT_MAP;
static const uint16_t s_hid_report_map_ext_desc_uuid = ESP_GATT_UUID_EXT_RPT_REF_DESCR;
static const uint16_t s_hid_info_char_uuid = ESP_GATT_UUID_HID_INFORMATION;
static const uint16_t s_hid_control_point_uuid = ESP_GATT_UUID_HID_CONTROL_POINT;
static const uint16_t s_hid_proto_mode_uuid = ESP_GATT_UUID_HID_PROTO_MODE;

// HID Report UUIDs
static const uint16_t s_hid_report_ref_descr_uuid = ESP_GATT_UUID_RPT_REF_DESCR;
static const uint16_t s_hid_report_uuid = ESP_GATT_UUID_HID_REPORT;

// HID BOOT UUIDs
static const uint16_t s_hid_boot_kb_input_uuid = ESP_GATT_UUID_HID_BT_KB_INPUT;
static const uint16_t s_hid_boot_kb_output_uuid = ESP_GATT_UUID_HID_BT_KB_OUTPUT;
static const uint16_t s_hid_boot_mouse_input_uuid = ESP_GATT_UUID_HID_BT_MOUSE_INPUT;

// Battery Service Attributes Indexes
enum {
    BAS_IDX_SVC,

    BAS_IDX_BATT_LVL_CHAR,
    BAS_IDX_BATT_LVL_VAL,
    BAS_IDX_BATT_LVL_CCC,
    BAS_IDX_BATT_LVL_PRES_FMT,

    BAS_IDX_NB,
};

// HID Service Attributes Indexes
enum {
    HIDD_LE_IDX_SVC,

    // Included Service
    HIDD_LE_IDX_INCL_SVC,

    // HID Information
    HIDD_LE_IDX_HID_INFO_CHAR,
    HIDD_LE_IDX_HID_INFO_VAL,

    // HID Control Point
    HIDD_LE_IDX_HID_CTNL_PT_CHAR,
    HIDD_LE_IDX_HID_CTNL_PT_VAL,

    // Protocol Mode
    HIDD_LE_IDX_PROTO_MODE_CHAR,
    HIDD_LE_IDX_PROTO_MODE_VAL,

    // Report Map
    HIDD_LE_IDX_REPORT_MAP_CHAR,
    HIDD_LE_IDX_REPORT_MAP_VAL,
    HIDD_LE_IDX_REPORT_MAP_EXT_REP_REF,

    HIDD_LE_IDX_NB,
};

typedef struct {
    esp_gatt_if_t               gatt_if;
    uint16_t                    handle;
} hidd_le_service_t;

typedef struct {
    esp_hid_raw_report_map_t    reports_map;
    uint8_t                     reports_len;
    hidd_le_report_item_t      *reports;
    hidd_le_service_t           hid_svc;
    uint16_t                    hid_control_handle;
    uint16_t                    hid_protocol_handle;
} hidd_dev_map_t;

struct esp_ble_hidd_dev_s {
    esp_hidd_dev_t             *dev;
    SemaphoreHandle_t            sem;
    esp_event_loop_handle_t     event_loop_handle;
    esp_hid_device_config_t     config;
    uint16_t                    appearance;

    bool                        connected;
    uint16_t                    conn_id;
    esp_bd_addr_t               remote_bda;

    hidd_le_ccc_value_t         bat_ccc;
    uint8_t                     bat_level;  // 0 - 100 - battery percentage
    uint8_t                     control;    // 0x00 suspend, 0x01 suspend off
    uint8_t                     protocol;   // 0x00 boot, 0x01 report

    hidd_le_service_t           bat_svc;
    hidd_le_service_t           info_svc;
    esp_gatts_incl_svc_desc_t   hid_incl_svc;

    uint16_t                    bat_level_handle;
    uint16_t                    bat_ccc_handle;

    uint8_t                     pnp[7];

    hidd_dev_map_t             *devices;
    uint8_t                     devices_len;
};
typedef struct esp_ble_hidd_dev_s esp_ble_hidd_dev_t;

// HID Information characteristic value
static const uint8_t hidInfo[4] = {
    0x11, 0x01,     // bcdHID (USB HID version)
    0x00,           // bCountryCode
    ESP_HID_FLAGS_REMOTE_WAKE | ESP_HID_FLAGS_NORMALLY_CONNECTABLE   // Flags
};

#define WAIT_CB(d) xSemaphoreTake(d->sem, portMAX_DELAY)
#define SEND_CB(d) xSemaphoreGive(d->sem)

static const char *gatts_evt_names[25] = { "REG", "READ", "WRITE", "EXEC_WRITE", "MTU", "CONF", "UNREG", "CREATE", "ADD_INCL_SRVC", "ADD_CHAR", "ADD_CHAR_DESCR", "DELETE", "START", "STOP", "CONNECT", "DISCONNECT", "OPEN", "CANCEL_OPEN", "CLOSE", "LISTEN", "CONGEST", "RESPONSE", "CREAT_ATTR_TAB", "SET_ATTR_VAL", "SEND_SERVICE_CHANGE"};

static const char *gatts_evt_str(uint8_t event)
{
    if (event >= (sizeof(gatts_evt_names) / sizeof(*gatts_evt_names))) {
        return "UNKNOWN";
    }
    return gatts_evt_names[event];
}

static void add_db_record(esp_gatts_attr_db_t *db, size_t index, uint8_t *uuid, uint8_t perm, uint16_t max_length, uint16_t length, uint8_t *value)
{
    db[index].attr_control.auto_rsp = ESP_GATT_AUTO_RSP;
    db[index].att_desc.uuid_length = ESP_UUID_LEN_16;
    db[index].att_desc.uuid_p = uuid;
    db[index].att_desc.perm = perm;
    db[index].att_desc.max_length = max_length;
    db[index].att_desc.length = length;
    db[index].att_desc.value = value;
}

static esp_gatts_attr_db_t *_last_db = NULL;

static esp_err_t create_bat_db(esp_ble_hidd_dev_t *dev)
{
    _last_db = (esp_gatts_attr_db_t *)malloc(sizeof(esp_gatts_attr_db_t) * BAS_IDX_NB);
    if (!_last_db) {
        ESP_LOGE(TAG, "Malloc bat_db failed");
        return ESP_FAIL;
    }
    add_db_record(_last_db, BAS_IDX_SVC, (uint8_t *)&s_primary_service_uuid, ESP_GATT_PERM_READ, 2, 2, (uint8_t *)&s_bat_svc);
    add_db_record(_last_db, BAS_IDX_BATT_LVL_CHAR, (uint8_t *)&s_character_declaration_uuid, ESP_GATT_PERM_READ, 1, 1, (uint8_t *)&s_char_prop_read_notify);
    add_db_record(_last_db, BAS_IDX_BATT_LVL_VAL, (uint8_t *)&s_bat_level_uuid, ESP_GATT_PERM_READ, 1, 1, (uint8_t *)&dev->bat_level);
    add_db_record(_last_db, BAS_IDX_BATT_LVL_CCC, (uint8_t *)&s_character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, 2, 0,  NULL);
    add_db_record(_last_db, BAS_IDX_BATT_LVL_PRES_FMT, (uint8_t *)&s_bat_char_pres_format_uuid, ESP_GATT_PERM_READ, 7, 0,  NULL);
    esp_err_t err = esp_ble_gatts_create_attr_tab(_last_db, dev->bat_svc.gatt_if, BAS_IDX_NB, 0);
    return err;
}

static esp_err_t create_info_db(esp_ble_hidd_dev_t *dev)
{
    _last_db = (esp_gatts_attr_db_t *)malloc(sizeof(esp_gatts_attr_db_t) * 7);
    if (!_last_db) {
        ESP_LOGE(TAG, "Malloc info_db failed");
        return ESP_FAIL;
    }
    size_t index = 0;
    add_db_record(_last_db, index++, (uint8_t *)&s_primary_service_uuid, ESP_GATT_PERM_READ, 2, 2, (uint8_t *)&s_device_info_svc);

    if (dev->config.product_id || dev->config.vendor_id || dev->config.version) {
        uint8_t pnp_val[7] = {
            0x02, //0x1=BT, 0x2=USB
            dev->config.vendor_id & 0xFF, (dev->config.vendor_id >> 8) & 0xFF, //VID
                       dev->config.product_id & 0xFF, (dev->config.product_id >> 8) & 0xFF, //PID
                       dev->config.version & 0xFF, (dev->config.version >> 8) & 0xFF  //VERSION
        };
        memcpy(dev->pnp, pnp_val, 7);
        add_db_record(_last_db, index++, (uint8_t *)&s_character_declaration_uuid, ESP_GATT_PERM_READ, 1, 1, (uint8_t *)&s_char_prop_read);
        add_db_record(_last_db, index++, (uint8_t *)&s_device_info_pnp_id_uuid, ESP_GATT_PERM_READ, 7, 7, (uint8_t *)dev->pnp);
    }

    if (dev->config.manufacturer_name && dev->config.manufacturer_name[0]) {
        size_t name_len = strlen(dev->config.manufacturer_name);
        add_db_record(_last_db, index++, (uint8_t *)&s_character_declaration_uuid, ESP_GATT_PERM_READ, 1, 1, (uint8_t *)&s_char_prop_read);
        add_db_record(_last_db, index++, (uint8_t *)&s_device_info_manufacturer_uuid, ESP_GATT_PERM_READ, name_len, name_len, (uint8_t *)dev->config.manufacturer_name);
    }

    if (dev->config.serial_number && dev->config.serial_number[0]) {
        size_t name_len = strlen(dev->config.serial_number);
        add_db_record(_last_db, index++, (uint8_t *)&s_character_declaration_uuid, ESP_GATT_PERM_READ, 1, 1, (uint8_t *)&s_char_prop_read);
        add_db_record(_last_db, index++, (uint8_t *)&s_device_info_serial_num_uuid, ESP_GATT_PERM_READ, name_len, name_len, (uint8_t *)dev->config.serial_number);
    }

    esp_err_t err = esp_ble_gatts_create_attr_tab(_last_db, dev->info_svc.gatt_if, index, 0);
    return err;
}

static esp_err_t create_hid_db(esp_ble_hidd_dev_t *dev, int device_index)
{
    size_t report_attr_len = 0;
    for (uint8_t i = 0; i < dev->devices[device_index].reports_len; i++) {
        if (dev->devices[device_index].reports[i].report_type == ESP_HID_REPORT_TYPE_INPUT) {
            report_attr_len += 3;
        } else {
            report_attr_len += 2;
        }
        if (dev->devices[device_index].reports[i].protocol_mode == ESP_HID_PROTOCOL_MODE_REPORT) {
            report_attr_len += 1;
        }
    }

    _last_db = (esp_gatts_attr_db_t *)malloc(sizeof(esp_gatts_attr_db_t) * (HIDD_LE_IDX_NB + report_attr_len + (dev->devices_len * 3)));
    if (!_last_db) {
        ESP_LOGE(TAG, "Malloc hid_db failed");
        return ESP_FAIL;
    }

    add_db_record(_last_db, HIDD_LE_IDX_SVC, (uint8_t *)&s_primary_service_uuid, ESP_GATT_PERM_READ_ENCRYPTED, 2, 2, (uint8_t *)&s_hid_le_svc);
    add_db_record(_last_db, HIDD_LE_IDX_INCL_SVC, (uint8_t *)&s_include_service_uuid, ESP_GATT_PERM_READ, sizeof(esp_gatts_incl_svc_desc_t), sizeof(esp_gatts_incl_svc_desc_t), (uint8_t *)&dev->hid_incl_svc);

    add_db_record(_last_db, HIDD_LE_IDX_HID_INFO_CHAR, (uint8_t *)&s_character_declaration_uuid, ESP_GATT_PERM_READ, 1, 1, (uint8_t *)&s_char_prop_read);
    add_db_record(_last_db, HIDD_LE_IDX_HID_INFO_VAL, (uint8_t *)&s_hid_info_char_uuid, ESP_GATT_PERM_READ, 4, 4, (uint8_t *)hidInfo);

    add_db_record(_last_db, HIDD_LE_IDX_HID_CTNL_PT_CHAR, (uint8_t *)&s_character_declaration_uuid, ESP_GATT_PERM_READ, 1, 1, (uint8_t *)&s_char_prop_write_nr);
    add_db_record(_last_db, HIDD_LE_IDX_HID_CTNL_PT_VAL, (uint8_t *)&s_hid_control_point_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, 1, 0, NULL);

    add_db_record(_last_db, HIDD_LE_IDX_PROTO_MODE_CHAR, (uint8_t *)&s_character_declaration_uuid, ESP_GATT_PERM_READ, 1, 1, (uint8_t *)&s_char_prop_read_write_nr);
    add_db_record(_last_db, HIDD_LE_IDX_PROTO_MODE_VAL, (uint8_t *)&s_hid_proto_mode_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, 1, 1, (uint8_t *)&dev->protocol);

    add_db_record(_last_db, HIDD_LE_IDX_REPORT_MAP_CHAR, (uint8_t *)&s_character_declaration_uuid, ESP_GATT_PERM_READ, 1, 1, (uint8_t *)&s_char_prop_read);
    add_db_record(_last_db, HIDD_LE_IDX_REPORT_MAP_VAL, (uint8_t *)&s_hid_report_map_uuid, ESP_GATT_PERM_READ, HIDD_LE_REPORT_MAP_MAX_LEN, dev->devices[device_index].reports_map.len, (uint8_t *)dev->devices[device_index].reports_map.data);
    add_db_record(_last_db, HIDD_LE_IDX_REPORT_MAP_EXT_REP_REF, (uint8_t *)&s_hid_report_map_ext_desc_uuid, ESP_GATT_PERM_READ, 2, 2, (uint8_t *)&s_bat_level_uuid);

    size_t index = HIDD_LE_IDX_NB;

    for (uint8_t i = 0; i < dev->devices[device_index].reports_len; i++) {
        hidd_le_report_item_t *report = &dev->devices[device_index].reports[i];
        if (report->protocol_mode == ESP_HID_PROTOCOL_MODE_REPORT) {
            if (report->report_type == ESP_HID_REPORT_TYPE_INPUT) {
                //Input Report
                add_db_record(_last_db, index++, (uint8_t *)&s_character_declaration_uuid, ESP_GATT_PERM_READ, 1, 1, (uint8_t *)&s_char_prop_read_notify);
                report->index = index;
                add_db_record(_last_db, index++, (uint8_t *)&s_hid_report_uuid, ESP_GATT_PERM_READ, report->value_len, 0, NULL);
                add_db_record(_last_db, index++, (uint8_t *)&s_character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE_ENCRYPTED, 2, 0, NULL);
            } else if (report->report_type == ESP_HID_REPORT_TYPE_OUTPUT) {
                //Output Report
                add_db_record(_last_db, index++, (uint8_t *)&s_character_declaration_uuid, ESP_GATT_PERM_READ, 1, 1, (uint8_t *)&s_char_prop_read_write_write_nr);
                report->index = index;
                add_db_record(_last_db, index++, (uint8_t *)&s_hid_report_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, report->value_len, 0, NULL);
            } else {
                //Feature Report
                add_db_record(_last_db, index++, (uint8_t *)&s_character_declaration_uuid, ESP_GATT_PERM_READ, 1, 1, (uint8_t *)&s_char_prop_read_write);
                report->index = index;
                add_db_record(_last_db, index++, (uint8_t *)&s_hid_report_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, report->value_len, 0, NULL);
            }
            add_db_record(_last_db, index++, (uint8_t *)&s_hid_report_ref_descr_uuid, ESP_GATT_PERM_READ, 2, 2, (uint8_t *)&report->report_id);
        } else {
            if (report->report_type == ESP_HID_REPORT_TYPE_INPUT) {
                add_db_record(_last_db, index++, (uint8_t *)&s_character_declaration_uuid, ESP_GATT_PERM_READ, 1, 1, (uint8_t *)&s_char_prop_read_notify);
                report->index = index;
                if (report->usage == ESP_HID_USAGE_KEYBOARD) { //Boot Keyboard Input
                    add_db_record(_last_db, index++, (uint8_t *)&s_hid_boot_kb_input_uuid, ESP_GATT_PERM_READ, HIDD_LE_BOOT_REPORT_MAX_LEN, 0, NULL);
                } else { //Boot Mouse Input
                    add_db_record(_last_db, index++, (uint8_t *)&s_hid_boot_mouse_input_uuid, ESP_GATT_PERM_READ, HIDD_LE_BOOT_REPORT_MAX_LEN, 0, NULL);
                }
                add_db_record(_last_db, index++, (uint8_t *)&s_character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, 2, 0, NULL);
            } else { //Boot Keyboard Output
                add_db_record(_last_db, index++, (uint8_t *)&s_character_declaration_uuid, ESP_GATT_PERM_READ, 1, 1, (uint8_t *)&s_char_prop_read_write_write_nr);
                report->index = index;
                add_db_record(_last_db, index++, (uint8_t *)&s_hid_boot_kb_output_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, HIDD_LE_BOOT_REPORT_MAX_LEN, 0, NULL);
            }
        }

    }
    esp_err_t err = esp_ble_gatts_create_attr_tab(_last_db, dev->devices[device_index].hid_svc.gatt_if, index, device_index);
    return err;
}

static void link_report_handles(hidd_dev_map_t *dev, uint16_t *handles)
{

    hidd_le_report_item_t *rpt = NULL;
    for (uint8_t i = 0; i < dev->reports_len; i++) {
        rpt = &dev->reports[i];
        rpt->handle = handles[rpt->index];
        if (rpt->report_type == ESP_HID_REPORT_TYPE_INPUT) {
            rpt->ccc_handle = handles[rpt->index + 1];
        }
    }
}

static hidd_le_report_item_t *get_report_by_handle(esp_ble_hidd_dev_t *dev, uint16_t handle)
{
    hidd_le_report_item_t *rpt = NULL;
    for (uint8_t d = 0; d < dev->devices_len; d++) {
        for (uint8_t i = 0; i < dev->devices[d].reports_len; i++) {
            rpt = &dev->devices[d].reports[i];
            if (rpt->handle == handle || rpt->ccc_handle == handle) {
                return rpt;
            }
        }
    }
    return NULL;
}

static hidd_le_report_item_t *get_report_by_id_and_type(esp_ble_hidd_dev_t *dev, uint8_t id, uint8_t type)
{
    hidd_le_report_item_t *rpt = NULL;
    for (uint8_t d = 0; d < dev->devices_len; d++) {
        for (uint8_t i = 0; i < dev->devices[d].reports_len; i++) {
            rpt = &dev->devices[d].reports[i];
            if (rpt->report_id == id && rpt->report_type == type && rpt->protocol_mode == ESP_HID_PROTOCOL_MODE_REPORT) {
                return rpt;
            }
        }
    }
    return NULL;
}

static void bat_event_handler(esp_ble_hidd_dev_t *dev, esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    switch (event) {
    case ESP_GATTS_REG_EVT:
        ESP_LOGV(TAG, "Battery REG App ID: 0x%x", param->reg.app_id);
        break;
    case ESP_GATTS_CREAT_ATTR_TAB_EVT:
        dev->bat_svc.handle = param->add_attr_tab.handles[BAS_IDX_SVC];
        dev->bat_level_handle = param->add_attr_tab.handles[BAS_IDX_BATT_LVL_VAL];//so we notify of the change
        dev->bat_ccc_handle = param->add_attr_tab.handles[BAS_IDX_BATT_LVL_CCC];//so we know if we can send notify
        ESP_LOGV(TAG, "Battery CREAT_ATTR_TAB service handle = %d", dev->bat_svc.handle);

        dev->hid_incl_svc.start_hdl = dev->bat_svc.handle;
        dev->hid_incl_svc.end_hdl = dev->bat_svc.handle + BAS_IDX_NB - 1;

        esp_ble_gatts_start_service(dev->bat_svc.handle);

        // Add the info service next, because it's shared between all device maps
        create_info_db(dev);
        break;

    case ESP_GATTS_READ_EVT:
        if (param->read.handle == dev->bat_level_handle) {
            ESP_LOGD(TAG, "Battery READ %d", dev->bat_level);
        }
        break;
    case ESP_GATTS_WRITE_EVT: {
        if (param->write.handle == dev->bat_ccc_handle) {
            dev->bat_ccc.value = param->write.value[0];
            ESP_LOGV(TAG, "Battery CCC: Notify: %s, Indicate: %s", dev->bat_ccc.notify_enable ? "On" : "Off", dev->bat_ccc.indicate_enable ? "On" : "Off");
        }
        break;
    }
    case ESP_GATTS_SET_ATTR_VAL_EVT: {
        if (param->set_attr_val.attr_handle == dev->bat_level_handle) {
            ESP_LOGD(TAG, "Battery SET %d, status: 0x%02x", dev->bat_level, param->set_attr_val.status);
        }
        break;
    }
    default:
        ESP_LOGV(TAG, "Battery %s", gatts_evt_str(event));
        break;
    }
}

static void info_event_handler(esp_ble_hidd_dev_t *dev, esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    switch (event) {
    case ESP_GATTS_REG_EVT:
        ESP_LOGV(TAG, "Dev Info REG App ID: 0x%x", param->reg.app_id);
        break;
    case ESP_GATTS_CREAT_ATTR_TAB_EVT:
        dev->info_svc.handle = param->add_attr_tab.handles[0];
        ESP_LOGV(TAG, "Dev Info service handle = %d", dev->info_svc.handle);
        esp_ble_gatts_start_service(dev->info_svc.handle);
        create_hid_db(dev, 0);
        break;

    default:
        ESP_LOGV(TAG, "Dev Info %s", gatts_evt_str(event));
        break;
    }
}

static void hid_event_handler(esp_ble_hidd_dev_t *dev, int device_index, esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    switch (event) {
    case ESP_GATTS_REG_EVT: {
        ESP_LOGV(TAG, "HID REG[%d] App ID: 0x%x", device_index, param->reg.app_id);
        // is this the last report map app?
        if (device_index == (dev->devices_len - 1)) {
            // we should add the battery service first, because the hid service should include the battery service.
            create_bat_db(dev);
        }
        break;
    }
    case ESP_GATTS_CREAT_ATTR_TAB_EVT: {
        dev->devices[device_index].hid_svc.handle = param->add_attr_tab.handles[HIDD_LE_IDX_SVC];
        dev->devices[device_index].hid_control_handle = param->add_attr_tab.handles[HIDD_LE_IDX_HID_CTNL_PT_VAL];
        dev->devices[device_index].hid_protocol_handle = param->add_attr_tab.handles[HIDD_LE_IDX_PROTO_MODE_VAL];
        ESP_LOGV(TAG, "HID CREAT_ATTR_TAB[%u] service handle = %d", device_index, dev->devices[device_index].hid_svc.handle);

        link_report_handles(&dev->devices[device_index], param->add_attr_tab.handles);
        esp_ble_gatts_start_service(dev->devices[device_index].hid_svc.handle);
        if ((device_index + 1) < dev->devices_len) {
            create_hid_db(dev, device_index + 1);//add next device if support
        }
        break;
    }
    case ESP_GATTS_START_EVT: {
        ESP_LOGD(TAG, "HID START[%d] status: 0x%02x", device_index, param->start.status);
        if (device_index == (dev->devices_len - 1)) {
            esp_event_post_to(dev->event_loop_handle, ESP_HIDD_EVENTS, ESP_HIDD_START_EVENT, NULL, 0, portMAX_DELAY);
        }
        break;
    }
    case ESP_GATTS_CONNECT_EVT: {
        ESP_LOGD(TAG, "HID CONNECT[%d] conn_id = %x", device_index, param->connect.conn_id);
        if (!dev->connected && device_index == (dev->devices_len - 1)) {
            dev->connected = true;
            dev->conn_id   = param->connect.conn_id;
            memcpy(dev->remote_bda, param->connect.remote_bda, ESP_BD_ADDR_LEN);

            esp_ble_set_encryption(param->connect.remote_bda, ESP_BLE_SEC_ENCRYPT_NO_MITM);

            esp_hidd_event_data_t cb_param = {
                .connect.dev = dev->dev,
            };
            esp_event_post_to(dev->event_loop_handle, ESP_HIDD_EVENTS, ESP_HIDD_CONNECT_EVENT, &cb_param, sizeof(esp_hidd_event_data_t), portMAX_DELAY);
        }
        break;
    }
    case ESP_GATTS_DISCONNECT_EVT: {
        ESP_LOGD(TAG, "HID DISCONNECT[%d] 0x%x", device_index, param->disconnect.reason);
        if (dev->connected) {
            dev->connected = false;
            esp_hidd_event_data_t cb_param = {0};
            cb_param.disconnect.dev = dev->dev;
            cb_param.disconnect.reason = param->disconnect.reason;
            esp_event_post_to(dev->event_loop_handle, ESP_HIDD_EVENTS, ESP_HIDD_DISCONNECT_EVENT, &cb_param, sizeof(esp_hidd_event_data_t), portMAX_DELAY);
        }
        break;
    }
    case ESP_GATTS_READ_EVT: {
        hidd_le_report_item_t *map = get_report_by_handle(dev, param->read.handle);
        if (map && map->handle == param->read.handle) {
            ESP_LOGV(TAG, "HID READ[%d] %8s %7s %6s id: %d, need_resp: %d", device_index, esp_hid_usage_str(map->usage), esp_hid_report_type_str(map->report_type), esp_hid_protocol_mode_str(map->protocol_mode), map->report_id, param->read.need_rsp);
        }
        break;
    }
    case ESP_GATTS_WRITE_EVT: {

        if (param->write.handle == dev->devices[device_index].hid_control_handle) {
            dev->control = param->write.value[0];

            esp_hidd_event_data_t cb_param = {0};
            cb_param.control.dev = dev->dev;
            cb_param.control.control = dev->control;
            cb_param.control.map_index = device_index;
            esp_event_post_to(dev->event_loop_handle, ESP_HIDD_EVENTS, ESP_HIDD_CONTROL_EVENT, &cb_param, sizeof(esp_hidd_event_data_t), portMAX_DELAY);
        } else if (param->write.handle == dev->devices[device_index].hid_protocol_handle) {
            dev->protocol = param->write.value[0];

            esp_hidd_event_data_t cb_param = {};
            cb_param.protocol_mode.dev = dev->dev;
            cb_param.protocol_mode.protocol_mode = dev->protocol;
            cb_param.protocol_mode.map_index = device_index;
            esp_event_post_to(dev->event_loop_handle, ESP_HIDD_EVENTS, ESP_HIDD_PROTOCOL_MODE_EVENT, &cb_param, sizeof(esp_hidd_event_data_t), portMAX_DELAY);
        } else {
            hidd_le_report_item_t *map = get_report_by_handle(dev, param->write.handle);
            if (map) {
                if (param->write.handle == map->ccc_handle) {
                    map->ccc.value = param->write.value[0];
                    ESP_LOGV(TAG, "HID CCC[%d] %8s %7s %6s id: %d, Notify: %s, Indicate: %s", device_index, esp_hid_usage_str(map->usage), esp_hid_report_type_str(map->report_type), esp_hid_protocol_mode_str(map->protocol_mode), map->report_id, map->ccc.notify_enable ? "On" : "Off", map->ccc.indicate_enable ? "On" : "Off");
                } else {
                    ESP_LOGV(TAG, "HID WRITE %8s %7s %6s id: %d, len: %d", esp_hid_usage_str(map->usage), esp_hid_report_type_str(map->report_type), esp_hid_protocol_mode_str(map->protocol_mode), map->report_id, param->write.len);

                    esp_hidd_event_data_t *p_cb_param = NULL;
                    size_t event_data_size = sizeof(esp_hidd_event_data_t);
                    if (param->write.len > 0 && param->write.value) {
                        event_data_size += param->write.len;
                    }

                    if ((p_cb_param = (esp_hidd_event_data_t *)malloc(event_data_size)) == NULL) {
                        ESP_LOGE(TAG, "%s malloc event data failed!", __func__);
                        break;
                    }
                    memset(p_cb_param, 0, event_data_size);
                    if (param->write.len > 0 && param->write.value) {
                        memcpy(((uint8_t *)p_cb_param) + sizeof(esp_hidd_event_data_t), param->write.value,
                               param->write.len);
                    }

                    if (map->report_type == ESP_HID_REPORT_TYPE_OUTPUT) {
                        p_cb_param->output.dev = dev->dev;
                        p_cb_param->output.report_id = map->report_id;
                        p_cb_param->output.usage = map->usage;
                        p_cb_param->output.length = param->write.len;
                        p_cb_param->output.data = param->write.value; /* move the data pointer in the wrapper loop handler */
                        p_cb_param->output.map_index = device_index;
                        esp_event_post_to(dev->event_loop_handle, ESP_HIDD_EVENTS, ESP_HIDD_OUTPUT_EVENT, p_cb_param, event_data_size, portMAX_DELAY);
                    } else {
                        p_cb_param->feature.dev = dev->dev;
                        p_cb_param->feature.report_id = map->report_id;
                        p_cb_param->feature.usage = map->usage;
                        p_cb_param->feature.length = param->write.len;
                        p_cb_param->feature.data = param->write.value; /* move the data pointer in the wrapper loop handler */
                        p_cb_param->feature.map_index = device_index;
                        esp_event_post_to(dev->event_loop_handle, ESP_HIDD_EVENTS, ESP_HIDD_FEATURE_EVENT, p_cb_param, event_data_size, portMAX_DELAY);
                    }

                    if (p_cb_param) {
                        free(p_cb_param);
                        p_cb_param = NULL;
                    }
                }
            }

        }
        break;
    }
    case ESP_GATTS_SET_ATTR_VAL_EVT: {
        hidd_le_report_item_t *map = get_report_by_handle(dev, param->set_attr_val.attr_handle);
        if (map && map->handle == param->set_attr_val.attr_handle) {
            ESP_LOGV(TAG, "HID SET[%d] %8s %7s %6s id: %d, status: 0x%02x", device_index, esp_hid_usage_str(map->usage), esp_hid_report_type_str(map->report_type), esp_hid_protocol_mode_str(map->protocol_mode), map->report_id, param->set_attr_val.status);
        }
        SEND_CB(dev);
        break;
    }
    case ESP_GATTS_CONF_EVT: {
        ESP_LOGV(TAG, "HID CONF[%d] status: 0x%02x, len: %d", device_index, param->conf.status, param->conf.len);
        SEND_CB(dev);
        break;
    }
    case ESP_GATTS_MTU_EVT:
        ESP_LOGV(TAG, "HID[%d] MTU = %d", device_index, param->mtu.mtu);
        break;

    default:
        ESP_LOGV(TAG, "HID[%d] %s", device_index, gatts_evt_str(event));
        break;
    }
}

static int get_device_map_index_by_gatts_if(esp_ble_hidd_dev_t *dev, esp_gatt_if_t gatts_if)
{
    for (uint8_t d = 0; d < dev->devices_len; d++) {
        if (dev->devices[d].hid_svc.gatt_if && gatts_if == dev->devices[d].hid_svc.gatt_if) {
            return d;
        }
    }
    return -1;
}

static esp_err_t ble_hid_start_gatts(esp_ble_hidd_dev_t *dev)
{
    esp_err_t ret;

    if ((ret = esp_ble_gatts_app_register(ESP_GATT_UUID_BATTERY_SERVICE_SVC)) != ESP_OK) {
        ESP_LOGE(TAG, "GATTS register battery service failed: %d", ret);
        return ret;
    }
    if ((ret = esp_ble_gatts_app_register(ESP_GATT_UUID_DEVICE_INFO_SVC)) != ESP_OK) {
        ESP_LOGE(TAG, "GATTS register device info service failed: %d", ret);
        return ret;
    }
    for (uint8_t i = 0; i < dev->devices_len; i++) {
        if ((ret = esp_ble_gatts_app_register(ESP_GATT_UUID_HID_SVC + i)) != ESP_OK) {
            ESP_LOGE(TAG, "GATTS register HID[%u] service failed: %d", i, ret);
            return ret;
        }
    }
    return ret;
}

static esp_err_t ble_hid_stop_gatts(esp_ble_hidd_dev_t *dev)
{
    esp_err_t ret = ESP_OK;

    for (uint8_t d = 0; d < dev->devices_len; d++) {
        esp_ble_gatts_stop_service(dev->devices[d].hid_svc.handle);
        esp_ble_gatts_delete_service(dev->devices[d].hid_svc.handle);
        esp_ble_gatts_app_unregister(dev->devices[d].hid_svc.gatt_if);
    }

    esp_ble_gatts_stop_service(dev->info_svc.handle);
    esp_ble_gatts_delete_service(dev->info_svc.handle);
    esp_ble_gatts_app_unregister(dev->info_svc.gatt_if);

    esp_ble_gatts_stop_service(dev->bat_svc.handle);
    esp_ble_gatts_delete_service(dev->bat_svc.handle);
    esp_ble_gatts_app_unregister(dev->bat_svc.gatt_if);

    return ret;
}

static esp_err_t ble_hid_init_config(esp_ble_hidd_dev_t *dev, const esp_hid_device_config_t *config)
{
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
            return ESP_FAIL;
        }
        memset(dev->devices, 0, config->report_maps_len * sizeof(hidd_dev_map_t));
        dev->devices_len = config->report_maps_len;
        for (uint8_t d = 0; d < dev->devices_len; d++) {

            //raw report map
            uint8_t *map = (uint8_t *)malloc(config->report_maps[d].len);
            if (map == NULL) {
                ESP_LOGE(TAG, "report map malloc(%d) failed", config->report_maps[d].len);
                return ESP_FAIL;
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
            dev->devices[d].reports = (hidd_le_report_item_t *)malloc(rmap->reports_len * sizeof(hidd_le_report_item_t));
            if (dev->devices[d].reports == NULL) {
                ESP_LOGE(TAG, "reports malloc(%d) failed", rmap->reports_len * sizeof(hidd_le_report_item_t));
                free(rmap);
                return ESP_FAIL;
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

static esp_err_t ble_hid_free_config(esp_ble_hidd_dev_t *dev)
{
    for (uint8_t d = 0; d < dev->devices_len; d++) {
        free((void *)dev->devices[d].reports);
        free((void *)dev->devices[d].reports_map.data);
    }

    free((void *)dev->devices);
    free((void *)dev->config.device_name);
    free((void *)dev->config.manufacturer_name);
    free((void *)dev->config.serial_number);
    if (dev->sem != NULL) {
        vSemaphoreDelete(dev->sem);
    }
    if (dev->event_loop_handle != NULL) {
        esp_event_loop_delete(dev->event_loop_handle);
    }
    return ESP_OK;
}

/*
 * PUBLIC FUNCTIONS
 * */

// there can be only one BLE HID device
static esp_ble_hidd_dev_t *s_dev = NULL;

void esp_hidd_gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    if (!s_dev) {
        return;
    }

    if (event == ESP_GATTS_REG_EVT) {
        if (param->reg.status != ESP_GATT_OK) {
            ESP_LOGE(TAG, "Reg app failed, app_id %04x, status %d", param->reg.app_id, param->reg.status);
            return;
        } else {
            if (param->reg.app_id == ESP_GATT_UUID_DEVICE_INFO_SVC) {
                s_dev->info_svc.gatt_if = gatts_if;
            } else if (param->reg.app_id == ESP_GATT_UUID_BATTERY_SERVICE_SVC) {
                s_dev->bat_svc.gatt_if = gatts_if;
            } else if (param->reg.app_id >= ESP_GATT_UUID_HID_SVC && param->reg.app_id < (ESP_GATT_UUID_HID_SVC + s_dev->devices_len)) {
                ESP_LOGV(TAG, "HID SVC[%u] IF: %d", param->reg.app_id - ESP_GATT_UUID_HID_SVC, gatts_if);
                s_dev->devices[param->reg.app_id - ESP_GATT_UUID_HID_SVC].hid_svc.gatt_if = gatts_if;
            } else {
                ESP_LOGE(TAG, "Unknown Application, app_id %04x", param->reg.app_id);
                return;
            }
        }
    } else if (event == ESP_GATTS_CREAT_ATTR_TAB_EVT) {
        free(_last_db);
        _last_db = NULL;
    }

    if (s_dev->bat_svc.gatt_if && gatts_if == s_dev->bat_svc.gatt_if) {
        bat_event_handler(s_dev, event, gatts_if, param);
    } else if (s_dev->info_svc.gatt_if && gatts_if == s_dev->info_svc.gatt_if) {
        info_event_handler(s_dev, event, gatts_if, param);
    } else {
        int devi = get_device_map_index_by_gatts_if(s_dev, gatts_if);
        if (devi >= 0) {
            hid_event_handler(s_dev, devi, event, gatts_if, param);
        } else {
            ESP_LOGE(TAG, "Unknown gatts_if %u", gatts_if);
        }
        return;
    }
}

static esp_err_t esp_ble_hidd_dev_deinit(void *devp)
{
    esp_ble_hidd_dev_t *dev = (esp_ble_hidd_dev_t *)devp;
    if (!s_dev) {
        ESP_LOGE(TAG, "HID device profile already uninitialized");
        return ESP_OK;
    }

    if (s_dev != dev) {
        ESP_LOGE(TAG, "Wrong HID device provided");
        return ESP_FAIL;
    }
    s_dev = NULL;

    ble_hid_stop_gatts(dev);
    esp_event_post_to(dev->event_loop_handle, ESP_HIDD_EVENTS, ESP_HIDD_STOP_EVENT, NULL, 0, portMAX_DELAY);
    ble_hid_free_config(dev);
    free(dev);
    return ESP_OK;
}

static bool esp_ble_hidd_dev_connected(void *devp)
{
    esp_ble_hidd_dev_t *dev = (esp_ble_hidd_dev_t *)devp;
    return (dev != NULL && s_dev == dev && dev->connected);
}

static esp_err_t esp_ble_hidd_dev_battery_set(void *devp, uint8_t level)
{
    esp_err_t ret;
    esp_ble_hidd_dev_t *dev = (esp_ble_hidd_dev_t *)devp;
    if (!dev || s_dev != dev) {
        return ESP_FAIL;
    }
    dev->bat_level = level;

    if (!dev->connected || dev->bat_ccc.value == 0) {
        //if we are not yet connected, that is not an error
        return ESP_OK;
    }

    if (dev->bat_ccc.notify_enable) {
        ret = esp_ble_gatts_send_indicate(dev->bat_svc.gatt_if, dev->conn_id, dev->bat_level_handle, 1, &dev->bat_level, false);
        if (ret) {
            ESP_LOGE(TAG, "esp_ble_gatts_send_notify failed: %d", ret);
            return ESP_FAIL;
        }
    }

    return ESP_OK;
}

static esp_err_t esp_ble_hidd_dev_input_set(void *devp, size_t index, size_t id, uint8_t *data, size_t length)
{
    hidd_le_report_item_t *p_rpt;
    esp_ble_hidd_dev_t *dev = (esp_ble_hidd_dev_t *)devp;
    if (!dev || s_dev != dev) {
        return ESP_FAIL;
    }

    if (!dev->connected) {
        ESP_LOGE(TAG, "%s Device Not Connected", __func__);
        return ESP_FAIL;
    }

    if (index >= dev->devices_len) {
        ESP_LOGE(TAG, "%s index out of range[0-%d]", __func__, dev->devices_len - 1);
        return ESP_FAIL;
    }

    if ((p_rpt = get_report_by_id_and_type(dev, id, ESP_HID_REPORT_TYPE_INPUT)) != NULL && p_rpt->ccc.value) {
        esp_err_t err = esp_ble_gatts_send_indicate(dev->devices[index].hid_svc.gatt_if, dev->conn_id, p_rpt->handle, length, data, p_rpt->ccc.indicate_enable);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Send Input Indicate Failed: %d", err);
            return ESP_FAIL;
        }
        WAIT_CB(dev);
    } else {
        ESP_LOGE(TAG, "Indicate Not Enabled: %d", 0);
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t esp_ble_hidd_dev_feature_set(void *devp, size_t index, size_t id, uint8_t *data, size_t length)
{
    esp_err_t ret;
    esp_ble_hidd_dev_t *dev = (esp_ble_hidd_dev_t *)devp;
    if (!dev || s_dev != dev) {
        return ESP_FAIL;
    }

    if (!dev->connected) {
        ESP_LOGE(TAG, "%s Device Not Connected", __func__);
        return ESP_FAIL;
    }

    if (index >= dev->devices_len) {
        ESP_LOGE(TAG, "%s index out of range[0-%d]", __func__, dev->devices_len - 1);
        return ESP_FAIL;
    }

    hidd_le_report_item_t *p_rpt;
    if ((p_rpt = get_report_by_id_and_type(dev, id, ESP_HID_REPORT_TYPE_FEATURE)) != NULL) {
        ret = esp_ble_gatts_set_attr_value(p_rpt->handle, length, data);
        if (ret) {
            ESP_LOGE(TAG, "esp_ble_gatts_set_attr_value failed: %d", ret);
            return ESP_FAIL;
        }
        WAIT_CB(dev);
        if (dev->connected && p_rpt->ccc.value) {
            ret = esp_ble_gatts_send_indicate(dev->devices[index].hid_svc.gatt_if, dev->conn_id, p_rpt->handle, length, data, p_rpt->ccc.indicate_enable);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Send Feature Indicate Failed: %d", ret);
                return ESP_FAIL;
            }
            WAIT_CB(dev);
        }
    } else {
        ESP_LOGE(TAG, "FEATURE %d not found", id);
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t esp_ble_hidd_dev_event_handler_register(void *devp, esp_event_handler_t callback, esp_hidd_event_t event)
{
    esp_ble_hidd_dev_t *dev = (esp_ble_hidd_dev_t *)devp;
    if (!dev || s_dev != dev) {
        return ESP_FAIL;
    }
    return esp_event_handler_register_with(dev->event_loop_handle, ESP_HIDD_EVENTS, event, callback, dev->dev);
}

static esp_err_t esp_ble_hidd_dev_event_handler_unregister(void *devp, esp_event_handler_t callback, esp_hidd_event_t event)
{
    esp_ble_hidd_dev_t *dev = (esp_ble_hidd_dev_t *)devp;
    if (!dev || s_dev != dev) {
        return ESP_FAIL;
    }
    return esp_event_handler_unregister_with(dev->event_loop_handle, ESP_HIDD_EVENTS, event, callback);
}

static void ble_hidd_dev_free(void)
{
    if (s_dev) {
        ble_hid_free_config(s_dev);
        free(s_dev);
        s_dev = NULL;
    }
}

esp_err_t esp_ble_hidd_dev_init(esp_hidd_dev_t *dev_p, const esp_hid_device_config_t *config, esp_event_handler_t callback)
{
    esp_err_t ret;

    if (s_dev) {
        ESP_LOGE(TAG, "HID device profile already initialized");
        return ESP_FAIL;
    }

    s_dev = (esp_ble_hidd_dev_t *)calloc(1, sizeof(esp_ble_hidd_dev_t));
    if (s_dev == NULL) {
        ESP_LOGE(TAG, "HID device could not be allocated");
        return ESP_FAIL;
    }

    // Reset the hid device target environment
    s_dev->bat_level = 100;
    s_dev->bat_ccc.value = 0;
    s_dev->control = ESP_HID_CONTROL_EXIT_SUSPEND;
    s_dev->protocol = ESP_HID_PROTOCOL_MODE_REPORT;
    s_dev->event_loop_handle = NULL;
    s_dev->dev = dev_p;
    s_dev->sem = xSemaphoreCreateBinary();
    if (s_dev->sem == NULL) {
        ESP_LOGE(TAG, "HID device semaphore could not be allocated");
        ble_hidd_dev_free();
        return ESP_FAIL;
    }

    esp_event_loop_args_t event_task_args = {
        .queue_size = 5,
        .task_name = "ble_hidd_events",
        .task_priority = uxTaskPriorityGet(NULL),
        .task_stack_size = 4096,
        .task_core_id = tskNO_AFFINITY
    };
    ret = esp_event_loop_create(&event_task_args, &s_dev->event_loop_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "HID device event loop could not be created");
        ble_hidd_dev_free();
        return ret;
    }

    ret = ble_hid_init_config(s_dev, config);
    if (ret != ESP_OK) {
        ble_hidd_dev_free();
        return ret;
    }

    dev_p->dev = s_dev;
    dev_p->connected = esp_ble_hidd_dev_connected;
    dev_p->deinit = esp_ble_hidd_dev_deinit;
    dev_p->battery_set = esp_ble_hidd_dev_battery_set;
    dev_p->input_set = esp_ble_hidd_dev_input_set;
    dev_p->feature_set = esp_ble_hidd_dev_feature_set;
    dev_p->event_handler_register = esp_ble_hidd_dev_event_handler_register;
    dev_p->event_handler_unregister = esp_ble_hidd_dev_event_handler_unregister;

    ret = esp_ble_hidd_dev_event_handler_register(s_dev, esp_hidd_process_event_data_handler, ESP_EVENT_ANY_ID);
    if (ret != ESP_OK) {
        ble_hidd_dev_free();
        return ret;
    }

    if (callback != NULL) {
        ret = esp_ble_hidd_dev_event_handler_register(s_dev, callback, ESP_EVENT_ANY_ID);
        if (ret != ESP_OK) {
            ble_hidd_dev_free();
            return ret;
        }
    }

    ret = ble_hid_start_gatts(s_dev);
    if (ret != ESP_OK) {
        ble_hidd_dev_free();
        return ret;
    }

    return ret;
}

#endif /* CONFIG_GATTS_ENABLE */
