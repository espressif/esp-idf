/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <zephyr/bluetooth/mesh.h>
#include <zephyr/bluetooth/crypto.h>
#include <zephyr/settings/settings.h>
#include "access.h"
#include "dfu.h"
#include "blob.h"
#include <common/bt_str.h>

#define LOG_LEVEL CONFIG_BT_MESH_DFU_LOG_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(bt_mesh_dfu_cli);

#define TARGETS_FOR_EACH(cli, target)                                          \
	SYS_SLIST_FOR_EACH_CONTAINER(                                          \
		(sys_slist_t *)&((cli)->blob.inputs)->targets, target, blob.n)

#define MSG_CTX(cli, dst)                                                      \
	{                                                                      \
		.app_idx = (cli)->blob.inputs->app_idx, .addr = dst,           \
		.send_ttl = (cli)->blob.inputs->ttl,                           \
	}

#define DFU_CLI(blob_cli) CONTAINER_OF(blob_cli, struct bt_mesh_dfu_cli, blob)

BUILD_ASSERT((DFU_UPDATE_START_MSG_MAXLEN + BT_MESH_MODEL_OP_LEN(BT_MESH_DFU_OP_UPDATE_START) +
	      BT_MESH_MIC_SHORT) <= BT_MESH_TX_SDU_MAX,
	     "The Firmware Update Start message does not fit into the maximum outgoing SDU size.");

BUILD_ASSERT((DFU_UPDATE_INFO_STATUS_MSG_MINLEN +
	      BT_MESH_MODEL_OP_LEN(BT_MESH_DFU_OP_UPDATE_INFO_STATUS) + BT_MESH_MIC_SHORT)
	     <= BT_MESH_RX_SDU_MAX,
	     "The Firmware Update Info Status message does not fit into the maximum incoming SDU "
	     "size.");

enum req {
	REQ_NONE,
	REQ_METADATA,
	REQ_IMG,
	REQ_STATUS,
};

enum {
	FLAG_FAILED = BIT(0),
	FLAG_CANCELLED = BIT(1),
	FLAG_SKIP_CAPS_GET = BIT(2),
	FLAG_RESUME = BIT(3),
	FLAG_COMPLETED = BIT(4),
};

enum {
	STATE_IDLE,
	STATE_TRANSFER,
	STATE_REFRESH,
	STATE_VERIFIED,
	STATE_APPLY,
	STATE_APPLIED,
	STATE_CONFIRM,
	STATE_CANCEL,
	STATE_SUSPENDED,
};

static int32_t dfu_cli_timeout = (10 * MSEC_PER_SEC);

static struct bt_mesh_dfu_target *target_get(struct bt_mesh_dfu_cli *cli,
					     uint16_t addr)
{
	struct bt_mesh_dfu_target *target;

	TARGETS_FOR_EACH(cli, target) {
		if (addr == target->blob.addr) {
			return target;
		}
	}

	return NULL;
}

static void target_failed(struct bt_mesh_dfu_cli *cli,
			  struct bt_mesh_dfu_target *target,
			  enum bt_mesh_dfu_status status)
{
	target->status = status;

	LOG_ERR("Target 0x%04x failed: %u", target->blob.addr, status);

	/* Invalidate blob status to prevent the target from being included in
	 * future sending:
	 */
	if (target->blob.status == BT_MESH_BLOB_SUCCESS) {
		target->blob.status = BT_MESH_BLOB_ERR_INTERNAL;
	}

	if (cli->cb && cli->cb->lost_target) {
		cli->cb->lost_target(cli, target);
	}
}

static void dfu_complete(struct bt_mesh_dfu_cli *cli)
{
	LOG_DBG("");

	if (cli->cb && cli->cb->ended) {
		cli->cb->ended(cli, BT_MESH_DFU_SUCCESS);
	}
}

static void dfu_applied(struct bt_mesh_dfu_cli *cli)
{
	LOG_DBG("");

	cli->xfer.state = STATE_APPLIED;

	if (cli->cb && cli->cb->applied) {
		cli->cb->applied(cli);
	}
}

static void dfu_failed(struct bt_mesh_dfu_cli *cli,
		       enum bt_mesh_dfu_status reason)
{
	LOG_DBG("%u", reason);

	cli->xfer.flags |= FLAG_FAILED;

	if (cli->cb && cli->cb->ended) {
		cli->cb->ended(cli, reason);
	}
}

static int req_setup(struct bt_mesh_dfu_cli *cli, enum req type, uint16_t addr,
		     void *params)
{
	if (cli->req.type != REQ_NONE) {
		return -EBUSY;
	}

	cli->req.addr = addr;
	cli->req.params = params;
	cli->req.type = type;

	return 0;
}

static int req_wait(struct bt_mesh_dfu_cli *cli, k_timeout_t timeout)
{
	int err;

	err = k_sem_take(&cli->req.sem, timeout);
	cli->req.type = REQ_NONE;

	return err;
}

static bool targets_active(struct bt_mesh_dfu_cli *cli)
{
	struct bt_mesh_dfu_target *target;

	TARGETS_FOR_EACH(cli, target) {
		if (target->status == BT_MESH_DFU_SUCCESS) {
			return true;
		}
	}

	return false;
}

/*******************************************************************************
 * Blob client
 ******************************************************************************/
static void refresh(struct bt_mesh_dfu_cli *cli);

static void blob_caps(struct bt_mesh_blob_cli *b,
		      const struct bt_mesh_blob_cli_caps *caps)
{
	struct bt_mesh_dfu_cli *cli = DFU_CLI(b);
	int err;

	if (!caps) {
		dfu_failed(cli, BT_MESH_DFU_ERR_RESOURCES);
		return;
	}

	cli->xfer.blob.block_size_log = caps->max_block_size_log;
	cli->xfer.blob.chunk_size = caps->max_chunk_size;

	/* If mode is not already set and server reported it supports all modes
	 * default to PUSH, otherwise set value reported by server. If mode
	 * was set and server supports all modes, keep old value; set
	 * reported value otherwise.
	 */
	if (!(cli->xfer.blob.mode & BT_MESH_BLOB_XFER_MODE_ALL)) {
		cli->xfer.blob.mode =
			caps->modes == BT_MESH_BLOB_XFER_MODE_ALL ?
			BT_MESH_BLOB_XFER_MODE_PUSH : caps->modes;
	} else {
		cli->xfer.blob.mode =
			caps->modes == BT_MESH_BLOB_XFER_MODE_ALL ?
			cli->xfer.blob.mode : caps->modes;
	}

	err = bt_mesh_blob_cli_send(b, b->inputs, &cli->xfer.blob, cli->xfer.io);
	if (err) {
		LOG_ERR("Starting BLOB xfer failed: %d", err);
		dfu_failed(cli, BT_MESH_DFU_ERR_BLOB_XFER_BUSY);
	}
}

static void blob_lost_target(struct bt_mesh_blob_cli *b,
			     struct bt_mesh_blob_target *blobt,
			     enum bt_mesh_blob_status reason)
{
	struct bt_mesh_dfu_target *target =
		CONTAINER_OF(blobt, struct bt_mesh_dfu_target, blob);
	struct bt_mesh_dfu_cli *cli = DFU_CLI(b);

	if ((cli->xfer.state == STATE_CONFIRM || cli->xfer.state == STATE_APPLY) &&
	    target->effect == BT_MESH_DFU_EFFECT_UNPROV) {
		/* Reset status for such targets to use them in consequent procedures. See sections
		 * 7.1.2.6 and 7.1.2.9 of the MeshDFU.
		 */
		target->blob.status = BT_MESH_BLOB_SUCCESS;
		return;
	}

	target_failed(cli, target, BT_MESH_DFU_ERR_INTERNAL);
}

static void blob_suspended(struct bt_mesh_blob_cli *b)
{
	struct bt_mesh_dfu_cli *cli = DFU_CLI(b);

	LOG_DBG("BLOB transfer suspended");

	cli->xfer.state = STATE_SUSPENDED;

	if (cli->cb && cli->cb->suspended) {
		cli->cb->suspended(cli);
	}
}

static void blob_end(struct bt_mesh_blob_cli *b,
		     const struct bt_mesh_blob_xfer *xfer, bool success)
{
	struct bt_mesh_dfu_cli *cli = DFU_CLI(b);

	cli->req.img_cb = NULL;

	if (success) {
		refresh(cli);
		return;
	}

	if (cli->xfer.state == STATE_CANCEL) {
		/* The user cancelled the transfer, DFU will end when all
		 * targets have been notified.
		 */
		return;
	}

	if (cli->xfer.state != STATE_TRANSFER) {
		LOG_ERR("Blob failed in invalid state %u", cli->xfer.state);
		return;
	}

	dfu_failed(cli, BT_MESH_DFU_ERR_INTERNAL);
}

const struct bt_mesh_blob_cli_cb _bt_mesh_dfu_cli_blob_handlers = {
	.caps = blob_caps,
	.lost_target = blob_lost_target,
	.suspended = blob_suspended,
	.end = blob_end,
};

/*******************************************************************************
 * Message sending
 ******************************************************************************/

static void tx_start(uint16_t dur, int err, void *cb_data);
static void tx_end(int err, void *cb_data);

static const struct bt_mesh_send_cb send_cb = {
	.start = tx_start,
	.end = tx_end,
};

static void tx_start(uint16_t dur, int err, void *cb_data)
{
	if (err) {
		tx_end(err, cb_data);
	}
}

static void tx_end(int err, void *cb_data)
{
	struct bt_mesh_dfu_cli *cli = cb_data;

	blob_cli_broadcast_tx_complete(&cli->blob);
}

static int tx(const struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx,
	      struct net_buf_simple *buf, const struct bt_mesh_send_cb *cb,
	      struct bt_mesh_dfu_cli *cli)
{
	int err;

	err = bt_mesh_model_send(mod, ctx, buf, cb, cli);
	if (err) {
		LOG_ERR("Send err: %d", err);
		if (cb) {
			cb->end(err, cli);
		}
		return err;
	}

	return 0;
}

static int info_get(struct bt_mesh_dfu_cli *cli, struct bt_mesh_msg_ctx *ctx,
		    uint8_t idx, uint8_t max_count,
		    const struct bt_mesh_send_cb *cb)
{
	BT_MESH_MODEL_BUF_DEFINE(buf, BT_MESH_DFU_OP_UPDATE_INFO_GET, 2);
	bt_mesh_model_msg_init(&buf, BT_MESH_DFU_OP_UPDATE_INFO_GET);
	net_buf_simple_add_u8(&buf, idx);
	net_buf_simple_add_u8(&buf, max_count);

	return tx(cli->mod, ctx, &buf, cb, cli);
}

static void send_info_get(struct bt_mesh_blob_cli *b, uint16_t dst)
{
	struct bt_mesh_dfu_cli *cli = DFU_CLI(b);
	struct bt_mesh_msg_ctx ctx = MSG_CTX(cli, dst);

	cli->req.img_cnt = 0xff;

	info_get(cli, &ctx, 0, cli->req.img_cnt, &send_cb);
}

static void send_update_start(struct bt_mesh_blob_cli *b, uint16_t dst)
{
	struct bt_mesh_dfu_cli *cli = DFU_CLI(b);
	struct bt_mesh_msg_ctx ctx = MSG_CTX(cli, dst);
	struct bt_mesh_dfu_target *target;

	if (b->tx.ctx.force_unicast) {
		target = target_get(cli, dst);
	} else {
		target = SYS_SLIST_PEEK_HEAD_CONTAINER(
						(sys_slist_t *)&((cli)->blob.inputs)->targets,
						target, blob.n);
	}

	BT_MESH_MODEL_BUF_DEFINE(buf, BT_MESH_DFU_OP_UPDATE_START,
				 DFU_UPDATE_START_MSG_MAXLEN);
	bt_mesh_model_msg_init(&buf, BT_MESH_DFU_OP_UPDATE_START);

	net_buf_simple_add_u8(&buf, cli->blob.inputs->ttl);
	net_buf_simple_add_le16(&buf, cli->blob.inputs->timeout_base);
	net_buf_simple_add_le64(&buf, cli->xfer.blob.id);
	net_buf_simple_add_u8(&buf, target->img_idx);
	net_buf_simple_add_mem(&buf, cli->xfer.slot->metadata,
			       cli->xfer.slot->metadata_len);

	(void)tx(cli->mod, &ctx, &buf, &send_cb, cli);
}

static void send_update_get(struct bt_mesh_blob_cli *b, uint16_t dst)
{
	struct bt_mesh_dfu_cli *cli = DFU_CLI(b);
	struct bt_mesh_msg_ctx ctx = MSG_CTX(cli, dst);

	BT_MESH_MODEL_BUF_DEFINE(buf, BT_MESH_DFU_OP_UPDATE_GET, 0);
	bt_mesh_model_msg_init(&buf, BT_MESH_DFU_OP_UPDATE_GET);

	(void)tx(cli->mod, &ctx, &buf, &send_cb, cli);
}

static void send_update_cancel(struct bt_mesh_blob_cli *b, uint16_t dst)
{
	struct bt_mesh_dfu_cli *cli = DFU_CLI(b);
	struct bt_mesh_msg_ctx ctx = MSG_CTX(cli, dst);

	BT_MESH_MODEL_BUF_DEFINE(buf, BT_MESH_DFU_OP_UPDATE_CANCEL, 0);
	bt_mesh_model_msg_init(&buf, BT_MESH_DFU_OP_UPDATE_CANCEL);

	(void)tx(cli->mod, &ctx, &buf, &send_cb, cli);
}

static void send_update_apply(struct bt_mesh_blob_cli *b, uint16_t dst)
{
	struct bt_mesh_dfu_cli *cli = DFU_CLI(b);
	struct bt_mesh_msg_ctx ctx = MSG_CTX(cli, dst);

	BT_MESH_MODEL_BUF_DEFINE(buf, BT_MESH_DFU_OP_UPDATE_APPLY, 0);
	bt_mesh_model_msg_init(&buf, BT_MESH_DFU_OP_UPDATE_APPLY);

	(void)tx(cli->mod, &ctx, &buf, &send_cb, cli);
}

/*******************************************************************************
 * Distribution procedure
 ******************************************************************************/
static void transfer(struct bt_mesh_blob_cli *b);
static void apply(struct bt_mesh_dfu_cli *cli);
static void applied(struct bt_mesh_blob_cli *b);
static void confirmed(struct bt_mesh_blob_cli *b);
static void cancelled(struct bt_mesh_blob_cli *b);

static void initiate(struct bt_mesh_dfu_cli *cli)
{
	struct blob_cli_broadcast_ctx tx = {
		.send = send_update_start,
		.next = transfer,
		.acked = true,
	};
	struct bt_mesh_dfu_target *target;
	int img_idx = -1;

	/** If firmware img index is the same for all targets, we can send Firmware Update Start
	 * message using multicast address. Otherwise, it has to be send in a unicast way.
	 */
	TARGETS_FOR_EACH(cli, target) {
		if (img_idx == -1) {
			img_idx = target->img_idx;
		} else if (target->img_idx != img_idx) {
			tx.force_unicast = true;
			break;
		}
	}

	LOG_DBG("");

	cli->op = BT_MESH_DFU_OP_UPDATE_STATUS;
	cli->xfer.state = STATE_TRANSFER;

	blob_cli_broadcast(&cli->blob, &tx);
}

static void skip_targets_from_broadcast(struct bt_mesh_dfu_cli *cli, bool skip)
{
	struct bt_mesh_dfu_target *target;

	TARGETS_FOR_EACH(cli, target) {
		/* If distributor is in the targets list, or target is in Verify phase,
		 * disable it until Retrieve Capabilities and BLOB Transfer procedures
		 * are completed.
		 */
		if (bt_mesh_has_addr(target->blob.addr) ||
		    target->phase == BT_MESH_DFU_PHASE_VERIFY) {
			target->blob.skip = skip;
			break;
		}
	}
}

static bool transfer_skip(struct bt_mesh_dfu_cli *cli)
{
	struct bt_mesh_dfu_target *target;

	TARGETS_FOR_EACH(cli, target) {
		if (!bt_mesh_has_addr(target->blob.addr) || !target->blob.skip) {
			return false;
		}
	}

	return true;
}

static void transfer(struct bt_mesh_blob_cli *b)
{
	struct bt_mesh_dfu_cli *cli = DFU_CLI(b);
	int err;

	LOG_DBG("");

	if (!targets_active(cli)) {
		dfu_failed(cli, BT_MESH_DFU_ERR_INTERNAL);
		return;
	}

	skip_targets_from_broadcast(cli, true);

	if (transfer_skip(cli)) {
		/* If distributor only updates itself, or all targets are in Verify phase,
		 * proceed to the refresh step immediately.
		 */
		refresh(cli);
		return;
	}

	if (cli->xfer.flags & FLAG_RESUME) {
		cli->xfer.flags ^= FLAG_RESUME;
		err = bt_mesh_blob_cli_resume(b);
		if (err) {
			LOG_ERR("Resuming BLOB xfer failed: %d", err);
			dfu_failed(cli, BT_MESH_DFU_ERR_BLOB_XFER_BUSY);
		}
	} else if (cli->xfer.flags & FLAG_SKIP_CAPS_GET) {
		cli->xfer.flags ^= FLAG_SKIP_CAPS_GET;
		err = bt_mesh_blob_cli_send(b, b->inputs, &cli->xfer.blob, cli->xfer.io);
		if (err) {
			LOG_ERR("Starting BLOB xfer failed: %d", err);
			dfu_failed(cli, BT_MESH_DFU_ERR_BLOB_XFER_BUSY);
		}
	} else {
		err = bt_mesh_blob_cli_caps_get(&cli->blob, cli->blob.inputs);
		if (err) {
			LOG_ERR("Failed starting blob xfer: %d", err);
			dfu_failed(cli, BT_MESH_DFU_ERR_BLOB_XFER_BUSY);
		}
	}
}

static void refreshed(struct bt_mesh_blob_cli *b)
{
	struct bt_mesh_dfu_cli *cli = DFU_CLI(b);

	if (!targets_active(cli)) {
		dfu_failed(cli, BT_MESH_DFU_ERR_INTERNAL);
		return;
	}

	cli->xfer.state = STATE_VERIFIED;
	dfu_complete(cli);
}

static void refresh(struct bt_mesh_dfu_cli *cli)
{
	const struct blob_cli_broadcast_ctx tx = {
		.send = send_update_get,
		.next = refreshed,
		.acked = true
	};

	LOG_DBG("");

	cli->xfer.state = STATE_REFRESH;
	cli->op = BT_MESH_DFU_OP_UPDATE_STATUS;

	/* If distributor is in the targets list, enable it again so it participates in Distribute
	 * Firmware procedure.
	 */
	skip_targets_from_broadcast(cli, false);

	blob_cli_broadcast(&cli->blob, &tx);
}

static void apply(struct bt_mesh_dfu_cli *cli)
{
	const struct blob_cli_broadcast_ctx tx = {
		.send = send_update_apply,
		.next = applied,
		.acked = true
	};

	LOG_DBG("");

	cli->xfer.state = STATE_APPLY;
	cli->op = BT_MESH_DFU_OP_UPDATE_STATUS;

	blob_cli_broadcast(&cli->blob, &tx);
}

static void applied(struct bt_mesh_blob_cli *b)
{
	struct bt_mesh_dfu_cli *cli = DFU_CLI(b);

	if (!targets_active(cli)) {
		dfu_failed(cli, BT_MESH_DFU_ERR_INTERNAL);
		return;
	}

	dfu_applied(cli);
}

static enum bt_mesh_dfu_iter target_img_cb(struct bt_mesh_dfu_cli *cli,
					   struct bt_mesh_msg_ctx *ctx,
					   uint8_t idx, uint8_t cnt,
					   const struct bt_mesh_dfu_img *img,
					   void *cb_data)
{
	struct bt_mesh_dfu_target *target;

	if ((img->fwid_len != cli->xfer.slot->fwid_len) ||
	    memcmp(cli->xfer.slot->fwid, img->fwid, img->fwid_len)) {
		return BT_MESH_DFU_ITER_CONTINUE;
	}

	target = target_get(cli, ctx->addr);
	if (target) {
		LOG_DBG("SUCCESS: 0x%04x applied dfu (as image %u)", ctx->addr,
			idx);
		target->phase = BT_MESH_DFU_PHASE_APPLY_SUCCESS;
		blob_cli_broadcast_rsp(&cli->blob, &target->blob);
	} else {
		LOG_WRN("Target 0x%04x not found", ctx->addr);
	}

	return BT_MESH_DFU_ITER_STOP;
}

static void confirm(struct bt_mesh_dfu_cli *cli)
{
	const struct blob_cli_broadcast_ctx tx = {
		.send = send_info_get,
		.next = confirmed,
		.acked = true,
		.optional = true,
	};

	LOG_DBG("");

	cli->op = BT_MESH_DFU_OP_UPDATE_INFO_STATUS;
	cli->req.img_cb = target_img_cb;
	cli->req.ttl = cli->blob.inputs->ttl;

	blob_cli_broadcast(&cli->blob, &tx);
}

static void confirmed(struct bt_mesh_blob_cli *b)
{
	struct bt_mesh_dfu_cli *cli = DFU_CLI(b);
	struct bt_mesh_dfu_target *target;
	bool success = false;

	cli->req.img_cb = NULL;

	TARGETS_FOR_EACH(cli, target) {
		if (target->status != BT_MESH_DFU_SUCCESS) {
			/* Target either failed at earlier stage or during confirmation. In any
			 * case, the app is already notified. Don't consider the target here.
			 */
			continue;
		}

		if (target->effect == BT_MESH_DFU_EFFECT_UNPROV) {
			if (!target->blob.acked) {
				success = true;
				continue;
			}

			LOG_DBG("Target 0x%04x still provisioned", target->blob.addr);
			target->phase = BT_MESH_DFU_PHASE_APPLY_FAIL;
			target_failed(cli, target, BT_MESH_DFU_ERR_INTERNAL);
		} else if (!target->blob.acked) {
			LOG_DBG("Target 0x%04x failed to respond", target->blob.addr);
			target->phase = BT_MESH_DFU_PHASE_APPLY_FAIL;
			target_failed(cli, target, BT_MESH_DFU_ERR_INTERNAL);
		} else if (target->status == BT_MESH_DFU_SUCCESS) {
			success = true;
		}
	}

	if (success) {
		cli->xfer.state = STATE_IDLE;
		cli->xfer.flags = FLAG_COMPLETED;

		if (cli->cb && cli->cb->confirmed) {
			cli->cb->confirmed(cli);
		}
	} else {
		dfu_failed(cli, BT_MESH_DFU_ERR_INTERNAL);
	}
}

static void cancel(struct bt_mesh_dfu_cli *cli)
{
	const struct blob_cli_broadcast_ctx tx = {
		.send = send_update_cancel,
		.next = cancelled,
		.acked = true
	};

	LOG_DBG("");

	cli->op = BT_MESH_DFU_OP_UPDATE_STATUS;

	blob_cli_broadcast(&cli->blob, &tx);
}

static void cancelled(struct bt_mesh_blob_cli *b)
{
	struct bt_mesh_dfu_cli *cli = DFU_CLI(b);

	cli->xfer.flags |= FLAG_CANCELLED;
	dfu_failed(cli, BT_MESH_DFU_ERR_INTERNAL);
}

/*******************************************************************************
 * Message handlers
 ******************************************************************************/

static int handle_status(const struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx,
			 struct net_buf_simple *buf)
{
	struct bt_mesh_dfu_cli *cli = mod->rt->user_data;
	enum bt_mesh_dfu_status status;
	enum bt_mesh_dfu_phase phase;
	struct bt_mesh_dfu_target *target;
	uint8_t byte;

	byte = net_buf_simple_pull_u8(buf);
	status = byte & BIT_MASK(3);
	phase = byte >> 5;

	if (cli->req.type == REQ_STATUS && cli->req.addr == ctx->addr) {
		if (cli->req.params) {
			struct bt_mesh_dfu_target_status *rsp = cli->req.params;

			rsp->status = status;
			rsp->phase = phase;
			if (buf->len == 13) {
				rsp->ttl = net_buf_simple_pull_u8(buf);
				rsp->effect = net_buf_simple_pull_u8(buf) & BIT_MASK(5);
				rsp->timeout_base = net_buf_simple_pull_le16(buf);
				rsp->blob_id = net_buf_simple_pull_le64(buf);
				rsp->img_idx = net_buf_simple_pull_u8(buf);
			} else if (buf->len) {
				return -EINVAL;
			}

			rsp->ttl = 0U;
			rsp->effect = BT_MESH_DFU_EFFECT_NONE;
			rsp->timeout_base = 0U;
			rsp->blob_id = 0U;
			rsp->img_idx = 0U;
		}
		k_sem_give(&cli->req.sem);
	}
	if (cli->op != BT_MESH_DFU_OP_UPDATE_STATUS) {
		return 0;
	}

	target = target_get(cli, ctx->addr);
	if (!target) {
		LOG_WRN("Unknown target 0x%04x", ctx->addr);
		return -ENOENT;
	}

	LOG_DBG("%u phase: %u, cur state: %u", status, phase, cli->xfer.state);

	target->phase = phase;

	if (cli->xfer.state == STATE_APPLY && phase == BT_MESH_DFU_PHASE_IDLE &&
	    status == BT_MESH_DFU_ERR_WRONG_PHASE) {
		LOG_DBG("Response received with Idle phase");
		blob_cli_broadcast_rsp(&cli->blob, &target->blob);
		return 0;
	}

	if (status != BT_MESH_DFU_SUCCESS) {
		target_failed(cli, target, status);
		blob_cli_broadcast_rsp(&cli->blob, &target->blob);
		return 0;
	}

	if (buf->len == 13) {
		net_buf_simple_pull_u8(buf); /* ttl */
		target->effect = net_buf_simple_pull_u8(buf) & BIT_MASK(5);
		net_buf_simple_pull_le16(buf); /* timeout */

		if (net_buf_simple_pull_le64(buf) != cli->xfer.blob.id) {
			LOG_WRN("Invalid BLOB ID");
			target_failed(cli, target, BT_MESH_DFU_ERR_BLOB_XFER_BUSY);
			blob_cli_broadcast_rsp(&cli->blob, &target->blob);
			return 0;
		}

		target->img_idx = net_buf_simple_pull_u8(buf);

		LOG_DBG("Target 0x%04x receiving transfer", ctx->addr);
	} else if (buf->len) {
		return -EINVAL;
	}

	if (cli->xfer.state == STATE_REFRESH) {
		if (phase == BT_MESH_DFU_PHASE_VERIFY) {
			LOG_DBG("Still pending...");
			return 0;
		} else if (phase == BT_MESH_DFU_PHASE_VERIFY_FAIL) {
			LOG_WRN("Verification failed on target 0x%04x",
				target->blob.addr);
			target_failed(cli, target, BT_MESH_DFU_ERR_WRONG_PHASE);
		}
	} else if (cli->xfer.state == STATE_APPLY) {
		if (phase != BT_MESH_DFU_PHASE_APPLYING &&
		    (target->effect == BT_MESH_DFU_EFFECT_UNPROV ||
		     phase != BT_MESH_DFU_PHASE_IDLE)) {
			LOG_WRN("Target 0x%04x in phase %u after apply",
				target->blob.addr, phase);
			target_failed(cli, target, BT_MESH_DFU_ERR_WRONG_PHASE);
			blob_cli_broadcast_rsp(&cli->blob, &target->blob);
			return 0;
		}
		return 0;
	} else if (cli->xfer.state == STATE_CONFIRM) {
		if (phase == BT_MESH_DFU_PHASE_APPLYING) {
			LOG_DBG("Still pending...");
			return 0;
		}

		if (phase != BT_MESH_DFU_PHASE_IDLE) {
			LOG_WRN("Target 0x%04x in phase %u after apply",
				target->blob.addr, phase);
			target->phase = BT_MESH_DFU_PHASE_APPLY_FAIL;
			target_failed(cli, target, BT_MESH_DFU_ERR_WRONG_PHASE);
			blob_cli_broadcast_rsp(&cli->blob, &target->blob);
			return 0;
		}
	} else if (cli->xfer.state == STATE_CANCEL) {
		target->phase = BT_MESH_DFU_PHASE_TRANSFER_CANCELED;
	}

	blob_cli_broadcast_rsp(&cli->blob, &target->blob);

	return 0;
}

static int handle_info_status(const struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx,
			      struct net_buf_simple *buf)
{
	struct bt_mesh_dfu_cli *cli = mod->rt->user_data;
	struct bt_mesh_dfu_target *target;
	enum bt_mesh_dfu_iter it = BT_MESH_DFU_ITER_CONTINUE;
	uint8_t img_cnt, idx;

	if (!cli->req.img_cb ||
	    (cli->req.type == REQ_IMG && cli->req.addr != ctx->addr)) {
		LOG_WRN("Unexpected info status from 0x%04x", ctx->addr);
		return 0;
	}

	img_cnt = net_buf_simple_pull_u8(buf);
	if (img_cnt < cli->req.img_cnt) {
		cli->req.img_cnt = img_cnt;
	}

	idx = net_buf_simple_pull_u8(buf);
	if (idx >= img_cnt) {
		LOG_WRN("Invalid idx %u", idx);
		return -ENOENT;
	}

	LOG_DBG("Image list from 0x%04x from index %u", ctx->addr, idx);

	while (buf->len && cli->req.img_cb && idx < cli->req.img_cnt) {
		char uri_buf[CONFIG_BT_MESH_DFU_URI_MAXLEN + 1];
		struct bt_mesh_dfu_img img;
		size_t uri_len;

		img.fwid_len = net_buf_simple_pull_u8(buf);
		if (buf->len < img.fwid_len + 1) {
			LOG_WRN("Invalid format: fwid");
			return -EINVAL;
		}

		img.fwid = net_buf_simple_pull_mem(buf, img.fwid_len);

		uri_len = net_buf_simple_pull_u8(buf);
		if (buf->len < uri_len) {
			LOG_WRN("Invalid format: uri");
			return -EINVAL;
		}

		LOG_DBG("\tImage %u\n\r\tfwid: %s", idx, bt_hex(img.fwid, img.fwid_len));

		if (uri_len) {
			size_t uri_buf_len =
				MIN(CONFIG_BT_MESH_DFU_URI_MAXLEN, uri_len);

			memcpy(uri_buf, net_buf_simple_pull_mem(buf, uri_len),
			       uri_buf_len);
			uri_buf[uri_buf_len] = '\0';
			img.uri = uri_buf;
		} else {
			img.uri = NULL;
		}

		it = cli->req.img_cb(cli, ctx, idx, img_cnt, &img,
				     cli->req.params);
		if (it != BT_MESH_DFU_ITER_CONTINUE) {
			if (cli->req.type == REQ_IMG) {
				k_sem_give(&cli->req.sem);
			}

			return 0;
		}

		idx++;
	}

	if (idx < cli->req.img_cnt) {
		LOG_DBG("Fetching more images (%u/%u)", idx, cli->req.img_cnt);
		ctx->send_ttl = cli->req.ttl;
		info_get(cli, ctx, idx, cli->req.img_cnt - idx,
			 (cli->req.type == REQ_IMG) ? NULL : &send_cb);
		return 0;
	}

	if (cli->req.type == REQ_IMG) {
		k_sem_give(&cli->req.sem);
		return 0;
	}

	/* Confirm-procedure termination: */
	target = target_get(cli, ctx->addr);
	if (target) {
		LOG_WRN("Target 0x%04x failed to apply image: %s", ctx->addr,
			bt_hex(cli->xfer.slot->fwid, cli->xfer.slot->fwid_len));
		target->phase = BT_MESH_DFU_PHASE_APPLY_FAIL;
		target_failed(cli, target, BT_MESH_DFU_ERR_INTERNAL);
		blob_cli_broadcast_rsp(&cli->blob, &target->blob);
	}

	return 0;
}

static int handle_metadata_status(const struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx,
				  struct net_buf_simple *buf)
{
	struct bt_mesh_dfu_cli *cli = mod->rt->user_data;
	struct bt_mesh_dfu_metadata_status *rsp = cli->req.params;
	uint8_t hdr, idx;

	hdr = net_buf_simple_pull_u8(buf);
	idx = net_buf_simple_pull_u8(buf);

	if (cli->req.type != REQ_METADATA || ctx->addr != cli->req.addr ||
	    idx != rsp->idx) {
		LOG_WRN("Unexpected metadata status from 0x%04x img %u",
			ctx->addr, idx);
		if (cli->req.type != REQ_METADATA) {
			LOG_WRN("Expected %u", cli->req.type);
		} else {
			LOG_WRN("Expected 0x%04x img %u", cli->req.addr, idx);
		}

		return 0;
	}

	rsp->status = hdr & BIT_MASK(3);
	rsp->effect = (hdr >> 3);
	k_sem_give(&cli->req.sem);

	return 0;
}

const struct bt_mesh_model_op _bt_mesh_dfu_cli_op[] = {
	{BT_MESH_DFU_OP_UPDATE_STATUS, BT_MESH_LEN_MIN(1), handle_status},
	{BT_MESH_DFU_OP_UPDATE_INFO_STATUS, BT_MESH_LEN_MIN(2), handle_info_status},
	{BT_MESH_DFU_OP_UPDATE_METADATA_STATUS, BT_MESH_LEN_EXACT(2), handle_metadata_status},
	BT_MESH_MODEL_OP_END,
};

static int dfu_cli_init(const struct bt_mesh_model *mod)
{
	struct bt_mesh_dfu_cli *cli = mod->rt->user_data;

	if (mod->rt->elem_idx != 0) {
		LOG_ERR("DFU update client must be instantiated on first elem");
		return -EINVAL;
	}

	cli->mod = mod;

	if (IS_ENABLED(CONFIG_BT_MESH_MODEL_EXTENSIONS)) {
		bt_mesh_model_extend(mod, cli->blob.mod);
	}

	k_sem_init(&cli->req.sem, 0, 1);

	return 0;
}

static void dfu_cli_reset(const struct bt_mesh_model *mod)
{
	struct bt_mesh_dfu_cli *cli = mod->rt->user_data;

	cli->req.type = REQ_NONE;
	cli->req.addr = BT_MESH_ADDR_UNASSIGNED;
	cli->req.img_cnt = 0;
	cli->req.img_cb = NULL;
	cli->xfer.state = STATE_IDLE;
	cli->xfer.flags = 0;
}

const struct bt_mesh_model_cb _bt_mesh_dfu_cli_cb = {
	.init = dfu_cli_init,
	.reset = dfu_cli_reset,
};

/*******************************************************************************
 * Public API
 ******************************************************************************/

int bt_mesh_dfu_cli_send(struct bt_mesh_dfu_cli *cli,
			 const struct bt_mesh_blob_cli_inputs *inputs,
			 const struct bt_mesh_blob_io *io,
			 const struct bt_mesh_dfu_cli_xfer *xfer)
{
	struct bt_mesh_dfu_target *target;

	if (bt_mesh_dfu_cli_is_busy(cli)) {
		return -EBUSY;
	}

	cli->xfer.blob.mode = xfer->mode;
	cli->xfer.blob.size = xfer->slot->size;

	if (xfer->blob_id == 0) {
		int err = bt_rand(&cli->xfer.blob.id, sizeof(cli->xfer.blob.id));

		if (err) {
			return err;
		}
	} else {
		cli->xfer.blob.id = xfer->blob_id;
	}

	cli->xfer.io = io;
	cli->blob.inputs = inputs;
	cli->xfer.slot = xfer->slot;
	cli->xfer.flags = 0U;

	if (xfer->blob_params) {
		cli->xfer.flags |= FLAG_SKIP_CAPS_GET;
		cli->xfer.blob.block_size_log = xfer->blob_params->block_size_log;
		cli->xfer.blob.chunk_size = xfer->blob_params->chunk_size;
	}

	/* Phase will be set based on target status messages: */
	TARGETS_FOR_EACH(cli, target) {
		target->status = BT_MESH_DFU_SUCCESS;
		target->phase = BT_MESH_DFU_PHASE_UNKNOWN;
	}

	initiate(cli);
	return 0;
}

int bt_mesh_dfu_cli_suspend(struct bt_mesh_dfu_cli *cli)
{
	int err;

	err = bt_mesh_blob_cli_suspend(&cli->blob);
	if (!err) {
		cli->xfer.state = STATE_SUSPENDED;
	}

	return err;
}

int bt_mesh_dfu_cli_resume(struct bt_mesh_dfu_cli *cli)
{
	struct bt_mesh_dfu_target *target;

	if (cli->xfer.state != STATE_SUSPENDED) {
		return -EINVAL;
	}

	cli->xfer.flags = FLAG_RESUME;

	/* Restore timed out targets. */
	TARGETS_FOR_EACH(cli, target) {
		if (!!target->blob.timedout) {
			target->status = BT_MESH_DFU_SUCCESS;
			target->phase = BT_MESH_DFU_PHASE_UNKNOWN;
		}
	}

	initiate(cli);
	return 0;
}

int bt_mesh_dfu_cli_cancel(struct bt_mesh_dfu_cli *cli,
			   struct bt_mesh_msg_ctx *ctx)
{
	if (ctx) {
		int err;

		err = req_setup(cli, REQ_STATUS, ctx->addr, NULL);
		if (err) {
			return err;
		}

		BT_MESH_MODEL_BUF_DEFINE(buf, BT_MESH_DFU_OP_UPDATE_CANCEL, 0);
		bt_mesh_model_msg_init(&buf, BT_MESH_DFU_OP_UPDATE_CANCEL);

		err = bt_mesh_model_send(cli->mod, ctx, &buf, NULL, NULL);
		if (err) {
			cli->req.type = REQ_NONE;
			return err;
		}

		return req_wait(cli, K_MSEC(dfu_cli_timeout));
	}

	if (cli->xfer.state == STATE_IDLE) {
		return -EALREADY;
	}

	cli->xfer.state = STATE_CANCEL;
	blob_cli_broadcast_abort(&cli->blob);
	cancel(cli);
	return 0;
}

int bt_mesh_dfu_cli_apply(struct bt_mesh_dfu_cli *cli)
{
	if (cli->xfer.state != STATE_VERIFIED) {
		return -EBUSY;
	}

	apply(cli);

	return 0;
}

int bt_mesh_dfu_cli_confirm(struct bt_mesh_dfu_cli *cli)
{
	if (cli->xfer.state != STATE_APPLIED) {
		return -EBUSY;
	}

	cli->xfer.state = STATE_CONFIRM;
	confirm(cli);

	return 0;
}

uint8_t bt_mesh_dfu_cli_progress(struct bt_mesh_dfu_cli *cli)
{
	if (cli->xfer.state == STATE_TRANSFER) {
		return bt_mesh_blob_cli_xfer_progress_active_get(&cli->blob);
	}

	if (cli->xfer.state == STATE_IDLE) {
		if (cli->xfer.flags & FLAG_COMPLETED) {
			return 100U;
		}
		return 0U;
	}

	return 100U;
}

bool bt_mesh_dfu_cli_is_busy(struct bt_mesh_dfu_cli *cli)
{
	return (cli->xfer.state == STATE_TRANSFER ||
		cli->xfer.state == STATE_REFRESH ||
		cli->xfer.state == STATE_APPLY ||
		cli->xfer.state == STATE_CONFIRM) &&
	       !(cli->xfer.flags & FLAG_FAILED);
}

int bt_mesh_dfu_cli_imgs_get(struct bt_mesh_dfu_cli *cli,
			     struct bt_mesh_msg_ctx *ctx,
			     bt_mesh_dfu_img_cb_t cb, void *cb_data,
			     uint8_t max_count)
{
	int err;

	if (cli->req.img_cb) {
		return -EBUSY;
	}

	err = req_setup(cli, REQ_IMG, ctx->addr, NULL);
	if (err) {
		return err;
	}

	cli->req.img_cb = cb;
	cli->req.params = cb_data;
	cli->req.ttl = ctx->send_ttl;
	cli->req.img_cnt = max_count;

	err = info_get(cli, ctx, 0, cli->req.img_cnt,  NULL);
	if (err) {
		cli->req.img_cb = NULL;
		cli->req.type = REQ_NONE;
		return err;
	}

	err = req_wait(cli, K_MSEC(dfu_cli_timeout));

	cli->req.img_cb = NULL;

	return err;
}

int bt_mesh_dfu_cli_metadata_check(struct bt_mesh_dfu_cli *cli,
				   struct bt_mesh_msg_ctx *ctx, uint8_t img_idx,
				   const struct bt_mesh_dfu_slot *slot,
				   struct bt_mesh_dfu_metadata_status *rsp)
{
	int err;

	err = req_setup(cli, REQ_METADATA, ctx->addr, rsp);
	if (err) {
		return err;
	}

	BT_MESH_MODEL_BUF_DEFINE(buf, BT_MESH_DFU_OP_UPDATE_METADATA_CHECK,
				 1 + CONFIG_BT_MESH_DFU_METADATA_MAXLEN);
	bt_mesh_model_msg_init(&buf, BT_MESH_DFU_OP_UPDATE_METADATA_CHECK);

	net_buf_simple_add_u8(&buf, img_idx);

	if (slot->metadata_len) {
		net_buf_simple_add_mem(&buf, slot->metadata,
				       slot->metadata_len);
	}

	rsp->idx = img_idx;

	err = bt_mesh_model_send(cli->mod, ctx, &buf, NULL, NULL);
	if (err) {
		cli->req.type = REQ_NONE;
		return err;
	}

	return req_wait(cli, K_MSEC(dfu_cli_timeout));
}

int bt_mesh_dfu_cli_status_get(struct bt_mesh_dfu_cli *cli,
			       struct bt_mesh_msg_ctx *ctx,
			       struct bt_mesh_dfu_target_status *rsp)
{
	int err;

	err = req_setup(cli, REQ_STATUS, ctx->addr, rsp);
	if (err) {
		return err;
	}

	BT_MESH_MODEL_BUF_DEFINE(buf, BT_MESH_DFU_OP_UPDATE_GET, 0);
	bt_mesh_model_msg_init(&buf, BT_MESH_DFU_OP_UPDATE_GET);

	err = bt_mesh_model_send(cli->mod, ctx, &buf, NULL, NULL);
	if (err) {
		cli->req.type = REQ_NONE;
		return err;
	}

	return req_wait(cli, K_MSEC(dfu_cli_timeout));
}

int32_t bt_mesh_dfu_cli_timeout_get(void)
{
	return dfu_cli_timeout;
}

void bt_mesh_dfu_cli_timeout_set(int32_t t)
{
	dfu_cli_timeout = t;
}
