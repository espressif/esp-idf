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

#include "client_common.h"

/**
 * @brief Bluetooth Mesh
 * @defgroup bt_mesh_health_cli Bluetooth Mesh Health Client Model
 * @ingroup bt_mesh
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Health client model common structure */
typedef bt_mesh_client_user_data_t      bt_mesh_health_client_t;
typedef bt_mesh_client_internal_data_t  health_internal_data_t;

extern const struct bt_mesh_model_op bt_mesh_health_cli_op[];
extern const struct bt_mesh_model_cb bt_mesh_health_cli_cb;

#define BLE_MESH_MODEL_HEALTH_CLI(cli_data)             \
        BLE_MESH_MODEL_CB(BLE_MESH_MODEL_ID_HEALTH_CLI,    \
            bt_mesh_health_cli_op, NULL, cli_data, &bt_mesh_health_cli_cb)

int bt_mesh_health_fault_get(bt_mesh_client_common_param_t *param, uint16_t cid);

int bt_mesh_health_fault_clear(bt_mesh_client_common_param_t *param,
                               uint16_t cid, bool need_ack);

int bt_mesh_health_fault_test(bt_mesh_client_common_param_t *param,
                              uint16_t cid, uint8_t test_id, bool need_ack);

int bt_mesh_health_period_get(bt_mesh_client_common_param_t *param);

int bt_mesh_health_period_set(bt_mesh_client_common_param_t *param,
                              uint8_t divisor, bool need_ack);

int bt_mesh_health_attention_get(bt_mesh_client_common_param_t *param);

int bt_mesh_health_attention_set(bt_mesh_client_common_param_t *param,
                                 uint8_t attention, bool need_ack);

/* Health Client Status Message Context */

struct bt_mesh_health_current_status {
    uint8_t  test_id;
    uint16_t cid;
    struct net_buf_simple *fault_array;
};

struct bt_mesh_health_fault_status {
    uint8_t  test_id;
    uint16_t cid;
    struct net_buf_simple *fault_array;
};

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* __BLE_MESH_HEALTH_CLI_H */
