/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * BLE 5.0 dual local-identity GATT server.
 *
 * The device advertises TWO connectable extended advertising sets at the same
 * time, each using a different local identity:
 *
 *   - Adv set 0 : Public address          -> identity A
 *   - Adv set 1 : fixed Static Random addr -> identity B
 *
 * A single phone can connect to BOTH identities simultaneously. With
 * CONFIG_BT_BLE_PERIPH_PSEUDO_ADDR_BOND enabled, the Host derives a distinct
 * pseudo address per (local_identity, peer) pair, so the two links get
 * independent device records, LTKs and NVS bond sections. Deleting one
 * identity's bond never affects the other.
 *
 * IMPORTANT for the application:
 *   - remote_bda reported here is the Host PSEUDO (the same phone shows up as
 *     two different addresses, one per identity). Use conn_id as the link key.
 *   - Call esp_ble_gap_get_conn_identity(pseudo, &id) to recover the real peer
 *     MAC and the local identity that the link was established with.
 */

#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "ble50_dual_identity_server.h"

#define TAG                          "DUAL_ID"

#define ESP_APP_ID                   0x55
#define SVC_INST_ID                  0
#define GATTS_DEMO_CHAR_VAL_LEN_MAX  0x40
#define MAX_CONN                     3
#define NOTIFY_ENABLE                0x0001

/* Two simultaneously-advertising connectable extended advertising sets. */
#define ADV_HANDLE_PUBLIC            0       /* identity A : Public           */
#define ADV_HANDLE_RANDOM            1       /* identity B : Static Random    */
#define NUM_ADV_SET                  2

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

/* A FIXED static random address for identity B. It MUST be persistent across
 * reboots (top two bits = 0b11) so the per-identity bond bucket matches on
 * reconnect. Do NOT generate it randomly at every boot. */
static esp_bd_addr_t s_identity_b_addr = {0xC1, 0x22, 0x33, 0x44, 0x55, 0x66};

static uint16_t s_handle_table[HRS_IDX_NB];

/* Track which advertising sets are currently on-air so that on a disconnect we
 * only restart the set(s) that had been consumed by a connection. */
static bool s_adv_on_air[NUM_ADV_SET];

/* ---- advertising data: one human-readable name per identity ---- */
static uint8_t adv_data_public[] = {
    0x02, ESP_BLE_AD_TYPE_FLAG, 0x06,
    0x12, ESP_BLE_AD_TYPE_NAME_CMPL,
    'E', 'S', 'P', '_', 'D', 'U', 'A', 'L', '_', 'P', 'U', 'B', 'L', 'I', 'C', '_', 'A',
};

static uint8_t adv_data_random[] = {
    0x02, ESP_BLE_AD_TYPE_FLAG, 0x06,
    0x12, ESP_BLE_AD_TYPE_NAME_CMPL,
    'E', 'S', 'P', '_', 'D', 'U', 'A', 'L', '_', 'R', 'A', 'N', 'D', 'O', 'M', '_', 'B',
};

static esp_ble_gap_ext_adv_t s_ext_adv[NUM_ADV_SET] = {
    [0] = {ADV_HANDLE_PUBLIC, 0, 0},
    [1] = {ADV_HANDLE_RANDOM, 0, 0},
};

static esp_ble_gap_ext_adv_params_t s_adv_params_public = {
    .type = ESP_BLE_GAP_SET_EXT_ADV_PROP_CONNECTABLE,
    .interval_min = ESP_BLE_GAP_ADV_ITVL_MS(40),
    .interval_max = ESP_BLE_GAP_ADV_ITVL_MS(40),
    .channel_map = ADV_CHNL_ALL,
    .filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
    .primary_phy = ESP_BLE_GAP_PHY_1M,
    .max_skip = 0,
    .secondary_phy = ESP_BLE_GAP_PHY_2M,
    .sid = 0,
    .scan_req_notif = false,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .tx_power = EXT_ADV_TX_PWR_NO_PREFERENCE,
};

static esp_ble_gap_ext_adv_params_t s_adv_params_random = {
    .type = ESP_BLE_GAP_SET_EXT_ADV_PROP_CONNECTABLE,
    .interval_min = ESP_BLE_GAP_ADV_ITVL_MS(40),
    .interval_max = ESP_BLE_GAP_ADV_ITVL_MS(40),
    .channel_map = ADV_CHNL_ALL,
    .filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
    .primary_phy = ESP_BLE_GAP_PHY_1M,
    .max_skip = 0,
    .secondary_phy = ESP_BLE_GAP_PHY_2M,
    .sid = 1,
    .scan_req_notif = false,
    .own_addr_type = BLE_ADDR_TYPE_RANDOM,
    .tx_power = EXT_ADV_TX_PWR_NO_PREFERENCE,
};

/* ---- minimal GATT database (one read/write/notify characteristic) ---- */
static const uint16_t primary_service_uuid         = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid   = ESP_GATT_UUID_CHAR_DECLARE;
static const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
static const uint16_t GATTS_SERVICE_UUID_TEST      = 0x00FF;
static const uint16_t GATTS_CHAR_UUID_TEST_A       = 0xFF01;
static const uint8_t  char_prop_read_write_notify  = ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
static const uint8_t  ccc[2]                        = {0x00, 0x00};
static const uint8_t  char_value[4]                = {0x11, 0x22, 0x33, 0x44};
#define CHAR_DECLARATION_SIZE  (sizeof(uint8_t))

static const esp_gatts_attr_db_t gatt_db[HRS_IDX_NB] = {
    [IDX_SVC] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
      sizeof(uint16_t), sizeof(GATTS_SERVICE_UUID_TEST), (uint8_t *)&GATTS_SERVICE_UUID_TEST}},

    [IDX_CHAR_A] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_write_notify}},

    /* Require encryption to read/write so the link must be paired/encrypted. */
    [IDX_CHAR_VAL_A] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_TEST_A,
      ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value), (uint8_t *)char_value}},

    [IDX_CHAR_CFG_A] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid,
      ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      sizeof(uint16_t), sizeof(ccc), (uint8_t *)ccc}},
};

static esp_gatt_if_t s_gatts_if = ESP_GATT_IF_NONE;

/* Per-link notify subscription (CCC). Indexed by conn_id. */
static bool s_notify_enabled[MAX_CONN];

/* -------------------------------------------------------------------------- */

static const char *auth_req_to_str(esp_ble_auth_req_t auth_req)
{
    switch (auth_req) {
    case ESP_LE_AUTH_NO_BOND:          return "NO_BOND";
    case ESP_LE_AUTH_BOND:             return "BOND";
    case ESP_LE_AUTH_REQ_MITM:         return "MITM";
    case ESP_LE_AUTH_REQ_BOND_MITM:    return "BOND_MITM";
    case ESP_LE_AUTH_REQ_SC_ONLY:      return "SC_ONLY";
    case ESP_LE_AUTH_REQ_SC_BOND:      return "SC_BOND";
    case ESP_LE_AUTH_REQ_SC_MITM:      return "SC_MITM";
    case ESP_LE_AUTH_REQ_SC_MITM_BOND: return "SC_MITM_BOND";
    default:                           return "INVALID";
    }
}

static void show_bonded_devices(void)
{
    int dev_num = esp_ble_get_bond_device_num();
    if (dev_num <= 0) {
        ESP_LOGI(TAG, "Bonded devices: 0");
        return;
    }
    esp_ble_bond_dev_t *list = malloc(sizeof(esp_ble_bond_dev_t) * dev_num);
    if (!list) {
        return;
    }
    esp_ble_get_bond_device_list(&dev_num, list);
    ESP_LOGI(TAG, "Bonded devices: %d (each entry is a (local,peer) identity = one pseudo)", dev_num);
    for (int i = 0; i < dev_num; i++) {
        ESP_LOGI(TAG, "  [%d] pseudo "ESP_BD_ADDR_STR"", i, ESP_BD_ADDR_HEX(list[i].bd_addr));
    }
    free(list);
}

#if CONFIG_BT_BLE_PERIPH_PSEUDO_ADDR_BOND
static void log_conn_identity(const char *tag, esp_bd_addr_t pseudo)
{
    esp_ble_conn_identity_t id;
    if (esp_ble_gap_get_conn_identity(pseudo, &id) == ESP_OK) {
        ESP_LOGI(TAG, "%s identity: real peer "ESP_BD_ADDR_STR" (type %u), local "ESP_BD_ADDR_STR" (type %u)",
                 tag,
                 ESP_BD_ADDR_HEX(id.peer_addr), id.peer_addr_type,
                 ESP_BD_ADDR_HEX(id.local_addr), id.local_addr_type);
    } else {
        ESP_LOGW(TAG, "%s identity: pseudo not finalized yet (link may still be on real peer)", tag);
    }
}
#endif

static void request_ext_adv_start(uint8_t inst)
{
    if (inst >= NUM_ADV_SET || s_adv_on_air[inst]) {
        return;
    }
    esp_err_t ret = esp_ble_gap_ext_adv_start(1, &s_ext_adv[inst]);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ext_adv_start enqueue failed for instance %u: %s", inst, esp_err_to_name(ret));
    }
}

static void start_idle_adv_sets(void)
{
    for (int i = 0; i < NUM_ADV_SET; i++) {
        request_ext_adv_start(i);
    }
}

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT:
        ESP_LOGI(TAG, "Set random addr (identity B) done, status %d", param->ext_adv_set_rand_addr.status);
        if (param->ext_adv_set_rand_addr.status == ESP_BT_STATUS_SUCCESS) {
            esp_ble_gap_config_ext_adv_data_raw(ADV_HANDLE_RANDOM, sizeof(adv_data_random), adv_data_random);
        } else {
            ESP_LOGE(TAG, "Identity B random addr failed, skip adv data/start");
        }
        break;
    case ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT:
        ESP_LOGI(TAG, "Ext adv params set, instance %u, status %d",
                 param->ext_adv_set_params.instance, param->ext_adv_set_params.status);
        if (param->ext_adv_set_params.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "Ext adv params failed for instance %u, skip downstream setup",
                     param->ext_adv_set_params.instance);
            break;
        }
        if (param->ext_adv_set_params.instance == ADV_HANDLE_PUBLIC) {
            esp_ble_gap_config_ext_adv_data_raw(ADV_HANDLE_PUBLIC, sizeof(adv_data_public), adv_data_public);
            esp_ble_gap_ext_adv_set_params(ADV_HANDLE_RANDOM, &s_adv_params_random);
        } else if (param->ext_adv_set_params.instance == ADV_HANDLE_RANDOM) {
            esp_ble_gap_ext_adv_set_rand_addr(ADV_HANDLE_RANDOM, s_identity_b_addr);
        }
        break;
    case ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT:
        ESP_LOGI(TAG, "Ext adv data set, status %d, instance %u",
                 param->ext_adv_data_set.status, param->ext_adv_data_set.instance);
        if (param->ext_adv_data_set.status == ESP_BT_STATUS_SUCCESS) {
            request_ext_adv_start(param->ext_adv_data_set.instance);
        }
        break;
    case ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT:
        ESP_LOGI(TAG, "Ext adv start, status %d, instance_num %u",
                 param->ext_adv_start.status, param->ext_adv_start.instance_num);
        for (uint8_t j = 0; j < param->ext_adv_start.instance_num; j++) {
            uint8_t inst = param->ext_adv_start.instance[j];
            if (inst >= NUM_ADV_SET) {
                continue;
            }
            if (param->ext_adv_start.status == ESP_BT_STATUS_SUCCESS) {
                s_adv_on_air[inst] = true;
            } else {
                s_adv_on_air[inst] = false;
                ESP_LOGE(TAG, "Ext adv start failed for instance %u", inst);
            }
        }
        break;
    case ESP_GAP_BLE_ADV_TERMINATED_EVT:
        ESP_LOGI(TAG, "Adv terminated: instance %u status 0x%x conn_idx %u",
                 param->adv_terminate.adv_instance, param->adv_terminate.status,
                 param->adv_terminate.conn_idx);
        if (param->adv_terminate.adv_instance < NUM_ADV_SET) {
            /* This set produced a connection -> it stopped advertising. */
            s_adv_on_air[param->adv_terminate.adv_instance] = false;
        }
        break;
    case ESP_GAP_BLE_SEC_REQ_EVT:
        esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);
        break;
    case ESP_GAP_BLE_NC_REQ_EVT:
        esp_ble_confirm_reply(param->ble_security.ble_req.bd_addr, true);
        break;
    case ESP_GAP_BLE_PASSKEY_NOTIF_EVT:
        ESP_LOGI(TAG, "Passkey notify: %06" PRIu32, param->ble_security.key_notif.passkey);
        break;
    case ESP_GAP_BLE_KEY_EVT:
        ESP_LOGI(TAG, "Key exchanged on link "ESP_BD_ADDR_STR" type %d",
                 ESP_BD_ADDR_HEX(param->ble_security.ble_key.bd_addr),
                 param->ble_security.ble_key.key_type);
        break;
    case ESP_GAP_BLE_AUTH_CMPL_EVT: {
        esp_bd_addr_t pseudo;
        memcpy(pseudo, param->ble_security.auth_cmpl.bd_addr, sizeof(esp_bd_addr_t));
        if (param->ble_security.auth_cmpl.success) {
            ESP_LOGI(TAG, "Pairing OK on link "ESP_BD_ADDR_STR", auth %s",
                     ESP_BD_ADDR_HEX(pseudo),
                     auth_req_to_str(param->ble_security.auth_cmpl.auth_mode));
#if CONFIG_BT_BLE_PERIPH_PSEUDO_ADDR_BOND
            log_conn_identity("AUTH_CMPL", pseudo);
#endif
        } else {
            ESP_LOGE(TAG, "Pairing FAILED on link "ESP_BD_ADDR_STR", reason 0x%x",
                     ESP_BD_ADDR_HEX(pseudo), param->ble_security.auth_cmpl.fail_reason);
        }
        show_bonded_devices();
        break;
    }
    case ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT:
        ESP_LOGI(TAG, "Bond removed, status %d, pseudo "ESP_BD_ADDR_STR"",
                 param->remove_bond_dev_cmpl.status,
                 ESP_BD_ADDR_HEX(param->remove_bond_dev_cmpl.bd_addr));
        break;
    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
        ESP_LOGI(TAG, "Conn params update, link "ESP_BD_ADDR_STR", status %d, "
                 "conn_int %u, latency %u, timeout %u",
                 ESP_BD_ADDR_HEX(param->update_conn_params.bda),
                 param->update_conn_params.status,
                 param->update_conn_params.conn_int,
                 param->update_conn_params.latency,
                 param->update_conn_params.timeout);
        break;
    case ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT:
        ESP_LOGI(TAG, "Data length update, status %d, rx %u, tx %u",
                 param->pkt_data_length_cmpl.status,
                 param->pkt_data_length_cmpl.params.rx_len,
                 param->pkt_data_length_cmpl.params.tx_len);
        break;
    default:
        break;
    }
}

static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
                                esp_ble_gatts_cb_param_t *param)
{
    switch (event) {
    case ESP_GATTS_REG_EVT:
        if (param->reg.status == ESP_GATT_OK) {
            s_gatts_if = gatts_if;
            esp_ble_gatts_create_attr_tab(gatt_db, gatts_if, HRS_IDX_NB, SVC_INST_ID);
        }
        break;
    case ESP_GATTS_CREAT_ATTR_TAB_EVT:
        if (param->add_attr_tab.status == ESP_GATT_OK && param->add_attr_tab.num_handle == HRS_IDX_NB) {
            memcpy(s_handle_table, param->add_attr_tab.handles, sizeof(s_handle_table));
            esp_ble_gatts_start_service(s_handle_table[IDX_SVC]);
        } else {
            ESP_LOGE(TAG, "Create attr table failed");
        }
        break;
    case ESP_GATTS_CONNECT_EVT:
        ESP_LOGI(TAG, "CONNECT conn_id %u, remote(pseudo) "ESP_BD_ADDR_STR"",
                 param->connect.conn_id, ESP_BD_ADDR_HEX(param->connect.remote_bda));
        ESP_LOGI(TAG, "  -> use conn_id %u as the link key (remote_bda may be a Host pseudo)",
                 param->connect.conn_id);
        /* Trigger pairing / encryption for this link. */
        esp_ble_set_encryption(param->connect.remote_bda, ESP_BLE_SEC_ENCRYPT_MITM);
        break;
    case ESP_GATTS_WRITE_EVT:
        if (param->write.is_prep) {
            break;
        }
        ESP_LOGI(TAG, "WRITE conn_id %u handle %u len %d",
                 param->write.conn_id, param->write.handle, param->write.len);
        if (param->write.handle == s_handle_table[IDX_CHAR_CFG_A] && param->write.len == 2) {
            uint16_t descr_value = (param->write.value[1] << 8) | param->write.value[0];
            if (param->write.conn_id < MAX_CONN) {
                if (descr_value == NOTIFY_ENABLE) {
                    s_notify_enabled[param->write.conn_id] = true;
                    ESP_LOGI(TAG, "Notify enabled on conn_id %u", param->write.conn_id);
                } else if (descr_value == 0x0000) {
                    s_notify_enabled[param->write.conn_id] = false;
                    ESP_LOGI(TAG, "Notify disabled on conn_id %u", param->write.conn_id);
                }
            }
        } else if (param->write.handle == s_handle_table[IDX_CHAR_VAL_A]) {
            ESP_LOG_BUFFER_HEX(TAG, param->write.value, param->write.len);
            if (param->write.conn_id < MAX_CONN && s_notify_enabled[param->write.conn_id] &&
                param->write.len > 0) {
                esp_err_t ret = esp_ble_gatts_send_indicate(gatts_if, param->write.conn_id,
                                                            s_handle_table[IDX_CHAR_VAL_A],
                                                            param->write.len, param->write.value, false);
                if (ret != ESP_OK) {
                    ESP_LOGE(TAG, "Notify echo failed on conn_id %u: %s",
                             param->write.conn_id, esp_err_to_name(ret));
                } else {
                    ESP_LOGI(TAG, "Notify echo sent on conn_id %u, len %d",
                             param->write.conn_id, param->write.len);
                }
            }
        }
        break;
    case ESP_GATTS_READ_EVT:
        ESP_LOGI(TAG, "READ conn_id %u (link is encrypted)", param->read.conn_id);
        break;
    case ESP_GATTS_MTU_EVT:
        ESP_LOGI(TAG, "MTU exchange, conn_id %u, mtu %u",
                 param->mtu.conn_id, param->mtu.mtu);
        break;
    case ESP_GATTS_CONF_EVT:
        ESP_LOGI(TAG, "Notify/indicate confirm, conn_id %u, status %d, handle %u",
                 param->conf.conn_id, param->conf.status, param->conf.handle);
        break;
    case ESP_GATTS_DISCONNECT_EVT:
        ESP_LOGI(TAG, "DISCONNECT conn_id %u, remote(pseudo) "ESP_BD_ADDR_STR", reason 0x%x",
                 param->disconnect.conn_id, ESP_BD_ADDR_HEX(param->disconnect.remote_bda),
                 param->disconnect.reason);
        if (param->disconnect.conn_id < MAX_CONN) {
            s_notify_enabled[param->disconnect.conn_id] = false;
        }
        /* Re-advertise only the set(s) that are not currently on-air. */
        start_idle_adv_sets();
        break;
    default:
        break;
    }
}

static void setup_advertising(void)
{
    /* Kick off the event-driven setup chain:
     * PUBLIC params -> PUBLIC data -> RANDOM params -> RANDOM rand_addr -> RANDOM data.
     * Each step is gated on the previous COMPLETE event status. */
    esp_err_t ret = esp_ble_gap_ext_adv_set_params(ADV_HANDLE_PUBLIC, &s_adv_params_public);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ext_adv_set_params(PUBLIC) enqueue failed: %s", esp_err_to_name(ret));
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

    esp_bluedroid_config_t cfg = BT_BLUEDROID_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bluedroid_init_with_cfg(&cfg));
    ESP_ERROR_CHECK(esp_bluedroid_enable());

    ESP_ERROR_CHECK(esp_ble_gatts_register_callback(gatts_event_handler));
    ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_event_handler));
    ESP_ERROR_CHECK(esp_ble_gatts_app_register(ESP_APP_ID));

    /* Security: bond + MITM + Secure Connections, static passkey. */
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

#if !CONFIG_BT_BLE_PERIPH_PSEUDO_ADDR_BOND
    ESP_LOGW(TAG, "CONFIG_BT_BLE_PERIPH_PSEUDO_ADDR_BOND is DISABLED:");
    ESP_LOGW(TAG, "  two identities of the same phone will share one bond and overwrite each other.");
    ESP_LOGW(TAG, "  Enable it to get isolated bonds per local identity.");
#else
    ESP_LOGI(TAG, "Pseudo-address dual-identity bond isolation ENABLED");
#endif

    setup_advertising();
    ESP_LOGI(TAG, "Advertising as two identities: Public (A) + Static Random "ESP_BD_ADDR_STR" (B)",
             ESP_BD_ADDR_HEX(s_identity_b_addr));
}
