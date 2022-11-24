// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_hid_gap.h"

static const char *TAG = "ESP_HID_GAP";

// uncomment to print all devices that were seen during a scan
#define GAP_DBG_PRINTF(...) //printf(__VA_ARGS__)
//static const char * gap_bt_prop_type_names[5] = {"","BDNAME","COD","RSSI","EIR"};

static esp_hid_scan_result_t *bt_scan_results = NULL;
static size_t num_bt_scan_results = 0;

static esp_hid_scan_result_t *ble_scan_results = NULL;
static size_t num_ble_scan_results = 0;

static xSemaphoreHandle bt_hidh_cb_semaphore = NULL;
#define WAIT_BT_CB() xSemaphoreTake(bt_hidh_cb_semaphore, portMAX_DELAY)
#define SEND_BT_CB() xSemaphoreGive(bt_hidh_cb_semaphore)

static xSemaphoreHandle ble_hidh_cb_semaphore = NULL;
#define WAIT_BLE_CB() xSemaphoreTake(ble_hidh_cb_semaphore, portMAX_DELAY)
#define SEND_BLE_CB() xSemaphoreGive(ble_hidh_cb_semaphore)

#define SIZEOF_ARRAY(a) (sizeof(a)/sizeof(*a))

static const char *ble_gap_evt_names[] = { "ADV_DATA_SET_COMPLETE", "SCAN_RSP_DATA_SET_COMPLETE", "SCAN_PARAM_SET_COMPLETE", "SCAN_RESULT", "ADV_DATA_RAW_SET_COMPLETE", "SCAN_RSP_DATA_RAW_SET_COMPLETE", "ADV_START_COMPLETE", "SCAN_START_COMPLETE", "AUTH_CMPL", "KEY", "SEC_REQ", "PASSKEY_NOTIF", "PASSKEY_REQ", "OOB_REQ", "LOCAL_IR", "LOCAL_ER", "NC_REQ", "ADV_STOP_COMPLETE", "SCAN_STOP_COMPLETE", "SET_STATIC_RAND_ADDR", "UPDATE_CONN_PARAMS", "SET_PKT_LENGTH_COMPLETE", "SET_LOCAL_PRIVACY_COMPLETE", "REMOVE_BOND_DEV_COMPLETE", "CLEAR_BOND_DEV_COMPLETE", "GET_BOND_DEV_COMPLETE", "READ_RSSI_COMPLETE", "UPDATE_WHITELIST_COMPLETE"};
static const char *bt_gap_evt_names[] = { "DISC_RES", "DISC_STATE_CHANGED", "RMT_SRVCS", "RMT_SRVC_REC", "AUTH_CMPL", "PIN_REQ", "CFM_REQ", "KEY_NOTIF", "KEY_REQ", "READ_RSSI_DELTA"};
static const char *ble_addr_type_names[] = {"PUBLIC", "RANDOM", "RPA_PUBLIC", "RPA_RANDOM"};

const char *ble_addr_type_str(esp_ble_addr_type_t ble_addr_type)
{
    if (ble_addr_type > BLE_ADDR_TYPE_RPA_RANDOM) {
        return "UNKNOWN";
    }
    return ble_addr_type_names[ble_addr_type];
}

const char *ble_gap_evt_str(uint8_t event)
{
    if (event >= SIZEOF_ARRAY(ble_gap_evt_names)) {
        return "UNKNOWN";
    }
    return ble_gap_evt_names[event];
}

const char *bt_gap_evt_str(uint8_t event)
{
    if (event >= SIZEOF_ARRAY(bt_gap_evt_names)) {
        return "UNKNOWN";
    }
    return bt_gap_evt_names[event];
}

const char *esp_ble_key_type_str(esp_ble_key_type_t key_type)
{
    const char *key_str = NULL;
    switch (key_type) {
    case ESP_LE_KEY_NONE:
        key_str = "ESP_LE_KEY_NONE";
        break;
    case ESP_LE_KEY_PENC:
        key_str = "ESP_LE_KEY_PENC";
        break;
    case ESP_LE_KEY_PID:
        key_str = "ESP_LE_KEY_PID";
        break;
    case ESP_LE_KEY_PCSRK:
        key_str = "ESP_LE_KEY_PCSRK";
        break;
    case ESP_LE_KEY_PLK:
        key_str = "ESP_LE_KEY_PLK";
        break;
    case ESP_LE_KEY_LLK:
        key_str = "ESP_LE_KEY_LLK";
        break;
    case ESP_LE_KEY_LENC:
        key_str = "ESP_LE_KEY_LENC";
        break;
    case ESP_LE_KEY_LID:
        key_str = "ESP_LE_KEY_LID";
        break;
    case ESP_LE_KEY_LCSRK:
        key_str = "ESP_LE_KEY_LCSRK";
        break;
    default:
        key_str = "INVALID BLE KEY TYPE";
        break;

    }
    return key_str;
}

void esp_hid_scan_results_free(esp_hid_scan_result_t *results)
{
    esp_hid_scan_result_t *r = NULL;
    while (results) {
        r = results;
        results = results->next;
        if (r->name != NULL) {
            free((char *)r->name);
        }
        free(r);
    }
}

static esp_hid_scan_result_t *find_scan_result(esp_bd_addr_t bda, esp_hid_scan_result_t *results)
{
    esp_hid_scan_result_t *r = results;
    while (r) {
        if (memcmp(bda, r->bda, sizeof(esp_bd_addr_t)) == 0) {
            return r;
        }
        r = r->next;
    }
    return NULL;
}

static void add_bt_scan_result(esp_bd_addr_t bda, esp_bt_cod_t *cod, esp_bt_uuid_t *uuid, uint8_t *name, uint8_t name_len, int rssi)
{
    esp_hid_scan_result_t *r = find_scan_result(bda, bt_scan_results);
    if (r) {
        //Some info may come later
        if (r->name == NULL && name && name_len) {
            char *name_s = (char *)malloc(name_len + 1);
            if (name_s == NULL) {
                ESP_LOGE(TAG, "Malloc result name failed!");
                return;
            }
            memcpy(name_s, name, name_len);
            name_s[name_len] = 0;
            r->name = (const char *)name_s;
        }
        if (r->bt.uuid.len == 0 && uuid->len) {
            memcpy(&r->bt.uuid, uuid, sizeof(esp_bt_uuid_t));
        }
        if (rssi != 0) {
            r->rssi = rssi;
        }
        return;
    }

    r = (esp_hid_scan_result_t *)malloc(sizeof(esp_hid_scan_result_t));
    if (r == NULL) {
        ESP_LOGE(TAG, "Malloc bt_hidh_scan_result_t failed!");
        return;
    }
    r->transport = ESP_HID_TRANSPORT_BT;
    memcpy(r->bda, bda, sizeof(esp_bd_addr_t));
    memcpy(&r->bt.cod, cod, sizeof(esp_bt_cod_t));
    memcpy(&r->bt.uuid, uuid, sizeof(esp_bt_uuid_t));
    r->usage = esp_hid_usage_from_cod((uint32_t)cod);
    r->rssi = rssi;
    r->name = NULL;
    if (name_len && name) {
        char *name_s = (char *)malloc(name_len + 1);
        if (name_s == NULL) {
            free(r);
            ESP_LOGE(TAG, "Malloc result name failed!");
            return;
        }
        memcpy(name_s, name, name_len);
        name_s[name_len] = 0;
        r->name = (const char *)name_s;
    }
    r->next = bt_scan_results;
    bt_scan_results = r;
    num_bt_scan_results++;
}

static void add_ble_scan_result(esp_bd_addr_t bda, esp_ble_addr_type_t addr_type, uint16_t appearance, uint8_t *name, uint8_t name_len, int rssi)
{
    if (find_scan_result(bda, ble_scan_results)) {
        ESP_LOGW(TAG, "Result already exists!");
        return;
    }
    esp_hid_scan_result_t *r = (esp_hid_scan_result_t *)malloc(sizeof(esp_hid_scan_result_t));
    if (r == NULL) {
        ESP_LOGE(TAG, "Malloc ble_hidh_scan_result_t failed!");
        return;
    }
    r->transport = ESP_HID_TRANSPORT_BLE;
    memcpy(r->bda, bda, sizeof(esp_bd_addr_t));
    r->ble.appearance = appearance;
    r->ble.addr_type = addr_type;
    r->usage = esp_hid_usage_from_appearance(appearance);
    r->rssi = rssi;
    r->name = NULL;
    if (name_len && name) {
        char *name_s = (char *)malloc(name_len + 1);
        if (name_s == NULL) {
            free(r);
            ESP_LOGE(TAG, "Malloc result name failed!");
            return;
        }
        memcpy(name_s, name, name_len);
        name_s[name_len] = 0;
        r->name = (const char *)name_s;
    }
    r->next = ble_scan_results;
    ble_scan_results = r;
    num_ble_scan_results++;
}

void print_uuid(esp_bt_uuid_t *uuid)
{
    if (uuid->len == ESP_UUID_LEN_16) {
        GAP_DBG_PRINTF("UUID16: 0x%04x", uuid->uuid.uuid16);
    } else if (uuid->len == ESP_UUID_LEN_32) {
        GAP_DBG_PRINTF("UUID32: 0x%08x", uuid->uuid.uuid32);
    } else if (uuid->len == ESP_UUID_LEN_128) {
        GAP_DBG_PRINTF("UUID128: %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x", uuid->uuid.uuid128[0],
                       uuid->uuid.uuid128[1], uuid->uuid.uuid128[2], uuid->uuid.uuid128[3],
                       uuid->uuid.uuid128[4], uuid->uuid.uuid128[5], uuid->uuid.uuid128[6],
                       uuid->uuid.uuid128[7], uuid->uuid.uuid128[8], uuid->uuid.uuid128[9],
                       uuid->uuid.uuid128[10], uuid->uuid.uuid128[11], uuid->uuid.uuid128[12],
                       uuid->uuid.uuid128[13], uuid->uuid.uuid128[14], uuid->uuid.uuid128[15]);
    }
}

static void handle_bt_device_result(struct disc_res_param *disc_res)
{
    GAP_DBG_PRINTF("BT : " ESP_BD_ADDR_STR, ESP_BD_ADDR_HEX(disc_res->bda));
    uint32_t codv = 0;
    esp_bt_cod_t *cod = (esp_bt_cod_t *)&codv;
    int8_t rssi = 0;
    uint8_t *name = NULL;
    uint8_t name_len = 0;
    esp_bt_uuid_t uuid;

    uuid.len = ESP_UUID_LEN_16;
    uuid.uuid.uuid16 = 0;

    for (int i = 0; i < disc_res->num_prop; i++) {
        esp_bt_gap_dev_prop_t *prop = &disc_res->prop[i];
        if (prop->type != ESP_BT_GAP_DEV_PROP_EIR) {
            GAP_DBG_PRINTF(", %s: ", gap_bt_prop_type_names[prop->type]);
        }
        if (prop->type == ESP_BT_GAP_DEV_PROP_BDNAME) {
            name = (uint8_t *)prop->val;
            name_len = strlen((const char *)name);
            GAP_DBG_PRINTF("%s", (const char *)name);
        } else if (prop->type == ESP_BT_GAP_DEV_PROP_RSSI) {
            rssi = *((int8_t *)prop->val);
            GAP_DBG_PRINTF("%d", rssi);
        } else if (prop->type == ESP_BT_GAP_DEV_PROP_COD) {
            memcpy(&codv, prop->val, sizeof(uint32_t));
            GAP_DBG_PRINTF("major: %s, minor: %d, service: 0x%03x", esp_hid_cod_major_str(cod->major), cod->minor, cod->service);
        } else if (prop->type == ESP_BT_GAP_DEV_PROP_EIR) {
            uint8_t len = 0;
            uint8_t *data = 0;

            data = esp_bt_gap_resolve_eir_data((uint8_t *)prop->val, ESP_BT_EIR_TYPE_CMPL_16BITS_UUID, &len);
            if (data == NULL) {
                data = esp_bt_gap_resolve_eir_data((uint8_t *)prop->val, ESP_BT_EIR_TYPE_INCMPL_16BITS_UUID, &len);
            }
            if (data && len == ESP_UUID_LEN_16) {
                uuid.len = ESP_UUID_LEN_16;
                uuid.uuid.uuid16 = data[0] + (data[1] << 8);
                GAP_DBG_PRINTF(", "); print_uuid(&uuid);
                continue;
            }

            data = esp_bt_gap_resolve_eir_data((uint8_t *)prop->val, ESP_BT_EIR_TYPE_CMPL_32BITS_UUID, &len);
            if (data == NULL) {
                data = esp_bt_gap_resolve_eir_data((uint8_t *)prop->val, ESP_BT_EIR_TYPE_INCMPL_32BITS_UUID, &len);
            }
            if (data && len == ESP_UUID_LEN_32) {
                uuid.len = len;
                memcpy(&uuid.uuid.uuid32, data, sizeof(uint32_t));
                GAP_DBG_PRINTF(", "); print_uuid(&uuid);
                continue;
            }

            data = esp_bt_gap_resolve_eir_data((uint8_t *)prop->val, ESP_BT_EIR_TYPE_CMPL_128BITS_UUID, &len);
            if (data == NULL) {
                data = esp_bt_gap_resolve_eir_data((uint8_t *)prop->val, ESP_BT_EIR_TYPE_INCMPL_128BITS_UUID, &len);
            }
            if (data && len == ESP_UUID_LEN_128) {
                uuid.len = len;
                memcpy(uuid.uuid.uuid128, (uint8_t *)data, len);
                GAP_DBG_PRINTF(", "); print_uuid(&uuid);
                continue;
            }

            //try to find a name
            if (name == NULL) {
                data = esp_bt_gap_resolve_eir_data((uint8_t *)prop->val, ESP_BT_EIR_TYPE_CMPL_LOCAL_NAME, &len);
                if (data == NULL) {
                    data = esp_bt_gap_resolve_eir_data((uint8_t *)prop->val, ESP_BT_EIR_TYPE_SHORT_LOCAL_NAME, &len);
                }
                if (data && len) {
                    name = data;
                    name_len = len;
                    GAP_DBG_PRINTF(", NAME: ");
                    for (int x = 0; x < len; x++) {
                        GAP_DBG_PRINTF("%c", (char)data[x]);
                    }
                }
            }
        }
    }
    GAP_DBG_PRINTF("\n");

    if (cod->major == ESP_BT_COD_MAJOR_DEV_PERIPHERAL || (find_scan_result(disc_res->bda, bt_scan_results) != NULL)) {
        add_bt_scan_result(disc_res->bda, cod, &uuid, name, name_len, rssi);
    }
}

static void handle_ble_device_result(struct ble_scan_result_evt_param *scan_rst)
{

    uint16_t uuid = 0;
    uint16_t appearance = 0;
    char name[64] = {0};

    uint8_t uuid_len = 0;
    uint8_t *uuid_d = esp_ble_resolve_adv_data(scan_rst->ble_adv, ESP_BLE_AD_TYPE_16SRV_CMPL, &uuid_len);
    if (uuid_d != NULL && uuid_len) {
        uuid = uuid_d[0] + (uuid_d[1] << 8);
    }

    uint8_t appearance_len = 0;
    uint8_t *appearance_d = esp_ble_resolve_adv_data(scan_rst->ble_adv, ESP_BLE_AD_TYPE_APPEARANCE, &appearance_len);
    if (appearance_d != NULL && appearance_len) {
        appearance = appearance_d[0] + (appearance_d[1] << 8);
    }

    uint8_t adv_name_len = 0;
    uint8_t *adv_name = esp_ble_resolve_adv_data(scan_rst->ble_adv, ESP_BLE_AD_TYPE_NAME_CMPL, &adv_name_len);

    if (adv_name == NULL) {
        adv_name = esp_ble_resolve_adv_data(scan_rst->ble_adv, ESP_BLE_AD_TYPE_NAME_SHORT, &adv_name_len);
    }

    if (adv_name != NULL && adv_name_len) {
        memcpy(name, adv_name, adv_name_len);
        name[adv_name_len] = 0;
    }

    GAP_DBG_PRINTF("BLE: " ESP_BD_ADDR_STR ", ", ESP_BD_ADDR_HEX(scan_rst->bda));
    GAP_DBG_PRINTF("RSSI: %d, ", scan_rst->rssi);
    GAP_DBG_PRINTF("UUID: 0x%04x, ", uuid);
    GAP_DBG_PRINTF("APPEARANCE: 0x%04x, ", appearance);
    GAP_DBG_PRINTF("ADDR_TYPE: '%s'", ble_addr_type_str(scan_rst->ble_addr_type));
    if (adv_name_len) {
        GAP_DBG_PRINTF(", NAME: '%s'", name);
    }
    GAP_DBG_PRINTF("\n");

    if (uuid == ESP_GATT_UUID_HID_SVC) {
        add_ble_scan_result(scan_rst->bda, scan_rst->ble_addr_type, appearance, adv_name, adv_name_len, scan_rst->rssi);
    }
}


/*
 * BT GAP
 * */

static void bt_gap_event_handler(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_BT_GAP_DISC_STATE_CHANGED_EVT: {
        ESP_LOGV(TAG, "BT GAP DISC_STATE %s", (param->disc_st_chg.state == ESP_BT_GAP_DISCOVERY_STARTED) ? "START" : "STOP");
        if (param->disc_st_chg.state == ESP_BT_GAP_DISCOVERY_STOPPED) {
            SEND_BT_CB();
        }
        break;
    }
    case ESP_BT_GAP_DISC_RES_EVT: {
        handle_bt_device_result(&param->disc_res);
        break;
    }
#if (CONFIG_BT_SSP_ENABLED)
    case ESP_BT_GAP_KEY_NOTIF_EVT:
        ESP_LOGI(TAG, "BT GAP KEY_NOTIF passkey:%d", param->key_notif.passkey);
        break;
    case ESP_BT_GAP_CFM_REQ_EVT: {
        ESP_LOGI(TAG, "ESP_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %d", param->cfm_req.num_val);
        esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;
    }
#endif
    case ESP_BT_GAP_PIN_REQ_EVT: {
        ESP_LOGI(TAG, "ESP_BT_GAP_PIN_REQ_EVT min_16_digit:%d", param->pin_req.min_16_digit);
        if (param->pin_req.min_16_digit) {
            ESP_LOGI(TAG, "Input pin code: 0000 0000 0000 0000");
            esp_bt_pin_code_t pin_code = {0};
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 16, pin_code);
        } else {
            ESP_LOGI(TAG, "Input pin code: 1234");
            esp_bt_pin_code_t pin_code;
            pin_code[0] = '1';
            pin_code[1] = '2';
            pin_code[2] = '3';
            pin_code[3] = '4';
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
        }
        break;
    }
    default:
        ESP_LOGW(TAG, "BT GAP EVENT %s", bt_gap_evt_str(event));
        break;
    }
}

static esp_err_t init_bt_gap(void)
{
    esp_err_t ret;
#if (CONFIG_BT_SSP_ENABLED)
    /* Set default parameters for Secure Simple Pairing */
    esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;
    esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
#endif
    /*
     * Set default parameters for Legacy Pairing
     * Use variable pin, input pin code when pairing
     */
    esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_VARIABLE;
    esp_bt_pin_code_t pin_code;
    esp_bt_gap_set_pin(pin_type, 0, pin_code);

    if ((ret = esp_bt_gap_register_callback(bt_gap_event_handler)) != ESP_OK) {
        ESP_LOGE(TAG, "esp_bt_gap_register_callback failed: %d", ret);
        return ret;
    }

    // Allow BT devices to connect back to us
    if ((ret = esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_NON_DISCOVERABLE)) != ESP_OK) {
        ESP_LOGE(TAG, "esp_bt_gap_set_scan_mode failed: %d", ret);
        return ret;
    }
    return ret;
}

static esp_err_t start_bt_scan(uint32_t seconds)
{
    esp_err_t ret = ESP_OK;
    if ((ret = esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, (int)(seconds / 1.28), 0)) != ESP_OK) {
        ESP_LOGE(TAG, "esp_bt_gap_start_discovery failed: %d", ret);
        return ret;
    }
    return ret;
}

/*
 * BLE GAP
 * */

static void ble_gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
    /*
     * SCAN
     * */
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT: {
        ESP_LOGV(TAG, "BLE GAP EVENT SCAN_PARAM_SET_COMPLETE");
        SEND_BLE_CB();
        break;
    }
    case ESP_GAP_BLE_SCAN_RESULT_EVT: {
        esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;
        switch (scan_result->scan_rst.search_evt) {
        case ESP_GAP_SEARCH_INQ_RES_EVT: {
            handle_ble_device_result(&scan_result->scan_rst);
            break;
        }
        case ESP_GAP_SEARCH_INQ_CMPL_EVT:
            ESP_LOGV(TAG, "BLE GAP EVENT SCAN DONE: %d", scan_result->scan_rst.num_resps);
            SEND_BLE_CB();
            break;
        default:
            break;
        }
        break;
    }
    case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT: {
        ESP_LOGV(TAG, "BLE GAP EVENT SCAN CANCELED");
        break;
    }

    /*
     * ADVERTISEMENT
     * */
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        ESP_LOGV(TAG, "BLE GAP ADV_DATA_SET_COMPLETE");
        break;

    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        ESP_LOGV(TAG, "BLE GAP ADV_START_COMPLETE");
        break;

    /*
     * AUTHENTICATION
     * */
    case ESP_GAP_BLE_AUTH_CMPL_EVT:
        if (!param->ble_security.auth_cmpl.success) {
            ESP_LOGE(TAG, "BLE GAP AUTH ERROR: 0x%x", param->ble_security.auth_cmpl.fail_reason);
        } else {
            ESP_LOGI(TAG, "BLE GAP AUTH SUCCESS");
        }
        break;

    case ESP_GAP_BLE_KEY_EVT: //shows the ble key info share with peer device to the user.
        ESP_LOGI(TAG, "BLE GAP KEY type = %s", esp_ble_key_type_str(param->ble_security.ble_key.key_type));
        break;

    case ESP_GAP_BLE_PASSKEY_NOTIF_EVT: // ESP_IO_CAP_OUT
        // The app will receive this evt when the IO has Output capability and the peer device IO has Input capability.
        // Show the passkey number to the user to input it in the peer device.
        ESP_LOGI(TAG, "BLE GAP PASSKEY_NOTIF passkey:%d", param->ble_security.key_notif.passkey);
        break;

    case ESP_GAP_BLE_NC_REQ_EVT: // ESP_IO_CAP_IO
        // The app will receive this event when the IO has DisplayYesNO capability and the peer device IO also has DisplayYesNo capability.
        // show the passkey number to the user to confirm it with the number displayed by peer device.
        ESP_LOGI(TAG, "BLE GAP NC_REQ passkey:%d", param->ble_security.key_notif.passkey);
        esp_ble_confirm_reply(param->ble_security.key_notif.bd_addr, true);
        break;

    case ESP_GAP_BLE_PASSKEY_REQ_EVT: // ESP_IO_CAP_IN
        // The app will receive this evt when the IO has Input capability and the peer device IO has Output capability.
        // See the passkey number on the peer device and send it back.
        ESP_LOGI(TAG, "BLE GAP PASSKEY_REQ");
        //esp_ble_passkey_reply(param->ble_security.ble_req.bd_addr, true, 1234);
        break;

    case ESP_GAP_BLE_SEC_REQ_EVT:
        ESP_LOGI(TAG, "BLE GAP SEC_REQ");
        // Send the positive(true) security response to the peer device to accept the security request.
        // If not accept the security request, should send the security response with negative(false) accept value.
        esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);
        break;

    default:
        ESP_LOGV(TAG, "BLE GAP EVENT %s", ble_gap_evt_str(event));
        break;
    }
}

static esp_err_t init_ble_gap(void)
{
    esp_err_t ret;

    if ((ret = esp_ble_gap_register_callback(ble_gap_event_handler)) != ESP_OK) {
        ESP_LOGE(TAG, "esp_ble_gap_register_callback failed: %d", ret);
        return ret;
    }
    return ret;
}

static esp_ble_scan_params_t hid_scan_params = {
    .scan_type              = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval          = 0x50,
    .scan_window            = 0x30,
    .scan_duplicate         = BLE_SCAN_DUPLICATE_ENABLE,
};

static esp_err_t start_ble_scan(uint32_t seconds)
{
    esp_err_t ret = ESP_OK;
    if ((ret = esp_ble_gap_set_scan_params(&hid_scan_params)) != ESP_OK) {
        ESP_LOGE(TAG, "esp_ble_gap_set_scan_params failed: %d", ret);
        return ret;
    }
    WAIT_BLE_CB();

    if ((ret = esp_ble_gap_start_scanning(seconds)) != ESP_OK) {
        ESP_LOGE(TAG, "esp_ble_gap_start_scanning failed: %d", ret);
        return ret;
    }
    return ret;
}

esp_err_t esp_hid_ble_gap_adv_init(uint16_t appearance, const char *device_name)
{

    esp_err_t ret;

    const uint8_t hidd_service_uuid128[] = {
        0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x12, 0x18, 0x00, 0x00,
    };

    esp_ble_adv_data_t ble_adv_data = {
        .set_scan_rsp = false,
        .include_name = true,
        .include_txpower = true,
        .min_interval = 0x0006, //slave connection min interval, Time = min_interval * 1.25 msec
        .max_interval = 0x0010, //slave connection max interval, Time = max_interval * 1.25 msec
        .appearance = appearance,
        .manufacturer_len = 0,
        .p_manufacturer_data =  NULL,
        .service_data_len = 0,
        .p_service_data = NULL,
        .service_uuid_len = sizeof(hidd_service_uuid128),
        .p_service_uuid = (uint8_t *)hidd_service_uuid128,
        .flag = 0x6,
    };

    esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_SC_MITM_BOND;
    //esp_ble_io_cap_t iocap = ESP_IO_CAP_OUT;//you have to enter the key on the host
    //esp_ble_io_cap_t iocap = ESP_IO_CAP_IN;//you have to enter the key on the device
    esp_ble_io_cap_t iocap = ESP_IO_CAP_IO;//you have to agree that key matches on both
    //esp_ble_io_cap_t iocap = ESP_IO_CAP_NONE;//device is not capable of input or output, unsecure
    uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    uint8_t key_size = 16; //the key size should be 7~16 bytes
    uint32_t passkey = 1234;//ESP_IO_CAP_OUT

    if ((ret = esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, 1)) != ESP_OK) {
        ESP_LOGE(TAG, "GAP set_security_param AUTHEN_REQ_MODE failed: %d", ret);
        return ret;
    }

    if ((ret = esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, 1)) != ESP_OK) {
        ESP_LOGE(TAG, "GAP set_security_param IOCAP_MODE failed: %d", ret);
        return ret;
    }

    if ((ret = esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, 1)) != ESP_OK) {
        ESP_LOGE(TAG, "GAP set_security_param SET_INIT_KEY failed: %d", ret);
        return ret;
    }

    if ((ret = esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, 1)) != ESP_OK) {
        ESP_LOGE(TAG, "GAP set_security_param SET_RSP_KEY failed: %d", ret);
        return ret;
    }

    if ((ret = esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, 1)) != ESP_OK) {
        ESP_LOGE(TAG, "GAP set_security_param MAX_KEY_SIZE failed: %d", ret);
        return ret;
    }

    if ((ret = esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey, sizeof(uint32_t))) != ESP_OK) {
        ESP_LOGE(TAG, "GAP set_security_param SET_STATIC_PASSKEY failed: %d", ret);
        return ret;
    }

    if ((ret = esp_ble_gap_set_device_name(device_name)) != ESP_OK) {
        ESP_LOGE(TAG, "GAP set_device_name failed: %d", ret);
        return ret;
    }

    if ((ret = esp_ble_gap_config_adv_data(&ble_adv_data)) != ESP_OK) {
        ESP_LOGE(TAG, "GAP config_adv_data failed: %d", ret);
        return ret;
    }

    return ret;
}

esp_err_t esp_hid_ble_gap_adv_start(void)
{
    static esp_ble_adv_params_t hidd_adv_params = {
        .adv_int_min        = 0x20,
        .adv_int_max        = 0x30,
        .adv_type           = ADV_TYPE_IND,
        .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
        .channel_map        = ADV_CHNL_ALL,
        .adv_filter_policy  = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
    };
    return esp_ble_gap_start_advertising(&hidd_adv_params);
}

/*
 * CONTROLLER INIT
 * */

static esp_err_t init_low_level(uint8_t mode)
{
    esp_err_t ret;
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if (mode & ESP_BT_MODE_CLASSIC_BT) {
        bt_cfg.mode = mode;
        bt_cfg.bt_max_acl_conn = 3;
        bt_cfg.bt_max_sync_conn = 3;
    } else {
        ret = esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
        if (ret) {
            ESP_LOGE(TAG, "esp_bt_controller_mem_release failed: %d", ret);
            return ret;
        }
    }
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(TAG, "esp_bt_controller_init failed: %d", ret);
        return ret;
    }

    ret = esp_bt_controller_enable(mode);
    if (ret) {
        ESP_LOGE(TAG, "esp_bt_controller_enable failed: %d", ret);
        return ret;
    }

    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(TAG, "esp_bluedroid_init failed: %d", ret);
        return ret;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(TAG, "esp_bluedroid_enable failed: %d", ret);
        return ret;
    }

    if (mode & ESP_BT_MODE_CLASSIC_BT) {
        ret = init_bt_gap();
        if (ret) {
            return ret;
        }
    }

    if (mode & ESP_BT_MODE_BLE) {
        ret = init_ble_gap();
        if (ret) {
            return ret;
        }
    }
    return ret;
}




esp_err_t esp_hid_gap_init(uint8_t mode)
{
    esp_err_t ret;
    if (!mode || mode > ESP_BT_MODE_BTDM) {
        ESP_LOGE(TAG, "Invalid mode given!");
        return ESP_FAIL;
    }

    if (bt_hidh_cb_semaphore != NULL) {
        ESP_LOGE(TAG, "Already initialised");
        return ESP_FAIL;
    }

    bt_hidh_cb_semaphore = xSemaphoreCreateBinary();
    if (bt_hidh_cb_semaphore == NULL) {
        ESP_LOGE(TAG, "xSemaphoreCreateMutex failed!");
        return ESP_FAIL;
    }

    ble_hidh_cb_semaphore = xSemaphoreCreateBinary();
    if (ble_hidh_cb_semaphore == NULL) {
        ESP_LOGE(TAG, "xSemaphoreCreateMutex failed!");
        vSemaphoreDelete(bt_hidh_cb_semaphore);
        bt_hidh_cb_semaphore = NULL;
        return ESP_FAIL;
    }

    ret = init_low_level(mode);
    if (ret != ESP_OK) {
        vSemaphoreDelete(bt_hidh_cb_semaphore);
        bt_hidh_cb_semaphore = NULL;
        vSemaphoreDelete(ble_hidh_cb_semaphore);
        ble_hidh_cb_semaphore = NULL;
        return ret;
    }

    return ESP_OK;
}

esp_err_t esp_hid_scan(uint32_t seconds, size_t *num_results, esp_hid_scan_result_t **results)
{
    if (num_bt_scan_results || bt_scan_results || num_ble_scan_results || ble_scan_results) {
        ESP_LOGE(TAG, "There are old scan results. Free them first!");
        return ESP_FAIL;
    }

    if (start_ble_scan(seconds) == ESP_OK) {
        if (start_bt_scan(seconds) == ESP_OK) {
            WAIT_BT_CB();
        }
        WAIT_BLE_CB();
    } else {
        return ESP_FAIL;
    }

    *num_results = num_bt_scan_results + num_ble_scan_results;
    *results = bt_scan_results;
    if (num_bt_scan_results) {
        while (bt_scan_results->next != NULL) {
            bt_scan_results = bt_scan_results->next;
        }
        bt_scan_results->next = ble_scan_results;
    } else {
        *results = ble_scan_results;
    }

    num_bt_scan_results = 0;
    bt_scan_results = NULL;
    num_ble_scan_results = 0;
    ble_scan_results = NULL;
    return ESP_OK;
}
