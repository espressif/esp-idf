/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_gatt_common_api.h"
#include "ble_ams.h"
#include "esp_timer.h"

#define BLE_AMS_TAG                               "BLE_AMS"
#define EXAMPLE_DEVICE_NAME                       "ESP_BLE_AMS"
#define PROFILE_A_APP_ID                          0
#define PROFILE_NUM                               1
#define ADV_CONFIG_FLAG                           (1 << 0)
#define SCAN_RSP_CONFIG_FLAG                      (1 << 1)
#define INVALID_HANDLE                            0

static uint8_t adv_config_done = 0;
static bool get_service = false;
static esp_gattc_char_elem_t *char_elem_result   = NULL;
static esp_gattc_descr_elem_t *descr_elem_result = NULL;
static void periodic_timer_callback(void* arg);
esp_timer_handle_t periodic_timer;

const esp_timer_create_args_t periodic_timer_args = {
    .callback = &periodic_timer_callback,
    .name = "periodic"
};

struct data_source_buffer {
    uint8_t buffer[1024];
    uint16_t len;
};

static struct data_source_buffer data_buffer = {0};

// Apple Media Service (AMS) UUIDs
// Service UUID: 89D3502B-0F36-433A-8EF4-C502AD55F8DC
uint8_t Apple_Media_Service_UUID[16] = {0xDC, 0xF8, 0x55, 0xAD, 0x02, 0xC5, 0xF4, 0x8E, 0x3A, 0x43, 0x36, 0x0F, 0x2B, 0x50, 0xD3, 0x89};
// Remote Command UUID: 9B3C81D8-57B1-4A8A-B8DF-0E56F7CA51C2 (writeable, notifiable)
uint8_t remote_command[16] = {0xC2, 0x51, 0xCA, 0xF7, 0x56, 0x0E, 0xDF, 0xB8, 0x8A, 0x4A, 0xB1, 0x57, 0xD8, 0x81, 0x3C, 0x9B};
// Entity Update UUID: 2F7CABCE-808D-411F-9A0C-BB92BA96C102 (notifiable)
uint8_t entity_update[16] = {0x02, 0xC1, 0x96, 0xBA, 0x92, 0xBB, 0x0C, 0x9A, 0x1F, 0x41, 0x8D, 0x80, 0xCE, 0xAB, 0x7C, 0x2F};
// Entity Attribute UUID: C6B2F38C-23AB-46D8-A6AB-A3A870BBD5D7 (readable, writeable)
uint8_t entity_attribute[16] = {0xD7, 0xD5, 0xBB, 0x70, 0xA8, 0xA3, 0xAB, 0xA6, 0xD8, 0x46, 0xAB, 0x23, 0x8C, 0xF3, 0xB2, 0xC6};

static void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);

static esp_bt_uuid_t apple_media_service_uuid = {
    .len = ESP_UUID_LEN_128,
};

static uint8_t hidd_service_uuid128[] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    //first uuid, 16bit, [12],[13] is the value
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x12, 0x18, 0x00, 0x00,
};

// config adv data
static esp_ble_adv_data_t adv_config = {
    .set_scan_rsp = false,
    .include_txpower = false,
    .min_interval = 0x0006, //slave connection min interval, Time = min_interval * 1.25 msec
    .max_interval = 0x0010, //slave connection max interval, Time = max_interval * 1.25 msec
    .appearance = ESP_BLE_APPEARANCE_GENERIC_HID,
    .service_uuid_len = sizeof(hidd_service_uuid128),
    .p_service_uuid = hidd_service_uuid128,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

// config scan response data
static esp_ble_adv_data_t scan_rsp_config = {
    .set_scan_rsp = true,
    .include_name = true,
    .manufacturer_len = 0,
    .p_manufacturer_data = NULL,
};

static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = 0x100,
    .adv_int_max        = 0x100,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_RPA_PUBLIC,
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
        .gattc_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
};

// Helper function to send remote command
void esp_send_remote_command(uint8_t command_id)
{
    ESP_LOGI(BLE_AMS_TAG, "Sending remote command: %s", RemoteCommandID_to_String(command_id));
    
    esp_ble_gattc_write_char(gl_profile_tab[PROFILE_A_APP_ID].gattc_if,
                             gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                             gl_profile_tab[PROFILE_A_APP_ID].remote_command_handle,
                             1,
                             &command_id,
                             ESP_GATT_WRITE_TYPE_RSP,
                             ESP_GATT_AUTH_REQ_NONE);
}

// Helper function to read entity attribute
void esp_read_entity_attribute(uint8_t entity_id, uint8_t attribute_id)
{
    uint8_t cmd[2] = {entity_id, attribute_id};
    
    ESP_LOGI(BLE_AMS_TAG, "Reading entity attribute - Entity:%s Attribute ID:%d", 
             EntityID_to_String(entity_id), attribute_id);
    
    esp_ble_gattc_write_char(gl_profile_tab[PROFILE_A_APP_ID].gattc_if,
                             gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                             gl_profile_tab[PROFILE_A_APP_ID].entity_attribute_handle,
                             2,
                             cmd,
                             ESP_GATT_WRITE_TYPE_RSP,
                             ESP_GATT_AUTH_REQ_NONE);
}

static void periodic_timer_callback(void* arg)
{
    esp_timer_stop(periodic_timer);
    if (data_buffer.len > 0) {
        esp_receive_apple_entity_attribute(data_buffer.buffer, data_buffer.len);
        memset(data_buffer.buffer, 0, data_buffer.len);
        data_buffer.len = 0;
    }
}

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    ESP_LOGV(BLE_AMS_TAG, "GAP_EVT, event %d", event);

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
        //advertising start complete event to indicate advertising start successfully or failed
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(BLE_AMS_TAG, "advertising start failed, error status = %x", param->adv_start_cmpl.status);
            break;
        }
        ESP_LOGI(BLE_AMS_TAG, "advertising start success");
        break;
    case ESP_GAP_BLE_PASSKEY_REQ_EVT:                           /* passkey request event */
        ESP_LOGI(BLE_AMS_TAG, "ESP_GAP_BLE_PASSKEY_REQ_EVT");
        /* Call the following function to input the passkey which is displayed on the remote device */
        //esp_ble_passkey_reply(heart_rate_profile_tab[HEART_PROFILE_APP_IDX].remote_bda, true, 0x00);
        break;
    case ESP_GAP_BLE_OOB_REQ_EVT: {
        ESP_LOGI(BLE_AMS_TAG, "ESP_GAP_BLE_OOB_REQ_EVT");
        uint8_t tk[16] = {1}; //If you paired with OOB, both devices need to use the same tk
        esp_ble_oob_req_reply(param->ble_security.ble_req.bd_addr, tk, sizeof(tk));
        break;
    }
    case ESP_GAP_BLE_NC_REQ_EVT:
        /* The app will receive this evt when the IO has DisplayYesNO capability and the peer device IO also has DisplayYesNo capability.
        show the passkey number to the user to confirm it with the number displayed by peer device. */
        esp_ble_confirm_reply(param->ble_security.ble_req.bd_addr, true);
        ESP_LOGI(BLE_AMS_TAG, "ESP_GAP_BLE_NC_REQ_EVT, the passkey Notify number:%" PRIu32, param->ble_security.key_notif.passkey);
        break;
    case ESP_GAP_BLE_SEC_REQ_EVT:
        /* send the positive(true) security response to the peer device to accept the security request.
        If not accept the security request, should send the security response with negative(false) accept value*/
        esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);
        break;
    case ESP_GAP_BLE_PASSKEY_NOTIF_EVT:  ///the app will receive this evt when the IO  has Output capability and the peer device IO has Input capability.
        ///show the passkey number to the user to input it in the peer device.
        ESP_LOGI(BLE_AMS_TAG, "The passkey Notify number:%06" PRIu32, param->ble_security.key_notif.passkey);
        break;
    case ESP_GAP_BLE_AUTH_CMPL_EVT: {
        ESP_LOG_BUFFER_HEX("addr", param->ble_security.auth_cmpl.bd_addr, ESP_BD_ADDR_LEN);
        ESP_LOGI(BLE_AMS_TAG, "pair status = %s",param->ble_security.auth_cmpl.success ? "success" : "fail");
        if (!param->ble_security.auth_cmpl.success) {
            ESP_LOGI(BLE_AMS_TAG, "fail reason = 0x%x",param->ble_security.auth_cmpl.fail_reason);
        }
        break;
    }
    case ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT:
        if (param->local_privacy_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(BLE_AMS_TAG, "config local privacy failed, error status = %x", param->local_privacy_cmpl.status);
            break;
        }

        esp_err_t ret = esp_ble_gap_config_adv_data(&adv_config);
        if (ret) {
            ESP_LOGE(BLE_AMS_TAG, "config adv data failed, error code = %x", ret);
        } else {
            adv_config_done |= ADV_CONFIG_FLAG;
        }

        ret = esp_ble_gap_config_adv_data(&scan_rsp_config);
        if (ret) {
            ESP_LOGE(BLE_AMS_TAG, "config adv data failed, error code = %x", ret);
        } else {
            adv_config_done |= SCAN_RSP_CONFIG_FLAG;
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
        ESP_LOGI(BLE_AMS_TAG, "REG_EVT");
        esp_ble_gap_set_device_name(EXAMPLE_DEVICE_NAME);
        esp_ble_gap_config_local_icon (ESP_BLE_APPEARANCE_GENERIC_WATCH);
        //generate a resolvable random address
        esp_ble_gap_config_local_privacy(true);
        break;
    case ESP_GATTC_OPEN_EVT:
        if (param->open.status != ESP_GATT_OK) {
            ESP_LOGE(BLE_AMS_TAG, "open failed, error status = %x", param->open.status);
            break;
        }
        ESP_LOGI(BLE_AMS_TAG, "ESP_GATTC_OPEN_EVT");
        gl_profile_tab[PROFILE_A_APP_ID].conn_id = param->open.conn_id;
        esp_ble_set_encryption(param->open.remote_bda, ESP_BLE_SEC_ENCRYPT_MITM);
        esp_err_t mtu_ret = esp_ble_gattc_send_mtu_req (gattc_if, param->open.conn_id);
        if (mtu_ret) {
            ESP_LOGE(BLE_AMS_TAG, "config MTU error, error code = %x", mtu_ret);
        }
        break;
    case ESP_GATTC_CFG_MTU_EVT:
        if (param->cfg_mtu.status != ESP_GATT_OK) {
            ESP_LOGE(BLE_AMS_TAG,"config mtu failed, error status = %x", param->cfg_mtu.status);
        }
        ESP_LOGI(BLE_AMS_TAG, "ESP_GATTC_CFG_MTU_EVT, Status %d, MTU %d, conn_id %d", param->cfg_mtu.status, param->cfg_mtu.mtu, param->cfg_mtu.conn_id);
        gl_profile_tab[PROFILE_A_APP_ID].MTU_size = param->cfg_mtu.mtu;
        memcpy(apple_media_service_uuid.uuid.uuid128, Apple_Media_Service_UUID, 16);
        esp_ble_gattc_search_service(gl_profile_tab[PROFILE_A_APP_ID].gattc_if, gl_profile_tab[PROFILE_A_APP_ID].conn_id, &apple_media_service_uuid);
        break;
    case ESP_GATTC_SEARCH_RES_EVT: {
        if (param->search_res.srvc_id.uuid.len == ESP_UUID_LEN_128) {
            gl_profile_tab[PROFILE_A_APP_ID].service_start_handle = param->search_res.start_handle;
            gl_profile_tab[PROFILE_A_APP_ID].service_end_handle = param->search_res.end_handle;
            get_service = true;
        }
        break;
    }
    case ESP_GATTC_SEARCH_CMPL_EVT:
        if (param->search_cmpl.status != ESP_GATT_OK) {
            ESP_LOGE(BLE_AMS_TAG, "search service failed, error status = %x", param->search_cmpl.status);
            break;
        }
        ESP_LOGI(BLE_AMS_TAG, "ESP_GATTC_SEARCH_CMPL_EVT");
        if (get_service) {
            uint16_t count  = 0;
            uint16_t offset = 0;
            esp_gatt_status_t ret_status = esp_ble_gattc_get_attr_count(gattc_if,
                                                                        gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                                                        ESP_GATT_DB_CHARACTERISTIC,
                                                                        gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                                                                        gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                                                                        INVALID_HANDLE,
                                                                        &count);
            if (ret_status != ESP_GATT_OK) {
                ESP_LOGE(BLE_AMS_TAG, "esp_ble_gattc_get_attr_count error, %d", __LINE__);
                break;
            }
            if (count > 0) {
                char_elem_result = (esp_gattc_char_elem_t *)malloc(sizeof(esp_gattc_char_elem_t) * count);
                if (!char_elem_result) {
                    ESP_LOGE(BLE_AMS_TAG, "gattc no mem");
                    break;
                } else {
                    memset(char_elem_result, 0xff, sizeof(esp_gattc_char_elem_t) * count);
                    ret_status = esp_ble_gattc_get_all_char(gattc_if,
                                                            gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                                            gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                                                            gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                                                            char_elem_result,
                                                            &count,
                                                            offset);
                    if (ret_status != ESP_GATT_OK) {
                        ESP_LOGE(BLE_AMS_TAG, "esp_ble_gattc_get_all_char error, %d", __LINE__);
                        free(char_elem_result);
                        char_elem_result = NULL;
                        break;
                    }
                    if (count > 0) {

                        for (int i = 0; i < count; i ++) {
                            if (char_elem_result[i].uuid.len == ESP_UUID_LEN_128) {
                                if (char_elem_result[i].properties & ESP_GATT_CHAR_PROP_BIT_WRITE
                                    && char_elem_result[i].properties & ESP_GATT_CHAR_PROP_BIT_NOTIFY
                                    && memcmp(char_elem_result[i].uuid.uuid.uuid128, remote_command, 16) == 0) {
                                    gl_profile_tab[PROFILE_A_APP_ID].remote_command_handle = char_elem_result[i].char_handle;
                                    esp_ble_gattc_register_for_notify (gattc_if,
                                                                       gl_profile_tab[PROFILE_A_APP_ID].remote_bda,
                                                                       char_elem_result[i].char_handle);
                                    ESP_LOGI(BLE_AMS_TAG, "Find Apple Media Service remote command char");

                                } else if (char_elem_result[i].properties & ESP_GATT_CHAR_PROP_BIT_NOTIFY
                                            && memcmp(char_elem_result[i].uuid.uuid.uuid128, entity_update, 16) == 0) {
                                    gl_profile_tab[PROFILE_A_APP_ID].entity_update_handle = char_elem_result[i].char_handle;
                                    esp_ble_gattc_register_for_notify (gattc_if,
                                                                       gl_profile_tab[PROFILE_A_APP_ID].remote_bda,
                                                                       char_elem_result[i].char_handle);
                                    ESP_LOGI(BLE_AMS_TAG, "Find Apple Media Service entity update char");

                                } else if (char_elem_result[i].properties & ESP_GATT_CHAR_PROP_BIT_READ
                                            && char_elem_result[i].properties & ESP_GATT_CHAR_PROP_BIT_WRITE
                                            && memcmp(char_elem_result[i].uuid.uuid.uuid128, entity_attribute, 16) == 0) {
                                    gl_profile_tab[PROFILE_A_APP_ID].entity_attribute_handle = char_elem_result[i].char_handle;
                                    ESP_LOGI(BLE_AMS_TAG, "Find Apple Media Service entity attribute char");

                                }
                            }
                        }
                    }
                }
                free(char_elem_result);
                char_elem_result = NULL;
            }
        } else {
            ESP_LOGE(BLE_AMS_TAG, "No Apple Media Service found");
        }

        break;
    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
        if (param->reg_for_notify.status != ESP_GATT_OK) {
            ESP_LOGI(BLE_AMS_TAG, "ESP_GATTC_REG_FOR_NOTIFY_EVT status %d", param->reg_for_notify.status);
            break;
        }
        uint16_t count = 0;
        uint16_t offset = 0;
        uint8_t notify_en[2] = {0x01, 0x00};
        esp_gatt_status_t ret_status = esp_ble_gattc_get_attr_count(gattc_if,
                                                                    gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                                                    ESP_GATT_DB_DESCRIPTOR,
                                                                    gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                                                                    gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                                                                    param->reg_for_notify.handle,
                                                                    &count);
        if (ret_status != ESP_GATT_OK) {
            ESP_LOGE(BLE_AMS_TAG, "esp_ble_gattc_get_attr_count error, %d", __LINE__);
            break;
        }
        if (count > 0) {
            descr_elem_result = malloc(sizeof(esp_gattc_descr_elem_t) * count);
            if (!descr_elem_result) {
                ESP_LOGE(BLE_AMS_TAG, "malloc error, gattc no mem");
                break;
            } else {
                ret_status = esp_ble_gattc_get_all_descr(gattc_if,
                                                         gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                                         param->reg_for_notify.handle,
                                                         descr_elem_result,
                                                         &count,
                                                         offset);
                if (ret_status != ESP_GATT_OK) {
                    ESP_LOGE(BLE_AMS_TAG, "esp_ble_gattc_get_all_descr error, %d", __LINE__);
                    free(descr_elem_result);
                    descr_elem_result = NULL;
                    break;
                }

                    for (int i = 0; i < count; ++ i) {
                        if (descr_elem_result[i].uuid.len == ESP_UUID_LEN_16 && descr_elem_result[i].uuid.uuid.uuid16 == ESP_GATT_UUID_CHAR_CLIENT_CONFIG) {
                            esp_ble_gattc_write_char_descr (gattc_if,
                                                            gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                                            descr_elem_result[i].handle,
                                                            sizeof(notify_en),
                                                            (uint8_t *)&notify_en,
                                                            ESP_GATT_WRITE_TYPE_RSP,
                                                            ESP_GATT_AUTH_REQ_NONE);

                            break;
                        }
                    }
                }
                free(descr_elem_result);
                descr_elem_result = NULL;
            }
        }
        break;
        case ESP_GATTC_NOTIFY_EVT:
        if (param->notify.handle == gl_profile_tab[PROFILE_A_APP_ID].entity_update_handle) {
            // Handle entity update notifications
            esp_receive_apple_entity_update(param->notify.value, param->notify.value_len);
            
            // Example: When track changes, read track information
            if (param->notify.value_len >= 2 && param->notify.value[0] == EntityIDTrack) {
                ESP_LOGI(BLE_AMS_TAG, "Track entity updated, reading track attributes");
                esp_read_entity_attribute(EntityIDTrack, TrackAttributeIDTitle);
                vTaskDelay(100 / portTICK_PERIOD_MS);
                esp_read_entity_attribute(EntityIDTrack, TrackAttributeIDArtist);
                vTaskDelay(100 / portTICK_PERIOD_MS);
                esp_read_entity_attribute(EntityIDTrack, TrackAttributeIDAlbum);
            }
            
        } else if (param->notify.handle == gl_profile_tab[PROFILE_A_APP_ID].remote_command_handle) {
            // Handle remote command responses
            ESP_LOGI(BLE_AMS_TAG, "Remote command response received");
            ESP_LOG_BUFFER_HEX(BLE_AMS_TAG, param->notify.value, param->notify.value_len);
        }
        break;
    case ESP_GATTC_READ_CHAR_EVT:
        if (param->read.status != ESP_GATT_OK) {
            ESP_LOGE(BLE_AMS_TAG, "read char failed, error status = %x", param->read.status);
            break;
        }
        if (param->read.handle == gl_profile_tab[PROFILE_A_APP_ID].entity_attribute_handle) {
            // Handle entity attribute read response
            memcpy(&data_buffer.buffer[data_buffer.len], param->read.value, param->read.value_len);
            data_buffer.len += param->read.value_len;
            if (param->read.value_len == (gl_profile_tab[PROFILE_A_APP_ID].MTU_size - 3)) {
                // copy and wait next packet, start timer 500ms
                esp_timer_start_periodic(periodic_timer, 500000);
            } else {
                esp_timer_stop(periodic_timer);
                esp_receive_apple_entity_attribute(data_buffer.buffer, data_buffer.len);
                memset(data_buffer.buffer, 0, data_buffer.len);
                data_buffer.len = 0;
            }
        }
        break;
    case ESP_GATTC_WRITE_DESCR_EVT:
        if (param->write.status != ESP_GATT_OK) {
            ESP_LOGE(BLE_AMS_TAG, "write descr failed, error status = %x", param->write.status);
            break;
        }
        // After enabling notifications, read initial media information
        if (param->write.handle && get_service) {
            static bool first_setup = true;
            if (first_setup) {
                first_setup = false;
                ESP_LOGI(BLE_AMS_TAG, "AMS setup complete, reading initial media info");
                vTaskDelay(500 / portTICK_PERIOD_MS);
                esp_read_entity_attribute(EntityIDPlayer, PlayerAttributeIDName);
                vTaskDelay(100 / portTICK_PERIOD_MS);
                esp_read_entity_attribute(EntityIDPlayer, PlayerAttributeIDPlaybackInfo);
                vTaskDelay(100 / portTICK_PERIOD_MS);
                esp_read_entity_attribute(EntityIDTrack, TrackAttributeIDTitle);
            }
        }
        break;
    case ESP_GATTC_SRVC_CHG_EVT: {
        ESP_LOGI(BLE_AMS_TAG, "ESP_GATTC_SRVC_CHG_EVT, bd_addr:");
        ESP_LOG_BUFFER_HEX(BLE_AMS_TAG, param->srvc_chg.remote_bda, 6);
        break;
    }
    case ESP_GATTC_WRITE_CHAR_EVT:
        if (param->write.status != ESP_GATT_OK) {
            ESP_LOGE(BLE_AMS_TAG, "write char failed, error status = %x", param->write.status);
            break;
        }
        if (param->write.handle == gl_profile_tab[PROFILE_A_APP_ID].entity_attribute_handle) {
            // Handle entity attribute write response - read the result
            esp_ble_gattc_read_char(gl_profile_tab[PROFILE_A_APP_ID].gattc_if,
                                   gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                   gl_profile_tab[PROFILE_A_APP_ID].entity_attribute_handle,
                                   ESP_GATT_AUTH_REQ_NONE);
        }
        break;
    case ESP_GATTC_DISCONNECT_EVT:
        ESP_LOGI(BLE_AMS_TAG, "ESP_GATTC_DISCONNECT_EVT, reason = 0x%x", param->disconnect.reason);
        get_service = false;
        esp_ble_gap_start_advertising(&adv_params);
        break;
    case ESP_GATTC_CONNECT_EVT:
        memcpy(gl_profile_tab[PROFILE_A_APP_ID].remote_bda, param->connect.remote_bda, 6);
        // create gattc virtual connection
        esp_ble_gatt_creat_conn_params_t creat_conn_params = {0};
        memcpy(&creat_conn_params.remote_bda, gl_profile_tab[PROFILE_A_APP_ID].remote_bda, ESP_BD_ADDR_LEN);
        creat_conn_params.remote_addr_type = BLE_ADDR_TYPE_RANDOM;
        creat_conn_params.own_addr_type = BLE_ADDR_TYPE_RPA_PUBLIC;
        creat_conn_params.is_direct = true;
        creat_conn_params.is_aux = false;
        creat_conn_params.phy_mask = 0x0;
        esp_ble_gattc_enh_open(gl_profile_tab[PROFILE_A_APP_ID].gattc_if,
                            &creat_conn_params);
        break;
    case ESP_GATTC_DIS_SRVC_CMPL_EVT:
        ESP_LOGI(BLE_AMS_TAG, "ESP_GATTC_DIS_SRVC_CMPL_EVT");
        break;
    default:
        break;
    }
}

static void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    /* If event is register event, store the gattc_if for each profile */
    if (event == ESP_GATTC_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            gl_profile_tab[param->reg.app_id].gattc_if = gattc_if;
        } else {
            ESP_LOGI(BLE_AMS_TAG, "Reg app failed, app_id %04x, status %d",
                    param->reg.app_id,
                    param->reg.status);
            return;
        }
    }

    /* If the gattc_if equal to profile A, call profile A cb handler,
     * so here call each profile's callback */
    do {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++) {
            if (gattc_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
                    gattc_if == gl_profile_tab[idx].gattc_if) {
                if (gl_profile_tab[idx].gattc_cb) {
                    gl_profile_tab[idx].gattc_cb(event, gattc_if, param);
                }
            }
        }
    } while (0);
}

// Task to demonstrate media control commands
void media_control_task(void *pvParameters)
{
    while (1) {
        if (get_service && gl_profile_tab[PROFILE_A_APP_ID].conn_id != ESP_GATT_IF_NONE) {
            // Wait 10 seconds, then send play/pause toggle
            vTaskDelay(10000 / portTICK_PERIOD_MS);
            esp_send_remote_command(RemoteCommandIDTogglePlayPause);
            
            // Wait 5 seconds, then send next track
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            esp_send_remote_command(RemoteCommandIDNextTrack);
            
            // Wait 5 seconds, then read current track info
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            esp_read_entity_attribute(EntityIDTrack, TrackAttributeIDTitle);
            
            // Wait 20 seconds before repeating
            vTaskDelay(20000 / portTICK_PERIOD_MS);
        } else {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
}

void init_timer(void)
{
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
}

void app_main(void)
{
    esp_err_t ret;

    // Initialize NVS.
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    // init timer
    init_timer();

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(BLE_AMS_TAG, "%s init controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(BLE_AMS_TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(BLE_AMS_TAG, "%s init bluetooth", __func__);

    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(BLE_AMS_TAG, "%s init bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(BLE_AMS_TAG, "%s enable bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    //register the callback function to the gattc module
    ret = esp_ble_gattc_register_callback(esp_gattc_cb);
    if (ret) {
        ESP_LOGE(BLE_AMS_TAG, "%s gattc register error, error code = %x", __func__, ret);
        return;
    }

    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret) {
        ESP_LOGE(BLE_AMS_TAG, "gap register error, error code = %x", ret);
        return;
    }

    ret = esp_ble_gattc_app_register(PROFILE_A_APP_ID);
    if (ret) {
        ESP_LOGE(BLE_AMS_TAG, "%s gattc app register error, error code = %x", __func__, ret);
    }

    ret = esp_ble_gatt_set_local_mtu(500);
    if (ret) {
        ESP_LOGE(BLE_AMS_TAG, "set local  MTU failed, error code = %x", ret);
    }

    /* set the security iocap & auth_req & key size & init key response key parameters to the stack*/
    esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_SC_MITM_BOND;     //bonding with peer device after authentication
    esp_ble_io_cap_t iocap = ESP_IO_CAP_NONE;           //set the IO capability to No output No input
    uint8_t key_size = 16;      //the key size should be 7~16 bytes
    uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    //set static passkey
    uint32_t passkey = 123456;
    uint8_t auth_option = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE;
    uint8_t oob_support = ESP_BLE_OOB_DISABLE;
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey, sizeof(uint32_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH, &auth_option, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_OOB_SUPPORT, &oob_support, sizeof(uint8_t));
    /* If your BLE device acts as a Slave, the init_key means you hope which types of key of the master should distribute to you,
    and the response key means which key you can distribute to the master;
    If your BLE device acts as a master, the response key means you hope which types of key of the slave should distribute to you,
    and the init key means which key you can distribute to the slave. */
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t));

    // Create media control task
    xTaskCreate(media_control_task, "media_control_task", 4096, NULL, 5, NULL);
}
