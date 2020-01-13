/* main.c - Application main entry point */

/*
 * Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_timer.h"
#include "esp_bt.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_generic_model_api.h"
#include "esp_ble_mesh_local_data_operation_api.h"

#include "board.h"
#include "ble_mesh_demo_init.h"
#include "esp_coexist.h"

#define CID_ESP             0x02E5
#define CID_NVAL            0xFFFF

#define PROV_OWN_ADDR       0x0001

#define MSG_SEND_TTL        0
#define MSG_SEND_REL        false
#define MSG_ROLE            ROLE_PROVISIONER

#define COMP_DATA_PAGE_0    0x00

#define APP_KEY_IDX         0x0000
#define APP_KEY_OCTET       0x12

#define MSG_1_OCTET(msg, offset) (msg[offset])
#define MSG_2_OCTET(msg, offset) (msg[offset + 1] << 8 | msg[offset])

static uint16_t primary_addr;
static uint8_t match[2] = {0xec, 0xa6};
static uint8_t dev_uuid[16];

/* The following is BLE Mesh tx/rx test related context */
#define MSG_TIMEOUT         200000      /* msg resend interval */
#define MSG_INTERVAL        1000000     /* msg send interval */
#define MSG_MAX_RESEND      3           /* max resend count */
#define MSG_TEST_COUNT      10          /* message test count */
#define MSG_TEST_UNIT       10          /* message test unit */

static esp_timer_handle_t   retransmit_timer;
static esp_timer_handle_t   interval_timer;

/* Used to record the index of the current sending message */
static uint32_t msg_index;
/* Used to record how many times each message is resent */
static struct {
    uint8_t resend;
    bool acked;
    int64_t time;
} __attribute__((packed)) msg_record[MSG_TEST_COUNT];
/* Used to indicate the transaction number in the current message */
static uint16_t trans_num;
static bool retrans_timer_start;
static void example_start_retransmit_timer(void);
static void example_start_interval_timer(void);

static SemaphoreHandle_t tx_rx_mutex;

/* The following is BLE Mesh deinit test flag */
bool example_deinit_test;

/* The following is BLE Mesh client message timeout with internal timer flag */
bool msg_to_internal;

typedef struct {
    uint8_t  uuid[16];
    uint16_t unicast;
    uint8_t  elem_num;
    uint8_t  onoff;
    uint8_t  tid;
} example_ble_mesh_node_info_t;

static example_ble_mesh_node_info_t nodes[CONFIG_BLE_MESH_MAX_PROV_NODES] = {
    [0 ... (CONFIG_BLE_MESH_MAX_PROV_NODES - 1)] = {
        .unicast = ESP_BLE_MESH_ADDR_UNASSIGNED,
        .elem_num = 0,
        .onoff = LED_OFF,
    }
};

static struct example_ble_mesh_key {
    uint16_t net_idx;
    uint16_t app_idx;
    uint8_t  app_key[16];
} prov_key;

static esp_ble_mesh_client_t config_client;

static esp_ble_mesh_cfg_srv_t config_server = {
    .relay = ESP_BLE_MESH_RELAY_DISABLED,
    .beacon = ESP_BLE_MESH_BEACON_ENABLED,
#if defined(CONFIG_BLE_MESH_FRIEND)
    .friend_state = ESP_BLE_MESH_FRIEND_ENABLED,
#else
    .friend_state = ESP_BLE_MESH_FRIEND_NOT_SUPPORTED,
#endif
#if defined(CONFIG_BLE_MESH_GATT_PROXY_SERVER)
    .gatt_proxy = ESP_BLE_MESH_GATT_PROXY_ENABLED,
#else
    .gatt_proxy = ESP_BLE_MESH_GATT_PROXY_NOT_SUPPORTED,
#endif
    .default_ttl = 7,
    /* 5 transmissions with 20ms interval */
    .net_transmit = ESP_BLE_MESH_TRANSMIT(4, 10),
    .relay_retransmit = ESP_BLE_MESH_TRANSMIT(2, 20),
};

#define ESP_BLE_MESH_VND_MODEL_ID_TEST_CLI      0x0000
#define ESP_BLE_MESH_VND_MODEL_ID_TEST_SRV      0x0001

#define ESP_BLE_MESH_VND_MODEL_OP_TEST_SEND     ESP_BLE_MESH_MODEL_OP_3(0x00, CID_ESP)
#define ESP_BLE_MESH_VND_MODEL_OP_TEST_STATUS   ESP_BLE_MESH_MODEL_OP_3(0x01, CID_ESP)

static const esp_ble_mesh_client_op_pair_t test_op_pair[] = {
    { ESP_BLE_MESH_VND_MODEL_OP_TEST_SEND, ESP_BLE_MESH_VND_MODEL_OP_TEST_STATUS },
};

static esp_ble_mesh_client_t test_client = {
    .op_pair_size = ARRAY_SIZE(test_op_pair),
    .op_pair = test_op_pair,
};

static esp_ble_mesh_model_op_t test_op[] = {
    ESP_BLE_MESH_MODEL_OP(ESP_BLE_MESH_VND_MODEL_OP_TEST_STATUS, 2),
    ESP_BLE_MESH_MODEL_OP_END,
};

static esp_ble_mesh_model_t root_models[] = {
    ESP_BLE_MESH_MODEL_CFG_SRV(&config_server),
    ESP_BLE_MESH_MODEL_CFG_CLI(&config_client),
};

static esp_ble_mesh_model_t vnd_models[] = {
    ESP_BLE_MESH_VENDOR_MODEL(CID_ESP, ESP_BLE_MESH_VND_MODEL_ID_TEST_CLI,
    test_op, NULL, &test_client),
};

static esp_ble_mesh_elem_t elements[] = {
    ESP_BLE_MESH_ELEMENT(0, root_models, vnd_models),
};

static esp_ble_mesh_comp_t composition = {
    .cid = CID_ESP,
    .elements = elements,
    .element_count = ARRAY_SIZE(elements),
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
    size_t i;

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

static example_ble_mesh_node_info_t *example_ble_mesh_get_node_info(uint16_t unicast)
{
    size_t i;

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
                                                 example_ble_mesh_node_info_t *node,
                                                 esp_ble_mesh_model_t *model, uint32_t opcode)
{
    if (!common || !node || !model) {
        return ESP_ERR_INVALID_ARG;
    }

    common->opcode = opcode;
    common->model = model;
    common->ctx.net_idx = prov_key.net_idx;
    common->ctx.app_idx = prov_key.app_idx;
    common->ctx.addr = node->unicast;
    common->ctx.send_ttl = MSG_SEND_TTL;
    common->ctx.send_rel = MSG_SEND_REL;
    common->msg_timeout = 0;
    common->msg_role = MSG_ROLE;

    return ESP_OK;
}

void example_ble_mesh_send_test_msg(bool resend)
{
    esp_ble_mesh_msg_ctx_t ctx = {0};
    bool need_ack = false;
    int32_t timeout;
    esp_err_t err;

    ctx.net_idx = prov_key.net_idx;
    ctx.app_idx = prov_key.app_idx;
    ctx.addr = nodes[0].unicast;
    ctx.send_ttl = MSG_SEND_TTL;
    ctx.send_rel = MSG_SEND_REL;

    uint8_t data[8] = {0};
    memcpy(data, &trans_num, sizeof(trans_num));

    if (msg_to_internal) {
        need_ack = true;
        timeout = 1000;
    } else {
        need_ack = false;
        timeout = 0;
    }

    err = esp_ble_mesh_client_model_send_msg(test_client.model,
            &ctx, ESP_BLE_MESH_VND_MODEL_OP_TEST_SEND,
            sizeof(data), data, timeout, need_ack, MSG_ROLE);
    if (err) {
        ESP_LOGE(TAG, "%s, Failed to send test message", __func__);
        return;
    }

    if (resend == true) {
        msg_record[msg_index].resend++;
    }
}

static esp_err_t prov_complete(int node_idx, const esp_ble_mesh_octet16_t uuid,
                               uint16_t unicast, uint8_t elem_num, uint16_t net_idx)
{
    esp_ble_mesh_client_common_param_t common = {0};
    esp_ble_mesh_cfg_client_get_state_t get = {0};
    example_ble_mesh_node_info_t *node = NULL;
    char name[10];
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

    example_ble_mesh_set_msg_common(&common, node, config_client.model, ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET);
    get.comp_data_get.page = COMP_DATA_PAGE_0;
    err = esp_ble_mesh_config_client_get_state(&common, &get);
    if (err) {
        ESP_LOGE(TAG, "%s: Send config comp data get failed", __func__);
        return ESP_FAIL;
    }

    return ESP_OK;
}

static void prov_link_open(esp_ble_mesh_prov_bearer_t bearer)
{
    ESP_LOGI(TAG, "%s link open", bearer == ESP_BLE_MESH_PROV_ADV ? "PB-ADV" : "PB-GATT");
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
    add_dev.addr_type = (uint8_t)addr_type;
    memcpy(add_dev.uuid, dev_uuid, 16);
    add_dev.oob_info = oob_info;
    add_dev.bearer = (uint8_t)bearer;
    /* Note: If unprovisioned device adv packets have not been received, we should not add
             device with ADD_DEV_START_PROV_NOW_FLAG set. */
    err = esp_ble_mesh_provisioner_add_unprov_dev(&add_dev,
            ADD_DEV_RM_AFTER_PROV_FLAG | ADD_DEV_START_PROV_NOW_FLAG | ADD_DEV_FLUSHABLE_DEV_FLAG);
    if (err) {
        ESP_LOGE(TAG, "%s: Add unprovisioned device into queue failed", __func__);
    }
}

extern bool iperf_test;

static void example_ble_mesh_provisioning_cb(esp_ble_mesh_prov_cb_event_t event,
                                             esp_ble_mesh_prov_cb_param_t *param)
{
    switch (event) {
    case ESP_BLE_MESH_PROV_REGISTER_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROV_REGISTER_COMP_EVT, err_code %d", param->prov_register_comp.err_code);
        break;
    case ESP_BLE_MESH_PROVISIONER_PROV_ENABLE_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_PROV_ENABLE_COMP_EVT, err_code %d", param->provisioner_prov_enable_comp.err_code);
        esp_coex_status_bit_clear(ESP_COEX_ST_TYPE_BLE, ESP_COEX_BLE_ST_MESH_CONFIG);
        esp_coex_status_bit_clear(ESP_COEX_ST_TYPE_BLE, ESP_COEX_BLE_ST_MESH_TRAFFIC);
        esp_coex_status_bit_clear(ESP_COEX_ST_TYPE_BLE, ESP_COEX_BLE_ST_MESH_STANDBY);
        if (iperf_test == true) {
            esp_coex_status_bit_set(ESP_COEX_ST_TYPE_BLE, ESP_COEX_BLE_ST_MESH_STANDBY);
            ESP_LOGW(TAG, "BLE Mesh enters Standby mode");
        } else {
            esp_coex_status_bit_set(ESP_COEX_ST_TYPE_BLE, ESP_COEX_BLE_ST_MESH_CONFIG);
            ESP_LOGW(TAG, "BLE Mesh enters Config mode");
        }
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
    case ESP_BLE_MESH_PROVISIONER_SET_PRIMARY_ELEM_ADDR_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_SET_PRIMARY_ELEM_ADDR_COMP_EVT, err_code %d", param->provisioner_set_primary_elem_addr_comp.err_code);
        if (!param->provisioner_set_primary_elem_addr_comp.err_code) {
            primary_addr = esp_ble_mesh_get_primary_element_address();
        }
        break;
    case ESP_BLE_MESH_PROVISIONER_SET_NODE_NAME_COMP_EVT:
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
    case ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_APP_KEY_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_APP_KEY_COMP_EVT, err_code %d", param->provisioner_add_app_key_comp.err_code);
        if (param->provisioner_add_app_key_comp.err_code == ESP_OK) {
            esp_err_t err = 0;
            prov_key.app_idx = param->provisioner_add_app_key_comp.app_idx;
            err = esp_ble_mesh_provisioner_bind_app_key_to_local_model(primary_addr, prov_key.app_idx,
                    ESP_BLE_MESH_VND_MODEL_ID_TEST_CLI, CID_ESP);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Provisioner bind local model appkey failed");
                return;
            }
        }
        break;
    case ESP_BLE_MESH_PROVISIONER_BIND_APP_KEY_TO_MODEL_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_BIND_APP_KEY_TO_MODEL_COMP_EVT, err_code %d", param->provisioner_bind_app_key_to_model_comp.err_code);
        break;
    default:
        break;
    }
}

static void example_ble_mesh_parse_node_comp_data(const uint8_t *data, uint16_t length)
{
    uint16_t cid, pid, vid, crpl, feat;
    uint16_t loc, model_id, company_id;
    uint8_t nums, numv;
    uint16_t offset;
    size_t i;

    cid = MSG_2_OCTET(data, 0);
    pid = MSG_2_OCTET(data, 2);
    vid = MSG_2_OCTET(data, 4);
    crpl = MSG_2_OCTET(data, 6);
    feat = MSG_2_OCTET(data, 8);
    offset = 10;

    ESP_LOGW(TAG, "***************** Composition Data Start *****************");
    ESP_LOGI(TAG, "* CID 0x%04x, PID 0x%04x, VID 0x%04x, CRPL 0x%04x, Features 0x%04x *", cid, pid, vid, crpl, feat);
    for (; offset < length; ) {
        loc = MSG_2_OCTET(data, offset);
        nums = MSG_1_OCTET(data, offset + 2);
        numv = MSG_1_OCTET(data, offset + 3);
        offset += 4;
        ESP_LOGI(TAG, "* Loc 0x%04x, NumS 0x%02x, NumV 0x%02x *", loc, nums, numv);
        for (i = 0; i < nums; i++) {
            model_id = MSG_2_OCTET(data, offset);
            ESP_LOGI(TAG, "* SIG Model ID 0x%04x *", model_id);
            offset += 2;
        }
        for (i = 0; i < numv; i++) {
            company_id = MSG_2_OCTET(data, offset);
            model_id = MSG_2_OCTET(data, offset + 2);
            ESP_LOGI(TAG, "* Vendor Model ID 0x%04x, Company ID 0x%04x *", model_id, company_id);
            offset += 4;
        }
    }
    ESP_LOGW(TAG, "****************** Composition Data End ******************");
}

static void example_ble_mesh_config_client_cb(esp_ble_mesh_cfg_client_cb_event_t event,
                                              esp_ble_mesh_cfg_client_cb_param_t *param)
{
    esp_ble_mesh_client_common_param_t common = {0};
    esp_ble_mesh_cfg_client_set_state_t set = {0};
    esp_ble_mesh_cfg_client_get_state_t get = {0};
    example_ble_mesh_node_info_t *node = NULL;
    esp_err_t err;

    if (param->error_code) {
        ESP_LOGE(TAG, "Send config message failed, opcode 0x%04x", param->params->opcode);
        return;
    }

    node = example_ble_mesh_get_node_info(param->params->ctx.addr);
    if (!node) {
        ESP_LOGE(TAG, "%s: Get node info failed", __func__);
        return;
    }

    switch (event) {
    case ESP_BLE_MESH_CFG_CLIENT_GET_STATE_EVT:
        if (param->params->opcode == ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET) {
            ESP_LOGI(TAG, "composition data %s", bt_hex(param->status_cb.comp_data_status.composition_data->data,
                param->status_cb.comp_data_status.composition_data->len));
            example_ble_mesh_parse_node_comp_data(param->status_cb.comp_data_status.composition_data->data,
                param->status_cb.comp_data_status.composition_data->len);
            esp_ble_mesh_provisioner_store_node_comp_data(param->params->ctx.addr,
                param->status_cb.comp_data_status.composition_data->data,
                param->status_cb.comp_data_status.composition_data->len);
            example_ble_mesh_set_msg_common(&common, node, config_client.model, ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD);
            set.app_key_add.net_idx = prov_key.net_idx;
            set.app_key_add.app_idx = prov_key.app_idx;
            memcpy(set.app_key_add.app_key, prov_key.app_key, 16);
            err = esp_ble_mesh_config_client_set_state(&common, &set);
            if (err) {
                ESP_LOGE(TAG, "%s: Config AppKey Add failed", __func__);
            }
        }
        break;
    case ESP_BLE_MESH_CFG_CLIENT_SET_STATE_EVT:
        if (param->params->opcode == ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD) {
            example_ble_mesh_set_msg_common(&common, node, config_client.model, ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND);
            set.model_app_bind.element_addr = node->unicast;
            set.model_app_bind.model_app_idx = prov_key.app_idx;
            set.model_app_bind.model_id = ESP_BLE_MESH_VND_MODEL_ID_TEST_SRV;
            set.model_app_bind.company_id = CID_ESP;
            err = esp_ble_mesh_config_client_set_state(&common, &set);
            if (err) {
                ESP_LOGE(TAG, "%s: Config Model App Bind failed", __func__);
            }
        } else if (param->params->opcode == ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND) {
            ESP_LOGW(TAG, "%s, Primary element address 0x%04x", __func__, esp_ble_mesh_get_primary_element_address());
            ESP_LOGW(TAG, "%s, Provisioning and config successfully", __func__);
        }
        break;
    case ESP_BLE_MESH_CFG_CLIENT_TIMEOUT_EVT:
        switch (param->params->opcode) {
        case ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET:
            example_ble_mesh_set_msg_common(&common, node, config_client.model, ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET);
            get.comp_data_get.page = COMP_DATA_PAGE_0;
            err = esp_ble_mesh_config_client_get_state(&common, &get);
            if (err) {
                ESP_LOGE(TAG, "%s: Config Composition Data Get failed", __func__);
            }
            break;
        case ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD:
            example_ble_mesh_set_msg_common(&common, node, config_client.model, ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD);
            set.app_key_add.net_idx = prov_key.net_idx;
            set.app_key_add.app_idx = prov_key.app_idx;
            memcpy(set.app_key_add.app_key, prov_key.app_key, 16);
            err = esp_ble_mesh_config_client_set_state(&common, &set);
            if (err) {
                ESP_LOGE(TAG, "%s: Config AppKey Add failed", __func__);
            }
            break;
        case ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND:
            example_ble_mesh_set_msg_common(&common, node, config_client.model, ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND);
            set.model_app_bind.element_addr = node->unicast;
            set.model_app_bind.model_app_idx = prov_key.app_idx;
            set.model_app_bind.model_id = ESP_BLE_MESH_VND_MODEL_ID_TEST_SRV;
            set.model_app_bind.company_id = CID_ESP;
            err = esp_ble_mesh_config_client_set_state(&common, &set);
            if (err) {
                ESP_LOGE(TAG, "%s: Config Model App Bind failed", __func__);
            }
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

static void example_tx_rx_mutex_create(void)
{
    if (!tx_rx_mutex) {
        tx_rx_mutex = xSemaphoreCreateMutex();
        assert(tx_rx_mutex);
    }
}

static void example_tx_rx_lock(void)
{
    if (tx_rx_mutex) {
        xSemaphoreTake(tx_rx_mutex, portMAX_DELAY);
    }
}

static void example_tx_rx_unlock(void)
{
    if (tx_rx_mutex) {
        xSemaphoreGive(tx_rx_mutex);
    }
}

static void example_test_result_output(void)
{
    uint32_t count[MSG_MAX_RESEND + 1] = {0};
    uint8_t compare[MSG_MAX_RESEND + 1];
    /**
     * Used to record if resend reaches max number, whether we have
     * received corresponding response.
     */
    uint32_t no_ack_count = 0;
    int64_t total_time = 0;
    size_t i, j, k;

    for (i = 0; i < sizeof(compare); i++) {
        compare[i] = i;
    }

    ESP_LOGI(TAG, "ble mesh tx rx test finished");

    for (i = 0; i < MSG_TEST_COUNT / MSG_TEST_UNIT; i++) {
        for (j = 0; j < MSG_TEST_UNIT; j++) {
            for (k = 0; k < sizeof(compare); k++) {
                if (msg_record[i * MSG_TEST_UNIT + j].resend == compare[k]) {
                    count[k]++;
                    if (msg_record[i * MSG_TEST_UNIT + j].resend == MSG_MAX_RESEND &&
                        msg_record[i * MSG_TEST_UNIT + j].acked == false) {
                        /* If msg is not acked, use 1s for its time */
                        msg_record[msg_index].time = 1000000;
                        no_ack_count++;
                    }
                    break;
                }
            }
            total_time += msg_record[msg_index].time;
        }
        ESP_LOGW(TAG, "Send messages the %d time, total %lldus, average %lldus", i, total_time, total_time / MSG_TEST_UNIT);
        ESP_LOGW(TAG, "0-resend %d, 1-resend %d, 2-resend %d, 3-resend %d, 3-resend-no-ack %d",
            count[0], count[1], count[2], count[3], no_ack_count);
        ESP_LOG_BUFFER_HEX("log", msg_record, MSG_TEST_COUNT * sizeof(msg_record[0]));
        bzero(count, sizeof(count));
        no_ack_count = 0;
        total_time = 0;
    }
}

static void example_ble_mesh_send_next(void)
{
    if (msg_index >= MSG_TEST_COUNT - 1) {
        example_test_result_output();
        return;
    }

    ++msg_index;
    ++trans_num;
    msg_record[msg_index].resend = 0;
    msg_record[msg_index].acked = false;
    msg_record[msg_index].time = 0;

    example_ble_mesh_send_test_msg(false);
}

static void example_ble_mesh_custom_model_cb(esp_ble_mesh_model_cb_event_t event,
                                             esp_ble_mesh_model_cb_param_t *param)
{
    switch (event) {
    case ESP_BLE_MESH_MODEL_OPERATION_EVT:
        if (!param->model_operation.model || !param->model_operation.model->op ||
            !param->model_operation.ctx) {
            ESP_LOGE(TAG, "%s, Invalid parameter", __func__);
            break;
        }
        if (msg_to_internal && param->model_operation.opcode == ESP_BLE_MESH_VND_MODEL_OP_TEST_STATUS) {
            uint16_t value = *(uint16_t *)param->model_operation.msg;
            ESP_LOGI(TAG, "response, %d %d %d", msg_index, value, trans_num);
            if (value == trans_num) {
                msg_record[msg_index].acked = true;
                int64_t time = esp_timer_get_time();
                // ESP_LOGW(TAG, "End %lldus", time);
                msg_record[msg_index].time = time - msg_record[msg_index].time;

                example_ble_mesh_send_next();
            }
        }
        break;
    case ESP_BLE_MESH_MODEL_SEND_COMP_EVT:
        if (param->model_send_comp.err_code == ESP_OK) {
            ESP_LOGI(TAG, "send, %d", msg_index);
            example_start_retransmit_timer();
            if (msg_record[msg_index].resend == 0) {
                /* If send successfully in the first time, start the interval timer */
                example_start_interval_timer();
                msg_record[msg_index].time = esp_timer_get_time();
                // ESP_LOGW(TAG, "Start %lldus", msg_record[msg_index].time);
            }
        }
        break;
    case ESP_BLE_MESH_CLIENT_MODEL_RECV_PUBLISH_MSG_EVT:
        example_tx_rx_lock();
        if (msg_to_internal == false && param->client_recv_publish_msg.opcode == ESP_BLE_MESH_VND_MODEL_OP_TEST_STATUS) {
            uint16_t value = *(uint16_t *)param->client_recv_publish_msg.msg;
            ESP_LOGI(TAG, "%d %d %d", msg_index, value, trans_num);
            if (value == trans_num && retrans_timer_start == true) {
                msg_record[msg_index].acked = true;
                esp_timer_stop(retransmit_timer);
                int64_t time = esp_timer_get_time();
                // ESP_LOGW(TAG, "End %lldus", time);
                msg_record[msg_index].time = time - msg_record[msg_index].time;
            }
        }
        example_tx_rx_unlock();
        break;
    case ESP_BLE_MESH_CLIENT_MODEL_SEND_TIMEOUT_EVT:
        ESP_LOGI(TAG, "timeout, %d", msg_index);
        example_ble_mesh_send_test_msg(true);
        break;
    default:
        break;
    }
}

void example_start_retransmit_timer(void)
{
    if (msg_to_internal) {
        return;
    }
    esp_timer_start_once(retransmit_timer, MSG_TIMEOUT);
    retrans_timer_start = true;
}

static void example_start_interval_timer(void)
{
    if (msg_to_internal) {
        return;
    }
    esp_timer_start_once(interval_timer, MSG_INTERVAL);
}

static void retransmit_timer_callback(void* arg)
{
    example_tx_rx_lock();
    retrans_timer_start = false;
    if (msg_record[msg_index].acked == false &&
        msg_record[msg_index].resend < MSG_MAX_RESEND) {
        example_ble_mesh_send_test_msg(true);
    }
    example_tx_rx_unlock();
}

static void interval_timer_callback(void* arg)
{
    /* Message interval timer timeout handler */
    example_ble_mesh_send_next();
}

void example_ble_mesh_test_init(void)
{
    bzero(msg_record, sizeof(msg_record));
    msg_index = 0x0;
    trans_num = 0x0;
    retrans_timer_start = false;
    esp_timer_stop(retransmit_timer);
    esp_timer_stop(interval_timer);
}

const esp_timer_create_args_t retransmit_timer_args = {
        .callback = &retransmit_timer_callback,
        .name = "retransmit",
};

const esp_timer_create_args_t interval_timer_args = {
        .callback = &interval_timer_callback,
        .name = "interval",
};

esp_err_t example_ble_mesh_start(void)
{
    esp_err_t err;

    err = esp_ble_mesh_init(&provision, &composition);
    if (err) {
        return err;
    }

    err = esp_ble_mesh_client_model_init(&vnd_models[0]);
    if (err) {
        return err;
    }

    err = esp_ble_mesh_provisioner_set_dev_uuid_match(match, sizeof(match), 0x0, false);
    if (err) {
        return err;
    }

    err = esp_ble_mesh_provisioner_prov_enable(ESP_BLE_MESH_PROV_ADV | ESP_BLE_MESH_PROV_GATT);
    if (err) {
        return err;
    }

    err = esp_ble_mesh_provisioner_set_primary_elem_addr(0x0006);
    if (err) {
        return err;
    }

    err = esp_ble_mesh_provisioner_add_local_app_key(prov_key.app_key, prov_key.net_idx, prov_key.app_idx);
    if (err) {
        return err;
    }

    ESP_LOGW(TAG, "BLE Mesh start");
    return 0;
}

static esp_err_t ble_mesh_init(void)
{
    esp_err_t err;

    primary_addr = PROV_OWN_ADDR;
    prov_key.net_idx = ESP_BLE_MESH_KEY_PRIMARY;
    prov_key.app_idx = APP_KEY_IDX;
    memset(prov_key.app_key, APP_KEY_OCTET, sizeof(prov_key.app_key));

    esp_ble_mesh_register_prov_callback(example_ble_mesh_provisioning_cb);
    esp_ble_mesh_register_config_client_callback(example_ble_mesh_config_client_cb);
    esp_ble_mesh_register_custom_model_callback(example_ble_mesh_custom_model_cb);

    if (example_deinit_test == true) {
        ESP_LOGW(TAG, "prev, free heap size %d", esp_get_free_heap_size());
        vTaskDelay(10 / portTICK_PERIOD_MS);
        ESP_LOGW(TAG, "start, free heap size %d", esp_get_free_heap_size());
        /* First time (make erase_flash, then make flash monitor), 72 bytes will be cost
         * during first (start - end) circle.
         * Second time (make monitor), 0 byte will be cost during first (start - end) cycle.
         */
        for (int i = 0; i < 100; i++) {
            err = esp_ble_mesh_init(&provision, &composition);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "%s: Failed to initialize BLE Mesh", __func__);
                return err;
            }
            err = esp_ble_mesh_client_model_init(&vnd_models[0]);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "%s: Failed to initialize vnd client model", __func__);
                return err;
            }
            err = esp_ble_mesh_client_model_deinit(&vnd_models[0]);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "%s: Failed to deinitialize vnd client model", __func__);
                return err;
            }
            esp_ble_mesh_deinit_param_t param = {
                .erase_flash = false,
            };
            err = esp_ble_mesh_deinit(&param);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "%s: Failed to de-initialize BLE Mesh", __func__);
                return err;
            }
            ESP_LOGW(TAG, "end, free heap size %d", esp_get_free_heap_size());
        }
    }

    err = example_ble_mesh_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s, BLE Mesh start failed", __func__);
        return err;
    }

    ESP_LOGI(TAG, "BLE Mesh Provisioner initialized");

    return err;
}

void ble_mesh_start(void)
{
    esp_err_t err;

    ESP_LOGI(TAG, "Initializing...");

    // esp_log_level_set(TAG, ESP_LOG_WARN);

    board_init();

    esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9);
    ESP_LOGI(TAG, "tx power level %d", esp_ble_tx_power_get(ESP_BLE_PWR_TYPE_ADV));

    ble_mesh_get_dev_uuid(dev_uuid);

    /* Initialize the Bluetooth Mesh Subsystem */
    err = ble_mesh_init();
    if (err) {
        ESP_LOGE(TAG, "Bluetooth mesh init failed (err %d)", err);
        return;
    }

    if (((MSG_MAX_RESEND + 1) * MSG_TIMEOUT) >= MSG_INTERVAL) {
        ESP_LOGE(TAG, "Invalid msg related parameters");
        return;
    }

    ESP_ERROR_CHECK(esp_timer_create(&retransmit_timer_args, &retransmit_timer));
    ESP_ERROR_CHECK(esp_timer_create(&interval_timer_args, &interval_timer));

    example_tx_rx_mutex_create();

    board_led_operation(LED_R, LED_ON);
}
