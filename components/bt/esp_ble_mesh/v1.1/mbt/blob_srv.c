/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <zephyr/bluetooth/mesh.h>
#include <common/bt_str.h>
#include "net.h"
#include "access.h"
#include "transport.h"
#include "lpn.h"
#include "blob.h"

#define LOG_LEVEL CONFIG_BT_MESH_MODEL_LOG_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(bt_mesh_blob_srv);

#define MTU_SIZE_MAX (BT_MESH_RX_SDU_MAX - BT_MESH_MIC_SHORT)

/* The Receive BLOB Timeout Timer */
#define SERVER_TIMEOUT_SECS(srv) (10 * (1 + (srv)->state.timeout_base))
/* The initial timer value used by an instance of the Pull BLOB State machine - T_BPI */
#define REPORT_TIMER_TIMEOUT K_SECONDS(CONFIG_BT_MESH_BLOB_REPORT_TIMEOUT)

BUILD_ASSERT(BLOB_BLOCK_SIZE_LOG_MIN <= BLOB_BLOCK_SIZE_LOG_MAX,
	     "The must be at least one number between the min and "
	     "max block size that is the power of two.");

BUILD_ASSERT((BLOB_XFER_STATUS_MSG_MAXLEN + BT_MESH_MODEL_OP_LEN(BT_MESH_BLOB_OP_XFER_STATUS) +
	      BT_MESH_MIC_SHORT) <= BT_MESH_TX_SDU_MAX,
	     "The BLOB Transfer Status message does not fit into the maximum outgoing SDU size.");

BUILD_ASSERT((BLOB_BLOCK_REPORT_STATUS_MSG_MAXLEN +
	      BT_MESH_MODEL_OP_LEN(BT_MESH_BLOB_OP_BLOCK_REPORT) + BT_MESH_MIC_SHORT)
	     <= BT_MESH_TX_SDU_MAX,
	     "The BLOB Partial Block Report message does not fit into the maximum outgoing SDU "
	     "size.");

BUILD_ASSERT((BLOB_BLOCK_STATUS_MSG_MAXLEN + BT_MESH_MODEL_OP_LEN(BT_MESH_BLOB_OP_BLOCK_STATUS) +
	      BT_MESH_MIC_SHORT) <= BT_MESH_TX_SDU_MAX,
	     "The BLOB Block Status message does not fit into the maximum outgoing SDU size.");

static void cancel(struct bt_mesh_blob_srv *srv);
static void suspend(struct bt_mesh_blob_srv *srv);

static inline uint32_t block_count_get(const struct bt_mesh_blob_srv *srv)
{
	return DIV_ROUND_UP(srv->state.xfer.size,
				(1U << srv->state.xfer.block_size_log));
}

static inline uint32_t max_chunk_size(const struct bt_mesh_blob_srv *srv)
{
	return MIN((srv->state.mtu_size - 2 - BT_MESH_MODEL_OP_LEN(BT_MESH_BLOB_OP_CHUNK)),
		   BLOB_RX_CHUNK_SIZE);
}

static inline uint32_t max_chunk_count(const struct bt_mesh_blob_srv *srv)
{
	return MIN(8 * (srv->state.mtu_size - 6),
		   CONFIG_BT_MESH_BLOB_CHUNK_COUNT_MAX);
}

static inline uint32_t missing_chunks(const struct bt_mesh_blob_block *block)
{
	int i;
	uint32_t count = 0;

	for (i = 0; i < ARRAY_SIZE(block->missing); ++i) {
		count += POPCOUNT(block->missing[i]);
	}

	return count;
}

static void store_state(const struct bt_mesh_blob_srv *srv)
{
	if (!IS_ENABLED(CONFIG_BT_SETTINGS)) {
		return;
	}

	/* Convert bit count to byte count: */
	uint32_t block_len = DIV_ROUND_UP(block_count_get(srv), 8);

	bt_mesh_model_data_store(
		srv->mod, false, NULL, &srv->state,
		offsetof(struct bt_mesh_blob_srv_state, blocks) + block_len);
}

static void erase_state(struct bt_mesh_blob_srv *srv)
{
	if (!IS_ENABLED(CONFIG_BT_SETTINGS)) {
		return;
	}

	bt_mesh_model_data_store(srv->mod, false, NULL, NULL, 0);
}

static int io_open(struct bt_mesh_blob_srv *srv)
{
	if (!srv->io->open) {
		return 0;
	}

	return srv->io->open(srv->io, &srv->state.xfer, BT_MESH_BLOB_WRITE);
}

static void io_close(struct bt_mesh_blob_srv *srv)
{
	if (!srv->io->close) {
		return;
	}

	srv->io->close(srv->io, &srv->state.xfer);
}

static void reset_timer(struct bt_mesh_blob_srv *srv)
{
	uint32_t timeout_secs =
		srv->state.xfer.mode == BT_MESH_BLOB_XFER_MODE_PULL ?
			MAX(SERVER_TIMEOUT_SECS(srv),
			    CONFIG_BT_MESH_BLOB_REPORT_TIMEOUT + 1) :
			SERVER_TIMEOUT_SECS(srv);
	k_work_reschedule(&srv->rx_timeout, K_SECONDS(timeout_secs));
}

static void buf_chunk_index_add(struct net_buf_simple *buf, uint16_t chunk)
{
	/* utf-8 encoded: */
	if (chunk < 0x80) {
		net_buf_simple_add_u8(buf, chunk);
	} else if (chunk < 0x8000) {
		net_buf_simple_add_u8(buf, 0xc0 | chunk >> 6);
		net_buf_simple_add_u8(buf, 0x80 | (chunk & BIT_MASK(6)));
	} else {
		net_buf_simple_add_u8(buf, 0xe0 | chunk >> 12);
		net_buf_simple_add_u8(buf, 0x80 | ((chunk >> 6) & BIT_MASK(6)));
		net_buf_simple_add_u8(buf, 0x80 | (chunk & BIT_MASK(6)));
	}
}

static int pull_req_max(const struct bt_mesh_blob_srv *srv)
{
	int count = CONFIG_BT_MESH_BLOB_SRV_PULL_REQ_COUNT;

#if defined(CONFIG_BT_MESH_LOW_POWER)
	/* No point in requesting more than the friend node can hold: */
	if (bt_mesh_lpn_established()) {
		uint32_t segments_per_chunk = DIV_ROUND_UP(
			BLOB_CHUNK_SDU_LEN(srv->state.xfer.chunk_size),
			BT_MESH_APP_SEG_SDU_MAX);

		count = MIN(CONFIG_BT_MESH_BLOB_SRV_PULL_REQ_COUNT,
			    bt_mesh.lpn.queue_size / segments_per_chunk);
	}
#endif

	return MIN(count, missing_chunks(&srv->block));
}

static void report_sent(int err, void *cb_data)
{
	struct bt_mesh_blob_srv *srv = cb_data;

	LOG_DBG("");

	if (IS_ENABLED(CONFIG_BT_MESH_LOW_POWER) && bt_mesh_lpn_established()) {
		bt_mesh_lpn_poll();
	}

	if (k_work_delayable_is_pending(&srv->rx_timeout)) {
		k_work_reschedule(&srv->pull.report, REPORT_TIMER_TIMEOUT);
	}
}

static void block_report(struct bt_mesh_blob_srv *srv)
{
	static const struct bt_mesh_send_cb report_cb = { .end = report_sent };
	struct bt_mesh_msg_ctx ctx = {
		.app_idx = srv->state.app_idx,
		.send_ttl = srv->state.ttl,
		.addr = srv->state.cli,
	};
	int count;
	int i;

	LOG_DBG("rx BLOB Timeout Timer: %i", k_work_delayable_is_pending(&srv->rx_timeout));

	BT_MESH_MODEL_BUF_DEFINE(buf, BT_MESH_BLOB_OP_BLOCK_REPORT,
				 BLOB_BLOCK_REPORT_STATUS_MSG_MAXLEN);
	bt_mesh_model_msg_init(&buf, BT_MESH_BLOB_OP_BLOCK_REPORT);

	count = pull_req_max(srv);

	for (i = 0; i < srv->block.chunk_count && count; ++i) {
		if (blob_chunk_missing_get(srv->block.missing, i)) {
			buf_chunk_index_add(&buf, i);
			count--;
		}
	}

	(void)bt_mesh_model_send(srv->mod, &ctx, &buf, &report_cb, srv);
}

static void phase_set(struct bt_mesh_blob_srv *srv,
		      enum bt_mesh_blob_xfer_phase phase)
{
	srv->phase = phase;
	LOG_DBG("Phase: %u", phase);
}

static void cancel(struct bt_mesh_blob_srv *srv)
{
	/* TODO: Could this state be preserved instead somehow? Wiping the
	 * entire transfer state is a bit overkill
	 */
	phase_set(srv, BT_MESH_BLOB_XFER_PHASE_INACTIVE);
	srv->state.xfer.mode = BT_MESH_BLOB_XFER_MODE_NONE;
	srv->state.ttl = BT_MESH_TTL_DEFAULT;
	srv->block.number = 0xffff;
	memset(srv->block.missing, 0, sizeof(srv->block.missing));
	srv->state.xfer.chunk_size = 0xffff;
	k_work_cancel_delayable(&srv->rx_timeout);
	k_work_cancel_delayable(&srv->pull.report);
	io_close(srv);
	erase_state(srv);

	if (srv->cb && srv->cb->end) {
		srv->cb->end(srv, srv->state.xfer.id, false);
	}
}

static void suspend(struct bt_mesh_blob_srv *srv)
{
	LOG_DBG("");
	k_work_cancel_delayable(&srv->rx_timeout);
	k_work_cancel_delayable(&srv->pull.report);
	phase_set(srv, BT_MESH_BLOB_XFER_PHASE_SUSPENDED);
	if (srv->cb && srv->cb->suspended) {
		srv->cb->suspended(srv);
	}
}

static void resume(struct bt_mesh_blob_srv *srv)
{
	LOG_DBG("Resuming");

	phase_set(srv, BT_MESH_BLOB_XFER_PHASE_WAITING_FOR_BLOCK);
	reset_timer(srv);
}

static void end(struct bt_mesh_blob_srv *srv)
{
	phase_set(srv, BT_MESH_BLOB_XFER_PHASE_COMPLETE);
	k_work_cancel_delayable(&srv->rx_timeout);
	k_work_cancel_delayable(&srv->pull.report);
	io_close(srv);
	erase_state(srv);

	if (srv->cb && srv->cb->end) {
		srv->cb->end(srv, srv->state.xfer.id, true);
	}
}

static bool all_blocks_received(struct bt_mesh_blob_srv *srv)
{
	for (int i = 0; i < ARRAY_SIZE(srv->state.blocks); ++i) {
		if (srv->state.blocks[i]) {
			return false;
		}
	}

	return true;
}

static bool pull_mode_xfer_complete(struct bt_mesh_blob_srv *srv)
{
	return srv->state.xfer.mode == BT_MESH_BLOB_XFER_MODE_PULL &&
	       srv->phase == BT_MESH_BLOB_XFER_PHASE_WAITING_FOR_CHUNK &&
	       all_blocks_received(srv);
}

static void timeout(struct k_work *work)
{
	struct bt_mesh_blob_srv *srv =
		CONTAINER_OF(work, struct bt_mesh_blob_srv, rx_timeout.work);

	LOG_DBG("");

	if (srv->phase == BT_MESH_BLOB_XFER_PHASE_WAITING_FOR_START) {
		cancel(srv);
	} else if (pull_mode_xfer_complete(srv)) {
		end(srv);
	} else {
		suspend(srv);
	}
}

static void report_timeout(struct k_work *work)
{
	struct bt_mesh_blob_srv *srv =
		CONTAINER_OF(work, struct bt_mesh_blob_srv, pull.report.work);

	LOG_DBG("");

	if (srv->phase != BT_MESH_BLOB_XFER_PHASE_WAITING_FOR_BLOCK &&
	    srv->phase != BT_MESH_BLOB_XFER_PHASE_WAITING_FOR_CHUNK) {
		return;
	}

	block_report(srv);
}

/*******************************************************************************
 * Message handling
 ******************************************************************************/

static void xfer_status_rsp(struct bt_mesh_blob_srv *srv,
			    struct bt_mesh_msg_ctx *ctx,
			    enum bt_mesh_blob_status status)
{
	BT_MESH_MODEL_BUF_DEFINE(buf, BT_MESH_BLOB_OP_XFER_STATUS,
				 BLOB_XFER_STATUS_MSG_MAXLEN);
	bt_mesh_model_msg_init(&buf, BT_MESH_BLOB_OP_XFER_STATUS);

	net_buf_simple_add_u8(&buf, ((status & BIT_MASK(4)) |
				     (srv->state.xfer.mode << 6)));
	net_buf_simple_add_u8(&buf, srv->phase);

	if (srv->phase == BT_MESH_BLOB_XFER_PHASE_INACTIVE) {
		goto send;
	}

	net_buf_simple_add_le64(&buf, srv->state.xfer.id);

	if (srv->phase == BT_MESH_BLOB_XFER_PHASE_WAITING_FOR_START) {
		goto send;
	}

	net_buf_simple_add_le32(&buf, srv->state.xfer.size);
	net_buf_simple_add_u8(&buf, srv->state.xfer.block_size_log);
	net_buf_simple_add_le16(&buf, srv->state.mtu_size);
	net_buf_simple_add_mem(&buf, srv->state.blocks,
			       DIV_ROUND_UP(block_count_get(srv), 8));

send:
	ctx->send_ttl = srv->state.ttl;
	(void)bt_mesh_model_send(srv->mod, ctx, &buf, NULL, NULL);
}

static void block_status_rsp(struct bt_mesh_blob_srv *srv,
			     struct bt_mesh_msg_ctx *ctx,
			     enum bt_mesh_blob_status status)
{
	enum bt_mesh_blob_chunks_missing format;
	uint32_t missing;
	int i;

	BT_MESH_MODEL_BUF_DEFINE(buf, BT_MESH_BLOB_OP_BLOCK_STATUS,
				 BLOB_BLOCK_STATUS_MSG_MAXLEN);
	bt_mesh_model_msg_init(&buf, BT_MESH_BLOB_OP_BLOCK_STATUS);

	if (srv->phase == BT_MESH_BLOB_XFER_PHASE_INACTIVE ||
	    srv->phase == BT_MESH_BLOB_XFER_PHASE_WAITING_FOR_START) {
		missing = srv->block.chunk_count;
	} else if (srv->phase == BT_MESH_BLOB_XFER_PHASE_COMPLETE) {
		missing = 0U;
	} else {
		missing = missing_chunks(&srv->block);
	}

	if (srv->state.xfer.mode == BT_MESH_BLOB_XFER_MODE_PULL) {
		format = BT_MESH_BLOB_CHUNKS_MISSING_ENCODED;
	} else if (missing == srv->block.chunk_count) {
		format = BT_MESH_BLOB_CHUNKS_MISSING_ALL;
	} else if (missing == 0) {
		format = BT_MESH_BLOB_CHUNKS_MISSING_NONE;
	} else {
		format = BT_MESH_BLOB_CHUNKS_MISSING_SOME;
	}

	LOG_DBG("Status: %u, missing: %u/%u", status, missing, srv->block.chunk_count);

	net_buf_simple_add_u8(&buf, (status & BIT_MASK(4)) | (format << 6));
	net_buf_simple_add_le16(&buf, srv->block.number);
	net_buf_simple_add_le16(&buf, srv->state.xfer.chunk_size);

	if (format == BT_MESH_BLOB_CHUNKS_MISSING_SOME) {
		net_buf_simple_add_mem(&buf, srv->block.missing,
				       DIV_ROUND_UP(srv->block.chunk_count,
							8));

		LOG_DBG("Bits: %s",
			bt_hex(srv->block.missing,
			       DIV_ROUND_UP(srv->block.chunk_count, 8)));

	} else if (format == BT_MESH_BLOB_CHUNKS_MISSING_ENCODED) {
		int count = pull_req_max(srv);

		for (i = 0; (i < srv->block.chunk_count) && count; ++i) {
			if (blob_chunk_missing_get(srv->block.missing, i)) {
				LOG_DBG("Missing %u", i);
				buf_chunk_index_add(&buf, i);
				count--;
			}
		}
	}

	if (srv->phase != BT_MESH_BLOB_XFER_PHASE_INACTIVE) {
		ctx->send_ttl = srv->state.ttl;
	}

	(void)bt_mesh_model_send(srv->mod, ctx, &buf, NULL, NULL);
}

static int handle_xfer_get(const struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx,
			   struct net_buf_simple *buf)
{
	struct bt_mesh_blob_srv *srv = mod->rt->user_data;

	LOG_DBG("");

	if (pull_mode_xfer_complete(srv)) {
		/* The client requested transfer. If we are in Pull mode and all blocks were
		 * received, we should change the Transfer state here to Complete so that the client
		 * receives the correct state.
		 */
		end(srv);
	}

	xfer_status_rsp(srv, ctx, BT_MESH_BLOB_SUCCESS);

	return 0;
}

static int handle_xfer_start(const struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx,
			     struct net_buf_simple *buf)
{
	struct bt_mesh_blob_srv *srv = mod->rt->user_data;
	enum bt_mesh_blob_status status;
	enum bt_mesh_blob_xfer_mode mode;
	uint64_t id;
	size_t size;
	uint8_t block_size_log;
	uint32_t block_count;
	uint16_t mtu_size;
	int err;

	mode = (net_buf_simple_pull_u8(buf) >> 6);
	id = net_buf_simple_pull_le64(buf);
	size = net_buf_simple_pull_le32(buf);
	block_size_log = net_buf_simple_pull_u8(buf);
	mtu_size = net_buf_simple_pull_le16(buf);

	LOG_DBG("\n\tsize: %u block size: %u\n\tmtu_size: %u\n\tmode: %s",
		size, (1U << block_size_log), mtu_size,
		mode == BT_MESH_BLOB_XFER_MODE_PUSH ? "push" : "pull");

	if (mode != BT_MESH_BLOB_XFER_MODE_PULL &&
	    mode != BT_MESH_BLOB_XFER_MODE_PUSH) {
		LOG_WRN("Invalid mode 0x%x", mode);
		return -EINVAL;
	}

	if (srv->phase == BT_MESH_BLOB_XFER_PHASE_INACTIVE) {
		status = BT_MESH_BLOB_ERR_WRONG_PHASE;
		LOG_WRN("Uninitialized");
		goto rsp;
	}

	if (srv->state.xfer.id != id) {
		status = BT_MESH_BLOB_ERR_WRONG_BLOB_ID;
		/* bt_hex uses static array for the resulting hex string.
		 * Not possible to use bt_hex in the same logging function twice.
		 */
		LOG_WRN("Invalid ID: %s", bt_hex(&id, sizeof(uint64_t)));
		LOG_WRN("Expected ID: %s", bt_hex(&srv->state.xfer.id, sizeof(uint64_t)));
		goto rsp;
	}

	if (srv->phase != BT_MESH_BLOB_XFER_PHASE_WAITING_FOR_START) {
		if (srv->state.xfer.mode != mode ||
		    srv->state.xfer.size != size ||
		    srv->state.xfer.block_size_log != block_size_log ||
		    srv->state.mtu_size > mtu_size) {
			status = BT_MESH_BLOB_ERR_WRONG_PHASE;
			LOG_WRN("Busy");
			goto rsp;
		}

		if (srv->phase == BT_MESH_BLOB_XFER_PHASE_SUSPENDED) {
			resume(srv);
			store_state(srv);
		} else {
			LOG_DBG("Duplicate");
		}

		status = BT_MESH_BLOB_SUCCESS;
		goto rsp;
	}

	if (size > CONFIG_BT_MESH_BLOB_SIZE_MAX) {
		LOG_WRN("Too large");
		status = BT_MESH_BLOB_ERR_BLOB_TOO_LARGE;
		goto rsp;
	}

	if (((1U << block_size_log) < CONFIG_BT_MESH_BLOB_BLOCK_SIZE_MIN) ||
	    ((1U << block_size_log) > CONFIG_BT_MESH_BLOB_BLOCK_SIZE_MAX)) {
		LOG_WRN("Invalid block size: %u", block_size_log);
		status = BT_MESH_BLOB_ERR_INVALID_BLOCK_SIZE;
		goto rsp;
	}

	srv->state.cli = ctx->addr;
	srv->state.app_idx = ctx->app_idx;
	srv->state.mtu_size = MIN(mtu_size, MTU_SIZE_MAX);
	srv->state.xfer.id = id;
	srv->state.xfer.size = size;
	srv->state.xfer.mode = mode;
	srv->state.xfer.block_size_log = block_size_log;
	srv->state.xfer.chunk_size = 0xffff;
	srv->block.number = 0xffff;

	block_count = block_count_get(srv);
	if (block_count > BT_MESH_BLOB_BLOCKS_MAX) {
		LOG_WRN("Invalid block count (%u)", block_count);
		status = BT_MESH_BLOB_ERR_INVALID_PARAM;
		cancel(srv);
		goto rsp;
	}

	memset(srv->state.blocks, 0, sizeof(srv->state.blocks));
	for (int i = 0; i < block_count; i++) {
		atomic_set_bit(srv->state.blocks, i);
	}

	err = io_open(srv);
	if (err) {
		LOG_ERR("Couldn't open stream (err: %d)", err);
		status = BT_MESH_BLOB_ERR_INTERNAL;
		cancel(srv);
		goto rsp;
	}

	if (srv->cb && srv->cb->start) {
		err = srv->cb->start(srv, ctx, &srv->state.xfer);
		if (err) {
			LOG_ERR("Couldn't start transfer (err: %d)", err);
			status = BT_MESH_BLOB_ERR_INTERNAL;
			cancel(srv);
			goto rsp;
		}
	}

	reset_timer(srv);
	phase_set(srv, BT_MESH_BLOB_XFER_PHASE_WAITING_FOR_BLOCK);
	store_state(srv);
	status = BT_MESH_BLOB_SUCCESS;

rsp:
	xfer_status_rsp(srv, ctx, status);

	return 0;
}

static int handle_xfer_cancel(const struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx,
			      struct net_buf_simple *buf)
{
	enum bt_mesh_blob_status status = BT_MESH_BLOB_SUCCESS;
	struct bt_mesh_blob_srv *srv = mod->rt->user_data;
	uint64_t id;

	id = net_buf_simple_pull_le64(buf);

	LOG_DBG("%u", (uint32_t)id);

	if (srv->phase == BT_MESH_BLOB_XFER_PHASE_INACTIVE) {
		goto rsp;
	}

	if (srv->state.xfer.id != id) {
		status = BT_MESH_BLOB_ERR_WRONG_BLOB_ID;
		goto rsp;
	}

	cancel(srv);

rsp:
	xfer_status_rsp(srv, ctx, status);

	return 0;
}

static int handle_block_get(const struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx,
			    struct net_buf_simple *buf)
{
	enum bt_mesh_blob_status status;
	struct bt_mesh_blob_srv *srv = mod->rt->user_data;

	switch (srv->phase) {
	case BT_MESH_BLOB_XFER_PHASE_WAITING_FOR_BLOCK:
	case BT_MESH_BLOB_XFER_PHASE_WAITING_FOR_CHUNK:
	case BT_MESH_BLOB_XFER_PHASE_COMPLETE:
		status = BT_MESH_BLOB_SUCCESS;
		break;
	case BT_MESH_BLOB_XFER_PHASE_SUSPENDED:
		status = BT_MESH_BLOB_ERR_INFO_UNAVAILABLE;
		break;
	case BT_MESH_BLOB_XFER_PHASE_WAITING_FOR_START:
	case BT_MESH_BLOB_XFER_PHASE_INACTIVE:
		status = BT_MESH_BLOB_ERR_WRONG_PHASE;
		break;
	default:
		status = BT_MESH_BLOB_ERR_INTERNAL;
		break;
	}

	LOG_DBG("");

	block_status_rsp(srv, ctx, status);

	return 0;
}

static int handle_block_start(const struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx,
			      struct net_buf_simple *buf)
{
	struct bt_mesh_blob_srv *srv = mod->rt->user_data;
	enum bt_mesh_blob_status status;
	uint16_t block_number, chunk_size;
	int err;

	block_number = net_buf_simple_pull_le16(buf);
	chunk_size = net_buf_simple_pull_le16(buf);

	if (srv->phase == BT_MESH_BLOB_XFER_PHASE_WAITING_FOR_START ||
	    srv->phase == BT_MESH_BLOB_XFER_PHASE_INACTIVE) {
		status = BT_MESH_BLOB_ERR_WRONG_PHASE;
		goto rsp;
	}

	reset_timer(srv);

	if (srv->phase == BT_MESH_BLOB_XFER_PHASE_WAITING_FOR_CHUNK) {
		if (block_number != srv->block.number ||
		    chunk_size != srv->state.xfer.chunk_size) {
			status = BT_MESH_BLOB_ERR_WRONG_PHASE;
		} else {
			status = BT_MESH_BLOB_SUCCESS;
		}

		goto rsp;
	}

	if (block_number >= block_count_get(srv)) {
		status = BT_MESH_BLOB_ERR_INVALID_BLOCK_NUM;
		goto rsp;
	}

	if (!chunk_size || chunk_size > max_chunk_size(srv) ||
	    (DIV_ROUND_UP((1 << srv->state.xfer.block_size_log), chunk_size) >
	     max_chunk_count(srv))) {
		LOG_WRN("Invalid chunk size: (chunk size: %u, max: %u, block log: %u, count: %u)",
			chunk_size, max_chunk_size(srv),
			srv->state.xfer.block_size_log,
			max_chunk_count(srv));
		status = BT_MESH_BLOB_ERR_INVALID_CHUNK_SIZE;
		goto rsp;
	}

	srv->block.size = blob_block_size(
		srv->state.xfer.size, srv->state.xfer.block_size_log, block_number);
	srv->block.number = block_number;
	srv->block.chunk_count = DIV_ROUND_UP(srv->block.size, chunk_size);
	srv->state.xfer.chunk_size = chunk_size;
	srv->block.offset = block_number * (1UL << srv->state.xfer.block_size_log);

	if (srv->phase == BT_MESH_BLOB_XFER_PHASE_COMPLETE ||
	    !atomic_test_bit(srv->state.blocks, block_number)) {
		memset(srv->block.missing, 0, sizeof(srv->block.missing));
		status = BT_MESH_BLOB_SUCCESS;
		goto rsp;
	}

	if (srv->phase == BT_MESH_BLOB_XFER_PHASE_SUSPENDED && srv->cb &&
	    srv->cb->resume) {
		srv->cb->resume(srv);
	}

	phase_set(srv, BT_MESH_BLOB_XFER_PHASE_WAITING_FOR_CHUNK);
	blob_chunk_missing_set_all(&srv->block);

	LOG_DBG("%u: (%u/%u)\n\tsize: %u\n\tchunk size: %u\n\tchunk count: %u",
		srv->block.number, srv->block.number + 1, block_count_get(srv),
		srv->block.size, chunk_size, srv->block.chunk_count);

	if (srv->io->block_start) {
		err = srv->io->block_start(srv->io, &srv->state.xfer,
					   &srv->block);
		if (err) {
			cancel(srv);
			status = BT_MESH_BLOB_ERR_INTERNAL;
			goto rsp;
		}
	}

	if (srv->state.xfer.mode == BT_MESH_BLOB_XFER_MODE_PULL) {
		/* Wait for the client to send the first chunk */
		k_work_reschedule(&srv->pull.report, REPORT_TIMER_TIMEOUT);
	}

	status = BT_MESH_BLOB_SUCCESS;

rsp:
	block_status_rsp(srv, ctx, status);

	return 0;
}

static int handle_chunk(const struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx,
			struct net_buf_simple *buf)
{
	struct bt_mesh_blob_srv *srv = mod->rt->user_data;
	struct bt_mesh_blob_chunk chunk;
	size_t expected_size = 0;
	uint16_t idx;
	int err;

	idx = net_buf_simple_pull_le16(buf);
	chunk.size = buf->len;
	chunk.data = net_buf_simple_pull_mem(buf, chunk.size);
	chunk.offset = idx * srv->state.xfer.chunk_size;

	if (srv->phase != BT_MESH_BLOB_XFER_PHASE_WAITING_FOR_CHUNK ||
	    idx >= srv->block.chunk_count) {
		LOG_ERR("Invalid phase or index (%u %u)", srv->phase,
		       idx);
		return -EINVAL;
	}

	if (idx == srv->block.chunk_count - 1) {
		expected_size = srv->block.size % srv->state.xfer.chunk_size;
	}

	if (expected_size == 0) {
		expected_size = srv->state.xfer.chunk_size;
	}

	if (chunk.size != expected_size) {
		LOG_ERR("Unexpected size: %u != %u", expected_size, chunk.size);
		return -EINVAL;
	}

	LOG_DBG("%u/%u (%u bytes)", idx + 1, srv->block.chunk_count,
		chunk.size);

	reset_timer(srv);
	if (srv->state.xfer.mode == BT_MESH_BLOB_XFER_MODE_PULL) {
		k_work_reschedule(&srv->pull.report, REPORT_TIMER_TIMEOUT);
	}

	if (!blob_chunk_missing_get(srv->block.missing, idx)) {
		LOG_DBG("Duplicate chunk %u", idx);
		return -EALREADY;
	}

	err = srv->io->wr(srv->io, &srv->state.xfer, &srv->block, &chunk);
	if (err) {
		return err;
	}

	blob_chunk_missing_set(srv->block.missing, idx, false);
	if (missing_chunks(&srv->block)) {
		return 0;
	}

	if (srv->state.xfer.mode == BT_MESH_BLOB_XFER_MODE_PULL) {
		block_report(srv);
	}

	if (srv->io->block_end) {
		srv->io->block_end(srv->io, &srv->state.xfer, &srv->block);
	}

	atomic_clear_bit(srv->state.blocks, srv->block.number);

	if (!all_blocks_received(srv)) {
		phase_set(srv, BT_MESH_BLOB_XFER_PHASE_WAITING_FOR_BLOCK);
		store_state(srv);
		return 0;
	}

	if (srv->state.xfer.mode == BT_MESH_BLOB_XFER_MODE_PULL) {
		/* By spec (section 5.2.4), the BLOB Server stops sending BLOB Partial Block Report
		 * messages "If the current block is the last block, then the server determines that
		 * the client knows the transfer is complete. For example, a higher-layer model may
		 * indicate that the client considers the transfer complete."
		 *
		 * We don't have any way for higher-layer model to indicate that the transfer is
		 * complete. Therefore we need to keep sending Partial Block Report messages until
		 * the client sends BLOB Transfer Get message or the Block Timer expires.
		 */
		return 0;
	}

	end(srv);
	return 0;
}

static int handle_info_get(const struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx,
			   struct net_buf_simple *buf)
{
	struct bt_mesh_blob_srv *srv = mod->rt->user_data;

	LOG_DBG("");

	BT_MESH_MODEL_BUF_DEFINE(rsp, BT_MESH_BLOB_OP_INFO_STATUS, 15);
	bt_mesh_model_msg_init(&rsp, BT_MESH_BLOB_OP_INFO_STATUS);
	net_buf_simple_add_u8(&rsp, BLOB_BLOCK_SIZE_LOG_MIN);
	net_buf_simple_add_u8(&rsp, BLOB_BLOCK_SIZE_LOG_MAX);
	net_buf_simple_add_le16(&rsp, CONFIG_BT_MESH_BLOB_CHUNK_COUNT_MAX);
	net_buf_simple_add_le16(&rsp, BLOB_RX_CHUNK_SIZE);
	net_buf_simple_add_le32(&rsp, CONFIG_BT_MESH_BLOB_SIZE_MAX);
	net_buf_simple_add_le16(&rsp, MTU_SIZE_MAX);
	net_buf_simple_add_u8(&rsp, BT_MESH_BLOB_XFER_MODE_ALL);

	if (srv->phase != BT_MESH_BLOB_XFER_PHASE_INACTIVE) {
		ctx->send_ttl = srv->state.ttl;
	}

	(void)bt_mesh_model_send(srv->mod, ctx, &rsp, NULL, NULL);

	return 0;
}

const struct bt_mesh_model_op _bt_mesh_blob_srv_op[] = {
	{ BT_MESH_BLOB_OP_XFER_GET, BT_MESH_LEN_EXACT(0), handle_xfer_get },
	{ BT_MESH_BLOB_OP_XFER_START, BT_MESH_LEN_EXACT(16), handle_xfer_start },
	{ BT_MESH_BLOB_OP_XFER_CANCEL, BT_MESH_LEN_EXACT(8), handle_xfer_cancel },
	{ BT_MESH_BLOB_OP_BLOCK_GET, BT_MESH_LEN_EXACT(0), handle_block_get },
	{ BT_MESH_BLOB_OP_BLOCK_START, BT_MESH_LEN_EXACT(4), handle_block_start },
	{ BT_MESH_BLOB_OP_CHUNK, BT_MESH_LEN_MIN(2), handle_chunk },
	{ BT_MESH_BLOB_OP_INFO_GET, BT_MESH_LEN_EXACT(0), handle_info_get },
	BT_MESH_MODEL_OP_END,
};

static int blob_srv_init(const struct bt_mesh_model *mod)
{
	struct bt_mesh_blob_srv *srv = mod->rt->user_data;

	srv->mod = mod;
	srv->state.ttl = BT_MESH_TTL_DEFAULT;
	srv->block.number = 0xffff;
	srv->state.xfer.chunk_size = 0xffff;
	k_work_init_delayable(&srv->rx_timeout, timeout);
	k_work_init_delayable(&srv->pull.report, report_timeout);

	return 0;
}

static int blob_srv_settings_set(const struct bt_mesh_model *mod, const char *name,
				 size_t len_rd, settings_read_cb read_cb,
				 void *cb_arg)
{
	struct bt_mesh_blob_srv *srv = mod->rt->user_data;
	ssize_t len;

	if (len_rd < offsetof(struct bt_mesh_blob_srv_state, blocks)) {
		return -EINVAL;
	}

	len = read_cb(cb_arg, &srv->state, sizeof(srv->state));
	if (len < 0) {
		return len;
	}

	srv->block.number = 0xffff;
	srv->state.xfer.chunk_size = 0xffff;

	if (block_count_get(srv) > BT_MESH_BLOB_BLOCKS_MAX) {
		LOG_WRN("Loaded block count too high (%u, max: %u)",
			block_count_get(srv), BT_MESH_BLOB_BLOCKS_MAX);
		return 0;
	}

	/* If device restarted before it handled `XFER_START` server we restore state into
	 * BT_MESH_BLOB_XFER_PHASE_WAITING_FOR_START phase, so `XFER_START` can be accepted
	 * as it would before reboot
	 */
	if (srv->state.cli == BT_MESH_ADDR_UNASSIGNED) {
		LOG_DBG("Transfer (id=%llu) waiting for start", srv->state.xfer.id);
		phase_set(srv, BT_MESH_BLOB_XFER_PHASE_WAITING_FOR_START);
	} else {
		phase_set(srv, BT_MESH_BLOB_XFER_PHASE_SUSPENDED);

		LOG_DBG("Recovered transfer from 0x%04x (%llu)", srv->state.cli,
			srv->state.xfer.id);
	}

	return 0;
}

static int blob_srv_start(const struct bt_mesh_model *mod)
{
	struct bt_mesh_blob_srv *srv = mod->rt->user_data;
	int err = -ENOTSUP;

	if (srv->phase == BT_MESH_BLOB_XFER_PHASE_INACTIVE) {
		return 0;
	}

	if (srv->cb && srv->cb->recover) {
		srv->io = NULL;
		err = srv->cb->recover(srv, &srv->state.xfer, &srv->io);
		if (!err && srv->io) {
			err = io_open(srv);
		}
	}

	if (err || !srv->io) {
		LOG_WRN("Abandoning transfer.");
		phase_set(srv, BT_MESH_BLOB_XFER_PHASE_INACTIVE);
		srv->state.xfer.mode = BT_MESH_BLOB_XFER_MODE_NONE;
		srv->state.ttl = BT_MESH_TTL_DEFAULT;
		erase_state(srv);
	}

	return 0;
}

static void blob_srv_reset(const struct bt_mesh_model *mod)
{
	struct bt_mesh_blob_srv *srv = mod->rt->user_data;

	phase_set(srv, BT_MESH_BLOB_XFER_PHASE_INACTIVE);
	srv->state.xfer.mode = BT_MESH_BLOB_XFER_MODE_NONE;
	k_work_cancel_delayable(&srv->rx_timeout);
	k_work_cancel_delayable(&srv->pull.report);
	erase_state(srv);
}

const struct bt_mesh_model_cb _bt_mesh_blob_srv_cb = {
	.init = blob_srv_init,
	.settings_set = blob_srv_settings_set,
	.start = blob_srv_start,
	.reset = blob_srv_reset,
};

int bt_mesh_blob_srv_recv(struct bt_mesh_blob_srv *srv, uint64_t id,
			  const struct bt_mesh_blob_io *io, uint8_t ttl,
			  uint16_t timeout_base)
{
	if (bt_mesh_blob_srv_is_busy(srv)) {
		return -EBUSY;
	}

	if (!io || !io->wr) {
		return -EINVAL;
	}

	srv->state.xfer.id = id;
	srv->state.ttl = ttl;
	srv->state.timeout_base = timeout_base;
	srv->io = io;
	srv->block.number = 0xffff;
	srv->state.xfer.chunk_size = 0xffff;
	phase_set(srv, BT_MESH_BLOB_XFER_PHASE_WAITING_FOR_START);
	store_state(srv);

	return 0;
}

int bt_mesh_blob_srv_cancel(struct bt_mesh_blob_srv *srv)
{
	if (!bt_mesh_blob_srv_is_busy(srv)) {
		return -EALREADY;
	}

	cancel(srv);

	return 0;
}

bool bt_mesh_blob_srv_is_busy(const struct bt_mesh_blob_srv *srv)
{
	return srv->phase != BT_MESH_BLOB_XFER_PHASE_INACTIVE &&
	       srv->phase != BT_MESH_BLOB_XFER_PHASE_SUSPENDED &&
	       srv->phase != BT_MESH_BLOB_XFER_PHASE_COMPLETE;
}

uint8_t bt_mesh_blob_srv_progress(const struct bt_mesh_blob_srv *srv)
{
	uint32_t total;
	uint32_t received;

	if (srv->phase == BT_MESH_BLOB_XFER_PHASE_INACTIVE ||
	    srv->phase == BT_MESH_BLOB_XFER_PHASE_WAITING_FOR_START) {
		return 0;
	}

	total = block_count_get(srv);

	received = 0;
	for (int i = 0; i < total; ++i) {
		if (!atomic_test_bit(srv->state.blocks, i)) {
			received++;
		}
	}

	return (100U * received) / total;
}
