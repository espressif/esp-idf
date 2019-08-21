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

#ifndef _MODEL_COMMON_H_
#define _MODEL_COMMON_H_

#include "mesh_access.h"

/** Mesh Client Model Context */
typedef struct {
    u32_t cli_op;              /* The client opcode */
    u32_t status_op;           /* The server status opcode corresponding to the client opcode */
} bt_mesh_client_op_pair_t;

/** Mesh Client Model Context */
typedef struct {
    struct bt_mesh_model *model;
    int op_pair_size; /* the size of op_pair */
    const bt_mesh_client_op_pair_t *op_pair;
    /**
     * @brief This function is a callback function used to push the received unsolicited
     *        messages to the application layer.
     *
     * @param[in]  opcode: Opcode of received status message
     * @param[in]  model:  Model associated with the status message
     * @param[in]  ctx:    Context information of the status message
     * @param[in]  buf:    Buffer contains the status message value
     *
     * @return None
     */
    void (*publish_status)(u32_t opcode, struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf);
    void *internal_data; /* Pointer of the structure of internal data */
    u8_t msg_role; /* device role of the tx message */
} bt_mesh_client_common_t;

typedef struct  {
    sys_slist_t queue;
} bt_mesh_internal_data_t;

typedef struct {
    sys_snode_t client_node;
    struct bt_mesh_msg_ctx ctx;
    u32_t opcode;     /* Indicate the opcode of the message sending */
    u32_t op_pending; /* Indicate the status message waiting for    */
    struct k_delayed_work timer; /* Message send Timer. Only for stack-internal use. */
} bt_mesh_client_node_t;

int bt_mesh_client_init(struct bt_mesh_model *model);

/**
 * @brief Check the msg is a publish msg or not
 *
 * @param model     Mesh (client) Model that the message belongs to.
 * @param ctx       Message context, includes keys, TTL, etc.
 * @param buf       The message buffer
 * @param need_pub  Indicate if the msg sent to app layer as a publish msg
 * @return 0 on success, or (negative) error code on failure.
 */
bt_mesh_client_node_t *bt_mesh_is_model_message_publish(struct bt_mesh_model *model,
        struct bt_mesh_msg_ctx *ctx,
        struct net_buf_simple *buf,
        bool need_pub);

bool bt_mesh_client_find_opcode_in_list(sys_slist_t *list, u32_t opcode);

bool bt_mesh_client_check_node_in_list(sys_slist_t *list, uint16_t tx_dst);

bt_mesh_client_node_t *bt_mesh_client_pick_node(sys_slist_t *list, u16_t tx_dst);

int bt_mesh_client_send_msg(struct bt_mesh_model *model,
                            u32_t opcode,
                            struct bt_mesh_msg_ctx *ctx,
                            struct net_buf_simple *msg,
                            k_work_handler_t timer_handler,
                            s32_t timeout, bool need_ack,
                            const struct bt_mesh_send_cb *cb, void *cb_data);

int bt_mesh_client_free_node(sys_slist_t *queue, bt_mesh_client_node_t *node);

enum {
    NODE = 0,
    PROVISIONER,
    FAST_PROV,
};

#define ROLE_NVAL 0xFF

struct bt_mesh_common_param {
    u32_t opcode;                     /* Message opcode           */
    struct bt_mesh_model *model;      /* Pointer to cli structure */
    struct bt_mesh_msg_ctx ctx;       /* Message context */
    s32_t msg_timeout;                /* Time to get response messages */
    const struct bt_mesh_send_cb *cb; /* User defined callback function       */
    void *cb_data;                    /* Data as parameter of the cb function */
};

typedef struct bt_mesh_role_param {
    struct bt_mesh_model *model;    /* The client model structure */
    u8_t  role;                     /* Role of the device - Node/Provisioner */
} bt_mesh_role_param_t;

/**
 * @brief This function copies node_index for stack internal use.
 *
 * @param[in] common: Pointer to the struct bt_mesh_role_param structure
 *
 * @return Zero - success, otherwise - fail
 */
int bt_mesh_set_model_role(bt_mesh_role_param_t *common);

/**
 * @brief This function gets msg role for stack internal use.
 *
 * @param[in] model:    Pointer to the model structure
 * @param[in] srv_send: Indicate if the message is sent by a server model
 *
 * @return 0 - Node, 1 - Provisioner
 */
u8_t bt_mesh_get_model_role(struct bt_mesh_model *model, bool srv_send);

#endif /* _MODEL_COMMON_H_ */

