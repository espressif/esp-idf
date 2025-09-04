/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
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
// #include "bt_target.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "ble_multiconn_prph_demo.h"
#include "esp_random.h"

#define DEMO_TAG "MULTI_CONN_PRPH"

#ifdef CONFIG_BT_BLE_50_FEATURES_SUPPORTED
#define BLE50_SUPPORTED (CONFIG_BT_BLE_50_FEATURES_SUPPORTED)
#else
#define BLE50_SUPPORTED (0)
#endif

/* Service */
#define SVC_INST_ID 0
#define MULTICONN_PROFILE_NUM 1
#define MULTICONN_PROFILE_APP_IDX 0
#define EXT_ADV_HANDLE 0
#define NUM_EXT_ADV_SET 1
#define EXT_ADV_DURATION 0
#define EXT_ADV_MAX_EVENTS 0
#define BLE_MAX_CONN (CONFIG_BT_ACL_CONNECTIONS)
#define GATTS_DEMO_CHAR_VAL_LEN_MAX 0x40
#define CHAR_DECLARATION_SIZE (sizeof(uint8_t))

static uint8_t char_value[GATTS_DEMO_CHAR_VAL_LEN_MAX] = {0x00};
static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static const uint8_t char_prop_write = ESP_GATT_CHAR_PROP_BIT_WRITE;
static const uint8_t gatt_svr_svc_uuid[] = {0x2d, 0x71, 0xa2, 0x59, 0xb4, 0x58, 0xc8, 0x12,
                                            0x99, 0x99, 0x43, 0x95, 0x12, 0x2f, 0x46, 0x59};

static const uint8_t gatt_svr_chr_uuid[] = {0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11,
                                            0x22, 0x22, 0x22, 0x22, 0x33, 0x33, 0x33, 0x33};
static uint16_t profile_handle_table[HRS_IDX_NB];

static uint8_t adv_data_raw[] = {
    0x02, ESP_BLE_AD_TYPE_FLAG, 0x06,
    0x03, ESP_BLE_AD_TYPE_16SRV_CMPL, 0xab, 0xcd,
    0x03, ESP_BLE_AD_TYPE_16SRV_CMPL, 0x18, 0x11,
    0x0f, ESP_BLE_AD_TYPE_NAME_CMPL, 'e', 's', 'p', '-','m', 'u', 'l', 't', 'i', '-', 'c', 'o', 'n', 'n',
};

static bool is_advertising = false;

#if (BLE50_SUPPORTED)
static esp_ble_gap_ext_adv_t ext_adv[1] = {
    [0] = {EXT_ADV_HANDLE, EXT_ADV_DURATION, EXT_ADV_MAX_EVENTS},
};

esp_ble_gap_ext_adv_params_t ext_adv_params = {
    .type = ESP_BLE_GAP_SET_EXT_ADV_PROP_CONNECTABLE,
    .interval_min = 0x20,
    .interval_max = 0x20,
    .channel_map = ADV_CHNL_ALL,
    .filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
    .primary_phy = ESP_BLE_GAP_PHY_1M,
    .max_skip = 0,
    .secondary_phy = ESP_BLE_GAP_PHY_1M,
    .sid = 0,
    .scan_req_notif = false,
    .own_addr_type = BLE_ADDR_TYPE_RANDOM,
    .tx_power = EXT_ADV_TX_PWR_NO_PREFERENCE,
};
#else
static esp_ble_adv_params_t legacy_adv_params = {
    .adv_int_min = 0x20,
    .adv_int_max = 0x40,
    .adv_type = ADV_TYPE_IND,
    .own_addr_type = BLE_ADDR_TYPE_RANDOM,
    .channel_map = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,

};
#endif

struct gatts_profile_inst
{
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
};

static void gatts_profile_event_handler(esp_gatts_cb_event_t event,
                                        esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

/* One gatt-based profile one app_id and one gatts_if, this array will store the gatts_if returned by ESP_GATTS_REG_EVT */
static struct gatts_profile_inst multi_conn_profile_tab[MULTICONN_PROFILE_NUM] = {
    [MULTICONN_PROFILE_APP_IDX] = {
        .gatts_cb = gatts_profile_event_handler,
        .gatts_if = ESP_GATT_IF_NONE, /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
};

static uint8_t prph_conn_num = 0;
static SemaphoreHandle_t restart_adv_sem = NULL;
static esp_bd_addr_t new_rand_addr;

static void ble_prph_set_new_adv(void)
{
    if (prph_conn_num >= BLE_MAX_CONN) {
        ESP_LOGI(DEMO_TAG, "Max connection reached");
#if (BLE50_SUPPORTED == 1)
        esp_ble_gap_ext_adv_stop(1, &ext_adv[0].instance);
#else
        esp_ble_gap_stop_advertising();
#endif
        return;
    }

    if (!is_advertising) {
        is_advertising = true;
        esp_ble_gap_addr_create_static(new_rand_addr);
#if (BLE50_SUPPORTED == 1)
        esp_ble_gap_ext_adv_set_rand_addr(EXT_ADV_HANDLE, new_rand_addr);
#else
        esp_ble_gap_set_rand_addr(new_rand_addr);
#endif
    }
}

static void ble_prph_restart_adv(void)
{
#if CONFIG_EXAMPLE_RESTART_ADV_AFTER_CONNECTED
    if (!xSemaphoreGive(restart_adv_sem)) {
        ESP_LOGE(DEMO_TAG, "Failed to give semaphore");
    }
#else
    ble_prph_set_new_adv();
#endif // CONFIG_EXAMPLE_RESTART_ADV_AFTER_CONNECTED
}

/* Full Database Description - Used to add attributes into the database */
static const esp_gatts_attr_db_t gatt_db[HRS_IDX_NB] =
{
    // Service Declaration
    [IDX_SVC] =
        {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ, sizeof(uint16_t), sizeof(gatt_svr_svc_uuid), (uint8_t *)&gatt_svr_svc_uuid}},
    // Characteristic Declaration
    [IDX_CHAR_A] =
        {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ, CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_write}},
    // Characteristic Value
    [IDX_CHAR_VAL_A] =
        {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_128, (uint8_t *)gatt_svr_chr_uuid, ESP_GATT_PERM_WRITE, GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value), (uint8_t *)&char_value}},
};

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
#if (BLE50_SUPPORTED)
    case ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT:
        ESP_LOGI(DEMO_TAG, "Extended adv random address set, status %d, "ESP_BD_ADDR_STR"",
            param->ext_adv_set_rand_addr.status, ESP_BD_ADDR_HEX(new_rand_addr));
        esp_ble_gap_ext_adv_start(NUM_EXT_ADV_SET, &ext_adv[0]);
        break;
    case ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT:
        ESP_LOGI(DEMO_TAG, "Extended advertising params set, status %d", param->ext_adv_set_params.status);
        esp_ble_gap_config_ext_adv_data_raw(EXT_ADV_HANDLE, sizeof(adv_data_raw), &adv_data_raw[0]);
        break;
    case ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT:
        ESP_LOGI(DEMO_TAG, "Extended advertising data set, status %d", param->ext_adv_data_set.status);
        ble_prph_restart_adv();
        break;
    case ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT:
        ESP_LOGI(DEMO_TAG, "Extended advertising start, status %d", param->ext_adv_start.status);
        is_advertising = true;
        break;
    case ESP_GAP_BLE_ADV_TERMINATED_EVT:
        ESP_LOGI(DEMO_TAG, "Extended advertising terminated, status = %d", param->adv_terminate.status);
        if (param->adv_terminate.status == 0x00) {
            ESP_LOGI(DEMO_TAG, "Advertising successfully ended with a connection being created");
            is_advertising = false;
        }
        break;
#else
    case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
        ESP_LOGI(DEMO_TAG, "Advertising data set, status %d", param->adv_data_raw_cmpl.status);
        esp_ble_gap_addr_create_static(new_rand_addr);
        esp_ble_gap_set_rand_addr(new_rand_addr);
        break;
    case ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT:
        ESP_LOGI(DEMO_TAG, "Random address set, status %d, addr "ESP_BD_ADDR_STR"",
            param->set_rand_addr_cmpl.status, ESP_BD_ADDR_HEX(new_rand_addr));
        esp_ble_gap_start_advertising(&legacy_adv_params);
        break;
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        ESP_LOGI(DEMO_TAG, "Advertising start, status %d", param->adv_start_cmpl.status);
        is_advertising = true;
        break;
    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        ESP_LOGI(DEMO_TAG, "Advertising stop, status %d", param->adv_stop_cmpl.status);
        is_advertising = false;
        break;
#endif
    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
        ESP_LOGI(DEMO_TAG, "Connection params update, status %d, conn_int %d, latency %d, timeout %d",
                 param->update_conn_params.status,
                 param->update_conn_params.conn_int,
                 param->update_conn_params.latency,
                 param->update_conn_params.timeout);
        break;
    default:
        break;
    }
}

static void gatts_profile_event_handler(esp_gatts_cb_event_t event,
                                        esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    switch (event)
    {
    case ESP_GATTS_REG_EVT:
        ESP_LOGI(DEMO_TAG, "GATT server register, status %d, app_id %d, gatts_if %d", param->reg.status, param->reg.app_id, gatts_if);
        esp_ble_gatts_create_attr_tab(gatt_db, gatts_if, HRS_IDX_NB, SVC_INST_ID);
        break;
    case ESP_GATTS_READ_EVT:
        ESP_LOGI(DEMO_TAG, "Characteristic read, conn_id %u", param->read.conn_id);
        break;
    case ESP_GATTS_WRITE_EVT:
        ESP_LOGI(DEMO_TAG, "Characteristic write, conn_id %u, value", param->write.conn_id);
        ESP_LOG_BUFFER_HEX(DEMO_TAG, param->write.value, param->write.len);
        break;
    case ESP_GATTS_CONNECT_EVT:
        ESP_LOGI(DEMO_TAG, "Connected, conn_id %u, remote "ESP_BD_ADDR_STR", total %u",
                 param->connect.conn_id, ESP_BD_ADDR_HEX(param->connect.remote_bda), ++prph_conn_num);
        is_advertising = false;
#if CONFIG_EXAMPLE_RESTART_ADV_AFTER_CONNECTED
        ble_prph_restart_adv();
#endif
        break;
    case ESP_GATTS_DISCONNECT_EVT:
        ESP_LOGI(DEMO_TAG, "Disconnected, remote "ESP_BD_ADDR_STR", reason 0x%x, total %d",
            ESP_BD_ADDR_HEX(param->disconnect.remote_bda), param->disconnect.reason, (prph_conn_num ? --prph_conn_num : prph_conn_num));
        /* start advertising again when disconnected */
        ble_prph_restart_adv();
        break;
    case ESP_GATTS_OPEN_EVT:
        break;
    case ESP_GATTS_CANCEL_OPEN_EVT:
        break;
    case ESP_GATTS_CLOSE_EVT:
        break;
    case ESP_GATTS_LISTEN_EVT:
        break;
    case ESP_GATTS_CONGEST_EVT:
        break;
    case ESP_GATTS_CREAT_ATTR_TAB_EVT:
    {
        ESP_LOGI(DEMO_TAG, "The number handle = %x", param->add_attr_tab.num_handle);
        if (param->create.status == ESP_GATT_OK) {
            if (param->add_attr_tab.num_handle == HRS_IDX_NB) {
                memcpy(profile_handle_table, param->add_attr_tab.handles,
                       sizeof(profile_handle_table));
                esp_ble_gatts_start_service(profile_handle_table[IDX_SVC]);
#if (BLE50_SUPPORTED == 1)
                esp_ble_gap_ext_adv_set_params(EXT_ADV_HANDLE, &ext_adv_params);
#else
                esp_ble_gap_config_adv_data_raw(adv_data_raw, sizeof(adv_data_raw));
#endif
            } else {
                ESP_LOGE(DEMO_TAG, "Create attribute table abnormally, num_handle (%d) doesn't equal to HRS_IDX_NB(%d)",
                         param->add_attr_tab.num_handle, HRS_IDX_NB);
            }
        } else {
            ESP_LOGE(DEMO_TAG, " Create attribute table failed, error code = %x", param->create.status);
        }
        break;
    }

    default:
        break;
    }
}

static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
                                esp_ble_gatts_cb_param_t *param)
{
    /* If event is register event, store the gatts_if for each profile */
    if (event == ESP_GATTS_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            multi_conn_profile_tab[MULTICONN_PROFILE_APP_IDX].gatts_if = gatts_if;
        } else {
            ESP_LOGI(DEMO_TAG, "Reg app failed, app_id %04x, status %d",
                     param->reg.app_id,
                     param->reg.status);
            return;
        }
    }

    do {
        int idx;
        for (idx = 0; idx < MULTICONN_PROFILE_NUM; idx++) {
            if (gatts_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
                gatts_if == multi_conn_profile_tab[idx].gatts_if) {
                if (multi_conn_profile_tab[idx].gatts_cb) {
                    multi_conn_profile_tab[idx].gatts_cb(event, gatts_if, param);
                }
            }
        }
    } while (0);
}

void app_main(void)
{
    esp_err_t ret;
    restart_adv_sem = xSemaphoreCreateBinary();

    // Initialize NVS.
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
        ESP_LOGE(DEMO_TAG, "%s init controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(DEMO_TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(DEMO_TAG, "%s init bluetooth", __func__);

    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(DEMO_TAG, "%s init bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(DEMO_TAG, "%s enable bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_ble_gatts_register_callback(gatts_event_handler);
    if (ret) {
        ESP_LOGE(DEMO_TAG, "gatts register error, error code = %x", ret);
        return;
    }

    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret) {
        ESP_LOGE(DEMO_TAG, "gap register error, error code = %x", ret);
        return;
    }

    ret = esp_ble_gatts_app_register(MULTICONN_PROFILE_APP_IDX);
    if (ret) {
        ESP_LOGE(DEMO_TAG, "gatts app register error, error code = %x", ret);
        return;
    }

#if CONFIG_EXAMPLE_RESTART_ADV_AFTER_CONNECTED
    while (true) {
        int delay_ms = 0;
        if (xSemaphoreTake(restart_adv_sem, portMAX_DELAY)) {
            delay_ms = (esp_random() % 300) + 100;
            vTaskDelay(pdMS_TO_TICKS(delay_ms));
            ble_prph_set_new_adv();
        } else {
            ESP_LOGE(DEMO_TAG, "Failed to take restart_adv_sem");
        }
    }
#endif // CONFIG_EXAMPLE_RESTART_ADV_AFTER_CONNECTED

}
