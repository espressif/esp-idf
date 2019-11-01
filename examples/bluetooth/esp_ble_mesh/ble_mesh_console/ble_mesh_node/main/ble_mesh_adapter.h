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

#ifndef _BLE_MESH_ADAPTER_H_
#define _BLE_MESH_ADAPTER_H_

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "ble_mesh_console_lib.h"
#include "ble_mesh_cfg_srv_model.h"

#define TAG "ble_mesh_node_console"

typedef enum {
    VENDOR_MODEL_PERF_OPERATION_TYPE_GET = 1,
    VENDOR_MODEL_PERF_OPERATION_TYPE_SET,
    VENDOR_MODEL_PERF_OPERATION_TYPE_SET_UNACK
} ble_mesh_perf_operation_type;

typedef struct {
    uint8_t current;
    uint8_t previous;
    char *name;
} ble_mesh_node_status;

typedef struct {
    uint32_t statistics;
    uint32_t package_num;
    uint16_t *package_index;
    uint32_t total_package_num;
} ble_mesh_node_statistics_t;
ble_mesh_node_statistics_t ble_mesh_node_statistics;

extern SemaphoreHandle_t ble_mesh_node_sema;

#define arg_int_to_value(src_msg, dst_msg, message) do { \
    if (src_msg->count != 0) {\
        ESP_LOGD(TAG, "\n%s, %s\n", __func__, message);\
        dst_msg = src_msg->ival[0];\
    } \
} while(0) \

#define ble_mesh_node_get_value(index, key, value) do { \
    uint16_t _index = 0; \
    xSemaphoreTake(ble_mesh_node_sema, portMAX_DELAY); \
    for (_index = 0; _index < NODE_MAX_GROUP_CONFIG; _index) { \
        if (node_set_prestore_params[_index].key == value) { \
            break; \
        } \
    } \
    index = _index; \
    xSemaphoreGive(ble_mesh_node_sema); \
} while(0) \

#define ble_mesh_node_set_state(status) do { \
    xSemaphoreTake(ble_mesh_node_sema, portMAX_DELAY); \
    node_status.previous = node_status.current; \
    node_status.current = status; \
    xSemaphoreGive(ble_mesh_node_sema); \
}while(0) \

#define ble_mesh_node_get_state(status) do { \
    xSemaphoreTake(ble_mesh_node_sema, portMAX_DELAY); \
    status = node_status.current; \
    xSemaphoreGive(ble_mesh_node_sema); \
}while(0) \

#define ble_mesh_callback_check_err_code(err_code, message) do { \
    if (err_code == ESP_OK) { \
        ESP_LOGI(TAG, "%s,OK\n", message); \
    } else { \
        ESP_LOGE(TAG, "%s,Fail,%d\n", message, err_code); \
    } \
}while(0) \

void ble_mesh_node_init(void);
void ble_mesh_set_node_prestore_params(uint16_t netkey_index, uint16_t unicast_addr);
esp_ble_mesh_model_t *ble_mesh_get_model(uint16_t model_id);
esp_ble_mesh_comp_t *ble_mesh_get_component(uint16_t model_id);
void ble_mesh_node_statistics_get(void);
int ble_mesh_node_statistics_accumultate(uint8_t *data, uint32_t value, uint16_t type);
int ble_mesh_node_statistics_init(uint16_t package_num);
void ble_mesh_node_statistics_destroy(void);

#endif //_BLE_MESH_ADAOTER_H_
