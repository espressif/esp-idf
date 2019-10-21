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

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_networking_api.h"

#include "ble_mesh_console_lib.h"
#include "ble_mesh_adapter.h"

void ble_mesh_register_server_operation(void);

typedef struct {
    struct arg_str *data;
    struct arg_int *opcode;
    struct arg_int *model;
    struct arg_int *role;
    struct arg_end *end;
} ble_mesh_publish_message;
ble_mesh_publish_message msg_publish;

void ble_mesh_register_server(void)
{
    ble_mesh_register_server_operation();
}

int ble_mesh_module_publish_message(int argc, char **argv)
{
    esp_err_t err;
    esp_ble_mesh_model_t *model = NULL;
    uint8_t *data = NULL;
    uint8_t device_role = ROLE_NODE;
    uint16_t length = 0;

    ESP_LOGD(TAG, "enter %s \n", __func__);

    int nerrors = arg_parse(argc, argv, (void **) &msg_publish);
    if (nerrors != 0) {
        arg_print_errors(stderr, msg_publish.end, argv[0]);
        return 1;
    }

    data = malloc(strlen(msg_publish.data->sval[0]));
    get_value_string((char *)msg_publish.data->sval[0], (char *) data);

    arg_int_to_value(msg_publish.role, device_role, "device role");
    model = ble_mesh_get_model(msg_publish.model->ival[0]);

    err = esp_ble_mesh_model_publish(model, msg_publish.opcode->ival[0], length, data, device_role);

    ESP_LOGD(TAG, "exit %s \n", __func__);
    free(data);
    return err;
}

void ble_mesh_register_server_operation(void)
{
    msg_publish.data = arg_str1("d", NULL, "<data>", "message data");
    msg_publish.opcode = arg_int1("o", NULL, "<opcode>", "operation opcode");
    msg_publish.model = arg_int1("m", NULL, "<module>", "module published to");
    msg_publish.role = arg_int1("r", NULL, "<role>", "device role");
    msg_publish.end = arg_end(1);

    const esp_console_cmd_t msg_publish_cmd = {
        .command = "bmpublish",
        .help = "ble mesh: publish message",
        .hint = NULL,
        .func = &ble_mesh_module_publish_message,
        .argtable = &msg_publish,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&msg_publish_cmd));
}

