/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
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
#include "ble_ancs.h"
#include "esp_timer.h"

#define BLE_ANCS_TAG                              "BLE_ANCS"
#define EXAMPLE_DEVICE_NAME                       "ESP_BLE_ANCS"
#define PROFILE_A_APP_ID                          0
#define PROFILE_NUM                               1
#define ADV_CONFIG_FLAG                           (1 << 0)
#define SCAN_RSP_CONFIG_FLAG                      (1 << 1)
#define INVALID_HANDLE                             0
static uint8_t adv_config_done = 0;
static bool get_service = false;
static esp_gattc_char_elem_t *char_elem_result   = NULL;
static esp_gattc_descr_elem_t *descr_elem_result = NULL;
static void periodic_timer_callback(void* arg);
esp_timer_handle_t periodic_timer;

const esp_timer_create_args_t periodic_timer_args = {
    .callback = &periodic_timer_callback,
    /* name is optional, but may help identify the timer when debugging */
    .name = "periodic"
};

struct data_source_buffer {
    uint8_t buffer[1024];
    uint16_t len;
};

static struct data_source_buffer data_buffer = {0};

//In its basic form, the ANCS exposes three characteristics:
// service UUID: 7905F431-B5CE-4E99-A40F-4B1E122D00D0
uint8_t Apple_NC_UUID[16] = {0xD0, 0x00, 0x2D, 0x12, 0x1E, 0x4B, 0x0F, 0xA4, 0x99, 0x4E, 0xCE, 0xB5, 0x31, 0xF4, 0x05, 0x79};
// Notification Source UUID: 9FBF120D-6301-42D9-8C58-25E699A21DBD(notifiable)
uint8_t notification_source[16] = {0xbd, 0x1d, 0xa2, 0x99, 0xe6, 0x25, 0x58, 0x8c, 0xd9, 0x42, 0x01, 0x63, 0x0d, 0x12, 0xbf, 0x9f};
// Control Point UUID:69D1D8F3-45E1-49A8-9821-9BBDFDAAD9D9(writeable with response)
uint8_t control_point[16] = {0xd9, 0xd9, 0xaa, 0xfd, 0xbd, 0x9b, 0x21, 0x98, 0xa8, 0x49, 0xe1, 0x45, 0xf3, 0xd8, 0xd1, 0x69};
// Data Source UUID:22EAC6E9-24D6-4BB5-BE44-B36ACE7C7BFB(notifiable)
uint8_t data_source[16] = {0xfb, 0x7b, 0x7c, 0xce, 0x6a, 0xb3, 0x44, 0xbe, 0xb5, 0x4b, 0xd6, 0x24, 0xe9, 0xc6, 0xea, 0x22};

/*
Note: There may be more characteristics present in the ANCS than the three listed above. That said, an NC may ignore any characteristic it does not recognize.
*/

static void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);

static esp_bt_uuid_t apple_nc_uuid = {
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
    uint16_t notification_source_handle;
    uint16_t data_source_handle;
    uint16_t contol_point_handle;
    esp_bd_addr_t remote_bda;
    uint16_t MTU_size;
};

static struct gattc_profile_inst gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_A_APP_ID] = {
        .gattc_cb = gattc_profile_event_handler,
        .gattc_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
};

esp_noti_attr_list_t p_attr[8] = {
    [attr_appidentifier_index] = {
        .noti_attribute_id = NotificationAttributeIDAppIdentifier,
        .attribute_len = 0,
    },
    [attr_title_index] = {
        .noti_attribute_id = NotificationAttributeIDTitle,
        .attribute_len = 0xFFFF,
    },
    [attr_subtitle_index] = {
        .noti_attribute_id = NotificationAttributeIDSubtitle,
        .attribute_len = 0xFFFF,
    },
    [attr_message_index] = {
        .noti_attribute_id = NotificationAttributeIDMessage,
        .attribute_len = 0xFFFF,
    },
    [attr_messagesize_index] = {
        .noti_attribute_id = NotificationAttributeIDMessageSize,
        .attribute_len = 0,
    },
    [attr_date_index] = {
        .noti_attribute_id = NotificationAttributeIDDate,
        .attribute_len = 0,
    },
    [attr_positiveactionlabel_index] = {
        .noti_attribute_id = NotificationAttributeIDPositiveActionLabel,
        .attribute_len = 0,
    },
    [attr_negativeactionlabel_index] = {
        .noti_attribute_id = NotificationAttributeIDNegativeActionLabel,
        .attribute_len = 0,
    },

};

/*
    | CommandID(1 Byte) | NotificationUID(4 Bytes) | AttributeIDs |
*/

void esp_get_notification_attributes(uint8_t *notificationUID, uint8_t num_attr, esp_noti_attr_list_t *p_attr)
{
    uint8_t cmd[600] = {0};
    uint32_t index = 0;
    cmd[0] = CommandIDGetNotificationAttributes;
    index ++;
    memcpy(&cmd[index], notificationUID, ESP_NOTIFICATIONUID_LEN);
    index += ESP_NOTIFICATIONUID_LEN;
    while(num_attr > 0) {
        cmd[index ++] = p_attr->noti_attribute_id;
        if (p_attr->attribute_len > 0) {
            cmd[index ++] = p_attr->attribute_len;
            cmd[index ++] = (p_attr->attribute_len << 8);
        }
        p_attr ++;
        num_attr --;
    }

    esp_ble_gattc_write_char( gl_profile_tab[PROFILE_A_APP_ID].gattc_if,
                              gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                              gl_profile_tab[PROFILE_A_APP_ID].contol_point_handle,
                              index,
                              cmd,
                              ESP_GATT_WRITE_TYPE_RSP,
                              ESP_GATT_AUTH_REQ_NONE);
}

void esp_get_app_attributes(uint8_t *appidentifier, uint16_t appidentifier_len, uint8_t num_attr, uint8_t *p_app_attrs)
{
    uint8_t buffer[600] = {0};
    uint32_t index = 0;
    buffer[0] = CommandIDGetAppAttributes;
    index ++;
    memcpy(&buffer[index], appidentifier, appidentifier_len);
    index += appidentifier_len;
    memcpy(&buffer[index], p_app_attrs, num_attr);
    index += num_attr;

    esp_ble_gattc_write_char( gl_profile_tab[PROFILE_A_APP_ID].gattc_if,
                              gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                              gl_profile_tab[PROFILE_A_APP_ID].contol_point_handle,
                              index,
                              buffer,
                              ESP_GATT_WRITE_TYPE_RSP,
                              ESP_GATT_AUTH_REQ_NONE);
}

void esp_perform_notification_action(uint8_t *notificationUID, uint8_t ActionID)
{
    uint8_t buffer[600] = {0};
    uint32_t index = 0;
    buffer[0] = CommandIDPerformNotificationAction;
    index ++;
    memcpy(&buffer[index], notificationUID, ESP_NOTIFICATIONUID_LEN);
    index += ESP_NOTIFICATIONUID_LEN;
    buffer[index] = ActionID;
    index ++;
    esp_ble_gattc_write_char( gl_profile_tab[PROFILE_A_APP_ID].gattc_if,
                              gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                              gl_profile_tab[PROFILE_A_APP_ID].contol_point_handle,
                              index,
                              buffer,
                              ESP_GATT_WRITE_TYPE_RSP,
                              ESP_GATT_AUTH_REQ_NONE);
}

static void periodic_timer_callback(void* arg)
{
    esp_timer_stop(periodic_timer);
    if (data_buffer.len > 0) {
        esp_receive_apple_data_source(data_buffer.buffer, data_buffer.len);
        memset(data_buffer.buffer, 0, data_buffer.len);
        data_buffer.len = 0;
    }
}

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    ESP_LOGV(BLE_ANCS_TAG, "GAP_EVT, event %d", event);

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
            ESP_LOGE(BLE_ANCS_TAG, "advertising start failed, error status = %x", param->adv_start_cmpl.status);
            break;
        }
        ESP_LOGI(BLE_ANCS_TAG, "advertising start success");
        break;
    case ESP_GAP_BLE_PASSKEY_REQ_EVT:                           /* passkey request event */
        ESP_LOGI(BLE_ANCS_TAG, "ESP_GAP_BLE_PASSKEY_REQ_EVT");
        /* Call the following function to input the passkey which is displayed on the remote device */
        //esp_ble_passkey_reply(heart_rate_profile_tab[HEART_PROFILE_APP_IDX].remote_bda, true, 0x00);
        break;
    case ESP_GAP_BLE_OOB_REQ_EVT: {
        ESP_LOGI(BLE_ANCS_TAG, "ESP_GAP_BLE_OOB_REQ_EVT");
        uint8_t tk[16] = {1}; //If you paired with OOB, both devices need to use the same tk
        esp_ble_oob_req_reply(param->ble_security.ble_req.bd_addr, tk, sizeof(tk));
        break;
    }
    case ESP_GAP_BLE_NC_REQ_EVT:
        /* The app will receive this evt when the IO has DisplayYesNO capability and the peer device IO also has DisplayYesNo capability.
        show the passkey number to the user to confirm it with the number displayed by peer device. */
        esp_ble_confirm_reply(param->ble_security.ble_req.bd_addr, true);
        ESP_LOGI(BLE_ANCS_TAG, "ESP_GAP_BLE_NC_REQ_EVT, the passkey Notify number:%" PRIu32, param->ble_security.key_notif.passkey);
        break;
    case ESP_GAP_BLE_SEC_REQ_EVT:
        /* send the positive(true) security response to the peer device to accept the security request.
        If not accept the security request, should send the security response with negative(false) accept value*/
        esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);
        break;
    case ESP_GAP_BLE_PASSKEY_NOTIF_EVT:  ///the app will receive this evt when the IO  has Output capability and the peer device IO has Input capability.
        ///show the passkey number to the user to input it in the peer device.
        ESP_LOGI(BLE_ANCS_TAG, "The passkey Notify number:%06" PRIu32, param->ble_security.key_notif.passkey);
        break;
    case ESP_GAP_BLE_AUTH_CMPL_EVT: {
        esp_log_buffer_hex("addr", param->ble_security.auth_cmpl.bd_addr, ESP_BD_ADDR_LEN);
        ESP_LOGI(BLE_ANCS_TAG, "pair status = %s",param->ble_security.auth_cmpl.success ? "success" : "fail");
        if (!param->ble_security.auth_cmpl.success) {
            ESP_LOGI(BLE_ANCS_TAG, "fail reason = 0x%x",param->ble_security.auth_cmpl.fail_reason);
        }
        break;
    }
    case ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT:
        if (param->local_privacy_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(BLE_ANCS_TAG, "config local privacy failed, error status = %x", param->local_privacy_cmpl.status);
            break;
        }

        esp_err_t ret = esp_ble_gap_config_adv_data(&adv_config);
        if (ret) {
            ESP_LOGE(BLE_ANCS_TAG, "config adv data failed, error code = %x", ret);
        } else {
            adv_config_done |= ADV_CONFIG_FLAG;
        }

        ret = esp_ble_gap_config_adv_data(&scan_rsp_config);
        if (ret) {
            ESP_LOGE(BLE_ANCS_TAG, "config adv data failed, error code = %x", ret);
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
        ESP_LOGI(BLE_ANCS_TAG, "REG_EVT");
        esp_ble_gap_set_device_name(EXAMPLE_DEVICE_NAME);
        esp_ble_gap_config_local_icon (ESP_BLE_APPEARANCE_GENERIC_WATCH);
        //generate a resolvable random address
        esp_ble_gap_config_local_privacy(true);
        break;
    case ESP_GATTC_OPEN_EVT:
        if (param->open.status != ESP_GATT_OK) {
            ESP_LOGE(BLE_ANCS_TAG, "open failed, error status = %x", param->open.status);
            break;
        }
        ESP_LOGI(BLE_ANCS_TAG, "ESP_GATTC_OPEN_EVT");
        gl_profile_tab[PROFILE_A_APP_ID].conn_id = param->open.conn_id;
        esp_ble_set_encryption(param->open.remote_bda, ESP_BLE_SEC_ENCRYPT_MITM);
        esp_err_t mtu_ret = esp_ble_gattc_send_mtu_req (gattc_if, param->open.conn_id);
        if (mtu_ret) {
            ESP_LOGE(BLE_ANCS_TAG, "config MTU error, error code = %x", mtu_ret);
        }
        break;
    case ESP_GATTC_CFG_MTU_EVT:
        if (param->cfg_mtu.status != ESP_GATT_OK) {
            ESP_LOGE(BLE_ANCS_TAG,"config mtu failed, error status = %x", param->cfg_mtu.status);
        }
        ESP_LOGI(BLE_ANCS_TAG, "ESP_GATTC_CFG_MTU_EVT, Status %d, MTU %d, conn_id %d", param->cfg_mtu.status, param->cfg_mtu.mtu, param->cfg_mtu.conn_id);
        gl_profile_tab[PROFILE_A_APP_ID].MTU_size = param->cfg_mtu.mtu;
        memcpy(apple_nc_uuid.uuid.uuid128, Apple_NC_UUID,16);
        esp_ble_gattc_search_service(gl_profile_tab[PROFILE_A_APP_ID].gattc_if, gl_profile_tab[PROFILE_A_APP_ID].conn_id, &apple_nc_uuid);
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
            ESP_LOGE(BLE_ANCS_TAG, "search service failed, error status = %x", param->search_cmpl.status);
            break;
        }
        ESP_LOGI(BLE_ANCS_TAG, "ESP_GATTC_SEARCH_CMPL_EVT");
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
                ESP_LOGE(BLE_ANCS_TAG, "esp_ble_gattc_get_attr_count error, %d", __LINE__);
                break;
            }
            if (count > 0) {
                char_elem_result = (esp_gattc_char_elem_t *)malloc(sizeof(esp_gattc_char_elem_t) * count);
                if (!char_elem_result) {
                    ESP_LOGE(BLE_ANCS_TAG, "gattc no mem");
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
                        ESP_LOGE(BLE_ANCS_TAG, "esp_ble_gattc_get_all_char error, %d", __LINE__);
                        free(char_elem_result);
                        char_elem_result = NULL;
                        break;
                    }
                    if (count > 0) {

                        for (int i = 0; i < count; i ++) {
                            if (char_elem_result[i].uuid.len == ESP_UUID_LEN_128) {
                                if (char_elem_result[i].properties & ESP_GATT_CHAR_PROP_BIT_NOTIFY
                                    && memcmp(char_elem_result[i].uuid.uuid.uuid128, notification_source, 16) == 0) {
                                    gl_profile_tab[PROFILE_A_APP_ID].notification_source_handle = char_elem_result[i].char_handle;
                                    esp_ble_gattc_register_for_notify (gattc_if,
                                                                       gl_profile_tab[PROFILE_A_APP_ID].remote_bda,
                                                                       char_elem_result[i].char_handle);
                                    ESP_LOGI(BLE_ANCS_TAG, "Find Apple noticification source char");

                                } else if (char_elem_result[i].properties & ESP_GATT_CHAR_PROP_BIT_NOTIFY
                                            && memcmp(char_elem_result[i].uuid.uuid.uuid128, data_source, 16) == 0) {
                                    gl_profile_tab[PROFILE_A_APP_ID].data_source_handle = char_elem_result[i].char_handle;
                                    esp_ble_gattc_register_for_notify (gattc_if,
                                                                       gl_profile_tab[PROFILE_A_APP_ID].remote_bda,
                                                                       char_elem_result[i].char_handle);
                                    ESP_LOGI(BLE_ANCS_TAG, "Find Apple data source char");

                                } else if (char_elem_result[i].properties & ESP_GATT_CHAR_PROP_BIT_WRITE
                                            && memcmp(char_elem_result[i].uuid.uuid.uuid128, control_point, 16) == 0) {
                                    gl_profile_tab[PROFILE_A_APP_ID].contol_point_handle = char_elem_result[i].char_handle;
                                    ESP_LOGI(BLE_ANCS_TAG, "Find Apple control point char");

                                }
                            }
                        }
                    }
                }
                free(char_elem_result);
                char_elem_result = NULL;
            }
        } else {
            ESP_LOGE(BLE_ANCS_TAG, "No Apple Notification Service found");
        }

        break;
    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
        if (param->reg_for_notify.status != ESP_GATT_OK) {
            ESP_LOGI(BLE_ANCS_TAG, "ESP_GATTC_REG_FOR_NOTIFY_EVT status %d", param->reg_for_notify.status);
            break;
        }
        uint16_t count = 0;
        uint16_t offset = 0;
        //uint16_t notify_en = 1;
        uint8_t notify_en[2] = {0x01, 0x00};
        esp_gatt_status_t ret_status = esp_ble_gattc_get_attr_count(gattc_if,
                                                                    gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                                                    ESP_GATT_DB_DESCRIPTOR,
                                                                    gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                                                                    gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                                                                    param->reg_for_notify.handle,
                                                                    &count);
        if (ret_status != ESP_GATT_OK) {
            ESP_LOGE(BLE_ANCS_TAG, "esp_ble_gattc_get_attr_count error, %d", __LINE__);
            break;
        }
        if (count > 0) {
            descr_elem_result = malloc(sizeof(esp_gattc_descr_elem_t) * count);
            if (!descr_elem_result) {
                ESP_LOGE(BLE_ANCS_TAG, "malloc error, gattc no mem");
                break;
            } else {
                ret_status = esp_ble_gattc_get_all_descr(gattc_if,
                                                         gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                                         param->reg_for_notify.handle,
                                                         descr_elem_result,
                                                         &count,
                                                         offset);
                if (ret_status != ESP_GATT_OK) {
                    ESP_LOGE(BLE_ANCS_TAG, "esp_ble_gattc_get_all_descr error, %d", __LINE__);
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
        break;
    }
    case ESP_GATTC_NOTIFY_EVT:
        //esp_log_buffer_hex(BLE_ANCS_TAG, param->notify.value, param->notify.value_len);
        if (param->notify.handle == gl_profile_tab[PROFILE_A_APP_ID].notification_source_handle) {
            esp_receive_apple_notification_source(param->notify.value, param->notify.value_len);
            uint8_t *notificationUID = &param->notify.value[4];
            if (param->notify.value[0] == EventIDNotificationAdded && param->notify.value[2] == CategoryIDIncomingCall) {
                 ESP_LOGI(BLE_ANCS_TAG, "IncomingCall, reject");
                 //Call reject
                 esp_perform_notification_action(notificationUID, ActionIDNegative);
             } else if (param->notify.value[0] == EventIDNotificationAdded) {
                //get more information
                ESP_LOGI(BLE_ANCS_TAG, "Get detailed information");
                esp_get_notification_attributes(notificationUID, sizeof(p_attr)/sizeof(esp_noti_attr_list_t), p_attr);
             }
        } else if (param->notify.handle == gl_profile_tab[PROFILE_A_APP_ID].data_source_handle) {
            memcpy(&data_buffer.buffer[data_buffer.len], param->notify.value, param->notify.value_len);
            data_buffer.len += param->notify.value_len;
            if (param->notify.value_len == (gl_profile_tab[PROFILE_A_APP_ID].MTU_size - 3)) {
                // copy and wait next packet, start timer 500ms
                esp_timer_start_periodic(periodic_timer, 500000);
            } else {
                esp_timer_stop(periodic_timer);
                esp_receive_apple_data_source(data_buffer.buffer, data_buffer.len);
                memset(data_buffer.buffer, 0, data_buffer.len);
                data_buffer.len = 0;
            }
        } else {
            ESP_LOGI(BLE_ANCS_TAG, "unknown handle, receive notify value:");
        }
        break;
    case ESP_GATTC_WRITE_DESCR_EVT:
        if (param->write.status != ESP_GATT_OK) {
            ESP_LOGE(BLE_ANCS_TAG, "write descr failed, error status = %x", param->write.status);
            break;
        }
        //ESP_LOGI(BLE_ANCS_TAG, "write descr successfully");
        break;
    case ESP_GATTC_SRVC_CHG_EVT: {
        ESP_LOGI(BLE_ANCS_TAG, "ESP_GATTC_SRVC_CHG_EVT, bd_addr:");
        esp_log_buffer_hex(BLE_ANCS_TAG, param->srvc_chg.remote_bda, 6);
        break;
    }
    case ESP_GATTC_WRITE_CHAR_EVT:
        if (param->write.status != ESP_GATT_OK) {
            char *Errstr = Errcode_to_String(param->write.status);
            if (Errstr) {
                 ESP_LOGE(BLE_ANCS_TAG, "write control point error %s", Errstr);
            }
            break;
        }
        //ESP_LOGI(BLE_ANCS_TAG, "Write char success ");
        break;
    case ESP_GATTC_DISCONNECT_EVT:
        ESP_LOGI(BLE_ANCS_TAG, "ESP_GATTC_DISCONNECT_EVT, reason = 0x%x", param->disconnect.reason);
        get_service = false;
        esp_ble_gap_start_advertising(&adv_params);
        break;
    case ESP_GATTC_CONNECT_EVT:
        //ESP_LOGI(BLE_ANCS_TAG, "ESP_GATTC_CONNECT_EVT");
        //esp_log_buffer_hex("bda", param->connect.remote_bda, 6);
        memcpy(gl_profile_tab[PROFILE_A_APP_ID].remote_bda, param->connect.remote_bda, 6);
        // create gattc virtual connection
        esp_ble_gattc_open(gl_profile_tab[PROFILE_A_APP_ID].gattc_if, gl_profile_tab[PROFILE_A_APP_ID].remote_bda, BLE_ADDR_TYPE_RANDOM, true);
        break;
    case ESP_GATTC_DIS_SRVC_CMPL_EVT:
        ESP_LOGI(BLE_ANCS_TAG, "ESP_GATTC_DIS_SRVC_CMPL_EVT");
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
            ESP_LOGI(BLE_ANCS_TAG, "Reg app failed, app_id %04x, status %d",
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
        ESP_LOGE(BLE_ANCS_TAG, "%s init controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(BLE_ANCS_TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(BLE_ANCS_TAG, "%s init bluetooth", __func__);

    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(BLE_ANCS_TAG, "%s init bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(BLE_ANCS_TAG, "%s enable bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    //register the callback function to the gattc module
    ret = esp_ble_gattc_register_callback(esp_gattc_cb);
    if (ret) {
        ESP_LOGE(BLE_ANCS_TAG, "%s gattc register error, error code = %x", __func__, ret);
        return;
    }

    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret) {
        ESP_LOGE(BLE_ANCS_TAG, "gap register error, error code = %x", ret);
        return;
    }

    ret = esp_ble_gattc_app_register(PROFILE_A_APP_ID);
    if (ret) {
        ESP_LOGE(BLE_ANCS_TAG, "%s gattc app register error, error code = %x", __func__, ret);
    }

    ret = esp_ble_gatt_set_local_mtu(500);
    if (ret) {
        ESP_LOGE(BLE_ANCS_TAG, "set local  MTU failed, error code = %x", ret);
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
}
