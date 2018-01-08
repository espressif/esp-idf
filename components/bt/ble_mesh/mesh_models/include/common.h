// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
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

/** @file
 *  @brief Bluetooth Mesh Model Common APIs.
 */

#ifndef __BT_MESH_COMMON_H
#define __BT_MESH_COMMON_H

#include "../../../bluedroid/osi/include/osi/allocator.h"
#include "mesh_access.h"
#include "mesh_kernel.h"
#include "mesh_main.h"
#include "provisioner_main.h"

struct bt_mesh_common_param {
    u32_t opcode;                     /* Message opcode           */
    struct bt_mesh_model *model;      /* Pointer to cli structure */
    struct bt_mesh_msg_ctx ctx;       /* Message context */
    s32_t msg_timeout;                /* Time to get response messages */
    const struct bt_mesh_send_cb *cb; /* User defined callback function       */
    void *cb_data;                    /* Data as parameter of the cb function */
};

enum {
    NODE = 0,
    PROVISIONER,
};

#define ROLE_NVAL 0xFF

typedef struct bt_mesh_role_param {
    struct bt_mesh_model *model;    /* The client model structure */
    u8_t  role;                     /* Role of the device - Node/Provisioner */
} bt_mesh_role_param_t;

/**
 * @brief This function is called to allocate memory to store sending message.
 *
 * @param[in] size: Length of memory allocated to store message value
 *
 * @return NULL-fail, pointer of a net_buf_simple structure-success
 */
struct net_buf_simple *bt_mesh_alloc_buf(u16_t size);

/**
 * @brief This function is called to free the memory allocated for the sending message.
 *
 * @param[in] buf: Pointer of the net_buf_simple structure to be freed
 *
 * @return none
 */
void bt_mesh_free_buf(struct net_buf_simple *buf);

/**
 * @brief This function is called to copy node_index for stack internal use.
 *
 * @param[in] common: Pointer of the struct bt_mesh_role_param structure
 *
 * @return Zero - success, other - fail
 */
int bt_mesh_copy_msg_role(bt_mesh_role_param_t *common);

/**
 * @brief This function is called to get msg role for stack internal use.
 *
 * @param[in] model:    Pointer of the model structure
 * @param[in] srv_send: Indicate if the message is sent by a server model
 *
 * @return 0 - Node, 1 - Provisioner
 */
u8_t bt_mesh_get_msg_role(struct bt_mesh_model *model, bool srv_send);

#endif /* __BT_MESH_COMMON_H */