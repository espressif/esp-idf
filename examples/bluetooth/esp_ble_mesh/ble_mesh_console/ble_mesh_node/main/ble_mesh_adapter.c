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

#include "ble_mesh_adapter.h"

esp_ble_mesh_model_t *ble_mesh_get_model(uint16_t model_id)
{
    esp_ble_mesh_model_t *model = NULL;

    switch (model_id) {
    case ESP_BLE_MESH_MODEL_ID_CONFIG_SRV:
        model = &config_server_models[0];
        break;
    case ESP_BLE_MESH_MODEL_ID_CONFIG_CLI:
        model = &config_client_models[0];
        break;
    case ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV:
        model = &gen_onoff_srv_models[1];
        break;
#if (CONFIG_BLE_MESH_GENERIC_ONOFF_CLI)
    case ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_CLI:
        model = &gen_onoff_cli_models[1];
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
        ESP_LOGE(TAG, "%s init fail, mesh node semaphore create fail", __func__);
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

void ble_mesh_node_statistics_get(void)
{
    xSemaphoreTake(ble_mesh_node_sema, portMAX_DELAY);
    ESP_LOGI(TAG, "statistics:%d,%d\n", ble_mesh_node_statistics.statistics, ble_mesh_node_statistics.package_num);
    xSemaphoreGive(ble_mesh_node_sema);
}

int ble_mesh_node_statistics_accumultate(uint8_t *data, uint32_t value, uint16_t type)
{
    uint16_t i;
    uint16_t sequence_num = (data[0] << 8) | data[1];

    xSemaphoreTake(ble_mesh_node_sema, portMAX_DELAY);
    for (i = 0; i < ble_mesh_node_statistics.total_package_num; i++) {
        if (ble_mesh_node_statistics.package_index[i] == sequence_num) {
            xSemaphoreGive(ble_mesh_node_sema);
            return 1;
        }
    }

    // package type wrong
    if (data[2] != type) {
        xSemaphoreGive(ble_mesh_node_sema);
        return 1;
    }

    for (i = 0; i < ble_mesh_node_statistics.total_package_num; i++) {
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
        return 1;
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
