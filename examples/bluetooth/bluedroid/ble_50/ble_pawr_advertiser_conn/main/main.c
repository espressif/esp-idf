/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/****************************************************************************
*
* This demo showcases BLE Periodic Advertising with Responses (PAwR) on the advertiser side
* with connection capability. The device starts extended advertising and periodic advertising
* with subevents. When the host receives ESP_GAP_BLE_PERIODIC_ADV_SUBEVT_DATA_REQUEST_EVT,
* it provides per-subevent data via esp_ble_gap_set_periodic_adv_subevent_data().
* If a synchronizer sends responses containing connection information, the advertiser will
* receive ESP_GAP_BLE_PERIODIC_ADV_RESPONSE_REPORT_EVT and can establish a connection using
* esp_ble_gattc_aux_open_with_pawr_synced().
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_defs.h"

#include "sdkconfig.h"

#define TAG "BLE_PAWR_ADV_CONN"

#define EXT_ADV_HANDLE      0
#define NUM_EXT_ADV         1
#define PROFILE_A_APP_ID    0

#define SEM_WAIT_TIMEOUT_MS     5000

/* Service UUID and Characteristic UUID (matching synchronizer) */
#define GATTS_SERVICE_UUID_PAWR_SYNC     0x00FF
#define GATTS_CHAR_UUID_PAWR_SYNC        0xFF01
#define INVALID_HANDLE                   0xFFFF

#define FUNC_SEND_WAIT_SEM(func, sem) do {\
        esp_err_t __err_rc = (func);\
        if (__err_rc != ESP_OK) { \
            ESP_LOGE(TAG, "%s failed, error = %d", #func, __err_rc); \
            return; \
        } \
        if (xSemaphoreTake(sem, pdMS_TO_TICKS(SEM_WAIT_TIMEOUT_MS)) != pdTRUE) { \
            ESP_LOGE(TAG, "%s, semaphore timeout", __func__); \
            return; \
        } \
} while(0);

// PAwR parameters
#define BLE_PAWR_SUB_DATA_LEN                 (20)

static SemaphoreHandle_t test_sem = NULL;
static esp_ble_subevent_params sub_data_params[10];
static uint8_t sub_data_pattern[BLE_PAWR_SUB_DATA_LEN] = {0};
static uint8_t subevent_data_buffers[10][BLE_PAWR_SUB_DATA_LEN];
static bool is_connected = false;
static uint16_t conn_id = 0xFFFF;

// Connection parameters
static esp_ble_conn_params_t conn_params = {
    .interval_min = 0x20,  // 40 * 1.25ms = 50ms
    .interval_max = 0x20,  // 40 * 1.25ms = 50ms
    .latency = 0,
    .supervision_timeout = 0x1F4,  // 500 * 10ms = 5000ms
};

// Extended advertising data
static uint8_t raw_ext_adv_data[] = {
    0x02, ESP_BLE_AD_TYPE_FLAG, 0x06,
    0x02, ESP_BLE_AD_TYPE_TX_PWR, 0xeb,
    0x10, ESP_BLE_AD_TYPE_NAME_CMPL, 'E', 'S', 'P', '_', 'E', 'X', 'T', 'P', 'A', 'w', 'R', '_', 'A', 'D', 'V'
};

static esp_ble_gap_ext_adv_params_t ext_adv_params = {
    .type = ESP_BLE_GAP_SET_EXT_ADV_PROP_NONCONN_NONSCANNABLE_UNDIRECTED,
    .interval_min = 600,
    .interval_max = 600,
    .channel_map = ADV_CHNL_ALL,
    .filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
    .primary_phy = ESP_BLE_GAP_PHY_1M,
    .max_skip = 0,
    .secondary_phy = ESP_BLE_GAP_PHY_2M,
    .sid = 0,
    .scan_req_notif = false,
    .own_addr_type = BLE_ADDR_TYPE_RANDOM,
    .tx_power = EXT_ADV_TX_PWR_NO_PREFERENCE,
};

static esp_ble_gap_periodic_adv_params_t periodic_adv_params = {
    .interval_min = 600,
    .interval_max = 600,
    .properties = 0, // Do not include TX power
    .num_subevents = 10,
    .subevent_interval = 44,
    .rsp_slot_delay = 20,
    .rsp_slot_spacing = 32,
    .num_rsp_slots = 5,
};

static esp_ble_gap_ext_adv_t ext_adv[1] = {
    [0] = {EXT_ADV_HANDLE, 0, 0},
};

// Forward declaration
static void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);

static struct gattc_profile_inst {
    esp_gattc_cb_t gattc_cb;
    uint16_t gattc_if;
    uint16_t app_id;
    uint16_t conn_id;
    esp_bd_addr_t remote_bda;
    uint16_t service_start_handle;
    uint16_t service_end_handle;
    uint16_t char_handle;
} gl_profile_tab[1] = {
    [PROFILE_A_APP_ID] = {
        .gattc_cb = gattc_profile_event_handler,
        .gattc_if = ESP_GATT_IF_NONE,
        .service_start_handle = INVALID_HANDLE,
        .service_end_handle = INVALID_HANDLE,
        .char_handle = INVALID_HANDLE,
    },
};

/* UUID for service discovery */
static esp_bt_uuid_t remote_filter_service_uuid = {
    .len = ESP_UUID_LEN_16,
    .uuid = {.uuid16 = GATTS_SERVICE_UUID_PAWR_SYNC},
};

static esp_bt_uuid_t remote_filter_char_uuid = {
    .len = ESP_UUID_LEN_16,
    .uuid = {.uuid16 = GATTS_CHAR_UUID_PAWR_SYNC},
};

static esp_bt_uuid_t notify_descr_uuid = {
    .len = ESP_UUID_LEN_16,
    .uuid = {.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG},
};

static bool get_server = false;

/**
 * @brief Print connection information
 */
static void print_conn_info(uint16_t conn_id, esp_bd_addr_t remote_bda)
{
    ESP_LOGI(TAG, "Connection Information:");
    ESP_LOGI(TAG, "  Connection ID: 0x%04x", conn_id);
    ESP_LOGI(TAG, "  Remote Address: "ESP_BD_ADDR_STR"", ESP_BD_ADDR_HEX(remote_bda));
}

/**
 * @brief GAP event handler
 */
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    if (param == NULL) {
        ESP_LOGE(TAG, "GAP event handler received NULL param");
        return;
    }

    switch (event) {
    case ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(TAG, "Extended advertising params set, status %d, instance %d",
                 param->ext_adv_set_params.status, param->ext_adv_set_params.instance);
        break;
    case ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(TAG, "Extended advertising data set, status %d, instance %d",
                 param->ext_adv_data_set.status, param->ext_adv_data_set.instance);
        break;
    case ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(TAG, "Extended advertising start, status %d, instance num %d",
                 param->ext_adv_start.status, param->ext_adv_start.instance_num);
        break;
    case ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(TAG, "Extended advertising set random address complete, status %d, instance %d",
                 param->ext_adv_set_rand_addr.status, param->ext_adv_set_rand_addr.instance);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_SET_PARAMS_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(TAG, "Periodic advertising params set, status %d, instance %d",
                 param->peroid_adv_set_params.status, param->peroid_adv_set_params.instance);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_START_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(TAG, "Periodic advertising start, status %d, instance %d",
                 param->period_adv_start.status, param->period_adv_start.instance);
        break;
    case ESP_GAP_BLE_SET_PERIODIC_ADV_SUBEVT_DATA_EVT:
        if (param->pa_subevt_data_evt.status != ESP_OK) {
            if (param->pa_subevt_data_evt.status == 0x146)
            {
                ESP_LOGW(TAG, "Periodic adv subevent data set too late, status %d, adv_handle %d",
                         param->pa_subevt_data_evt.status, param->pa_subevt_data_evt.adv_handle);
            } else {
                ESP_LOGE(TAG, "Periodic adv subevent data set failed, status %d, adv_handle %d",
                         param->pa_subevt_data_evt.status, param->pa_subevt_data_evt.adv_handle);
            }
            break;
        }
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_SUBEVT_DATA_REQUEST_EVT: {
        // ESP_LOGI(TAG, "[Request] data: 0x%02x, subevt start:%d, subevt count:%d",
        //          sub_data_pattern[0],
        //          param->pa_subevt_data_req_evt.subevt_start,
        //          param->pa_subevt_data_req_evt.subevt_data_count);

        uint8_t sent_num = param->pa_subevt_data_req_evt.subevt_data_count;
        if (sent_num > 10) {
            ESP_LOGE(TAG, "Invalid subevent count: %d", sent_num);
            sent_num = 10;
        }

        // Prepare subevent data
        for (uint8_t i = 0; i < sent_num && i < 10; i++) {
            uint8_t sub = (i + param->pa_subevt_data_req_evt.subevt_start) % 10;

            // Prepare data pattern with overflow protection
            sub_data_pattern[0] = (sub_data_pattern[0] + 1) % 256;
            memset(&sub_data_pattern[1], sub, BLE_PAWR_SUB_DATA_LEN - 1);

            // Copy to buffer
            memcpy(subevent_data_buffers[i], sub_data_pattern, BLE_PAWR_SUB_DATA_LEN);

            // Set subevent parameters
            sub_data_params[i].subevent = sub;
            sub_data_params[i].response_slot_start = 0;
            sub_data_params[i].response_slot_count = 5;
            sub_data_params[i].subevent_data_len = BLE_PAWR_SUB_DATA_LEN;
            sub_data_params[i].subevent_data = subevent_data_buffers[i];
        }

        // Set subevent data
        esp_ble_per_adv_subevent_data_params subevent_data_params = {0};
        subevent_data_params.adv_handle = param->pa_subevt_data_req_evt.adv_handle;
        subevent_data_params.num_subevents_with_data = sent_num;
        subevent_data_params.subevent_params = sub_data_params;

        esp_err_t ret = esp_ble_gap_set_periodic_adv_subevent_data(&subevent_data_params);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set Subevent Data, error = 0x%x", ret);
        }
        break;
    }
    case ESP_GAP_BLE_PERIODIC_ADV_RESPONSE_REPORT_EVT: {
        // ESP_LOGI(TAG, "[Response] adv_handle:%d, subevent:%d, tx_status:%d, num_rsp:%d",
        //          param->pa_rsp_rpt_evt.adv_handle,
        //          param->pa_rsp_rpt_evt.subevt,
        //          param->pa_rsp_rpt_evt.tx_status,
        //          param->pa_rsp_rpt_evt.num_rsp);

        if (param->pa_rsp_rpt_evt.pa_rsp_info && param->pa_rsp_rpt_evt.num_rsp > 0) {
            // Limit num_rsp to prevent array overflow
            uint8_t num_rsp = (param->pa_rsp_rpt_evt.num_rsp > 10) ? 10 : param->pa_rsp_rpt_evt.num_rsp;
            for (uint8_t i = 0; i < num_rsp && i < 10; i++) {
                esp_ble_pa_rsp_info *rsp_info = &param->pa_rsp_rpt_evt.pa_rsp_info[i];
                if (rsp_info->data_status == 0x00) {  // Data complete

                    if (rsp_info->data && rsp_info->data_len >= 10) {
                        // Extract peer address from response data (assuming format: [0x00, 0x02, addr[5], addr[4], addr[3], addr[2], addr[1], addr[0], ...])
                        // Need at least 8 bytes: [0x00, 0x02, addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]]
                        // Already validated data_len >= 10 above
                        if (!is_connected && gl_profile_tab[PROFILE_A_APP_ID].gattc_if != ESP_GATT_IF_NONE) {
                            ESP_LOGI(TAG, "Response slot:%d, data_length:%d", rsp_info->rsp_slot, rsp_info->data_len);
                            ESP_LOGI(TAG, "data: 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x",
                                rsp_info->data[0], rsp_info->data[1], rsp_info->data[2], rsp_info->data[3],
                                rsp_info->data[4], rsp_info->data[5], rsp_info->data[6], rsp_info->data[7],
                                rsp_info->data[8], rsp_info->data[9]);

                            esp_bd_addr_t peer_addr;
                            peer_addr[0] = rsp_info->data[7];
                            peer_addr[1] = rsp_info->data[6];
                            peer_addr[2] = rsp_info->data[5];
                            peer_addr[3] = rsp_info->data[4];
                            peer_addr[4] = rsp_info->data[3];
                            peer_addr[5] = rsp_info->data[2];
                            uint8_t adv_handle = param->pa_rsp_rpt_evt.adv_handle;
                            uint8_t subevent = param->pa_rsp_rpt_evt.subevt;
                            uint8_t phy_mask = ESP_BLE_PHY_1M_PREF_MASK;

                            // Connect using PAwR synced connection
                            esp_ble_gatt_pawr_conn_params_t pawr_conn_params = {0};
                            memcpy(pawr_conn_params.remote_bda, peer_addr, ESP_BD_ADDR_LEN);
                            pawr_conn_params.remote_addr_type = BLE_ADDR_TYPE_PUBLIC;
                            pawr_conn_params.own_addr_type = BLE_ADDR_TYPE_PUBLIC;
                            pawr_conn_params.adv_handle = adv_handle;
                            pawr_conn_params.subevent = subevent;
                            pawr_conn_params.phy_mask = phy_mask;
                            pawr_conn_params.phy_1m_conn_params = &conn_params;
                            // Create PAwR synced connection
                            esp_err_t ret = esp_ble_gattc_aux_open_with_pawr_synced(gl_profile_tab[PROFILE_A_APP_ID].gattc_if, &pawr_conn_params);
                            if (ret != ESP_OK) {
                                ESP_LOGE(TAG, "Failed to connect to device, error = 0x%x", ret);
                            } else {
                                ESP_LOGI(TAG, "Connection create sent, adv handle = %d, subevent = %d", adv_handle, subevent);
                                ESP_LOG_BUFFER_HEX(TAG, peer_addr, ESP_BD_ADDR_LEN);
                                is_connected = true;
                            }
                        } else if (is_connected) {
                            ESP_LOGD(TAG, "Already connected, skipping connection attempt");
                        } else if (gl_profile_tab[PROFILE_A_APP_ID].gattc_if == ESP_GATT_IF_NONE) {
                            ESP_LOGW(TAG, "GATTC interface not ready, cannot connect");
                        }
                    }
                } else {
                    ESP_LOGW(TAG, "Response slot:%d, data_status:%d",
                             rsp_info->rsp_slot, rsp_info->data_status);
                }
            }
        }
        break;
    }
    default:
        break;
    }
}

/**
 * @brief GATTC event handler
 */
static void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    if (param == NULL) {
        ESP_LOGE(TAG, "GATTC event handler received NULL param");
        return;
    }

    esp_ble_gattc_cb_param_t *p_data = (esp_ble_gattc_cb_param_t *)param;

    switch (event) {
    case ESP_GATTC_REG_EVT: {
        ESP_LOGI(TAG, "GATTC register app_id %04x, status %d, gattc_if %d",
                 p_data->reg.app_id, p_data->reg.status, gattc_if);
        if (p_data->reg.status == ESP_GATT_OK) {
            gl_profile_tab[PROFILE_A_APP_ID].gattc_if = gattc_if;
        }
        break;
    }
    case ESP_GATTC_CONNECT_EVT: {
        ESP_LOGI(TAG, "[Connection established], conn_id = 0x%04x", p_data->connect.conn_id);
        conn_id = p_data->connect.conn_id;
        memcpy(gl_profile_tab[PROFILE_A_APP_ID].remote_bda, p_data->connect.remote_bda, ESP_BD_ADDR_LEN);
        gl_profile_tab[PROFILE_A_APP_ID].conn_id = p_data->connect.conn_id;
        print_conn_info(conn_id, p_data->connect.remote_bda);

        // Request MTU exchange
        esp_err_t mtu_ret = esp_ble_gattc_send_mtu_req(gattc_if, p_data->connect.conn_id);
        if (mtu_ret != ESP_OK) {
            ESP_LOGE(TAG, "Config MTU error, error code = %x", mtu_ret);
        }
        break;
    }
    case ESP_GATTC_OPEN_EVT:
        if (p_data->open.status != ESP_GATT_OK) {
            ESP_LOGE(TAG, "GATTC open failed, status %d", p_data->open.status);
            break;
        }
        ESP_LOGI(TAG, "GATTC open successfully, conn_id = 0x%04x, MTU %u",
                 p_data->open.conn_id, p_data->open.mtu);
        // Start service discovery
        esp_err_t ret = esp_ble_gattc_search_service(gattc_if, p_data->open.conn_id, NULL);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Search service failed, error code = %x", ret);
        }
        break;
    case ESP_GATTC_CFG_MTU_EVT:
        ESP_LOGI(TAG, "MTU exchange, status %d, MTU %d", param->cfg_mtu.status, param->cfg_mtu.mtu);
        break;
    case ESP_GATTC_DIS_SRVC_CMPL_EVT:
        if (param->dis_srvc_cmpl.status != ESP_GATT_OK) {
            ESP_LOGE(TAG, "Service discover failed, status %d", param->dis_srvc_cmpl.status);
            break;
        }
        ESP_LOGI(TAG, "Service discover complete, conn_id %d", param->dis_srvc_cmpl.conn_id);
        // Search for specific service UUID
        esp_err_t search_ret = esp_ble_gattc_search_service(gattc_if, param->dis_srvc_cmpl.conn_id, &remote_filter_service_uuid);
        if (search_ret != ESP_OK) {
            ESP_LOGE(TAG, "Search service failed, error code = %x", search_ret);
        }
        break;
    case ESP_GATTC_SEARCH_RES_EVT: {
        ESP_LOGI(TAG, "Service search result, conn_id = 0x%04x, is_primary %d",
                 p_data->search_res.conn_id, p_data->search_res.is_primary);
        ESP_LOGI(TAG, "start handle %d, end handle %d",
                 p_data->search_res.start_handle, p_data->search_res.end_handle);
        if (p_data->search_res.srvc_id.uuid.len == ESP_UUID_LEN_16 &&
            p_data->search_res.srvc_id.uuid.uuid.uuid16 == GATTS_SERVICE_UUID_PAWR_SYNC) {
            ESP_LOGI(TAG, "PAwR Sync Service found");
            get_server = true;
            gl_profile_tab[PROFILE_A_APP_ID].service_start_handle = p_data->search_res.start_handle;
            gl_profile_tab[PROFILE_A_APP_ID].service_end_handle = p_data->search_res.end_handle;
            ESP_LOGI(TAG, "Service UUID16: 0x%04x", p_data->search_res.srvc_id.uuid.uuid.uuid16);
        }
        break;
    }
    case ESP_GATTC_SEARCH_CMPL_EVT:
        if (p_data->search_cmpl.status != ESP_GATT_OK) {
            ESP_LOGE(TAG, "Service search failed, status 0x%02x", p_data->search_cmpl.status);
            break;
        }
        ESP_LOGI(TAG, "Service search complete");
        if (get_server) {
            uint16_t count = 0;
            esp_gatt_status_t status = esp_ble_gattc_get_attr_count(gattc_if,
                                                                    p_data->search_cmpl.conn_id,
                                                                    ESP_GATT_DB_CHARACTERISTIC,
                                                                    gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                                                                    gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                                                                    INVALID_HANDLE,
                                                                    &count);
            if (status != ESP_GATT_OK) {
                ESP_LOGE(TAG, "esp_ble_gattc_get_attr_count error, status = 0x%02x", status);
                break;
            }

            if (count > 0) {
                esp_gattc_char_elem_t *char_elem_result = (esp_gattc_char_elem_t *)malloc(sizeof(esp_gattc_char_elem_t) * count);
                if (!char_elem_result) {
                    ESP_LOGE(TAG, "gattc no mem");
                    break;
                } else {
                    status = esp_ble_gattc_get_char_by_uuid(gattc_if,
                                                             p_data->search_cmpl.conn_id,
                                                             gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                                                             gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                                                             remote_filter_char_uuid,
                                                             char_elem_result,
                                                             &count);
                    if (status != ESP_GATT_OK) {
                        ESP_LOGE(TAG, "esp_ble_gattc_get_char_by_uuid error, status = 0x%02x", status);
                        free(char_elem_result);
                        break;
                    }

                    if (count > 0) {
                        gl_profile_tab[PROFILE_A_APP_ID].char_handle = char_elem_result[0].char_handle;
                        ESP_LOGI(TAG, "Characteristic found, handle = %d, properties = 0x%02x",
                                 char_elem_result[0].char_handle, char_elem_result[0].properties);

                        // Register for notify if supported
                        if (char_elem_result[0].properties & ESP_GATT_CHAR_PROP_BIT_NOTIFY) {
                            esp_err_t reg_ret = esp_ble_gattc_register_for_notify(gattc_if,
                                                                                   gl_profile_tab[PROFILE_A_APP_ID].remote_bda,
                                                                                   char_elem_result[0].char_handle);
                            if (reg_ret != ESP_OK) {
                                ESP_LOGE(TAG, "Register for notify failed, error code = %x", reg_ret);
                            }
                        }
                    }
                    free(char_elem_result);
                }
            } else {
                ESP_LOGE(TAG, "no char found");
            }
        }
        break;
    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
        if (p_data->reg_for_notify.status != ESP_GATT_OK) {
            ESP_LOGE(TAG, "Notification register failed, status %d", p_data->reg_for_notify.status);
            break;
        }
        ESP_LOGI(TAG, "Notification register successfully, handle = %d", p_data->reg_for_notify.handle);

        // Enable notify by writing to CCCD
        uint16_t count = 0;
        esp_gatt_status_t ret_status = esp_ble_gattc_get_attr_count(gattc_if,
                                                                    gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                                                    ESP_GATT_DB_DESCRIPTOR,
                                                                    gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                                                                    gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                                                                    gl_profile_tab[PROFILE_A_APP_ID].char_handle,
                                                                    &count);
        if (ret_status != ESP_GATT_OK) {
            ESP_LOGE(TAG, "esp_ble_gattc_get_attr_count error, status = 0x%02x", ret_status);
            break;
        }

        if (count > 0) {
            esp_gattc_descr_elem_t *descr_elem_result = malloc(sizeof(esp_gattc_descr_elem_t) * count);
            if (!descr_elem_result) {
                ESP_LOGE(TAG, "malloc error, gattc no mem");
                break;
            }

            ret_status = esp_ble_gattc_get_descr_by_char_handle(gattc_if,
                                                                  gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                                                  p_data->reg_for_notify.handle,
                                                                  notify_descr_uuid,
                                                                  descr_elem_result,
                                                                  &count);
            if (ret_status != ESP_GATT_OK) {
                ESP_LOGE(TAG, "esp_ble_gattc_get_descr_by_char_handle error, status = 0x%02x", ret_status);
                free(descr_elem_result);
                break;
            }

            if (count > 0 && descr_elem_result[0].uuid.len == ESP_UUID_LEN_16 &&
                descr_elem_result[0].uuid.uuid.uuid16 == ESP_GATT_UUID_CHAR_CLIENT_CONFIG) {
                uint16_t notify_en = 1;  // Enable notify
                ret_status = esp_ble_gattc_write_char_descr(gattc_if,
                                                              gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                                              descr_elem_result[0].handle,
                                                              sizeof(notify_en),
                                                              (uint8_t *)&notify_en,
                                                              ESP_GATT_WRITE_TYPE_RSP,
                                                              ESP_GATT_AUTH_REQ_NONE);
                if (ret_status != ESP_GATT_OK) {
                    ESP_LOGE(TAG, "esp_ble_gattc_write_char_descr error, status = 0x%02x", ret_status);
                } else {
                    ESP_LOGI(TAG, "Enable notify sent");
                }
            }
            free(descr_elem_result);
        }
        break;
    }
    case ESP_GATTC_NOTIFY_EVT:
        if (p_data->notify.is_notify) {
            ESP_LOGI(TAG, "Notification received, handle = %d, len = %d",
                     p_data->notify.handle, p_data->notify.value_len);
        } else {
            ESP_LOGI(TAG, "Indication received, handle = %d, len = %d",
                     p_data->notify.handle, p_data->notify.value_len);
        }
        if (p_data->notify.value != NULL && p_data->notify.value_len > 0) {
            ESP_LOG_BUFFER_HEX(TAG, p_data->notify.value, p_data->notify.value_len);
        }
        break;
    case ESP_GATTC_WRITE_DESCR_EVT:
        if (p_data->write.status != ESP_GATT_OK) {
            ESP_LOGE(TAG, "Descriptor write failed, status 0x%02x", p_data->write.status);
            break;
        }
        ESP_LOGI(TAG, "Descriptor write successfully, notify enabled");
        break;
    case ESP_GATTC_READ_CHAR_EVT:
        if (p_data->read.status != ESP_GATT_OK) {
            ESP_LOGE(TAG, "Characteristic read failed, status 0x%02x", p_data->read.status);
            break;
        }
        ESP_LOGI(TAG, "Characteristic read successfully, handle = %d, len = %d",
                 p_data->read.handle, p_data->read.value_len);
        if (p_data->read.value != NULL && p_data->read.value_len > 0) {
            ESP_LOG_BUFFER_HEX(TAG, p_data->read.value, p_data->read.value_len);
        }
        break;
    case ESP_GATTC_WRITE_CHAR_EVT:
        if (p_data->write.status != ESP_GATT_OK) {
            ESP_LOGE(TAG, "Characteristic write failed, status 0x%02x", p_data->write.status);
            break;
        }
        ESP_LOGI(TAG, "Characteristic write successfully");
        break;
    case ESP_GATTC_DISCONNECT_EVT:
        ESP_LOGI(TAG, "[Disconnected], conn_id = 0x%04x, reason = 0x%02x",
                 p_data->disconnect.conn_id, p_data->disconnect.reason);
        is_connected = false;
        conn_id = 0xFFFF;
        get_server = false;
        gl_profile_tab[PROFILE_A_APP_ID].service_start_handle = INVALID_HANDLE;
        gl_profile_tab[PROFILE_A_APP_ID].service_end_handle = INVALID_HANDLE;
        gl_profile_tab[PROFILE_A_APP_ID].char_handle = INVALID_HANDLE;
        break;
    default:
        break;
    }
}

/**
 * @brief Start periodic advertising with PAwR
 */
static void start_periodic_adv(void)
{
    // Create static random address
    esp_bd_addr_t rand_addr;
    esp_ble_gap_addr_create_static(rand_addr);

    ESP_LOG_BUFFER_HEX(TAG, rand_addr, ESP_BD_ADDR_LEN);

    // Set extended advertising parameters
    ESP_LOGI(TAG, "Set extended advertising params...");
    FUNC_SEND_WAIT_SEM(esp_ble_gap_ext_adv_set_params(EXT_ADV_HANDLE, &ext_adv_params), test_sem);
    FUNC_SEND_WAIT_SEM(esp_ble_gap_ext_adv_set_rand_addr(EXT_ADV_HANDLE, rand_addr), test_sem);

    // Set extended advertising data
    ESP_LOGI(TAG, "Set extended advertising data...");
    FUNC_SEND_WAIT_SEM(esp_ble_gap_config_ext_adv_data_raw(EXT_ADV_HANDLE,
                                                           sizeof(raw_ext_adv_data),
                                                           raw_ext_adv_data), test_sem);

    // Start extended advertising
    ESP_LOGI(TAG, "Start extended advertising...");
    FUNC_SEND_WAIT_SEM(esp_ble_gap_ext_adv_start(NUM_EXT_ADV, ext_adv), test_sem);

    // Set periodic advertising parameters
    ESP_LOGI(TAG, "Set periodic adv (PAwR) params...");
    FUNC_SEND_WAIT_SEM(esp_ble_gap_periodic_adv_set_params(EXT_ADV_HANDLE, &periodic_adv_params), test_sem);

    // Start periodic advertising (PAwR)
    ESP_LOGI(TAG, "Start periodic advertising (PAwR)...");
    FUNC_SEND_WAIT_SEM(esp_ble_gap_periodic_adv_start(EXT_ADV_HANDLE), test_sem);

    ESP_LOGI(TAG, "PAwR advertising started successfully");
}

void app_main(void)
{
    esp_err_t ret;

    // Initialize NVS
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(TAG, "%s initialize controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    esp_bluedroid_config_t cfg = BT_BLUEDROID_INIT_CONFIG_DEFAULT();
    ret = esp_bluedroid_init_with_cfg(&cfg);
    if (ret) {
        ESP_LOGE(TAG, "%s init bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(TAG, "%s enable bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret) {
        ESP_LOGE(TAG, "gap register error, error code = %x", ret);
        return;
    }

    ret = esp_ble_gattc_register_callback(gattc_profile_event_handler);
    if (ret) {
        ESP_LOGE(TAG, "gattc register error, error code = %x", ret);
        return;
    }

    ret = esp_ble_gattc_app_register(PROFILE_A_APP_ID);
    if (ret) {
        ESP_LOGE(TAG, "gattc app register error, error code = %x", ret);
        return;
    }

    // Create semaphore
    test_sem = xSemaphoreCreateBinary();

    // Start periodic advertising with PAwR
    start_periodic_adv();

    ESP_LOGI(TAG, "BLE PAwR Advertiser Connection example started");
}
