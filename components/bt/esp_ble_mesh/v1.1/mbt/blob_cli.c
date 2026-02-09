/*
 * SPDX-FileCopyrightText: 2020 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>

#include "blob.h"
#include "net.h"
#include "transport.h"
#include "mesh/common.h"
#include "mesh/slist.h"
#include "mesh_v1.1/mbt/blob_cli.h"
#include "mesh_v1.1/mbt/blob_srv.h"
#include "mesh_v1.1/mbt/blob.h"
#include "mesh/buf.h"

#if CONFIG_BLE_MESH_BLOB_CLI

#define CHUNK_SIZE_MAX BLOB_TX_CHUNK_SIZE

#define TARGETS_FOR_EACH(cli, target)                                          \
        SYS_SLIST_FOR_EACH_CONTAINER((sys_slist_t *)&(cli)->inputs->targets,   \
                                     target, n)

/* The Maximum BLOB Poll Interval - T_MBPI */
#define BLOB_POLL_TIME_MAX_SECS 30

#define CLIENT_TIMEOUT_MSEC(cli) (10 * MSEC_PER_SEC * (cli->inputs->timeout_base + 2) + \
                                  100 * cli->inputs->ttl)
#define BLOCK_REPORT_TIME_MSEC ((BLOB_POLL_TIME_MAX_SECS * 2 + 7) * 1000)

/* BLOB Client is running Send Data State Machine from section 6.2.4.2. */
#define SENDING_CHUNKS_IN_PULL_MODE(cli) ((cli)->state == BT_MESH_BLOB_CLI_STATE_BLOCK_SEND && \
                                         (cli)->xfer->mode == BT_MESH_BLOB_XFER_MODE_PULL)
#define UNICAST_MODE(cli) ((cli)->inputs->group == BLE_MESH_ADDR_UNASSIGNED || \
                          (cli)->tx.ctx.force_unicast)

_Static_assert((BLOB_XFER_STATUS_MSG_MAXLEN + BLE_MESH_MODEL_OP_LEN(BT_MESH_BLOB_OP_XFER_STATUS) +
                BLE_MESH_MIC_SHORT) <= BLE_MESH_RX_SDU_MAX,
               "The BLOB Transfer Status message does not fit into the maximum incoming SDU size.");

_Static_assert((BLOB_BLOCK_REPORT_STATUS_MSG_MAXLEN +
                BLE_MESH_MODEL_OP_LEN(BT_MESH_BLOB_OP_BLOCK_REPORT) + BLE_MESH_MIC_SHORT)
               <= BLE_MESH_RX_SDU_MAX,
               "The BLOB Partial Block Report message does not fit into the maximum incoming SDU "
               "size.");

_Static_assert((BLOB_BLOCK_STATUS_MSG_MAXLEN + BLE_MESH_MODEL_OP_LEN(BT_MESH_BLOB_OP_BLOCK_STATUS) +
                BLE_MESH_MIC_SHORT) <= BLE_MESH_RX_SDU_MAX,
               "The BLOB Block Status message does not fit into the maximum incoming SDU size.");

NET_BUF_SIMPLE_DEFINE_STATIC(chunk_buf, BLOB_CHUNK_SDU_LEN(CHUNK_SIZE_MAX));
static bool chunk_sending;
static bool last_chunk_sent;

struct block_status {
    enum bt_mesh_blob_status status;
    enum bt_mesh_blob_chunks_missing missing;
    struct bt_mesh_blob_block block;
};

static struct bt_mesh_blob_target *next_target(struct bt_mesh_blob_cli *cli,
                                               struct bt_mesh_blob_target **current);
static void transfer_cancel(struct bt_mesh_blob_cli *cli);

static void start_retry_timer(struct bt_mesh_blob_cli *cli)
{
    k_timeout_t next_timeout;

    if (SENDING_CHUNKS_IN_PULL_MODE(cli)) {
        int64_t next_timeout_ms = cli->tx.cli_timestamp;
        struct bt_mesh_blob_target *target = NULL;

        TARGETS_FOR_EACH(cli, target) {
            if (!target->procedure_complete &&
                    target->status == BT_MESH_BLOB_SUCCESS &&
                    target->pull->block_report_timestamp < next_timeout_ms) {
                next_timeout_ms = target->pull->block_report_timestamp;
            }
        }

        /* cli_timestamp and block_report_timestamp represent absolute time, while
         * k_work_* functions use relative time.
         */
        next_timeout_ms -= k_uptime_get();
        next_timeout = next_timeout_ms <= 0 ? K_NO_WAIT : K_MSEC(next_timeout_ms);
    } else {
        next_timeout = K_MSEC(CLIENT_TIMEOUT_MSEC(cli) /
                              CONFIG_BLE_MESH_BLOB_CLI_BLOCK_RETRIES);
    }

    (void)k_work_reschedule(&cli->tx.retry, next_timeout);
}

static void cli_state_reset(struct bt_mesh_blob_cli *cli)
{
    k_work_cancel_delayable(&cli->tx.retry);
    cli->xfer = NULL;
    cli->state = BT_MESH_BLOB_CLI_STATE_NONE;
    cli->tx.ctx.is_inited = 0;
    cli->tx.cli_timestamp = 0ll;
    cli->tx.sending = 0;
}

static struct bt_mesh_blob_target *target_get(struct bt_mesh_blob_cli *cli,
                                              uint16_t addr)
{
    struct bt_mesh_blob_target *target;

    TARGETS_FOR_EACH(cli, target) {
        if (target->addr == addr) {
            return target;
        }
    }

    BT_ERR("Unknown target 0x%04x", addr);
    return NULL;
}

static void target_drop(struct bt_mesh_blob_cli *cli,
                        struct bt_mesh_blob_target *target,
                        enum bt_mesh_blob_status reason)
{
    BT_WARN("Dropping 0x%04x: %u", target->addr, reason);

    target->status = reason;
    if (cli->cb && cli->cb->lost_target) {
        cli->cb->lost_target(cli, target, reason);
    }
}

static uint32_t targets_reset(struct bt_mesh_blob_cli *cli)
{
    struct bt_mesh_blob_target *target;
    uint32_t count = 0;

    TARGETS_FOR_EACH(cli, target) {
        if (target->status == BT_MESH_BLOB_SUCCESS) {
            target->acked = 0U;
            count++;
        }
    }

    return count;
}

static bool targets_active(struct bt_mesh_blob_cli *cli)
{
    struct bt_mesh_blob_target *target;

    TARGETS_FOR_EACH(cli, target) {
        if (target->status == BT_MESH_BLOB_SUCCESS) {
            return true;
        }
    }

    return false;
}

static bool targets_timedout(struct bt_mesh_blob_cli *cli)
{
    struct bt_mesh_blob_target *target;

    TARGETS_FOR_EACH(cli, target) {
        if (!!target->timedout) {
            return true;
        }
    }

    return false;
}

static int io_open(struct bt_mesh_blob_cli *cli)
{
    if (!cli->io->open) {
        return 0;
    }

    return cli->io->open(cli->io, cli->xfer, BT_MESH_BLOB_READ);
}

static void io_close(struct bt_mesh_blob_cli *cli)
{
    if (!cli->io->close) {
        return;
    }

    cli->io->close(cli->io, cli->xfer);
}

static uint16_t next_missing_chunk(struct bt_mesh_blob_cli *cli,
                                   const uint8_t *missing_chunks,
                                   uint16_t idx)
{
    do {
        if (blob_chunk_missing_get(missing_chunks, idx)) {
            break;
        }
    } while (++idx < cli->block.chunk_count);

    return idx;
}

/* Used in Pull mode to collect all missing chunks from each target in cli->block.missing. */
static void update_missing_chunks(struct bt_mesh_blob_cli *cli)
{
    struct bt_mesh_blob_target *target;

    memset(cli->block.missing, 0, sizeof(cli->block.missing));

    TARGETS_FOR_EACH(cli, target) {
        if (target->procedure_complete || target->timedout) {
            continue;
        }

        for (size_t idx = 0; idx < cli->block.chunk_count; idx++) {
            bool missing = blob_chunk_missing_get(cli->block.missing, idx) ||
                           blob_chunk_missing_get(target->pull->missing, idx);
            blob_chunk_missing_set(cli->block.missing, idx, missing);
        }
    }
}

static inline size_t chunk_size(const struct bt_mesh_blob_xfer *xfer,
                                const struct bt_mesh_blob_block *block,
                                uint16_t chunk_idx)
{
    if ((chunk_idx == block->chunk_count - 1) &&
            (block->size % xfer->chunk_size)) {
        return block->size % xfer->chunk_size;
    }

    return xfer->chunk_size;
}

static int chunk_idx_decode(struct net_buf_simple *buf)
{
    uint16_t data;
    uint8_t byte;

    if (buf->len == 0) {
        return -EINVAL;
    }

    byte = net_buf_simple_pull_u8(buf);

    /* utf-8 decoding */
    if ((byte & 0xf0) == 0xe0) { /* 0x800 - 0xffff */
        if (buf->len < 2) {
            return -EINVAL;
        }

        data = (byte & 0x0f) << 12;
        data |= (net_buf_simple_pull_u8(buf) & 0x3f) << 6;
        data |= (net_buf_simple_pull_u8(buf) & 0x3f);
    } else if ((byte & 0xe0) == 0xc0) { /* 0x80 - 0x7ff */
        if (buf->len < 1) {
            return -EINVAL;
        }

        data = (byte & 0x1f) << 6;
        data |= (net_buf_simple_pull_u8(buf) & 0x3f);
    } else { /* 0x00 - 0x7f */
        data = byte & 0x7f;
    }

    return data;
}

static void block_set(struct bt_mesh_blob_cli *cli, uint16_t block_idx)
{
    cli->block.number = block_idx;
    cli->block.offset = block_idx * (1UL << cli->xfer->block_size_log);
    cli->block.size = blob_block_size(cli->xfer->size, cli->xfer->block_size_log,
                                      block_idx);
    cli->block.chunk_count =
        DIV_ROUND_UP(cli->block.size, cli->xfer->chunk_size);

    if (cli->xfer->mode == BT_MESH_BLOB_XFER_MODE_PUSH) {
        blob_chunk_missing_set_all(&cli->block);
    } else {
        struct bt_mesh_blob_target *target;

        /* In pull mode, the server will tell us which blocks are missing. */
        memset(cli->block.missing, 0, sizeof(cli->block.missing));

        TARGETS_FOR_EACH(cli, target) {
            memset(target->pull->missing, 0, sizeof(target->pull->missing));
        }
    }

    BT_DBG("%u size: %u chunks: %u", block_idx, cli->block.size,
           cli->block.chunk_count);
}

static void suspend(struct bt_mesh_blob_cli *cli)
{
    cli->state = BT_MESH_BLOB_CLI_STATE_SUSPENDED;

    if (cli->cb && cli->cb->suspended) {
        cli->cb->suspended(cli);
    }
}

static void end(struct bt_mesh_blob_cli *cli, bool success)
{
    const struct bt_mesh_blob_xfer *xfer = cli->xfer;

    BT_DBG("%u", success);

    io_close(cli);
    cli_state_reset(cli);
    if (cli->cb && cli->cb->end) {
        cli->cb->end(cli, xfer, success);
    }
}

static enum bt_mesh_blob_status caps_adjust(struct bt_mesh_blob_cli *cli,
                                            const struct bt_mesh_blob_cli_caps *in)
{
    if (!(in->modes & cli->caps.modes)) {
        return BT_MESH_BLOB_ERR_UNSUPPORTED_MODE;
    }

    if ((in->min_block_size_log > cli->caps.max_block_size_log) ||
            (in->max_block_size_log < cli->caps.min_block_size_log)) {
        return BT_MESH_BLOB_ERR_INVALID_BLOCK_SIZE;
    }

    cli->caps.min_block_size_log =
        MAX(cli->caps.min_block_size_log, in->min_block_size_log);
    cli->caps.max_block_size_log =
        MIN(cli->caps.max_block_size_log, in->max_block_size_log);
    cli->caps.max_chunks = MIN(cli->caps.max_chunks, in->max_chunks);
    cli->caps.mtu_size = MIN(cli->caps.mtu_size, in->mtu_size);
    cli->caps.max_chunk_size = MIN(cli->caps.max_chunk_size, in->max_chunk_size);
    cli->caps.modes &= in->modes;
    cli->caps.max_size = MIN(cli->caps.max_size, in->max_size);

    return BT_MESH_BLOB_SUCCESS;
}

/*******************************************************************************
 * TX State machine
 *
 * All messages in the transfer are going out to all the targets, either through
 * group messaging or directly to each. The TX state machine implements this
 * pattern for the transfer state machine to use. It will send the messages to
 * all devices (through the group or directly), repeating until it receives a
 * response from each device, or the attempts run out. Messages may also be
 * marked as unacked if they require no response.
 ******************************************************************************/

static struct bt_mesh_blob_target *next_target(struct bt_mesh_blob_cli *cli,
                                               struct bt_mesh_blob_target **current)
{
    if (*current) {
        *current = SYS_SLIST_PEEK_NEXT_CONTAINER(*current, n);
    } else {
        *current = SYS_SLIST_PEEK_HEAD_CONTAINER(
                       (sys_slist_t *)&cli->inputs->targets, *current, n);
    }

    while (*current) {
        if ((*current)->acked || (*current)->procedure_complete ||
                (*current)->status != BT_MESH_BLOB_SUCCESS || (*current)->timedout ||
                (*current)->skip) {
            goto next;
        }

        if (SENDING_CHUNKS_IN_PULL_MODE(cli) &&
                (k_uptime_get() < (*current)->pull->block_report_timestamp ||
                 !blob_chunk_missing_get((*current)->pull->missing, cli->chunk_idx))) {
            /* Skip targets that didn't time out or timed out, but confirmed
             * the currently transmitted chunk (cli->chunk_idx).
             */
            goto next;
        }

        break;

next:
        *current = SYS_SLIST_PEEK_NEXT_CONTAINER(*current, n);
    }

    return *current;
}

static void send(struct bt_mesh_blob_cli *cli)
{
    cli->tx.sending = 1U;
    if (UNICAST_MODE(cli)) {
        cli->tx.ctx.send(cli, cli->tx.target->addr);
    } else {
        cli->tx.ctx.send(cli, cli->inputs->group);
    }
}

static void broadcast_complete(struct bt_mesh_blob_cli *cli)
{
    BT_DBG("%s", cli->tx.cancelled ? "cancelling" : "continuing");

    cli->tx.ctx.is_inited = 0;
    k_work_cancel_delayable(&cli->tx.retry);

    if (cli->tx.cancelled) {
        transfer_cancel(cli);
    } else {
        __ASSERT(cli->tx.ctx.next, "No next callback");
        cli->tx.ctx.next(cli);
    }
}

static void tx_complete(struct k_work *work)
{
    struct k_work_delayable *dwork = k_work_delayable_from_work(work);
    struct bt_mesh_blob_cli *cli = CONTAINER_OF(dwork, struct bt_mesh_blob_cli, tx.complete);

    if (!cli->tx.ctx.is_inited || !cli->tx.sending) {
        return;
    }

    cli->tx.sending = 0U;

    if (cli->tx.cancelled) {
        broadcast_complete(cli);
        return;
    }

    if (cli->tx.ctx.send_complete) {
        cli->tx.ctx.send_complete(cli, cli->tx.target->addr);
    }

    if (UNICAST_MODE(cli) && next_target(cli, &cli->tx.target)) {
        send(cli);
        return;
    }

    if (cli->tx.ctx.acked && cli->tx.pending) {
        start_retry_timer(cli);
        return;
    }

    broadcast_complete(cli);
}

static void drop_remaining_targets(struct bt_mesh_blob_cli *cli)
{
    struct bt_mesh_blob_target *target;

    BT_DBG("");

    cli->tx.pending = 0;

    TARGETS_FOR_EACH(cli, target) {
        if (!target->acked && !target->timedout && !target->procedure_complete &&
                !target->skip) {
            target->timedout = 1U;
            target_drop(cli, target, BT_MESH_BLOB_ERR_INTERNAL);
        }
    }

    /* Update missing chunks to exclude chunks from dropped targets. */
    if (SENDING_CHUNKS_IN_PULL_MODE(cli)) {
        update_missing_chunks(cli);
    }
}

static void retry_timeout(struct k_work *work)
{
    struct bt_mesh_blob_cli *cli =
        CONTAINER_OF(work, struct bt_mesh_blob_cli, tx.retry.work);

    /* When sending chunks in Pull mode, timeout is handled differently. Client will drop all
     * non-responsive servers by cli_timestamp. By calling broadcast_complete(), client will
     * either retransmit the missing chunks (if any), or proceed to the next block, or suspend
     * the transfer if all targets timed out. All this is handled in block_check_end().
     * Retry logic for all other procedures in Pull mode is handled as in Push mode.
     */
    if (SENDING_CHUNKS_IN_PULL_MODE(cli)) {
        if (k_uptime_get() >= cli->tx.cli_timestamp) {
            BT_DBG("Transfer timed out.");

            if (!cli->tx.ctx.optional) {
                drop_remaining_targets(cli);
            }
        }

        broadcast_complete(cli);
        return;
    }

    BT_DBG("%u", cli->tx.retries);

    cli->tx.retries--;
    cli->tx.target = NULL;

    __ASSERT(!cli->tx.sending, "still sending");
    __ASSERT(cli->tx.ctx.is_inited, "ctx is not initialized");

    if (!cli->tx.retries) {
        BT_DBG("Transfer timed out.");

        if (!cli->tx.ctx.optional) {
            drop_remaining_targets(cli);
        }

        broadcast_complete(cli);
        return;
    }

    if (!cli->tx.ctx.acked || !next_target(cli, &cli->tx.target) || cli->tx.cancelled) {
        broadcast_complete(cli);
        return;
    }

    send(cli);
}

void blob_cli_broadcast(struct bt_mesh_blob_cli *cli,
                        const struct blob_cli_broadcast_ctx *ctx)
{
    if (cli->tx.ctx.is_inited || cli->tx.sending) {
        BT_ERR("BLOB cli busy");
        return;
    }

    cli->tx.cancelled = 0U;
    cli->tx.retries = CONFIG_BLE_MESH_BLOB_CLI_BLOCK_RETRIES;
    cli->tx.ctx = *ctx;
    cli->tx.ctx.is_inited = 1U;

    cli->tx.pending = targets_reset(cli);

    BT_DBG("%u targets", cli->tx.pending);

    cli->tx.target = NULL;
    if (!next_target(cli, &cli->tx.target)) {
        BT_DBG("No active targets");
        broadcast_complete(cli);
        return;
    }

    send(cli);
}

void blob_cli_broadcast_tx_complete(struct bt_mesh_blob_cli *cli)
{
    k_work_schedule(&cli->tx.complete, K_MSEC(cli->tx.ctx.post_send_delay_ms));
}

void blob_cli_broadcast_rsp(struct bt_mesh_blob_cli *cli,
                            struct bt_mesh_blob_target *target)
{
    if (target->acked) {
        return;
    }

    BT_DBG("0x%04x, pending: %d", target->addr, cli->tx.pending);

    target->acked = 1U;

    if (!--cli->tx.pending && !cli->tx.sending) {
        broadcast_complete(cli);
    }
}

void blob_cli_broadcast_abort(struct bt_mesh_blob_cli *cli)
{
    if (!cli->tx.ctx.is_inited &&
        cli->state != BT_MESH_BLOB_CLI_STATE_SUSPENDED) {
        return;
    }

    if ((cli)->state >= BT_MESH_BLOB_CLI_STATE_START) {
        io_close(cli);
    }

    cli_state_reset(cli);
}

static void send_start(uint16_t duration, int err, void *cb_data);
static void send_end(int err, void *user_data);

static int tx(struct bt_mesh_blob_cli *cli, uint16_t addr,
              struct net_buf_simple *buf)
{
    static const struct bt_mesh_send_cb end_cb = {
        .start = send_start,
        .end = send_end,
    };
    struct bt_mesh_msg_ctx ctx = {
        .app_idx = cli->inputs->app_idx,
        .addr = addr,
        .send_ttl = cli->inputs->ttl,
    };

    if (chunk_sending) {
        memcpy(&ctx.enh, &cli->xfer->chunk_enh_params,
               sizeof(bt_mesh_msg_enh_params_t));
    }

    int err;

    err = bt_mesh_model_send((struct bt_mesh_model *)cli->mod, &ctx, buf, &end_cb, cli);
    if (err) {
        BT_ERR("Send err: %d", err);
        send_end(err, cli);
        return err;
    }

    return 0;
}

static void send_start(uint16_t duration, int err, void *cb_data)
{
    if (err) {
        BT_ERR("TX Start failed: %d", err);
        send_end(err, cb_data);
    }
}

static void send_end(int err, void *user_data)
{
    struct bt_mesh_blob_cli *cli = user_data;

    if (chunk_sending) {
        chunk_sending = false;
    }

    if (!cli->tx.ctx.is_inited) {
        return;
    }

    blob_cli_broadcast_tx_complete(cli);
}

/*******************************************************************************
 * TX
 ******************************************************************************/

static void info_get_tx(struct bt_mesh_blob_cli *cli, uint16_t dst)
{
    BLE_MESH_MODEL_BUF_DEFINE(buf, BT_MESH_BLOB_OP_INFO_GET, 0);
    bt_mesh_model_msg_init(&buf, BT_MESH_BLOB_OP_INFO_GET);

    tx(cli, dst, &buf);
}

static void xfer_start_tx(struct bt_mesh_blob_cli *cli, uint16_t dst)
{
    BLE_MESH_MODEL_BUF_DEFINE(buf, BT_MESH_BLOB_OP_XFER_START, 16);
    bt_mesh_model_msg_init(&buf, BT_MESH_BLOB_OP_XFER_START);
    net_buf_simple_add_u8(&buf, cli->xfer->mode << 6);
    net_buf_simple_add_le64(&buf, cli->xfer->id);
    net_buf_simple_add_le32(&buf, cli->xfer->size);
    net_buf_simple_add_u8(&buf, cli->xfer->block_size_log);
#if CONFIG_BLE_MESH_LONG_PACKET
    if (cli->xfer->chunk_enh_params.long_pkt_cfg_used) {
        net_buf_simple_add_le16(&buf, BLE_MESH_EXT_TX_SDU_MAX);
    } else {
        net_buf_simple_add_le16(&buf, BLE_MESH_TX_SDU_MAX);
    }
#else
    net_buf_simple_add_le16(&buf, BLE_MESH_TX_SDU_MAX);
#endif

    tx(cli, dst, &buf);
}

static void xfer_get_tx(struct bt_mesh_blob_cli *cli, uint16_t dst)
{
    BLE_MESH_MODEL_BUF_DEFINE(buf, BT_MESH_BLOB_OP_XFER_GET, 0);
    bt_mesh_model_msg_init(&buf, BT_MESH_BLOB_OP_XFER_GET);

    tx(cli, dst, &buf);
}

static void xfer_cancel_tx(struct bt_mesh_blob_cli *cli, uint16_t dst)
{
    BLE_MESH_MODEL_BUF_DEFINE(buf, BT_MESH_BLOB_OP_XFER_CANCEL, 8);
    bt_mesh_model_msg_init(&buf, BT_MESH_BLOB_OP_XFER_CANCEL);
    net_buf_simple_add_le64(&buf, cli->xfer->id);

    tx(cli, dst, &buf);
}

static void block_start_tx(struct bt_mesh_blob_cli *cli, uint16_t dst)
{
    BLE_MESH_MODEL_BUF_DEFINE(buf, BT_MESH_BLOB_OP_BLOCK_START, 4);
    bt_mesh_model_msg_init(&buf, BT_MESH_BLOB_OP_BLOCK_START);
    net_buf_simple_add_le16(&buf, cli->block.number);
    net_buf_simple_add_le16(&buf, cli->xfer->chunk_size);

    tx(cli, dst, &buf);
}

static void chunk_tx(struct bt_mesh_blob_cli *cli, uint16_t dst)
{
    struct bt_mesh_blob_chunk chunk;
    int err;

    BT_DBG("Chunk will send to 0x%04x", dst);

    /* Changed by Espressif:
     * Use static chunk buf to avoid taking up large stack space */
    bt_mesh_model_msg_init(&chunk_buf, BT_MESH_BLOB_OP_CHUNK);
    net_buf_simple_add_le16(&chunk_buf, cli->chunk_idx);

    chunk.size = chunk_size(cli->xfer, &cli->block, cli->chunk_idx);
    chunk.offset = cli->xfer->chunk_size * cli->chunk_idx;
    chunk.data = net_buf_simple_add(&chunk_buf, chunk.size);

    if ((cli->block.number == cli->block_count - 1) &&
        (cli->chunk_idx < cli->block.chunk_count)) {
        last_chunk_sent = true;
    } else {
        last_chunk_sent = false;
    }

    err = cli->io->rd(cli->io, cli->xfer, &cli->block, &chunk);
    if (err || cli->state == BT_MESH_BLOB_CLI_STATE_NONE) {
        bt_mesh_blob_cli_cancel(cli);
        return;
    }

    tx(cli, dst, &chunk_buf);
}

static void block_get_tx(struct bt_mesh_blob_cli *cli, uint16_t dst)
{
    BLE_MESH_MODEL_BUF_DEFINE(buf, BT_MESH_BLOB_OP_BLOCK_GET, 0);
    bt_mesh_model_msg_init(&buf, BT_MESH_BLOB_OP_BLOCK_GET);

    tx(cli, dst, &buf);
}

/**************************************************************************************************
 * State machine
 *
 * The BLOB Client state machine walks through the steps in the BLOB transfer in the following
 * fashion:
 *
 *                                                 .---------------------------------------.
 *                                                 V                                       |
 * xfer_start -> block_set -> block_start -> chunk_send -> chunk_send_end                  |
 *                   A                                           |                         |
 *                   |                                           V                         |
 *                   |                                [more missing chunks?]-----[Yes]-----+
 *                   |                                           |                         |
 *                   |                                         [No]                        |
 *                   |                                           |                         |
 *                   |                                           V                         |
 *                   |                                         [mode?]                     |
 *                   |                             .---[Push]---'   '---[Pull]---.         |
 *                   |                             |                             |         |
 *                   |                             V                             V         |
 *                   |                        block_check               block_report_wait  |
 *                   |                             |                             |         |
 *                   |                             '-----------.   .-------------'         |
 *                   |                                         |   |                       |
 *                   |                                         V   V                       |
 *                   |                                    block_check_end                  |
 *                   |                                           |                         |
 *                   |                                           V                         |
 *                   |                                   [block completed?]------[No]------'
 *                   |                                           |
 *                   |                                         [Yes]
 *                   |                                           |
 *                   |                                           V
 *                   '-------------------[No]------------[last block sent?]
 *                                                               |
 *                                                             [Yes]
 *                                                               |
 *                                                               V
 *                                                        confirm_transfer
 *                                                               |
 *                                                               V
 *                                                        transfer_complete
 *
 * In each state, the Client transmits a message to all target nodes. In each state, except when
 * sending chunks (chunk_send), the Client expects a response from all target nodes, before
 * proceeding to the next state.
 *
 * When a target node responds, the Client calls @ref blob_cli_broadcast_rsp for the corresponding
 * target. Once all target nodes has responded, the Client proceeds to the next state.
 *
 * When sending chunks in Push mode, the Client will proceed to the next state (block_check) after
 * transmitting all missing chunks. In the block_check state, the Client will request a block status
 * from all target nodes. If any targets have missing chunks, the Client will resend them.
 *
 * When sending chunks in Pull mode, the Client addresses each target node individually using
 * @ref bt_mesh_blob_target_pull structure. The Client uses @ref bt_mesh_blob_cli::block::missing
 * to keep all missing chunks for the current block. Missing chunks for an individual target
 * is kept in @ref bt_mesh_blob_target_pull::missing. The Client uses @ref
 * bt_mesh_blob_target_pull::block_report_timeout to decide if it can send a chunk to this target.
 *
 * After sending all reported missing chunks to each target, the Client updates
 * @ref bt_mesh_blob_target_pull::block_report_timestamp value for every target individually in
 * chunk_tx_complete. The Client then proceeds to block_report_wait state and uses the earliest of
 * all block_report_timestamp and cli_timestamp to schedule the retry timer. When the retry
 * timer expires, the Client proceeds to the block_check_end state.
 *
 * In Pull mode, target nodes send a Partial Block Report message to the Client to inform about
 * missing chunks. The Client doesn't control when these messages are sent by target nodes, and
 * therefore it can't use @ref blob_cli_broadcast_rsp when it receives them. When the Client
 * receives the Partial Block Report message, it updates missing chunks, resets
 * block_report_timestamp, and explicitly calls @ref broadcast_complete to proceed to
 * block_check_end state.
 *
 **************************************************************************************************/
static void caps_collected(struct bt_mesh_blob_cli *cli);
static void block_start(struct bt_mesh_blob_cli *cli);
static void chunk_send(struct bt_mesh_blob_cli *cli);
static void block_check(struct bt_mesh_blob_cli *cli);
static void block_check_end(struct bt_mesh_blob_cli *cli);
static void block_report_wait(struct bt_mesh_blob_cli *cli);
static void chunk_send_end(struct bt_mesh_blob_cli *cli);
static void confirm_transfer(struct bt_mesh_blob_cli *cli);
static void transfer_complete(struct bt_mesh_blob_cli *cli);

static void caps_get(struct bt_mesh_blob_cli *cli)
{
    const struct blob_cli_broadcast_ctx ctx = {
        .send = info_get_tx,
        .next = caps_collected,
        .acked = true,
    };

    cli->state = BT_MESH_BLOB_CLI_STATE_CAPS_GET;
    blob_cli_broadcast(cli, &ctx);
}

static void caps_collected(struct bt_mesh_blob_cli *cli)
{
    struct bt_mesh_blob_target *target;
    bool success = false;

    cli->state = BT_MESH_BLOB_CLI_STATE_NONE;

    cli_state_reset(cli);

    TARGETS_FOR_EACH(cli, target) {
        if (target->status == BT_MESH_BLOB_SUCCESS) {
            success = true;
            break;
        }
    }

    while (success &&
            (1UL << cli->caps.max_block_size_log) >
            (cli->caps.max_chunk_size * cli->caps.max_chunks)) {
        cli->caps.max_block_size_log--;
    }

    if (cli->cb && cli->cb->caps) {
        cli->cb->caps(cli, success ? &cli->caps : NULL);
    }
}

static int xfer_start(struct bt_mesh_blob_cli *cli)
{
    const struct blob_cli_broadcast_ctx ctx = {
        .send = xfer_start_tx,
        .next = block_start,
        .acked = true,
    };
    int err;

    err = io_open(cli);
    if (err) {
        return -EIO;
    }

    cli->state = BT_MESH_BLOB_CLI_STATE_START;

    blob_cli_broadcast(cli, &ctx);
    return 0;
}

static void block_start(struct bt_mesh_blob_cli *cli)
{
    const struct blob_cli_broadcast_ctx ctx = {
        .send = block_start_tx,
        .next = chunk_send,
        .acked = true,
    };
    struct bt_mesh_blob_target *target;

    if (!targets_active(cli)) {
        if (targets_timedout(cli)) {
            suspend(cli);
            return;
        }

        end(cli, false);
        return;
    }

    BT_DBG("%u (%u chunks, %u/%u)", cli->block.number,
           cli->block.chunk_count, cli->block.number + 1, cli->block_count);

    cli->chunk_idx = 0;
    cli->state = BT_MESH_BLOB_CLI_STATE_BLOCK_START;
    /* Client Timeout Timer in Send Data State Machine is initialized initially after
     * transmitting the first bunch of chunks (see block_report_wait()). Next time it will be
     * updated after every Partial Block Report message.
     */
    cli->tx.cli_timestamp = 0ll;

    TARGETS_FOR_EACH(cli, target) {
        target->procedure_complete = 0U;

        if (cli->xfer->mode == BT_MESH_BLOB_XFER_MODE_PULL) {
            target->pull->block_report_timestamp = 0ll;
        }
    }

    if (cli->io->block_start) {
        cli->io->block_start(cli->io, cli->xfer, &cli->block);
        if (cli->state == BT_MESH_BLOB_CLI_STATE_NONE) {
            return;
        }
    }

    blob_cli_broadcast(cli, &ctx);
}

static void chunk_tx_complete(struct bt_mesh_blob_cli *cli, uint16_t dst)
{
    if (cli->xfer->mode != BT_MESH_BLOB_XFER_MODE_PULL) {
        return;
    }

    /* Update Block Report Timer individually for each target after sending out the last chunk
     * in current iteration.
     */
    uint16_t chunk_idx = next_missing_chunk(cli, cli->tx.target->pull->missing,
                                            cli->chunk_idx + 1);
    if (chunk_idx < cli->block.chunk_count) {
        /* Will send more chunks to this target in this iteration. */
        return;
    }

    /* This was the last chunk sent for this target. Now start the Block Report Timeout Timer.
     */
    struct bt_mesh_blob_target *target;
    int64_t timestamp = k_uptime_get() + BLOCK_REPORT_TIME_MSEC;

    if (!UNICAST_MODE(cli)) {
        /* If using group addressing, reset timestamp for all targets after all chunks are
         * sent to the group address
         */
        TARGETS_FOR_EACH(cli, target) {
            target->pull->block_report_timestamp = timestamp;
        }
        return;
    }

    cli->tx.target->pull->block_report_timestamp = timestamp;
}

static void chunk_send(struct bt_mesh_blob_cli *cli)
{
    struct blob_cli_broadcast_ctx ctx = {
        .send = chunk_tx,
        .next = chunk_send_end,
        .acked = false,
        .post_send_delay_ms = cli->chunk_interval_ms,
    };

    if (cli->xfer->mode == BT_MESH_BLOB_XFER_MODE_PULL) {
        ctx.send_complete = chunk_tx_complete;
    }

    if (!targets_active(cli)) {
        if (targets_timedout(cli)) {
            suspend(cli);
            return;
        }

        end(cli, false);
        return;
    }

    BT_DBG("%u / %u size: %u", cli->chunk_idx + 1, cli->block.chunk_count,
           chunk_size(cli->xfer, &cli->block, cli->chunk_idx));

    cli->state = BT_MESH_BLOB_CLI_STATE_BLOCK_SEND;
    chunk_sending = true;

    blob_cli_broadcast(cli, &ctx);
}

static void chunk_send_end(struct bt_mesh_blob_cli *cli)
{
    /* In pull mode, the partial block reports are used to confirm which
     * chunks have been received, while in push mode, we just assume that a
     * sent chunk has been received.
     */

    BT_DBG("A Chunk sent finished");

    if (cli->xfer->mode == BT_MESH_BLOB_XFER_MODE_PUSH) {
        blob_chunk_missing_set(cli->block.missing, cli->chunk_idx, false);
    }

    cli->chunk_idx = next_missing_chunk(cli, cli->block.missing, cli->chunk_idx + 1);
    if (cli->chunk_idx < cli->block.chunk_count) {
        chunk_send(cli);
        return;
    }

    if (cli->xfer->mode == BT_MESH_BLOB_XFER_MODE_PUSH) {
        block_check(cli);
    } else {
        block_report_wait(cli);
    }
}

/* The block checking pair(block_check - block_check_end)
 * is relevant only for Push mode.
 */
static void block_check(struct bt_mesh_blob_cli *cli)
{
    const struct blob_cli_broadcast_ctx ctx = {
        .send = block_get_tx,
        .next = block_check_end,
        .acked = true,
    };

    cli->state = BT_MESH_BLOB_CLI_STATE_BLOCK_CHECK;

    BT_DBG("");

    blob_cli_broadcast(cli, &ctx);
}

static void block_report_wait(struct bt_mesh_blob_cli *cli)
{
    const struct blob_cli_broadcast_ctx ctx = {
        .next = block_check_end,
        .acked = false,
    };

    /* Check if all servers already confirmed all chunks during the transmission. */
    if (next_missing_chunk(cli, cli->block.missing, 0) >= cli->block.chunk_count) {
        block_check_end(cli);
        return;
    }

    BT_DBG("Waiting for partial block report...");
    cli->tx.ctx = ctx;

    /* Start Client Timeout Timer in Send Data sub-procedure for the first time. */
    if (!cli->tx.cli_timestamp) {
        cli->tx.cli_timestamp = k_uptime_get() + CLIENT_TIMEOUT_MSEC(cli);
    }

    start_retry_timer(cli);
}

static void block_check_end(struct bt_mesh_blob_cli *cli)
{
    BT_DBG("");

    if (!targets_active(cli)) {
        if (targets_timedout(cli)) {
            suspend(cli);
            return;
        }

        end(cli, false);
        return;
    }

    cli->chunk_idx = next_missing_chunk(cli, cli->block.missing, 0);
    if (cli->chunk_idx < cli->block.chunk_count) {
        chunk_send(cli);
        return;
    }

    BT_DBG("No more missing chunks for block %u", cli->block.number);

    if (cli->io->block_end) {
        cli->io->block_end(cli->io, cli->xfer, &cli->block);
        if (cli->state == BT_MESH_BLOB_CLI_STATE_NONE) {
            return;
        }
    }

    if (cli->block.number == cli->block_count - 1) {
        struct bt_mesh_blob_target *target;

        TARGETS_FOR_EACH(cli, target) {
            target->procedure_complete = 0U;
        }

        confirm_transfer(cli);
        return;
    }

    block_set(cli, cli->block.number + 1);
    block_start(cli);
}

static void confirm_transfer(struct bt_mesh_blob_cli *cli)
{
    const struct blob_cli_broadcast_ctx ctx = {
        .send = xfer_get_tx,
        .next = transfer_complete,
        .acked = true,
    };

    BT_DBG("");

    cli->state = BT_MESH_BLOB_CLI_STATE_XFER_CHECK;

    blob_cli_broadcast(cli, &ctx);
}

static void progress_checked(struct bt_mesh_blob_cli *cli)
{
    BT_DBG("");

    cli->state = BT_MESH_BLOB_CLI_STATE_NONE;

    if (cli->cb && cli->cb->end) {
        cli->cb->xfer_progress_complete(cli);
    }
}

static void check_transfer(struct bt_mesh_blob_cli *cli)
{
    const struct blob_cli_broadcast_ctx ctx = {
        .send = xfer_get_tx,
        .next = progress_checked,
        .acked = true,
    };

    BT_DBG("");

    cli->state = BT_MESH_BLOB_CLI_STATE_XFER_PROGRESS_GET;

    blob_cli_broadcast(cli, &ctx);
}

static void transfer_cancel(struct bt_mesh_blob_cli *cli)
{
    const struct blob_cli_broadcast_ctx ctx = {
        .send = xfer_cancel_tx,
        .next = transfer_complete,
        .acked = true,
    };

    BT_DBG("");

    cli->state = BT_MESH_BLOB_CLI_STATE_CANCEL;

    blob_cli_broadcast(cli, &ctx);
}

static void transfer_complete(struct bt_mesh_blob_cli *cli)
{
    bool success = targets_active(cli) &&
                   cli->state == BT_MESH_BLOB_CLI_STATE_XFER_CHECK;

    end(cli, success);
}

/*******************************************************************************
 * RX
 ******************************************************************************/

static void rx_block_status(struct bt_mesh_blob_cli *cli,
                            struct bt_mesh_blob_target *target,
                            struct block_status *block)
{
    if (cli->state != BT_MESH_BLOB_CLI_STATE_BLOCK_START &&
            cli->state != BT_MESH_BLOB_CLI_STATE_BLOCK_SEND &&
            cli->state != BT_MESH_BLOB_CLI_STATE_BLOCK_CHECK) {
        BT_WARN("Invalid state %u", cli->state);
        return;
    }

    BT_WARN("0x%04x: block: %u status: %u", target->addr, block->block.number, block->status);

    if (block->status != BT_MESH_BLOB_SUCCESS &&
            /**
             * Changed by Espressif.
             * This behavior is not allowed by spec!
             *
             * BT_MESH_BLOB_ERR_INFO_UNAVAILABLE will caused
             * by blob server suspend, and the suspend state
             * will be resume in most scenes.
             *
             * so let blob client try to resume blob server.
            */
            block->status != BT_MESH_BLOB_ERR_INFO_UNAVAILABLE) {
        target_drop(cli, target, block->status);
        blob_cli_broadcast_rsp(cli, target);
        return;
    }

    if (block->block.number != cli->block.number) {
        BT_DBG("Invalid block num (expected %u)", cli->block.number);
        return;
    }

    if (block->missing == BT_MESH_BLOB_CHUNKS_MISSING_NONE) {
        target->procedure_complete = 1U;

        if (cli->xfer->mode == BT_MESH_BLOB_XFER_MODE_PULL) {
            memset(target->pull->missing, 0, sizeof(target->pull->missing));
            update_missing_chunks(cli);
        }

        BT_DBG("Target 0x%04x received all chunks", target->addr);
    } else if (block->missing == BT_MESH_BLOB_CHUNKS_MISSING_ALL) {
        blob_chunk_missing_set_all(&cli->block);
    } else if (cli->xfer->mode == BT_MESH_BLOB_XFER_MODE_PULL) {
        memcpy(target->pull->missing, block->block.missing, sizeof(block->block.missing));

        BT_DBG("Missing: %s", bt_hex(target->pull->missing, cli->block.chunk_count));

        update_missing_chunks(cli);

        /* Target has responded. Reset the timestamp so that client can start transmitting
         * missing chunks to it.
         */
        target->pull->block_report_timestamp = 0ll;
    } else {
        for (int i = 0; i < ARRAY_SIZE(block->block.missing); ++i) {
            cli->block.missing[i] |= block->block.missing[i];
        }
    }

    if (SENDING_CHUNKS_IN_PULL_MODE(cli)) {
        if (!cli->tx.sending) {
            /* If not sending, then the retry timer is running. Call
             * broadcast_complete() to proceed to block_check_end() and start
             * transmitting missing chunks.
             */
            broadcast_complete(cli);
        }

        /* When sending chunks in Pull mode, we don't confirm transaction when receiving
         * Partial Block Report message.
         */
        return;
    }

    blob_cli_broadcast_rsp(cli, target);
}

static int handle_xfer_status(const struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    struct bt_mesh_blob_cli *cli = mod->user_data;
    enum bt_mesh_blob_xfer_phase expected_phase;
    struct bt_mesh_blob_target *target;
    struct bt_mesh_blob_xfer_info info = { 0 };
    uint8_t status_and_mode;

    status_and_mode = net_buf_simple_pull_u8(buf);
    info.status = status_and_mode & BIT_MASK(4);
    info.mode = status_and_mode >> 6;
    info.phase = net_buf_simple_pull_u8(buf);

    if (buf->len) {
        info.id = net_buf_simple_pull_le64(buf);
    }

    if (buf->len >= 7) {
        info.size = net_buf_simple_pull_le32(buf);
        info.block_size_log = net_buf_simple_pull_u8(buf);
        info.mtu_size = net_buf_simple_pull_le16(buf);
        info.missing_blocks = net_buf_simple_pull(buf, buf->len);
    }

    BT_DBG("status: %u %s phase: %u %s", info.status,
           info.mode == BT_MESH_BLOB_XFER_MODE_PUSH ? "push" : "pull",
           info.phase, bt_hex(&info.id, 8));

    if (cli->state != BT_MESH_BLOB_CLI_STATE_START &&
            cli->state != BT_MESH_BLOB_CLI_STATE_XFER_CHECK &&
            cli->state != BT_MESH_BLOB_CLI_STATE_CANCEL &&
            cli->state != BT_MESH_BLOB_CLI_STATE_XFER_PROGRESS_GET) {
        BT_WARN("Wrong state: %d", cli->state);
        return -EBUSY;
    }

    target = target_get(cli, ctx->addr);
    if (!target) {
        return -ENOENT;
    }

    if (cli->state == BT_MESH_BLOB_CLI_STATE_START) {
        expected_phase = BT_MESH_BLOB_XFER_PHASE_WAITING_FOR_BLOCK;
    } else if (cli->state == BT_MESH_BLOB_CLI_STATE_XFER_CHECK) {
        expected_phase = BT_MESH_BLOB_XFER_PHASE_COMPLETE;
    } else if (cli->state != BT_MESH_BLOB_CLI_STATE_XFER_PROGRESS_GET) {
        expected_phase = BT_MESH_BLOB_XFER_PHASE_INACTIVE;
    } else { /* cli->state == BT_MESH_BLOB_CLI_STATE_XFER_PROGRESS_GET */
        blob_cli_broadcast_rsp(cli, target);
        if (cli->cb && cli->cb->xfer_progress) {
            cli->cb->xfer_progress(cli, target, &info);
        }
        return 0;
    }

    if (info.status != BT_MESH_BLOB_SUCCESS) {
        target_drop(cli, target, info.status);
    } else if (info.phase != expected_phase) {
        BT_WARN("Wrong phase: %u != %u", expected_phase, info.phase);
        return -EINVAL;
    } else if (info.phase != BT_MESH_BLOB_XFER_PHASE_INACTIVE &&
               info.id != cli->xfer->id) {
        target_drop(cli, target, BT_MESH_BLOB_ERR_WRONG_BLOB_ID);
    }

    blob_cli_broadcast_rsp(cli, target);

    return 0;
}

static int handle_block_report(const struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx,
                               struct net_buf_simple *buf)
{
    struct bt_mesh_blob_cli *cli = mod->user_data;
    struct block_status status = {
        .status = BT_MESH_BLOB_SUCCESS,
        .block.number = cli->block.number,
        .missing = (buf->len ? BT_MESH_BLOB_CHUNKS_MISSING_ENCODED :
                    BT_MESH_BLOB_CHUNKS_MISSING_NONE),
    };
    struct bt_mesh_blob_target *target;

    if (!cli->xfer) {
        return -EINVAL;
    }

    if (cli->xfer->mode == BT_MESH_BLOB_XFER_MODE_PUSH) {
        BT_WARN("Unexpected encoded block report in push mode");
        return -EINVAL;
    }

    BT_DBG("");

    target = target_get(cli, ctx->addr);
    if (!target) {
        return -ENOENT;
    }

    while (buf->len) {
        int idx;

        idx = chunk_idx_decode(buf);
        if (idx < 0) {
            return idx;
        }

        blob_chunk_missing_set(status.block.missing, idx, true);
    }

    /* If all chunks were already confirmed by this target, Send Data State Machine is in Final
     * state for this target. Therefore, the message should be ignored.
     */
    if (next_missing_chunk(cli, target->pull->missing, 0) >= cli->block.chunk_count) {
        BT_DBG("All chunks already confirmed");
        return 0;
    }

    cli->tx.cli_timestamp = k_uptime_get() + CLIENT_TIMEOUT_MSEC(cli);

    rx_block_status(cli, target, &status);

    return 0;
}

static int handle_block_status(const struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx,
                               struct net_buf_simple *buf)
{
    struct bt_mesh_blob_cli *cli = mod->user_data;
    struct bt_mesh_blob_target *target;
    struct block_status status = { 0 };
    uint8_t status_and_format;
    uint16_t chunk_size;
    size_t len;
    int idx;

    target = target_get(cli, ctx->addr);
    if (!target) {
        return -ENOENT;
    }

    status_and_format = net_buf_simple_pull_u8(buf);
    status.status = status_and_format & BIT_MASK(4);
    status.missing = status_and_format >> 6;
    status.block.number = net_buf_simple_pull_le16(buf);
    chunk_size = net_buf_simple_pull_le16(buf);
    status.block.chunk_count =
        DIV_ROUND_UP(cli->block.size, chunk_size);

    BT_DBG("status: %u block: %u encoding: %u", status.status,
           status.block.number, status.missing);

    switch (status.missing) {
    case BT_MESH_BLOB_CHUNKS_MISSING_ALL:
        blob_chunk_missing_set_all(&status.block);
        break;
    case BT_MESH_BLOB_CHUNKS_MISSING_NONE:
        break;
    case BT_MESH_BLOB_CHUNKS_MISSING_SOME:
        if (buf->len > sizeof(status.block.missing)) {
            return -EINVAL;
        }

        len = buf->len;
        memcpy(status.block.missing, net_buf_simple_pull_mem(buf, len),
               len);

        BT_DBG("Missing: %s", bt_hex(status.block.missing, len));
        break;
    case BT_MESH_BLOB_CHUNKS_MISSING_ENCODED:
        /** MshMBTv1.0: 5.3.8: An empty Missing Chunks field entails that there are no
         * missing chunks for this block.
         */
        if (!buf->len) {
            status.missing = BT_MESH_BLOB_CHUNKS_MISSING_NONE;
        }

        while (buf->len) {
            idx = chunk_idx_decode(buf);
            if (idx < 0 || idx >= status.block.chunk_count) {
                BT_ERR("Invalid encoding");
                return -EINVAL;
            }

            BT_DBG("Missing %d", idx);

            blob_chunk_missing_set(status.block.missing, idx, true);
        }
        break;
    }

    rx_block_status(cli, target, &status);

    return 0;
}

static int handle_info_status(const struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx,
                              struct net_buf_simple *buf)
{
    struct bt_mesh_blob_cli *cli = mod->user_data;
    struct bt_mesh_blob_cli_caps caps;
    enum bt_mesh_blob_status status;
    struct bt_mesh_blob_target *target;

    if (cli->state != BT_MESH_BLOB_CLI_STATE_CAPS_GET) {
        return -EBUSY;
    }

    caps.min_block_size_log = net_buf_simple_pull_u8(buf);
    caps.max_block_size_log = net_buf_simple_pull_u8(buf);
    caps.max_chunks = net_buf_simple_pull_le16(buf);
    caps.max_chunk_size = net_buf_simple_pull_le16(buf);
    caps.max_size = net_buf_simple_pull_le32(buf);
    caps.mtu_size = net_buf_simple_pull_le16(buf);
    caps.modes = net_buf_simple_pull_u8(buf);

    if (caps.min_block_size_log < 0x06 ||
            caps.max_block_size_log > 0x20 ||
            caps.max_block_size_log < caps.min_block_size_log ||
            caps.max_chunks == 0 || caps.max_chunk_size < 8 ||
            caps.max_size == 0 || caps.mtu_size < 0x14) {
        return -EINVAL;
    }

    BT_DBG("0x%04x\n\tblock size: %u - %u\n\tchunks: %u\n\tchunk size: %u\n"
           "\tblob size: %u\n\tmtu size: %u\n\tmodes: %x",
           ctx->addr, caps.min_block_size_log, caps.max_block_size_log,
           caps.max_chunks, caps.max_chunk_size, caps.max_size,
           caps.mtu_size, caps.modes);

    target = target_get(cli, ctx->addr);
    if (!target) {
        return -ENOENT;
    }

    status = caps_adjust(cli, &caps);
    if (status != BT_MESH_BLOB_SUCCESS) {
        target_drop(cli, target, status);
    }

    blob_cli_broadcast_rsp(cli, target);

    return 0;
}

const struct bt_mesh_model_op _bt_mesh_blob_cli_op[] = {
    { BT_MESH_BLOB_OP_XFER_STATUS, 2, (void *)handle_xfer_status },
    { BT_MESH_BLOB_OP_BLOCK_REPORT, 0, (void *)handle_block_report },
    { BT_MESH_BLOB_OP_BLOCK_STATUS, 5, (void *)handle_block_status },
    { BT_MESH_BLOB_OP_INFO_STATUS, 13, (void *)handle_info_status },
    BLE_MESH_MODEL_OP_END,
};

static int blob_cli_init(struct bt_mesh_model *mod)
{
    struct bt_mesh_blob_cli *cli = mod->user_data;

    cli->mod = mod;

    bt_mesh_blob_cli_set_chunk_interval_ms(cli, CONFIG_BLE_MESH_TX_BLOB_CHUNK_SEND_INTERVAL);
    cli->tx.cli_timestamp = 0ll;
    k_work_init_delayable(&cli->tx.retry, retry_timeout);
    k_work_init_delayable(&cli->tx.complete, tx_complete);

    return 0;
}

static void blob_cli_reset(struct bt_mesh_model *mod)
{
    struct bt_mesh_blob_cli *cli = mod->user_data;

    cli_state_reset(cli);
}

static int blob_cli_deinit(struct bt_mesh_model *mod)
{
    blob_cli_reset(mod);
    return 0;
}

const struct bt_mesh_model_cb _bt_mesh_blob_cli_cb = {
    .init = blob_cli_init,
#if CONFIG_BLE_MESH_DEINIT
    .deinit = blob_cli_deinit,
#endif
};

int bt_mesh_blob_cli_caps_get(struct bt_mesh_blob_cli *cli,
                              const struct bt_mesh_blob_cli_inputs *inputs)
{
    if (bt_mesh_blob_cli_is_busy(cli)) {
        return -EBUSY;
    }

    cli->inputs = inputs;

    cli->caps.min_block_size_log = 0x06;
    cli->caps.max_block_size_log = 0x20;
    cli->caps.max_chunks = CONFIG_BLE_MESH_BLOB_CHUNK_COUNT_MAX;
    cli->caps.max_chunk_size = BLOB_TX_CHUNK_SIZE;
    cli->caps.max_size = 0xffffffff;
    cli->caps.mtu_size = 0xffff;
    cli->caps.modes = BT_MESH_BLOB_XFER_MODE_ALL;

    if (!targets_reset(cli)) {
        BT_ERR("No valid targets");
        return -ENODEV;
    }

    caps_get(cli);

    return 0;
}

int bt_mesh_blob_cli_send(struct bt_mesh_blob_cli *cli,
                          const struct bt_mesh_blob_cli_inputs *inputs,
                          const struct bt_mesh_blob_xfer *xfer,
                          const struct bt_mesh_blob_io *io)
{
    if (bt_mesh_blob_cli_is_busy(cli)) {
        BT_ERR("BLOB Client is busy");
        return -EBUSY;
    }

    if (!(xfer->mode & BT_MESH_BLOB_XFER_MODE_ALL) || xfer->block_size_log < 0x06 ||
            xfer->block_size_log > 0x20 || xfer->chunk_size < 8 ||
            xfer->chunk_size > BLOB_TX_CHUNK_SIZE) {
        BT_ERR("Incompatible transfer parameters");
        return -EINVAL;
    }

    cli->xfer = xfer;
    cli->inputs = inputs;
    cli->io = io;

    if (cli->xfer->block_size_log == 0x20) {
        cli->block_count = 1;
    } else {
        cli->block_count = DIV_ROUND_UP(cli->xfer->size, (1U << cli->xfer->block_size_log));
    }

    block_set(cli, 0);

    if (cli->block.chunk_count > CONFIG_BLE_MESH_BLOB_CHUNK_COUNT_MAX) {
        BT_ERR("Too many chunks");
        return -EINVAL;
    }

    if (!targets_reset(cli)) {
        BT_ERR("No valid targets");
        return -ENODEV;
    }

    BT_INFO("block size log: %u chunk size: %u blob size: %u tmode: %x",
            cli->xfer->block_size_log, cli->xfer->chunk_size,
            cli->xfer->size, cli->xfer->mode);

    return xfer_start(cli);
}

int bt_mesh_blob_cli_suspend(struct bt_mesh_blob_cli *cli)
{
    if (cli->state == BT_MESH_BLOB_CLI_STATE_SUSPENDED) {
        return 0;
    }

    if (cli->state != BT_MESH_BLOB_CLI_STATE_BLOCK_START &&
            cli->state != BT_MESH_BLOB_CLI_STATE_BLOCK_SEND &&
            cli->state != BT_MESH_BLOB_CLI_STATE_BLOCK_CHECK) {
        BT_WARN("BLOB xfer not started: %d", cli->state);
        return -EINVAL;
    }

    /* After the last chunk data is sent, if the server
     * successfully receives the chunk, it will be in the
     * complete state. At this time, if the client resumes
     * from suspend and restarts the transmission, an error
     * will occur, resulting in lost target */
    if (last_chunk_sent) {
        BT_WARN("About to end, refuse to suspend");
        return -EINVAL;
    }

    cli->state = BT_MESH_BLOB_CLI_STATE_SUSPENDED;
    (void)k_work_cancel_delayable(&cli->tx.retry);
    cli->tx.ctx.is_inited = 0;
    cli->tx.sending = 0;
    cli->tx.cli_timestamp = 0ll;
    return 0;
}

int bt_mesh_blob_cli_resume(struct bt_mesh_blob_cli *cli)
{
    struct bt_mesh_blob_target *target;

    if (cli->state != BT_MESH_BLOB_CLI_STATE_SUSPENDED) {
        BT_WARN("Not suspended");
        return -EINVAL;
    }

    /* Restore timed out targets. */
    TARGETS_FOR_EACH(cli, target) {
        if (!!target->timedout) {
            target->status = BT_MESH_BLOB_SUCCESS;
            target->timedout = 0U;
        }
    }

    if (!targets_reset(cli)) {
        BT_ERR("No valid targets");
        return -ENODEV;
    }

    block_set(cli, 0);
    return xfer_start(cli);
}

void bt_mesh_blob_cli_cancel(struct bt_mesh_blob_cli *cli)
{
    if (!bt_mesh_blob_cli_is_busy(cli)) {
        BT_WARN("BLOB xfer already cancelled");
        return;
    }

    BT_DBG("");

    if (cli->state == BT_MESH_BLOB_CLI_STATE_CAPS_GET ||
            cli->state == BT_MESH_BLOB_CLI_STATE_SUSPENDED) {
        cli_state_reset(cli);
        return;
    }

    cli->tx.cancelled = 1U;
    cli->state = BT_MESH_BLOB_CLI_STATE_CANCEL;
}

void bt_mesh_blob_cli_send_cancel(struct bt_mesh_blob_cli *cli,
                                  const struct bt_mesh_blob_xfer *xfer)
{
    cli->xfer = xfer;

    transfer_cancel(cli);
}

int bt_mesh_blob_cli_xfer_progress_get(struct bt_mesh_blob_cli *cli,
                                       const struct bt_mesh_blob_cli_inputs *inputs)
{
    if (bt_mesh_blob_cli_is_busy(cli)) {
        return -EBUSY;
    }

    cli->inputs = inputs;

    check_transfer(cli);

    return 0;
}

uint8_t bt_mesh_blob_cli_xfer_progress_active_get(struct bt_mesh_blob_cli *cli)
{
    if (cli->state < BT_MESH_BLOB_CLI_STATE_START) {
        return 0;
    }

    return (100U * cli->block.number) / cli->block_count;
}

bool bt_mesh_blob_cli_is_busy(struct bt_mesh_blob_cli *cli)
{
    return cli->state != BT_MESH_BLOB_CLI_STATE_NONE;
}

void bt_mesh_blob_cli_set_chunk_interval_ms(struct bt_mesh_blob_cli *cli, uint32_t interval_ms)
{
    cli->chunk_interval_ms = interval_ms;
}

#endif /* CONFIG_BLE_MESH_BLOB_CLI */
