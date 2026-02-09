/*
 * SPDX-FileCopyrightText: 2020 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "crypto.h"
#include "access.h"
#include "transport.h"
#include "mesh/byteorder.h"
#include "mesh/buf.h"
#include "mesh_v1.1/utils.h"
#include "mesh_v1.1/dfu/dfu_metadata.h"

#if CONFIG_BLE_MESH_DFU_METADATA
int bt_mesh_dfu_metadata_decode(struct net_buf_simple *buf,
                                struct bt_mesh_dfu_metadata *metadata)
{
    if (buf->len < 12) {
        return -EMSGSIZE;
    }

    metadata->fw_ver.major = net_buf_simple_pull_u8(buf);
    metadata->fw_ver.minor = net_buf_simple_pull_u8(buf);
    metadata->fw_ver.revision = net_buf_simple_pull_le16(buf);
    metadata->fw_ver.build_num = net_buf_simple_pull_le32(buf);
    metadata->fw_size = net_buf_simple_pull_le24(buf);
    metadata->fw_core_type = net_buf_simple_pull_u8(buf);

    if (metadata->fw_core_type & BLE_MESH_DFU_FW_CORE_TYPE_APP) {
        if (buf->len < 6) {
            return -EMSGSIZE;
        }

        metadata->comp_hash = net_buf_simple_pull_le32(buf);
        metadata->elems = net_buf_simple_pull_le16(buf);
    }

    metadata->user_data = buf->len > 0 ? buf->data : NULL;
    metadata->user_data_len = buf->len;

    return 0;
}

int bt_mesh_dfu_metadata_encode(const struct bt_mesh_dfu_metadata *metadata,
                                struct net_buf_simple *buf)
{
    size_t md_len_min = 12 + metadata->user_data_len;

    if (metadata->fw_core_type & BLE_MESH_DFU_FW_CORE_TYPE_APP) {
        md_len_min += 6;
    }

    if (net_buf_simple_tailroom(buf) < md_len_min) {
        return -EMSGSIZE;
    }

    net_buf_simple_add_u8(buf, metadata->fw_ver.major);
    net_buf_simple_add_u8(buf, metadata->fw_ver.minor);
    net_buf_simple_add_le16(buf, metadata->fw_ver.revision);
    net_buf_simple_add_le32(buf, metadata->fw_ver.build_num);
    net_buf_simple_add_le24(buf, metadata->fw_size);
    net_buf_simple_add_u8(buf, metadata->fw_core_type);
    if (metadata->fw_core_type & BLE_MESH_DFU_FW_CORE_TYPE_APP) {
        net_buf_simple_add_le32(buf, metadata->comp_hash);
        net_buf_simple_add_le16(buf, metadata->elems);
    }

    if (metadata->user_data_len > 0) {
        net_buf_simple_add_mem(buf, metadata->user_data, metadata->user_data_len);
    }

    return 0;
}

int bt_mesh_dfu_metadata_comp_hash_get(struct net_buf_simple *buf, uint8_t *key, uint32_t *hash)
{
    uint8_t mac[16];
    int err;
    struct bt_mesh_sg sg = {.data = buf->data, .len = buf->len};

    /* The implementation of this function is the same as function `bt_mesh_aes_cmac_raw_key` in Zephyr.  */
    err = bt_mesh_aes_cmac(key, &sg, 1, mac);
    if (err) {
        return err;
    }

    *hash = sys_get_le32(mac);

    return 0;
}

int bt_mesh_dfu_metadata_comp_hash_local_get(uint8_t *key, uint32_t *hash)
{
    NET_BUF_SIMPLE_DEFINE(buf, BLE_MESH_TX_SDU_MAX);
    int err;

    err = bt_mesh_get_comp_data(&buf, 0, 0, true);
    if (err) {
        return err;
    }

    err = bt_mesh_dfu_metadata_comp_hash_get(&buf, key, hash);
    return err;
}
#endif /* CONFIG_BLE_MESH_DFU_METADATA */
