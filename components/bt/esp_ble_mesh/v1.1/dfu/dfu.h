/*
 * SPDX-FileCopyrightText: 2020 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "mesh/buf.h"

#if CONFIG_BLE_MESH_DFU_SRV || CONFIG_BLE_MESH_DFU_CLI
#define BLE_MESH_DFU_OP_UPDATE_INFO_GET BLE_MESH_MODEL_OP_2(0x83, 0x08)
#define BLE_MESH_DFU_OP_UPDATE_INFO_STATUS BLE_MESH_MODEL_OP_2(0x83, 0x09)

#define BLE_MESH_DFU_OP_UPDATE_METADATA_CHECK BLE_MESH_MODEL_OP_2(0x83, 0x0a)
#define BLE_MESH_DFU_OP_UPDATE_METADATA_STATUS BLE_MESH_MODEL_OP_2(0x83, 0x0b)

#define BLE_MESH_DFU_OP_UPDATE_GET BLE_MESH_MODEL_OP_2(0x83, 0x0c)
#define BLE_MESH_DFU_OP_UPDATE_START BLE_MESH_MODEL_OP_2(0x83, 0x0d)
#define BLE_MESH_DFU_OP_UPDATE_CANCEL BLE_MESH_MODEL_OP_2(0x83, 0x0e)
#define BLE_MESH_DFU_OP_UPDATE_APPLY BLE_MESH_MODEL_OP_2(0x83, 0x0f)
#define BLE_MESH_DFU_OP_UPDATE_STATUS BLE_MESH_MODEL_OP_2(0x83, 0x10)

#define DFU_UPDATE_INFO_STATUS_MSG_MINLEN (4 + CONFIG_BLE_MESH_DFU_FWID_MAXLEN + \
                                          CONFIG_BLE_MESH_DFU_URI_MAXLEN)
#define DFU_UPDATE_START_MSG_MAXLEN (12 + CONFIG_BLE_MESH_DFU_METADATA_MAXLEN)

static inline uint16_t dfu_metadata_checksum(struct net_buf_simple *buf)
{
    /* Simple Fletcher-16 checksum to ensure duplicate start messages don't
     * have different metadata.
     */
    struct net_buf_simple_state state;
    uint8_t sum[2] = {0, 0};

    net_buf_simple_save(buf, &state);

    while (buf->len) {
        uint8_t byte = net_buf_simple_pull_u8(buf);

        sum[0] += byte;
        sum[1] += sum[0];
    }

    net_buf_simple_restore(buf, &state);

    return (sum[0] << 8U) | sum[1];
}
#endif /* CONFIG_BLE_MESH_DFU_SRV || CONFIG_BLE_MESH_DFU_CLI */
