/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_MESH_FAST_PROV_CLIENT_MODEL_H_
#define _BLE_MESH_FAST_PROV_CLIENT_MODEL_H_

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_FAST_PROV_CLIENT_MODEL_H_ */
