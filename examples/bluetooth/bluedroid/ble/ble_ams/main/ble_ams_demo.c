/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <inttypes.h>
#include <string.h>
#include "esp_log.h"
#include "esp_timer.h"
#include "nvs_flash.h"
#include "esp_bt.h"

#include "esp_gap_ble_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_gatt_common_api.h"
#include "ble_ams.h"

#define BLE_AMS_DEMO_TAG                          "BLE_AMS"
#define EXAMPLE_DEVICE_NAME                       "ESP_BLE_AMS"
#define PROFILE_A_APP_ID                          0
#define PROFILE_NUM                               1
#define ADV_CONFIG_FLAG                           (1 << 0)
#define SCAN_RSP_CONFIG_FLAG                      (1 << 1)
#define INVALID_HANDLE                            0
#define AMS_ENTITY_UPDATE_CCCD_DELAY_US           (500 * 1000)
#define ENTITY_ATTR_REQ_QUEUE_LEN                 4
#define ENTITY_UPDATE_SUB_CMD_MAX_LEN             8
#define ENTITY_UPDATE_SUB_QUEUE_LEN               3

static uint8_t adv_config_done = 0;
static bool get_service = false;
static bool entity_update_notify_enabled = false;
static esp_gattc_char_elem_t *char_elem_result = NULL;
static uint16_t remote_command_cccd_handle = 0;
static uint16_t entity_update_cccd_handle = 0;
static uint16_t pending_cccd_char_handle = 0;
static esp_gatt_if_t pending_entity_update_gattc_if = ESP_GATT_IF_NONE;
static esp_timer_handle_t entity_update_cccd_delay_timer;

static esp_bt_uuid_t cccd_uuid = {
    .len = ESP_UUID_LEN_16,
    .uuid = { .uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG },
};

static void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);
static void ams_schedule_entity_update_notify(esp_gatt_if_t gattc_if);

// Apple Media Service UUID: 89D3502B-0F36-433A-8EF4-C502AD55F8DC
static uint8_t Apple_MS_UUID[16] = {0xDC, 0xF8, 0x55, 0xAD, 0x02, 0xC5, 0xF4, 0x8E, 0x3A, 0x43, 0x36, 0x0F, 0x2B, 0x50, 0xD3, 0x89};
// Remote Command UUID: 9B3C81D8-57B1-4A8A-B8DF-0E56F7CA51C2 (writeable, notifiable)
static uint8_t remote_command_uuid[16] = {0xC2, 0x51, 0xCA, 0xF7, 0x56, 0x0E, 0xDF, 0xB8, 0x8A, 0x4A, 0xB1, 0x57, 0xD8, 0x81, 0x3C, 0x9B};
// Entity Update UUID: 2F7CABCE-808D-411F-9A0C-BB92BA96C102 (writeable with response, notifiable)
static uint8_t entity_update_uuid[16] = {0x02, 0xC1, 0x96, 0xBA, 0x92, 0xBB, 0x0C, 0x9A, 0x1F, 0x41, 0x8D, 0x80, 0xCE, 0xAB, 0x7C, 0x2F};
// Entity Attribute UUID: C6B2F38C-23AB-46D8-A6AB-A3A870BBD5D7 (readable, writeable)
static uint8_t entity_attribute_uuid[16] = {0xD7, 0xD5, 0xBB, 0x70, 0xA8, 0xA3, 0xAB, 0xA6, 0xD8, 0x46, 0xAB, 0x23, 0x8C, 0xF3, 0xB2, 0xC6};

static esp_bt_uuid_t apple_ms_uuid = {
    .len = ESP_UUID_LEN_128,
};

static uint8_t hidd_service_uuid128[] = {
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x12, 0x18, 0x00, 0x00,
};

static esp_ble_adv_data_t adv_config = {
    .set_scan_rsp = false,
    .include_txpower = false,
    .min_interval = ESP_BLE_GAP_CONN_ITVL_MS(7.5),
    .max_interval = ESP_BLE_GAP_CONN_ITVL_MS(20),
    .appearance = ESP_BLE_APPEARANCE_GENERIC_HID,
    .service_uuid_len = sizeof(hidd_service_uuid128),
    .p_service_uuid = hidd_service_uuid128,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

static esp_ble_adv_data_t scan_rsp_config = {
    .set_scan_rsp = true,
    .include_name = true,
};

static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = ESP_BLE_GAP_ADV_ITVL_MS(160),
    .adv_int_max        = ESP_BLE_GAP_ADV_ITVL_MS(160),
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

struct gattc_profile_inst {
    esp_gattc_cb_t gattc_cb;
    uint16_t gattc_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_start_handle;
    uint16_t service_end_handle;
    uint16_t remote_command_handle;
    uint16_t entity_update_handle;
    uint16_t entity_attribute_handle;
    esp_bd_addr_t remote_bda;
    uint16_t MTU_size;
};

static struct gattc_profile_inst gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_A_APP_ID] = {
        .gattc_cb = gattc_profile_event_handler,
        .gattc_if = ESP_GATT_IF_NONE,
    },
};

typedef struct {
    uint8_t entity_id;
    uint8_t attribute_id;
} entity_attr_req_t;

typedef struct {
    uint8_t cmd_len;
    uint8_t cmd[ENTITY_UPDATE_SUB_CMD_MAX_LEN];
} entity_update_sub_req_t;

static uint8_t pending_entity_id = 0;
static uint8_t pending_attribute_id = 0;
static bool entity_attr_req_in_flight = false;
static entity_attr_req_t entity_attr_req_queue[ENTITY_ATTR_REQ_QUEUE_LEN];
static uint8_t entity_attr_req_head = 0;
static uint8_t entity_attr_req_tail = 0;
static uint8_t entity_attr_req_count = 0;

static bool entity_update_sub_in_flight = false;
static entity_update_sub_req_t entity_update_sub_queue[ENTITY_UPDATE_SUB_QUEUE_LEN];
static uint8_t entity_update_sub_head = 0;
static uint8_t entity_update_sub_tail = 0;
static uint8_t entity_update_sub_count = 0;

static void entity_attr_req_queue_reset(void)
{
    entity_attr_req_head = 0;
    entity_attr_req_tail = 0;
    entity_attr_req_count = 0;
    entity_attr_req_in_flight = false;
    pending_entity_id = 0;
    pending_attribute_id = 0;
}

static void entity_update_sub_queue_reset(void)
{
    entity_update_sub_head = 0;
    entity_update_sub_tail = 0;
    entity_update_sub_count = 0;
    entity_update_sub_in_flight = false;
}

static bool entity_attr_req_queue_push(uint8_t entity_id, uint8_t attribute_id)
{
    if (entity_attr_req_count >= ENTITY_ATTR_REQ_QUEUE_LEN) {
        return false;
    }

    entity_attr_req_queue[entity_attr_req_tail].entity_id = entity_id;
    entity_attr_req_queue[entity_attr_req_tail].attribute_id = attribute_id;
    entity_attr_req_tail = (entity_attr_req_tail + 1) % ENTITY_ATTR_REQ_QUEUE_LEN;
    entity_attr_req_count++;
    return true;
}

static bool entity_attr_req_queue_pop(entity_attr_req_t *req)
{
    if (!req || entity_attr_req_count == 0) {
        return false;
    }

    *req = entity_attr_req_queue[entity_attr_req_head];
    entity_attr_req_head = (entity_attr_req_head + 1) % ENTITY_ATTR_REQ_QUEUE_LEN;
    entity_attr_req_count--;
    return true;
}

static void esp_process_next_entity_attribute_request(void)
{
    entity_attr_req_t req;
    uint8_t cmd[2];
    esp_err_t ret;

    if (gl_profile_tab[PROFILE_A_APP_ID].entity_attribute_handle == 0) {
        ESP_LOGW(BLE_AMS_DEMO_TAG, "Entity Attribute handle not discovered");
        return;
    }

    if (entity_attr_req_in_flight || !entity_attr_req_queue_pop(&req)) {
        return;
    }

    pending_entity_id = req.entity_id;
    pending_attribute_id = req.attribute_id;
    entity_attr_req_in_flight = true;
    cmd[0] = req.entity_id;
    cmd[1] = req.attribute_id;

    ret = esp_ble_gattc_write_char(gl_profile_tab[PROFILE_A_APP_ID].gattc_if,
                                   gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                   gl_profile_tab[PROFILE_A_APP_ID].entity_attribute_handle,
                                   sizeof(cmd),
                                   cmd,
                                   ESP_GATT_WRITE_TYPE_RSP,
                                   ESP_GATT_AUTH_REQ_NONE);
    if (ret != ESP_OK) {
        ESP_LOGE(BLE_AMS_DEMO_TAG, "write entity attribute request failed, error = %x", ret);
        entity_attr_req_in_flight = false;
        esp_process_next_entity_attribute_request();
    }
}

static bool entity_update_sub_queue_push(const uint8_t *cmd, uint8_t cmd_len)
{
    if (entity_update_sub_count >= ENTITY_UPDATE_SUB_QUEUE_LEN ||
        cmd_len > ENTITY_UPDATE_SUB_CMD_MAX_LEN) {
        return false;
    }

    entity_update_sub_queue[entity_update_sub_tail].cmd_len = cmd_len;
    memcpy(entity_update_sub_queue[entity_update_sub_tail].cmd, cmd, cmd_len);
    entity_update_sub_tail = (entity_update_sub_tail + 1) % ENTITY_UPDATE_SUB_QUEUE_LEN;
    entity_update_sub_count++;
    return true;
}

static bool entity_update_sub_queue_pop(entity_update_sub_req_t *req)
{
    if (!req || entity_update_sub_count == 0) {
        return false;
    }

    *req = entity_update_sub_queue[entity_update_sub_head];
    entity_update_sub_head = (entity_update_sub_head + 1) % ENTITY_UPDATE_SUB_QUEUE_LEN;
    entity_update_sub_count--;
    return true;
}

static void esp_process_next_entity_update_subscribe(void)
{
    entity_update_sub_req_t req;
    esp_err_t ret;

    if (entity_update_sub_in_flight || !entity_update_sub_queue_pop(&req)) {
        return;
    }

    entity_update_sub_in_flight = true;
    ESP_LOGI(BLE_AMS_DEMO_TAG, "Subscribe %s entity updates", EntityID_to_String(req.cmd[0]));

    ret = esp_ble_gattc_write_char(gl_profile_tab[PROFILE_A_APP_ID].gattc_if,
                                   gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                   gl_profile_tab[PROFILE_A_APP_ID].entity_update_handle,
                                   req.cmd_len,
                                   req.cmd,
                                   ESP_GATT_WRITE_TYPE_RSP,
                                   ESP_GATT_AUTH_REQ_NONE);
    if (ret != ESP_OK) {
        ESP_LOGE(BLE_AMS_DEMO_TAG, "write entity update subscribe request failed, error = %x", ret);
        entity_update_sub_in_flight = false;
        esp_process_next_entity_update_subscribe();
    }
}

static void esp_subscribe_entity_updates(void)
{
    const uint8_t track_cmd[] = {
        EntityIDTrack,
        TrackAttributeIDArtist,
        TrackAttributeIDAlbum,
        TrackAttributeIDTitle,
        TrackAttributeIDDuration,
    };
    const uint8_t player_cmd[] = {
        EntityIDPlayer,
        PlayerAttributeIDName,
        PlayerAttributeIDPlaybackInfo,
        PlayerAttributeIDVolume,
    };
    const uint8_t queue_cmd[] = {
        EntityIDQueue,
        QueueAttributeIDIndex,
        QueueAttributeIDCount,
        QueueAttributeIDShuffleMode,
        QueueAttributeIDRepeatMode,
    };

    entity_update_sub_queue_reset();

    if (!entity_update_sub_queue_push(track_cmd, sizeof(track_cmd)) ||
        !entity_update_sub_queue_push(player_cmd, sizeof(player_cmd)) ||
        !entity_update_sub_queue_push(queue_cmd, sizeof(queue_cmd))) {
        ESP_LOGW(BLE_AMS_DEMO_TAG, "Failed to queue entity update subscribe requests");
        return;
    }

    esp_process_next_entity_update_subscribe();
}

esp_err_t esp_send_remote_command(uint8_t command_id)
{
    if (gl_profile_tab[PROFILE_A_APP_ID].remote_command_handle == 0) {
        ESP_LOGW(BLE_AMS_DEMO_TAG, "Remote Command handle not discovered");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret = esp_ble_gattc_write_char(gl_profile_tab[PROFILE_A_APP_ID].gattc_if,
                                             gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                             gl_profile_tab[PROFILE_A_APP_ID].remote_command_handle,
                                             1,
                                             &command_id,
                                             ESP_GATT_WRITE_TYPE_RSP,
                                             ESP_GATT_AUTH_REQ_NONE);
    if (ret != ESP_OK) {
        ESP_LOGE(BLE_AMS_DEMO_TAG, "write remote command failed, error = %x", ret);
    }
    return ret;
}

static void esp_request_entity_attribute(uint8_t entity_id, uint8_t attribute_id)
{
    if (!entity_attr_req_queue_push(entity_id, attribute_id)) {
        ESP_LOGW(BLE_AMS_DEMO_TAG,
                 "Entity Attribute request dropped (entity=%u attr=%u): queue full (max=%u)",
                 entity_id,
                 attribute_id,
                 ENTITY_ATTR_REQ_QUEUE_LEN);
        return;
    }

    esp_process_next_entity_attribute_request();
}

static bool ams_find_cccd_handle(esp_gatt_if_t gattc_if, uint16_t char_handle,
                                 uint16_t *cccd_handle, const char *char_name)
{
    esp_gattc_descr_elem_t descr;
    uint16_t count = 1;

    esp_gatt_status_t status = esp_ble_gattc_get_descr_by_char_handle(gattc_if,
                                                                     gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                                                     char_handle,
                                                                     cccd_uuid,
                                                                     &descr,
                                                                     &count);
    if (status != ESP_GATT_OK || count == 0) {
        ESP_LOGE(BLE_AMS_DEMO_TAG, "%s CCCD not found (char 0x%04x, status 0x%x)",
                 char_name, char_handle, status);
        return false;
    }

    if (descr.uuid.len != ESP_UUID_LEN_16 ||
            descr.uuid.uuid.uuid16 != ESP_GATT_UUID_CHAR_CLIENT_CONFIG) {
        ESP_LOGE(BLE_AMS_DEMO_TAG, "%s unexpected descriptor UUID (char 0x%04x)", char_name, char_handle);
        return false;
    }

    *cccd_handle = descr.handle;
    ESP_LOGI(BLE_AMS_DEMO_TAG, "Found %s CCCD handle 0x%04x (char 0x%04x)",
             char_name, *cccd_handle, char_handle);
    return true;
}

static void ams_write_cccd(esp_gatt_if_t gattc_if, uint16_t cccd_handle, uint16_t char_handle)
{
    uint8_t notify_en[2] = {0x01, 0x00};
    esp_err_t ret;

    pending_cccd_char_handle = char_handle;
    ret = esp_ble_gattc_write_char_descr(gattc_if,
                                         gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                         cccd_handle,
                                         sizeof(notify_en),
                                         notify_en,
                                         ESP_GATT_WRITE_TYPE_RSP,
                                         ESP_GATT_AUTH_REQ_NONE);
    if (ret != ESP_OK) {
        pending_cccd_char_handle = 0;
        ESP_LOGE(BLE_AMS_DEMO_TAG, "write cccd failed, error = %x", ret);
        if (char_handle == gl_profile_tab[PROFILE_A_APP_ID].remote_command_handle) {
            ESP_LOGW(BLE_AMS_DEMO_TAG, "Remote Command CCCD write failed, continue Entity Update subscribe");
            ams_schedule_entity_update_notify(gattc_if);
        }
    }
}

static void ams_start_remote_command_notify(esp_gatt_if_t gattc_if)
{
    struct gattc_profile_inst *profile = &gl_profile_tab[PROFILE_A_APP_ID];
    esp_err_t ret;

    if (profile->remote_command_handle == 0) {
        return;
    }

    if (!ams_find_cccd_handle(gattc_if, profile->remote_command_handle,
                              &remote_command_cccd_handle, "Remote Command")) {
        ams_schedule_entity_update_notify(gattc_if);
        return;
    }

    ret = esp_ble_gattc_register_for_notify(gattc_if, profile->remote_bda, profile->remote_command_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(BLE_AMS_DEMO_TAG, "register Remote Command notify failed, error = %x", ret);
        ams_schedule_entity_update_notify(gattc_if);
    }
}

static void ams_start_entity_update_notify(esp_gatt_if_t gattc_if)
{
    struct gattc_profile_inst *profile = &gl_profile_tab[PROFILE_A_APP_ID];
    esp_err_t ret;

    if (profile->entity_update_handle == 0) {
        return;
    }

    if (!ams_find_cccd_handle(gattc_if, profile->entity_update_handle,
                              &entity_update_cccd_handle, "Entity Update")) {
        return;
    }

    ret = esp_ble_gattc_register_for_notify(gattc_if, profile->remote_bda, profile->entity_update_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(BLE_AMS_DEMO_TAG, "register Entity Update notify failed, error = %x", ret);
    }
}

static void ams_entity_update_cccd_delay_timer_cb(void *arg)
{
    esp_gatt_if_t gattc_if = pending_entity_update_gattc_if;

    pending_entity_update_gattc_if = ESP_GATT_IF_NONE;
    if (gattc_if == ESP_GATT_IF_NONE) {
        return;
    }

    ESP_LOGI(BLE_AMS_DEMO_TAG, "Starting Entity Update CCCD subscribe after delay");
    ams_start_entity_update_notify(gattc_if);
}

static void ams_schedule_entity_update_notify(esp_gatt_if_t gattc_if)
{
    pending_entity_update_gattc_if = gattc_if;
    esp_timer_stop(entity_update_cccd_delay_timer);
    esp_timer_start_once(entity_update_cccd_delay_timer, AMS_ENTITY_UPDATE_CCCD_DELAY_US);
}

static void ble_ams_start_adv_config(void)
{
    esp_err_t ret = esp_ble_gap_config_adv_data(&adv_config);
    if (ret) {
        ESP_LOGE(BLE_AMS_DEMO_TAG, "config adv data failed, error code = %x", ret);
    } else {
        adv_config_done |= ADV_CONFIG_FLAG;
    }

    ret = esp_ble_gap_config_adv_data(&scan_rsp_config);
    if (ret) {
        ESP_LOGE(BLE_AMS_DEMO_TAG, "config scan rsp data failed, error code = %x", ret);
    } else {
        adv_config_done |= SCAN_RSP_CONFIG_FLAG;
    }
}

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
        adv_config_done &= (~SCAN_RSP_CONFIG_FLAG);
        if (adv_config_done == 0) {
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        adv_config_done &= (~ADV_CONFIG_FLAG);
        if (adv_config_done == 0) {
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(BLE_AMS_DEMO_TAG, "advertising start failed, error status = %x", param->adv_start_cmpl.status);
            break;
        }
        ESP_LOGI(BLE_AMS_DEMO_TAG, "advertising start success");
        break;
    case ESP_GAP_BLE_OOB_REQ_EVT: {
        uint8_t tk[16] = {1};
        esp_ble_oob_req_reply(param->ble_security.ble_req.bd_addr, tk, sizeof(tk));
        break;
    }
    case ESP_GAP_BLE_NC_REQ_EVT:
        esp_ble_confirm_reply(param->ble_security.ble_req.bd_addr, true);
        ESP_LOGI(BLE_AMS_DEMO_TAG, "ESP_GAP_BLE_NC_REQ_EVT, the passkey Notify number:%" PRIu32,
                 param->ble_security.key_notif.passkey);
        break;
    case ESP_GAP_BLE_SEC_REQ_EVT:
        esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);
        break;
    case ESP_GAP_BLE_PASSKEY_NOTIF_EVT:
        ESP_LOGI(BLE_AMS_DEMO_TAG, "The passkey Notify number:%06" PRIu32, param->ble_security.key_notif.passkey);
        break;
    case ESP_GAP_BLE_AUTH_CMPL_EVT:
        ESP_LOG_BUFFER_HEX("addr", param->ble_security.auth_cmpl.bd_addr, ESP_BD_ADDR_LEN);
        ESP_LOGI(BLE_AMS_DEMO_TAG, "pair status = %s", param->ble_security.auth_cmpl.success ? "success" : "fail");
        if (!param->ble_security.auth_cmpl.success) {
            ESP_LOGI(BLE_AMS_DEMO_TAG, "fail reason = 0x%x", param->ble_security.auth_cmpl.fail_reason);
        }
        break;
    default:
        break;
    }
}

static void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    switch (event) {
    case ESP_GATTC_REG_EVT:
        ESP_LOGI(BLE_AMS_DEMO_TAG, "REG_EVT");
        esp_ble_gap_set_device_name(EXAMPLE_DEVICE_NAME);
        esp_ble_gap_config_local_icon(ESP_BLE_APPEARANCE_GENERIC_WATCH);
        ble_ams_start_adv_config();
        break;
    case ESP_GATTC_OPEN_EVT:
        if (param->open.status != ESP_GATT_OK) {
            ESP_LOGE(BLE_AMS_DEMO_TAG, "open failed, error status = %x", param->open.status);
            break;
        }
        ESP_LOGI(BLE_AMS_DEMO_TAG, "ESP_GATTC_OPEN_EVT");
        gl_profile_tab[PROFILE_A_APP_ID].conn_id = param->open.conn_id;
        esp_ble_set_encryption(param->open.remote_bda, ESP_BLE_SEC_ENCRYPT_MITM);
        esp_err_t mtu_ret = esp_ble_gattc_send_mtu_req(gattc_if, param->open.conn_id);
        if (mtu_ret) {
            ESP_LOGE(BLE_AMS_DEMO_TAG, "config MTU error, error code = %x", mtu_ret);
        }
        break;
    case ESP_GATTC_CFG_MTU_EVT:
        if (param->cfg_mtu.status != ESP_GATT_OK) {
            ESP_LOGE(BLE_AMS_DEMO_TAG, "config mtu failed, error status = %x", param->cfg_mtu.status);
        }
        ESP_LOGI(BLE_AMS_DEMO_TAG, "ESP_GATTC_CFG_MTU_EVT, Status %d, MTU %d, conn_id %d",
                 param->cfg_mtu.status, param->cfg_mtu.mtu, param->cfg_mtu.conn_id);
        gl_profile_tab[PROFILE_A_APP_ID].MTU_size = param->cfg_mtu.mtu;
        memcpy(apple_ms_uuid.uuid.uuid128, Apple_MS_UUID, 16);
        esp_ble_gattc_search_service(gl_profile_tab[PROFILE_A_APP_ID].gattc_if,
                                     gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                     &apple_ms_uuid);
        break;
    case ESP_GATTC_SEARCH_RES_EVT:
        if (param->search_res.srvc_id.uuid.len == ESP_UUID_LEN_128) {
            gl_profile_tab[PROFILE_A_APP_ID].service_start_handle = param->search_res.start_handle;
            gl_profile_tab[PROFILE_A_APP_ID].service_end_handle = param->search_res.end_handle;
            get_service = true;
        }
        break;
    case ESP_GATTC_SEARCH_CMPL_EVT:
        if (param->search_cmpl.status != ESP_GATT_OK) {
            ESP_LOGE(BLE_AMS_DEMO_TAG, "search service failed, error status = %x", param->search_cmpl.status);
            break;
        }
        ESP_LOGI(BLE_AMS_DEMO_TAG, "ESP_GATTC_SEARCH_CMPL_EVT");
        if (!get_service) {
            ESP_LOGE(BLE_AMS_DEMO_TAG, "No Apple Media Service found");
            break;
        }

        {
            uint16_t count = 0;
            uint16_t offset = 0;
            esp_gatt_status_t ret_status = esp_ble_gattc_get_attr_count(gattc_if,
                                                                        gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                                                        ESP_GATT_DB_CHARACTERISTIC,
                                                                        gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                                                                        gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                                                                        INVALID_HANDLE,
                                                                        &count);
            if (ret_status != ESP_GATT_OK || count == 0) {
                ESP_LOGE(BLE_AMS_DEMO_TAG, "esp_ble_gattc_get_attr_count error");
                break;
            }

            char_elem_result = (esp_gattc_char_elem_t *)malloc(sizeof(esp_gattc_char_elem_t) * count);
            if (!char_elem_result) {
                ESP_LOGE(BLE_AMS_DEMO_TAG, "gattc no mem");
                break;
            }

            memset(char_elem_result, 0xff, sizeof(esp_gattc_char_elem_t) * count);
            ret_status = esp_ble_gattc_get_all_char(gattc_if,
                                                    gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                                    gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                                                    gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                                                    char_elem_result,
                                                    &count,
                                                    offset);
            if (ret_status != ESP_GATT_OK) {
                ESP_LOGE(BLE_AMS_DEMO_TAG, "esp_ble_gattc_get_all_char error");
                free(char_elem_result);
                char_elem_result = NULL;
                break;
            }

            for (int i = 0; i < count; i++) {
                if (char_elem_result[i].uuid.len != ESP_UUID_LEN_128) {
                    continue;
                }

                if ((char_elem_result[i].properties & ESP_GATT_CHAR_PROP_BIT_NOTIFY) &&
                    memcmp(char_elem_result[i].uuid.uuid.uuid128, remote_command_uuid, 16) == 0) {
                    gl_profile_tab[PROFILE_A_APP_ID].remote_command_handle = char_elem_result[i].char_handle;
                    ESP_LOGI(BLE_AMS_DEMO_TAG, "Found Remote Command characteristic");
                } else if ((char_elem_result[i].properties & ESP_GATT_CHAR_PROP_BIT_NOTIFY) &&
                           memcmp(char_elem_result[i].uuid.uuid.uuid128, entity_update_uuid, 16) == 0) {
                    gl_profile_tab[PROFILE_A_APP_ID].entity_update_handle = char_elem_result[i].char_handle;
                    ESP_LOGI(BLE_AMS_DEMO_TAG, "Found Entity Update characteristic");
                } else if (memcmp(char_elem_result[i].uuid.uuid.uuid128, entity_attribute_uuid, 16) == 0) {
                    gl_profile_tab[PROFILE_A_APP_ID].entity_attribute_handle = char_elem_result[i].char_handle;
                    ESP_LOGI(BLE_AMS_DEMO_TAG, "Found Entity Attribute characteristic");
                }
            }

            if (gl_profile_tab[PROFILE_A_APP_ID].remote_command_handle != 0) {
                ams_start_remote_command_notify(gattc_if);
            } else if (gl_profile_tab[PROFILE_A_APP_ID].entity_update_handle != 0) {
                ams_start_entity_update_notify(gattc_if);
            }

            free(char_elem_result);
            char_elem_result = NULL;
        }
        break;
    case ESP_GATTC_REG_FOR_NOTIFY_EVT:
        if (param->reg_for_notify.status != ESP_GATT_OK) {
            ESP_LOGI(BLE_AMS_DEMO_TAG, "ESP_GATTC_REG_FOR_NOTIFY_EVT status %d", param->reg_for_notify.status);
            if (param->reg_for_notify.handle == gl_profile_tab[PROFILE_A_APP_ID].remote_command_handle) {
                ams_schedule_entity_update_notify(gattc_if);
            }
            break;
        }

        if (param->reg_for_notify.handle == gl_profile_tab[PROFILE_A_APP_ID].remote_command_handle &&
                remote_command_cccd_handle != 0) {
            ams_write_cccd(gattc_if, remote_command_cccd_handle,
                           gl_profile_tab[PROFILE_A_APP_ID].remote_command_handle);
        } else if (param->reg_for_notify.handle == gl_profile_tab[PROFILE_A_APP_ID].entity_update_handle &&
                   entity_update_cccd_handle != 0) {
            ams_write_cccd(gattc_if, entity_update_cccd_handle,
                           gl_profile_tab[PROFILE_A_APP_ID].entity_update_handle);
        }
        break;
    case ESP_GATTC_WRITE_DESCR_EVT:
        if (param->write.status != ESP_GATT_OK) {
            bool remote_command_pending = (pending_cccd_char_handle != 0 &&
                                          pending_cccd_char_handle == gl_profile_tab[PROFILE_A_APP_ID].remote_command_handle);
            ESP_LOGE(BLE_AMS_DEMO_TAG, "write descr failed, error status = %x", param->write.status);
            pending_cccd_char_handle = 0;
            if (remote_command_pending) {
                ESP_LOGW(BLE_AMS_DEMO_TAG, "Remote Command CCCD callback failed, continue Entity Update subscribe");
                ams_schedule_entity_update_notify(gattc_if);
            }
            break;
        }

        if (pending_cccd_char_handle != 0 &&
            pending_cccd_char_handle == gl_profile_tab[PROFILE_A_APP_ID].remote_command_handle) {
            pending_cccd_char_handle = 0;
            remote_command_cccd_handle = 0;
            ESP_LOGI(BLE_AMS_DEMO_TAG, "Remote Command notifications enabled, delay 500 ms before Entity Update CCCD");
            ams_schedule_entity_update_notify(gattc_if);
        } else if (pending_cccd_char_handle != 0 &&
                   pending_cccd_char_handle == gl_profile_tab[PROFILE_A_APP_ID].entity_update_handle &&
                   !entity_update_notify_enabled) {
            pending_cccd_char_handle = 0;
            entity_update_cccd_handle = 0;
            entity_update_notify_enabled = true;
            ESP_LOGI(BLE_AMS_DEMO_TAG, "Entity Update notifications enabled");
            esp_subscribe_entity_updates();
        } else {
            pending_cccd_char_handle = 0;
        }
        break;
    case ESP_GATTC_NOTIFY_EVT:
        if (param->notify.handle == gl_profile_tab[PROFILE_A_APP_ID].remote_command_handle) {
            esp_receive_remote_command_notification(param->notify.value, param->notify.value_len);
        } else if (param->notify.handle == gl_profile_tab[PROFILE_A_APP_ID].entity_update_handle) {
            if (!param->notify.value || param->notify.value_len < 3) {
                ESP_LOGW(BLE_AMS_DEMO_TAG, "Entity update notification too short (%u)", param->notify.value_len);
                break;
            }
            esp_receive_entity_update(param->notify.value, param->notify.value_len);
            if (param->notify.value[2] & EntityUpdateFlagTruncated) {
                esp_request_entity_attribute(param->notify.value[0], param->notify.value[1]);
            }
        } else {
            ESP_LOGI(BLE_AMS_DEMO_TAG, "Unknown notify handle 0x%04x", param->notify.handle);
        }
        break;
    case ESP_GATTC_WRITE_CHAR_EVT:
        if (param->write.status != ESP_GATT_OK) {
            ESP_LOGE(BLE_AMS_DEMO_TAG, "write char error, status = 0x%x (%s)",
                     param->write.status, Errcode_to_String(param->write.status));
            if (param->write.handle == gl_profile_tab[PROFILE_A_APP_ID].entity_attribute_handle) {
                entity_attr_req_in_flight = false;
                esp_process_next_entity_attribute_request();
            } else if (param->write.handle == gl_profile_tab[PROFILE_A_APP_ID].entity_update_handle) {
                entity_update_sub_in_flight = false;
                esp_process_next_entity_update_subscribe();
            }
            break;
        }

        if (param->write.handle == gl_profile_tab[PROFILE_A_APP_ID].entity_update_handle) {
            entity_update_sub_in_flight = false;
            esp_process_next_entity_update_subscribe();
            break;
        }

        if (param->write.handle == gl_profile_tab[PROFILE_A_APP_ID].entity_attribute_handle) {
            esp_err_t ret = esp_ble_gattc_read_char(gattc_if,
                                                    gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                                    gl_profile_tab[PROFILE_A_APP_ID].entity_attribute_handle,
                                                    ESP_GATT_AUTH_REQ_NONE);
            if (ret != ESP_OK) {
                ESP_LOGE(BLE_AMS_DEMO_TAG, "read char request failed, error = %x", ret);
                entity_attr_req_in_flight = false;
                esp_process_next_entity_attribute_request();
            }
        }
        break;
    case ESP_GATTC_READ_CHAR_EVT:
        if (param->read.status != ESP_GATT_OK) {
            ESP_LOGE(BLE_AMS_DEMO_TAG, "read char failed, error status = %x", param->read.status);
        }
        if (param->read.handle == gl_profile_tab[PROFILE_A_APP_ID].entity_attribute_handle) {
            if (param->read.status == ESP_GATT_OK) {
                esp_receive_entity_attribute_value(pending_entity_id,
                                                   pending_attribute_id,
                                                   param->read.value,
                                                   param->read.value_len);
            }
            entity_attr_req_in_flight = false;
            esp_process_next_entity_attribute_request();
        }
        break;
    case ESP_GATTC_DISCONNECT_EVT:
        ESP_LOGI(BLE_AMS_DEMO_TAG, "ESP_GATTC_DISCONNECT_EVT, reason = 0x%x", param->disconnect.reason);
        get_service = false;
        entity_update_notify_enabled = false;
        pending_cccd_char_handle = 0;
        remote_command_cccd_handle = 0;
        entity_update_cccd_handle = 0;
        pending_entity_update_gattc_if = ESP_GATT_IF_NONE;
        entity_attr_req_queue_reset();
        entity_update_sub_queue_reset();
        esp_timer_stop(entity_update_cccd_delay_timer);
        gl_profile_tab[PROFILE_A_APP_ID].remote_command_handle = 0;
        gl_profile_tab[PROFILE_A_APP_ID].entity_update_handle = 0;
        gl_profile_tab[PROFILE_A_APP_ID].entity_attribute_handle = 0;
        esp_ble_gap_start_advertising(&adv_params);
        break;
    case ESP_GATTC_CONNECT_EVT:
        memcpy(gl_profile_tab[PROFILE_A_APP_ID].remote_bda, param->connect.remote_bda, ESP_BD_ADDR_LEN);
        esp_ble_gatt_creat_conn_params_t creat_conn_params = {0};
        memcpy(&creat_conn_params.remote_bda, gl_profile_tab[PROFILE_A_APP_ID].remote_bda, ESP_BD_ADDR_LEN);
        creat_conn_params.remote_addr_type = BLE_ADDR_TYPE_RANDOM;
        creat_conn_params.own_addr_type = BLE_ADDR_TYPE_PUBLIC;
        creat_conn_params.is_direct = true;
        creat_conn_params.is_aux = false;
        creat_conn_params.phy_mask = 0x0;
        esp_ble_gattc_enh_open(gl_profile_tab[PROFILE_A_APP_ID].gattc_if, &creat_conn_params);
        break;
    case ESP_GATTC_SRVC_CHG_EVT:
        ESP_LOGI(BLE_AMS_DEMO_TAG, "ESP_GATTC_SRVC_CHG_EVT");
        break;
    default:
        break;
    }
}

static void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    if (event == ESP_GATTC_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            gl_profile_tab[param->reg.app_id].gattc_if = gattc_if;
        } else {
            ESP_LOGI(BLE_AMS_DEMO_TAG, "Reg app failed, app_id %04x, status %d",
                     param->reg.app_id, param->reg.status);
            return;
        }
    }

    for (int idx = 0; idx < PROFILE_NUM; idx++) {
        if (gattc_if == ESP_GATT_IF_NONE || gattc_if == gl_profile_tab[idx].gattc_if) {
            if (gl_profile_tab[idx].gattc_cb) {
                gl_profile_tab[idx].gattc_cb(event, gattc_if, param);
            }
        }
    }
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));

    ESP_LOGI(BLE_AMS_DEMO_TAG, "%s init bluetooth", __func__);

    esp_bluedroid_config_t cfg = BT_BLUEDROID_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bluedroid_init_with_cfg(&cfg));
    ESP_ERROR_CHECK(esp_bluedroid_enable());
    ESP_ERROR_CHECK(esp_ble_gattc_register_callback(esp_gattc_cb));
    ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_event_handler));
    ESP_ERROR_CHECK(esp_ble_gattc_app_register(PROFILE_A_APP_ID));
    ESP_ERROR_CHECK(esp_ble_gatt_set_local_mtu(500));

    const esp_timer_create_args_t entity_update_cccd_delay_timer_args = {
        .callback = ams_entity_update_cccd_delay_timer_cb,
        .name = "ams_eu_cccd",
    };
    ESP_ERROR_CHECK(esp_timer_create(&entity_update_cccd_delay_timer_args, &entity_update_cccd_delay_timer));

    esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_SC_MITM_BOND;
    esp_ble_io_cap_t iocap = ESP_IO_CAP_NONE;
    uint8_t key_size = 16;
    uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    uint32_t passkey = 123456;
    uint8_t auth_option = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE;
    uint8_t oob_support = ESP_BLE_OOB_DISABLE;

    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey, sizeof(uint32_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH, &auth_option, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_OOB_SUPPORT, &oob_support, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t));
}
