/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BTC_BLE_MESH_MODEL_COMMON_H_
#define _BTC_BLE_MESH_MODEL_COMMON_H_

#include "btc/btc_manage.h"

#include "mesh/access.h"
#include "mesh/client_common.h"
#include "esp_ble_mesh_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline void btc_ble_mesh_set_client_common_param(esp_ble_mesh_client_common_param_t *input,
                                                        bt_mesh_client_common_param_t *output,
                                                        bool use_dev_key)
{
    if (input && output) {
        output->opcode         = input->opcode;
        output->model          = (struct bt_mesh_model *)input->model;
        output->ctx.net_idx    = input->ctx.net_idx;
        output->ctx.app_idx    = use_dev_key ? BLE_MESH_KEY_DEV : input->ctx.app_idx;
        output->ctx.addr       = input->ctx.addr;
        output->ctx.send_szmic = input->ctx.send_szmic;
        output->ctx.send_ttl   = input->ctx.send_ttl;
        output->ctx.send_cred  = input->ctx.send_cred;
        output->ctx.send_tag   = input->ctx.send_tag;
        output->msg_timeout    = input->msg_timeout;
    }
}

#ifdef __cplusplus
}
#endif

#endif /* _BTC_BLE_MESH_MODEL_COMMON_H_ */
