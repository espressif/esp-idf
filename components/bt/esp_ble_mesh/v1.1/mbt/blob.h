/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <zephyr/sys/util.h>

#define BT_MESH_BLOB_OP_XFER_GET BT_MESH_MODEL_OP_2(0x83, 0x00)
#define BT_MESH_BLOB_OP_XFER_START BT_MESH_MODEL_OP_2(0x83, 0x01)
#define BT_MESH_BLOB_OP_XFER_CANCEL BT_MESH_MODEL_OP_2(0x83, 0x02)
#define BT_MESH_BLOB_OP_XFER_STATUS BT_MESH_MODEL_OP_2(0x83, 0x03)
#define BT_MESH_BLOB_OP_BLOCK_GET BT_MESH_MODEL_OP_2(0x83, 0x05)
#define BT_MESH_BLOB_OP_BLOCK_START BT_MESH_MODEL_OP_2(0x83, 0x04)
#define BT_MESH_BLOB_OP_CHUNK BT_MESH_MODEL_OP_1(0x66)
#define BT_MESH_BLOB_OP_BLOCK_STATUS BT_MESH_MODEL_OP_1(0x67)
#define BT_MESH_BLOB_OP_BLOCK_REPORT BT_MESH_MODEL_OP_1(0x68)
#define BT_MESH_BLOB_OP_INFO_GET BT_MESH_MODEL_OP_2(0x83, 0x06)
#define BT_MESH_BLOB_OP_INFO_STATUS BT_MESH_MODEL_OP_2(0x83, 0x07)

#define BLOB_BLOCK_NOT_SET 0xffff

#define BLOB_CHUNK_SDU_OVERHEAD                                                \
	(BT_MESH_MODEL_OP_LEN(BT_MESH_BLOB_OP_CHUNK) + 2 + BT_MESH_MIC_SHORT)

#define BLOB_CHUNK_SIZE_MAX(sdu_max) ((sdu_max) - BLOB_CHUNK_SDU_OVERHEAD)
#define BLOB_CHUNK_SDU_LEN(chunk_size) (BLOB_CHUNK_SDU_OVERHEAD + (chunk_size))

#if CONFIG_BT_MESH_ALIGN_CHUNK_SIZE_TO_MAX_SEGMENT ||                                              \
	CONFIG_BT_MESH_RX_BLOB_CHUNK_SIZE > BLOB_CHUNK_SIZE_MAX(BT_MESH_RX_SDU_MAX)
#define BLOB_RX_CHUNK_SIZE BLOB_CHUNK_SIZE_MAX(BT_MESH_RX_SDU_MAX)
#else
#define BLOB_RX_CHUNK_SIZE CONFIG_BT_MESH_RX_BLOB_CHUNK_SIZE
#endif

#if CONFIG_BT_MESH_ALIGN_CHUNK_SIZE_TO_MAX_SEGMENT ||                                              \
	CONFIG_BT_MESH_TX_BLOB_CHUNK_SIZE > BLOB_CHUNK_SIZE_MAX(BT_MESH_TX_SDU_MAX)
#define BLOB_TX_CHUNK_SIZE BLOB_CHUNK_SIZE_MAX(BT_MESH_TX_SDU_MAX)
#else
#define BLOB_TX_CHUNK_SIZE CONFIG_BT_MESH_TX_BLOB_CHUNK_SIZE
#endif

/* Utility macros for calculating log2 of a number at compile time.
 * Used to determine the log2 representation of the block size, which
 * is configured as a raw number, but encoded as log2.
 *
 * The macros expand to a series of ternary expressions, effectively
 * searching through power of twos until a match is found.
 * According to MshMBTv1.0, the block size cannot be larger than 2^20,
 * so we'll stop the search at 20.
 */
#define _BLOB_LOG_2_CEIL(l, x) ((x) <= (1U << l)) ? l :
#define _BLOB_LOG_2_FLOOR(l, x) ((x) < (1U << (l + 1))) ? l :

#define BLOB_BLOCK_SIZE_LOG_CEIL(x) (LISTIFY(20, _BLOB_LOG_2_CEIL, (), x) 20)
#define BLOB_BLOCK_SIZE_LOG_FLOOR(x) (LISTIFY(20, _BLOB_LOG_2_FLOOR, (), x) 20)

/* Log2 representation of the minimum block size */
#define BLOB_BLOCK_SIZE_LOG_MIN BLOB_BLOCK_SIZE_LOG_CEIL(CONFIG_BT_MESH_BLOB_BLOCK_SIZE_MIN)
/* Log2 representation of the maximum block size */
#define BLOB_BLOCK_SIZE_LOG_MAX BLOB_BLOCK_SIZE_LOG_FLOOR(CONFIG_BT_MESH_BLOB_BLOCK_SIZE_MAX)

#if defined(CONFIG_BT_MESH_BLOB_SRV)
#define BLOB_BLOCK_REPORT_STATUS_MSG_MAXLEN ( \
					MAX(sizeof(((struct bt_mesh_blob_block *)0)->missing), \
					    CONFIG_BT_MESH_BLOB_SRV_PULL_REQ_COUNT * 3))
#define BLOB_BLOCK_STATUS_MSG_MAXLEN (5 + \
				      MAX(sizeof(((struct bt_mesh_blob_block *)0)->missing), \
					  CONFIG_BT_MESH_BLOB_SRV_PULL_REQ_COUNT * 3))
#else
#define BLOB_BLOCK_REPORT_STATUS_MSG_MAXLEN sizeof(((struct bt_mesh_blob_srv *)0)->block.missing)
#define BLOB_BLOCK_STATUS_MSG_MAXLEN (5 + sizeof(((struct bt_mesh_blob_srv *)0)->block.missing))
#endif

#define BLOB_XFER_STATUS_MSG_MAXLEN (17 + sizeof(((struct bt_mesh_blob_srv *)0)->state.blocks))

enum bt_mesh_blob_chunks_missing {
	BT_MESH_BLOB_CHUNKS_MISSING_ALL,
	BT_MESH_BLOB_CHUNKS_MISSING_NONE,
	BT_MESH_BLOB_CHUNKS_MISSING_SOME,
	BT_MESH_BLOB_CHUNKS_MISSING_ENCODED,
};

static inline size_t blob_block_size(size_t xfer_size, uint8_t block_size_log,
				     uint32_t idx)
{
	if (((idx + 1U) << block_size_log) <= xfer_size) {
		return (1U << block_size_log);
	}

	return xfer_size & BIT_MASK(block_size_log);
}

static inline void blob_chunk_missing_set(uint8_t *missing_chunks,
					  int idx, bool missing)
{
	WRITE_BIT(missing_chunks[idx / 8], idx % 8, missing);
}

static inline bool
blob_chunk_missing_get(const uint8_t *missing_chunks, int idx)
{
	return !!(missing_chunks[idx / 8] & BIT(idx % 8));
}

static inline void blob_chunk_missing_set_all(struct bt_mesh_blob_block *block)
{
	size_t bytes = block->chunk_count / 8;

	memset(block->missing, 0xff, bytes);
	if (block->chunk_count % 8) {
		block->missing[bytes] = BIT_MASK(block->chunk_count % 8);
	}
}

static inline void blob_chunk_missing_set_none(struct bt_mesh_blob_block *block)
{
	memset(block->missing, 0, sizeof(block->missing));
}

/** @brief Perform a message broadcast to all BLOB Transfer Client Target nodes.
 *
 *  Will send to a group or each Target node individually, repeating until
 *  all Target nodes have responded or the retry time has run out.
 *
 *  @param cli BLOB Transfer Client instance
 *  @param ctx Broadcast context
 */
void blob_cli_broadcast(struct bt_mesh_blob_cli *cli,
			const struct blob_cli_broadcast_ctx *ctx);

/** @brief Register that a Target node responded to a broadcast.
 *
 *  @param cli    BLOB Transfer Client instance
 *  @param target Target node that responded.
 */
void blob_cli_broadcast_rsp(struct bt_mesh_blob_cli *cli,
			    struct bt_mesh_blob_target *target);

/** @brief Notify the BLOB Transfer Client that the requested transmission is complete.
 *
 *  Should be called once for each call to the @ref blob_cli_broadcast_ctx.send
 *  callback.
 *
 *  @param cli BLOB Transfer Client instance.
 */
void blob_cli_broadcast_tx_complete(struct bt_mesh_blob_cli *cli);

/** @brief Aborts any ongoing BLOB Transfer Client operations.
 *
 * @param cli BLOB Transfer Client instance.
 */
void blob_cli_broadcast_abort(struct bt_mesh_blob_cli *cli);
