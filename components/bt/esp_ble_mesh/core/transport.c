/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "crypto.h"
#include "tag.h"
#include "adv.h"
#include "mesh.h"
#include "lpn.h"
#include "rpl.h"
#include "friend.h"
#include "access.h"
#include "foundation.h"
#include "settings.h"
#include "transport.h"
#include "mesh/main.h"
#include "fast_prov.h"
#include "mesh/common.h"
#include "mesh/cfg_srv.h"
#include "heartbeat.h"

#if CONFIG_BLE_MESH_V11_SUPPORT
#include "mesh_v1.1/utils.h"
#endif /* CONFIG_BLE_MESH_V11_SUPPORT */

/* The transport layer needs at least three buffers for itself to avoid
 * deadlocks. Ensure that there are a sufficient number of advertising
 * buffers available compared to the maximum supported outgoing segment
 * count.
 */
_Static_assert(CONFIG_BLE_MESH_ADV_BUF_COUNT >= (CONFIG_BLE_MESH_TX_SEG_MAX + 3),
               "Too small BLE Mesh adv buffer count");

#define AID_MASK                    ((uint8_t)(BIT_MASK(6)))

#define SEG(data)                   ((data)[0] >> 7)
#define AKF(data)                   (((data)[0] >> 6) & 0x01)
#define AID(data)                   ((data)[0] & AID_MASK)
#define ASZMIC(data)                (((data)[1] >> 7) & 1)

#define APP_MIC_LEN(aszmic)         ((aszmic) ? BLE_MESH_MIC_LONG : BLE_MESH_MIC_SHORT)

#define UNSEG_HDR(akf, aid)         ((akf << 6) | (aid & AID_MASK))
#define SEG_HDR(akf, aid)           (UNSEG_HDR(akf, aid) | 0x80)

#define BLOCK_COMPLETE(seg_n)       (uint32_t)(((uint64_t)1 << (seg_n + 1)) - 1)

#define SEQ_AUTH(iv_index, seq)     (((uint64_t)iv_index) << 24 | (uint64_t)seq)

/* Number of retransmit attempts (after the initial transmit) per segment */
#define SEG_RETRANSMIT_ATTEMPTS     4

/* How long to wait for available buffers before giving up */
#define BUF_TIMEOUT                 K_NO_WAIT

/* "This timer shall be set to a minimum of 200 + 50 * TTL milliseconds.".
 * We use 400 since 300 is a common send duration for standard HCI, and we
 * need to have a timeout that's bigger than that.
 */
#define SEG_RETRANSMIT_TIMEOUT_UNICAST(tx)  (K_MSEC(400) + 50 * (tx)->ttl)

/* When sending to a group, the messages are not acknowledged, and there's no
 * reason to delay the repetitions significantly. Delaying by more than 0 ms
 * to avoid flooding the network.
 */
#define SEG_RETRANSMIT_TIMEOUT_GROUP        K_MSEC(50)

#define SEG_RETRANSMIT_TIMEOUT(tx)          (BLE_MESH_ADDR_IS_UNICAST((tx)->dst) ? \
                                             SEG_RETRANSMIT_TIMEOUT_UNICAST(tx) : \
                                             SEG_RETRANSMIT_TIMEOUT_GROUP)

struct seg_info {
    uint8_t     ctl:1;         /* Control message */
#if CONFIG_BLE_MESH_LONG_PACKET
    uint8_t     long_pkt:1;    /* Long packet */
#endif
};

static struct seg_tx {
    struct bt_mesh_subnet *sub;
    // @todo: select the max one between large seg max and normal seg max
    struct net_buf        *seg[CONFIG_BLE_MESH_TX_SEG_MAX];
    uint64_t               seq_auth;
    uint16_t               dst;
    uint8_t                xmit;        /* Segment transmit */
    uint8_t                seg_n:5,     /* Last segment index */
                           new_key:1;   /* New/old key */
    uint8_t                nack_count;  /* Number of unacked segs */
    uint8_t                ttl;
    uint8_t                seg_pending; /* Number of segments pending */
    uint8_t                attempts;    /* Transmit attempts */
    uint8_t                cred;        /* Security credentials */
    uint8_t                tag;         /* Additional metadata */
    const struct bt_mesh_send_cb *cb;
    void                  *cb_data;
    struct k_delayed_work  rtx_timer;   /* Segment Retransmission timer */
    bt_mesh_mutex_t        lock;
} seg_tx[CONFIG_BLE_MESH_TX_SEG_MSG_COUNT];

static struct seg_rx {
    struct bt_mesh_subnet *sub;
    uint64_t               seq_auth;
    uint16_t               seg_n:5,
                           ctl:1,
                           in_use:1,
                           obo:1,
                           ext:1;
    uint8_t                hdr;
    uint8_t                ttl;
    uint16_t               src;
    uint16_t               dst;
    uint32_t               block;
    uint32_t               last;
    struct k_delayed_work  ack_timer;
    struct net_buf_simple  buf;
} seg_rx[CONFIG_BLE_MESH_RX_SEG_MSG_COUNT];

static uint8_t seg_rx_buf_data[(CONFIG_BLE_MESH_RX_SEG_MSG_COUNT *
                                CONFIG_BLE_MESH_RX_SDU_MAX)];
#if CONFIG_BLE_MESH_LONG_PACKET
/* @todo: could merge ext_seg_rx into seg_rx */
static struct seg_rx ext_seg_rx[CONFIG_BLE_MESH_LONG_PACKET_RX_SEG_CNT];
static uint8_t ext_seg_rx_buf_data[(CONFIG_BLE_MESH_LONG_PACKET_RX_SEG_CNT *
                                    BLE_MESH_EXT_RX_SDU_MAX)];
#endif

static struct seg_rx *seg_rx_alloc(struct bt_mesh_net_rx *net_rx,
    const uint8_t *hdr, const uint64_t *seq_auth,
    uint8_t seg_n);

static bt_mesh_mutex_t seg_rx_lock;

static inline void bt_mesh_seg_tx_lock(struct seg_tx *tx)
{
    bt_mesh_r_mutex_lock(&tx->lock);
}

static inline void bt_mesh_seg_tx_unlock(struct seg_tx *tx)
{
    bt_mesh_r_mutex_unlock(&tx->lock);
}

static inline void bt_mesh_seg_rx_lock(void)
{
    bt_mesh_r_mutex_lock(&seg_rx_lock);
}

static inline void bt_mesh_seg_rx_unlock(void)
{
    bt_mesh_r_mutex_unlock(&seg_rx_lock);
}

uint8_t bt_mesh_get_seg_rtx_num(void)
{
    BT_DBG("SegRTXNum %u", SEG_RETRANSMIT_ATTEMPTS);

    return SEG_RETRANSMIT_ATTEMPTS;
}

int32_t bt_mesh_get_seg_rtx_timeout(uint16_t dst, uint8_t ttl)
{
    /* This function will be used when a client model sending an
     * acknowledged message. And if the dst of a message is not
     * a unicast address, the function will not be invoked.
     * So we can directly use the SEG_RETRANSMIT_TIMEOUT_UNICAST
     * macro here.
     */
    struct seg_tx tx = {
        .ttl = ttl,
    };

    BT_DBG("SegRTXTimeout, TTL %u", ttl);

    return SEG_RETRANSMIT_TIMEOUT_UNICAST(&tx);
}

static int send_unseg(struct bt_mesh_net_tx *tx, struct net_buf_simple *sdu,
                      const struct bt_mesh_send_cb *cb, void *cb_data,
                      const uint8_t *ctl_op)
{
    struct net_buf *buf = NULL;

    enum bt_mesh_adv_type adv_type = BLE_MESH_ADV_DATA;
    BT_DBG("SendUnseg");
    BT_DBG("Src 0x%04x Dst 0x%04x AppIdx 0x%04x CtlOp 0x%02x SduLen %u",
           tx->src, tx->ctx->addr, tx->ctx->app_idx,
           ctl_op ? *ctl_op : 0xFF, sdu->len);

#if CONFIG_BLE_MESH_EXT_ADV
#if CONFIG_BLE_MESH_LONG_PACKET
    if (tx->ctx->enh.long_pkt_cfg_used &&
        (tx->ctx->enh.long_pkt_cfg == BLE_MESH_LONG_PACKET_FORCE ||
        (tx->ctx->enh.long_pkt_cfg == BLE_MESH_LONG_PACKET_PREFER &&
        sdu->len > (BLE_MESH_SDU_UNSEG_MAX + 4)))) {
        adv_type = BLE_MESH_ADV_EXT_LONG_DATA;
    } else
#endif /* CONFIG_BLE_MESH_LONG_PACKET */
    {
        if (tx->ctx->enh.ext_adv_cfg_used) {
            adv_type = BLE_MESH_ADV_EXT_DATA;
        }
    }
#endif /* CONFIG_BLE_MESH_EXT_ADV */

    buf = bt_mesh_adv_create(adv_type, BUF_TIMEOUT);
    if (!buf) {
        BT_ERR("Out of network buffers");
        return -ENOBUFS;
    }

    if (tx->ctx->enh.adv_cfg_used) {
        BLE_MESH_ADV(buf)->channel_map = tx->ctx->enh.adv_cfg.channel_map;
        BLE_MESH_ADV(buf)->adv_cnt = tx->ctx->enh.adv_cfg.adv_cnt;
        BLE_MESH_ADV(buf)->adv_itvl = tx->ctx->enh.adv_cfg.adv_itvl;
    }

#if CONFIG_BLE_MESH_EXT_ADV
    if (tx->ctx->enh.ext_adv_cfg_used) {
        EXT_ADV(buf)->primary_phy = tx->ctx->enh.ext_adv_cfg.primary_phy;
        EXT_ADV(buf)->secondary_phy = tx->ctx->enh.ext_adv_cfg.secondary_phy;
        EXT_ADV(buf)->include_tx_power = tx->ctx->enh.ext_adv_cfg.include_tx_power;
        EXT_ADV(buf)->tx_power = tx->ctx->enh.ext_adv_cfg.tx_power;
    }
#endif

    net_buf_reserve(buf, BLE_MESH_NET_HDR_LEN);

    if (ctl_op) {
        net_buf_add_u8(buf, TRANS_CTL_HDR(*ctl_op, 0));
    } else if (tx->ctx->app_idx == BLE_MESH_KEY_DEV) {
        net_buf_add_u8(buf, UNSEG_HDR(0, 0));
    } else {
        net_buf_add_u8(buf, UNSEG_HDR(1, tx->aid));
    }

    net_buf_add_mem(buf, sdu->data, sdu->len);

    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND)) {
        if (!bt_mesh_friend_queue_has_space(tx->sub->net_idx,
                                            tx->src, tx->ctx->addr,
                                            NULL, 1)) {
            BT_WARN("NoSpaceInFrndQueue, SegCount 1");

            if (BLE_MESH_ADDR_IS_UNICAST(tx->ctx->addr)) {
                BT_ERR("NotSentToUnicast");
                net_buf_unref(buf);
                return -ENOBUFS;
            }

            goto send;
        }

        if (bt_mesh_friend_enqueue_tx(tx, BLE_MESH_FRIEND_PDU_SINGLE,
                                      NULL, 1, &buf->b) &&
            BLE_MESH_ADDR_IS_UNICAST(tx->ctx->addr)) {
            /* PDUs for a specific Friend should only go
             * out through the Friend Queue.
             */
            BT_DBG("FrndTxEnqueued, SegCount 1");

            net_buf_unref(buf);
            send_cb_finalize(cb, cb_data);
            return 0;
        }
    }

send:
    return bt_mesh_net_send(tx, buf, cb, cb_data);
}

static inline uint8_t seg_len(struct seg_info *si)
{
    uint8_t len = 0;

    if (si->ctl) {
        len =
#if CONFIG_BLE_MESH_LONG_PACKET
            si->long_pkt ? BLE_MESH_EXT_CTL_SEG_SDU_MAX :
#endif
                           BLE_MESH_CTL_SEG_SDU_MAX;
    } else {
        len =
#if CONFIG_BLE_MESH_LONG_PACKET
            si->long_pkt ? BLE_MESH_EXT_APP_SEG_SDU_MAX :
#endif
                        BLE_MESH_APP_SEG_SDU_MAX;
    }

    BT_DBG("SegLen %u", len);

    return len;
}

bool bt_mesh_tx_in_progress(void)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(seg_tx); i++) {
        if (seg_tx[i].nack_count) {
            BT_DBG("SegTxInProgress");
            return true;
        }
    }

    BT_DBG("SegTxNotInProgress");
    return false;
}

static void seg_tx_done(struct seg_tx *tx, uint8_t seg_idx)
{
    BT_DBG("SegTxDone, SegIdx %u", seg_idx);
    /* If the data is sent to itself, then last seg's ref count be equal to 3 */
    if (((IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_provisioned()) ||
         (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) && bt_mesh_is_provisioner_en())) &&
        (bt_mesh_fixed_group_match(tx->dst) || bt_mesh_elem_find(tx->dst))) {
        bt_mesh_adv_buf_ref_debug(__func__, tx->seg[seg_idx], 4U, BLE_MESH_BUF_REF_SMALL);
    } else {
        bt_mesh_adv_buf_ref_debug(__func__, tx->seg[seg_idx], 3U, BLE_MESH_BUF_REF_SMALL);
    }

    /* When cancelling a segment that is still in the adv sending queue,
     * the `tx->seg_pending` must be decremented by one.
     * More details could be found in BLEMESH24-26.
     */
    if (bt_mesh_atomic_cas(&BLE_MESH_ADV_BUSY(tx->seg[seg_idx]), 1, 0)) {
        BT_DBG("SegPendingDec %u", tx->seg_pending);
        tx->seg_pending--;
    }

    BT_DBG("NackCountDec %u", tx->nack_count);

    net_buf_unref(tx->seg[seg_idx]);
    tx->seg[seg_idx] = NULL;
    tx->nack_count--;
}

static void seg_tx_reset(struct seg_tx *tx)
{
    uint8_t seg_n = 0U;
    int i;

    BT_DBG("SegTxReset");

    bt_mesh_seg_tx_lock(tx);

    k_delayed_work_cancel(&tx->rtx_timer);

    tx->cb = NULL;
    tx->cb_data = NULL;
    tx->seq_auth = 0U;
    tx->sub = NULL;
    tx->dst = BLE_MESH_ADDR_UNASSIGNED;

    seg_n = MIN(tx->seg_n, ARRAY_SIZE(tx->seg) - 1);
    for (i = 0; i <= seg_n && tx->nack_count; i++) {
        if (!tx->seg[i]) {
            continue;
        }

        seg_tx_done(tx, i);
    }

    tx->nack_count = 0U;

    bt_mesh_seg_tx_unlock(tx);

    if (bt_mesh_atomic_test_and_clear_bit(bt_mesh.flags, BLE_MESH_IVU_PENDING)) {
        BT_DBG("Proceeding with pending IV Update");

        /* bt_mesh_net_iv_update() will re-enable the flag if this
         * wasn't the only transfer.
         */
        if (bt_mesh_net_iv_update(bt_mesh.iv_index, false)) {
            bt_mesh_net_sec_update(NULL);
        }
    }
}

static inline void seg_tx_complete(struct seg_tx *tx, int err)
{
    const struct bt_mesh_send_cb *cb = tx->cb;
    void *cb_data = tx->cb_data;

    BT_DBG("SegTxComplete, Err %d", err);

    seg_tx_reset(tx);

    /* TODO: notify the completion of sending segmented message */

    if (cb && cb->end) {
        cb->end(err, cb_data);
    }
}

static void schedule_retransmit(struct seg_tx *tx)
{
    /* It's possible that a segment broadcast hasn't finished, but the tx
     * has already been released. Only the seg_pending of this segment
     * remains unprocessed.
     * So, here we determine if the tx are released by checking if the
     * destination (dst) is unassigned, and then process the seg_pending
     * of this segment.
     * See BLEMESH25-92 for details */

    BT_DBG("ScheduleRetransmit, Dst 0x%04x", tx->dst);

    bt_mesh_seg_tx_lock(tx);

    if (tx->dst == BLE_MESH_ADDR_UNASSIGNED) {
        BT_DBG("SegPending %u", tx->seg_pending);

        if (tx->seg_pending) {
            tx->seg_pending--;
        }
        goto end;
    }

    BT_DBG("SegPending %u Attempts %u", tx->seg_pending, tx->attempts);

    if (--tx->seg_pending) {
        goto end;
    }

    if (!BLE_MESH_ADDR_IS_UNICAST(tx->dst) && !tx->attempts) {
        BT_INFO("TxSduToGroupDone");

        seg_tx_complete(tx, 0);
        goto end;
    }

    k_delayed_work_submit(&tx->rtx_timer, SEG_RETRANSMIT_TIMEOUT(tx));

end:
    bt_mesh_seg_tx_unlock(tx);
}

static void seg_first_send_start(uint16_t duration, int err, void *user_data)
{
    struct seg_tx *tx = user_data;

    BT_DBG("SegFirstSendStart, Err %d", err);

    if (tx->cb && tx->cb->start) {
        tx->cb->start(duration, err, tx->cb_data);
    }
}

static void seg_send_start(uint16_t duration, int err, void *user_data)
{
    struct seg_tx *tx = user_data;

    BT_DBG("SegSendStart, Err %d", err);

    /* If there's an error in transmitting the 'sent' callback will never
     * be called. Make sure that we kick the retransmit timer also in this
     * case since otherwise we risk the transmission of becoming stale.
     */
    if (err) {
        schedule_retransmit(tx);
    }
}

static void seg_sent(int err, void *user_data)
{
    struct seg_tx *tx = user_data;

    BT_DBG("SegSent, Err %d", err);

    schedule_retransmit(tx);
}

static const struct bt_mesh_send_cb first_sent_cb = {
    .start = seg_first_send_start,
    .end = seg_sent,
};

static const struct bt_mesh_send_cb seg_sent_cb = {
    .start = seg_send_start,
    .end = seg_sent,
};

static void seg_tx_send_unacked(struct seg_tx *tx)
{
    int i, err = 0;

    BT_DBG("SegTxSendUnacked, Attempts %u", tx->attempts);

    bt_mesh_seg_tx_lock(tx);

    if (!(tx->attempts--)) {
        BT_WARN("Ran out of retransmit attempts");

        bt_mesh_seg_tx_unlock(tx);

        seg_tx_complete(tx, -ETIMEDOUT);
        return;
    }

    for (i = 0; i <= tx->seg_n; i++) {
        struct net_buf *seg = tx->seg[i];

        if (!seg) {
            continue;
        }

        if (bt_mesh_atomic_get(&BLE_MESH_ADV_BUSY(seg))) {
            BT_DBG("Skipping segment that's still advertising");
            continue;
        }

        BT_INFO("SegPendingInc %u", tx->seg_pending);
        BT_INFO("SegResend %u/%u Cred %u", i, tx->seg_n, tx->cred);

        tx->seg_pending++;

        /* TODO:
         * tx->new_key should be replaced with sub->kr_flag,
         * since there is a chance that the key is refreshed
         * during the retransmission of segments.
         */
        err = bt_mesh_net_resend(tx->sub, seg, tx->new_key,
                                 &tx->cred, tx->tag,
                                 &seg_sent_cb, tx);
        if (err) {
            BT_ERR("ResendSegFailed, Err %d", err);

            bt_mesh_seg_tx_unlock(tx);

            seg_tx_complete(tx, -EIO);
            return;
        }
    }

    bt_mesh_seg_tx_unlock(tx);
}

static void seg_retransmit(struct k_work *work)
{
    struct seg_tx *tx = CONTAINER_OF(work, struct seg_tx, rtx_timer);

    BT_DBG("SegRetransmit");

    seg_tx_send_unacked(tx);
}

static int send_seg(struct bt_mesh_net_tx *net_tx, struct net_buf_simple *sdu,
                    const struct bt_mesh_send_cb *cb, void *cb_data,
                    const uint8_t *ctl_op)
{
    enum bt_mesh_adv_type adv_type = BLE_MESH_ADV_DATA;
    struct seg_tx *tx = NULL;
    struct seg_info si = {0};
    uint16_t seq_zero = 0U;
    uint8_t seg_hdr = 0U;
    uint8_t seg_o = 0U;
    int err = 0;
    int i;

    BT_DBG("SendSeg");
    BT_DBG("Src 0x%04x Dst 0x%04x AppIdx 0x%04x Aszmic %u SduLen %u",
           net_tx->src, net_tx->ctx->addr, net_tx->ctx->app_idx,
           net_tx->aszmic, sdu->len);

    for (tx = NULL, i = 0; i < ARRAY_SIZE(seg_tx); i++) {
        /* In some critical conditions, the tx might be reset before
         * a segment transmission is finished.
         * If this happens, the seg_pending of the segment will not
         * be processed.
         * And to avoid assigning this uncleared tx to a new message,
         * extra checks for seg_pending being 0 are added.
         * See BLEMESH25-92 for details.
         */
        BT_DBG("Seg%u: NackCount %u SegPending %u",
               i, seg_tx[i].nack_count, seg_tx[i].seg_pending);

        if (!seg_tx[i].nack_count && !seg_tx[i].seg_pending) {
            tx = &seg_tx[i];
            break;
        }
    }

    if (!tx) {
        BT_ERR("No multi-segment message contexts available");
        return -EBUSY;
    }

    if (ctl_op) {
        si.ctl = 1;
        seg_hdr = TRANS_CTL_HDR(*ctl_op, 1);
    } else if (net_tx->ctx->app_idx == BLE_MESH_KEY_DEV) {
        si.ctl = 0;
        seg_hdr = SEG_HDR(0, 0);
    } else {
        si.ctl = 0;
        seg_hdr = SEG_HDR(1, net_tx->aid);
    }

#if CONFIG_BLE_MESH_LONG_PACKET
    if (net_tx->ctx->enh.long_pkt_cfg_used &&
        (net_tx->ctx->enh.long_pkt_cfg == BLE_MESH_LONG_PACKET_FORCE ||
        (net_tx->ctx->enh.long_pkt_cfg == BLE_MESH_LONG_PACKET_PREFER &&
        sdu->len > BLE_MESH_TX_SDU_MAX))) {
        si.long_pkt = 1;
    }
#endif

    tx->dst = net_tx->ctx->addr;
    if (sdu->len) {
        tx->seg_n = (sdu->len - 1) / seg_len(&si);
    } else {
        tx->seg_n = 0;
    }
    if (tx->seg_n >= ARRAY_SIZE(tx->seg)) {
        BT_ERR("TooLargeSegNReceived %u/%u", tx->seg_n, ARRAY_SIZE(tx->seg));
        seg_tx_reset(tx);
        return -EMSGSIZE;
    }
    tx->nack_count = tx->seg_n + 1;
    tx->seq_auth = SEQ_AUTH(BLE_MESH_NET_IVI_TX, bt_mesh.seq);
    tx->sub = net_tx->sub;
    tx->new_key = net_tx->sub->kr_flag;
    tx->attempts = SEG_RETRANSMIT_ATTEMPTS;
    tx->seg_pending = 0;
    tx->cred = net_tx->ctx->send_cred;
    tx->tag = net_tx->ctx->send_tag;
    tx->cb = cb;
    tx->cb_data = cb_data;

    if (net_tx->ctx->send_ttl == BLE_MESH_TTL_DEFAULT) {
        tx->ttl = bt_mesh_default_ttl_get();
    } else {
        tx->ttl = net_tx->ctx->send_ttl;
    }

    seq_zero = tx->seq_auth & TRANS_SEQ_ZERO_MASK;

    BT_DBG("SegHdr 0x%02x SegN %u NackCount %u NewKey %u TTL %u SeqZero 0x%04x",
           seg_hdr, tx->seg_n, tx->nack_count, tx->new_key, tx->ttl, seq_zero);

    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND) &&
        !bt_mesh_friend_queue_has_space(tx->sub->net_idx, net_tx->src,
                                        tx->dst, &tx->seq_auth,
                                        tx->seg_n + 1) &&
        BLE_MESH_ADDR_IS_UNICAST(tx->dst)) {
        BT_WARN("NoSpaceInFrndQueue, SegCount %u", tx->seg_n + 1);

        seg_tx_reset(tx);
        return -ENOBUFS;
    }

    bt_mesh_seg_tx_lock(tx);

    for (seg_o = 0U; sdu->len; seg_o++) {
        struct net_buf *seg = NULL;
        uint16_t len = 0U;
#if CONFIG_BLE_MESH_LONG_PACKET
        if (si.long_pkt) {
            adv_type = BLE_MESH_ADV_EXT_LONG_DATA;
        } else
#endif
        {
#if CONFIG_BLE_MESH_EXT_ADV
            if (net_tx->ctx->enh.ext_adv_cfg_used) {
                adv_type = BLE_MESH_ADV_EXT_DATA;
            } else
#endif
            {
                adv_type = BLE_MESH_ADV_DATA;
            }
        }
        seg = bt_mesh_adv_create(adv_type, BUF_TIMEOUT);
        if (!seg) {
            BT_ERR("Out of segment buffers");
            err = -ENOBUFS;
            break;
        }

        if (net_tx->ctx->enh.adv_cfg_used) {
            BLE_MESH_ADV(seg)->channel_map = net_tx->ctx->enh.adv_cfg.channel_map;
            BLE_MESH_ADV(seg)->adv_cnt = net_tx->ctx->enh.adv_cfg.adv_cnt;
            BLE_MESH_ADV(seg)->adv_itvl = net_tx->ctx->enh.adv_cfg.adv_itvl;
        }

#if CONFIG_BLE_MESH_EXT_ADV
        if (net_tx->ctx->enh.ext_adv_cfg_used) {
            EXT_ADV(seg)->primary_phy = net_tx->ctx->enh.ext_adv_cfg.primary_phy;
            EXT_ADV(seg)->secondary_phy = net_tx->ctx->enh.ext_adv_cfg.secondary_phy;
            EXT_ADV(seg)->include_tx_power = net_tx->ctx->enh.ext_adv_cfg.include_tx_power;
            EXT_ADV(seg)->tx_power = net_tx->ctx->enh.ext_adv_cfg.tx_power;
        }
#endif /* CONFIG_BLE_MESH_EXT_ADV */

        net_buf_reserve(seg, BLE_MESH_NET_HDR_LEN);

        net_buf_add_u8(seg, seg_hdr);
        net_buf_add_u8(seg, (net_tx->aszmic << 7) | seq_zero >> 6);
        net_buf_add_u8(seg, (((seq_zero & 0x3f) << 2) | (seg_o >> 3)));
        net_buf_add_u8(seg, ((seg_o & 0x07) << 5) | tx->seg_n);

        len = MIN(sdu->len, seg_len(&si));
        net_buf_add_mem(seg, net_buf_simple_pull_mem(sdu, len), len);

        if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND)) {
            enum bt_mesh_friend_pdu_type type = BLE_MESH_FRIEND_PDU_PARTIAL;

            if (seg_o == tx->seg_n) {
                type = BLE_MESH_FRIEND_PDU_COMPLETE;
            } else {
                type = BLE_MESH_FRIEND_PDU_PARTIAL;
            }

            BT_DBG("FrndPDUType %u", type);

            if (bt_mesh_friend_enqueue_tx(net_tx, type,
                                          &tx->seq_auth,
                                          tx->seg_n + 1,
                                          &seg->b) &&
                BLE_MESH_ADDR_IS_UNICAST(net_tx->ctx->addr)) {
                /* PDUs for a specific Friend should only go
                 * out through the Friend Queue.
                 */
                BT_DBG("FrndTxEnqueued, SegCount %u", tx->seg_n + 1);

                net_buf_unref(seg);
                continue;
            }
        }

        tx->seg[seg_o] = net_buf_ref(seg);

        BT_INFO("SegPendingInc %u", tx->seg_pending);
        BT_INFO("SegSend %u/%u Cred %u", seg_o, tx->seg_n, tx->cred);

        tx->seg_pending++;

        err = bt_mesh_net_send(net_tx, seg,
                               seg_o ? &seg_sent_cb : &first_sent_cb,
                               tx);
        if (err) {
            BT_ERR("SendSegFailed, Err %d", err);
            break;
        }

        /* If security credentials is updated in the network layer,
         * need to store the security credentials for the segments,
         * which will be used for retransmission later.
         */
        if (tx->cred != net_tx->ctx->send_cred) {
            BT_INFO("OldCred %u NewCred %u", tx->cred, net_tx->ctx->send_cred);
            tx->cred = net_tx->ctx->send_cred;
        }
    }

    bt_mesh_seg_tx_unlock(tx);

    if (err) {
        seg_tx_reset(tx);
        return err;
    }

    /* This can happen if segments only went into the Friend Queue */
    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND) && !tx->seg[0]) {
        BT_DBG("OnlyToFrndQueue");

        seg_tx_reset(tx);

        /* If there was a callback notify sending immediately since
         * there's no other way to track this (at least currently)
         * with the Friend Queue.
         */
        send_cb_finalize(cb, cb_data);
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER) &&
        bt_mesh_lpn_established()) {
        bt_mesh_lpn_poll();
    }

    return 0;
}

int bt_mesh_trans_send(struct bt_mesh_net_tx *tx, struct net_buf_simple *msg,
                       const struct bt_mesh_send_cb *cb, void *cb_data)
{
    const uint8_t *key = NULL;
    uint8_t *ad = NULL;
    uint8_t aid = 0U;
    int err = 0;

    BT_DBG("transcend");

    if (msg->len < 1) {
        BT_ERR("Zero-length SDU not allowed");
        return -EINVAL;
    }

#if CONFIG_BLE_MESH_LONG_PACKET
    if (tx->ctx->enh.long_pkt_cfg_used == true) {
        if (tx->ctx->enh.long_pkt_cfg == BLE_MESH_LONG_PACKET_FORCE &&
            msg->len > BLE_MESH_EXT_SDU_UNSEG_MAX) {
            tx->ctx->send_tag |= BLE_MESH_TAG_SEND_SEGMENTED;
        } else if (tx->ctx->enh.long_pkt_cfg == BLE_MESH_LONG_PACKET_PREFER &&
            msg->len > BLE_MESH_SDU_UNSEG_MAX) {
            tx->ctx->send_tag |= BLE_MESH_TAG_SEND_SEGMENTED;
        }
    } else {
        if (msg->len > BLE_MESH_SDU_UNSEG_MAX) {
            tx->ctx->send_tag |= BLE_MESH_TAG_SEND_SEGMENTED;
        }
    }
#else
    if (msg->len > BLE_MESH_SDU_UNSEG_MAX) {
        tx->ctx->send_tag |= BLE_MESH_TAG_SEND_SEGMENTED;
    }
#endif

    BT_DBG("NetIdx 0x%04x AppIdx 0x%04x Dst 0x%04x",
           tx->sub->net_idx, tx->ctx->app_idx, tx->ctx->addr);
    BT_DBG("Len %u: %s", msg->len, bt_hex(msg->data, msg->len));

    err = bt_mesh_upper_key_get(tx->sub, tx->ctx->app_idx, &key,
                                &aid, tx->ctx->addr);
    if (err) {
        return err;
    }

    tx->aid = aid;

    if (!bt_mesh_tag_send_segmented(tx->ctx->send_tag) ||
        tx->ctx->send_szmic == BLE_MESH_SEG_SZMIC_SHORT ||
        net_buf_simple_tailroom(msg) < BLE_MESH_MIC_LONG) {
        tx->aszmic = 0U;
    } else {
        tx->aszmic = 1U;
    }

    BT_INFO("%s, Tag 0x%02x Szmic %u Aszmic %u",
            bt_mesh_tag_send_segmented(tx->ctx->send_tag) ? "Seg" : "Unseg",
            tx->ctx->send_tag, tx->ctx->send_szmic, tx->aszmic);

    if (BLE_MESH_ADDR_IS_VIRTUAL(tx->ctx->addr)) {
        ad = bt_mesh_label_uuid_get(tx->ctx->addr);
    } else {
        ad = NULL;
    }

    err = bt_mesh_app_encrypt(key, tx->ctx->app_idx == BLE_MESH_KEY_DEV,
                              tx->aszmic, msg, ad, tx->src,
                              tx->ctx->addr, bt_mesh.seq,
                              BLE_MESH_NET_IVI_TX);
    if (err) {
        BT_ERR("AppEncryptFailed, Err %d", err);
        return err;
    }

    if (bt_mesh_tag_send_segmented(tx->ctx->send_tag)) {
        return send_seg(tx, msg, cb, cb_data, NULL);
    }

    return send_unseg(tx, msg, cb, cb_data, NULL);
}

static void revoke_dev_key(const uint8_t *dev_key)
{
    if (!memcmp(dev_key, bt_mesh.dev_key_ca, 16)) {
        BT_INFO("RevokeDevKey");

        memcpy(bt_mesh.dev_key, bt_mesh.dev_key_ca, 16);
        memset(bt_mesh.dev_key_ca, 0, 16);

        if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
            bt_mesh_store_net();
            bt_mesh_clear_dkca();
        }
    }
}

static int sdu_recv(struct bt_mesh_net_rx *rx, uint32_t seq, uint8_t hdr,
                    uint8_t aszmic, struct net_buf_simple *buf)
{
    struct net_buf_simple *sdu = NULL;
    size_t array_size = 0U;
    uint8_t *ad = NULL;
    size_t i = 0U;
    int err = 0;

    BT_DBG("SduRecv");
    BT_DBG("Aszmic %u AKF %u AID 0x%02x", aszmic, AKF(&hdr), AID(&hdr));
    BT_DBG("Len %u: %s", buf->len, bt_hex(buf->data, buf->len));

    if (buf->len < 1 + APP_MIC_LEN(aszmic)) {
        BT_ERR("Too short SDU + MIC (len %u)", buf->len);
        return -EINVAL;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND) && !rx->local_match) {
        BT_DBG("Ignoring PDU for LPN 0x%04x of this Friend",
               rx->ctx.recv_dst);
        return 0;
    }

    if (BLE_MESH_ADDR_IS_VIRTUAL(rx->ctx.recv_dst)) {
        ad = bt_mesh_label_uuid_get(rx->ctx.recv_dst);
    } else {
        ad = NULL;
    }

    /* Adjust the length to not contain the MIC at the end */
    buf->len -= APP_MIC_LEN(aszmic);

    /* Use bt_mesh_alloc_buf() instead of NET_BUF_SIMPLE_DEFINE to avoid
     * causing btu task stack overflow.
     */
    sdu = bt_mesh_alloc_buf(buf->len);
    if (!sdu) {
        BT_ERR("%s, Out of memory", __func__);
        return -ENOMEM;
    }

    if (!AKF(&hdr)) {
        array_size = bt_mesh_rx_devkey_size();

        for (i = 0U; i < array_size; i++) {
            const uint8_t *dev_key = NULL;

            dev_key = bt_mesh_rx_devkey_get(i, rx->ctx.addr);
            if (!dev_key) {
                BT_DBG("DevKeyNotFound");
                continue;
            }

            net_buf_simple_reset(sdu);

            err = bt_mesh_app_decrypt(dev_key, true, aszmic, buf,
                                      sdu, ad, rx->ctx.addr,
                                      rx->ctx.recv_dst, seq,
                                      BLE_MESH_NET_IVI_RX(rx));
            if (err) {
                BT_DBG("DevKeyNotDecrypt");
                continue;
            }

            BT_BQB(BLE_MESH_BQB_TEST_LOG_LEVEL_PRIMARY_ID_NODE | \
                   BLE_MESH_BQB_TEST_LOG_LEVEL_SUB_ID_TNPT,
                   "\nTNPTRecv: ctl: 0x%04x, ttl: 0x%04x, src: 0x%04x, dst: 0x%04x, payload: 0x%s",
                   rx->ctl, rx->ctx.recv_ttl, rx->ctx.addr, rx->ctx.recv_dst,
                   bt_hex(sdu->data, sdu->len));

            /* When the Device Key Candidate is available, and an access message
             * is decrypted using the Device Key Candidate that was delivered to
             * the access layer, then the node shall revoke the device key, the
             * Device Key Candidate shall become the device key, and the Device
             * Key Candidate shall become unavailable.
             */
            revoke_dev_key(dev_key);

            rx->ctx.app_idx = BLE_MESH_KEY_DEV;
            bt_mesh_model_recv(rx, sdu);

            bt_mesh_free_buf(sdu);
            return 0;
        }

        BT_WARN("Unable to decrypt with DevKey");

        bt_mesh_free_buf(sdu);
        return -ENODEV;
    }

    array_size = bt_mesh_rx_appkey_size();

    for (i = 0U; i < array_size; i++) {
        struct bt_mesh_app_keys *keys = NULL;
        struct bt_mesh_app_key *key = NULL;

        key = bt_mesh_rx_appkey_get(i);
        if (!key) {
            BT_DBG("AppKeyNotFound");
            continue;
        }

        /* Make sure that this AppKey matches received net_idx */
        if (key->net_idx != rx->sub->net_idx) {
            continue;
        }

        if (rx->new_key && key->updated) {
            keys = &key->keys[1];
        } else {
            keys = &key->keys[0];
        }

        /* Check that the AppKey ID matches */
        if (AID(&hdr) != keys->id) {
            continue;
        }

        net_buf_simple_reset(sdu);

        err = bt_mesh_app_decrypt(keys->val, false, aszmic, buf,
                                  sdu, ad, rx->ctx.addr,
                                  rx->ctx.recv_dst, seq,
                                  BLE_MESH_NET_IVI_RX(rx));

        BT_BQB(BLE_MESH_BQB_TEST_LOG_LEVEL_PRIMARY_ID_NODE | \
               BLE_MESH_BQB_TEST_LOG_LEVEL_SUB_ID_TNPT,
               "\nTNPTRecv: ctl: 0x%04x, ttl: 0x%04x, src: 0x%04x, dst: 0x%04x, payload: 0x%s",
               rx->ctl, rx->ctx.recv_ttl, rx->ctx.addr, rx->ctx.recv_dst,
               bt_hex(sdu->data, sdu->len));

        if (err) {
            BT_DBG("AppKeyNotDecrypt, AppIdx 0x%04x", key->app_idx);
            continue;
        }

        rx->ctx.app_idx = key->app_idx;
        bt_mesh_model_recv(rx, sdu);

        bt_mesh_free_buf(sdu);
        return 0;
    }

    if (rx->local_match) {
        BT_WARN("NoMatchAppKey");
    }

    bt_mesh_free_buf(sdu);
    return 0;
}

static struct seg_tx *seg_tx_lookup(uint16_t seq_zero, uint8_t obo, uint16_t addr)
{
    struct seg_tx *tx = NULL;
    int i;

    BT_DBG("SegTxLookup, SeqZero 0x%04x OBO %u Addr 0x%04x", seq_zero, obo, addr);

    for (i = 0; i < ARRAY_SIZE(seg_tx); i++) {
        tx = &seg_tx[i];

        if ((tx->seq_auth & TRANS_SEQ_ZERO_MASK) != seq_zero) {
            continue;
        }

        if (tx->dst == addr) {
            BT_DBG("SegTxFound, Dst 0x%04x", addr);
            return tx;
        }

        /* If the expected remote address doesn't match,
         * but the OBO flag is set and this is the first
         * acknowledgement, assume it's a Friend that's
         * responding and therefore accept the message.
         */
        if (obo && tx->nack_count == tx->seg_n + 1) {
            BT_DBG("SegTxOboFound, Dst 0x%04x", addr);
            tx->dst = addr;
            return tx;
        }
    }

    return NULL;
}

static int trans_ack(struct bt_mesh_net_rx *rx, uint8_t hdr,
                     struct net_buf_simple *buf, uint64_t *seq_auth)
{
    struct seg_tx *tx = NULL;
    bool tx_complete = false;
    uint16_t seq_zero = 0U;
    unsigned int bit = 0;
    uint32_t ack = 0U;
    uint8_t obo = 0U;

    BT_DBG("TransAck");

    if (buf->len != 6) {
        BT_ERR("Malformed Segment Ack (len %u)", buf->len);
        return -EINVAL;
    }

    seq_zero = net_buf_simple_pull_be16(buf);
    obo = seq_zero >> 15;
    seq_zero = (seq_zero >> 2) & TRANS_SEQ_ZERO_MASK;

    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND) && rx->friend_match) {
        BT_DBG("Ack for LPN 0x%04x of this Friend", rx->ctx.recv_dst);
        /* Best effort - we don't have enough info for true SeqAuth */
        *seq_auth = SEQ_AUTH(BLE_MESH_NET_IVI_RX(rx), seq_zero);
        return 0;
    }

    ack = net_buf_simple_pull_be32(buf);

    BT_DBG("OBO %u SeqZero 0x%04x Ack 0x%08lx", obo, seq_zero, ack);

    tx = seg_tx_lookup(seq_zero, obo, rx->ctx.addr);
    if (!tx) {
        BT_INFO("No matching TX context for Seg Ack");
        return -EINVAL;
    }

    bt_mesh_seg_tx_lock(tx);

    if (!BLE_MESH_ADDR_IS_UNICAST(tx->dst)) {
        bt_mesh_seg_tx_unlock(tx);
        BT_WARN("Received ack for segments to group");
        return -EINVAL;
    }

    *seq_auth = tx->seq_auth;

    if (!ack) {
        bt_mesh_seg_tx_unlock(tx);
        BT_WARN("SDU canceled");
        seg_tx_complete(tx, -ECANCELED);
        return 0;
    }

    if (find_msb_set(ack) - 1 > tx->seg_n) {
        bt_mesh_seg_tx_unlock(tx);
        BT_ERR("Too large segment number in ack");
        return -EINVAL;
    }

    k_delayed_work_cancel(&tx->rtx_timer);

    while ((bit = find_lsb_set(ack))) {
        if (tx->seg[bit - 1]) {
            BT_INFO("Seg %u/%u acked", bit - 1, tx->seg_n);

            seg_tx_done(tx, bit - 1);
        }

        ack &= ~BIT(bit - 1);
    }

    tx_complete = (tx->nack_count == 0);

    bt_mesh_seg_tx_unlock(tx);

    if (tx_complete == false) {
        seg_tx_send_unacked(tx);
    } else {
        BT_DBG("SDU TX complete");
        seg_tx_complete(tx, 0);
    }

    return 0;
}

static int trans_heartbeat(struct bt_mesh_net_rx *rx,
                           struct net_buf_simple *buf)
{
    uint8_t init_ttl = 0U, hops = 0U;
    uint16_t feat = 0U;

    BT_DBG("TransHeartbeat");

    if (buf->len != 3) {
        BT_ERR("Malformed heartbeat message (len %u)", buf->len);
        return -EINVAL;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_provisioned() &&
        rx->ctx.recv_dst != bt_mesh_get_hb_sub_dst()) {
        BT_WARN("Ignoring heartbeat to non-subscribed destination");
        return 0;
    }

    init_ttl = (net_buf_simple_pull_u8(buf) & 0x7f);
    feat = net_buf_simple_pull_be16(buf);

    hops = (init_ttl - rx->ctx.recv_ttl + 1);

    BT_INFO("Src 0x%04x TTL %u InitTTL %u Hops %u Feat 0x%04x",
            rx->ctx.addr, rx->ctx.recv_ttl, init_ttl, hops, feat);

    if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_provisioned()) {
        bt_mesh_heartbeat_recv(rx->ctx.addr, rx->ctx.recv_dst, hops, feat);
    } else if (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER_RECV_HB) && bt_mesh_is_provisioner_en()) {
        bt_mesh_pvnr_heartbeat_recv(rx->ctx.addr, rx->ctx.recv_dst,
                                    init_ttl, rx->ctx.recv_ttl,
                                    hops, feat, rx->ctx.recv_rssi);
    }

    return 0;
}

static int ctl_recv(struct bt_mesh_net_rx *rx, uint8_t hdr,
                    struct net_buf_simple *buf, uint64_t *seq_auth)
{
    uint8_t ctl_op = TRANS_CTL_OP(&hdr);

    BT_DBG("CTLRecv, OpCode 0x%02x Len %u", ctl_op, buf->len);

    BT_BQB(BLE_MESH_BQB_TEST_LOG_LEVEL_PRIMARY_ID_NODE | \
           BLE_MESH_BQB_TEST_LOG_LEVEL_SUB_ID_TNPT,
           "\nTNPTRecv: ctl: 0x%04x, ttl: 0x%04x, src: 0x%04x, dst: 0x%04x, payload: 0x%s",
           rx->ctl, rx->ctx.recv_ttl, rx->ctx.addr, rx->ctx.recv_dst,
           bt_hex(buf->data, buf->len));

    switch (ctl_op) {
    case TRANS_CTL_OP_ACK:
        return trans_ack(rx, hdr, buf, seq_auth);
    case TRANS_CTL_OP_HEARTBEAT:
        return trans_heartbeat(rx, buf);
    }

    /* Only acks and heartbeats may need processing without local_match */
    if (!rx->local_match) {
        BT_DBG("LocalNotMatch");
        return 0;
    }

#if CONFIG_BLE_MESH_DF_SRV
    switch (ctl_op) {
    case TRANS_CTL_OP_PATH_REQ:
    case TRANS_CTL_OP_PATH_REPLY:
    case TRANS_CTL_OP_PATH_CFM:
    case TRANS_CTL_OP_PATH_ECHO_REQ:
    case TRANS_CTL_OP_PATH_ECHO_REPLY:
    case TRANS_CTL_OP_DEP_NODE_UPDATE:
    case TRANS_CTL_OP_PATH_REQ_SOLIC:
        return bt_mesh_directed_forwarding_ctl_recv(ctl_op, rx, buf);
    }
#endif /* CONFIG_BLE_MESH_DF_SRV */

    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND) && !bt_mesh_lpn_established()) {
        switch (ctl_op) {
        case TRANS_CTL_OP_FRIEND_POLL:
            return bt_mesh_friend_poll(rx, buf);
        case TRANS_CTL_OP_FRIEND_REQ:
            return bt_mesh_friend_req(rx, buf);
        case TRANS_CTL_OP_FRIEND_CLEAR:
            return bt_mesh_friend_clear(rx, buf);
        case TRANS_CTL_OP_FRIEND_CLEAR_CFM:
            return bt_mesh_friend_clear_cfm(rx, buf);
        case TRANS_CTL_OP_FRIEND_SUB_ADD:
            return bt_mesh_friend_sub_add(rx, buf);
        case TRANS_CTL_OP_FRIEND_SUB_REM:
            return bt_mesh_friend_sub_rem(rx, buf);
        }
    }

#if CONFIG_BLE_MESH_LOW_POWER
    if (ctl_op == TRANS_CTL_OP_FRIEND_OFFER) {
        return bt_mesh_lpn_friend_offer(rx, buf);
    }

    if (rx->ctx.addr == bt_mesh.lpn.frnd) {
        if (ctl_op == TRANS_CTL_OP_FRIEND_CLEAR_CFM) {
            return bt_mesh_lpn_friend_clear_cfm(rx, buf);
        }

        if (rx->ctx.recv_cred != BLE_MESH_FRIENDSHIP_CRED) {
            BT_WARN("Message from friend with wrong credentials");
            return -EINVAL;
        }

        switch (ctl_op) {
        case TRANS_CTL_OP_FRIEND_UPDATE:
            return bt_mesh_lpn_friend_update(rx, buf);
        case TRANS_CTL_OP_FRIEND_SUB_CFM:
            return bt_mesh_lpn_friend_sub_cfm(rx, buf);
        }
    }
#endif /* CONFIG_BLE_MESH_LOW_POWER */

    BT_WARN("Unhandled TransOpCode 0x%02x", ctl_op);

    return -ENOENT;
}

static int trans_unseg(struct net_buf_simple *buf, struct bt_mesh_net_rx *rx,
                       uint64_t *seq_auth)
{
    uint8_t hdr = 0U;

    BT_DBG("TransUnseg, AKF %u AID 0x%02x", AKF(buf->data), AID(buf->data));

    if (buf->len < 1) {
        BT_ERR("Too small unsegmented PDU");
        return -EINVAL;
    }

    if (bt_mesh_rpl_check(rx, NULL)) {
        BT_WARN("Replay, Src 0x%04x Dst 0x%04x Seq 0x%06x",
                rx->ctx.addr, rx->ctx.recv_dst, rx->seq);
        return -EINVAL;
    }

    hdr = net_buf_simple_pull_u8(buf);

    if (rx->ctl) {
        return ctl_recv(rx, hdr, buf, seq_auth);
    }

    /* SDUs must match a local element or an LPN of this Friend. */
    if (!rx->local_match && !rx->friend_match) {
        BT_DBG("LocalAndFrndNotMatch");
        return 0;
    }

    return sdu_recv(rx, rx->seq, hdr, 0, buf);
}

static inline int32_t ack_timeout(struct seg_rx *rx)
{
    int32_t to = 0;
    uint8_t ttl = 0U;

    if (rx->ttl == BLE_MESH_TTL_DEFAULT) {
        ttl = bt_mesh_default_ttl_get();
    } else {
        ttl = rx->ttl;
    }

    /* The acknowledgment timer shall be set to a minimum of
     * 150 + 50 * TTL milliseconds.
     */
    to = K_MSEC(150 + (ttl * 50U));

    /* 100 ms for every not yet received segment */
    to += K_MSEC(((rx->seg_n + 1) - popcount(rx->block)) * 100U);

    BT_DBG("AckTimeout, TTL %u TO %ld", ttl, to);

    /* Make sure we don't send more frequently than the duration for
     * each packet (default is 300ms).
     */
    return MAX(to, K_MSEC(400));
}

int bt_mesh_ctl_send(struct bt_mesh_net_tx *tx, uint8_t ctl_op, void *data,
                     size_t data_len, const struct bt_mesh_send_cb *cb,
                     void *cb_data)
{
    struct net_buf_simple buf = {0};

    BT_DBG("CtlSend");

    net_buf_simple_init_with_data(&buf, data, data_len);

    if (data_len > BLE_MESH_SDU_UNSEG_MAX) {
        tx->ctx->send_tag |= BLE_MESH_TAG_SEND_SEGMENTED;
    }

    /* Set app_idx to unused here since CTL is only encrypted with NetKey */
    tx->ctx->app_idx = BLE_MESH_KEY_UNUSED;

    BT_DBG("Src 0x%04x Dst 0x%04x TTL 0x%02x CTL 0x%02x",
           tx->src, tx->ctx->addr, tx->ctx->send_ttl, ctl_op);
    BT_DBG("Len %u: %s", data_len, bt_hex(data, data_len));

    if (bt_mesh_tag_send_segmented(tx->ctx->send_tag)) {
        return send_seg(tx, &buf, cb, cb_data, &ctl_op);
    }

    return send_unseg(tx, &buf, cb, cb_data, &ctl_op);
}

static int send_ack(struct bt_mesh_subnet *sub, uint16_t src, uint16_t dst,
                    uint8_t ttl, uint64_t *seq_auth, uint32_t block, uint8_t obo)
{
    struct bt_mesh_msg_ctx ctx = {
        .net_idx   = sub->net_idx,
        .app_idx   = BLE_MESH_KEY_UNUSED,
        .addr      = dst,
        .send_ttl  = ttl,
        /* TODO:
         * Could be Managed flooding/Friendship/Directed security credentials.
         * The "recv_cred" could be used to initialize "send_cred".
         */
        .send_cred = BLE_MESH_FLOODING_CRED,
    };
    struct bt_mesh_net_tx tx = {
        .sub  = sub,
        .ctx  = &ctx,
        .src  = obo ? bt_mesh_primary_addr() : src,
        .xmit = bt_mesh_net_transmit_get(),
    };
    uint16_t seq_zero = *seq_auth & TRANS_SEQ_ZERO_MASK;
    uint8_t buf[6] = {0};

    BT_DBG("SendAck, SeqZero 0x%04x Block 0x%08lx OBO %u", seq_zero, block, obo);

    if (bt_mesh_lpn_established()) {
        BT_WARN("Not sending ack when LPN is enabled");
        return 0;
    }

    /* This can happen if the segmented message was destined for a group
     * or virtual address.
     */
    if (!BLE_MESH_ADDR_IS_UNICAST(src)) {
        BT_INFO("Not sending ack for non-unicast address");
        return 0;
    }

    sys_put_be16(((seq_zero << 2) & 0x7ffc) | (obo << 15), buf);
    sys_put_be32(block, &buf[2]);

    return bt_mesh_ctl_send(&tx, TRANS_CTL_OP_ACK, buf, sizeof(buf),
                            NULL, NULL);
}

static void seg_rx_reset(struct seg_rx *rx, bool full_reset)
{
    BT_DBG("SegRxReset, FullReset %u", full_reset);

    bt_mesh_seg_rx_lock();

    k_delayed_work_cancel(&rx->ack_timer);

    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND) && rx->obo &&
        rx->block != BLOCK_COMPLETE(rx->seg_n)) {
        BT_WARN("Clearing incomplete buffers from Friend queue");
        bt_mesh_friend_clear_incomplete(rx->sub, rx->src, rx->dst,
                                        &rx->seq_auth);
    }

    rx->in_use = 0U;

    /* We don't always reset these values since we need to be able to
     * send an ack if we receive a segment after we've already received
     * the full SDU.
     */
    if (full_reset) {
        rx->seq_auth = 0U;
        rx->sub = NULL;
        rx->src = BLE_MESH_ADDR_UNASSIGNED;
        rx->dst = BLE_MESH_ADDR_UNASSIGNED;
    }

    bt_mesh_seg_rx_unlock();
}

static uint32_t incomplete_timeout(struct seg_rx *rx)
{
    uint32_t timeout = 0U;
    uint8_t ttl = 0U;

    if (rx->ttl == BLE_MESH_TTL_DEFAULT) {
        ttl = bt_mesh_default_ttl_get();
    } else {
        ttl = rx->ttl;
    }

    /* "The incomplete timer shall be set to a minimum of 10 seconds." */
    timeout = K_SECONDS(10);

    /* The less segments being received, the shorter timeout will be used. */
    timeout += K_MSEC(ttl * popcount(rx->block) * 100U);

    BT_DBG("IncompleteTimeout %lu", timeout);

    return MIN(timeout, K_SECONDS(60));
}

static void seg_ack(struct k_work *work)
{
    struct seg_rx *rx = CONTAINER_OF(work, struct seg_rx, ack_timer);

    BT_DBG("SegAck, Last %lu", rx->last);

    bt_mesh_seg_rx_lock();

    if (k_uptime_get_32() - rx->last > incomplete_timeout(rx)) {
        BT_WARN("Incomplete timer expired");

        bt_mesh_seg_rx_unlock();

        seg_rx_reset(rx, false);
        return;
    }

    /* Add this check in case the timeout handler is just executed
     * after the seg_rx_reset() which may reset rx->sub to NULL.
     */
    if (rx->sub == NULL) {
        bt_mesh_seg_rx_unlock();
        return;
    }

    send_ack(rx->sub, rx->dst, rx->src, rx->ttl, &rx->seq_auth,
             rx->block, rx->obo);

    k_delayed_work_submit(&rx->ack_timer, ack_timeout(rx));

    bt_mesh_seg_rx_unlock();
}

static inline uint16_t sdu_len_max(uint8_t seg_n,uint16_t seg_len)
{
    BT_DBG("IsSduLenOK,Len:%u,SegN:%u", seg_len, seg_n);

    return ((seg_n + 1) * seg_len);
}

static inline bool sdu_len_is_ok(bool ctl, uint8_t seg_n, uint16_t buf_len)
{
    struct seg_info si = {
        .ctl = ctl,
    };

    BT_DBG("IsSduLenOK, CTL %u SegN %u", ctl, seg_n);

#if CONFIG_BLE_MESH_LONG_PACKET
    if ((sdu_len_max(seg_n, buf_len) > CONFIG_BLE_MESH_RX_SDU_MAX)) {
        si.long_pkt = 1;
        return ((seg_n + 1) * seg_len(&si) <= BLE_MESH_EXT_RX_SDU_MAX);
    }
#endif
    return ((seg_n + 1) * seg_len(&si) <= CONFIG_BLE_MESH_RX_SDU_MAX);
}

static struct seg_rx *seg_rx_find_with_buf(struct bt_mesh_net_rx *net_rx,
                                           const uint64_t *seq_auth,
                                           struct seg_rx *rx_buf,
                                           uint16_t rx_buf_size )
{
    int i;

    BT_DBG("SegRxFind, SeqAuth 0x%llx", *seq_auth);
    for (i = 0; i < rx_buf_size; i++) {
        struct seg_rx *rx = &rx_buf[i];

        BT_DBG("Seg%u/%u: Src 0x%04x Dst 0x%04x SeqAuth 0x%llx",
               i, rx->seg_n, rx->src, rx->dst, rx->seq_auth);

        if (rx->src != net_rx->ctx.addr ||
            rx->dst != net_rx->ctx.recv_dst) {
            continue;
        }

        /* When ">=" is used, return newer RX context in addition to an exact match,
         * so the calling function can properly discard an old SeqAuth.
         */
#if CONFIG_BLE_MESH_DISCARD_OLD_SEQ_AUTH
        if (rx->seq_auth >= *seq_auth) {
#else /* CONFIG_BLE_MESH_DISCARD_OLD_SEQ_AUTH */
        if (rx->seq_auth == *seq_auth) {
#endif /* CONFIG_BLE_MESH_DISCARD_OLD_SEQ_AUTH */
            return rx;
        }

        if (rx->in_use) {
            BT_WARN("Duplicate SDU from src 0x%04x", net_rx->ctx.addr);

            /* Clear out the old context since the sender
             * has apparently started sending a new SDU.
             */
            seg_rx_reset(rx, true);

            /* Return non-match so caller can re-allocate */
            return NULL;
        }
    }

    return NULL;
}


static struct seg_rx *seg_rx_find(struct bt_mesh_net_rx *net_rx,
                                  const uint64_t *seq_auth)
{
#if CONFIG_BLE_MESH_LONG_PACKET
    struct seg_rx *rx = NULL;
    rx = seg_rx_find_with_buf(net_rx, seq_auth, seg_rx, ARRAY_SIZE(seg_rx));

    if (rx) {
        if (likely(!net_rx->ctx.enh.long_pkt_cfg_used)) {
            return rx;
        }

        if (rx->seq_auth != *seq_auth) {
            return rx;
        }

        /* The first received segment that is possible is the last segment.
         * The segment information is stored in seg_rx, so it needs to be
         * confirmed in seg_rx.
         * This will only happen when the last segment is received
         * */
        assert(rx->block == BIT(rx->seg_n));
        assert(rx->ctl == net_rx->ctl);
        struct seg_info si = {
            .ctl = net_rx->ctl,
            .long_pkt = 0,
        };

        /* @note: This situation will not occur in theory.
         * If it occurs, please check the reason carefully */
        if (rx->buf.len <= (rx->seg_n * seg_len(&si))) {
            BT_ERR("Incorrect packet length, the rx will be reset");
            seg_rx_reset(rx, true);
            return NULL;
        }

        /* Copy the information in seg_rx into ext_seg_rx */
        struct seg_rx *ext_rx = seg_rx_alloc(net_rx, &(rx->hdr), seq_auth, rx->seg_n);
        if (!ext_rx) {
            BT_ERR("Failed to alloc ext_rx for long packet");
            return NULL;
        }

        uint16_t last_seg_len = rx->buf.len - (rx->seg_n * seg_len(&si));
        uint8_t  *last_seg = rx->buf.data + (rx->seg_n * seg_len(&si));

        /* Update to long packet length */
        si.long_pkt = 1;
        ext_rx->buf.len = rx->seg_n * seg_len(&si) + last_seg_len;
        ext_rx->block = rx->block;
        ext_rx->obo = rx->obo;

        memcpy(ext_rx->buf.data + (rx->seg_n * seg_len(&si)), last_seg, last_seg_len);
        seg_rx_reset(rx, true);
        return ext_rx;
    }
    return seg_rx_find_with_buf(net_rx, seq_auth, ext_seg_rx, ARRAY_SIZE(ext_seg_rx));
#else
    return seg_rx_find_with_buf(net_rx, seq_auth, seg_rx, ARRAY_SIZE(seg_rx));
#endif
}

static bool seg_rx_is_valid(struct seg_rx *rx, struct bt_mesh_net_rx *net_rx,
                            const uint8_t *hdr, uint8_t seg_n)
{
    BT_DBG("IsSegRxValid");

    if (rx->hdr != *hdr || rx->seg_n != seg_n) {
        BT_ERR("Invalid segment for ongoing session");
        return false;
    }

    if (rx->src != net_rx->ctx.addr || rx->dst != net_rx->ctx.recv_dst) {
        BT_ERR("Invalid source or destination for segment");
        return false;
    }

    if (rx->ctl != net_rx->ctl) {
        BT_ERR("Inconsistent CTL in segment");
        return false;
    }

    return true;
}

static struct seg_rx *seg_rx_alloc(struct bt_mesh_net_rx *net_rx,
                                   const uint8_t *hdr, const uint64_t *seq_auth,
                                   uint8_t seg_n)
{
    int i;
    /* By default, traditional seg_rx is used for allocation.
     * If the first segment received is the last segment of
     * the long packet, and its length is the length of the traditional packet,
     * Since it is impossible to determine whether it is a long packet
     * under the current situation, can only copy the rx information
     * into the ext_rx buffer when the next segment received and can be
     * confirmed to be a long packet*/
    struct seg_rx *seg_rx_buf = seg_rx;
    uint16_t rx_buf_size = ARRAY_SIZE(seg_rx);

#if CONFIG_BLE_MESH_LONG_PACKET
    if (net_rx->ctx.enh.long_pkt_cfg_used) {
        seg_rx_buf = ext_seg_rx;
        rx_buf_size = ARRAY_SIZE(ext_seg_rx);
    }
#endif

    BT_DBG("SegRxAlloc, SegN %u", seg_n);

    for (i = 0; i < rx_buf_size; i++) {
        struct seg_rx *rx = &seg_rx_buf[i];

        if (rx->in_use) {
            continue;
        }

        rx->in_use = 1U;
        net_buf_simple_reset(&rx->buf);
        rx->sub = net_rx->sub;
        rx->ctl = net_rx->ctl;
        rx->seq_auth = *seq_auth;
        rx->seg_n = seg_n;
        rx->hdr = *hdr;
        rx->ttl = net_rx->ctx.recv_ttl;
        rx->src = net_rx->ctx.addr;
        rx->dst = net_rx->ctx.recv_dst;
        rx->block = 0U;
#if CONFIG_BLE_MESH_LONG_PACKET
        rx->ext = (seg_rx_buf == ext_seg_rx);
#else
        rx->ext = 0;
#endif
        BT_DBG("New RX context. Block Complete 0x%08x",
               BLOCK_COMPLETE(seg_n));

        return rx;
    }

    return NULL;
}

static int trans_seg(struct net_buf_simple *buf, struct bt_mesh_net_rx *net_rx,
                     enum bt_mesh_friend_pdu_type *pdu_type, uint64_t *seq_auth,
                     uint8_t *seg_count)
{
    struct bt_mesh_rpl *rpl = NULL;
    struct seg_rx *rx = NULL;
    uint8_t *hdr = buf->data;
    uint16_t seq_zero = 0U;
    uint8_t seg_n = 0U;
    uint8_t seg_o = 0U;
    int err = 0;

    BT_DBG("TransSeg");

    if (buf->len < 5) {
        BT_ERR("Too short segmented message (len %u)", buf->len);
        return -EINVAL;
    }

    if (bt_mesh_rpl_check(net_rx, &rpl)) {
        BT_WARN("Replay, Src 0x%04x Dst 0x%04x Seq 0x%06x",
                net_rx->ctx.addr, net_rx->ctx.recv_dst, net_rx->seq);
        return -EINVAL;
    }

    BT_DBG("Aszmic %u AKF %u AID 0x%02x", ASZMIC(hdr), AKF(hdr), AID(hdr));

    net_buf_simple_pull(buf, 1);

    seq_zero = net_buf_simple_pull_be16(buf);
    seg_o = (seq_zero & 0x03) << 3;
    seq_zero = (seq_zero >> 2) & TRANS_SEQ_ZERO_MASK;
    seg_n = net_buf_simple_pull_u8(buf);
    seg_o |= seg_n >> 5;
    seg_n &= 0x1f;

    BT_DBG("SeqZero 0x%04x SegO %u SegN %u", seq_zero, seg_o, seg_n);

    if (seg_o > seg_n) {
        BT_ERR("SegO greater than SegN (%u > %u)", seg_o, seg_n);
        return -EINVAL;
    }

#if CONFIG_BLE_MESH_LONG_PACKET
    if (buf->len > BLE_MESH_EXT_APP_SEG_SDU_MAX) {
        BT_ERR("The seg length(%d) exceeds the maximum seg length(%d) supported",
                buf->len, BLE_MESH_EXT_APP_SEG_SDU_MAX);
        return -EINVAL;
    }
#endif

    /* According to Mesh 1.0 specification:
     * "The SeqAuth is composed of the IV Index and the sequence number
     *  (SEQ) of the first segment"
     *
     * Therefore we need to calculate very first SEQ in order to find
     * seqAuth. We can calculate as below:
     *
     * SEQ(0) = SEQ(n) - (delta between seqZero and SEQ(n) by looking into
     * 14 least significant bits of SEQ(n))
     *
     * Mentioned delta shall be >= 0, if it is not then seq_auth will
     * be broken and it will be verified by the code below.
     */
    *seq_auth = SEQ_AUTH(BLE_MESH_NET_IVI_RX(net_rx),
                         (net_rx->seq -
                          ((((net_rx->seq & BIT_MASK(14)) - seq_zero)) & BIT_MASK(13))));

    *seg_count = seg_n + 1;

    BT_DBG("Src 0x%04x Dst 0x%04x SeqAuth 0x%llx SegCount %u",
           net_rx->ctx.addr, net_rx->ctx.recv_dst, *seq_auth, *seg_count);

    /* Look for old RX sessions */
    rx = seg_rx_find(net_rx, seq_auth);
    if (rx) {
        /* Discard old SeqAuth packet */
        if (rx->seq_auth > *seq_auth) {
            BT_WARN("SeqAuth 0x%llx vs. 0x%llx", rx->seq_auth, *seq_auth);
            return -EINVAL;
        }

        if (!seg_rx_is_valid(rx, net_rx, hdr, seg_n)) {
            return -EINVAL;
        }

        if (rx->in_use) {
            BT_DBG("Existing RX context. Block 0x%08x", rx->block);
            goto found_rx;
        }

        if (rx->block == BLOCK_COMPLETE(rx->seg_n)) {
            BT_INFO("Got segment for already complete SDU");

            send_ack(net_rx->sub, net_rx->ctx.recv_dst,
                     net_rx->ctx.addr, net_rx->ctx.send_ttl,
                     seq_auth, rx->block, rx->obo);

            if (rpl) {
                bt_mesh_update_rpl(rpl, net_rx);
            }

            return -EALREADY;
        }

        /* We ignore instead of sending block ack 0 since the
         * ack timer is always smaller than the incomplete
         * timer, i.e. the sender is misbehaving.
         */
        BT_WARN("Got segment for canceled SDU");
        return -EINVAL;
    }

    /* Bail out early if we're not ready to receive such a large SDU */
    if (!sdu_len_is_ok(net_rx->ctl, seg_n, buf->len)) {
        BT_ERR("Too big incoming SDU length");
        send_ack(net_rx->sub, net_rx->ctx.recv_dst, net_rx->ctx.addr,
                 net_rx->ctx.send_ttl, seq_auth, 0,
                 net_rx->friend_match);
        return -EMSGSIZE;
    }

    /* Verify early that there will be space in the Friend Queue(s) in
     * case this message is destined to an LPN of ours.
     */
    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND) &&
        net_rx->friend_match && !net_rx->local_match &&
        !bt_mesh_friend_queue_has_space(net_rx->sub->net_idx,
                                        net_rx->ctx.addr,
                                        net_rx->ctx.recv_dst, seq_auth,
                                        *seg_count)) {
        BT_WARN("NoSpaceInFrndQueue, SegCount %u", *seg_count);

        send_ack(net_rx->sub, net_rx->ctx.recv_dst,
                 net_rx->ctx.addr, net_rx->ctx.send_ttl,
                 seq_auth, 0, net_rx->friend_match);
        return -ENOBUFS;
    }

    /* Look for free slot for a new RX session */
    rx = seg_rx_alloc(net_rx, hdr, seq_auth, seg_n);
    if (!rx) {
        /* Warn but don't cancel since the existing slots will
         * eventually be freed up and we'll be able to process
         * this one.
         */
        BT_WARN("SegRxFull, Src %04x", net_rx->ctx.addr);
        return -ENOMEM;
    }

    rx->obo = net_rx->friend_match;

found_rx:
    if (BIT(seg_o) & rx->block) {
        BT_INFO("Received already received fragment");
        return -EALREADY;
    }

    struct seg_info si = {
        .ctl = rx->ctl,
#if CONFIG_BLE_MESH_LONG_PACKET
        .long_pkt = rx->ext,
#endif
    };

    /* All segments, except the last one, must either have 8 bytes of
     * payload (for 64bit Net MIC) or 12 bytes of payload (for 32bit
     * Net MIC).
     */
    if (seg_o == seg_n) {
        /* Set the expected final buffer length */
        rx->buf.len = seg_n * seg_len(&si) + buf->len;

        BT_DBG("Target len %u * %u + %u = %u",
            seg_n, seg_len(&si), buf->len, rx->buf.len);

        if ((!rx->ext && rx->buf.len > CONFIG_BLE_MESH_RX_SDU_MAX)
#if CONFIG_BLE_MESH_LONG_PACKET
            || (rx->ext && rx->buf.len > BLE_MESH_EXT_RX_SDU_MAX)
#endif
        ) {
            BT_ERR("Too large SDU len %u", rx->buf.len);
            send_ack(net_rx->sub, net_rx->ctx.recv_dst,
                     net_rx->ctx.addr, net_rx->ctx.send_ttl,
                     seq_auth, 0, rx->obo);

            seg_rx_reset(rx, true);

            return -EMSGSIZE;
        }
    } else {
        if (buf->len != seg_len(&si)) {
            BT_ERR("Incorrect segment size for message type %d %d len %d except %d",
                    rx->ext, rx->ctl, buf->len, seg_len(&si));
            return -EINVAL;
        }
    }

    /* Reset the Incomplete Timer */
    rx->last = k_uptime_get_32();

    if (!k_delayed_work_remaining_get(&rx->ack_timer) &&
        !bt_mesh_lpn_established()) {
        k_delayed_work_submit(&rx->ack_timer, ack_timeout(rx));
    }

    /* Location in buffer can be calculated based on seg_o & rx->ctl */
    memcpy(rx->buf.data + (seg_o * seg_len(&si)), buf->data, buf->len);

    BT_INFO("Seg %u/%u received",seg_o, seg_n);

    /* Mark segment as received */
    rx->block |= BIT(seg_o);

    if (rx->block != BLOCK_COMPLETE(seg_n)) {
        BT_DBG("FrndPDUPartial");
        *pdu_type = BLE_MESH_FRIEND_PDU_PARTIAL;
        return 0;
    }

    BT_DBG("Complete SDU");

    if (rpl) {
        bt_mesh_update_rpl(rpl, net_rx);
    }

    *pdu_type = BLE_MESH_FRIEND_PDU_COMPLETE;

    k_delayed_work_cancel(&rx->ack_timer);

    send_ack(net_rx->sub, net_rx->ctx.recv_dst, net_rx->ctx.addr,
             net_rx->ctx.send_ttl, seq_auth, rx->block, rx->obo);

    if (net_rx->ctl) {
        err = ctl_recv(net_rx, *hdr, &rx->buf, seq_auth);
    } else {
        err = sdu_recv(net_rx, (rx->seq_auth & 0xffffff),
                       *hdr, ASZMIC(hdr), &rx->buf);
    }

    seg_rx_reset(rx, false);

    return err;
}

int bt_mesh_trans_recv(struct net_buf_simple *buf, struct bt_mesh_net_rx *rx)
{
    enum bt_mesh_friend_pdu_type pdu_type = BLE_MESH_FRIEND_PDU_SINGLE;
    struct net_buf_simple_state state = {0};
    uint64_t seq_auth = TRANS_SEQ_AUTH_NVAL;
    uint8_t seg_count = 0U;
    int err = 0;

    BT_DBG("TransRecv");

    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND)) {
        rx->friend_match = bt_mesh_friend_match(rx->sub->net_idx,
                                                rx->ctx.recv_dst);
    } else {
        rx->friend_match = false;
    }

    BT_DBG("Src 0x%04x Dst 0x%04x Seq 0x%06x FrndMatch %u",
           rx->ctx.addr, rx->ctx.recv_dst, rx->seq, rx->friend_match);

    /* Remove network headers */
    net_buf_simple_pull(buf, BLE_MESH_NET_HDR_LEN);

    BT_DBG("PDU %u %s", buf->len, bt_hex(buf->data, buf->len));

    /* If LPN mode is enabled messages are only accepted when we've
     * requested the Friend to send them. The messages must also
     * be encrypted using the Friend Credentials.
     */
    if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER) &&
        bt_mesh_lpn_established() &&
        rx->net_if == BLE_MESH_NET_IF_ADV &&
        (!bt_mesh_lpn_waiting_update() ||
         rx->ctx.recv_cred != BLE_MESH_FRIENDSHIP_CRED)) {
        BT_WARN("Ignoring unexpected message in Low Power mode");
        return -EAGAIN;
    }

    /* Save the app-level state so the buffer can later be placed in
     * the Friend Queue.
     */
    net_buf_simple_save(buf, &state);

    if (SEG(buf->data)) {
        /* Segmented messages must match a local element or an
         * LPN of this Friend.
         */
        if (!rx->local_match && !rx->friend_match) {
            BT_DBG("LocalAndFrndNotMatch");
            return 0;
        }

        bt_mesh_seg_rx_lock();
        err = trans_seg(buf, rx, &pdu_type, &seq_auth, &seg_count);
        bt_mesh_seg_rx_unlock();
    } else {
        seg_count = 1U;

        err = trans_unseg(buf, rx, &seq_auth);
    }

    /* Notify LPN state machine so a Friend Poll will be sent. If the
     * message was a Friend Update it's possible that a Poll was already
     * queued for sending, however that's fine since then the
     * bt_mesh_lpn_waiting_update() function will return false:
     * we still need to go through the actual sending to the bearer and
     * wait for ReceiveDelay before transitioning to WAIT_UPDATE state.
     * Another situation where we want to notify the LPN state machine
     * is if it's configured to use an automatic Friendship establishment
     * timer, in which case we want to reset the timer at this point.
     *
     */
    if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER) &&
        (bt_mesh_lpn_timer() ||
        (bt_mesh_lpn_established() && bt_mesh_lpn_waiting_update()))) {
        bt_mesh_lpn_msg_received(rx);
    }

    net_buf_simple_restore(buf, &state);

    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND) && rx->friend_match && !err) {
        if (seq_auth == TRANS_SEQ_AUTH_NVAL) {
            bt_mesh_friend_enqueue_rx(rx, pdu_type, NULL,
                                      seg_count, buf);
        } else {
            bt_mesh_friend_enqueue_rx(rx, pdu_type, &seq_auth,
                                      seg_count, buf);
        }
    }

    return err;
}

void bt_mesh_rx_reset(void)
{
    int i;

    BT_DBG("RxReset");

    for (i = 0; i < ARRAY_SIZE(seg_rx); i++) {
        seg_rx_reset(&seg_rx[i], true);
    }

#if CONFIG_BLE_MESH_LONG_PACKET
    for (i = 0; i < ARRAY_SIZE(ext_seg_rx); i++) {
        seg_rx_reset(&ext_seg_rx[i], true);
    }
#endif /* CONFIG_BLE_MESH_LONG_PACKET */
}

void bt_mesh_tx_reset(void)
{
    int i;

    BT_DBG("TxReset");

    for (i = 0; i < ARRAY_SIZE(seg_tx); i++) {
        seg_tx_reset(&seg_tx[i]);
    }
}

void bt_mesh_rx_reset_single(uint16_t src)
{
    int i;

    BT_DBG("RxResetSingle, Src 0x%04x", src);

    if (!BLE_MESH_ADDR_IS_UNICAST(src)) {
        return;
    }

    for (i = 0; i < ARRAY_SIZE(seg_rx); i++) {
        struct seg_rx *rx = &seg_rx[i];
        if (src == rx->src) {
            seg_rx_reset(rx, true);
        }
    }

#if CONFIG_BLE_MESH_LONG_PACKET
    for (i = 0; i < ARRAY_SIZE(ext_seg_rx); i++) {
        struct seg_rx *rx = &ext_seg_rx[i];
        if (src == rx->src) {
            seg_rx_reset(rx, true);
        }
    }
#endif /* CONFIG_BLE_MESH_LONG_PACKET */
}

void bt_mesh_tx_reset_single(uint16_t dst)
{
    int i;

    BT_DBG("TxResetSingle, Dst 0x%04x", dst);

    if (!BLE_MESH_ADDR_IS_UNICAST(dst)) {
        return;
    }

    for (i = 0; i < ARRAY_SIZE(seg_tx); i++) {
        struct seg_tx *tx = &seg_tx[i];
        if (dst == tx->dst) {
            seg_tx_reset(tx);
        }
    }
}

void bt_mesh_trans_init(void)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(seg_tx); i++) {
        k_delayed_work_init(&seg_tx[i].rtx_timer, seg_retransmit);
        bt_mesh_r_mutex_create(&seg_tx[i].lock);
    }

    for (i = 0; i < ARRAY_SIZE(seg_rx); i++) {
        k_delayed_work_init(&seg_rx[i].ack_timer, seg_ack);
        seg_rx[i].buf.size = CONFIG_BLE_MESH_RX_SDU_MAX;
        seg_rx[i].buf.__buf = (seg_rx_buf_data +
                               (i * CONFIG_BLE_MESH_RX_SDU_MAX));
        seg_rx[i].buf.data = seg_rx[i].buf.__buf;
    }

#if CONFIG_BLE_MESH_LONG_PACKET
    for (i = 0; i < ARRAY_SIZE(ext_seg_rx); i++) {
        k_delayed_work_init(&ext_seg_rx[i].ack_timer, seg_ack);
        ext_seg_rx[i].buf.size = BLE_MESH_EXT_RX_SDU_MAX;
        ext_seg_rx[i].buf.__buf = (ext_seg_rx_buf_data +
                                (i * BLE_MESH_EXT_RX_SDU_MAX));
        ext_seg_rx[i].buf.data = ext_seg_rx[i].buf.__buf;
    }
#endif

    bt_mesh_r_mutex_create(&seg_rx_lock);
}

#if CONFIG_BLE_MESH_DEINIT
void bt_mesh_trans_deinit(bool erase)
{
    int i;

    bt_mesh_rx_reset();
    bt_mesh_tx_reset();
    bt_mesh_rpl_reset(erase);

    for (i = 0; i < ARRAY_SIZE(seg_tx); i++) {
        k_delayed_work_free(&seg_tx[i].rtx_timer);
        bt_mesh_r_mutex_free(&seg_tx[i].lock);
    }

    for (i = 0; i < ARRAY_SIZE(seg_rx); i++) {
        k_delayed_work_free(&seg_rx[i].ack_timer);
    }

#if CONFIG_BLE_MESH_LONG_PACKET
    for (i = 0; i < ARRAY_SIZE(ext_seg_rx); i++) {
        k_delayed_work_free(&ext_seg_rx[i].ack_timer);
    }
#endif /* CONFIG_BLE_MESH_LONG_PACKET */

    bt_mesh_r_mutex_free(&seg_rx_lock);
}
#endif /* CONFIG_BLE_MESH_DEINIT */
