/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _CLIENT_COMMON_H_
#define _CLIENT_COMMON_H_

#include "mesh/access.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Client model opcode pair table */
typedef struct {
    uint32_t cli_op;    /* Client message opcode */
    uint32_t status_op; /* Corresponding status message opcode */
} bt_mesh_client_op_pair_t;

/** Client model user data context */
typedef struct {
    /** Pointer to the client model */
    struct bt_mesh_model *model;

    /** Size of the opcode pair table */
    uint32_t op_pair_size;

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
    void (*publish_status)(uint32_t opcode, struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf);

    /** Pointer to the internal data of client model */
    void *internal_data;

    /** Pointer to the vendor data of client model */
    void *vendor_data;

    /** Role of the device to which the client model belongs */
    uint8_t msg_role __attribute__((deprecated));
} bt_mesh_client_user_data_t;

/** Client model internal data context */
typedef struct {
    sys_slist_t queue;
} bt_mesh_client_internal_data_t;

/** Client model sending message related context */
typedef struct {
    sys_snode_t client_node;
    struct bt_mesh_model *model;    /* Pointer to the client model */
    struct bt_mesh_msg_ctx ctx;     /* Message context */
    uint32_t opcode;                /* Message opcode */
    uint32_t op_pending;            /* Expected status message opcode */
    int32_t  timeout;               /* Calculated message timeout value */
    struct k_delayed_work timer;    /* Time used to get response. Only for internal use. */
} bt_mesh_client_node_t;

/** Client model sending message parameters */
typedef struct {
    uint32_t opcode;                    /* Message opcode */
    struct bt_mesh_model *model;        /* Pointer to the client model */
    struct bt_mesh_msg_ctx ctx;         /* Message context */
    int32_t msg_timeout;                /* Time to get corresponding response */
    uint8_t msg_role __attribute__((deprecated));   /* Role (Node/Provisioner) of the device */
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
bt_mesh_client_node_t *bt_mesh_is_client_recv_publish_msg(struct bt_mesh_model *model,
                                                          struct bt_mesh_msg_ctx *ctx,
                                                          struct net_buf_simple *buf,
                                                          bool need_pub);

int bt_mesh_client_send_msg(bt_mesh_client_common_param_t *param,
                            struct net_buf_simple *msg, bool need_ack,
                            k_work_handler_t timer_handler);

int bt_mesh_client_free_node(bt_mesh_client_node_t *node);

int bt_mesh_client_clear_list(void *data);

#ifdef __cplusplus
}
#endif

#endif /* _CLIENT_COMMON_H_ */
