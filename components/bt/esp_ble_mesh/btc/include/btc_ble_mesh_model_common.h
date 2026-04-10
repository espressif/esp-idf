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
    if (src->enh.adv_cfg_used) {
        dst->enh.adv_cfg_used = src->enh.adv_cfg_used;
        dst->enh.adv_cfg.adv_cnt = src->enh.adv_cfg.adv_cnt;
        dst->enh.adv_cfg.adv_itvl = src->enh.adv_cfg.adv_itvl;
        dst->enh.adv_cfg.channel_map = src->enh.adv_cfg.channel_map;
    }
#if CONFIG_BLE_MESH_EXT_ADV
    if (src->enh.ext_adv_cfg_used) {
        dst->enh.ext_adv_cfg_used = src->enh.ext_adv_cfg_used;
        dst->enh.ext_adv_cfg.primary_phy = src->enh.ext_adv_cfg.primary_phy;
        dst->enh.ext_adv_cfg.secondary_phy = src->enh.ext_adv_cfg.secondary_phy;
        dst->enh.ext_adv_cfg.include_tx_power = src->enh.ext_adv_cfg.include_tx_power;
        dst->enh.ext_adv_cfg.tx_power = src->enh.ext_adv_cfg.tx_power;
    }
#if CONFIG_BLE_MESH_LONG_PACKET
    if (src->enh.long_pkt_cfg_used) {
        dst->enh.long_pkt_cfg_used = src->enh.long_pkt_cfg_used;
        dst->enh.long_pkt_cfg = src->enh.long_pkt_cfg;
    }
#endif /* CONFIG_BLE_MESH_LONG_PACKET */
#endif /* CONFIG_BLE_MESH_EXT_ADV */
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
