/*
 * SPDX-FileCopyrightText: 2020 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_MESH_v11_DFD_SRV_INTERNAL_H__
#define _BLE_MESH_v11_DFD_SRV_INTERNAL_H__

#include "mesh_v1.1/dfu/dfd_srv.h"

#if CONFIG_BLE_MESH_DFD_SRV
#ifdef __cplusplus
extern "C" {
#endif

struct bt_mesh_dfd_start_params {
    uint16_t app_idx, timeout_base, slot_idx, group;
    enum bt_mesh_blob_xfer_mode xfer_mode;
    uint8_t ttl;
    bool apply;
};

enum bt_mesh_dfd_status bt_mesh_dfd_srv_receiver_add(struct bt_mesh_dfd_srv *srv, uint16_t addr,
                                                     uint8_t img_idx);

enum bt_mesh_dfd_status bt_mesh_dfd_srv_receivers_delete_all(struct bt_mesh_dfd_srv *srv);

enum bt_mesh_dfd_status bt_mesh_dfd_srv_start(struct bt_mesh_dfd_srv *srv,
                                              struct bt_mesh_dfd_start_params *params);

enum bt_mesh_dfd_status bt_mesh_dfd_srv_suspend(struct bt_mesh_dfd_srv *srv);

enum bt_mesh_dfd_status bt_mesh_dfd_srv_cancel(struct bt_mesh_dfd_srv *srv,
                                               struct bt_mesh_msg_ctx *ctx);

enum bt_mesh_dfd_status bt_mesh_dfd_srv_apply(struct bt_mesh_dfd_srv *srv);

enum bt_mesh_dfd_status bt_mesh_dfd_srv_fw_delete(struct bt_mesh_dfd_srv *srv, size_t *fwid_len,
                                                  const uint8_t **fwid);

enum bt_mesh_dfd_status bt_mesh_dfd_srv_fw_delete_all(struct bt_mesh_dfd_srv *srv);

#ifdef __cplusplus
}
#endif
#endif
#endif /* _BLE_MESH_v11_DFD_SRV_INTERNAL_H__ */
