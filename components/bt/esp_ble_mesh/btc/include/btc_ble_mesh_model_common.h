/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
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

static inline void btc_ble_mesh_msg_ctx_copy(struct bt_mesh_msg_ctx *dst,
                                             const struct bt_mesh_msg_ctx *src,
                                             bool use_dev_key)
{
    if (dst == NULL ||
        src == NULL) {
        return;
    }

    dst->net_idx    = src->net_idx;
    dst->app_idx    = use_dev_key ? BLE_MESH_KEY_DEV : src->app_idx;
    dst->addr       = src->addr;
    dst->send_szmic = src->send_szmic;
    dst->send_ttl   = src->send_ttl;
    dst->send_cred  = src->send_cred;
    dst->send_tag   = src->send_tag;
}

static inline void btc_ble_mesh_set_client_common_param(esp_ble_mesh_client_common_param_t *input,
                                                        bt_mesh_client_common_param_t *output,
                                                        bool use_dev_key)
{
    if (input && output) {
        output->opcode         = input->opcode;
        output->model          = (struct bt_mesh_model *)input->model;
        output->msg_timeout    = input->msg_timeout;
        btc_ble_mesh_msg_ctx_copy(&output->ctx, (const struct bt_mesh_msg_ctx *)&input->ctx, use_dev_key);
    }
}

#ifdef __cplusplus
}
#endif

#endif /* _BTC_BLE_MESH_MODEL_COMMON_H_ */
