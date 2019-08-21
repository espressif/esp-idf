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

esp_ble_mesh_model_t *ble_mesh_get_model(uint16_t model_id)
{
    esp_ble_mesh_model_t *model = NULL;

    switch (model_id) {
    case ESP_BLE_MESH_MODEL_ID_CONFIG_SRV:
        model = config_server_models;
        break;
#if (CONFIG_BLE_MESH_CFG_CLI)
    case ESP_BLE_MESH_MODEL_ID_CONFIG_CLI:
        model = &gen_onoff_cli_models[1];
        break;
#endif
    case ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV:
        model = &gen_onoff_srv_models[1];
        break;
#if (CONFIG_BLE_MESH_GENERIC_ONOFF_CLI)
    case ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_CLI:
        model = &gen_onoff_cli_models[2];
        break;
#endif
    case ESP_BLE_MESH_VND_MODEL_ID_TEST_PERF_CLI:
        model = &test_perf_cli_models[0];
        break;
    case ESP_BLE_MESH_VND_MODEL_ID_TEST_PERF_SRV:
        model = &test_perf_srv_models[0];
        break;
    }

    return model;
}

esp_ble_mesh_comp_t *ble_mesh_get_component(uint16_t model_id)
{
    esp_ble_mesh_comp_t *comp = NULL;

    switch (model_id) {
    case ESP_BLE_MESH_MODEL_ID_CONFIG_SRV:
        comp = &config_server_comp;
        break;
    case ESP_BLE_MESH_MODEL_ID_CONFIG_CLI:
        comp = &config_client_comp;
        break;
    case ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV:
        comp = &gen_onoff_srv_comp;
        break;
#if (CONFIG_BLE_MESH_GENERIC_ONOFF_CLI)
    case ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_CLI:
        comp = &gen_onoff_cli_comp;
        break;
#endif
    case ESP_BLE_MESH_VND_MODEL_ID_TEST_PERF_CLI:
        comp = &test_perf_cli_comp;
        break;
    case ESP_BLE_MESH_VND_MODEL_ID_TEST_PERF_SRV:
        comp = &test_perf_srv_comp;
        break;
    }

    return comp;
}

void ble_mesh_node_init(void)
{
    uint16_t i;

    for (i = 0; i < NODE_MAX_GROUP_CONFIG; i++) {
        ble_mesh_node_prestore_params[i].net_idx = 0xFFFF;
        ble_mesh_node_prestore_params[i].unicast_addr = 0xFFFF;
    }

    ble_mesh_node_sema = xSemaphoreCreateMutex();
    if (!ble_mesh_node_sema) {
        ESP_LOGE(TAG, "%s failed to init, failed to create mesh node semaphore", __func__);
    }
}

void ble_mesh_set_node_prestore_params(uint16_t netkey_index, uint16_t unicast_addr)
{
    uint16_t i;
    xSemaphoreTake(ble_mesh_node_sema, portMAX_DELAY);
    for (i = 0; i < NODE_MAX_GROUP_CONFIG; i++) {
        if (ble_mesh_node_prestore_params[i].net_idx != 0xFFFF && ble_mesh_node_prestore_params[i].unicast_addr != 0xFFFF) {
            ble_mesh_node_prestore_params[i].net_idx = netkey_index;
            ble_mesh_node_prestore_params[i].unicast_addr = unicast_addr;
        }
    }
    xSemaphoreGive(ble_mesh_node_sema);
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
    uint32_t i, j;
    uint32_t sum_time = 0;

    xSemaphoreTake(ble_mesh_test_perf_sema, portMAX_DELAY);

    for (i = 0, j = 0; i < test_perf_statistics.test_num; i++) {
        if (test_perf_statistics.time[i] != 0) {
            sum_time += test_perf_statistics.time[i];
            j += 1;
        } else {
            continue;
        }

        if (j == test_perf_statistics.test_num - 1) {
            break;
        }
    }

    ESP_LOGI(TAG, "VendorModel:Statistics,%d,%d\n",
             test_perf_statistics.statistics, (sum_time / (j + 1)));

    xSemaphoreGive(ble_mesh_test_perf_sema);
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

    xSemaphoreTake(ble_mesh_test_perf_sema, portMAX_DELAY);

    time_level_num = ((max_time - min_time) / 50 + 1);
    statistics_time_percent = malloc(sizeof(statistics_time_performance) * time_level_num);

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
        printf(",%d:%d", statistics_time_percent[j].time_level, statistics_time_percent[j].time_num);
    }
    printf("\n");

    free(statistics_time_percent);
    xSemaphoreGive(ble_mesh_test_perf_sema);
}

void ble_mesh_test_performance_client_model_accumulate_statistics(uint32_t value)
{
    xSemaphoreTake(ble_mesh_test_perf_sema, portMAX_DELAY);
    test_perf_statistics.statistics += value;
    xSemaphoreGive(ble_mesh_test_perf_sema);
}

int ble_mesh_test_performance_client_model_accumulate_time(uint16_t time, uint8_t *data, uint8_t ack_ttl, uint16_t length)
{
    uint16_t i;
    uint16_t sequence_num = 0;
    uint16_t node_received_ttl = 0;
    xSemaphoreTake(ble_mesh_test_perf_sema, portMAX_DELAY);

    // received fail
    if (length != test_perf_statistics.test_length) {
        xSemaphoreGive(ble_mesh_test_perf_sema);
        return 1;
    }

    if (data != NULL) {
        sequence_num = (data[0] << 8) | data[1];
        if (data[2] == VENDOR_MODEL_PERF_OPERATION_TYPE_SET) {
            node_received_ttl = data[3];
        }
    }

    for (i = 0; i < test_perf_statistics.test_num; i++) {
        if (test_perf_statistics.package_index[i] == sequence_num) {
            xSemaphoreGive(ble_mesh_test_perf_sema);
            return 1;
        }
    }

    for (i = 0; i < test_perf_statistics.test_num; i++) {
        if (test_perf_statistics.package_index[i] == 0) {
            test_perf_statistics.package_index[i] = sequence_num;
            if (data[2] == VENDOR_MODEL_PERF_OPERATION_TYPE_SET) {
                if (node_received_ttl == test_perf_statistics.ttl && ack_ttl == test_perf_statistics.ttl) {
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

    xSemaphoreGive(ble_mesh_test_perf_sema);
    return 0;
}

int ble_mesh_test_performance_client_model_init(uint16_t node_num, uint32_t test_num, uint8_t ttl)
{
    uint16_t i;

    // malloc time
    test_perf_statistics.time = malloc(test_num * sizeof(uint16_t));
    if (test_perf_statistics.time == NULL) {
        ESP_LOGE(TAG, " %s %d, malloc fail\n", __func__, __LINE__);
        return 1;
    }

    test_perf_statistics.package_index = malloc(test_num * sizeof(uint16_t));
    if (test_perf_statistics.package_index == NULL) {
        ESP_LOGE(TAG, " %s %d, malloc fail\n", __func__, __LINE__);
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
