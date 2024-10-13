/* main.c - Application main entry point */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_generic_model_api.h"
#include "esp_ble_mesh_rpr_model_api.h"

#include "mesh/adapter.h"
#include "ble_mesh_example_init.h"

#include "board.h"
#define TAG "EXAMPLE"

#define CID_ESP             0x02E5
#define CID_NVAL            0xFFFF

#define PROV_OWN_ADDR       0x0001

#define MSG_SEND_TTL        3
#define MSG_TIMEOUT         0
#define MSG_ROLE            ROLE_PROVISIONER

#define COMP_DATA_PAGE_0    0x00

#define APP_KEY_IDX         0x0000
#define APP_KEY_OCTET       0x12

#define COMP_DATA_1_OCTET(msg, offset)      (msg[offset])
#define COMP_DATA_2_OCTET(msg, offset)      (msg[offset + 1] << 8 | msg[offset])

static uint8_t dev_uuid[16];

/* That uuid mask used to specify the unprovision device which should be provisioning by remote provisioning */
/* That uuid mask should be same as the dev_uuid in the ../node/main/main.c#L34 */
static uint8_t remote_dev_uuid_match[2] = {0x55, 0x55};
static uint16_t cur_rpr_cli_opcode;
uint8_t click_to_send_onoff_set = 0;
uint8_t message_tid = 0;
static uint16_t remote_rpr_srv_addr = 0;
typedef struct {
    uint8_t  uuid[16];
    uint16_t unicast;
    uint8_t  elem_num;
    uint8_t  onoff;
    uint8_t *sig_model_num;
    uint8_t *vnd_model_num;
    uint16_t **sig_models;
    uint32_t **vnd_models;
} esp_ble_mesh_node_info_t;

static esp_ble_mesh_node_info_t nodes[CONFIG_BLE_MESH_MAX_PROV_NODES] = {0};

static struct esp_ble_mesh_key {
    uint16_t net_idx;
    uint16_t app_idx;
    uint8_t  app_key[16];
} prov_key;

#if CONFIG_BLE_MESH_RPR_CLI
static esp_ble_mesh_client_t remote_prov_client;
#endif
static esp_ble_mesh_client_t config_client;
static esp_ble_mesh_client_t onoff_client;

static esp_ble_mesh_cfg_srv_t config_server = {
    /* 3 transmissions with 20ms interval */
    .net_transmit = ESP_BLE_MESH_TRANSMIT(2, 20),
    .relay = ESP_BLE_MESH_RELAY_DISABLED,
    .relay_retransmit = ESP_BLE_MESH_TRANSMIT(2, 20),
    .beacon = ESP_BLE_MESH_BEACON_ENABLED,
#if defined(CONFIG_BLE_MESH_GATT_PROXY_SERVER)
    .gatt_proxy = ESP_BLE_MESH_GATT_PROXY_ENABLED,
#else
    .gatt_proxy = ESP_BLE_MESH_GATT_PROXY_NOT_SUPPORTED,
#endif
#if defined(CONFIG_BLE_MESH_FRIEND)
    .friend_state = ESP_BLE_MESH_FRIEND_ENABLED,
#else
    .friend_state = ESP_BLE_MESH_FRIEND_NOT_SUPPORTED,
#endif
    .default_ttl = 7,
};

static esp_ble_mesh_model_t root_models[] = {
    ESP_BLE_MESH_MODEL_CFG_SRV(&config_server),
    ESP_BLE_MESH_MODEL_CFG_CLI(&config_client),
#if CONFIG_BLE_MESH_RPR_CLI
    ESP_BLE_MESH_MODEL_RPR_CLI(&remote_prov_client),
#endif
    ESP_BLE_MESH_MODEL_GEN_ONOFF_CLI(NULL, &onoff_client),
};

static esp_ble_mesh_elem_t elements[] = {
    ESP_BLE_MESH_ELEMENT(0, root_models, ESP_BLE_MESH_MODEL_NONE),
};

static esp_ble_mesh_comp_t composition = {
    .cid = CID_ESP,
    .element_count = ARRAY_SIZE(elements),
    .elements = elements,
};

static esp_ble_mesh_prov_t provision = {
    .prov_uuid           = dev_uuid,
    .prov_unicast_addr   = PROV_OWN_ADDR,
    .prov_start_address  = 0x0005,
    .prov_attention      = 0x00,
    .prov_algorithm      = 0x00,
    .prov_pub_key_oob    = 0x00,
    .prov_static_oob_val = NULL,
    .prov_static_oob_len = 0x00,
    .flags               = 0x00,
    .iv_index            = 0x00,
};

static esp_err_t example_ble_mesh_store_node_info(const uint8_t uuid[16], uint16_t unicast,
                                                  uint8_t elem_num, uint8_t onoff_state)
{
    int i;

    if (!uuid || !ESP_BLE_MESH_ADDR_IS_UNICAST(unicast)) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Judge if the device has been provisioned before */
    for (i = 0; i < ARRAY_SIZE(nodes); i++) {
        if (!memcmp(nodes[i].uuid, uuid, 16)) {
            ESP_LOGW(TAG, "%s: reprovisioned device 0x%04x", __func__, unicast);
            nodes[i].unicast = unicast;
            nodes[i].elem_num = elem_num;
            nodes[i].onoff = onoff_state;
            return ESP_OK;
        }
    }

    for (i = 0; i < ARRAY_SIZE(nodes); i++) {
        if (nodes[i].unicast == ESP_BLE_MESH_ADDR_UNASSIGNED) {
            memcpy(nodes[i].uuid, uuid, 16);
            nodes[i].unicast = unicast;
            nodes[i].elem_num = elem_num;
            nodes[i].onoff = onoff_state;
            return ESP_OK;
        }
    }

    return ESP_FAIL;
}

static esp_ble_mesh_node_info_t *example_ble_mesh_get_node_info(uint16_t unicast)
{
    int i;

    if (!ESP_BLE_MESH_ADDR_IS_UNICAST(unicast)) {
        return NULL;
    }

    for (i = 0; i < ARRAY_SIZE(nodes); i++) {
        if (nodes[i].unicast <= unicast &&
                nodes[i].unicast + nodes[i].elem_num > unicast) {
            return &nodes[i];
        }
    }

    return NULL;
}

static esp_err_t example_ble_mesh_set_msg_common(esp_ble_mesh_client_common_param_t *common,
                                                 uint16_t unicast,
                                                 esp_ble_mesh_model_t *model, uint32_t opcode)
{
    if (!common || !unicast || !model) {
        return ESP_ERR_INVALID_ARG;
    }

    common->opcode = opcode;
    common->model = model;
    common->ctx.net_idx = prov_key.net_idx;
    common->ctx.app_idx = prov_key.app_idx;
    common->ctx.addr = unicast;
    common->ctx.send_ttl = MSG_SEND_TTL;
    common->msg_timeout = MSG_TIMEOUT;

    return ESP_OK;
}

static esp_err_t prov_complete(int node_idx, const esp_ble_mesh_octet16_t uuid,
                               uint16_t unicast, uint8_t elem_num, uint16_t net_idx)
{
    esp_ble_mesh_client_common_param_t common = {0};
    esp_ble_mesh_cfg_client_get_state_t get_state = {0};
    esp_ble_mesh_node_info_t *node = NULL;
    char name[11] = {0};
    int err;

    ESP_LOGI(TAG, "node index: 0x%x, unicast address: 0x%02x, element num: %d, netkey index: 0x%02x",
             node_idx, unicast, elem_num, net_idx);
    ESP_LOGI(TAG, "device uuid: %s", bt_hex(uuid, 16));

    sprintf(name, "%s%d", "NODE-", node_idx);
    err = esp_ble_mesh_provisioner_set_node_name(node_idx, name);
    if (err) {
        ESP_LOGE(TAG, "%s: Set node name failed", __func__);
        return ESP_FAIL;
    }

    err = example_ble_mesh_store_node_info(uuid, unicast, elem_num, LED_OFF);
    if (err) {
        ESP_LOGE(TAG, "%s: Store node info failed", __func__);
        return ESP_FAIL;
    }

    node = example_ble_mesh_get_node_info(unicast);
    if (!node) {
        ESP_LOGE(TAG, "%s: Get node info failed", __func__);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Provisioning node by common method");
    ESP_LOGI(TAG, "That node will be act as remote provisioning server to help Provisioner to provisioning another node");

    example_ble_mesh_set_msg_common(&common, unicast, config_client.model, ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET);
    get_state.comp_data_get.page = COMP_DATA_PAGE_0;
    err = esp_ble_mesh_config_client_get_state(&common, &get_state);
    if (err) {
        ESP_LOGE(TAG, "%s: Send config comp data get failed", __func__);
        return ESP_FAIL;
    }

    return ESP_OK;
}

static void prov_link_open(esp_ble_mesh_prov_bearer_t bearer)
{
    ESP_LOGI(TAG, "%s link open", bearer == ESP_BLE_MESH_PROV_ADV ? "PB-ADV" : "PB-GATT");
    board_led_operation(LED_OFF, LED_ON, LED_OFF);
}

static void prov_link_close(esp_ble_mesh_prov_bearer_t bearer, uint8_t reason)
{
    ESP_LOGI(TAG, "%s link close, reason 0x%02x",
             bearer == ESP_BLE_MESH_PROV_ADV ? "PB-ADV" : "PB-GATT", reason);
}

static void recv_unprov_adv_pkt(uint8_t dev_uuid[16], uint8_t addr[BD_ADDR_LEN],
                                esp_ble_mesh_addr_type_t addr_type, uint16_t oob_info,
                                uint8_t adv_type, esp_ble_mesh_prov_bearer_t bearer)
{
    esp_ble_mesh_unprov_dev_add_t add_dev = {0};
    int err;

    /* Due to the API esp_ble_mesh_provisioner_set_dev_uuid_match, Provisioner will only
     * use this callback to report the devices, whose device UUID starts with 0xdd & 0xdd,
     * to the application layer.
     */

    ESP_LOGI(TAG, "address: %s, address type: %d, adv type: %d", bt_hex(addr, BD_ADDR_LEN), addr_type, adv_type);
    ESP_LOGI(TAG, "device uuid: %s", bt_hex(dev_uuid, 16));
    ESP_LOGI(TAG, "oob info: %d, bearer: %s", oob_info, (bearer & ESP_BLE_MESH_PROV_ADV) ? "PB-ADV" : "PB-GATT");

    memcpy(add_dev.addr, addr, BD_ADDR_LEN);
    add_dev.addr_type = (esp_ble_mesh_addr_type_t)addr_type;
    memcpy(add_dev.uuid, dev_uuid, 16);
    add_dev.oob_info = oob_info;
    add_dev.bearer = (esp_ble_mesh_prov_bearer_t)bearer;
    /* Note: If unprovisioned device adv packets have not been received, we should not add
             device with ADD_DEV_START_PROV_NOW_FLAG set. */
    err = esp_ble_mesh_provisioner_add_unprov_dev(&add_dev,
            ADD_DEV_RM_AFTER_PROV_FLAG | ADD_DEV_START_PROV_NOW_FLAG | ADD_DEV_FLUSHABLE_DEV_FLAG);
    if (err) {
        ESP_LOGE(TAG, "%s: Add unprovisioned device into queue failed", __func__);
    }

    return;
}

void example_ble_mesh_send_gen_onoff_set(uint8_t onoff)
{
    esp_ble_mesh_generic_client_set_state_t set = {0};
    esp_ble_mesh_client_common_param_t common = {0};
    esp_err_t err = ESP_OK;

    example_ble_mesh_set_msg_common(&common, 0xffff, onoff_client.model, ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK);

    set.onoff_set.op_en = false;
    set.onoff_set.onoff = onoff;
    set.onoff_set.tid = message_tid++;

    err = esp_ble_mesh_generic_client_set_state(&common, &set);
    if (err) {
        ESP_LOGE(TAG, "Send Generic OnOff Set Unack failed");
        return;
    }
}

void example_ble_mesh_send_remote_provisioning_scan_start(void)
{

    esp_ble_mesh_client_common_param_t common = {0};
    esp_err_t err = ESP_OK;

    if (!remote_rpr_srv_addr) {
        ESP_LOGE(TAG, "No valid remote provisioning server address");
        return;
    }

    /* Send a ESP_BLE_MESH_MODEL_OP_RPR_SCAN_GET to get the scan status of remote provisioning server */
    example_ble_mesh_set_msg_common(&common, remote_rpr_srv_addr, remote_prov_client.model, ESP_BLE_MESH_MODEL_OP_RPR_SCAN_GET);
    err = esp_ble_mesh_rpr_client_send(&common, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to send Remote Provisioning Client msg: Scan Get");
    }
    cur_rpr_cli_opcode = ESP_BLE_MESH_MODEL_OP_RPR_SCAN_GET;
}


static void example_ble_mesh_parse_node_comp_data(esp_ble_mesh_node_info_t* node, const uint8_t *data, uint16_t length)
{
    uint16_t cid, pid, vid, crpl, feat;
    uint16_t loc, model_id, company_id;
    uint8_t nums, numv;
    uint16_t offset;
    uint8_t seq = 0;
    int i;

    if (!node || !data) {
        ESP_LOGE(TAG, "Invalid Argument");
        return;
    }

    cid = COMP_DATA_2_OCTET(data, 0);
    pid = COMP_DATA_2_OCTET(data, 2);
    vid = COMP_DATA_2_OCTET(data, 4);
    crpl = COMP_DATA_2_OCTET(data, 6);
    feat = COMP_DATA_2_OCTET(data, 8);
    offset = 10;

    node->sig_model_num = (uint8_t *)calloc(node->elem_num, sizeof(uint8_t));
    if (!node->sig_model_num) {
        ESP_LOGW(TAG, "No Free memory to store composition data");
        return;
    }

    node->vnd_model_num = (uint8_t *)calloc(node->elem_num, sizeof(uint8_t));
    if (!node->vnd_model_num) {
        ESP_LOGW(TAG, "No Free memory to store composition data");
        return;
    }

    node->sig_models = (uint16_t **)calloc(node->elem_num, sizeof(uint16_t*));
    if (!node->sig_models) {
        ESP_LOGW(TAG, "No Free memory to store composition data");
        return;
    }

    node->vnd_models = (uint32_t **)calloc(node->elem_num, sizeof(uint32_t*));
    if (!node->sig_models) {
        ESP_LOGW(TAG, "No Free memory to store composition data");
        return;
    }

    ESP_LOGI(TAG, "********************** Composition Data Start **********************");
    ESP_LOGI(TAG, "* CID 0x%04x, PID 0x%04x, VID 0x%04x, CRPL 0x%04x, Features 0x%04x *", cid, pid, vid, crpl, feat);
    for (; offset < length; ) {
        loc = COMP_DATA_2_OCTET(data, offset);
        nums = COMP_DATA_1_OCTET(data, offset + 2);
        numv = COMP_DATA_1_OCTET(data, offset + 3);
        node->sig_model_num[seq] = nums;
        node->vnd_model_num[seq] = numv;

        if (nums) {
            node->sig_models[seq] = (uint16_t *)calloc(nums, sizeof(uint16_t));
            if (!(node->sig_models[seq])) {
                ESP_LOGW(TAG, "No Free memory to store composition data");
                return;
            }
        } else {
            node->sig_models[seq] = NULL;
        }

        if (numv) {
            node->vnd_models[seq] = (uint32_t *)calloc(numv, sizeof(uint32_t));
            if (!(node->vnd_models[seq])) {
                ESP_LOGW(TAG, "No Free memory to store composition data");
                return;
            }
        } else {
            node->vnd_models[seq] = NULL;
        }

        offset += 4;
        ESP_LOGI(TAG, "* Loc 0x%04x, NumS 0x%02x, NumV 0x%02x *", loc, nums, numv);
        for (i = 0; i < nums; i++) {
            model_id = COMP_DATA_2_OCTET(data, offset);
            node->sig_models[seq][i] = model_id;
            ESP_LOGI(TAG, "* SIG Model ID 0x%04x *", model_id);
            offset += 2;
        }
        for (i = 0; i < numv; i++) {
            company_id = COMP_DATA_2_OCTET(data, offset);
            model_id = COMP_DATA_2_OCTET(data, offset + 2);
            node->vnd_models[seq][i] = company_id << 16 | model_id;
            ESP_LOGI(TAG, "* Vendor Model ID 0x%04x, Company ID 0x%04x *", model_id, company_id);
            offset += 4;
        }
        seq++;
    }
    ESP_LOGI(TAG, "*********************** Composition Data End ***********************");
}

static bool example_ble_mesh_query_element_have_model(uint16_t elem_addr, uint16_t model_id, uint16_t company_id)
{
    esp_ble_mesh_node_info_t *node = NULL;
    uint8_t elem_idx = 0;
    uint8_t model_num = 0;
    int i = 0;

    node = example_ble_mesh_get_node_info(elem_addr);

    elem_idx = elem_addr - node->unicast;

    if (company_id == CID_NVAL) {
        model_num = node->sig_model_num[elem_idx];
        for (i = 0; i < model_num; i++) {
            if (node->sig_models[elem_idx][i] == model_id) {
                return true;
            }
        }
    } else {
        model_num = node->vnd_model_num[elem_idx];
        for (i = 0; i < model_num; i++) {
            if (node->vnd_models[elem_idx][i] == (company_id << 16 | model_id)) {
                return true;
            }
        }
    }

    return false;
}

static void example_ble_mesh_provisioning_cb(esp_ble_mesh_prov_cb_event_t event,
                                             esp_ble_mesh_prov_cb_param_t *param)
{
    switch (event) {
    case ESP_BLE_MESH_PROVISIONER_PROV_ENABLE_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_PROV_ENABLE_COMP_EVT, err_code %d", param->provisioner_prov_enable_comp.err_code);
        break;
    case ESP_BLE_MESH_PROVISIONER_PROV_DISABLE_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_PROV_DISABLE_COMP_EVT, err_code %d", param->provisioner_prov_disable_comp.err_code);
        break;
    case ESP_BLE_MESH_PROVISIONER_RECV_UNPROV_ADV_PKT_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_RECV_UNPROV_ADV_PKT_EVT");
        recv_unprov_adv_pkt(param->provisioner_recv_unprov_adv_pkt.dev_uuid, param->provisioner_recv_unprov_adv_pkt.addr,
                            param->provisioner_recv_unprov_adv_pkt.addr_type, param->provisioner_recv_unprov_adv_pkt.oob_info,
                            param->provisioner_recv_unprov_adv_pkt.adv_type, param->provisioner_recv_unprov_adv_pkt.bearer);
        break;
    case ESP_BLE_MESH_PROVISIONER_PROV_LINK_OPEN_EVT:
        prov_link_open(param->provisioner_prov_link_open.bearer);
        break;
    case ESP_BLE_MESH_PROVISIONER_PROV_LINK_CLOSE_EVT:
        prov_link_close(param->provisioner_prov_link_close.bearer, param->provisioner_prov_link_close.reason);
        break;
    case ESP_BLE_MESH_PROVISIONER_PROV_COMPLETE_EVT:
        prov_complete(param->provisioner_prov_complete.node_idx, param->provisioner_prov_complete.device_uuid,
                      param->provisioner_prov_complete.unicast_addr, param->provisioner_prov_complete.element_num,
                      param->provisioner_prov_complete.netkey_idx);
        break;
    case ESP_BLE_MESH_PROVISIONER_ADD_UNPROV_DEV_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_ADD_UNPROV_DEV_COMP_EVT, err_code %d", param->provisioner_add_unprov_dev_comp.err_code);
        break;
    case ESP_BLE_MESH_PROVISIONER_SET_DEV_UUID_MATCH_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_SET_DEV_UUID_MATCH_COMP_EVT, err_code %d", param->provisioner_set_dev_uuid_match_comp.err_code);
        break;
    case ESP_BLE_MESH_PROVISIONER_SET_NODE_NAME_COMP_EVT: {
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_SET_NODE_NAME_COMP_EVT, err_code %d", param->provisioner_set_node_name_comp.err_code);
        if (param->provisioner_set_node_name_comp.err_code == ESP_OK) {
            const char *name = NULL;
            name = esp_ble_mesh_provisioner_get_node_name(param->provisioner_set_node_name_comp.node_index);
            if (!name) {
                ESP_LOGE(TAG, "Get node name failed");
                return;
            }
            ESP_LOGI(TAG, "Node %d name is: %s", param->provisioner_set_node_name_comp.node_index, name);
        }
        break;
    }
    case ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_APP_KEY_COMP_EVT: {
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_APP_KEY_COMP_EVT, err_code %d", param->provisioner_add_app_key_comp.err_code);
        if (param->provisioner_add_app_key_comp.err_code == ESP_OK) {
            esp_err_t err = 0;
            prov_key.app_idx = param->provisioner_add_app_key_comp.app_idx;
            err = esp_ble_mesh_provisioner_bind_app_key_to_local_model(PROV_OWN_ADDR, prov_key.app_idx,
                    ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_CLI, ESP_BLE_MESH_CID_NVAL);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Provisioner bind local model appkey failed");
                return;
            }
        }
        break;
    }
    case ESP_BLE_MESH_PROVISIONER_BIND_APP_KEY_TO_MODEL_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_BIND_APP_KEY_TO_MODEL_COMP_EVT, err_code %d", param->provisioner_bind_app_key_to_model_comp.err_code);
        break;
    default:
        break;
    }

    return;
}

static void example_ble_mesh_config_client_cb(esp_ble_mesh_cfg_client_cb_event_t event,
                                              esp_ble_mesh_cfg_client_cb_param_t *param)
{
    esp_ble_mesh_client_common_param_t common = {0};
    esp_ble_mesh_node_info_t *node = NULL;
    uint32_t opcode;
    uint16_t addr;
    int err;

    opcode = param->params->opcode;
    addr = param->params->ctx.addr;

    ESP_LOGI(TAG, "%s, error_code = 0x%02x, event = 0x%02x, addr: 0x%04x, opcode: 0x%04" PRIx32,
             __func__, param->error_code, event, param->params->ctx.addr, opcode);

    if (param->error_code) {
        ESP_LOGE(TAG, "Send config client message failed, opcode 0x%04" PRIx32, opcode);
        return;
    }

    node = example_ble_mesh_get_node_info(addr);
    if (!node) {
        ESP_LOGE(TAG, "%s: Get node info failed", __func__);
        return;
    }

    switch (event) {
    case ESP_BLE_MESH_CFG_CLIENT_GET_STATE_EVT:
        switch (opcode) {
        case ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET: {
            ESP_LOGI(TAG, "composition data %s", bt_hex(param->status_cb.comp_data_status.composition_data->data,
                     param->status_cb.comp_data_status.composition_data->len));
            example_ble_mesh_parse_node_comp_data(node, param->status_cb.comp_data_status.composition_data->data,
                                                        param->status_cb.comp_data_status.composition_data->len);
            esp_ble_mesh_cfg_client_set_state_t set_state = {0};
            example_ble_mesh_set_msg_common(&common, addr, config_client.model, ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD);
            set_state.app_key_add.net_idx = prov_key.net_idx;
            set_state.app_key_add.app_idx = prov_key.app_idx;
            memcpy(set_state.app_key_add.app_key, prov_key.app_key, 16);
            err = esp_ble_mesh_config_client_set_state(&common, &set_state);
            if (err) {
                ESP_LOGE(TAG, "%s: Config AppKey Add failed", __func__);
                return;
            }
            break;
        }
        default:
            break;
        }
        break;
    case ESP_BLE_MESH_CFG_CLIENT_SET_STATE_EVT:
        switch (opcode) {
        case ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD: {
            esp_ble_mesh_cfg_client_set_state_t set_state = {0};

            if (!example_ble_mesh_query_element_have_model(addr, ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV, CID_NVAL)) {
                ESP_LOGE(TAG, "Element (addr: 0x%04x) does not support onoff srv model");
                return;
            }

            example_ble_mesh_set_msg_common(&common, addr, config_client.model, ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND);
            set_state.model_app_bind.element_addr = node->unicast;
            set_state.model_app_bind.model_app_idx = prov_key.app_idx;
            set_state.model_app_bind.model_id = ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV;
            set_state.model_app_bind.company_id = ESP_BLE_MESH_CID_NVAL;
            err = esp_ble_mesh_config_client_set_state(&common, &set_state);
            if (err) {
                ESP_LOGE(TAG, "%s: Config Model App Bind failed", __func__);
                return;
            }
            break;
        }
        case ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND: {

            if (!example_ble_mesh_query_element_have_model(addr, ESP_BLE_MESH_MODEL_ID_RPR_SRV, CID_NVAL)) {
                if (remote_rpr_srv_addr) {
                    ESP_LOGI(TAG, "The last node have been provisioned, You could click button to send Generic Onoff Set");
                    click_to_send_onoff_set = 1;
                } else {
                    ESP_LOGE(TAG, "Element (addr: 0x%04x) does not support remote provisioning srv model");
                }
            } else {
                click_to_send_onoff_set = 0;
                ESP_LOGI(TAG, "The Remote Provisioning Server have been provisioned, You could click button to start remote provisioning");
                remote_rpr_srv_addr = addr;
            }
            board_led_operation(LED_OFF, LED_OFF, LED_OFF);
            break;
        }
        default:
            break;
        }
        break;
    case ESP_BLE_MESH_CFG_CLIENT_PUBLISH_EVT:
        switch (opcode) {
        case ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_STATUS:
            ESP_LOG_BUFFER_HEX("composition data %s", param->status_cb.comp_data_status.composition_data->data,
                               param->status_cb.comp_data_status.composition_data->len);
            break;
        case ESP_BLE_MESH_MODEL_OP_APP_KEY_STATUS:
            break;
        default:
            break;
        }
        break;
    case ESP_BLE_MESH_CFG_CLIENT_TIMEOUT_EVT:
        switch (opcode) {
        case ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET: {
            esp_ble_mesh_cfg_client_get_state_t get_state = {0};
            example_ble_mesh_set_msg_common(&common, addr, config_client.model, ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET);
            get_state.comp_data_get.page = COMP_DATA_PAGE_0;
            err = esp_ble_mesh_config_client_get_state(&common, &get_state);
            if (err) {
                ESP_LOGE(TAG, "%s: Config Composition Data Get failed", __func__);
                return;
            }
            break;
        }
        case ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD: {
            esp_ble_mesh_cfg_client_set_state_t set_state = {0};
            example_ble_mesh_set_msg_common(&common, addr, config_client.model, ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD);
            set_state.app_key_add.net_idx = prov_key.net_idx;
            set_state.app_key_add.app_idx = prov_key.app_idx;
            memcpy(set_state.app_key_add.app_key, prov_key.app_key, 16);
            err = esp_ble_mesh_config_client_set_state(&common, &set_state);
            if (err) {
                ESP_LOGE(TAG, "%s: Config AppKey Add failed", __func__);
                return;
            }
            break;
        }
        case ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND: {
            esp_ble_mesh_cfg_client_set_state_t set_state = {0};
            example_ble_mesh_set_msg_common(&common, addr, config_client.model, ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND);
            set_state.model_app_bind.element_addr = addr;
            set_state.model_app_bind.model_app_idx = prov_key.app_idx;
            set_state.model_app_bind.model_id = ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV;
            set_state.model_app_bind.company_id = ESP_BLE_MESH_CID_NVAL;
            err = esp_ble_mesh_config_client_set_state(&common, &set_state);
            if (err) {
                ESP_LOGE(TAG, "%s: Config Model App Bind failed", __func__);
                return;
            }
            break;
        }
        default:
            break;
        }
        break;
    default:
        ESP_LOGE(TAG, "Not a config client status message event");
        break;
    }
}

static void example_ble_mesh_generic_client_cb(esp_ble_mesh_generic_client_cb_event_t event,
                                               esp_ble_mesh_generic_client_cb_param_t *param)
{
    esp_ble_mesh_client_common_param_t common = {0};
    esp_ble_mesh_node_info_t *node = NULL;
    uint32_t opcode;
    uint16_t addr;
    int err;

    opcode = param->params->opcode;
    addr = param->params->ctx.addr;

    ESP_LOGI(TAG, "%s, error_code = 0x%02x, event = 0x%02x, addr: 0x%04x, opcode: 0x%04" PRIx32,
             __func__, param->error_code, event, param->params->ctx.addr, opcode);

    if (param->error_code) {
        ESP_LOGE(TAG, "Send generic client message failed, opcode 0x%04" PRIx32, opcode);
        return;
    }

    node = example_ble_mesh_get_node_info(addr);
    if (!node) {
        ESP_LOGE(TAG, "%s: Get node info failed", __func__);
        return;
    }

    switch (event) {
    case ESP_BLE_MESH_GENERIC_CLIENT_GET_STATE_EVT:
        switch (opcode) {
        case ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET: {
            esp_ble_mesh_generic_client_set_state_t set_state = {0};
            node->onoff = param->status_cb.onoff_status.present_onoff;
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET onoff: 0x%02x", node->onoff);
            /* After Generic OnOff Status for Generic OnOff Get is received, Generic OnOff Set will be sent */
            example_ble_mesh_set_msg_common(&common, addr, onoff_client.model, ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET);
            set_state.onoff_set.op_en = false;
            set_state.onoff_set.onoff = !node->onoff;
            set_state.onoff_set.tid = 0;
            err = esp_ble_mesh_generic_client_set_state(&common, &set_state);
            if (err) {
                ESP_LOGE(TAG, "%s: Generic OnOff Set failed", __func__);
                return;
            }
            break;
        }
        default:
            break;
        }
        break;
    case ESP_BLE_MESH_GENERIC_CLIENT_SET_STATE_EVT:
        switch (opcode) {
        case ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET:
            node->onoff = param->status_cb.onoff_status.present_onoff;
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET onoff: 0x%02x", node->onoff);
            break;
        default:
            break;
        }
        break;
    case ESP_BLE_MESH_GENERIC_CLIENT_PUBLISH_EVT:
        break;
    case ESP_BLE_MESH_GENERIC_CLIENT_TIMEOUT_EVT:
        /* If failed to receive the responses, these messages will be resend */
        switch (opcode) {
        case ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET: {
            esp_ble_mesh_generic_client_get_state_t get_state = {0};
            example_ble_mesh_set_msg_common(&common, addr, onoff_client.model, ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET);
            err = esp_ble_mesh_generic_client_get_state(&common, &get_state);
            if (err) {
                ESP_LOGE(TAG, "%s: Generic OnOff Get failed", __func__);
                return;
            }
            break;
        }
        case ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET: {
            esp_ble_mesh_generic_client_set_state_t set_state = {0};
            node->onoff = param->status_cb.onoff_status.present_onoff;
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET onoff: 0x%02x", node->onoff);
            example_ble_mesh_set_msg_common(&common, addr, onoff_client.model, ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET);
            set_state.onoff_set.op_en = false;
            set_state.onoff_set.onoff = !node->onoff;
            set_state.onoff_set.tid = 0;
            err = esp_ble_mesh_generic_client_set_state(&common, &set_state);
            if (err) {
                ESP_LOGE(TAG, "%s: Generic OnOff Set failed", __func__);
                return;
            }
            break;
        }
        default:
            break;
        }
        break;
    default:
        ESP_LOGE(TAG, "Not a generic client status message event");
        break;
    }
}

static void example_ble_mesh_remote_prov_client_callback(esp_ble_mesh_rpr_client_cb_event_t event,
                                        esp_ble_mesh_rpr_client_cb_param_t *param)
{
    static uint8_t remote_dev_uuid[16] = {0};
    esp_ble_mesh_rpr_client_msg_t msg = {0};
    esp_ble_mesh_client_common_param_t common = {0};
    esp_err_t err = ESP_OK;
    uint16_t addr = 0;

    switch (event) {
    case ESP_BLE_MESH_RPR_CLIENT_SEND_COMP_EVT:
        ESP_LOGW(TAG, "Remote Prov Client Send Comp, err_code %d", param->send.err_code);
        break;
    case ESP_BLE_MESH_RPR_CLIENT_SEND_TIMEOUT_EVT:
        ESP_LOGW(TAG, "Remote Prov Client Send Timeout, opcode 0x%04x, to 0x%04x",
                 param->send.params->opcode, param->send.params->ctx.addr);
        break;
    case ESP_BLE_MESH_RPR_CLIENT_RECV_PUB_EVT:
    case ESP_BLE_MESH_RPR_CLIENT_RECV_RSP_EVT:
        ESP_LOGW(TAG, "Remote Prov Client Recv RSP, opcode 0x%04x, from 0x%04x",
                 param->recv.params->ctx.recv_op, param->recv.params->ctx.addr);
        switch (param->recv.params->ctx.recv_op) {
        case ESP_BLE_MESH_MODEL_OP_RPR_SCAN_CAPS_STATUS:
            break;
        case ESP_BLE_MESH_MODEL_OP_RPR_SCAN_STATUS:
            addr = param->recv.params->ctx.addr;
            ESP_LOGI(TAG, "scan_status, status 0x%02x", param->recv.val.scan_status.status);
            ESP_LOGI(TAG, "scan_status, rpr_scanning 0x%02x", param->recv.val.scan_status.rpr_scanning);
            ESP_LOGI(TAG, "scan_status, scan_items_limit 0x%02x", param->recv.val.scan_status.scan_items_limit);
            ESP_LOGI(TAG, "scan_status, timeout 0x%02x", param->recv.val.scan_status.timeout);
            switch (cur_rpr_cli_opcode) {
            case ESP_BLE_MESH_MODEL_OP_RPR_SCAN_GET: {
                if (param->recv.val.scan_status.status == ESP_BLE_MESH_RPR_STATUS_SUCCESS) {
                    switch (param->recv.val.scan_status.rpr_scanning) {
                        /**
                         *  If the remote provisioning server's scan state is idle,
                         *  that state indicates that remote provisioning server could
                         *  start scan process.
                         */
                        case ESP_BLE_MESH_RPR_SCAN_IDLE: {
                            err = example_ble_mesh_set_msg_common(&common, addr, remote_prov_client.model,
                                                                    ESP_BLE_MESH_MODEL_OP_RPR_SCAN_START);
                            if (err != ESP_OK) {
                                ESP_LOGE(TAG, "Set message common fail:%d", __LINE__);
                                return ;
                            }

                            msg.scan_start.scan_items_limit = 0; /* 0 indicates there is no limit for scan items' count */
                            msg.scan_start.timeout = 0x0A;       /* 0x0A is the default timeout */
                            msg.scan_start.uuid_en = 0;          /* If uuid enabled, a specify device which have the same uuid will be report */
                                                                 /* If uuid disable, any unprovision device all will be report */

                            err = esp_ble_mesh_rpr_client_send(&common, &msg);
                            if (err != ESP_OK) {
                                ESP_LOGE(TAG, "Failed to send Remote Provisioning Client msg: Scan start");
                            }
                            cur_rpr_cli_opcode = ESP_BLE_MESH_MODEL_OP_RPR_SCAN_START;
                            break;
                        }
                        default:
                            ESP_LOGW(TAG, "Remote Provisioning Server(addr: 0x%04x) Busy", addr);
                            break;
                    }
                } else {
                    ESP_LOGE(TAG, "Remote Provisioning Client Scan Get Fail");
                }
            }
                break;
            case ESP_BLE_MESH_MODEL_OP_RPR_SCAN_START: {
                if (param->recv.val.scan_status.status == ESP_BLE_MESH_RPR_STATUS_SUCCESS) {
                    ESP_LOGI(TAG, "Start Remote Provisioning Server(addr: 0x%04x) Scan Success", addr);
                } else {
                    ESP_LOGE(TAG, "Remote Provisioning Client Scan Start Fail");
                }
                break;
            }
            default:
                ESP_LOGW(TAG, "Unknown Process opcode 0x%04x:%d", cur_rpr_cli_opcode,__LINE__);
                break;
            }
            break;
        case ESP_BLE_MESH_MODEL_OP_RPR_SCAN_REPORT:
            addr = param->recv.params->ctx.addr;
            ESP_LOGI(TAG, "scan_report, rssi %ddBm", param->recv.val.scan_report.rssi);
            ESP_LOG_BUFFER_HEX(TAG": scan_report, uuid", param->recv.val.scan_report.uuid, 16);
            ESP_LOGI(TAG, "scan_report, oob_info 0x%04x", param->recv.val.scan_report.oob_info);
            ESP_LOGI(TAG, "scan_report, uri_hash 0x%08x", param->recv.val.scan_report.uri_hash);

            if (param->recv.val.scan_report.uuid[0] != remote_dev_uuid_match[0] ||
                param->recv.val.scan_report.uuid[1] != remote_dev_uuid_match[1]) {
                ESP_LOGI(TAG, "This device is not expect device");
                return;
            }

            memcpy(remote_dev_uuid, param->recv.val.scan_report.uuid, 16);

            /* Send ESP_BLE_MESH_MODEL_OP_RPR_LINK_GET to remote provisioning server get link status */
            err = example_ble_mesh_set_msg_common(&common, addr,remote_prov_client.model
                                        , ESP_BLE_MESH_MODEL_OP_RPR_LINK_GET);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Set message common fail:%d", __LINE__);
                return;
            }

            err = esp_ble_mesh_rpr_client_send(&common, NULL);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Failed to send Remote Provisioning Client msg:Link Get");
            }

            cur_rpr_cli_opcode = ESP_BLE_MESH_MODEL_OP_RPR_LINK_GET;
            break;
        case ESP_BLE_MESH_MODEL_OP_RPR_EXT_SCAN_REPORT:
            break;
        case ESP_BLE_MESH_MODEL_OP_RPR_LINK_STATUS:
            addr = param->recv.params->ctx.addr;
            ESP_LOGI(TAG, "link_status, status 0x%02x", param->recv.val.link_status.status);
            ESP_LOGI(TAG, "link_status, rpr_state 0x%02x", param->recv.val.link_status.rpr_state);
            switch (cur_rpr_cli_opcode) {
            case ESP_BLE_MESH_MODEL_OP_RPR_LINK_GET: {
                if (param->recv.val.link_status.status == ESP_BLE_MESH_RPR_STATUS_SUCCESS) {
                    switch (param->recv.val.link_status.rpr_state) {
                    case ESP_BLE_MESH_RPR_LINK_IDLE:
                        /**
                         *  Link status is idle, send ESP_BLE_MESH_MODEL_OP_RPR_LINK_OPEN
                         *  to remote provisioning server to open prov link
                         */
                        err = example_ble_mesh_set_msg_common(&common, addr, remote_prov_client.model
                                                    , ESP_BLE_MESH_MODEL_OP_RPR_LINK_OPEN);
                        if (err != ESP_OK) {
                            ESP_LOGE(TAG, "Set message common fail:%d", __LINE__);
                            return;
                        }

                        msg.link_open.uuid_en = 1;
                        memcpy(msg.link_open.uuid, remote_dev_uuid, 16);
                        msg.link_open.timeout_en = 0;

                        err = esp_ble_mesh_rpr_client_send(&common, &msg);
                        if (err != ESP_OK) {
                            ESP_LOGE(TAG, "Failed to send Remote Provisioning Client msg:Link open");
                        }
                        cur_rpr_cli_opcode = ESP_BLE_MESH_MODEL_OP_RPR_LINK_OPEN;
                        break;
                    default:
                        ESP_LOGW(TAG, "Remote Provisioning Server(addr: 0x%04x) Busy", addr);
                        break;
                    }
                }
                break;
            }
            case ESP_BLE_MESH_MODEL_OP_RPR_LINK_OPEN: {
                if (param->recv.val.link_status.status == ESP_BLE_MESH_RPR_STATUS_SUCCESS) {
                    ESP_LOGI(TAG, "Remote Provisioning Server(addr: 0x%04x) Recv Link Open Success", addr);
                } else {
                    ESP_LOGI(TAG, "Remote Provisioning Server(addr: 0x%04x) Recv Link Open Fail", addr);
                }
            }
                break;
            case ESP_BLE_MESH_MODEL_OP_RPR_LINK_CLOSE: {
                if (param->recv.val.link_status.status == ESP_BLE_MESH_RPR_STATUS_SUCCESS) {
                    ESP_LOGI(TAG, "Remote Provisioning Server(addr: 0x%04x) Recv Link Close Success", addr);
                } else {
                    ESP_LOGI(TAG, "Remote Provisioning Server(addr: 0x%04x) Recv Link Close Fail", addr);
                }
            }
                break;
            default:
                ESP_LOGW(TAG, "Unknown Process opcode 0x%04x:%d", cur_rpr_cli_opcode,__LINE__);
                break;
            }
            break;
        case ESP_BLE_MESH_MODEL_OP_RPR_LINK_REPORT:
            addr = param->recv.params->ctx.addr;
            ESP_LOGI(TAG, "link_report, status 0x%02x", param->recv.val.link_report.status);
            ESP_LOGI(TAG, "link_report, rpr_state 0x%02x", param->recv.val.link_report.rpr_state);
            if (param->recv.val.link_report.reason_en) {
                ESP_LOGI(TAG, "link_report, reason 0x%02x", param->recv.val.link_report.reason);
            }
            switch (cur_rpr_cli_opcode) {
            case ESP_BLE_MESH_MODEL_OP_RPR_LINK_OPEN:
                if (param->recv.val.link_report.status == ESP_BLE_MESH_RPR_STATUS_SUCCESS) {
                    switch (param->recv.val.link_report.rpr_state)
                    {
                    case ESP_BLE_MESH_RPR_LINK_ACTIVE: {
                        ESP_LOGI(TAG, "Remote Provisioning Server(addr: 0x%04x) Link Open Success", addr);
                        esp_ble_mesh_rpr_client_act_param_t param = {0};
                        param.start_rpr.model = remote_prov_client.model;
                        param.start_rpr.rpr_srv_addr = addr;

                        /* Let remote provisioning server start provisioning */
                        err = esp_ble_mesh_rpr_client_action(ESP_BLE_MESH_RPR_CLIENT_ACT_START_RPR,
                                                                        &param);
                        if (err) {
                            ESP_LOGE(TAG, "Failed to perform Remote Provisioning Client action: Start Prov");
                        }
                        board_led_operation(LED_OFF, LED_ON, LED_OFF);
                        break;
                    }
                    default:
                        ESP_LOGI(TAG, "Remote Provisioning Server(addr: 0x%04x) Status error", addr);
                        break;
                    }
                } else {
                    ESP_LOGW(TAG, "Remote Provisioning Server(addr: 0x%04x) Link open fail");
                }
                break;
            }
            break;
        case ESP_BLE_MESH_MODEL_OP_RPR_LINK_CLOSE:
            switch (param->recv.val.link_report.status)
            {
            case ESP_BLE_MESH_RPR_STATUS_LINK_CLOSED_BY_CLIENT:
                ESP_LOGI(TAG, "Link closed by client");
                break;
            case ESP_BLE_MESH_RPR_STATUS_LINK_CLOSED_BY_DEVICE:
                ESP_LOGI(TAG, "Link closed by device");
                break;
            case ESP_BLE_MESH_RPR_STATUS_LINK_CLOSED_BY_SERVER:
                ESP_LOGI(TAG, "Link closed by server");
                break;
            case ESP_BLE_MESH_RPR_STATUS_LINK_CLOSED_AS_CANNOT_RECEIVE_PDU:
                ESP_LOGI(TAG, "Link closed as cannot receive pdu");
                break;
            case ESP_BLE_MESH_RPR_STATUS_LINK_CLOSED_AS_CANNOT_SEND_PDU:
                ESP_LOGI(TAG, "Link closed as cannot send pdu");
                break;
            case ESP_BLE_MESH_RPR_STATUS_LINK_CLOSED_AS_CANNOT_DELIVER_PDU_REPORT:
                ESP_LOGI(TAG, "Link closed as cannot send pdu report");
                break;
            default:
                ESP_LOGW(TAG, "Unknown link close status, %d", param->recv.val.link_report.status);
                break;
            }
            break;
        default:
            ESP_LOGW(TAG, "Unknown Process opcode 0x%04x:%d", cur_rpr_cli_opcode,__LINE__);
            break;
        }
        break;
    case ESP_BLE_MESH_RPR_CLIENT_ACT_COMP_EVT:
        ESP_LOGW(TAG, "Remote Prov Client Act Comp, sub_evt 0x%02x", param->act.sub_evt);
        switch (param->act.sub_evt) {
        case ESP_BLE_MESH_START_RPR_COMP_SUB_EVT:
            ESP_LOGI(TAG, "Start Remote Prov Comp, err_code %d, rpr_srv_addr 0x%04x",
                     param->act.start_rpr_comp.err_code,
                     param->act.start_rpr_comp.rpr_srv_addr);
            break;
        default:
            ESP_LOGE(TAG, "Unknown Remote Provisioning Client sub event");
            break;
        }
        break;
    case ESP_BLE_MESH_RPR_CLIENT_LINK_OPEN_EVT:
        ESP_LOGW(TAG, "Remote Prov Client Link Open");
        break;
    case ESP_BLE_MESH_RPR_CLIENT_LINK_CLOSE_EVT:
        ESP_LOGW(TAG, "Remote Prov Client Link Close");
        break;
    case ESP_BLE_MESH_RPR_CLIENT_PROV_COMP_EVT:
        ESP_LOGW(TAG, "Remote Prov Client Prov Complete");
        ESP_LOGI(TAG, "Net Idx: 0x%04x", param->prov.net_idx);
        ESP_LOGI(TAG, "Node addr: 0x%04x", param->prov.unicast_addr);
        ESP_LOGI(TAG, "Node element num: 0x%04x", param->prov.element_num);
        ESP_LOG_BUFFER_HEX(TAG": Node UUID: ", param->prov.uuid, 16);
        err = example_ble_mesh_set_msg_common(&common, param->prov.rpr_srv_addr, remote_prov_client.model,
                                              ESP_BLE_MESH_MODEL_OP_RPR_LINK_CLOSE);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Set message common fail:%d", __LINE__);
            return;
        }
        msg.link_close.reason = ESP_BLE_MESH_RPR_REASON_SUCCESS;

        err = esp_ble_mesh_rpr_client_send(&common, &msg);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to send Remote Provisioning Client msg:Link open");
        }
        cur_rpr_cli_opcode = ESP_BLE_MESH_MODEL_OP_RPR_LINK_CLOSE;

        prov_complete(param->prov.net_idx,  param->prov.uuid,
            param->prov.unicast_addr, param->prov.element_num, param->prov.net_idx);
        break;
    default:
        break;
    }
}

static esp_err_t ble_mesh_init(void)
{
    uint8_t match[2] = {0x55, 0xaa};

    esp_err_t err = ESP_OK;

    prov_key.net_idx = ESP_BLE_MESH_KEY_PRIMARY;
    prov_key.app_idx = APP_KEY_IDX;
    memset(prov_key.app_key, APP_KEY_OCTET, sizeof(prov_key.app_key));

    esp_ble_mesh_register_prov_callback(example_ble_mesh_provisioning_cb);
    esp_ble_mesh_register_config_client_callback(example_ble_mesh_config_client_cb);
    esp_ble_mesh_register_generic_client_callback(example_ble_mesh_generic_client_cb);
    esp_ble_mesh_register_rpr_client_callback(example_ble_mesh_remote_prov_client_callback);

    err = esp_ble_mesh_init(&provision, &composition);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize mesh stack (err %d)", err);
        return err;
    }

    err = esp_ble_mesh_provisioner_set_dev_uuid_match(match, sizeof(match), 0x0, false);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set matching device uuid (err %d)", err);
        return err;
    }

    err = esp_ble_mesh_provisioner_prov_enable((esp_ble_mesh_prov_bearer_t)(ESP_BLE_MESH_PROV_ADV | ESP_BLE_MESH_PROV_GATT));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable mesh provisioner (err %d)", err);
        return err;
    }

    err = esp_ble_mesh_provisioner_add_local_app_key(prov_key.app_key, prov_key.net_idx, prov_key.app_idx);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add local AppKey (err %d)", err);
        return err;
    }

    ESP_LOGI(TAG, "BLE Mesh Provisioner initialized");

    return err;
}

void app_main(void)
{
    esp_err_t err;

    ESP_LOGI(TAG, "Initializing...");

    board_init();

    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    err = bluetooth_init();
    if (err) {
        ESP_LOGE(TAG, "esp32_bluetooth_init failed (err %d)", err);
        return;
    }

    ble_mesh_get_dev_uuid(dev_uuid);

    /* Initialize the Bluetooth Mesh Subsystem */
    err = ble_mesh_init();
    if (err) {
        ESP_LOGE(TAG, "Bluetooth mesh init failed (err %d)", err);
    }
}
