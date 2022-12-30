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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "freertos/FreeRTOSConfig.h"

#include "esp_ble_mesh_networking_api.h"
#include "ble_mesh_adapter.h"
#include "transaction.h"

typedef struct {
    struct arg_str *action_type;
    struct arg_int *playload_byte;
    struct arg_int *test_num;
    struct arg_int *opcode;
    struct arg_int *unicast_address;
    struct arg_int *ttl;
    struct arg_int *app_idx;
    struct arg_int *net_idx;
    struct arg_int *dev_role;
    struct arg_end *end;
} ble_mesh_test_perf_client_model_t;
ble_mesh_test_perf_client_model_t test_perf_client_model;

typedef struct {
    struct arg_str *action_type;
    struct arg_int *test_size;
    struct arg_int *node_num;
    struct arg_int *ttl;
    struct arg_end *end;
} ble_mesh_test_perf_client_model_statistics_t;
ble_mesh_test_perf_client_model_statistics_t test_perf_client_model_statistics;

bool deinit_flag = false;

void ble_mesh_performance_client_model_command(void);

void ble_mesh_register_mesh_test_performance_client(void)
{
    ble_mesh_performance_client_model_command();
}

void ble_mesh_vendor_model_cb(esp_ble_mesh_model_cb_event_t event, esp_ble_mesh_model_cb_param_t *param)
{
    uint16_t result;
    uint8_t data[4];
    uint64_t *start_time = NULL;
    transaction_t *trans = NULL;

    ESP_LOGD(TAG, "enter %s, event=%x", __func__, event);
    do {
        trans = transaction_get(TRANS_TYPE_MESH_PERF, TRANS_MESH_SEND_MESSAGE, trans);
        if (trans) {
            start_time = (uint64_t *)trans->input;
            break;
        }
    }while(trans);

    switch (event) {
    case ESP_BLE_MESH_MODEL_OPERATION_EVT:
        if (param->model_operation.model != NULL && param->model_operation.model->op != NULL) {
            if (param->model_operation.opcode == ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_SET) {
                ESP_LOGI(TAG, "VndSrvModel:SetAck,Success,%d", param->model_operation.ctx->recv_ttl);
                data[0] = param->model_operation.msg[0];
                data[1] = param->model_operation.msg[1];
                data[2] = param->model_operation.msg[2];
                data[3] = param->model_operation.ctx->recv_ttl;
                result = ble_mesh_node_statistics_accumulate(param->model_operation.msg, param->model_operation.length, VENDOR_MODEL_PERF_OPERATION_TYPE_SET);
                if (result == 0) {
                    esp_ble_mesh_server_model_send_msg(param->model_operation.model, param->model_operation.ctx,
                                                       ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_STATUS, sizeof(data), data);
                }
            } else if (param->model_operation.opcode == ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_SET_UNACK) {
                ESP_LOGI(TAG, "VndSrvModel:SetUnAck,Success,%d,%d", param->model_operation.ctx->recv_ttl, param->model_operation.length);
                result = ble_mesh_node_statistics_accumulate(param->model_operation.msg, param->model_operation.length, VENDOR_MODEL_PERF_OPERATION_TYPE_SET_UNACK);
            } else if (param->model_operation.opcode == ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_STATUS) {
                if (trans) {
                    uint64_t current_time = esp_timer_get_time();
                    result = ble_mesh_test_performance_client_model_accumulate_time(((uint32_t)(current_time - *start_time) / 1000), param->model_operation.msg,
                        param->model_operation.ctx->recv_ttl, param->model_operation.length);
                    transaction_set_events(trans, TRANS_MESH_SEND_MESSAGE_EVT);
                }
            }
        }
        break;
    case ESP_BLE_MESH_MODEL_SEND_COMP_EVT:
        if (param->model_send_comp.opcode == ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_SET_UNACK) {
            transaction_set_events(trans, TRANS_MESH_SEND_MESSAGE_EVT);
        }
        if (param->model_send_comp.err_code == ESP_OK) {
            ESP_LOGI(TAG, "VndModel:ModelSend,OK");
        } else {
            ESP_LOGE(TAG, "VndModel:ModelSend,Fail");
        }
        break;
    case ESP_BLE_MESH_MODEL_PUBLISH_COMP_EVT:
        ESP_LOGI(TAG, "VndModel:PublishSend,OK,0x%x,%d,", param->model_publish_comp.model->model_id, param->model_publish_comp.model->pub->msg->len);
        break;
    case ESP_BLE_MESH_CLIENT_MODEL_RECV_PUBLISH_MSG_EVT:
        ESP_LOGI(TAG, "VndModel:PublishReceive,OK,0x%06x,%d,%d", param->client_recv_publish_msg.opcode, param->client_recv_publish_msg.length, param->client_recv_publish_msg.msg[1]);
        if (trans) {
            uint64_t current_time = esp_timer_get_time();
            result = ble_mesh_test_performance_client_model_accumulate_time(((uint32_t)(current_time - *start_time) / 2000), param->client_recv_publish_msg.msg,
                param->client_recv_publish_msg.ctx->recv_ttl, param->client_recv_publish_msg.length);
            transaction_set_events(trans, TRANS_MESH_SEND_MESSAGE_EVT);
        }
        break;
    case ESP_BLE_MESH_MODEL_PUBLISH_UPDATE_EVT:
        ESP_LOGI(TAG, "VndModel:PublishUpdate,OK");
        break;
    case ESP_BLE_MESH_CLIENT_MODEL_SEND_TIMEOUT_EVT:
        ESP_LOGI(TAG, "VndModel:TimeOut,0x%06x", param->client_send_timeout.opcode);
        if (trans) {
            transaction_set_events(trans, TRANS_MESH_SEND_MESSAGE_EVT);
        }
        break;
    case ESP_BLE_MESH_MODEL_EVT_MAX:
        ESP_LOGI(TAG, "VndModel:MaxEvt");
        break;
    default:
        break;
    }

}

void ble_mesh_test_performance_client_model_throughput(void *params)
{
    uint16_t i;
    uint8_t *data = NULL;
    uint64_t start_time;
    esp_ble_mesh_msg_ctx_t ctx;
    transaction_t *trans = NULL;
    ble_mesh_test_perf_throughput_data *profile_context = (ble_mesh_test_perf_throughput_data *)params;
    esp_err_t result = ESP_OK;

    ctx.net_idx = profile_context->net_idx;
    ctx.app_idx = profile_context->app_idx;
    ctx.addr = profile_context->address;
    ctx.send_ttl = profile_context->ttl;
    ctx.model = profile_context->model;
    ctx.send_rel = 0;
    test_perf_statistics.test_length = profile_context->length;
    // create send data
    data = malloc(profile_context->length);
    if (data == NULL) {
        ESP_LOGE(TAG, " %s, %d, malloc fail", __func__, __LINE__);
        goto cleanup;
    }

    for (i = 1; i <= profile_context->test_num; i++) {
        ble_mesh_create_send_data((char *)data, profile_context->length, i, profile_context->opcode);
        start_time = esp_timer_get_time();
        TRANSACTION_INIT(&trans, TRANS_TYPE_MESH_PERF, TRANS_MESH_SEND_MESSAGE,
                    TRANS_MESH_SEND_MESSAGE_EVT, SEND_MESSAGE_TIMEOUT, &start_time, NULL);
        //tx: data  profile_context->length
        result = esp_ble_mesh_client_model_send_msg(profile_context->model, &ctx, profile_context->opcode,
                                                    profile_context->length, data, 8000,
                                                    profile_context->need_ack,
                                                    profile_context->device_role);

        if (result != ESP_OK) {
            ESP_LOGE(TAG, "VendorModel:SendPackage,Fail");
        }

        ble_mesh_test_performance_client_model_accumulate_statistics(profile_context->length);

        if (deinit_flag) {
            ESP_LOGI(TAG, "Already deinit, stop sending message");
            break;
        }

        result = transaction_run(trans);
        if (result == ESP_ERR_INVALID_STATE) {
            ESP_LOGI(TAG, "Already deinit, transactions abort");
            break;
        }
    }

    ESP_LOGI(TAG, "VendorModel:SendPackage,Finish");

cleanup:
    free(params);
    if (data != NULL) {
        free(data);
    }
    vTaskDelete(NULL);
}

int ble_mesh_test_performance_client_model(int argc, char **argv)
{
    esp_ble_mesh_elem_t *element = NULL;
    esp_ble_mesh_model_t *model;
    esp_err_t result = ESP_OK;
    ble_mesh_test_perf_throughput_data *profile_data = NULL;
    uint16_t company_id = CID_ESP;

    int nerrors = arg_parse(argc, argv, (void **) &test_perf_client_model);
    if (nerrors != 0) {
        arg_print_errors(stderr, test_perf_client_model.end, argv[0]);
        return 1;
    }

    element = esp_ble_mesh_find_element(esp_ble_mesh_get_primary_element_address());
    if (!element) {
        ESP_LOGE(TAG, "Element 0x%04x not exists", esp_ble_mesh_get_primary_element_address());
        return ESP_FAIL;
    }

    model = esp_ble_mesh_find_vendor_model(element, company_id, ESP_BLE_MESH_VND_MODEL_ID_TEST_PERF_CLI);
    if (!model) {
        ESP_LOGE(TAG, "VendorClient:LoadModel,Fail");
        return ESP_FAIL;
    }

    if (strcmp(test_perf_client_model.action_type->sval[0], "init") == 0) {
        result = esp_ble_mesh_client_model_init(model);
        if (result == ESP_OK) {
            ESP_LOGI(TAG, "VendorClientModel:Init,OK");
        } else {
            ESP_LOGE(TAG, "VendorClientModel:Init,Fail,%d", result);
        }
    } else if (strcmp(test_perf_client_model.action_type->sval[0], "start") == 0) {
        profile_data = malloc(sizeof(ble_mesh_test_perf_throughput_data));
        profile_data->model = model;
        if (profile_data == NULL) {
            ESP_LOGE(TAG, " %s, %d malloc fail", __func__, __LINE__);
            return ESP_ERR_NO_MEM;
        }

        arg_int_to_value(test_perf_client_model.playload_byte, profile_data->length, "length");
        arg_int_to_value(test_perf_client_model.opcode, profile_data->opcode, "opcode");
        arg_int_to_value(test_perf_client_model.unicast_address, profile_data->address, "publish address");
        arg_int_to_value(test_perf_client_model.ttl, profile_data->ttl, "model ttl");
        arg_int_to_value(test_perf_client_model.app_idx, profile_data->app_idx, "appkey index");
        arg_int_to_value(test_perf_client_model.net_idx, profile_data->net_idx, "network key index");
        arg_int_to_value(test_perf_client_model.dev_role, profile_data->device_role, "device role");
        arg_int_to_value(test_perf_client_model.test_num, profile_data->test_num, "test number");

        if (profile_data->opcode == ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_SET) {
            profile_data->need_ack = true;
        } else {
            profile_data->need_ack = false;
        }

        xTaskCreate(ble_mesh_test_performance_client_model_throughput, "MESHTHROUGHPUTSEND", 4048, profile_data, 1, NULL);
    }

    return result;
}

int ble_mesh_test_performance_client_model_performance(int argc, char **argv)
{
    uint8_t result = 0;

    int nerrors = arg_parse(argc, argv, (void **) &test_perf_client_model_statistics);
    if (nerrors != 0) {
        arg_print_errors(stderr, test_perf_client_model_statistics.end, argv[0]);
        return 1;
    }

    if (strcmp(test_perf_client_model_statistics.action_type->sval[0], "init") == 0) {
        result = ble_mesh_test_performance_client_model_init(test_perf_client_model_statistics.node_num->ival[0],
                 test_perf_client_model_statistics.test_size->ival[0], test_perf_client_model_statistics.ttl->ival[0]);
        if (result == 0) {
            ESP_LOGI(TAG, "VendorPerfTest:InitStatistics,OK");
        }
    } else if (strcmp(test_perf_client_model_statistics.action_type->sval[0], "get") == 0) {
        ble_mesh_test_performance_client_model_get();
    } else if (strcmp(test_perf_client_model_statistics.action_type->sval[0], "destroy") == 0) {
        ble_mesh_test_performance_client_model_destroy();
        ESP_LOGI(TAG, "VendorPerfTest:DestroyStatistics,OK");
    } else if (strcmp(test_perf_client_model_statistics.action_type->sval[0], "percent") == 0) {
        ble_mesh_test_performance_client_model_get_received_percent();
        ESP_LOGI(TAG, "VendorPerfTest:GetPercent,OK");
    }

    return 0;
}

void ble_mesh_performance_client_model_command(void)
{
    test_perf_client_model.action_type = arg_str1("z", NULL, "<action>", "action type");
    test_perf_client_model.playload_byte = arg_int0("p", NULL, "<byte>", "playload byte");
    test_perf_client_model.test_num = arg_int0("n", NULL, "<number>", "test number");
    // set test num default to 1000
    test_perf_client_model.test_num->ival[0] = 1000;
    test_perf_client_model.opcode = arg_int0("o", NULL, "<opcode>", "opcode");
    test_perf_client_model.unicast_address = arg_int0("u", NULL, "<address>", "unicast address");
    test_perf_client_model.ttl = arg_int0("t", NULL, "<ttl>", "ttl");
    test_perf_client_model.app_idx = arg_int0("a", NULL, "<appkey>", "appkey index");
    test_perf_client_model.net_idx = arg_int0("i", NULL, "<network key>", "network key index");
    test_perf_client_model.dev_role = arg_int0("d", NULL, "<role>", "device role");
    test_perf_client_model.dev_role->ival[0] = ROLE_PROVISIONER;
    test_perf_client_model.end = arg_end(1);

    const esp_console_cmd_t test_perf_client_model_cmd = {
        .command = "bmtpcvm",
        .help = "ble mesh test performance client vendor model",
        .hint = NULL,
        .func = &ble_mesh_test_performance_client_model,
        .argtable = &test_perf_client_model,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&test_perf_client_model_cmd));

    test_perf_client_model_statistics.action_type = arg_str1("z", NULL, "<action>", "action type");
    test_perf_client_model_statistics.test_size = arg_int0("s", NULL, "<test size>", "test size");
    test_perf_client_model_statistics.node_num = arg_int0("n", NULL, "<node number>", "node number");
    test_perf_client_model_statistics.ttl = arg_int0("l", NULL, "<test number>", "ttl");
    test_perf_client_model_statistics.end = arg_end(1);

    const esp_console_cmd_t test_perf_client_model_performance_cmd = {
        .command = "bmcperf",
        .help = "ble mesh client: test performance",
        .hint = NULL,
        .func = &ble_mesh_test_performance_client_model_performance,
        .argtable = &test_perf_client_model_statistics,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&test_perf_client_model_performance_cmd));
}
