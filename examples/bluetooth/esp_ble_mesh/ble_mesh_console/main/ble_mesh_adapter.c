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

#include "esp_ble_mesh_networking_api.h"
#include "ble_mesh_adapter.h"

ble_mesh_performance_statistics_t test_perf_statistics;
ble_mesh_node_statistics_t ble_mesh_node_statistics;

ESP_BLE_MESH_MODEL_PUB_DEFINE(onoff_pub_0, 2 + 3, ROLE_NODE);
ESP_BLE_MESH_MODEL_PUB_DEFINE(model_pub_config, 2 + 1, ROLE_NODE);

static esp_ble_mesh_model_t srv_models[] = {
    ESP_BLE_MESH_MODEL_CFG_SRV(&cfg_srv),
    ESP_BLE_MESH_MODEL_GEN_ONOFF_SRV(&onoff_pub_0, &onoff_server),
};

esp_ble_mesh_model_t vendor_srv_models[] = {
    ESP_BLE_MESH_VENDOR_MODEL(CID_ESP, ESP_BLE_MESH_VND_MODEL_ID_TEST_PERF_SRV,
    test_perf_srv_op, NULL, NULL),
};

static esp_ble_mesh_elem_t srv_elements[] = {
    ESP_BLE_MESH_ELEMENT(0, srv_models, vendor_srv_models),
};

static esp_ble_mesh_comp_t srv_composition = {
    .cid = CID_ESP,
    .elements = srv_elements,
    .element_count = ARRAY_SIZE(srv_elements),
};

//client models
esp_ble_mesh_model_t cli_models[] = {
    ESP_BLE_MESH_MODEL_CFG_SRV(&cfg_srv),
    ESP_BLE_MESH_MODEL_CFG_CLI(&cfg_cli),
    ESP_BLE_MESH_MODEL_GEN_ONOFF_CLI(&model_pub_config, &gen_onoff_cli),
    ESP_BLE_MESH_MODEL_GEN_ONOFF_SRV(&onoff_pub_0, &onoff_server),
};

esp_ble_mesh_model_t vendor_cli_models[] = {
    ESP_BLE_MESH_VENDOR_MODEL(CID_ESP, ESP_BLE_MESH_VND_MODEL_ID_TEST_PERF_CLI,
    test_perf_cli_op, &vendor_model_pub_config, &test_perf_cli),
};

static esp_ble_mesh_elem_t cli_elements[] = {
    ESP_BLE_MESH_ELEMENT(0, cli_models, vendor_cli_models),
};

static esp_ble_mesh_comp_t cli_composition = {
    .cid = CID_ESP,
    .elements = cli_elements,
    .element_count = ARRAY_SIZE(cli_elements),
};

esp_ble_mesh_comp_t *ble_mesh_get_component(uint16_t model_id)
{
    esp_ble_mesh_comp_t *comp = NULL;
    switch (model_id) {
    case ESP_BLE_MESH_MODEL_ID_CONFIG_SRV:
    case ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV:
    case ESP_BLE_MESH_VND_MODEL_ID_TEST_PERF_SRV:
        comp = &srv_composition;
        break;
    case ESP_BLE_MESH_MODEL_ID_CONFIG_CLI:
#if (CONFIG_BLE_MESH_GENERIC_ONOFF_CLI)
    case ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_CLI:
#endif
    case ESP_BLE_MESH_VND_MODEL_ID_TEST_PERF_CLI:
        comp = &cli_composition;
        break;
    default:
        break;
    }
    return comp;
}

int ble_mesh_init_node_prestore_params(void)
{
    uint16_t i;

    for (i = 0; i < NODE_MAX_GROUP_CONFIG; i++) {
        ble_mesh_node_prestore_params[i].net_idx = ESP_BLE_MESH_KEY_UNUSED;
        ble_mesh_node_prestore_params[i].unicast_addr = ESP_BLE_MESH_ADDR_UNASSIGNED;
    }
    if(ble_mesh_node_sema == NULL) {
        ble_mesh_node_sema = xSemaphoreCreateMutex();
        if (!ble_mesh_node_sema) {
            ESP_LOGE(TAG, "%s init fail, mesh node semaphore create fail", __func__);
            return ESP_ERR_NO_MEM;
        }
    }
    return 0;
}

void ble_mesh_set_node_prestore_params(uint16_t netkey_index, uint16_t unicast_addr)
{
    uint16_t i;
    xSemaphoreTake(ble_mesh_node_sema, portMAX_DELAY);
    for (i = 0; i < NODE_MAX_GROUP_CONFIG; i++) {
        if (ble_mesh_node_prestore_params[i].net_idx != ESP_BLE_MESH_KEY_UNUSED && ble_mesh_node_prestore_params[i].unicast_addr != ESP_BLE_MESH_ADDR_UNASSIGNED) {
            ble_mesh_node_prestore_params[i].net_idx = netkey_index;
            ble_mesh_node_prestore_params[i].unicast_addr = unicast_addr;
        }
    }
    xSemaphoreGive(ble_mesh_node_sema);
}

void ble_mesh_deinit_node_prestore_params(void)
{
    if (ble_mesh_node_sema != NULL) {
        vSemaphoreDelete(ble_mesh_node_sema);
        ble_mesh_node_sema = NULL;
    }
}

void ble_mesh_node_statistics_get(void)
{
    xSemaphoreTake(ble_mesh_node_sema, portMAX_DELAY);
    ESP_LOGI(TAG, "Statistics:%d\n", ble_mesh_node_statistics.package_num);
    xSemaphoreGive(ble_mesh_node_sema);
}

int ble_mesh_node_statistics_accumulate(uint8_t *data, uint32_t value, uint16_t type)
{
    uint16_t i;
    uint16_t sequence_num = (data[0] << 8) | data[1];

    xSemaphoreTake(ble_mesh_node_sema, portMAX_DELAY);

    for (i = 0; i < ble_mesh_node_statistics.total_package_num; i++) {
        /* Filter out repeated packages during retransmission */
        if (ble_mesh_node_statistics.package_index[i] == sequence_num) {
            xSemaphoreGive(ble_mesh_node_sema);
            return 0;
        }
    }

    // package type wrong
    if (data[2] != type) {
        xSemaphoreGive(ble_mesh_node_sema);
        return 1;
    }

    for (i = 0; i < ble_mesh_node_statistics.total_package_num; i++) {
        /* Judge whether the package is received for the first time */
        if (ble_mesh_node_statistics.package_index[i] == 0) {
            ble_mesh_node_statistics.package_index[i] = sequence_num;
            ble_mesh_node_statistics.package_num += 1;
            ble_mesh_node_statistics.statistics += value;
            break;
        }
    }
    xSemaphoreGive(ble_mesh_node_sema);

    return 0;
}

int ble_mesh_node_statistics_init(uint16_t package_num)
{
    uint16_t i;

    ble_mesh_node_statistics.package_index = malloc(sizeof(uint16_t) * package_num);
    ble_mesh_node_statistics.total_package_num = package_num;
    if (ble_mesh_node_statistics.package_index == NULL) {
        ESP_LOGE(TAG, " %s, %d malloc fail\n", __func__, __LINE__);
        return ESP_ERR_NO_MEM;
    }

    ble_mesh_node_statistics.package_num = 0;
    for (i = 0; i < package_num; i++) {
        ble_mesh_node_statistics.package_index[i] = 0;
    }
    return 0;
}

void ble_mesh_node_statistics_destroy(void)
{
    if (ble_mesh_node_statistics.package_index != NULL) {
        free(ble_mesh_node_statistics.package_index);
    }
}

void ble_mesh_create_send_data(char *data, uint16_t byte_num, uint16_t sequence_num, uint32_t opcode)
{
    uint16_t i;

    // first two bytes are sequence num, third is type
    data[0] = sequence_num >> 8;
    data[1] = sequence_num & 0xFF;
    switch (opcode) {
    case ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_GET:
        data[2] = VENDOR_MODEL_PERF_OPERATION_TYPE_GET;
        break;
    case ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_SET:
        data[2] = VENDOR_MODEL_PERF_OPERATION_TYPE_SET;
        break;
    case ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_SET_UNACK:
        data[2] = VENDOR_MODEL_PERF_OPERATION_TYPE_SET_UNACK;
        break;
    }

    for (i = 3; i < byte_num; i++) {
        data[i] = i;
    }
}

void ble_mesh_test_performance_client_model_get(void)
{
    uint32_t i;
    uint32_t succeed_packet_count;
    uint32_t sum_time = 0;
    uint32_t failed_packet_num = 0;
    uint32_t rtt = 0;

    for (i = 0, succeed_packet_count = 0; i < test_perf_statistics.test_num; i++) {
        if (test_perf_statistics.time[i] != 0) {
            sum_time += test_perf_statistics.time[i];
            succeed_packet_count += 1;
        } else {
            failed_packet_num += 1;
        }
    }

    if(succeed_packet_count != 0){
        rtt = (int)(sum_time / succeed_packet_count);
    }

    ESP_LOGI(TAG, "VendorModel:Statistics,%d,%d\n", failed_packet_num, rtt);
}

void ble_mesh_test_performance_client_model_get_received_percent(void)
{
    uint32_t i, j;
    uint32_t max_time = 1400;
    uint32_t min_time = 0;
    uint32_t time_level_num = 0;
    typedef struct {
        uint16_t time_level;
        uint16_t time_num;
    } statistics_time_performance;
    statistics_time_performance *statistics_time_percent;

    time_level_num = ((max_time - min_time) / 50 + 1);
    statistics_time_percent = malloc(sizeof(statistics_time_performance) * time_level_num);
    if (statistics_time_percent == NULL) {
        ESP_LOGI(TAG, "malloc error");
        return;
    }

    for (j = 0; j < time_level_num; j++) {
        statistics_time_percent[j].time_level = min_time + 50 * j;
        statistics_time_percent[j].time_num = 0;
    }

    for (i = 0; i < test_perf_statistics.test_num; i++) {
        for (j = 0; j < time_level_num; j++) {
            if (test_perf_statistics.time[i] > max_time) {
                j -= 1;
                break;
            }
            if (test_perf_statistics.time[i] >= min_time + 50 * j
                    && test_perf_statistics.time[i] < min_time + 50 * (j + 1)) {
                statistics_time_percent[j].time_num += 1;
                break;
            }
        }
    }

    // for script match
    ESP_LOGI(TAG, "VendorModel:Statistics");
    for (j = 0; j < time_level_num; j++) {
        ESP_LOGI("", "%d:%d", statistics_time_percent[j].time_level, statistics_time_percent[j].time_num);
    }
    free(statistics_time_percent);
}

void ble_mesh_test_performance_client_model_accumulate_statistics(uint32_t value)
{
    test_perf_statistics.statistics += value;
}

int ble_mesh_test_performance_client_model_accumulate_time(uint16_t time, uint8_t *data, uint8_t ack_ttl, uint16_t length)
{
    uint16_t i;
    uint16_t sequence_num = 0;
    uint16_t node_received_ttl = 0;

    if (data != NULL) {
        sequence_num = (data[0] << 8) | data[1];
        if (data[2] == VENDOR_MODEL_PERF_OPERATION_TYPE_SET) {
            node_received_ttl = data[3];
        }
    }
    for (i = 0; i < test_perf_statistics.test_num; i++) {
        if (test_perf_statistics.package_index[i] == sequence_num) {
            return 1;
        }
    }

    for (i = 0; i < test_perf_statistics.test_num; i++) {
        if (test_perf_statistics.package_index[i] == 0) {
            test_perf_statistics.package_index[i] = sequence_num;
            if (data[2] == VENDOR_MODEL_PERF_OPERATION_TYPE_SET) {
                if (node_received_ttl == test_perf_statistics.ttl) {
                    test_perf_statistics.time[i] = time;
                } else {
                    test_perf_statistics.time[i] = 0;
                }
            } else if (data[2] == VENDOR_MODEL_PERF_OPERATION_TYPE_SET_UNACK) {
                test_perf_statistics.time[i] = time;
            }
            break;
        }
    }

    return 0;
}

int ble_mesh_test_performance_client_model_init(uint16_t node_num, uint32_t test_num, uint8_t ttl)
{
    uint16_t i;

    test_perf_statistics.time = malloc(test_num * sizeof(uint16_t));
    if (test_perf_statistics.time == NULL) {
        ESP_LOGE(TAG, " %s %d, malloc fail\n", __func__, __LINE__);
        return ESP_ERR_NO_MEM;
    }

    test_perf_statistics.package_index = malloc(test_num * sizeof(uint16_t));
    if (test_perf_statistics.package_index == NULL) {
        ESP_LOGE(TAG, " %s %d, malloc fail\n", __func__, __LINE__);
        return ESP_ERR_NO_MEM;
    }
    for (i = 0; i < test_num; i++) {
        test_perf_statistics.time[i] = 0;
        test_perf_statistics.package_index[i] = 0;
    }

    test_perf_statistics.test_num = test_num;
    test_perf_statistics.node_num = node_num;
    test_perf_statistics.ttl = ttl;
    test_perf_statistics.statistics = 0;
    return 0;
}

void ble_mesh_test_performance_client_model_destroy(void)
{
    if (test_perf_statistics.time != NULL) {
        free(test_perf_statistics.time);
    }

    if (test_perf_statistics.package_index != NULL) {
        free(test_perf_statistics.package_index);
    }

    test_perf_statistics.test_num = 0;
    test_perf_statistics.ttl = 0;
    test_perf_statistics.node_num = 0;
    test_perf_statistics.statistics = 0;
}
