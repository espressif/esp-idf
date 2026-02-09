/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "ble_multiconn_cent_demo.h"
#include "esp_log.h"

#define DEMO_TAG "MULTI_CONN_CENT"

#ifdef CONFIG_BT_BLE_50_FEATURES_SUPPORTED
#define BLE50_SUPPORTED (CONFIG_BT_BLE_50_FEATURES_SUPPORTED)
#else
#define BLE50_SUPPORTED (0)
#endif

#define EXT_SCAN_DURATION 0
#define EXT_SCAN_PERIOD 0
static const char remote_target_name[] = "esp-multi-conn";

#define GATTC_PROFILE_NUM 1
#define GATTC_PROFILE_APP_ID 0
#define GATTS_PROFILE_NUM 1
#define GATTS_PROFILE_APP_ID 0

#define BLE_PEER_MAX_NUM MAX_CONN_NUM - 1
#define BLE_PREF_EVT_LEN_MS (5)
#define BLE_PREF_CONN_ITVL_MS ((BLE_PEER_MAX_NUM) * BLE_PREF_EVT_LEN_MS)
#define BLE_PREF_CE_LEN (BLE_PREF_EVT_LEN_MS * 1000 / 625)

static uint8_t multi_conn_num = 0;
static uint16_t prph_conn_id = 0xFFFF;  // ID of connection with peripheral role
static int advertising_state = DISABLED;
static int scan_state = DISABLED;
static SemaphoreHandle_t restart_scan_sem = NULL;

#define MULTICONN_PROFILE_NUM 1
#define MULTICONN_PROFILE_APP_ID 0
#define ADV_HANDLE_INST 0
#define ADV_HANDLE_NUM 1
#define ADV_DURATION 0
#define ADV_MAX_EVTS 0
#define GATTS_DEMO_CHAR_VAL_LEN_MAX 0x40
#define SVC_INST_ID 0
#define CHAR_DECLARATION_SIZE (sizeof(uint8_t))

static uint8_t char_value[GATTS_DEMO_CHAR_VAL_LEN_MAX] = {0};
static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static const uint8_t char_prop_write = ESP_GATT_CHAR_PROP_BIT_WRITE;
static const uint8_t remote_svc_uuid[] = {0x2d, 0x71, 0xa2, 0x59, 0xb4, 0x58, 0xc8, 0x12, 0x99, 0x99, 0x43, 0x95, 0x12, 0x2f, 0x46, 0x59};
static const uint8_t gatt_svr_chr_uuid[] = {0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x22, 0x22, 0x22, 0x22, 0x33, 0x33, 0x33, 0x33};

static uint8_t adv_data_raw[] = {
    0x02, ESP_BLE_AD_TYPE_FLAG, 0x06,
    0x12, ESP_BLE_AD_TYPE_NAME_CMPL, 'e', 's', 'p', '-', 'b', 'l', 'e', '-', 'r', 'o', 'l', 'e', '-', 'c', 'o', 'e', 'x',
};

static esp_gattc_char_elem_t char_elem_result;
static esp_bd_addr_t new_rand_addr;
static esp_bd_addr_t adv_rand_addr;

static esp_bt_uuid_t remote_svc_uuid_t = {
    .len = ESP_UUID_LEN_128,
    .uuid = {
        .uuid128 = {0x2d, 0x71, 0xa2, 0x59, 0xb4, 0x58, 0xc8, 0x12, 0x99, 0x99, 0x43, 0x95, 0x12, 0x2f, 0x46, 0x59},
    },
};

static esp_bt_uuid_t char_uuid_t = {
    .len = ESP_UUID_LEN_128,
    .uuid = {
        .uuid128 = {0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x22, 0x22, 0x22, 0x22, 0x33, 0x33, 0x33, 0x33},
    },
};

#if (BLE50_SUPPORTED == 0)
static esp_ble_scan_params_t legacy_scan_params = {
    .own_addr_type = BLE_ADDR_TYPE_RANDOM,
    .scan_filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_type = BLE_SCAN_TYPE_PASSIVE,
    .scan_duplicate = BLE_SCAN_DUPLICATE_ENABLE,
    .scan_interval = 800,
    .scan_window = 320,

};
#else
static esp_ble_ext_scan_params_t ext_scan_params = {
    .own_addr_type = BLE_ADDR_TYPE_RANDOM,
    .filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_duplicate = BLE_SCAN_DUPLICATE_ENABLE,
    .cfg_mask = ESP_BLE_GAP_EXT_SCAN_CFG_UNCODE_MASK,
    .uncoded_cfg = {BLE_SCAN_TYPE_PASSIVE, 800, 320},
};
#endif

const static esp_ble_conn_params_t phy_1m_conn_params = {
    .scan_interval = 480,
    .scan_window = 160,
    .interval_min = BLE_PREF_CONN_ITVL_MS * 1000 / 1250,
    .interval_max = BLE_PREF_CONN_ITVL_MS * 1000 / 1250,
    .latency = 0,
    .supervision_timeout = ESP_BLE_GAP_SUPERVISION_TIMEOUT_MS(6000),
    .min_ce_len = BLE_PREF_CE_LEN,
    .max_ce_len = BLE_PREF_CE_LEN,
};

#if (BLE50_SUPPORTED == 1)
const static esp_ble_conn_params_t phy_2m_conn_params = {
    .scan_interval = 480,
    .scan_window = 160,
    .interval_min = BLE_PREF_CONN_ITVL_MS * 1000 / 1250,
    .interval_max = BLE_PREF_CONN_ITVL_MS * 1000 / 1250,
    .latency = 0,
    .supervision_timeout = ESP_BLE_GAP_SUPERVISION_TIMEOUT_MS(6000),
    .min_ce_len = BLE_PREF_CE_LEN,
    .max_ce_len = BLE_PREF_CE_LEN,
};

const static esp_ble_conn_params_t phy_coded_conn_params = {
    .scan_interval = 480,
    .scan_window = 160,
    .interval_min = BLE_PREF_CONN_ITVL_MS * 1000 / 1250,
    .interval_max = BLE_PREF_CONN_ITVL_MS * 1000 / 1250,
    .latency = 0,
    .supervision_timeout = ESP_BLE_GAP_SUPERVISION_TIMEOUT_MS(6000),
    .min_ce_len = BLE_PREF_CE_LEN,
    .max_ce_len = BLE_PREF_CE_LEN,
};
#endif

struct gattc_profile_inst
{
    esp_gattc_cb_t gattc_cb;
    uint16_t gattc_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_start_handle;
    uint16_t service_end_handle;
    uint16_t notify_char_handle;
    esp_bd_addr_t remote_bda;
};

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

#if (BLE50_SUPPORTED == 0)
esp_ble_adv_params_t legacy_adv_params = {
    .adv_int_min = ESP_BLE_GAP_ADV_ITVL_MS(20),
    .adv_int_max = ESP_BLE_GAP_ADV_ITVL_MS(20),
    .adv_type = ADV_TYPE_IND,
    .own_addr_type = BLE_ADDR_TYPE_RANDOM,
    .channel_map = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,

};
#else
esp_ble_gap_ext_adv_params_t ext_adv_params = {
    .type = ESP_BLE_GAP_SET_EXT_ADV_PROP_CONNECTABLE,
    .interval_min = ESP_BLE_GAP_ADV_ITVL_MS(20),
    .interval_max = ESP_BLE_GAP_ADV_ITVL_MS(20),
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
#endif

static void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);
static void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
static void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);
static void esp_gatts_cb(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

static struct gattc_profile_inst gattc_profile_tab[GATTC_PROFILE_NUM] = {
    [GATTC_PROFILE_APP_ID] = {
        .gattc_cb = gattc_profile_event_handler,
        .gattc_if = ESP_GATT_IF_NONE,
    },
};

static struct gatts_profile_inst gatts_profile_tab[GATTS_PROFILE_NUM] = {
    [GATTS_PROFILE_APP_ID] = {
        .gatts_cb = gatts_profile_event_handler,
        .gatts_if = ESP_GATT_IF_NONE,
    }};

/* Full Database Description - Used to add attributes into the database */
static const esp_gatts_attr_db_t gatt_db[HRS_IDX_NB] =
{
    // Service Declaration
    [IDX_SVC] =
        {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ, sizeof(uint16_t), sizeof(remote_svc_uuid), (uint8_t *)&remote_svc_uuid}},

    // Characteristic Declaration
    [IDX_CHAR_A] =
        {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ, CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_write}},
    // Characteristic Value
    [IDX_CHAR_VAL_A] =
        {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_128, (uint8_t *)gatt_svr_chr_uuid, ESP_GATT_PERM_WRITE, GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value), (uint8_t *)&char_value}},
};
static uint16_t profile_handle_table[HRS_IDX_NB];

#if (BLE50_SUPPORTED == 1)
static esp_ble_gap_ext_adv_t ext_adv[1] = {
    [0] = {ADV_HANDLE_INST, ADV_DURATION, ADV_MAX_EVTS},
};
#endif

static void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    switch (event) {
    case ESP_GATTC_REG_EVT:
        ESP_LOGI(DEMO_TAG, "GATT client register, status %d, app_id %d, gattc_if %d", param->reg.status, param->reg.app_id, gattc_if);
#if (BLE50_SUPPORTED == 1)
        esp_ble_gap_set_ext_scan_params(&ext_scan_params);
#else
        esp_ble_gap_set_scan_params(&legacy_scan_params);
#endif
        break;
    case ESP_GATTC_CONNECT_EVT:
        if (param->connect.link_role == 0) {
            ESP_LOGI(DEMO_TAG, "Connected, conn_id %d, remote "ESP_BD_ADDR_STR", total %u", param->connect.conn_id,
                     ESP_BD_ADDR_HEX(param->connect.remote_bda), ++multi_conn_num);
            Peer new_peer;
            new_peer.conn_id = param->connect.conn_id;
            new_peer.conn_handle = param->connect.conn_handle;
            new_peer.gattc_if = gattc_profile_tab->gattc_if;
            new_peer.char_handle = 0xFFFF;
            memcpy(new_peer.peer_addr, param->connect.remote_bda, sizeof(esp_bd_addr_t));
            peer_add(&new_peer);
            if (multi_conn_num < BLE_PEER_MAX_NUM && scan_state == DISABLED) {
                scan_state = PENDING;
                esp_ble_gap_addr_create_static(new_rand_addr);
                esp_ble_gap_set_rand_addr((uint8_t *)new_rand_addr);
            } else {
                ESP_LOGI(DEMO_TAG, "BLE peer max number reached");
#if (BLE50_SUPPORTED == 0)
                advertising_state = PENDING;
                esp_ble_gap_addr_create_static(adv_rand_addr);
                esp_ble_gap_set_rand_addr((uint8_t *)adv_rand_addr);
#endif
            }
        }
        break;
    case ESP_GATTC_DISCONNECT_EVT:
        ESP_LOGI(DEMO_TAG, "Disconnected, remote "ESP_BD_ADDR_STR", reason 0x%02x, total %u",
            ESP_BD_ADDR_HEX(param->disconnect.remote_bda), param->disconnect.reason,
            (multi_conn_num ? --multi_conn_num : multi_conn_num));
        if (param->disconnect.conn_id != prph_conn_id) {
            Peer *peer = find_peer(param->disconnect.conn_id);
            if (peer) {
                peer_remove(peer->conn_id);
            }
            xSemaphoreGive(restart_scan_sem);
        } else {
            prph_conn_id = 0xFFFF;
        }
        break;
    case ESP_GATTC_OPEN_EVT:
        ESP_LOGI(DEMO_TAG, "Open, conn_id %d, status %d", param->open.conn_id, param->open.status);
        break;
    case ESP_GATTC_CLOSE_EVT:
        break;
    case ESP_GATTC_DIS_SRVC_CMPL_EVT:
        ESP_LOGI(DEMO_TAG, "Service discover complete, conn_id %d", param->dis_srvc_cmpl.conn_id);
        esp_ble_gattc_search_service(gattc_if, param->dis_srvc_cmpl.conn_id, &remote_svc_uuid_t);
        break;
    case ESP_GATTC_SEARCH_RES_EVT:
        ESP_LOGI(DEMO_TAG, "Service search result, conn_id %d", param->search_res.conn_id);
        if (param->search_res.srvc_id.uuid.len == ESP_UUID_LEN_128 && !memcmp(param->search_res.srvc_id.uuid.uuid.uuid128, remote_svc_uuid_t.uuid.uuid128, ESP_UUID_LEN_128)) {
            uint16_t count = 1;
            esp_gatt_status_t ret_status = esp_ble_gattc_get_char_by_uuid(gattc_if,
                                                                          param->search_res.conn_id,
                                                                          param->search_res.start_handle,
                                                                          param->search_res.end_handle,
                                                                          char_uuid_t,
                                                                          &char_elem_result,
                                                                          &count);
            if (ret_status != ESP_GATT_OK) {
                ESP_LOGE(DEMO_TAG, "esp_ble_gattc_get_char_by_uuid error, conn_id = %d, status = %d", param->search_res.conn_id, ret_status);
                break;
            }
            Peer *target_peer = find_peer(param->search_res.conn_id);
            if (target_peer) {
                target_peer->char_handle = char_elem_result.char_handle;
            } else {
                ESP_LOGE(DEMO_TAG, "can't find peer");
                break;
            }
        }
        break;
    case ESP_GATTC_SEARCH_CMPL_EVT:
        ESP_LOGI(DEMO_TAG, "Service search complete, status %d, conn_id %d", param->search_cmpl.status, param->search_cmpl.conn_id);
        break;
    case ESP_GATTC_WRITE_CHAR_EVT:
        ESP_LOGI(DEMO_TAG, "Characteristic write, conn_id %u, status %d", param->write.conn_id, param->write.status);
        break;
    case ESP_GATTC_NOTIFY_EVT:
        if (param->notify.is_notify){
            ESP_LOGI(DEMO_TAG, "Notification received, conn_id %u", param->notify.conn_id);
        }else{
            ESP_LOGI(DEMO_TAG, "Indication received, conn_id %u", param->notify.conn_id);
        }
        ESP_LOG_BUFFER_HEX(DEMO_TAG, param->notify.value, param->notify.value_len);
        break;
    default:
        break;
    }
}

static void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    switch (event) {
    case ESP_GATTS_REG_EVT:
        ESP_LOGI(DEMO_TAG, "GATT server register, status %d, app_id %d, gatts_if %d", param->reg.status, param->reg.app_id, gatts_if);
#if (BLE50_SUPPORTED == 1)
        esp_ble_gap_ext_adv_set_params(ADV_HANDLE_INST, &ext_adv_params);
#else
        esp_ble_gap_config_adv_data_raw(adv_data_raw, sizeof(adv_data_raw));
#endif
        esp_ble_gatts_create_attr_tab(gatt_db, gatts_if, HRS_IDX_NB, SVC_INST_ID);
        break;
    case ESP_GATTS_WRITE_EVT:
        ESP_LOGI(DEMO_TAG, "Characteristic write received, conn_id %u, value", param->write.conn_id);
        ESP_LOG_BUFFER_HEX(DEMO_TAG, param->write.value, param->write.len);
        traverse_send_peer(param->write.len, param->write.value);
        break;
    case ESP_GATTS_CONNECT_EVT:
        if (param->connect.link_role == 1) {
            ESP_LOGI(DEMO_TAG, "Connected, conn_id %u, remote "ESP_BD_ADDR_STR", total %u",
                param->connect.conn_id, ESP_BD_ADDR_HEX(param->connect.remote_bda), ++multi_conn_num);
            prph_conn_id = param->connect.conn_id;
            advertising_state = DISABLED;
#if (BLE50_SUPPORTED == 1)
            esp_ble_gap_ext_adv_stop(1, &ext_adv[0].instance);
#else
            esp_ble_gap_stop_advertising();
#endif
        }
        break;
    case ESP_GATTS_OPEN_EVT:
        break;
    case ESP_GATTS_DISCONNECT_EVT:
        if (param->disconnect.conn_id == prph_conn_id) {
            ESP_LOGI(DEMO_TAG, "Disconnected, remote "ESP_BD_ADDR_STR", reason 0x%x, total %u",
                ESP_BD_ADDR_HEX(param->disconnect.remote_bda), param->disconnect.reason,
                (multi_conn_num ? --multi_conn_num : multi_conn_num));
            advertising_state = PENDING;
#if (BLE50_SUPPORTED == 1)
            esp_ble_gap_addr_create_static(adv_rand_addr);
            esp_ble_gap_ext_adv_set_rand_addr(ADV_HANDLE_INST, adv_rand_addr);
#else
            esp_ble_gap_addr_create_static(adv_rand_addr);
            esp_ble_gap_set_rand_addr((uint8_t *)adv_rand_addr);
#endif
        }
        break;
    case ESP_GATTS_CREAT_ATTR_TAB_EVT:
        ESP_LOGI(DEMO_TAG, "The number handle = %x", param->add_attr_tab.num_handle);
        if (param->create.status == ESP_GATT_OK) {
            if (param->add_attr_tab.num_handle == HRS_IDX_NB) {
                memcpy(profile_handle_table, param->add_attr_tab.handles,
                       sizeof(profile_handle_table));
                esp_ble_gatts_start_service(profile_handle_table[IDX_SVC]);
            } else {
                ESP_LOGE(DEMO_TAG, "Create attribute table abnormally, num_handle (%d) doesn't equal to HRS_IDX_NB(%d)",
                         param->add_attr_tab.num_handle, HRS_IDX_NB);
            }
        } else {
            ESP_LOGE(DEMO_TAG, " Create attribute table failed, status %x", param->create.status);
        }
        break;
    default:
        break;
    }
}

static void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    if (event == ESP_GATTC_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            gattc_profile_tab[param->reg.app_id].gattc_if = gattc_if;
        } else {
            ESP_LOGE(DEMO_TAG, "reg app failed, app_id %d", param->reg.app_id);
            return;
        }
    }
    do {
        int idx;
        for (idx = 0; idx < GATTC_PROFILE_NUM; idx++) {
            if (gattc_if == ESP_GATT_IF_NONE || gattc_if == gattc_profile_tab[idx].gattc_if) {
                if (gattc_profile_tab[idx].gattc_cb) {
                    gattc_profile_tab[idx].gattc_cb(event, gattc_if, param);
                }
            }
        }
    } while (0);
}

static void esp_gatts_cb(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    if (event == ESP_GATTS_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            gatts_profile_tab[0].gatts_if = gatts_if;
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
                gatts_if == gatts_profile_tab[idx].gatts_if) {
                if (gatts_profile_tab[idx].gatts_cb) {
                    gatts_profile_tab[idx].gatts_cb(event, gatts_if, param);
                }
            }
        }
    } while (0);
}

static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT:
        ESP_LOGI(DEMO_TAG, "Random address set, status %d, addr "ESP_BD_ADDR_STR"",
            param->set_rand_addr_cmpl.status, ESP_BD_ADDR_HEX(new_rand_addr));
        if (advertising_state == PENDING && prph_conn_id == 0xFFFF) {
#if (BLE50_SUPPORTED == 0)
            advertising_state = ENABLED;
            esp_ble_gap_start_advertising(&legacy_adv_params);
#endif
        }
        if (scan_state == PENDING && multi_conn_num < BLE_PEER_MAX_NUM) {
            scan_state = ENABLED;
#if (BLE50_SUPPORTED == 1)
            esp_ble_gap_start_ext_scan(EXT_SCAN_DURATION, EXT_SCAN_PERIOD);
#else
            esp_ble_gap_start_scanning(0);
#endif
        }
        break;
#if (BLE50_SUPPORTED == 1)
    case ESP_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT:
        ESP_LOGI(DEMO_TAG, "Extended scanning params set, status %d", param->set_ext_scan_params.status);
        scan_state = PENDING;
        esp_ble_gap_addr_create_static(new_rand_addr);
        esp_ble_gap_set_rand_addr((uint8_t *)new_rand_addr);
        break;
    case ESP_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT:
        ESP_LOGI(DEMO_TAG, "Extended scanning start, status %d", param->ext_scan_start.status);
        break;
    case ESP_GAP_BLE_EXT_SCAN_STOP_COMPLETE_EVT:
        ESP_LOGI(DEMO_TAG, "Extended scanning stop");
        break;
    case ESP_GAP_BLE_EXT_ADV_REPORT_EVT: {
        uint8_t *adv_name = NULL;
        uint8_t adv_name_len = 0;
        adv_name = esp_ble_resolve_adv_data_by_type(param->ext_adv_report.params.adv_data,
                                            param->ext_adv_report.params.adv_data_len,
                                            ESP_BLE_AD_TYPE_NAME_CMPL, &adv_name_len);
        // ESP_LOGI(DEMO_TAG, "Scan result, device "ESP_BD_ADDR_STR", name len %u", ESP_BD_ADDR_HEX(param->ext_adv_report.params.addr), adv_name_len);
        // ESP_LOG_BUFFER_CHAR(DEMO_TAG, adv_name, adv_name_len);
        if (strlen(remote_target_name) == adv_name_len && strncmp((char *)adv_name, remote_target_name, adv_name_len) == 0)
        {
            esp_ble_gap_stop_ext_scan();
            scan_state = DISABLED;

            #if CONFIG_SOC_BLE_MULTI_CONN_OPTIMIZATION
            /*
             * The scheduling time for each connection is the common factor divided by the number of connections.
             * Note that the time allocated to each connection should be greater than the duration of
             * one TX/RX packet exchange to maintain the connection without disconnection.
             */
            esp_ble_gap_set_sch_len(0, BLE_PREF_EVT_LEN_MS * 1000);
            #endif

            // create gattc virtual connection
            esp_ble_gatt_creat_conn_params_t creat_conn_params = {0};
            memcpy(&creat_conn_params.remote_bda, param->ext_adv_report.params.addr, ESP_BD_ADDR_LEN);
            creat_conn_params.remote_addr_type = param->ext_adv_report.params.addr_type;
            creat_conn_params.own_addr_type = BLE_ADDR_TYPE_RANDOM;
            creat_conn_params.is_direct = true;
            creat_conn_params.is_aux = true;
            creat_conn_params.phy_mask = ESP_BLE_PHY_1M_PREF_MASK | ESP_BLE_PHY_2M_PREF_MASK | ESP_BLE_PHY_CODED_PREF_MASK;
            creat_conn_params.phy_1m_conn_params = &phy_1m_conn_params;
            creat_conn_params.phy_2m_conn_params = &phy_2m_conn_params;
            creat_conn_params.phy_coded_conn_params = &phy_coded_conn_params;
            esp_ble_gattc_enh_open(gattc_profile_tab->gattc_if, &creat_conn_params);
            ESP_LOGI(DEMO_TAG, "Create connection, remote "ESP_BD_ADDR_STR"", ESP_BD_ADDR_HEX(param->ext_adv_report.params.addr));
        }
        break;
    }
#else
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT:
        ESP_LOGI(DEMO_TAG, "Scanning params set, status %d", param->scan_param_cmpl.status);
        scan_state = PENDING;
        esp_ble_gap_addr_create_static(new_rand_addr);
        esp_ble_gap_set_rand_addr((uint8_t *)new_rand_addr);
        break;
    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
        ESP_LOGI(DEMO_TAG, "Scanning start, status %d", param->scan_start_cmpl.status);
        break;
    case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
        ESP_LOGI(DEMO_TAG, "Scanning stop");
        break;
    case ESP_GAP_BLE_SCAN_RESULT_EVT: {
        esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;
        if (scan_result->scan_rst.search_evt == ESP_GAP_SEARCH_INQ_RES_EVT) {
            uint8_t *adv_name = NULL;
            uint8_t adv_name_len = 0;
            adv_name = esp_ble_resolve_adv_data_by_type(scan_result->scan_rst.ble_adv,
                                                        scan_result->scan_rst.adv_data_len + scan_result->scan_rst.scan_rsp_len,
                                                        ESP_BLE_AD_TYPE_NAME_CMPL,
                                                        &adv_name_len);
            // ESP_LOGI(DEMO_TAG, "Scan result, device "ESP_BD_ADDR_STR", name len %u", ESP_BD_ADDR_HEX(scan_result->scan_rst.bda), adv_name_len);
            // ESP_LOG_BUFFER_CHAR(DEMO_TAG, adv_name, adv_name_len);
            if (strlen(remote_target_name) == adv_name_len && strncmp((char *)adv_name, remote_target_name, adv_name_len) == 0) {
                esp_ble_gap_stop_scanning();
                scan_state = DISABLED;

                #if CONFIG_SOC_BLE_MULTI_CONN_OPTIMIZATION
                /*
                 * The scheduling time for each connection is the common factor divided by the number of connections.
                 * Note that the time allocated to each connection should be greater than the duration of
                 * one TX/RX packet exchange to maintain the connection without disconnection.
                 */
                esp_ble_gap_set_sch_len(0, BLE_PREF_EVT_LEN_MS * 1000);
                #endif

                esp_ble_gatt_creat_conn_params_t creat_conn_params = {0};
                memcpy(&creat_conn_params.remote_bda, param->scan_rst.bda, ESP_BD_ADDR_LEN);
                creat_conn_params.remote_addr_type = param->scan_rst.ble_addr_type;
                creat_conn_params.own_addr_type = BLE_ADDR_TYPE_RANDOM;
                creat_conn_params.is_direct = true;
                creat_conn_params.is_aux = false;
                creat_conn_params.phy_mask = ESP_BLE_PHY_1M_PREF_MASK;
                creat_conn_params.phy_1m_conn_params = &phy_1m_conn_params;
                creat_conn_params.phy_2m_conn_params = NULL;
                creat_conn_params.phy_coded_conn_params = NULL;
                esp_ble_gattc_enh_open(gattc_profile_tab->gattc_if, &creat_conn_params);
                ESP_LOGI(DEMO_TAG, "Create connection, remote "ESP_BD_ADDR_STR"", ESP_BD_ADDR_HEX(param->scan_rst.bda));
            }
        }
        break;
    }
#endif
#if (BLE50_SUPPORTED == 1)
    case ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT:
        ESP_LOGI(DEMO_TAG, "Extended adv random address set, status %d, addr "ESP_BD_ADDR_STR"",
            param->ext_adv_set_rand_addr.status, ESP_BD_ADDR_HEX(adv_rand_addr));
        advertising_state = ENABLED;
        esp_ble_gap_ext_adv_start(ADV_HANDLE_NUM, &ext_adv[0]);
        break;
    case ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT:
        ESP_LOGI(DEMO_TAG, "Extended adv start, status %d", param->ext_adv_start.status);
        break;
    case ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT:
        ESP_LOGI(DEMO_TAG, "Extended adv param set, status %d", param->ext_adv_set_params.status);
        esp_ble_gap_config_ext_adv_data_raw(ADV_HANDLE_INST, sizeof(adv_data_raw), &adv_data_raw[0]);
        break;
    case ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT:
        ESP_LOGI(DEMO_TAG, "Extended adv data set, status %d", param->ext_adv_data_set.status);
        advertising_state = PENDING;
        esp_ble_gap_addr_create_static(adv_rand_addr);
        esp_ble_gap_ext_adv_set_rand_addr(ADV_HANDLE_INST, adv_rand_addr);
        break;
    case ESP_GAP_BLE_ADV_TERMINATED_EVT:
        ESP_LOGI(DEMO_TAG, "Extended adv terminate, status = %d", param->adv_terminate.status);
        if (param->adv_terminate.status == 0x00) {
            ESP_LOGI(DEMO_TAG, "ADV successfully ended with a connection being created");
        }
        break;
#else
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        ESP_LOGI(DEMO_TAG, "Advertising start, status %d", param->adv_start_cmpl.status);
        break;
    case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
        ESP_LOGI(DEMO_TAG, "Advertising data set, status %d", param->adv_data_raw_cmpl.status);
        break;
    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        ESP_LOGI(DEMO_TAG, "Advertising stop, status %d", param->adv_stop_cmpl.status);
        if (scan_state == PENDING) {
            esp_ble_gap_addr_create_static(new_rand_addr);
            esp_ble_gap_set_rand_addr((uint8_t *)new_rand_addr);
        }
        break;
#endif
    case ESP_GAP_BLE_SET_COMMON_FACTOR_CMPL_EVT:
        ESP_LOGI(DEMO_TAG, "Common factor set, status %d", param->set_common_factor_cmpl.status);
        break;
    case ESP_GAP_BLE_SET_SCH_LEN_CMPL_EVT:
        ESP_LOGI(DEMO_TAG, "Scheduling length set, status %d", param->set_sch_len_cmpl.status);
        break;
    default:
        break;
    }
}
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
        ESP_LOGE(DEMO_TAG, "%s initialize controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(DEMO_TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    esp_bluedroid_config_t cfg = BT_BLUEDROID_INIT_CONFIG_DEFAULT();
    ret = esp_bluedroid_init_with_cfg(&cfg);
    if (ret) {
        ESP_LOGE(DEMO_TAG, "%s init bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(DEMO_TAG, "%s enable bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    // register the  callback function to the gap module
    ret = esp_ble_gap_register_callback(esp_gap_cb);
    if (ret) {
        ESP_LOGE(DEMO_TAG, "%s gap register error, error code = %x", __func__, ret);
        return;
    }

    // register the callback function to the gattc module
    ret = esp_ble_gattc_register_callback(esp_gattc_cb);
    if (ret) {
        ESP_LOGE(DEMO_TAG, "%s gattc register error, error code = %x", __func__, ret);
        return;
    }

    // register the callback function to the gatts module
    ret = esp_ble_gatts_register_callback(esp_gatts_cb);
    if (ret) {
        ESP_LOGE(DEMO_TAG, "%s gatts register error, error code = %x", __func__, ret);
    }

    restart_scan_sem = xSemaphoreCreateBinary();
    peer_manager_init();

    #if CONFIG_SOC_BLE_MULTI_CONN_OPTIMIZATION
    /*
     * In this example, all connection intervals are the same,
     * so the common factor is equal to each connection’s interval.
     */
    uint32_t common_factor = (BLE_PREF_CONN_ITVL_MS * 1000) / 625;
    esp_ble_gap_set_common_factor(common_factor);
    #endif

    esp_ble_gap_addr_create_static(new_rand_addr);
    esp_ble_gap_set_rand_addr((uint8_t *)new_rand_addr);

    ret = esp_ble_gattc_app_register(GATTC_PROFILE_APP_ID);
    if (ret) {
        ESP_LOGE(DEMO_TAG, "%s gattc app register error, error code = %x", __func__, ret);
    }

    ret = esp_ble_gatts_app_register(GATTS_PROFILE_APP_ID);
    if (ret) {
        ESP_LOGE(DEMO_TAG, "%s gatts app register error, error code = %x", __func__, ret);
    }

    ret = esp_ble_gatt_set_local_mtu(200);
    if (ret) {
        ESP_LOGE(DEMO_TAG, "set local  MTU failed, error code = %x", ret);
    }

    while (1) {
        if (xSemaphoreTake(restart_scan_sem, portMAX_DELAY) == pdTRUE) {
            if (multi_conn_num < BLE_PEER_MAX_NUM && scan_state == DISABLED) {
                scan_state = PENDING;
    #if (BLE50_SUPPORTED == 0)
                if (advertising_state != DISABLED) {
                    advertising_state = DISABLED;
                    esp_ble_gap_stop_advertising();
                    break;
                }
    #endif
                esp_ble_gap_addr_create_static(new_rand_addr);
                esp_ble_gap_set_rand_addr((uint8_t *)new_rand_addr);
            }
        }
    }

    return;
}
