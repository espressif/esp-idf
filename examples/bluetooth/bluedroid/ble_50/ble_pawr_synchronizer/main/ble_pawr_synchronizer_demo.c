/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/****************************************************************************
*
* This demo showcases BLE Periodic Advertising with Responses (PAwR) on the synchronizer side.
* The device performs extended scanning and creates periodic sync to the advertiser.
* After sync is established, it selects a list of subevents to synchronize to via
* esp_ble_gap_set_periodic_sync_subevent().
* For received subevents, it sends responses via esp_ble_gap_set_periodic_adv_response_data().
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
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
#include "esp_gatt_common_api.h"

#include "sdkconfig.h"

#include "freertos/semphr.h"


#define FUNC_SEND_WAIT_SEM(func, sem) do {\
        esp_err_t __err_rc = (func);\
        if (__err_rc != ESP_OK) { \
            ESP_LOGE(LOG_TAG, "%s failed, error = %d", #func, __err_rc); \
            return; \
        } \
        xSemaphoreTake(sem, portMAX_DELAY); \
} while(0);

#define LOG_TAG "PAwR_SYNC"
#define EXT_SCAN_DURATION     0
#define EXT_SCAN_PERIOD       0

#define PROFILE_A_APP_ID 0
#define SVC_INST_ID           0

/* Service UUID and Characteristic UUID */
#define GATTS_DEMO_CHAR_VAL_LEN_MAX      500

/* UUID values (must be variables, not macros, to take address) */
static const uint16_t GATTS_SERVICE_UUID_PAWR_SYNC = 0x00FF;
static const uint16_t GATTS_CHAR_UUID_PAWR_SYNC = 0xFF01;

/* Attributes State Machine */
enum {
    IDX_SVC,
    IDX_CHAR_A,
    IDX_CHAR_VAL_A,
    IDX_CHAR_CFG_A,
    PAWR_SYNC_IDX_NB,
};

static char remote_device_name[ESP_BLE_ADV_NAME_LEN_MAX] = "ESP_EXTPAwR_ADV";
static SemaphoreHandle_t test_sem = NULL;
uint8_t rsp_slot_idx = 0;

/* Cached local address (read once during initialization) */
static esp_bd_addr_t cached_local_addr = {0};
static uint8_t cached_local_addr_type = 0;
static bool local_addr_cached = false;

// Forward declaration
static void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

// GATTS profile structure
static struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    esp_bd_addr_t remote_bda;
} gl_profile_tab[1] = {
    [PROFILE_A_APP_ID] = {
        .gatts_cb = gatts_profile_event_handler,  /* Set callback at initialization */
        .gatts_if = ESP_GATT_IF_NONE,            /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
};

/* Handle table for service attributes */
static uint16_t pawr_sync_handle_table[PAWR_SYNC_IDX_NB];

/* UUID definitions */
static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
static const uint8_t char_prop_read_write_notify = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
static const uint8_t char_value[4] = {0x00, 0x00, 0x00, 0x00};
static const uint8_t char_client_config_ccc[2] = {0x00, 0x00};

/* Full Database Description - Used to add attributes into the database */
static const esp_gatts_attr_db_t gatt_db[PAWR_SYNC_IDX_NB] =
{
    // Service Declaration
    [IDX_SVC]        =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
      sizeof(uint16_t), sizeof(GATTS_SERVICE_UUID_PAWR_SYNC), (uint8_t *)&GATTS_SERVICE_UUID_PAWR_SYNC}},

    /* Characteristic Declaration */
    [IDX_CHAR_A]     =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      sizeof(uint8_t), sizeof(uint8_t), (uint8_t *)&char_prop_read_write_notify}},

    /* Characteristic Value */
    [IDX_CHAR_VAL_A] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_PAWR_SYNC, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value), (uint8_t *)char_value}},

    /* Client Characteristic Configuration Descriptor */
    [IDX_CHAR_CFG_A]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      sizeof(uint16_t), sizeof(char_client_config_ccc), (uint8_t *)char_client_config_ccc}},
};

static esp_ble_ext_scan_params_t ext_scan_params = {
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_duplicate = BLE_SCAN_DUPLICATE_ENABLE,
    .cfg_mask = ESP_BLE_GAP_EXT_SCAN_CFG_UNCODE_MASK | ESP_BLE_GAP_EXT_SCAN_CFG_CODE_MASK,
    .uncoded_cfg = {BLE_SCAN_TYPE_ACTIVE, 40, 40},
    .coded_cfg = {BLE_SCAN_TYPE_ACTIVE, 40, 40},
};

static esp_ble_gap_periodic_adv_sync_params_t periodic_adv_sync_params = {
    .filter_policy = 0,
    .sid = 0,
    .addr_type = BLE_ADDR_TYPE_RANDOM,
    .skip = 10,
    .sync_timeout = 1000,
};

bool periodic_sync = false;


static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(LOG_TAG, "Extended scanning params set, status %d", param->set_ext_scan_params.status);
        break;
    case ESP_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(LOG_TAG, "Extended scanning start, status %d", param->ext_scan_start.status);

        // Cache local address after scan starts successfully (address is ready at this point)
        if (param->ext_scan_start.status == ESP_BT_STATUS_SUCCESS && !local_addr_cached) {
            esp_err_t addr_ret = esp_ble_gap_get_local_used_addr(cached_local_addr, &cached_local_addr_type);
            if (addr_ret == ESP_OK) {
                local_addr_cached = true;
                ESP_LOGI(LOG_TAG, "Local address cached: "ESP_BD_ADDR_STR", type = %d",
                         ESP_BD_ADDR_HEX(cached_local_addr), cached_local_addr_type);
            } else {
                ESP_LOGW(LOG_TAG, "Failed to get local address, error = 0x%x", addr_ret);
            }
        } else if (param->ext_scan_start.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(LOG_TAG, "Extended scanning start failed, status = 0x%02x", param->ext_scan_start.status);
        }
        break;
    case ESP_GAP_BLE_EXT_SCAN_STOP_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(LOG_TAG, "Extended scanning stop, status %d", param->ext_scan_stop.status);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_CREATE_SYNC_COMPLETE_EVT:
        ESP_LOGI(LOG_TAG, "Periodic advertising create sync, status %d", param->period_adv_create_sync.status);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_SYNC_CANCEL_COMPLETE_EVT:
        ESP_LOGI(LOG_TAG, "Periodic advertising sync cancel, status %d", param->period_adv_sync_cancel.status);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_SYNC_TERMINATE_COMPLETE_EVT:
        ESP_LOGI(LOG_TAG, "Periodic advertising sync terminate, status %d", param->period_adv_sync_term.status);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_SYNC_LOST_EVT:
        ESP_LOGI(LOG_TAG, "Periodic advertising sync lost, sync handle %d", param->periodic_adv_sync_lost.sync_handle);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_SYNC_ESTAB_EVT:
        ESP_LOGI(LOG_TAG, "Periodic advertising sync establish, status %d", param->periodic_adv_sync_estab.status);
        ESP_LOGI(LOG_TAG, "address "ESP_BD_ADDR_STR"", ESP_BD_ADDR_HEX(param->periodic_adv_sync_estab.adv_addr));
        ESP_LOGI(LOG_TAG, "sync handle %d sid %d perioic adv interval %d adv phy %d", param->periodic_adv_sync_estab.sync_handle,
                                                                                      param->periodic_adv_sync_estab.sid,
                                                                                      param->periodic_adv_sync_estab.period_adv_interval,
                                                                                      param->periodic_adv_sync_estab.adv_phy);
        ESP_LOGI(LOG_TAG, "num_subevt %d subevt_interval %d rsp_slot_delay %d rsp_slot_spacing %d", param->periodic_adv_sync_estab.num_subevt,
                                                                                                    param->periodic_adv_sync_estab.subevt_interval,
                                                                                                    param->periodic_adv_sync_estab.rsp_slot_delay,
                                                                                                    param->periodic_adv_sync_estab.rsp_slot_spacing);

        uint8_t subevents[5] = {0, 1, 2, 3, 4};
        esp_ble_per_sync_subevent_params sync_subevent_params = {0};
        sync_subevent_params.sync_handle = param->periodic_adv_sync_estab.sync_handle;
        sync_subevent_params.periodic_adv_properties = 0;
        sync_subevent_params.num_subevents_to_sync = 5;
        sync_subevent_params.subevent = malloc(sizeof(uint8_t) * 5);
        if (!sync_subevent_params.subevent) {
            ESP_LOGE(LOG_TAG, "No memory");
            break;
        }
        for (uint8_t i = 0; i < 5; i++)
        {
            sync_subevent_params.subevent[i] = subevents[i];
        }

        esp_err_t ret = esp_ble_gap_set_periodic_sync_subevent(&sync_subevent_params);
        if (ret != ESP_OK) {
            ESP_LOGE(LOG_TAG, "Failed to set periodic sync subevent, error = 0x%x", ret);
        }
        // Free memory after API call (data is deep copied by the API)
        free(sync_subevent_params.subevent);
        break;
    case ESP_GAP_BLE_EXT_ADV_REPORT_EVT: {
        uint8_t *adv_name = NULL;
        uint8_t adv_name_len = 0;
	    adv_name = esp_ble_resolve_adv_data_by_type(param->ext_adv_report.params.adv_data,
                                            param->ext_adv_report.params.adv_data_len,
                                            ESP_BLE_AD_TYPE_NAME_CMPL,
                                            &adv_name_len);
	    if ((adv_name != NULL) && (memcmp(adv_name, remote_device_name, adv_name_len) == 0) && !periodic_sync) {
            // Note: If there are multiple devices with the same device name, the device may sync to an unintended one.
            // It is recommended to change the default device name to ensure it is unique.
            periodic_sync = true;
	        char adv_temp_name[30] = {'0'};
	        size_t copy_len = (adv_name_len < sizeof(adv_temp_name)) ? adv_name_len : (sizeof(adv_temp_name) - 1);
	        memcpy(adv_temp_name, adv_name, copy_len);
	        adv_temp_name[copy_len] = '\0';  // Ensure null termination
	        ESP_LOGI(LOG_TAG, "Create sync with the peer device %s", adv_temp_name);
            periodic_adv_sync_params.sid = param->ext_adv_report.params.sid;
	        periodic_adv_sync_params.addr_type = param->ext_adv_report.params.addr_type;
	        memcpy(periodic_adv_sync_params.addr, param->ext_adv_report.params.addr, sizeof(esp_bd_addr_t));
            esp_ble_gap_periodic_adv_create_sync(&periodic_adv_sync_params);
	    }
    }
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_REPORT_EVT:
        // ESP_LOGI(LOG_TAG, "Periodic adv report, sync handle %d, data status %d, data len %d, rssi %d", param->period_adv_report.params.sync_handle,
        //                                                                                             param->period_adv_report.params.data_status,
        //                                                                                             param->period_adv_report.params.data_length,
        //                                                                                             param->period_adv_report.params.rssi);
        //ESP_LOGI(LOG_TAG, "periodic_evt_counter %d subevt %d",  param->period_adv_report.params.periodic_evt_counter, param->period_adv_report.params.subevt);
        if (param->period_adv_report.params.subevt == 0xFF) {
            break;
        }
        rsp_slot_idx = (uint8_t)((rsp_slot_idx + 1) % 5);
        esp_ble_per_adv_response_data_params rsp_data_params = {0};

        rsp_data_params.sync_handle = param->period_adv_report.params.sync_handle;
        rsp_data_params.request_event = param->period_adv_report.params.periodic_evt_counter;
        rsp_data_params.request_subevent = param->period_adv_report.params.subevt;
        rsp_data_params.response_subevent = param->period_adv_report.params.subevt;
        rsp_data_params.response_slot = rsp_slot_idx; // 0..4

        // Use cached local address (read once after scan starts)
        if (!local_addr_cached) {
            ESP_LOGW(LOG_TAG, "Local address not cached yet, skipping address in response");
            // Fall back to original behavior without address
            // Note: data_length is uint8_t (max 255), so no overflow check needed
            rsp_data_params.response_data_len = param->period_adv_report.params.data_length;
            if (rsp_data_params.response_data_len > 0) {
                rsp_data_params.response_data = malloc(rsp_data_params.response_data_len);
                if (rsp_data_params.response_data) {
                    memcpy(rsp_data_params.response_data, param->period_adv_report.params.data, rsp_data_params.response_data_len);
                    esp_err_t ret = esp_ble_gap_set_periodic_adv_response_data(&rsp_data_params);
                    if (ret != ESP_OK) {
                        ESP_LOGE(LOG_TAG, "Failed to set periodic adv response data, error = 0x%x", ret);
                    }
                    free(rsp_data_params.response_data);
                } else {
                    ESP_LOGE(LOG_TAG, "Failed to allocate memory for response data");
                }
            } else {
                rsp_data_params.response_data = NULL;
                esp_err_t ret = esp_ble_gap_set_periodic_adv_response_data(&rsp_data_params);
                if (ret != ESP_OK) {
                    ESP_LOGE(LOG_TAG, "Failed to set periodic adv response data, error = 0x%x", ret);
                }
            }
            break;
        }

        // Format: [0x00, 0x02, addr[5], addr[4], addr[3], addr[2], addr[1], addr[0], ...original_data]
        // Total length: 8 bytes (address header) + original data length
        const uint8_t ADDR_HEADER_LEN = 8;
        // Note: data_length is uint8_t (max 255), and response_data_len is also uint8_t (max 255)
        // Check if adding ADDR_HEADER_LEN would exceed uint8_t range
        if (param->period_adv_report.params.data_length > (UINT8_MAX - ADDR_HEADER_LEN)) {
            ESP_LOGE(LOG_TAG, "Data length too large, would exceed uint8_t range: %d", param->period_adv_report.params.data_length);
            break;
        }
        rsp_data_params.response_data_len = ADDR_HEADER_LEN + param->period_adv_report.params.data_length;
        rsp_data_params.response_data = malloc(rsp_data_params.response_data_len);
        if (rsp_data_params.response_data) {
            // Set address header: [0x00, 0x02, addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]]
            rsp_data_params.response_data[0] = 0x00;
            rsp_data_params.response_data[1] = 0x02;
            rsp_data_params.response_data[2] = cached_local_addr[5]; // MSB
            rsp_data_params.response_data[3] = cached_local_addr[4];
            rsp_data_params.response_data[4] = cached_local_addr[3];
            rsp_data_params.response_data[5] = cached_local_addr[2];
            rsp_data_params.response_data[6] = cached_local_addr[1];
            rsp_data_params.response_data[7] = cached_local_addr[0]; // LSB

            // Append original received data
            if (param->period_adv_report.params.data_length > 0) {
                memcpy(&rsp_data_params.response_data[ADDR_HEADER_LEN],
                       param->period_adv_report.params.data,
                       param->period_adv_report.params.data_length);
            }

            esp_err_t ret = esp_ble_gap_set_periodic_adv_response_data(&rsp_data_params);
            if (ret != ESP_OK) {
                ESP_LOGE(LOG_TAG, "Failed to set periodic adv response data, error = 0x%x", ret);
            }
            // Free memory after API call (data is deep copied by the API)
            free(rsp_data_params.response_data);
        } else {
            ESP_LOGE(LOG_TAG, "Failed to allocate memory for response data");
        }

        break;
    case ESP_GAP_BLE_SET_PERIODIC_ADV_SUBEVT_DATA_EVT:
        ESP_LOGI(LOG_TAG, "Set PA subevent data, status %d", param->pa_subevt_data_evt.status);
        break;
    case ESP_GAP_BLE_SET_PERIODIC_ADV_RESPONSE_DATA_EVT:
        if (param->pa_rsp_data_evt.status != ESP_BT_STATUS_SUCCESS)
        {
            ESP_LOGE(LOG_TAG, "Failed to set periodic adv response data, error = 0x%x", param->pa_rsp_data_evt.status);
        }
        // ESP_LOGI(LOG_TAG, "Set PA response data, status %d", param->pa_rsp_data_evt.status);
        break;
    case ESP_GAP_BLE_SET_PERIODIC_SYNC_SUBEVT_EVT:
        ESP_LOGI(LOG_TAG, "PA sync subevent, status %d", param->pa_sync_subevt_evt.status);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_SUBEVT_DATA_REQUEST_EVT:
        ESP_LOGI(LOG_TAG, "PA subevent data req, hdl %d start %d count %d", param->pa_subevt_data_req_evt.adv_handle, param->pa_subevt_data_req_evt.subevt_start, param->pa_subevt_data_req_evt.subevt_data_count);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_RESPONSE_REPORT_EVT:
        ESP_LOGI(LOG_TAG, "Set Periodic advertising response report, adv_handle %d", param->pa_rsp_rpt_evt.adv_handle);
        break;
    default:
        break;
    }
}

/**
 * @brief GATTS event handler
 */
static void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    if (param == NULL) {
        ESP_LOGE(LOG_TAG, "GATTS event handler received NULL param");
        return;
    }

    switch (event) {
    case ESP_GATTS_REG_EVT: {
        ESP_LOGI(LOG_TAG, "GATTS register app_id %04x, status %d, gatts_if %d",
                 param->reg.app_id, param->reg.status, gatts_if);
        if (param->reg.status == ESP_GATT_OK) {
            gl_profile_tab[PROFILE_A_APP_ID].gatts_if = gatts_if;

            // Create attribute table
            esp_err_t create_attr_ret = esp_ble_gatts_create_attr_tab(gatt_db, gatts_if, PAWR_SYNC_IDX_NB, SVC_INST_ID);
            if (create_attr_ret != ESP_OK) {
                ESP_LOGE(LOG_TAG, "create attr table failed, error code = %x", create_attr_ret);
            }
        } else {
            ESP_LOGE(LOG_TAG, "GATTS register failed, app_id %04x, status %d",
                     param->reg.app_id, param->reg.status);
        }
        break;
    }
    case ESP_GATTS_CREAT_ATTR_TAB_EVT: {
        if (param->add_attr_tab.status != ESP_GATT_OK) {
            ESP_LOGE(LOG_TAG, "create attribute table failed, error code=0x%x", param->add_attr_tab.status);
        } else if (param->add_attr_tab.num_handle != PAWR_SYNC_IDX_NB) {
            ESP_LOGE(LOG_TAG, "create attribute table abnormally, num_handle (%d) doesn't equal to PAWR_SYNC_IDX_NB(%d)",
                     param->add_attr_tab.num_handle, PAWR_SYNC_IDX_NB);
        } else {
            ESP_LOGI(LOG_TAG, "create attribute table successfully, the number handle = %d", param->add_attr_tab.num_handle);
            memcpy(pawr_sync_handle_table, param->add_attr_tab.handles, sizeof(pawr_sync_handle_table));
            esp_ble_gatts_start_service(pawr_sync_handle_table[IDX_SVC]);
            ESP_LOGI(LOG_TAG, "Service started, service_handle = %d", pawr_sync_handle_table[IDX_SVC]);
        }
        break;
    }
    case ESP_GATTS_READ_EVT:
        ESP_LOGI(LOG_TAG, "ESP_GATTS_READ_EVT, handle = %d", param->read.handle);
        break;
    case ESP_GATTS_WRITE_EVT:
        ESP_LOGI(LOG_TAG, "ESP_GATTS_WRITE_EVT, handle = %d, len = %d", param->write.handle, param->write.len);
        if (param->write.value != NULL && param->write.len > 0) {
            ESP_LOG_BUFFER_HEX(LOG_TAG, param->write.value, param->write.len);
        }

        // Handle CCCD write (notify enable/disable)
        // Check if handle table is initialized (service created successfully)
        if (pawr_sync_handle_table[IDX_CHAR_CFG_A] != 0 &&
            pawr_sync_handle_table[IDX_CHAR_CFG_A] == param->write.handle &&
            param->write.len == 2 && param->write.value != NULL) {
            uint16_t descr_value = param->write.value[1] << 8 | param->write.value[0];
            if (descr_value == 0x0001) {
                ESP_LOGI(LOG_TAG, "notify enable");
            } else if (descr_value == 0x0000) {
                ESP_LOGI(LOG_TAG, "notify disable");
            }
        }

        if (param->write.need_rsp) {
            esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
        }
        break;
    case ESP_GATTS_START_EVT:
        ESP_LOGI(LOG_TAG, "SERVICE_START_EVT, status %d, service_handle %d", param->start.status, param->start.service_handle);
        break;
    case ESP_GATTS_CONNECT_EVT: {
        ESP_LOGI(LOG_TAG, "[Connection established], conn_id = 0x%04x", param->connect.conn_id);
        gl_profile_tab[PROFILE_A_APP_ID].conn_id = param->connect.conn_id;
        memcpy(gl_profile_tab[PROFILE_A_APP_ID].remote_bda, param->connect.remote_bda, ESP_BD_ADDR_LEN);
        ESP_LOGI(LOG_TAG, "Remote Address: "ESP_BD_ADDR_STR"", ESP_BD_ADDR_HEX(param->connect.remote_bda));
        break;
    }
    case ESP_GATTS_DISCONNECT_EVT:
        ESP_LOGI(LOG_TAG, "[Disconnected], conn_id = 0x%04x, reason = 0x%02x",
                 param->disconnect.conn_id, param->disconnect.reason);
        gl_profile_tab[PROFILE_A_APP_ID].conn_id = 0xFFFF;
        break;
    case ESP_GATTS_MTU_EVT:
        ESP_LOGI(LOG_TAG, "MTU update event, conn_id = 0x%04x, mtu = %d",
                 param->mtu.conn_id, param->mtu.mtu);
        break;
    default:
        break;
    }
}

/**
 * @brief GATTS callback function
 */
static void esp_gatts_cb(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    if (param == NULL) {
        ESP_LOGE(LOG_TAG, "GATTS callback received NULL param");
        return;
    }

    /* If event is register event, store the gatts_if for each profile */
    if (event == ESP_GATTS_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            gl_profile_tab[param->reg.app_id].gatts_if = gatts_if;
        } else {
            ESP_LOGE(LOG_TAG, "reg app failed, app_id %04x, status %d",
                     param->reg.app_id, param->reg.status);
            return;
        }
    }

    /* If the gatts_if equal to profile A, call profile A cb handler */
    do {
        int idx;
        for (idx = 0; idx < 1; idx++) {
            if (gatts_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
                    gatts_if == gl_profile_tab[idx].gatts_if) {
                if (gl_profile_tab[idx].gatts_cb) {
                    gl_profile_tab[idx].gatts_cb(event, gatts_if, param);
                }
            }
        }
    } while (0);
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

    #if CONFIG_EXAMPLE_CI_PIPELINE_ID
    memcpy(remote_device_name, esp_bluedroid_get_example_name(), sizeof(remote_device_name));
    #endif

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(LOG_TAG, "%s initialize controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(LOG_TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    esp_bluedroid_config_t cfg = BT_BLUEDROID_INIT_CONFIG_DEFAULT();
    ret = esp_bluedroid_init_with_cfg(&cfg);
    if (ret) {
        ESP_LOGE(LOG_TAG, "%s init bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(LOG_TAG, "%s enable bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret){
        ESP_LOGE(LOG_TAG, "gap register error, error code = %x", ret);
        return;
    }

    // Register GATTS callback
    ret = esp_ble_gatts_register_callback(esp_gatts_cb);
    if (ret){
        ESP_LOGE(LOG_TAG, "gatts register error, error code = %x", ret);
        return;
    }

    // Register GATTS app (this will trigger ESP_GATTS_REG_EVT event immediately)
    ret = esp_ble_gatts_app_register(PROFILE_A_APP_ID);
    if (ret){
        ESP_LOGE(LOG_TAG, "gatts app register error, error code = %x", ret);
        return;
    }

    vTaskDelay(200 / portTICK_PERIOD_MS);

    test_sem = xSemaphoreCreateBinary();

    FUNC_SEND_WAIT_SEM(esp_ble_gap_set_ext_scan_params(&ext_scan_params), test_sem);
    FUNC_SEND_WAIT_SEM(esp_ble_gap_start_ext_scan(EXT_SCAN_DURATION, EXT_SCAN_PERIOD), test_sem);


    return;
}
