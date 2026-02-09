/*
 * SPDX-FileCopyrightText: 2017-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdbool.h>

#include "ble_hidd.h"
#include "esp_private/esp_hidd_private.h"
#include "esp_log.h"

#include <assert.h>
#include <string.h>
#include <errno.h>
#include "nimble/nimble_opt.h"
#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "host/ble_hs_adv.h"
#include "host/ble_hs_hci.h"
#include "host/ble_att.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "services/bas/ble_svc_bas.h"
#include "services/hid/ble_svc_hid.h"
#include "services/dis/ble_svc_dis.h"
#include "services/sps/ble_svc_sps.h"

#if CONFIG_BT_NIMBLE_HID_SERVICE

static const char *TAG = "NIMBLE_HIDD";
#define BLE_SVC_BAS_UUID16                    0x180F

typedef struct esp_ble_hidd_dev_s esp_ble_hidd_dev_t;
// there can be only one BLE HID device
static esp_ble_hidd_dev_t *s_dev = NULL;
/** service index is used to identify the hid service instance
    of the registered characteristic.
    Assuming the first instance of the hid service is registered first.
    Increment service index as the hid services get registered */
static int service_index = -1;

typedef hidd_report_item_t hidd_le_report_item_t;

typedef struct {
    esp_hid_raw_report_map_t    reports_map;
    uint8_t                     reports_len;
    hidd_le_report_item_t      *reports;
    uint16_t                    hid_svc;
    uint16_t                    hid_control_handle;
    uint16_t                    hid_protocol_handle;
} hidd_dev_map_t;

struct esp_ble_hidd_dev_s {
    esp_hidd_dev_t             *dev;
    esp_event_loop_handle_t     event_loop_handle;
    esp_hid_device_config_t     config;
    uint16_t                    appearance;

    bool                        connected;
    uint16_t                    conn_id;

    uint8_t                     control;    // 0x00 suspend, 0x01 suspend off
    uint8_t                     protocol;   // 0x00 boot, 0x01 report

    uint16_t                    bat_svc_handle;
    uint16_t                    info_svc_handle;
    struct ble_gatt_svc         hid_incl_svc;

    uint16_t                    bat_level_handle;
    uint8_t                     pnp[7]; /* something related to device info service */
    hidd_dev_map_t             *devices;
    uint8_t                     devices_len;
};

// HID Information characteristic value
static const uint8_t hidInfo[4] = {
    0x11, 0x01,     // bcdHID (USB HID version)
    0x00,           // bCountryCode
    ESP_HID_FLAGS_REMOTE_WAKE | ESP_HID_FLAGS_NORMALLY_CONNECTABLE   // Flags
};

static int create_hid_db(int device_index)
{
    int rc = 0;
    struct ble_svc_hid_params hparams = {0};
    int report_mode_rpts = 0;

    /* fill hid info */
    memcpy(&hparams.hid_info, hidInfo, sizeof hparams.hid_info);

    /* fill report map */
    memcpy(&hparams.report_map, (uint8_t *)s_dev->devices[device_index].reports_map.data, s_dev->devices[device_index].reports_map.len);
    hparams.report_map_len = s_dev->devices[device_index].reports_map.len;
    hparams.external_rpt_ref = BLE_SVC_BAS_UUID16;

    /* fill protocol mode */
    hparams.proto_mode_present = 1;
    hparams.proto_mode = s_dev->protocol;

    for (uint8_t i = 0; i < s_dev->devices[device_index].reports_len; i++) {
        hidd_le_report_item_t *report = &s_dev->devices[device_index].reports[i];
        if (report->protocol_mode == ESP_HID_PROTOCOL_MODE_REPORT) {
            /* only consider report mode reports, all boot mode reports will be registered by default */
            if (report->report_type == ESP_HID_REPORT_TYPE_INPUT) {
                /* Input Report */
                hparams.rpts[report_mode_rpts].type = ESP_HID_REPORT_TYPE_INPUT;
            } else if (report->report_type == ESP_HID_REPORT_TYPE_OUTPUT) {
                /* Output Report */
                hparams.rpts[report_mode_rpts].type = ESP_HID_REPORT_TYPE_OUTPUT;
            } else {
                /* Feature Report */
                hparams.rpts[report_mode_rpts].type = ESP_HID_REPORT_TYPE_FEATURE;
            }
            hparams.rpts[report_mode_rpts].id = report->report_id;
            report_mode_rpts++;
        } else {
            if (report->report_type == ESP_HID_REPORT_TYPE_INPUT) {
                /* Boot mode reports */
                if (report->usage == ESP_HID_USAGE_KEYBOARD) { //Boot Keyboard Input
                    hparams.kbd_inp_present = 1;
                } else { //Boot Mouse Input
                    hparams.mouse_inp_present = 1;
                }
            } else { //Boot Keyboard Output
                hparams.kbd_out_present = 1;
            }
        }
    }
    hparams.rpts_len = report_mode_rpts;
    /* Add service */
    rc = ble_svc_hid_add(hparams);
    if (rc != 0) {
        return rc;
    }
    return rc;
}

static int ble_hid_create_info_db(void)
{
    int rc;

    rc = 0;
    ble_svc_dis_init();
    uint8_t pnp_val[7] = {
        0x02, //0x1=BT, 0x2=USB
        s_dev->config.vendor_id & 0xFF, (s_dev->config.vendor_id >> 8) & 0xFF, //VID
                     s_dev->config.product_id & 0xFF, (s_dev->config.product_id >> 8) & 0xFF, //PID
                     s_dev->config.version & 0xFF, (s_dev->config.version >> 8) & 0xFF  //VERSION
    };
    memcpy(s_dev->pnp, pnp_val, 7);
    ble_svc_dis_pnp_id_set((char *)s_dev->pnp);
    if (s_dev->config.manufacturer_name && s_dev->config.manufacturer_name[0]) {
        rc = ble_svc_dis_manufacturer_name_set(s_dev->config.manufacturer_name);
    }
    if (s_dev->config.serial_number && s_dev->config.serial_number[0]) {
        rc = ble_svc_dis_serial_number_set(s_dev->config.serial_number);
    }
    return rc;
}

static int nimble_hid_start_gatts(void)
{
    int rc = ESP_OK;

    ble_svc_gap_init();
    ble_svc_gatt_init();
    ble_svc_sps_init(0, 0); // initialize with 0
    ble_svc_bas_init();
    ble_hid_create_info_db();

    for (uint8_t d = 0; d < s_dev->devices_len; d++) {
        rc = create_hid_db(d);
        if (rc != 0) {
            return rc;
        }
    }
    /* init the hid svc */
    ble_svc_hid_init();

    return rc;
}

static int nimble_hid_stop_gatts(esp_ble_hidd_dev_t *dev)
{
    int rc = ESP_OK;

    /* stop gatt database */
    ble_gatts_stop();

    ble_svc_hid_deinit();
    ble_svc_hid_reset();
    ble_svc_dis_deinit();
    ble_svc_bas_deinit();
    ble_svc_sps_deinit();
    ble_svc_gatt_deinit();
    ble_svc_gap_deinit();

    return rc;
}

/* Identify the reports using the report map */
static int ble_hid_init_config(esp_ble_hidd_dev_t *dev, const esp_hid_device_config_t *config)
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

static int ble_hid_free_config(esp_ble_hidd_dev_t *dev)
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
    }
    return ESP_OK;
}

static int nimble_hidd_dev_deinit(void *devp)
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
    service_index = -1; // resetting the value

    nimble_hid_stop_gatts(dev);
    esp_event_post_to(dev->event_loop_handle, ESP_HIDD_EVENTS, ESP_HIDD_STOP_EVENT, NULL, 0, portMAX_DELAY);
    ble_hid_free_config(dev);
    free(dev);
    return ESP_OK;
}

static bool nimble_hidd_dev_connected(void *devp)
{
    esp_ble_hidd_dev_t *dev = (esp_ble_hidd_dev_t *)devp;
    return (dev != NULL && s_dev == dev && dev->connected);
}

static int nimble_hidd_dev_battery_set(void *devp, uint8_t level)
{
    int rc;
    esp_ble_hidd_dev_t *dev = (esp_ble_hidd_dev_t *)devp;
    if (!dev || s_dev != dev) {
        return ESP_FAIL;
    }

    if (!dev->connected) {
        /* Return success if not yet connected */
        return ESP_OK;
    }

    rc = ble_svc_bas_battery_level_set(level);
    if (rc) {
        ESP_LOGE(TAG, "esp_ble_gatts_send_notify failed: %d", rc);
        return ESP_FAIL;
    }

    return ESP_OK;
}

/* if mode is NULL, find the first matching report */
static hidd_le_report_item_t* find_report(uint8_t id, uint8_t type, uint8_t *mode)
{
    hidd_le_report_item_t *rpt;
    for (uint8_t d = 0; d < s_dev->devices_len; d++) {
        for (uint8_t i = 0; i < s_dev->devices[d].reports_len; i++) {
            rpt = &s_dev->devices[d].reports[i];
            if (rpt->report_id == id && rpt->report_type == type && (!mode || (mode && *mode == rpt->protocol_mode))) {
                return rpt;
            }
        }
    }
    return NULL;
}
static hidd_le_report_item_t* find_report_by_usage_and_type(uint8_t usage, uint8_t type, uint8_t *mode)
{
    hidd_le_report_item_t *rpt;
    for (uint8_t d = 0; d < s_dev->devices_len; d++) {
        for (uint8_t i = 0; i < s_dev->devices[d].reports_len; i++) {
            rpt = &s_dev->devices[d].reports[i];
            if (rpt->usage == usage && rpt->report_type == type && (!mode || (mode && *mode == rpt->protocol_mode))) {
                return rpt;
            }
        }
    }
    return NULL;
}

static int nimble_hidd_dev_input_set(void *devp, size_t index, size_t id, uint8_t *data, size_t length)
{
    hidd_le_report_item_t *p_rpt;
    esp_ble_hidd_dev_t *dev = (esp_ble_hidd_dev_t *)devp;
    int rc;
    struct os_mbuf *om;
    if (!dev || s_dev != dev) {
        return ESP_FAIL;
    }

    if (!dev->connected) {
        ESP_LOGE(TAG, "%s Device Not Connected", __func__);
        return ESP_FAIL;
    }

    /* check the protocol mode */
    /* as the protocol mode is always present, its safe to read the characteristic */
    rc = ble_att_svr_read_local(s_dev->devices[index].hid_protocol_handle, &om);
    if (rc != 0) {
        ESP_LOGE(TAG, "Unable to fetch protocol_mode\n");
        return ESP_FAIL;
    }
    rc = ble_hs_mbuf_to_flat(om, &dev->protocol, sizeof(dev->protocol), NULL);
    if (rc != 0) {
        return ESP_FAIL;
    }
    /* free the mbuf */
    os_mbuf_free_chain(om);
    om = NULL;

    p_rpt = find_report(id, ESP_HID_REPORT_TYPE_INPUT, &dev->protocol);
    assert(p_rpt != NULL);
    om = ble_hs_mbuf_from_flat((void*)data, length);
    assert(om != NULL);

    /* NOTE : om is freed by stack */
    rc = ble_gatts_notify_custom(s_dev->conn_id, p_rpt->handle, om);

    if (rc != 0) {
        ESP_LOGE(TAG, "Write Input Report Failed: %d", rc);
        return ESP_FAIL;
    }
    return ESP_OK;
}

static int nimble_hidd_dev_feature_set(void *devp, size_t index, size_t id, uint8_t *data, size_t length)
{
    /* This function is a no-op for now */
    hidd_le_report_item_t *p_rpt;
    esp_ble_hidd_dev_t *dev = (esp_ble_hidd_dev_t *)devp;
    int rc;
    struct os_mbuf *om;
    if (!dev || s_dev != dev) {
        return ESP_FAIL;
    }

    if (!dev->connected) {
        ESP_LOGE(TAG, "%s Device Not Connected", __func__);
        return ESP_FAIL;
    }

    /* check the protocol mode */
    /* as the protocol mode is always present, its safe to read the characteristic */
    rc = ble_att_svr_read_local(s_dev->devices[index].hid_protocol_handle, &om);
    if (rc != 0) {
        ESP_LOGE(TAG, "Unable to fetch protocol_mode\n");
        return ESP_FAIL;
    }
    rc = ble_hs_mbuf_to_flat(om, &dev->protocol, sizeof(dev->protocol), NULL);
    if (rc != 0) {
        return ESP_FAIL;
    }
    /* free the mbuf */
    os_mbuf_free_chain(om);
    om = NULL;

    p_rpt = find_report(id, ESP_HID_REPORT_TYPE_FEATURE, &dev->protocol);
    assert(p_rpt != NULL);
    om = ble_hs_mbuf_from_flat((void*)data, length);
    assert(om != NULL);
    /* NOTE : om is freed by stack*/
    rc = ble_att_svr_write_local(p_rpt->handle, om);
    if (rc != 0) {
        ESP_LOGE(TAG, "Set Feature Report Failed: %d", rc);
        return ESP_FAIL;
    }
    return ESP_OK;
}

static int nimble_hidd_dev_event_handler_register(void *devp, esp_event_handler_t callback, esp_hidd_event_t event)
{
    esp_ble_hidd_dev_t *dev = (esp_ble_hidd_dev_t *)devp;
    if (!dev || s_dev != dev) {
        return ESP_FAIL;
    }
    return esp_event_handler_register_with(dev->event_loop_handle, ESP_HIDD_EVENTS, event, callback, dev->dev);
}

static int esp_ble_hidd_dev_event_handler_unregister(void *devp, esp_event_handler_t callback, esp_hidd_event_t event)
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

static int nimble_hid_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    struct os_mbuf *om;
    uint8_t data;
    int rc;

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        ESP_LOGD(TAG, "connection %s; status=%d",
                 event->connect.status == 0 ? "established" : "failed",
                 event->connect.status);
        rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
        assert(rc == 0);

        /* save connection handle */
        s_dev->connected = true;
        s_dev->conn_id = event->connect.conn_handle;
        esp_hidd_event_data_t cb_param = {
            .connect.dev = s_dev->dev,
            .connect.status = event->connect.status
        };

        /* reset the protocol mode value */
        data = ESP_HID_PROTOCOL_MODE_REPORT;
        om = ble_hs_mbuf_from_flat(&data, 1);
        if (om == NULL) {
            ESP_LOGD(TAG, "No memory to allocate mbuf");
        }
        /* NOTE : om is freed by stack */
        for (int i = 0; i < s_dev->devices_len; i++) {
            rc = ble_att_svr_write_local(s_dev->devices[i].hid_protocol_handle, om);
            if (rc != 0) {
                ESP_LOGE(TAG, "Write on Protocol Mode Failed: %d", rc);
            }
        }

        esp_event_post_to(s_dev->event_loop_handle, ESP_HIDD_EVENTS, ESP_HIDD_CONNECT_EVENT,
                          &cb_param, sizeof(esp_hidd_event_data_t), portMAX_DELAY);
        return 0;
        break;
    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGD(TAG, "disconnect; reason=%d", event->disconnect.reason);

        if (s_dev->connected) {
            s_dev->connected = false;
            esp_hidd_event_data_t cb_param = {0};
            cb_param.disconnect.dev = s_dev->dev;
            cb_param.disconnect.reason = event->disconnect.reason;
            esp_event_post_to(s_dev->event_loop_handle, ESP_HIDD_EVENTS, ESP_HIDD_DISCONNECT_EVENT,
                              &cb_param, sizeof(esp_hidd_event_data_t), portMAX_DELAY);
        }
        return 0;
    }
    return 0;
}

static void nimble_gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg)
{
    char buf[BLE_UUID_STR_LEN];
    hidd_le_report_item_t *rpt = NULL;
    struct os_mbuf *om;
    uint16_t uuid16;
    uint16_t report_info;
    uint8_t report_type, report_id;
    uint16_t report_handle;
    uint8_t protocol_mode;
    int rc;
    switch (ctxt->op) {
    case BLE_GATT_REGISTER_OP_SVC:
        ESP_LOGD(TAG, "registered service %s with handle=%d",
                 ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
                 ctxt->svc.handle);
        uuid16 = ble_uuid_u16(ctxt->svc.svc_def->uuid);
        if (uuid16 == BLE_SVC_HID_UUID16) {
            ++service_index;
            s_dev->devices[service_index].hid_svc = ctxt->svc.handle;
        }

        break;

    case BLE_GATT_REGISTER_OP_CHR:
        ESP_LOGD(TAG, "registering characteristic %s with "
                 "def_handle=%d val_handle=%d\n",
                 ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
                 ctxt->chr.def_handle,
                 ctxt->chr.val_handle);
        uuid16 = ble_uuid_u16(ctxt->chr.chr_def->uuid);
        if (uuid16 == BLE_SVC_HID_CHR_UUID16_HID_CTRL_PT) {
            /* assuming this characteristic is from the last registered hid service */
            s_dev->devices[service_index].hid_control_handle = ctxt->chr.val_handle;
        }
        if (uuid16 == BLE_SVC_HID_CHR_UUID16_PROTOCOL_MODE) {
            /* assuming this characteristic is from the last registered hid service */
            s_dev->devices[service_index].hid_protocol_handle = ctxt->chr.val_handle;
        }
        if (uuid16 == BLE_SVC_HID_CHR_UUID16_BOOT_KBD_INP) {
            protocol_mode = ESP_HID_PROTOCOL_MODE_BOOT;
            rpt = find_report_by_usage_and_type(ESP_HID_USAGE_KEYBOARD, ESP_HID_REPORT_TYPE_INPUT, &protocol_mode);
            if (rpt == NULL) {
                ESP_LOGE(TAG, "Unknown boot kbd input report registration");
                return;
            }
            rpt->handle = ctxt->chr.val_handle;
        }
        if (uuid16 == BLE_SVC_HID_CHR_UUID16_BOOT_KBD_OUT) {
            protocol_mode = ESP_HID_PROTOCOL_MODE_BOOT;
            rpt = find_report_by_usage_and_type(ESP_HID_USAGE_KEYBOARD, ESP_HID_REPORT_TYPE_OUTPUT, &protocol_mode);
            if (rpt == NULL) {
                ESP_LOGE(TAG, "Unknown boot kbd output report registration");
                return;
            }
            rpt->handle = ctxt->chr.val_handle;
        }
        if (uuid16 == BLE_SVC_HID_CHR_UUID16_BOOT_MOUSE_INP) {
            protocol_mode = ESP_HID_PROTOCOL_MODE_BOOT;
            rpt = find_report_by_usage_and_type(ESP_HID_USAGE_MOUSE, ESP_HID_REPORT_TYPE_INPUT, &protocol_mode);
            if (rpt == NULL) {
                ESP_LOGE(TAG, "Unknown boot mouse input report registration");
                return;
            }
            rpt->handle = ctxt->chr.val_handle;
        }
        break;

    case BLE_GATT_REGISTER_OP_DSC:
        ESP_LOGD(TAG, "registering descriptor %s with handle=%d",
                 ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
                 ctxt->dsc.handle);
        uuid16 = ble_uuid_u16(ctxt->dsc.dsc_def->uuid);
        if (uuid16 == BLE_SVC_HID_DSC_UUID16_RPT_REF) {
            rc = ble_att_svr_read_local(ctxt->dsc.handle, &om);
            assert(rc == 0);

            ble_hs_mbuf_to_flat(om, &report_info, sizeof report_info, NULL);
            report_type = (uint8_t)((report_info & 0xFF00) >> 8);
            report_id = report_info & 0x00FF;
            report_handle = (*(uint16_t*)(ctxt->dsc.dsc_def->arg));
            protocol_mode = ESP_HID_PROTOCOL_MODE_REPORT;
            rpt = find_report(report_id, report_type, &protocol_mode);
            assert(rpt != NULL);
            rpt->handle = report_handle;
            /* free the mbuf */
            os_mbuf_free_chain(om);
            om = NULL;
        }
        break;

    default:
        assert(0);
        break;
    }
}

static void nimble_host_synced(void)
{
    esp_event_post_to(s_dev->event_loop_handle, ESP_HIDD_EVENTS, ESP_HIDD_START_EVENT, NULL, 0, portMAX_DELAY);
}

void nimble_host_reset(int reason)
{
    MODLOG_DFLT(ERROR, "Resetting state; reason=%d\n", reason);
}

static struct ble_gap_event_listener nimble_gap_event_listener;
esp_err_t esp_ble_hidd_dev_init(esp_hidd_dev_t *dev_p, const esp_hid_device_config_t *config, esp_event_handler_t callback)
{
    int rc;

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
    s_dev->control = ESP_HID_CONTROL_EXIT_SUSPEND;
    s_dev->protocol = ESP_HID_PROTOCOL_MODE_REPORT;
    s_dev->event_loop_handle = NULL;
    s_dev->dev = dev_p;

    esp_event_loop_args_t event_task_args = {
        .queue_size = 5,
        .task_name = "ble_hidd_events",
        .task_priority = uxTaskPriorityGet(NULL),
        .task_stack_size = 4096,
        .task_core_id = tskNO_AFFINITY
    };
    rc = esp_event_loop_create(&event_task_args, &s_dev->event_loop_handle);
    if (rc != ESP_OK) {
        ESP_LOGE(TAG, "HID device event loop could not be created");
        ble_hidd_dev_free();
        return rc;
    }

    rc = ble_hid_init_config(s_dev, config);
    if (rc != ESP_OK) {
        ble_hidd_dev_free();
        return rc;
    }

    dev_p->dev = s_dev;
    dev_p->connected = nimble_hidd_dev_connected;
    dev_p->deinit = nimble_hidd_dev_deinit;
    dev_p->battery_set = nimble_hidd_dev_battery_set;
    dev_p->input_set = nimble_hidd_dev_input_set;
    dev_p->feature_set = nimble_hidd_dev_feature_set;
    dev_p->event_handler_register = nimble_hidd_dev_event_handler_register;
    dev_p->event_handler_unregister = esp_ble_hidd_dev_event_handler_unregister;

    rc = nimble_hidd_dev_event_handler_register(s_dev, esp_hidd_process_event_data_handler, ESP_EVENT_ANY_ID);
    if (rc != ESP_OK) {
        ble_hidd_dev_free();
        return rc;
    }

    if (callback != NULL) {
        rc = nimble_hidd_dev_event_handler_register(s_dev, callback, ESP_EVENT_ANY_ID);
        if (rc != ESP_OK) {
            ble_hidd_dev_free();
            return rc;
        }
    }

    ble_hs_cfg.reset_cb = nimble_host_reset;
    ble_hs_cfg.sync_cb = nimble_host_synced;
    ble_hs_cfg.gatts_register_cb = nimble_gatt_svr_register_cb;
    rc = nimble_hid_start_gatts();
    if (rc != ESP_OK) {
        return rc;
    }
    ble_gap_event_listener_register(&nimble_gap_event_listener,
                                    nimble_hid_gap_event, NULL);

    return rc;
}
#endif // CONFIG_BT_NIMBLE_HID_SERVICE
