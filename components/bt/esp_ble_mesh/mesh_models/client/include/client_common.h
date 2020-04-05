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

#ifndef _CLIENT_COMMON_H_
#define _CLIENT_COMMON_H_

#include "mesh_access.h"

/** Client model opcode pair table */
typedef struct {
    u32_t cli_op;       /* Client message opcode */
    u32_t status_op;    /* Corresponding status message opcode */
} bt_mesh_client_op_pair_t;

/** Client model user data context */
typedef struct {
    /** Pointer to the client model */
    struct bt_mesh_model *model;

    /** Size of the opcode pair table */
    int op_pair_size;

    /** Pointer to the opcode pair table */
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

    /** Pointer to the internal data of client model */
    void *internal_data;

    /** Role of the device to which the client model belongs */
    u8_t msg_role;
} bt_mesh_client_user_data_t;

/** Client model internal data context */
typedef struct  {
    sys_slist_t queue;
} bt_mesh_client_internal_data_t;

/** Client model sending message related context */
typedef struct {
    sys_snode_t client_node;
    struct bt_mesh_msg_ctx ctx;     /* Message context */
    u32_t opcode;                   /* Message opcode */
    u32_t op_pending;               /* Expected status message opcode */
    s32_t timeout;                  /* Calculated message timeout value */
    struct k_delayed_work timer;    /* Time used to get response. Only for internal use. */
} bt_mesh_client_node_t;

/** Client model sending message parameters */
typedef struct {
    u32_t opcode;                       /* Message opcode */
    struct bt_mesh_model *model;        /* Pointer to the client model */
    struct bt_mesh_msg_ctx ctx;         /* Message context */
    s32_t msg_timeout;                  /* Time to get corresponding response */
    const struct bt_mesh_send_cb *cb;   /* User defined callback function */
    void *cb_data;                      /* User defined callback value */
} bt_mesh_client_common_param_t;

void bt_mesh_client_model_lock(void);

void bt_mesh_client_model_unlock(void);

int bt_mesh_client_init(struct bt_mesh_model *model);

int bt_mesh_client_deinit(struct bt_mesh_model *model);

/**
 * @brief Check if the msg received by client model is a publish msg or not
 *
 * @param model     Mesh (client) Model that the message belongs to.
 * @param ctx       Message context, includes keys, TTL, etc.
 * @param buf       The message buffer
 * @param need_pub  Indicate if the msg sent to app layer as a publish msg
 * @return 0 on success, or (negative) error code on failure.
 */
bt_mesh_client_node_t *bt_mesh_is_client_recv_publish_msg(
    struct bt_mesh_model *model,
    struct bt_mesh_msg_ctx *ctx,
    struct net_buf_simple *buf, bool need_pub);

int bt_mesh_client_send_msg(struct bt_mesh_model *model,
                            u32_t opcode,
                            struct bt_mesh_msg_ctx *ctx,
                            struct net_buf_simple *msg,
                            k_work_handler_t timer_handler,
                            s32_t timeout, bool need_ack,
                            const struct bt_mesh_send_cb *cb, void *cb_data);

int bt_mesh_client_free_node(bt_mesh_client_node_t *node);

int bt_mesh_client_clear_list(void *data);

typedef struct {
    struct bt_mesh_model *model;    /* The client model structure */
    u8_t  role;                     /* Role of the device - Node/Provisioner */
} bt_mesh_role_param_t;

/**
 * @brief This function copies node_index for stack internal use.
 *
 * @param[in] common: Pointer to the bt_mesh_role_param_t structure
 *
 * @return Zero - success, otherwise - fail
 */
int bt_mesh_set_client_model_role(bt_mesh_role_param_t *common);

#endif /* _CLIENT_COMMON_H_ */

