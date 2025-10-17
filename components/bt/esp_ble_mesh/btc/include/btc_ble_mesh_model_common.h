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
        if (input->ctx.enh.adv_cfg_used) {
            output->ctx.enh.adv_cfg_used = input->ctx.enh.adv_cfg_used;
            output->ctx.enh.adv_cfg.adv_cnt = input->ctx.enh.adv_cfg.adv_cnt;
            output->ctx.enh.adv_cfg.adv_itvl = input->ctx.enh.adv_cfg.adv_itvl;
            output->ctx.enh.adv_cfg.channel_map = input->ctx.enh.adv_cfg.channel_map;
        }
#if CONFIG_BLE_MESH_EXT_ADV
        if (input->ctx.enh.ext_adv_cfg_used) {
            output->ctx.enh.ext_adv_cfg_used = input->ctx.enh.ext_adv_cfg_used;
            output->ctx.enh.ext_adv_cfg.primary_phy = input->ctx.enh.ext_adv_cfg.primary_phy;
            output->ctx.enh.ext_adv_cfg.secondary_phy = input->ctx.enh.ext_adv_cfg.secondary_phy;
            output->ctx.enh.ext_adv_cfg.include_tx_power = input->ctx.enh.ext_adv_cfg.include_tx_power;
            output->ctx.enh.ext_adv_cfg.tx_power = input->ctx.enh.ext_adv_cfg.tx_power;
        }
#if CONFIG_BLE_MESH_LONG_PACKET
        if (input->ctx.enh.long_pkt_cfg_used) {
            output->ctx.enh.long_pkt_cfg_used = input->ctx.enh.long_pkt_cfg_used;
            output->ctx.enh.long_pkt_cfg = input->ctx.enh.long_pkt_cfg;
        }
#endif
#endif
    }
}

#ifdef __cplusplus
}
#endif

#endif /* _BTC_BLE_MESH_MODEL_COMMON_H_ */
