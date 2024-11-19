/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define BT_MESH_DFU_OP_UPDATE_INFO_GET BT_MESH_MODEL_OP_2(0x83, 0x08)
#define BT_MESH_DFU_OP_UPDATE_INFO_STATUS BT_MESH_MODEL_OP_2(0x83, 0x09)

#define BT_MESH_DFU_OP_UPDATE_METADATA_CHECK BT_MESH_MODEL_OP_2(0x83, 0x0a)
#define BT_MESH_DFU_OP_UPDATE_METADATA_STATUS BT_MESH_MODEL_OP_2(0x83, 0x0b)

#define BT_MESH_DFU_OP_UPDATE_GET BT_MESH_MODEL_OP_2(0x83, 0x0c)
#define BT_MESH_DFU_OP_UPDATE_START BT_MESH_MODEL_OP_2(0x83, 0x0d)
#define BT_MESH_DFU_OP_UPDATE_CANCEL BT_MESH_MODEL_OP_2(0x83, 0x0e)
#define BT_MESH_DFU_OP_UPDATE_APPLY BT_MESH_MODEL_OP_2(0x83, 0x0f)
#define BT_MESH_DFU_OP_UPDATE_STATUS BT_MESH_MODEL_OP_2(0x83, 0x10)

#define DFU_UPDATE_INFO_STATUS_MSG_MINLEN (4 + CONFIG_BT_MESH_DFU_FWID_MAXLEN + \
					   CONFIG_BT_MESH_DFU_URI_MAXLEN)
#define DFU_UPDATE_START_MSG_MAXLEN (12 + CONFIG_BT_MESH_DFU_METADATA_MAXLEN)

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
