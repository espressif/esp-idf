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

#ifndef _ESP_FAST_PROV_CLIENT_MODEL_H
#define _ESP_FAST_PROV_CLIENT_MODEL_H

#include "esp_ble_mesh_defs.h"

enum {
    SEND_SELF_PROV_NODE_ADDR_START, /* Flag indicates the timer used to send self-provisioned node addresses has been started */
    CLI_MAX_FLAGS,
};

#define SEND_SELF_PROV_NODE_ADDR_TIMEOUT    K_SECONDS(5)

void example_send_self_prov_node_addr(struct k_work *work);

esp_err_t example_fast_prov_client_recv_timeout(uint32_t opcode, esp_ble_mesh_model_t *model,
        esp_ble_mesh_msg_ctx_t *ctx);

esp_err_t example_fast_prov_client_recv_status(esp_ble_mesh_model_t *model,
        esp_ble_mesh_msg_ctx_t *ctx,
        uint16_t len, const uint8_t *data);

#endif /* _ESP_FAST_PROV_CLIENT_MODEL_H */