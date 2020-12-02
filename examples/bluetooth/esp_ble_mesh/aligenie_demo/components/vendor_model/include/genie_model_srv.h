// Copyright (C) 2018-2020 Alibaba Group Holding Limited
// Adaptations to ESP-IDF Copyright (c) 2020 Espressif Systems (Shanghai) Co. Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _GENIE_MODEL_SRV_H
#define _GENIE_MODEL_SRV_H

#include "genie_dlist.h"
#include "genie_util.h"

#ifdef __cplusplus
extern "C" {
#endif /**< __cplusplus */

#define GENIE_VENDOR_MSG_LIST_MAXSIZE 8

typedef struct _genie_vendor_model_msg_node {
    genie_dnode_t node;
    int8_t left_retry;
    int64_t timeout;
    genie_model_msg_t msg;
} genie_model_msg_node_t;

typedef void (* genie_model_opcode_cb_t)(esp_ble_mesh_model_t *model, esp_ble_mesh_msg_ctx_t *ctx, struct net_buf_simple *buf);

typedef struct genie_opcode_cb_t {
    uint32_t opcode;
    genie_model_opcode_cb_t cb;
} genie_opcode_cb_t;

/** @def genie_model_msg_send
 *
 *  @brief send the vendor model message
 *
 *  @param pointer to the message to be sent
 *
 *  @return 0 for success; negative for failure
 */
esp_err_t genie_model_msg_send(genie_model_msg_t *p_model_msg);

/**
 * @brief
 * @return
 */
void genie_model_dispatch(uint32_t opcode, esp_ble_mesh_model_t *model,
                          esp_ble_mesh_msg_ctx_t *ctx, uint8_t *msg, uint16_t length);

#ifdef __cplusplus
}
#endif /**< __cplusplus */

#endif /* _GENIE_MODEL_SRV_H */
