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

#include "ble_mesh_model.h"
uint8_t dev_uuid[16] = {0xdd, 0xdd};


esp_ble_mesh_prov_t prov = {
#if CONFIG_BLE_MESH_NODE
    .uuid = dev_uuid,
#endif //CONFIG_BLE_MESH_NODE
#if CONFIG_BLE_MESH_PROVISIONER
    .prov_uuid           = dev_uuid,
    .prov_unicast_addr   = 0x0001,
    .prov_start_address  = 0x0005,
    .prov_attention      = 0x00,
    .prov_algorithm      = 0x00,
    .prov_pub_key_oob    = 0x00,
    .prov_static_oob_val = NULL,
    .prov_static_oob_len = 0x00,
    .flags               = 0x00,
    .iv_index            = 0x00,
#endif //CONFIG_BLE_MESH_PROVISIONER
};

esp_ble_mesh_model_pub_t vendor_model_pub_config;

// Configuration server model
esp_ble_mesh_cfg_srv_t cfg_srv = {
    .relay = ESP_BLE_MESH_RELAY_ENABLED,
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

    /* 3 transmissions with 20ms interval */
    .net_transmit = ESP_BLE_MESH_TRANSMIT(2, 20),
    .relay_retransmit = ESP_BLE_MESH_TRANSMIT(2, 20),
};

// Configuration Client model
esp_ble_mesh_client_t cfg_cli;

esp_ble_mesh_model_t config_client_models[] = {
    ESP_BLE_MESH_MODEL_CFG_SRV(&cfg_srv),
    ESP_BLE_MESH_MODEL_CFG_CLI(&cfg_cli),
};

esp_ble_mesh_elem_t config_client_elements[] = {
    ESP_BLE_MESH_ELEMENT(0, config_client_models, ESP_BLE_MESH_MODEL_NONE),
};

esp_ble_mesh_comp_t config_client_comp = {
    .cid = CID_ESP,
    .elements = config_client_elements,
    .element_count = ARRAY_SIZE(config_client_elements),
};

// Generic OnOff Server model
esp_ble_mesh_gen_onoff_srv_t onoff_server = {
    .rsp_ctrl.get_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
    .rsp_ctrl.set_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
};

// Generic OnOff Client model
#if (CONFIG_BLE_MESH_GENERIC_ONOFF_CLI)
esp_ble_mesh_client_t gen_onoff_cli;
#endif //CONFIG_BLE_MESH_GENERIC_ONOFF_CLI

//CONFIG VENDOR MODEL TEST PERFORMANCE
#define ESP_BLE_MESH_VND_MODEL_ID_TEST_PERF_SRV 0x2000
#define ESP_BLE_MESH_VND_MODEL_ID_TEST_PERF_CLI 0x2001

#define ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_GET                 ESP_BLE_MESH_MODEL_OP_3(0x01, CID_ESP)
#define ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_SET                 ESP_BLE_MESH_MODEL_OP_3(0x02, CID_ESP)
#define ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_SET_UNACK           ESP_BLE_MESH_MODEL_OP_3(0x03, CID_ESP)
#define ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_STATUS              ESP_BLE_MESH_MODEL_OP_3(0x04, CID_ESP)

esp_ble_mesh_client_op_pair_t test_perf_cli_op_pair[] = {
    {ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_GET, ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_STATUS},
    {ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_SET, ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_STATUS},
    {ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_SET_UNACK, ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_STATUS},
};

esp_ble_mesh_client_t test_perf_cli = {
    .op_pair_size = ARRAY_SIZE(test_perf_cli_op_pair),
    .op_pair = test_perf_cli_op_pair,
};

esp_ble_mesh_model_op_t test_perf_srv_op[] = {
    ESP_BLE_MESH_MODEL_OP(ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_GET,       1),
    ESP_BLE_MESH_MODEL_OP(ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_SET,       1),
    ESP_BLE_MESH_MODEL_OP(ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_SET_UNACK, 1),
    ESP_BLE_MESH_MODEL_OP_END,
};

esp_ble_mesh_model_op_t test_perf_cli_op[] = {
    ESP_BLE_MESH_MODEL_OP(ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_STATUS, 1),
    ESP_BLE_MESH_MODEL_OP_END,
};

esp_ble_mesh_model_t config_models[] = {
    ESP_BLE_MESH_MODEL_CFG_SRV(&cfg_srv),
    ESP_BLE_MESH_MODEL_CFG_CLI(&cfg_cli),
};
