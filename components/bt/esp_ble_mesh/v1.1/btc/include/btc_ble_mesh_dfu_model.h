/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BTC_BLE_MESH_DFU_MODEL_H_
#define _BTC_BLE_MESH_DFU_MODEL_H_

#include "btc/btc_manage.h"
#include "esp_ble_mesh_dfu_model_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_BLE_MESH_DFU_CLI
typedef enum {
    BTC_BLE_MESH_ACT_DFU_CLIENT_GET_STATE,
    BTC_BLE_MESH_ACT_DFU_CLIENT_IMG_SEND,
    BTC_BLE_MESH_ACT_DFU_CLIENT_MAX,
} btc_ble_mesh_dfu_client_act_t;

typedef union {
    struct {
        esp_ble_mesh_client_common_param_t *params;
        esp_ble_mesh_dfu_client_get_t *get;
    } dfu_get;
    struct {
        struct esp_ble_mesh_dfu_cli *cli;
        struct esp_ble_mesh_blob_cli_inputs *inputs;
        struct esp_ble_mesh_blob_io *io;
        struct esp_ble_mesh_dfu_cli_xfer *xfer;
    } send_arg;
} btc_ble_mesh_dfu_client_args_t;

typedef enum {
    BTC_BLE_MESH_EVT_DFU_CLIENT_TIMEOUT,
    BTC_BLE_MESH_EVT_DFU_CLIENT_RECV_GET_RSP,
    BTC_BLE_MESH_EVT_DFU_CLIENT_IMG_SEND_CMP,
    BTC_BLE_MESH_EVT_DFU_CLIENT_MAX,
} btc_ble_mesh_dfu_client_cb_evt_t;

void btc_ble_mesh_dfu_client_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);
void btc_ble_mesh_dfu_client_arg_deep_free(btc_msg_t *msg);
void bt_mesh_dfu_client_cb_evt_to_btc(uint32_t opcode, uint8_t event,
                                      struct bt_mesh_model *model,
                                      struct bt_mesh_msg_ctx *ctx,
                                      const uint8_t *val, size_t len);
void btc_ble_mesh_dfu_client_call_handler(btc_msg_t *msg);
void btc_ble_mesh_dfu_client_cb_handler(btc_msg_t *msg);
uint8_t btc_ble_mesh_dfu_cli_progress(struct esp_ble_mesh_dfu_cli *cli);
#endif /* CONFIG_BLE_MESH_DFU_CLI */

#if CONFIG_BLE_MESH_DFU_SRV

void btc_ble_mesh_dfu_srv_verified(esp_ble_mesh_dfu_srv_t *srv);

void btc_ble_mesh_dfu_srv_rejected(esp_ble_mesh_dfu_srv_t *srv);

void btc_ble_mesh_dfu_srv_cancel(esp_ble_mesh_dfu_srv_t *srv);

void btc_ble_mesh_dfu_srv_applied(esp_ble_mesh_dfu_srv_t *srv);

bool btc_ble_mesh_dfu_srv_is_busy(const esp_ble_mesh_dfu_srv_t *srv);

uint8_t btc_ble_mesh_dfu_srv_progress(const esp_ble_mesh_dfu_srv_t *srv);
#endif /* CONFIG_BLE_MESH_DFU_SRV */

#if CONFIG_BLE_MESH_DFD_CLI
typedef enum {
    BTC_BLE_MESH_ACT_DFD_CLIENT_GET,
    BTC_BLE_MESH_ACT_DFD_CLIENT_SET,
    BTC_BLE_MESH_ACT_DFD_CLIEND_SEND_COMP,
    BTC_BLE_MESH_ACT_DFD_CLIENT_MAX,
} btc_ble_mesh_dfd_client_act_t;

typedef enum {
    BTC_BLE_MESH_EVT_DFD_CLIENT_TIMEOUT,
    BTC_BLE_MESH_EVT_DFD_CLIENT_RECV_RSP,
    BTC_BLE_MESH_EVT_DFD_CLIENT_MAX,
} btc_ble_mesh_dfd_client_cb_evt_t;

typedef union {
    struct {
        esp_ble_mesh_client_common_param_t *params;
        esp_ble_mesh_dfd_client_get_param_t *get;
    } dfd_client_get;
    struct {
        esp_ble_mesh_client_common_param_t *params;
        esp_ble_mesh_dfd_client_set_param_t *set;
    } dfd_client_set;
} btc_ble_mesh_dfd_client_args_t;

void btc_ble_mesh_dfd_client_cb_handler(btc_msg_t *msg);
void btc_ble_mesh_dfd_client_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);
void btc_ble_mesh_dfd_client_arg_deep_free(btc_msg_t *msg);
void btc_ble_mesh_dfd_client_rsp_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);
void btc_ble_mesh_dfd_client_rsp_deep_free(btc_msg_t *msg);
void bt_mesh_dfd_client_cb_evt_to_btc(btc_ble_mesh_dfd_client_cb_evt_t event,
                                      struct bt_mesh_model *model,
                                      struct bt_mesh_msg_ctx *ctx,
                                      const uint8_t *val, size_t len);
void btc_ble_mesh_dfd_client_call_handler(btc_msg_t *msg);

#endif /* CONFIG_BLE_MESH_DFD_CLI */

#if CONFIG_BLE_MESH_DFD_SRV
#if CONFIG_BLE_MESH_DFD_SRV_OOB_UPLOAD
int btc_ble_mesh_dfd_srv_oob_check_complete(struct esp_ble_mesh_dfd_srv *srv,
                                            const struct esp_ble_mesh_dfu_slot *slot, int status,
                                            uint8_t *fwid, size_t fwid_len);
int btc_ble_mesh_dfd_srv_oob_store_complete(struct esp_ble_mesh_dfd_srv *srv,
                                        const struct esp_ble_mesh_dfu_slot *slot, bool success,
                                        size_t size, const uint8_t *metadata, size_t metadata_len);
#endif /* CONFIG_BLE_MESH_DFD_SRV_OOB_UPLOAD */
#endif /* CONFIG_BLE_MESH_DFD_SRV */

#ifdef __cplusplus
}
#endif

#endif /* _BTC_BLE_MESH_DFU_MODEL_H_ */
