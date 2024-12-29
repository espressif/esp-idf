/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/****************************************************************************
*
* This is the demo to test the BLE throughput. It should be used together with throughput_server demo.
*
****************************************************************************/

#include "sdkconfig.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include "nvs.h"
#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_timer.h"

/**********************************************************
 * Thread/Task reference
 **********************************************************/
#ifdef CONFIG_BLUEDROID_PINNED_TO_CORE
#define BLUETOOTH_TASK_PINNED_TO_CORE              (CONFIG_BLUEDROID_PINNED_TO_CORE < CONFIG_FREERTOS_NUMBER_OF_CORES ? CONFIG_BLUEDROID_PINNED_TO_CORE : tskNO_AFFINITY)
#else
#define BLUETOOTH_TASK_PINNED_TO_CORE              (0)
#endif

#define REMOTE_SERVICE_UUID        0x00FF
#define REMOTE_NOTIFY_CHAR_UUID    0xFF01
#define PROFILE_NUM      1
#define PROFILE_A_APP_ID 0
#define INVALID_HANDLE   0
#define SECOND_TO_USECOND          1000000

#define EXT_SCAN_DURATION 0
#define EXT_SCAN_PERIOD 0

static const char *GATTC_TAG = "GATTC_DEMO_PHY";
static const char remote_device_name[] = "THROUGHPUT_PHY_DEMO";
static bool connect    = false;
static bool get_server = false;
static esp_gattc_char_elem_t *char_elem_result   = NULL;
static esp_gattc_descr_elem_t *descr_elem_result = NULL;
#if (CONFIG_GATTS_NOTIFY_THROUGHPUT)
static bool start = false;
static uint64_t notify_len = 0;
static uint64_t start_time = 0;
static uint64_t current_time = 0;
#endif /* #if (CONFIG_GATTS_NOTIFY_THROUGHPUT) */

#if (CONFIG_GATTC_WRITE_THROUGHPUT)
#define GATTC_WRITE_LEN 495

static bool can_send_write = false;
static SemaphoreHandle_t gattc_semaphore;
uint8_t write_data[GATTC_WRITE_LEN] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0e, 0x0f};
#endif /* #if (CONFIG_GATTC_WRITE_THROUGHPUT) */

static bool is_connect = false;

/* Declare static functions */
static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
static void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);
static void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);


static esp_bt_uuid_t remote_filter_service_uuid = {
    .len = ESP_UUID_LEN_16,
    .uuid = {.uuid16 = REMOTE_SERVICE_UUID,},
};

static esp_bt_uuid_t remote_filter_char_uuid = {
    .len = ESP_UUID_LEN_16,
    .uuid = {.uuid16 = REMOTE_NOTIFY_CHAR_UUID,},
};

static esp_bt_uuid_t notify_descr_uuid = {
    .len = ESP_UUID_LEN_16,
    .uuid = {.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG,},
};

static esp_ble_ext_scan_params_t ext_scan_params = {
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_duplicate = BLE_SCAN_DUPLICATE_DISABLE,
    .cfg_mask = ESP_BLE_GAP_EXT_SCAN_CFG_CODE_MASK | ESP_BLE_GAP_EXT_SCAN_CFG_UNCODE_MASK,
    .uncoded_cfg = {BLE_SCAN_TYPE_PASSIVE, 40, 40},
    .coded_cfg = {BLE_SCAN_TYPE_PASSIVE, 40, 40},
};

// If the interference in the air is severe, the connection interval can be reduced.
const esp_ble_conn_params_t phy_1m_conn_params = {
    .interval_max = 104,  // 130ms
    .interval_min = 104,
    .latency = 0,
    .max_ce_len = 0,
    .min_ce_len = 0,
    .scan_interval = 0x40,
    .scan_window = 0x40,
    .supervision_timeout = 600,
};

const esp_ble_conn_params_t phy_2m_conn_params = {
    .interval_max = 104,  // 130ms
    .interval_min = 104,
    .latency = 0,
    .max_ce_len = 0,
    .min_ce_len = 0,
    .scan_interval = 0x40,
    .scan_window = 0x40,
    .supervision_timeout = 600,
};

const esp_ble_conn_params_t phy_coded_conn_params = {
    .interval_max = 104,  // 130ms
    .interval_min = 104,
    .latency = 0,
    .max_ce_len = 0,
    .min_ce_len = 0,
    .scan_interval = 0x40,
    .scan_window = 0x40,
    .supervision_timeout = 600,
};

struct gattc_profile_inst {
    esp_gattc_cb_t gattc_cb;
    uint16_t gattc_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_start_handle;
    uint16_t service_end_handle;
    uint16_t char_handle;
    esp_bd_addr_t remote_bda;
};

/* One gatt-based profile one app_id and one gattc_if, this array will store the gattc_if returned by ESP_GATTS_REG_EVT */
static struct gattc_profile_inst gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_A_APP_ID] = {
        .gattc_cb = gattc_profile_event_handler,
        .gattc_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
};

static uint8_t check_sum(uint8_t *addr, uint16_t count)
{
    uint32_t sum = 0;

    if (addr == NULL || count == 0) {
        return 0;
    }

    for(int i = 0; i < count; i++) {
        sum = sum + addr[i];
    }

    while (sum >> 8) {
        sum = (sum & 0xff) + (sum >> 8);
    }

    return (uint8_t)~sum;
}

static void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    esp_ble_gattc_cb_param_t *p_data = (esp_ble_gattc_cb_param_t *)param;

    switch (event) {
    case ESP_GATTC_REG_EVT:
        ESP_LOGI(GATTC_TAG, "GATT client register, status %d, app_id %d, gattc_if %d", param->reg.status, param->reg.app_id, gattc_if);
        esp_err_t scan_ret = esp_ble_gap_set_ext_scan_params(&ext_scan_params);
        if (scan_ret){
            ESP_LOGE(GATTC_TAG, "set scan params error, error code = %x", scan_ret);
        }
        break;
    case ESP_GATTC_CONNECT_EVT: {
        ESP_LOGI(GATTC_TAG, "Connected, conn_id %d, remote "ESP_BD_ADDR_STR"", p_data->connect.conn_id,
                 ESP_BD_ADDR_HEX(p_data->connect.remote_bda));
        gl_profile_tab[PROFILE_A_APP_ID].conn_id = p_data->connect.conn_id;
        memcpy(gl_profile_tab[PROFILE_A_APP_ID].remote_bda, p_data->connect.remote_bda, sizeof(esp_bd_addr_t));
        esp_err_t mtu_ret = esp_ble_gattc_send_mtu_req (gattc_if, p_data->connect.conn_id);
        if (mtu_ret){
            ESP_LOGE(GATTC_TAG, "config MTU error, error code = %x", mtu_ret);
        }
        esp_ble_gap_read_phy(p_data->connect.remote_bda);
        break;
    }
    case ESP_GATTC_OPEN_EVT:
        if (param->open.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "Open failed, status %d", p_data->open.status);
            break;
        }
        ESP_LOGI(GATTC_TAG, "Open successfully, MTU %u", param->open.mtu);
        break;
    case ESP_GATTC_CFG_MTU_EVT:
        is_connect = true;
        ESP_LOGI(GATTC_TAG, "MTU exchange, status %d, MTU %d", param->cfg_mtu.status, param->cfg_mtu.mtu);
        esp_ble_gattc_search_service(gattc_if, param->cfg_mtu.conn_id, &remote_filter_service_uuid);
        break;
    case ESP_GATTC_SEARCH_RES_EVT: {
        ESP_LOGI(GATTC_TAG, "Service search result");
        esp_gatt_srvc_id_t *srvc_id =(esp_gatt_srvc_id_t *)&p_data->search_res.srvc_id;
        if (srvc_id->id.uuid.len == ESP_UUID_LEN_16 && srvc_id->id.uuid.uuid.uuid16 == REMOTE_SERVICE_UUID) {
            ESP_LOGI(GATTC_TAG, "Service found");
            get_server = true;
            gl_profile_tab[PROFILE_A_APP_ID].service_start_handle = p_data->search_res.start_handle;
            gl_profile_tab[PROFILE_A_APP_ID].service_end_handle = p_data->search_res.end_handle;
            ESP_LOGI(GATTC_TAG, "UUID16: %x", srvc_id->id.uuid.uuid.uuid16);
        }
        break;
    }
    case ESP_GATTC_SEARCH_CMPL_EVT:
        if (p_data->search_cmpl.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "Service search failed, status %x", p_data->search_cmpl.status);
            break;
        }
        ESP_LOGI(GATTC_TAG, "Service search complete");
        if (get_server){
            uint16_t count = 0;
            esp_gatt_status_t status = esp_ble_gattc_get_attr_count( gattc_if,
                                                                     p_data->search_cmpl.conn_id,
                                                                     ESP_GATT_DB_CHARACTERISTIC,
                                                                     gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                                                                     gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                                                                     INVALID_HANDLE,
                                                                     &count);
            if (status != ESP_GATT_OK){
                ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_attr_count error");
                break;
            }

            if (count > 0){
                char_elem_result = (esp_gattc_char_elem_t *)malloc(sizeof(esp_gattc_char_elem_t) * count);
                if (!char_elem_result){
                    ESP_LOGE(GATTC_TAG, "gattc no mem");
                    break;
                }else{
                    status = esp_ble_gattc_get_char_by_uuid( gattc_if,
                                                             p_data->search_cmpl.conn_id,
                                                             gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                                                             gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                                                             remote_filter_char_uuid,
                                                             char_elem_result,
                                                             &count);
                    if (status != ESP_GATT_OK) {
                        ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_char_by_uuid error");
                        free(char_elem_result);
                        char_elem_result = NULL;
                        break;
                    }

                    /*  Every service has only one char in our 'throughput_server' demo, so we use first 'char_elem_result' */
                    if (count > 0 && (char_elem_result[0].properties & ESP_GATT_CHAR_PROP_BIT_NOTIFY)){
                        gl_profile_tab[PROFILE_A_APP_ID].char_handle = char_elem_result[0].char_handle;
                        esp_ble_gattc_register_for_notify (gattc_if, gl_profile_tab[PROFILE_A_APP_ID].remote_bda, char_elem_result[0].char_handle);
                    }
                }
                /* free char_elem_result */
                free(char_elem_result);
                char_elem_result = NULL;
            }else{
                ESP_LOGE(GATTC_TAG, "no char found");
            }
        }
         break;
    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
        if (p_data->reg_for_notify.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "Notification register failed, status %d", p_data->reg_for_notify.status);
        }else{
            ESP_LOGI(GATTC_TAG, "Notification register successfully");
            uint16_t count = 0;
            uint16_t notify_en = 1;
            esp_gatt_status_t ret_status = esp_ble_gattc_get_attr_count( gattc_if,
                                                                         gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                                                         ESP_GATT_DB_DESCRIPTOR,
                                                                         gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                                                                         gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                                                                         gl_profile_tab[PROFILE_A_APP_ID].char_handle,
                                                                         &count);
            if (ret_status != ESP_GATT_OK){
                ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_attr_count error");
            }
            if (count > 0){
                descr_elem_result = malloc(sizeof(esp_gattc_descr_elem_t) * count);
                if (!descr_elem_result){
                    ESP_LOGE(GATTC_TAG, "malloc error, gattc no mem");
                }else{
                    ret_status = esp_ble_gattc_get_descr_by_char_handle( gattc_if,
                                                                         gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                                                         p_data->reg_for_notify.handle,
                                                                         notify_descr_uuid,
                                                                         descr_elem_result,
                                                                         &count);
                    if (ret_status != ESP_GATT_OK){
                        ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_descr_by_char_handle error");
                    }

                    /* Every char has only one descriptor in our 'throughput_server' demo, so we use first 'descr_elem_result' */
                    if (count > 0 && descr_elem_result[0].uuid.len == ESP_UUID_LEN_16 && descr_elem_result[0].uuid.uuid.uuid16 == ESP_GATT_UUID_CHAR_CLIENT_CONFIG){
                        ret_status = esp_ble_gattc_write_char_descr( gattc_if,
                                                                     gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                                                     descr_elem_result[0].handle,
                                                                     sizeof(notify_en),
                                                                     (uint8_t *)&notify_en,
                                                                     ESP_GATT_WRITE_TYPE_RSP,
                                                                     ESP_GATT_AUTH_REQ_NONE);
                    }

                    if (ret_status != ESP_GATT_OK){
                        ESP_LOGE(GATTC_TAG, "esp_ble_gattc_write_char_descr error");
                    }

                    /* free descr_elem_result */
                    free(descr_elem_result);
                }
            }
            else{
                ESP_LOGE(GATTC_TAG, "decsr not found");
            }

        }
        break;
    }
    case ESP_GATTC_NOTIFY_EVT: {
#if (CONFIG_GATTS_NOTIFY_THROUGHPUT)
        if (p_data->notify.is_notify &&
            (p_data->notify.value[p_data->notify.value_len - 1] ==
             check_sum(p_data->notify.value, p_data->notify.value_len - 1))){
            notify_len += p_data->notify.value_len;
        } else {
            ESP_LOGE(GATTC_TAG, "Indication received, value:");
        }
        if (start == false) {
            start_time = esp_timer_get_time();
            start = true;
            break;
        }

#endif /* #if (CONFIG_GATTS_NOTIFY_THROUGHPUT) */
        break;
    }
    case ESP_GATTC_WRITE_DESCR_EVT:
        if (p_data->write.status != ESP_GATT_OK) {
            ESP_LOGE(GATTC_TAG, "Descriptor write failed, status %x", p_data->write.status);
            break;
        }
        ESP_LOGI(GATTC_TAG, "Descriptor write successfully");
#if (CONFIG_GATTC_WRITE_THROUGHPUT)
        can_send_write = true;
        xSemaphoreGive(gattc_semaphore);
#endif /* #if (CONFIG_GATTC_WRITE_THROUGHPUT) */
        break;
    case ESP_GATTC_SRVC_CHG_EVT: {
        esp_bd_addr_t bda;
        memcpy(bda, p_data->srvc_chg.remote_bda, sizeof(esp_bd_addr_t));
        ESP_LOGI(GATTC_TAG, "Service change from "ESP_BD_ADDR_STR"", ESP_BD_ADDR_HEX(bda));
        break;
    }
    case ESP_GATTC_WRITE_CHAR_EVT:
        if (p_data->write.status != ESP_GATT_OK) {
            ESP_LOGE(GATTC_TAG, "Characteristic write failed, status %x", p_data->write.status);
            break;
        }
        break;
    case ESP_GATTC_DISCONNECT_EVT:
        is_connect = false;
        get_server = false;
#if (CONFIG_GATTS_NOTIFY_THROUGHPUT)
        start = false;
        start_time = 0;
        current_time = 0;
        notify_len = 0;
#endif /* #if (CONFIG_GATTS_NOTIFY_THROUGHPUT) */
        ESP_LOGI(GATTC_TAG, "Disconnected, remote "ESP_BD_ADDR_STR", reason 0x%02x",
                 ESP_BD_ADDR_HEX(p_data->disconnect.remote_bda), p_data->disconnect.reason);
        break;
     case ESP_GATTC_CONGEST_EVT:
#if (CONFIG_GATTC_WRITE_THROUGHPUT)
        if (param->congest.congested) {
            can_send_write = false;
        } else {
            can_send_write = true;
            xSemaphoreGive(gattc_semaphore);
        }
#endif /* #if (CONFIG_GATTC_WRITE_THROUGHPUT) */
        break;
    default:
        break;
    }
}

static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT: {
        if (param->set_ext_scan_params.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTC_TAG,"Extend scanning parameters set failed, status %x", param->set_ext_scan_params.status);
            break;
        }
        esp_ble_gap_start_ext_scan(EXT_SCAN_DURATION,EXT_SCAN_PERIOD);
        break;
    }
    case ESP_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT:
        //scan start complete event to indicate scan start successfully or failed
        if (param->ext_scan_start.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTC_TAG, "Extended scanning start failed, status %x", param->ext_scan_start.status);
            break;
        }
        ESP_LOGI(GATTC_TAG, "Extended scanning start successfully");
        break;
    case ESP_GAP_BLE_EXT_ADV_REPORT_EVT: {
        uint8_t *adv_name = NULL;
        uint8_t adv_name_len = 0;
        if(param->ext_adv_report.params.event_type & ESP_BLE_GAP_SET_EXT_ADV_PROP_LEGACY) {
            ESP_LOGI(GATTC_TAG, "Legacy adv, adv type 0x%x data len %d", param->ext_adv_report.params.event_type, param->ext_adv_report.params.adv_data_len);
        } else {
            ESP_LOGI(GATTC_TAG, "Extend adv, adv type 0x%x data len %d", param->ext_adv_report.params.event_type, param->ext_adv_report.params.adv_data_len);
        }
        adv_name = esp_ble_resolve_adv_data_by_type(param->ext_adv_report.params.adv_data,
                                            param->ext_adv_report.params.adv_data_len,
                                            ESP_BLE_AD_TYPE_NAME_CMPL,
                                            &adv_name_len);
        if (!connect && strlen(remote_device_name) == adv_name_len && strncmp((char *)adv_name, remote_device_name, adv_name_len) == 0) {
            ESP_LOGI(GATTC_TAG, "searched device %s", remote_device_name);
            connect = true;
            esp_ble_gap_stop_ext_scan();
            ESP_LOGI(GATTC_TAG, "Device found "ESP_BD_ADDR_STR"", ESP_BD_ADDR_HEX(param->ext_adv_report.params.addr));
            ESP_LOG_BUFFER_CHAR("Adv name", adv_name, adv_name_len);
            ESP_LOGI(GATTC_TAG, "Stop extend scan and create aux open, primary_phy %d secondary phy %d", param->ext_adv_report.params.primary_phy, param->ext_adv_report.params.secondly_phy);
            esp_ble_gatt_creat_conn_params_t creat_conn_params = {0};
            memcpy(&creat_conn_params.remote_bda, param->ext_adv_report.params.addr, ESP_BD_ADDR_LEN);
            creat_conn_params.remote_addr_type = param->ext_adv_report.params.addr_type;
            creat_conn_params.own_addr_type = BLE_ADDR_TYPE_PUBLIC;
            creat_conn_params.is_direct = true;
            creat_conn_params.is_aux = true;
            creat_conn_params.phy_mask = ESP_BLE_PHY_1M_PREF_MASK | ESP_BLE_PHY_2M_PREF_MASK | ESP_BLE_PHY_CODED_PREF_MASK;
            creat_conn_params.phy_1m_conn_params = &phy_1m_conn_params;
            creat_conn_params.phy_2m_conn_params = &phy_2m_conn_params;
            creat_conn_params.phy_coded_conn_params = &phy_coded_conn_params;
            esp_ble_gattc_enh_open(gl_profile_tab[PROFILE_A_APP_ID].gattc_if, &creat_conn_params);
        }
        break;
    }
    case ESP_GAP_BLE_EXT_SCAN_STOP_COMPLETE_EVT:
        if (param->ext_scan_stop.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTC_TAG, "Scanning stop failed, status %x", param->ext_scan_stop.status);
            break;
        }
        ESP_LOGI(GATTC_TAG, "Scanning stop successfully");
        break;
    case ESP_GAP_BLE_EXT_ADV_STOP_COMPLETE_EVT:
        if (param->ext_adv_stop.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTC_TAG, "Advertising stop failed, status %x", param->ext_adv_stop.status);
            break;
        }
        ESP_LOGI(GATTC_TAG, "Advertising stop successfully");
        break;
    case ESP_GAP_BLE_READ_PHY_COMPLETE_EVT:
        ESP_LOGI(GATTC_TAG, "Read PHY, status %x, TX_PHY %u, RX_PHY %u", param->read_phy.status,
                 param->read_phy.tx_phy, param->read_phy.rx_phy);
        break;
    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
         ESP_LOGI(GATTC_TAG, "Connection params update, status %d, conn_int %d, latency %d, timeout %d",
                  param->update_conn_params.status,
                  param->update_conn_params.conn_int,
                  param->update_conn_params.latency,
                  param->update_conn_params.timeout);
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
            ESP_LOGI(GATTC_TAG, "reg app failed, app_id %04x, status %d",
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

#if (CONFIG_GATTC_WRITE_THROUGHPUT)
static void throughput_client_task(void *param)
{
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    uint8_t sum = check_sum(write_data, sizeof(write_data) - 1);
    write_data[GATTC_WRITE_LEN - 1] = sum;

    while(1) {

            if (!can_send_write) {
                int res = xSemaphoreTake(gattc_semaphore, portMAX_DELAY);
                assert(res == pdTRUE);
            } else {
                if (is_connect) {
                    int free_buff_num = esp_ble_get_cur_sendable_packets_num(gl_profile_tab[PROFILE_A_APP_ID].conn_id);
                    if(free_buff_num > 0) {
                        for( ; free_buff_num > 0; free_buff_num--) {
                            // the app data set to 490 just for divided into two packages to send in the low layer
                            // when the packet length set to 251.
                            esp_ble_gattc_write_char(gl_profile_tab[PROFILE_A_APP_ID].gattc_if,
                                                            gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                                            gl_profile_tab[PROFILE_A_APP_ID].char_handle,
                                                            sizeof(write_data), write_data,
                                                            ESP_GATT_WRITE_TYPE_NO_RSP,
                                                            ESP_GATT_AUTH_REQ_NONE);
                        }
                    } else { //Add the vTaskDelay to prevent this task from consuming the CPU all the time, causing low-priority tasks to not be executed at all.
                        vTaskDelay( 10 / portTICK_PERIOD_MS );
                    }
                } else {
                    vTaskDelay(300 / portTICK_PERIOD_MS );
                }
            }

    }
}
#endif /* #if (CONFIG_GATTC_WRITE_THROUGHPUT) */

#if (CONFIG_GATTS_NOTIFY_THROUGHPUT)
static void throughput_cal_task(void *param)
{
    while (1)
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        if(is_connect){
            uint32_t bit_rate = 0;
            if (start_time) {
                current_time = esp_timer_get_time();
                bit_rate = notify_len * SECOND_TO_USECOND / (current_time - start_time);
                ESP_LOGI(GATTC_TAG, "Notify Bit rate = %" PRIu32 " Byte/s, = %" PRIu32 " bit/s, time = %ds",
                        bit_rate, bit_rate<<3, (int)((current_time - start_time) / SECOND_TO_USECOND));
            } else {
                ESP_LOGI(GATTC_TAG, "Notify Bit rate = 0 Byte/s, = 0 bit/s");
            }
        }
    }

}
#endif /* #if (CONFIG_GATTS_NOTIFY_THROUGHPUT) */

void app_main(void)
{
    // Initialize NVS.
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(GATTC_TAG, "%s initialize controller failed, error code = %x", __func__, ret);
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(GATTC_TAG, "%s enable controller failed, error code = %x", __func__, ret);
        return;
    }

    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(GATTC_TAG, "%s init bluetooth failed, error code = %x", __func__, ret);
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(GATTC_TAG, "%s enable bluetooth failed, error code = %x", __func__, ret);
        return;
    }

    //register the  callback function to the gap module
    ret = esp_ble_gap_register_callback(esp_gap_cb);
    if (ret){
        ESP_LOGE(GATTC_TAG, "%s gap register failed, error code = %x", __func__, ret);
        return;
    }

    //register the callback function to the gattc module
    ret = esp_ble_gattc_register_callback(esp_gattc_cb);
    if(ret){
        ESP_LOGE(GATTC_TAG, "%s gattc register failed, error code = %x", __func__, ret);
        return;
    }

    ret = esp_ble_gattc_app_register(PROFILE_A_APP_ID);
    if (ret){
        ESP_LOGE(GATTC_TAG, "%s gattc app register failed, error code = %x", __func__, ret);
    }
    // set the maximum MTU for used.
    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(517);
    if (local_mtu_ret){
        ESP_LOGE(GATTC_TAG, "set local  MTU failed, error code = %x", local_mtu_ret);
    }
#if (CONFIG_GATTC_WRITE_THROUGHPUT)
    // The task is only created on the CPU core that Bluetooth is working on,
    // preventing the sending task from using the un-updated Bluetooth state on another CPU.
    xTaskCreatePinnedToCore(&throughput_client_task, "throughput_client_task", 4096, NULL, 10, NULL, BLUETOOTH_TASK_PINNED_TO_CORE);
#endif

#if (CONFIG_GATTS_NOTIFY_THROUGHPUT)
    xTaskCreatePinnedToCore(&throughput_cal_task, "throughput_cal_task", 4096, NULL, 9, NULL, BLUETOOTH_TASK_PINNED_TO_CORE);
#endif

#if (CONFIG_GATTC_WRITE_THROUGHPUT)
    gattc_semaphore = xSemaphoreCreateBinary();
    if (!gattc_semaphore) {
        ESP_LOGE(GATTC_TAG, "%s, init fail, the gattc semaphore create fail.", __func__);
        return;
    }
#endif /* #if (CONFIG_GATTC_WRITE_THROUGHPUT) */
}
