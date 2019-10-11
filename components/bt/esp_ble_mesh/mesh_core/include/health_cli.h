/** @file
 *  @brief Bluetooth Mesh Health Client Model APIs.
 */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _BLE_MESH_HEALTH_CLI_H_
#define _BLE_MESH_HEALTH_CLI_H_

#include "mesh_access.h"
#include "mesh_kernel.h"
#include "client_common.h"

/**
 * @brief Bluetooth Mesh
 * @defgroup bt_mesh_health_cli Bluetooth Mesh Health Client Model
 * @ingroup bt_mesh
 * @{
 */

/* Health client model common structure */
typedef bt_mesh_client_user_data_t      bt_mesh_health_client_t;
typedef bt_mesh_client_internal_data_t  health_internal_data_t;

extern const struct bt_mesh_model_op bt_mesh_health_cli_op[];

#define BLE_MESH_MODEL_HEALTH_CLI(cli_data)             \
        BLE_MESH_MODEL(BLE_MESH_MODEL_ID_HEALTH_CLI,    \
            bt_mesh_health_cli_op, NULL, cli_data)

int bt_mesh_health_cli_set(struct bt_mesh_model *model);

int bt_mesh_health_fault_get(struct bt_mesh_msg_ctx *ctx, u16_t cid);

int bt_mesh_health_fault_clear(struct bt_mesh_msg_ctx *ctx, u16_t cid,
                               bool need_ack);

int bt_mesh_health_fault_test(struct bt_mesh_msg_ctx *ctx,
                              u16_t cid, u8_t test_id, bool need_ack);

int bt_mesh_health_period_get(struct bt_mesh_msg_ctx *ctx);

int bt_mesh_health_period_set(struct bt_mesh_msg_ctx *ctx,
                              u8_t divisor, bool need_ack);

int bt_mesh_health_attention_get(struct bt_mesh_msg_ctx *ctx);

int bt_mesh_health_attention_set(struct bt_mesh_msg_ctx *ctx,
                                 u8_t attention, bool need_ack);

s32_t bt_mesh_health_cli_timeout_get(void);
void bt_mesh_health_cli_timeout_set(s32_t timeout);

/* Health Client Status Message Context */

struct bt_mesh_health_current_status {
    u8_t  test_id;
    u16_t cid;
    struct net_buf_simple *fault_array;
};

struct bt_mesh_health_fault_status {
    u8_t  test_id;
    u16_t cid;
    struct net_buf_simple *fault_array;
};

/**
 * @}
 */

#endif /* __BLE_MESH_HEALTH_CLI_H */
