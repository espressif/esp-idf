/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "crypto.h"
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

#include "mesh_v1.1/utils.h"

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

/* How long to wait for available buffers before giving up */
#define BUF_TIMEOUT                 K_NO_WAIT

static struct seg_tx {
    struct bt_mesh_subnet *sub;
    struct net_buf        *seg[CONFIG_BLE_MESH_TX_SEG_MAX];
    uint64_t               seq_auth;
    uint8_t                hdr;
    uint16_t               src;
    uint16_t               dst;
    uint16_t               app_idx;
    uint8_t                xmit;            /* Segment transmit */
    uint8_t                aszmic:1;        /* MIC size */
    uint8_t                ttl;             /* TTL */
    uint8_t                cred;            /* Security credentials */
    uint8_t                tag;             /* Additional metadata */
    uint16_t               len;
    uint8_t                nack_count;      /* Number of unacked segs */
    uint32_t               seg_n:5,         /* Last segment index */
                           last_seg_n:5,    /* Last transmitted segment index */
                           lsn_updated:1,   /* Last transmitted segment index updated */
                           surc:4,          /* SAR Unicast Retransmission Count */
                           surwpc:4,        /* SAR Unicast Retransmission Without Progress Count
                                             * (i.e., without newly marking any segment as acknowledged)
                                             */
                           smrc:4,          /* SAR Multicast Retransmission Count */
                           new_key:1,       /* New/Old Key */
                           ctl:1,           /* Control packet */
                           resend:1;        /* Segments are been retransmitted */
    const struct bt_mesh_send_cb *cb;
    void                  *cb_data;
    struct k_delayed_work  seg_timer;       /* Segment Interval timer */
    struct k_delayed_work  rtx_timer;       /* Segment Retransmission timer */
} seg_tx[CONFIG_BLE_MESH_TX_SEG_MSG_COUNT];

static struct seg_rx {
    struct bt_mesh_subnet *sub;
    uint64_t               seq_auth;
    uint16_t               seg_n:5,
                           ctl:1,
                           in_use:1,
                           obo:1,
                           new_seg:1,       /* Indicate if a new segment is just received */
                           sarc:2;          /* SAR ACK Retransmissions Count */
    uint8_t                hdr;
    uint8_t                ttl;
    uint16_t               src;
    uint16_t               dst;
    uint32_t               block;
    uint32_t               last_ack;
    struct k_delayed_work  ack_timer;
    struct k_delayed_work  dis_timer;
    struct net_buf_simple  buf;
} seg_rx[CONFIG_BLE_MESH_RX_SEG_MSG_COUNT] = {
    [0 ... (CONFIG_BLE_MESH_RX_SEG_MSG_COUNT - 1)] = {
        .buf.size = CONFIG_BLE_MESH_RX_SDU_MAX,
    },
};

static uint8_t seg_rx_buf_data[(CONFIG_BLE_MESH_RX_SEG_MSG_COUNT *
                                CONFIG_BLE_MESH_RX_SDU_MAX)];

static const struct bt_mesh_send_cb seg_sent_cb;

static bt_mesh_mutex_t seg_tx_lock;
static bt_mesh_mutex_t seg_rx_lock;

static inline void bt_mesh_seg_tx_lock(void)
{
    bt_mesh_r_mutex_lock(&seg_tx_lock);
}

static inline void bt_mesh_seg_tx_unlock(void)
{
    bt_mesh_r_mutex_unlock(&seg_tx_lock);
}

static inline void bt_mesh_seg_rx_lock(void)
{
    bt_mesh_r_mutex_lock(&seg_rx_lock);
}

static inline void bt_mesh_seg_rx_unlock(void)
{
    bt_mesh_r_mutex_unlock(&seg_rx_lock);
}

uint8_t bt_mesh_seg_send_interval(void)
{
    return (bt_mesh_get_sar_sis() + 1) * 10;
}

uint8_t bt_mesh_get_seg_rtx_num(void)
{
    return bt_mesh_get_sar_urc();
}

int32_t bt_mesh_seg_rtx_interval(uint16_t dst, uint8_t ttl)
{
    if (BLE_MESH_ADDR_IS_UNICAST(dst)) {
        if (ttl == 0) {
            return (bt_mesh_get_sar_uris() + 1) * 25;
        }

        return ((bt_mesh_get_sar_uris() + 1) * 25 +
                (bt_mesh_get_sar_urii() + 1) * 25 * (ttl - 1));
    }

    return (bt_mesh_get_sar_mris() + 1) * 25;
}

int32_t bt_mesh_get_seg_rtx_timeout(uint16_t dst, uint8_t ttl)
{
    return bt_mesh_seg_rtx_interval(dst, ttl);
}

uint32_t bt_mesh_seg_discard_timeout(void)
{
    return K_SECONDS((bt_mesh_get_sar_dt() + 1) * 5);
}

uint32_t bt_mesh_seg_rx_interval(void)
{
    return (bt_mesh_get_sar_rsis() + 1) * 10;
}

uint32_t bt_mesh_seg_ack_timeout(uint8_t seg_n)
{
    float min = MIN((float)seg_n + 0.5, (float)bt_mesh_get_sar_adi() + 1.5);
    return (uint32_t)(min * bt_mesh_seg_rx_interval());
}

uint32_t bt_mesh_seg_ack_period(void)
{
    float val = (float)bt_mesh_get_sar_adi() + 1.5;
    return (uint32_t)(val * bt_mesh_seg_rx_interval());
}

struct bt_mesh_app_key *bt_mesh_app_key_get(uint16_t app_idx)
{
    if (bt_mesh_is_provisioned()) {
#if CONFIG_BLE_MESH_NODE
        if (!IS_ENABLED(CONFIG_BLE_MESH_FAST_PROV)) {
            for (int i = 0; i < ARRAY_SIZE(bt_mesh.app_keys); i++) {
                if (bt_mesh.app_keys[i].net_idx != BLE_MESH_KEY_UNUSED &&
                    bt_mesh.app_keys[i].app_idx == app_idx) {
                    return &bt_mesh.app_keys[i];
                }
            }
        } else {
            return bt_mesh_fast_prov_app_key_find(app_idx);
        }
#endif
    } else if (bt_mesh_is_provisioner_en()) {
#if CONFIG_BLE_MESH_PROVISIONER
        for (int i = 0; i < ARRAY_SIZE(bt_mesh.p_app_keys); i++) {
            if (bt_mesh.p_app_keys[i] &&
                bt_mesh.p_app_keys[i]->net_idx != BLE_MESH_KEY_UNUSED &&
                bt_mesh.p_app_keys[i]->app_idx == app_idx) {
                return bt_mesh.p_app_keys[i];
            }
        }
#endif
    }

    return NULL;
}

int bt_mesh_upper_key_get(const struct bt_mesh_subnet *subnet, uint16_t app_idx,
                          const uint8_t **key, uint8_t *aid, uint16_t dst)
{
    struct bt_mesh_app_key *app_key = NULL;

    if (app_idx == BLE_MESH_KEY_DEV) {
        *key = bt_mesh_dev_key_get(dst);
        if (!*key) {
            BT_ERR("DevKey of 0x%04x not found", dst);
            return -EINVAL;
        }

        *aid = 0U;
        return 0;
    }

    if (!subnet) {
        BT_ERR("Invalid subnet");
        return -EINVAL;
    }

    app_key = bt_mesh_app_key_get(app_idx);
    if (!app_key) {
        BT_ERR("AppKey 0x%04x not found", app_idx);
        return -ENOENT;
    }

    if (subnet->kr_phase == BLE_MESH_KR_PHASE_2 && app_key->updated) {
        *key = app_key->keys[1].val;
        *aid = app_key->keys[1].id;
    } else {
        *key = app_key->keys[0].val;
        *aid = app_key->keys[0].id;
    }

    return 0;
}

static int send_unseg(struct bt_mesh_net_tx *tx, struct net_buf_simple *sdu,
                      const struct bt_mesh_send_cb *cb, void *cb_data,
                      const uint8_t *ctl_op)
{
    struct net_buf *buf = NULL;

    BT_DBG("src 0x%04x dst 0x%04x app_idx 0x%04x sdu_len %u",
           tx->src, tx->ctx->addr, tx->ctx->app_idx, sdu->len);

    buf = bt_mesh_adv_create(BLE_MESH_ADV_DATA, BUF_TIMEOUT);
    if (!buf) {
        BT_ERR("Out of network buffers");
        return -ENOBUFS;
    }

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
            if (BLE_MESH_ADDR_IS_UNICAST(tx->ctx->addr)) {
                BT_ERR("Not enough space in Friend Queue");
                net_buf_unref(buf);
                return -ENOBUFS;
            }

            BT_WARN("No space in Friend Queue");
            goto send;
        }

        if (bt_mesh_friend_enqueue_tx(tx, BLE_MESH_FRIEND_PDU_SINGLE,
                                      NULL, 1, &buf->b) &&
            BLE_MESH_ADDR_IS_UNICAST(tx->ctx->addr)) {
            /* PDUs for a specific Friend should only go
             * out through the Friend Queue.
             */
            net_buf_unref(buf);
            send_cb_finalize(cb, cb_data);
            return 0;
        }
    }

send:
    return bt_mesh_net_send(tx, buf, cb, cb_data);
}

static inline uint8_t seg_len(bool ctl)
{
    if (ctl) {
        return BLE_MESH_CTL_SEG_SDU_MAX;
    }

    return BLE_MESH_APP_SEG_SDU_MAX;
}

bool bt_mesh_tx_in_progress(void)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(seg_tx); i++) {
        if (seg_tx[i].nack_count) {
            return true;
        }
    }

    return false;
}

static bool seg_tx_blocks(struct seg_tx *tx, uint16_t src, uint16_t dst)
{
    return (tx->src == src) && (tx->dst == dst);
}

static void seg_tx_done(struct seg_tx *tx, uint8_t seg_idx)
{
    /* If the segments are sent from local network interface, buf->ref
     * should be smaller than 4.
     * For other network interfaces, buf->ref should be smaller than 3.
     */
    bt_mesh_adv_buf_ref_debug(__func__, tx->seg[seg_idx], 4U, BLE_MESH_BUF_REF_SMALL);

    bt_mesh_atomic_set(&BLE_MESH_ADV_BUSY(tx->seg[seg_idx]), 0);
    net_buf_unref(tx->seg[seg_idx]);

    tx->seg[seg_idx] = NULL;
    tx->nack_count--;
}

static void seg_tx_reset(struct seg_tx *tx)
{
    bt_mesh_seg_tx_lock();

    k_delayed_work_free(&tx->seg_timer);
    k_delayed_work_free(&tx->rtx_timer);

    tx->cb = NULL;
    tx->cb_data = NULL;
    tx->seq_auth = 0U;
    tx->sub = NULL;
    tx->last_seg_n = 0;
    tx->lsn_updated = 0;
    tx->dst = BLE_MESH_ADDR_UNASSIGNED;
    tx->surc = 0;
    tx->surwpc = 0;
    tx->smrc = 0;

    for (size_t i = 0; i <= tx->seg_n && tx->nack_count; i++) {
        if (tx->seg[i] == NULL) {
            continue;
        }

        seg_tx_done(tx, i);
    }

    tx->nack_count = 0U;
    tx->seg_n = 0;

    bt_mesh_seg_tx_unlock();

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

static void seg_tx_complete(struct seg_tx *tx, int err)
{
    const struct bt_mesh_send_cb *cb = tx->cb;
    void *cb_data = tx->cb_data;

    seg_tx_reset(tx);

    /* TODO: notify the completion of sending segmented message */

    if (cb && cb->end) {
        cb->end(err, cb_data);
    }
}

static bool all_seg_acked(struct seg_tx *tx, uint8_t *seg_n)
{
    for (size_t i = 0; i <= tx->seg_n; i++) {
        if (tx->seg[i]) {
            if (seg_n) {
                *seg_n = i;
            }
            return false;
        }
    }

    return true;
}

static bool send_next_segment(struct seg_tx *tx, int *result)
{
    struct bt_mesh_msg_ctx ctx = {
        .app_idx = tx->app_idx,
        .addr = tx->dst,
        .send_ttl = tx->ttl,
        .send_cred = tx->cred,
        .send_tag = tx->tag,
    };
    struct bt_mesh_net_tx net_tx = {
        .sub = tx->sub,
        .ctx = &ctx,
        .src = tx->src,
        .xmit = tx->xmit,
        .aszmic = tx->aszmic,
        .aid = AID(&tx->hdr),
    };
    struct net_buf *seg = NULL;
    int err = 0;

    /* Check if all the segments are acknowledged. This could happen
     * when the complete Segment ACK (i.e. with all ack bits set) is
     * received before sending the next segment, which will cause the
     * corresponding "struct seg_tx" been reset.
     */
    if (all_seg_acked(tx, NULL)) {
        assert(tx->nack_count == 0 && "NACK count is not 0");
        return false;
    }

    assert(tx->sub && "NULL seg_tx sub");

    /* While the Segment Interval timer is active, the last_seg_n may
     * be updated(i.e. a Segment ACK is received), and it could just
     * be equal to seg_n, so we need to check that if tx->last_seg_n
     * is "<=" tx->seg_n here(i.e. not "<").
     */
    assert(tx->last_seg_n <= tx->seg_n && "Too large last_seg_n");

    /* After the Segment Interval timer expired, try to send the next
     * unacknowledged segment.
     * Note:
     * If the lsn_updated flag is 1, which means the last_seg_n is just
     * updated, we should sent the segment indicated by the last_seg_n.
     */
    for (size_t i = tx->last_seg_n + (tx->lsn_updated ? 0 : 1);
                i <= tx->seg_n; i++) {
        /* If tx->seg[i] not equals to NULL, which means the segment has
         * not been acknowledged.
         */
        if (tx->seg[i]) {
            tx->last_seg_n = i;
            seg = tx->seg[i];
            break;
        }
    }

    /* All the segments could be marked as acknowledged at this moment.
     * For example before the seg_timer expires(i.e. this function is
     * invoked), some segment ack is received from the BTU task, which
     * marks all the remaining segments as acknowledged.
     */
    if (seg == NULL) {
        BT_INFO("All segments acked, not send next segment");
        /* "seg_tx_complete" shall not be invoked here */
        return false;
    }

    /* The segment may have already been transmitted, for example, the
     * Segment Retransmission timer is expired earlier.
     */
    if (bt_mesh_atomic_get(&BLE_MESH_ADV_BUSY(seg))) {
        return false;
    }

    BT_INFO("Send next seg %u, cred %u", tx->last_seg_n, tx->cred);

    if (tx->resend) {
        err = bt_mesh_net_resend(tx->sub, seg, tx->new_key, &tx->cred,
                                 tx->tag, &seg_sent_cb, tx);
        if (err) {
            BT_ERR("Resend seg %u failed (err %d)", tx->last_seg_n, err);
            *result = -EIO;
            return true;
        }
        return false;
    }

    net_tx.ctx->net_idx = tx->sub->net_idx;

    /**
     * Add one to the ref count only if the segment can be further
     * processed by the network.
     */
    seg = net_buf_ref(seg);

    err = bt_mesh_net_send(&net_tx, seg, &seg_sent_cb, tx);
    if (err) {
        BT_ERR("Send seg %u failed (err %d)", tx->last_seg_n, err);
        *result = -EIO;
        return true;
    }

    /* If security credentials is updated in the network layer,
     * need to store the security credentials for the segments,
     * which will be used for retransmission later.
     */
    if (tx->cred != net_tx.ctx->send_cred) {
        BT_ERR("Mismatch seg cred %u/%u", tx->cred, net_tx.ctx->send_cred);
        *result = -EIO;
        return true;
    }

    return false;
}

static void send_next_seg(struct k_work *work)
{
    struct seg_tx *tx = CONTAINER_OF(work, struct seg_tx, seg_timer);
    bool tx_complete = false;
    int result = 0;

    bt_mesh_seg_tx_lock();
    tx_complete = send_next_segment(tx, &result);
    bt_mesh_seg_tx_unlock();

    if (tx_complete) {
        seg_tx_complete(tx, result);
    }
}

static void prepare_next_seg(struct seg_tx *tx)
{
    int32_t interval = 0;
    uint8_t seg_n = 0;
    uint8_t xmit = 0;

    /* Check if all the segments are acknowledged. This could happen
     * when the complete Segment ACK (i.e. with all ack bits set) is
     * received before the completion of sending last segment, which
     * will cause the corresponding "struct seg_tx" been reset.
     */
    if (all_seg_acked(tx, &seg_n)) {
        assert(tx->nack_count == 0 && "NACK count is not 0");
        return;
    }

    /* The last_seg_n must not be larger than the seg_n */
    assert(tx->last_seg_n <= tx->seg_n && "Too large last_seg_n");

    if (tx->last_seg_n < tx->seg_n) {
        /* Once an unacknowledged segment is transmitted, need to check
         * if it is the last segment marked as unacknowledged. If yes,
         * start the segments retransmission timer; otherwise start the
         * segment interval timer.
         */
        for (size_t i = tx->last_seg_n + 1; i <= tx->seg_n; i++) {
            /* Check if the segment is already been acknowledged */
            if (tx->seg[i] == NULL) {
                continue;
            }

            /* The duration of transmitting a segment will be decided by
             * the Network Transmit state. And if the segment interval
             * is smaller than the duration, which will cause the next
             * segment been posted to queue before the previous segment
             * is transmitted successfully, which is meaningless.
             * In case of this, just give a debug log here. By default,
             * we transmit a PDU for about 90ms, and the segment interval
             * is 60ms.
             */
            xmit = bt_mesh_net_transmit_get();

            if (bt_mesh_pdu_duration(xmit) > bt_mesh_seg_send_interval()) {
                BT_INFO("Segment interval should be at least %u (cur %u)",
                        bt_mesh_pdu_duration(xmit), bt_mesh_seg_send_interval());
            }

            interval = bt_mesh_seg_send_interval();

            BT_INFO("Send next segment %u after %dms", i, interval);

            k_delayed_work_submit(&tx->seg_timer, interval);
            return;
        }
    }

    /* If the first round of sending segments is finished, update the
     * resend flag to 1 here, because when a segment is retransmitted,
     * we need to decrypt it firstly.
     */
    if (tx->resend == 0) {
        tx->resend = 1;
    }

    /* Update the last_seg_n to the first unacknowledged SegN */
    tx->last_seg_n = seg_n;
    tx->lsn_updated = 1;

    /* Start the SAR retransmission timer */
    interval = bt_mesh_seg_rtx_interval(tx->dst, tx->ttl);

    BT_INFO("All segments sent, resend after %dms", interval);

    k_delayed_work_submit(&tx->rtx_timer, interval);
}

static void seg_send_start(uint16_t duration, int err, void *user_data)
{
    struct seg_tx *tx = user_data;

    /* If there's an error in transmitting the 'sent' callback will never
     * be called. Make sure that we kick the retransmit timer also in this
     * case since otherwise we risk the transmission of becoming stale.
     */
    if (err) {
        seg_tx_complete(tx, -EIO);
        return;
    }

    if (tx->resend == 0 && tx->last_seg_n == 0) {
        /* Start sending the multi-segment message */
        if (tx->cb && tx->cb->start) {
            tx->cb->start(duration, err, tx->cb_data);
        }
    }

    bt_mesh_seg_tx_lock();
    prepare_next_seg(tx);
    bt_mesh_seg_tx_unlock();
}

static void seg_send_end(int err, void *user_data)
{
    struct seg_tx *tx = user_data;

    if (err) {
        seg_tx_complete(tx, -EIO);
    }
}

static const struct bt_mesh_send_cb seg_sent_cb = {
    .start = seg_send_start,
    .end = seg_send_end,
};

static bool resend_unacked_seg(struct seg_tx *tx, int *result)
{
    struct net_buf *seg = NULL;
    int err = 0;

    /* Check if all the segments are acknowledged. This could happen
     * when the complete Segment ACK(i.e. with all ack bits set) is
     * received before the completion of sending last segment, which
     * will cause the corresponding "struct seg_tx" been reset.
     */
    if (all_seg_acked(tx, NULL)) {
        assert(tx->nack_count == 0 && "NACK count is not 0");
        return false;
    }

    /* Unacknowledged segments will be retransmitted when:
     * - Segment retransmission timer is expired;
     * - A Segment ACK is received, and "tx->nack_count" indicates
     *   that some segments of the message(to unicast address) are
     *   missing by the peer device.
     */

    if (BLE_MESH_ADDR_IS_UNICAST(tx->dst)) {
        /* When the SAR Unicast Retransmissions Timer expires and either
         * the remaining number of retransmissions or the remaining number
         * of retransmissions without progress is 0, the lower transport
         * layer shall cancel the transmission of the Upper Transport PDU,
         * and shall notify the upper transport layer that the transmission
         * of the Upper Transport PDU has timed out.
         */
        if (tx->surc == 0 || tx->surwpc == 0) {
            BT_WARN("Ran out of retransmission to 0x%04x (%u/%u)",
                    tx->dst, tx->surc, tx->surwpc);
            *result = -ETIMEDOUT;
            return true;
        }

        tx->surc -= 1;
        tx->surwpc -= 1;

        BT_INFO("Unicast Retransmission Count left: %u/%u", tx->surc, tx->surwpc);
    } else {
        if (tx->smrc == 0) {
            BT_INFO("Complete tx sdu to multicast 0x%04x", tx->dst);
            *result = 0;
            return true;
        }

        tx->smrc -= 1;

        BT_INFO("Multicast Retransmission Count left: %u", tx->smrc);
    }

    /* If the Segment Retransmission timer is expired before all the segments
     * are transmitted in the first round, we shall not resend the segments,
     * since during retransmission, we will decode and encode a segment, and
     * a segment will only be encoded after the first round.
     */
    if (tx->resend == 0) {
        BT_WARN("Segments in the first round not all sent");
        return false;
    }

    /* Get the first unacknowledged segment and retransmit it.
     * Note:
     * Try to find the first unacknowledged segment from "last_seg_n"
     * since it marks the first unacknowledged segment previously.
     * But before the rtx_timer expires(i.e. this function is invoked),
     * some segment ack could be received from the BTU task, which
     * marks some of the remaining segments as acknowledged. In this
     * case, we need to find the next unacknowledged segment.
     */
    for (size_t i = tx->last_seg_n; i <= tx->seg_n; i++) {
        if (tx->seg[i]) {
            tx->last_seg_n = i;
            seg = tx->seg[i];
            break;
        }
    }

    if (seg == NULL) {
        BT_INFO("All segments acked, not retransmit");
        /* "seg_tx_complete" shall not be invoked here */
        return false;
    }

    /* The "busy" flag of a segment could be 1 here for example:
     * Segment A is being transmitted because the Segment Interval timer
     * just expired. While during the previous segments transmission(i.e.,
     * segments before Segment A), a Segment ACK is received.
     * After handling the Segment ACK, the Segment Retransmission timer
     * is started. In the Segment Retransmission timeout handler, Segment
     * A is still going to be retransmitted, but at this moment we could
     * find that the "busy" flag of Segment A is 1.
     */
    if (bt_mesh_atomic_get(&BLE_MESH_ADV_BUSY(seg))) {
        return false;
    }

    BT_INFO("Resend seg %u, cred %u", tx->last_seg_n, tx->cred);

    /* TODO:
     * The "tx->new_key" should be replaced with sub->kr_flag,
     * since there is a chance that the key is refreshed during
     * the retransmission of segments.
     */
    err = bt_mesh_net_resend(tx->sub, seg, tx->new_key,
                             &tx->cred, tx->tag,
                             &seg_sent_cb, tx);
    if (err) {
        BT_ERR("Resend seg %u failed (err %d)", tx->last_seg_n, err);
        *result = -EIO;
        return true;
    }

    tx->lsn_updated = 0;

    return false;
}

static void seg_retransmit(struct k_work *work)
{
    struct seg_tx *tx = CONTAINER_OF(work, struct seg_tx, rtx_timer);
    bool tx_complete = false;
    int err = 0;

    bt_mesh_seg_tx_lock();
    tx_complete = resend_unacked_seg(tx, &err);
    bt_mesh_seg_tx_unlock();

    if (tx_complete) {
        seg_tx_complete(tx, err);
    }
}

static int send_seg(struct bt_mesh_net_tx *net_tx, struct net_buf_simple *sdu,
                    const struct bt_mesh_send_cb *cb, void *cb_data,
                    const uint8_t *ctl_op)
{
    struct seg_tx *tx = NULL;
    uint16_t seq_zero = 0U;
    uint8_t seg_o = 0U;
    int err = 0;
    size_t i;

    BT_DBG("src 0x%04x dst 0x%04x app_idx 0x%04x aszmic %u sdu_len %u",
           net_tx->src, net_tx->ctx->addr, net_tx->ctx->app_idx,
           net_tx->aszmic, sdu->len);

    for (i = 0; i < ARRAY_SIZE(seg_tx); i++) {
        if (seg_tx[i].nack_count) {
            /* The lower transport layer shall not transmit segmented messages
             * for more than one Upper Transport PDU to the same destination
             * at the same time.
             */
            if (seg_tx_blocks(&seg_tx[i], net_tx->src, net_tx->ctx->addr)) {
                BT_ERR("Multi-segment message to dst 0x%04x blocked", net_tx->ctx->addr);
                return -EBUSY;
            }
        }
    }

    for (tx = NULL, i = 0; i < ARRAY_SIZE(seg_tx); i++) {
        if (!seg_tx[i].nack_count) {
            tx = &seg_tx[i];
            break;
        }
    }

    if (!tx) {
        BT_ERR("No multi-segment message contexts available");
        return -EBUSY;
    }

    err = k_delayed_work_init(&tx->seg_timer, send_next_seg);
    if (err) {
        BT_ERR("No free seg_timer for sending multi-segment message");
        return -ENODEV;
    }

    err = k_delayed_work_init(&tx->rtx_timer, seg_retransmit);
    if (err) {
        BT_ERR("No free rtx_timer for sending multi-segment message");
        k_delayed_work_free(&tx->seg_timer);    /* Must do */
        return -ENODEV;
    }

    tx->sub = net_tx->sub;
    tx->seq_auth = SEQ_AUTH(BLE_MESH_NET_IVI_TX, bt_mesh.seq);
    if (ctl_op) {
        tx->hdr = TRANS_CTL_HDR(*ctl_op, 1);
    } else if (net_tx->ctx->app_idx == BLE_MESH_KEY_DEV) {
        tx->hdr = SEG_HDR(0, 0);
    } else {
        tx->hdr = SEG_HDR(1, net_tx->aid);
    }
    tx->src = net_tx->src;
    tx->dst = net_tx->ctx->addr;
    tx->app_idx = net_tx->ctx->app_idx;
    tx->xmit = net_tx->xmit;
    tx->aszmic = net_tx->aszmic;
    if (net_tx->ctx->send_ttl == BLE_MESH_TTL_DEFAULT) {
        tx->ttl = bt_mesh_default_ttl_get();
    } else {
        tx->ttl = net_tx->ctx->send_ttl;
    }
    tx->cred = net_tx->ctx->send_cred;
    tx->tag = net_tx->ctx->send_tag;
    tx->len = sdu->len;
    if (sdu->len) {
        tx->seg_n = (sdu->len - 1) / seg_len(!!ctl_op);
    } else {
        tx->seg_n = 0;
    }
    tx->last_seg_n = 0;
    tx->lsn_updated = 0;
    tx->nack_count = tx->seg_n + 1;
    if (BLE_MESH_ADDR_IS_UNICAST(tx->dst)) {
        /* When the lower transport layer starts a new transfer of an
         * Upper Transport PDU that is destined to a unicast address,
         * the lower transport layer shall set the remaining number
         * of retransmissions to the initial value and shall set the
         * remaining number of retransmissions without progress to the
         * initial value.
         */
        tx->surc = bt_mesh_get_sar_urc();
        tx->surwpc = bt_mesh_get_sar_urwpc();
    } else {
        /* When the lower transport layer starts a new transfer of an
         * Upper Transport PDU that is destined to a group address or
         * a virtual address, he lower transport layer shall set the
         * remaining number of retransmissions to the initial value.
         */
        tx->smrc = bt_mesh_get_sar_mrc();
    }
    tx->new_key = net_tx->sub->kr_flag;
    tx->ctl = !!ctl_op;
    tx->resend = 0;
    tx->cb = cb;
    tx->cb_data = cb_data;

    seq_zero = tx->seq_auth & TRANS_SEQ_ZERO_MASK;

    BT_DBG("SeqZero 0x%04x (segs: %u)", seq_zero, tx->nack_count);

    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND) &&
        !bt_mesh_friend_queue_has_space(tx->sub->net_idx, net_tx->src,
                                        tx->dst, &tx->seq_auth,
                                        tx->seg_n + 1) &&
        BLE_MESH_ADDR_IS_UNICAST(tx->dst)) {
        BT_ERR("Not enough space in Friend Queue for %u segments",
               tx->seg_n + 1);
        seg_tx_reset(tx);
        return -ENOBUFS;
    }

    for (seg_o = 0U; sdu->len; seg_o++) {
        struct net_buf *seg = NULL;
        uint16_t len = 0U;

        seg = bt_mesh_adv_create(BLE_MESH_ADV_DATA, BUF_TIMEOUT);
        if (!seg) {
            BT_ERR("Out of segment buffers");
            seg_tx_reset(tx);
            return -ENOBUFS;
        }

        net_buf_reserve(seg, BLE_MESH_NET_HDR_LEN);

        net_buf_add_u8(seg, tx->hdr);
        net_buf_add_u8(seg, (net_tx->aszmic << 7) | seq_zero >> 6);
        net_buf_add_u8(seg, (((seq_zero & 0x3f) << 2) | (seg_o >> 3)));
        net_buf_add_u8(seg, ((seg_o & 0x07) << 5) | tx->seg_n);

        len = MIN(sdu->len, seg_len(!!ctl_op));
        net_buf_add_mem(seg, net_buf_simple_pull_mem(sdu, len), len);

        if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND)) {
            enum bt_mesh_friend_pdu_type type = BLE_MESH_FRIEND_PDU_PARTIAL;

            if (seg_o == tx->seg_n) {
                type = BLE_MESH_FRIEND_PDU_COMPLETE;
            } else {
                type = BLE_MESH_FRIEND_PDU_PARTIAL;
            }

            if (bt_mesh_friend_enqueue_tx(net_tx, type,
                                          &tx->seq_auth,
                                          tx->seg_n + 1,
                                          &seg->b) &&
                BLE_MESH_ADDR_IS_UNICAST(net_tx->ctx->addr)) {
                /* PDUs for a specific Friend should only go
                 * out through the Friend Queue.
                 */
                net_buf_unref(seg);
                continue;
            }
        }

        /**
         * If the net buffer allocation of the subsequent
         * segments of this segment message fails, it will
         * cause the ref count of the previously allocated
         * successful segments to not be unref, which will
         * cause the net buffer leakage to occur, so it is
         * necessary to wait until all the segments have been
         * allocated, and then when the segment is confirmed
         * that it will be network layer for further processing,
         * then ref of the net buffer should be plus one.
         */
        tx->seg[seg_o] = seg;

        BT_DBG("Seg %u/%u prepared", seg_o, tx->seg_n);
    }

    /* If all the segments are enqueued in the friend queue, then the
     * tx->seg[0] will be NULL here.
     */
    if (tx->seg[0]) {
        /**
         * Add one to the ref count only if the segment can be further
         * processed by the network.
         */
        tx->seg[0] = net_buf_ref(tx->seg[0]);
        err = bt_mesh_net_send(net_tx, tx->seg[0], &seg_sent_cb, tx);
        if (err) {
            BT_ERR("Send 1st seg failed (err %d)", err);
            seg_tx_reset(tx);
            return err;
        }

        /* If security credentials is updated in the network layer,
         * need to store the security credentials for the segments,
         * which will be used for retransmission later.
         */
        if (tx->cred != net_tx->ctx->send_cred) {
            tx->cred = net_tx->ctx->send_cred;
        }
    }

    /* This can happen if segments only went into the Friend Queue */
    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND) && !tx->seg[0]) {
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

    if (msg->len < 1) {
        BT_ERR("Zero-length SDU not allowed");
        return -EINVAL;
    }

    if (msg->len > BLE_MESH_SDU_UNSEG_MAX) {
        tx->ctx->send_tag |= BLE_MESH_TAG_SEND_SEGMENTED;
    }

    BT_DBG("net_idx 0x%04x app_idx 0x%04x dst 0x%04x", tx->sub->net_idx,
           tx->ctx->app_idx, tx->ctx->addr);
    BT_DBG("len %u: %s", msg->len, bt_hex(msg->data, msg->len));

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

    BT_INFO("%s, send_tag 0x%02x, send_szmic %d, aszmic %d",
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
        BT_ERR("Encrypt failed (err %d)", err);
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
        BT_INFO("Revoke Device Key");

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

    BT_DBG("ASZMIC %u AKF %u AID 0x%02x", aszmic, AKF(&hdr), AID(&hdr));
    BT_DBG("len %u: %s", buf->len, bt_hex(buf->data, buf->len));

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
    sdu = bt_mesh_alloc_buf(CONFIG_BLE_MESH_RX_SDU_MAX - BLE_MESH_MIC_SHORT);
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
                BT_DBG("DevKey not found");
                continue;
            }

            net_buf_simple_reset(sdu);

            err = bt_mesh_app_decrypt(dev_key, true, aszmic, buf,
                                      sdu, ad, rx->ctx.addr,
                                      rx->ctx.recv_dst, seq,
                                      BLE_MESH_NET_IVI_RX(rx));
            if (err) {
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
            BT_DBG("AppKey not found");
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
            BT_DBG("Unable to decrypt with AppKey 0x%03x",
                   key->app_idx);
            continue;
        }

        rx->ctx.app_idx = key->app_idx;
        bt_mesh_model_recv(rx, sdu);

        bt_mesh_free_buf(sdu);
        return 0;
    }

    if (rx->local_match) {
        BT_WARN("No matching AppKey");
    }
    bt_mesh_free_buf(sdu);

    return 0;
}

static struct seg_tx *seg_tx_lookup(uint16_t seq_zero, uint8_t obo,
                                    uint16_t addr, uint16_t net_idx)
{
    struct seg_tx *tx = NULL;
    int i;

    for (i = 0; i < ARRAY_SIZE(seg_tx); i++) {
        tx = &seg_tx[i];

        if ((tx->seq_auth & TRANS_SEQ_ZERO_MASK) != seq_zero) {
            continue;
        }

        /* The message was secured using the same NetKey that was
         * used to secure the segmented message.
         */
        if (tx->sub == NULL || tx->sub->net_idx != net_idx) {
            continue;
        }

        if (tx->dst == addr) {
            return tx;
        }

        /* If the expected remote address doesn't match,
         * but the OBO flag is set and this is the first
         * acknowledgement, assume it's a Friend that's
         * responding and therefore accept the message.
         */
        if (obo && tx->nack_count == tx->seg_n + 1) {
            tx->dst = addr;
            return tx;
        }
    }

    return NULL;
}

static int recv_seg_ack(struct bt_mesh_net_rx *rx, uint8_t hdr,
                        struct net_buf_simple *buf, uint64_t *seq_auth,
                        struct seg_tx **seg_tx, bool *tx_complete,
                        int *result)
{
    struct seg_tx *tx = NULL;
    bool newly_mark = false;
    unsigned int bit = 0;
    uint32_t ack = 0U;
    uint16_t seq_zero = 0U;
    int32_t interval = 0;
    uint8_t obo = 0U;
    uint8_t seg_n = 0;

    *seg_tx = NULL;
    *tx_complete = false;
    *result = 0;

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

    BT_DBG("OBO %u seq_zero 0x%04x ack 0x%08x", obo, seq_zero, ack);

    tx = seg_tx_lookup(seq_zero, obo, rx->ctx.addr, rx->ctx.net_idx);
    if (!tx) {
        BT_INFO("No matching TX context for Seg Ack");
        return -EINVAL;
    }

    *seg_tx = tx;

    /* When the lower transport layer starts a new transfer of an Upper
     * Transport PDU that is destined to a group address or a virtual
     * address, the lower transport layer shall set the remaining number
     * of retransmissions to the initial value. Segment Acknowledgment
     * messages are not sent by the destination.
     */
    if (!BLE_MESH_ADDR_IS_UNICAST(tx->dst)) {
        BT_WARN("Received ack for segments to group");
        return -EINVAL;
    }

    *seq_auth = tx->seq_auth;

    if (ack == 0) {
        BT_WARN("SDU canceled");
        *tx_complete = true;
        *result = -ECANCELED;
        return 0;
    }

    if (find_msb_set(ack) - 1 > tx->seg_n) {
        BT_ERR("Too large segment number in ack");
        return -EINVAL;
    }

    k_delayed_work_cancel(&tx->rtx_timer);

    while ((bit = find_lsb_set(ack))) {
        if (tx->seg[bit - 1]) {
            BT_INFO("Seg %u/%u acked", bit - 1, tx->seg_n);

            seg_tx_done(tx, bit - 1);

            if (newly_mark == false) {
                newly_mark = true;
            }
        }

        ack &= ~BIT(bit - 1);
    }

    if (tx->nack_count == 0) {
        BT_INFO("SDU TX complete");
        *tx_complete = true;
        *result = 0;
        return 0;
    }

    /* If at least one segment is newly marked as acknowledged as
     * a result of receiving the Segment Acknowledgment message,
     * the lower transport layer shall set the remaining number of
     * retransmissions without progress to the initial value.
     */
    if (newly_mark) {
        tx->surwpc = bt_mesh_get_sar_urwpc();
    }

    if (tx->surc == 0 || tx->surwpc == 0) {
        BT_WARN("Ran out of retransmission to 0x%04x (%u/%u)",
                tx->dst, tx->surc, tx->surwpc);
        *tx_complete = true;
        *result = -ETIMEDOUT;
        return 0;
    }

    assert(all_seg_acked(tx, &seg_n) == false && "All segments acked");

    if (tx->resend == 1) {
        /* Only update the last_seg_n to the first unacked SegN while
         * the first round transmission is finished, because we need
         * the segment in the tx buffer been encrypted.
         * If the last_seg_n is updated here, it may cause the segments
         * been wrongly transmitted (i.e. not retransmitted) after the
         * last segment starts being transmitted.
         *
         * For example, 3 segments A, B and C exist.
         * - Segment A send start(i.e. seg_send_start)
         * - Segment A send end(i.e. seg_send_end)
         * - Segment B send start(i.e. seg_send_start)
         * - Segment B send end(i.e. seg_send_end)
         * - Segment ACK received
         * - The last_seg_n updated
         * - Segment C send start(i.e. seg_send_start)
         * - The "resend" flag is 0, start Segment Interval timer
         * - Segment Interval timer expired, send next segment
         * - Since the "resend" flag is 0, bt_mesh_net_send invoked
         * - "Insufficient MIC space for PDU" reported
         */
        tx->last_seg_n = seg_n;
        tx->lsn_updated = 1;
    }

    /* Restart the SAR Unicast Retransmission timer */
    interval = bt_mesh_seg_rtx_interval(tx->dst, tx->ttl);

    BT_INFO("Resend segments after %dms", interval);

    k_delayed_work_submit(&tx->rtx_timer, interval);

    return 0;
}

static int trans_ack(struct bt_mesh_net_rx *rx, uint8_t hdr,
                     struct net_buf_simple *buf, uint64_t *seq_auth)
{
    struct seg_tx *tx = NULL;
    bool tx_complete = false;
    int result = 0;
    int err = 0;

    bt_mesh_seg_tx_lock();
    err = recv_seg_ack(rx, hdr, buf, seq_auth, &tx, &tx_complete, &result);
    bt_mesh_seg_tx_unlock();

    if (tx_complete) {
        seg_tx_complete(tx, result);
    }

    return err;
}

static int trans_heartbeat(struct bt_mesh_net_rx *rx,
                           struct net_buf_simple *buf)
{
    uint8_t init_ttl = 0U, hops = 0U;
    uint16_t feat = 0U;

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

    BT_INFO("src 0x%04x TTL %u InitTTL %u (%u hop%s) feat 0x%04x",
            rx->ctx.addr, rx->ctx.recv_ttl, init_ttl, hops,
            (hops == 1U) ? "" : "s", feat);

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

    BT_DBG("OpCode 0x%02x len %u", ctl_op, buf->len);

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
        return 0;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_DF_SRV)) {
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
    }

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

    BT_DBG("AFK %u AID 0x%02x", AKF(buf->data), AID(buf->data));

    if (buf->len < 1) {
        BT_ERR("Too small unsegmented PDU");
        return -EINVAL;
    }

    if (bt_mesh_rpl_check(rx, NULL)) {
        BT_WARN("Replay: src 0x%04x dst 0x%04x seq 0x%06x",
                rx->ctx.addr, rx->ctx.recv_dst, rx->seq);
        return -EINVAL;
    }

    hdr = net_buf_simple_pull_u8(buf);

    if (rx->ctl) {
        return ctl_recv(rx, hdr, buf, seq_auth);
    }

    /* SDUs must match a local element or an LPN of this Friend. */
    if (!rx->local_match && !rx->friend_match) {
        return 0;
    }

    return sdu_recv(rx, rx->seq, hdr, 0, buf);
}

int bt_mesh_ctl_send(struct bt_mesh_net_tx *tx, uint8_t ctl_op, void *data,
                     size_t data_len, const struct bt_mesh_send_cb *cb,
                     void *cb_data)
{
    struct net_buf_simple buf = {0};

    net_buf_simple_init_with_data(&buf, data, data_len);

    if (data_len > BLE_MESH_SDU_UNSEG_MAX) {
        tx->ctx->send_tag |= BLE_MESH_TAG_SEND_SEGMENTED;
    }

    /* Set app_idx to unused here since CTL is only encrypted with NetKey */
    tx->ctx->app_idx = BLE_MESH_KEY_UNUSED;

    BT_DBG("src 0x%04x dst 0x%04x ttl 0x%02x ctl 0x%02x", tx->src,
            tx->ctx->addr, tx->ctx->send_ttl, ctl_op);
    BT_DBG("len %zu: %s", data_len, bt_hex(data, data_len));

    if (bt_mesh_tag_send_segmented(tx->ctx->send_tag)) {
        return send_seg(tx, &buf, cb, cb_data, &ctl_op);
    }

    return send_unseg(tx, &buf, cb, cb_data, &ctl_op);
}

static void seg_ack_send_start(uint16_t duration, int err, void *user_data)
{
    struct seg_rx *rx = user_data;

    BT_INFO("Send segment ack start (err %d)", err);

    if (err) {
        rx->last_ack = k_uptime_get_32();
    }
}

static void seg_ack_send_end(int err, void *user_data)
{
    struct seg_rx *rx = user_data;

    BT_INFO("Send segment ack end");

    /* This could happen when during the Segment ACK transaction,
     * the seg_rx is been reset.
     */
    if (rx->in_use == 0) {
        return;
    }

    rx->last_ack = k_uptime_get_32();

    /* If the seg_rx is in use, we will restart the SAR ACK timer if
     * the SegN is greater than the SAR Segments Threshold.
     * Note:
     * Check the "in_use" flag here in case the seg_rx has been reset,
     * in this case, the SAR ACK timer will not be restarted.
     *
     * Check the "new_seg" flag here in case a seg is received during
     * the Segment ACK transaction, which will cause the SAR ACK timer
     * been started with the initial value. In this case, we shall not
     * restart the Segment ACK timer here.
     */
    if (rx->seg_n > bt_mesh_get_sar_st() &&
        rx->sarc &&
        rx->new_seg == 0) {
        /* Decrement the SAR ACK Retransmissions Count */
        rx->sarc -= 1;

        BT_INFO("Resend segment ack after %dms", bt_mesh_seg_rx_interval());

        /* Introduce a delay for the Segment ACK retransmission */
        k_delayed_work_submit(&rx->ack_timer, bt_mesh_seg_rx_interval());
    }
}

static const struct bt_mesh_send_cb seg_ack_sent_cb = {
    .start = seg_ack_send_start,
    .end = seg_ack_send_end,
};

/* The Segment Acknowledgment message shall use the same NetKey as
 * the first received segment of a multi-segment message, and the
 * DST field shall have the same value as the SRC field of the first
 * received segment of a multi-segment message.
 */
static int send_ack(struct bt_mesh_subnet *sub, uint16_t src, uint16_t dst,
                    uint8_t ttl, uint64_t *seq_auth, uint32_t block, uint8_t obo,
                    struct seg_rx *rx)
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

    BT_DBG("SeqZero 0x%04x Block 0x%08x OBO %u", seq_zero, block, obo);

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
                            rx ? &seg_ack_sent_cb : NULL, rx);
}

static void seg_rx_reset(struct seg_rx *rx, bool full_reset)
{
    bt_mesh_seg_rx_lock();

    k_delayed_work_free(&rx->dis_timer);
    if (BLE_MESH_ADDR_IS_UNICAST(rx->dst)) {
        k_delayed_work_free(&rx->ack_timer);
    }

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
        rx->seg_n = 0;
        rx->ctl = 0;
        rx->obo = 0;
        rx->sub = NULL;
        rx->ttl = 0;
        rx->src = BLE_MESH_ADDR_UNASSIGNED;
        rx->dst = BLE_MESH_ADDR_UNASSIGNED;
        rx->block = 0;
        rx->last_ack = 0;
    }

    bt_mesh_seg_rx_unlock();
}

static void send_seg_ack(struct k_work *work)
{
    struct seg_rx *rx = CONTAINER_OF(work, struct seg_rx, ack_timer);

    bt_mesh_seg_rx_lock();

    /* This could happen when the SAR ACK timer expired, and a BTC
     * event is posted to the BTC queue.
     * But before the BTC event been handled, the seg_rx is fully
     * reset, we must return here, since the rx->sub will be NULL.
     */
    if (rx->sub == NULL) {
        goto end;
    }

    /* Even if the seg_rx is reset(but not fully), we will continue
     * sending the Segment ACK.
     */
    send_ack(rx->sub, rx->dst, rx->src, rx->ttl, &rx->seq_auth,
             rx->block, rx->obo, rx);

    rx->new_seg = 0;

end:
    bt_mesh_seg_rx_unlock();
}

static void discard_msg(struct k_work *work)
{
    struct seg_rx *rx = CONTAINER_OF(work, struct seg_rx, dis_timer);

    /* This could happen when the SAR Discard timer expired, and a
     * BTC event is posted to the BTC queue.
     * But before the BTC event been handled, the last segment is
     * received from the BTU task(higher priority than the BTC task).
     * In this case, the segmented message will be marked as fully
     * received, and we ignore the discard timeout here.
     */
    if (rx->in_use == 0) {
        return;
    }

    /* Stop SAR ACK timer when SAR Discard timer expires */
    if (BLE_MESH_ADDR_IS_UNICAST(rx->dst)) {
        k_delayed_work_cancel(&rx->ack_timer);
    }

    BT_WARN("Discard timer expired (%dms)", bt_mesh_seg_discard_timeout());

    /* Not fully reset the seg_rx, in case any segment of this
     * message is received later.
     */
    seg_rx_reset(rx, false);
}

static inline bool sdu_len_is_ok(bool ctl, uint8_t seg_n)
{
    return ((seg_n + 1) * seg_len(ctl) <= CONFIG_BLE_MESH_RX_SDU_MAX);
}

static void seg_rx_reset_pending(struct bt_mesh_net_rx *net_rx,
                                 const uint64_t *seq_auth)
{
    for (size_t i = 0; i < ARRAY_SIZE(seg_rx); i++) {
        struct seg_rx *rx = &seg_rx[i];

        if (rx->src == net_rx->ctx.addr &&
            rx->dst == net_rx->ctx.recv_dst &&
            rx->seq_auth < *seq_auth &&
            rx->in_use) {
            BT_WARN("Discard pending reassembly, src 0x%04x dst 0x%04x",
                    net_rx->ctx.addr, net_rx->ctx.recv_dst);

            /* In this case, fully reset the seg_rx */
            seg_rx_reset(rx, true);
        }
    }
}

static struct seg_rx *seg_rx_find(struct bt_mesh_net_rx *net_rx,
                                  const uint64_t *seq_auth)
{
    for (size_t i = 0; i < ARRAY_SIZE(seg_rx); i++) {
        struct seg_rx *rx = &seg_rx[i];

        if (rx->src == net_rx->ctx.addr &&
            rx->dst == net_rx->ctx.recv_dst &&
            rx->seq_auth >= *seq_auth) {
            return rx;
        }
    }

    return NULL;
}

static bool seg_rx_is_valid(struct seg_rx *rx, struct bt_mesh_net_rx *net_rx,
                            const uint8_t *hdr, uint8_t seg_n)
{
    if (rx->hdr != *hdr || rx->seg_n != seg_n) {
        BT_ERR("Invalid segment for ongoing session");
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
    int err = 0;

    for (size_t i = 0; i < ARRAY_SIZE(seg_rx); i++) {
        struct seg_rx *rx = &seg_rx[i];

        if (rx->in_use) {
            continue;
        }

        err = k_delayed_work_init(&rx->dis_timer, discard_msg);
        if (err) {
            BT_ERR("No free dis_timer for new incoming segmented message");
            return NULL;
        }

        if (BLE_MESH_ADDR_IS_UNICAST(net_rx->ctx.recv_dst)) {
            err = k_delayed_work_init(&rx->ack_timer, send_seg_ack);
            if (err) {
                BT_ERR("No free ack_timer for new incoming segmented message");
                k_delayed_work_free(&rx->dis_timer);    /* Must do */
                return NULL;
            }
        }

        rx->in_use = 1U;
        net_buf_simple_reset(&rx->buf);
        rx->sub = net_rx->sub;
        rx->ctl = net_rx->ctl;
        rx->seq_auth = *seq_auth;
        rx->seg_n = seg_n;
        rx->hdr = *hdr;
        rx->ttl = net_rx->ctx.send_ttl;
        rx->src = net_rx->ctx.addr;
        rx->dst = net_rx->ctx.recv_dst;
        rx->block = 0U;
        rx->last_ack = 0;

        BT_DBG("New RX context. Block Complete 0x%08x",
               BLOCK_COMPLETE(seg_n));

        return rx;
    }

    BT_WARN("No free slots for new incoming segmented messages");
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

    if (buf->len < 5) {
        BT_ERR("Too short segmented message (len %u)", buf->len);
        return -EINVAL;
    }

    if (bt_mesh_rpl_check(net_rx, &rpl)) {
        BT_WARN("Replay: src 0x%04x dst 0x%04x seq 0x%06x",
                net_rx->ctx.addr, net_rx->ctx.recv_dst, net_rx->seq);
        return -EINVAL;
    }

    BT_DBG("ASZMIC %u AKF %u AID 0x%02x", ASZMIC(hdr), AKF(hdr), AID(hdr));

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

    /* SEQ(0) = SEQ(n) - (delta between seqZero and SEQ(n) by looking
     * into 14 least significant bits of SEQ(n))
     */
    *seq_auth = SEQ_AUTH(BLE_MESH_NET_IVI_RX(net_rx),
                         (net_rx->seq -
                          (((net_rx->seq & BIT_MASK(14)) - seq_zero) & BIT_MASK(13))));

    *seg_count = seg_n + 1;

    /* If this is the first segment, check if any pending reassembly
     * exists. If yes, we need to discard the pending reassembly.
     * Note:
     * The first segment may not have SeqO with 0 here, because the
     * segment with SegO = 0 may be missed and SegO = 1 is received.
     */
    seg_rx_reset_pending(net_rx, seq_auth);

    /* Look for old RX sessions */
    rx = seg_rx_find(net_rx, seq_auth);
    if (rx) {
        /* Processing result is SeqAuth Error, ignore the segment */
        if (rx->seq_auth > *seq_auth) {
            BT_WARN("Ignoring old SeqAuth, src 0x%04x, dst 0x%04x",
                    rx->src, rx->dst);
            return -EINVAL;
        }

        if (!seg_rx_is_valid(rx, net_rx, hdr, seg_n)) {
            return -EINVAL;
        }

        if (rx->in_use) {
            BT_DBG("Existing RX context. Block 0x%08x", rx->block);
            goto found_rx;
        }

        /* If a segmented message is received successfully, the "in_use"
         * flag will be set to 0, but the other information, e.g. src,
         * dst, seq_auth will not be reset. These information could be
         * used to check if any segment of an already received message
         * is received again. In this case, the processing result will
         * be Most Recent SeqAuth, and a Segment ACK will be sent.
         */
        if (rx->block == BLOCK_COMPLETE(rx->seg_n)) {
            BT_INFO("Got segment for already complete SDU");

            /* Make sure not sending more than one Segment ACK for the
             * same SeqAuth in a period of:
             * [ack_delay_increment * seg_reception_interval] ms
             */
            if (k_uptime_get_32() - rx->last_ack >= bt_mesh_seg_ack_period()) {
                send_ack(net_rx->sub, net_rx->ctx.recv_dst,
                         net_rx->ctx.addr, net_rx->ctx.send_ttl,
                         seq_auth, rx->block, rx->obo, rx);
            }

            if (rpl) {
                bt_mesh_update_rpl(rpl, net_rx);
            }

            return -EALREADY;
        }

        /* The "in_use" flag is 0, which means the seg_rx has been reset,
         * but not all the segments are received. Ignore this segment.
         */
        BT_WARN("Got segment for canceled SDU");
        return -EINVAL;
    }

    /* Bail out early if we're not ready to receive such a large SDU */
    if (!sdu_len_is_ok(net_rx->ctl, seg_n)) {
        BT_ERR("Too big incoming SDU length");
        send_ack(net_rx->sub, net_rx->ctx.recv_dst, net_rx->ctx.addr,
                 net_rx->ctx.send_ttl, seq_auth, 0,
                 net_rx->friend_match, NULL);
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
        BT_ERR("No space in Friend Queue for %u segments", *seg_count);
        send_ack(net_rx->sub, net_rx->ctx.recv_dst, net_rx->ctx.addr,
                 net_rx->ctx.send_ttl, seq_auth, 0,
                 net_rx->friend_match, NULL);
        return -ENOBUFS;
    }

    /* Look for free slot for a new RX session */
    rx = seg_rx_alloc(net_rx, hdr, seq_auth, seg_n);
    if (!rx) {
        /* Processing result is Message Rejected, respond with a Segment
         * ACK with the AckedSegments field set to 0x00000000.
         */
        if (BLE_MESH_ADDR_IS_UNICAST(net_rx->ctx.recv_dst)) {
            send_ack(net_rx->sub, net_rx->ctx.recv_dst, net_rx->ctx.addr,
                     net_rx->ctx.send_ttl, seq_auth, 0,
                     net_rx->friend_match, NULL);
        }
        return -ENOMEM;
    }

    rx->obo = net_rx->friend_match;

found_rx:
    /* Processing result is Repeated Segment, ignore the segment */
    if (BIT(seg_o) & rx->block) {
        BT_INFO("Received already received fragment");
        return -EALREADY;
    }

    /* All segments, except the last one, must either have 8 bytes of
     * payload (for 64bit Net MIC) or 12 bytes of payload (for 32bit
     * Net MIC).
     * Processing result is Last Segment.
     */
    if (seg_o == seg_n) {
        /* Set the expected final buffer length */
        rx->buf.len = seg_n * seg_len(rx->ctl) + buf->len;

        BT_DBG("Target len %u * %u + %u = %u", seg_n, seg_len(rx->ctl),
               buf->len, rx->buf.len);

        /* This should not happen, since we have made sure the whole
         * SDU could be received while handling the first segment.
         * But if the peer device sends the segments of a segmented
         * message with different CTL, then the following could happen.
         */
        if (rx->buf.len > CONFIG_BLE_MESH_RX_SDU_MAX) {
            BT_ERR("Too large SDU len %u/%u", rx->buf.len,
                    CONFIG_BLE_MESH_RX_SDU_MAX);

            send_ack(net_rx->sub, net_rx->ctx.recv_dst,
                     net_rx->ctx.addr, net_rx->ctx.send_ttl,
                     seq_auth, 0, rx->obo, NULL);
            seg_rx_reset(rx, true);

            return -EMSGSIZE;
        }
    } else {
        if (buf->len != seg_len(rx->ctl)) {
            BT_ERR("Incorrect segment size for message type");
            return -EINVAL;
        }
    }

    /* Location in buffer can be calculated based on seg_o & rx->ctl */
    memcpy(rx->buf.data + (seg_o * seg_len(rx->ctl)), buf->data, buf->len);

    BT_INFO("Seg %u/%u received", seg_o, seg_n);

    /* Restart the timer with no need to check if its remaining time is 0 */
    if (!bt_mesh_lpn_established()) {
        /* Start SAR Discard timer when processing result is First Segment
         * or Next Segment.
         */
        k_delayed_work_submit(&rx->dis_timer, bt_mesh_seg_discard_timeout());

        /* Start SAR ACK timer when processing result is First Segment
         * or Next Segment.
         */
        if (BLE_MESH_ADDR_IS_UNICAST(rx->dst)) {
            /* Update the SAR ACK Retransmissions Count in case the SegN
             * is greater than the SAR Segments Threshold.
             */
            rx->sarc = (seg_n > bt_mesh_get_sar_st() ? bt_mesh_get_sar_arc() : 0);
            rx->new_seg = 1;

            BT_INFO("Send segment ack after %dms", bt_mesh_seg_ack_timeout(seg_n));

            k_delayed_work_submit(&rx->ack_timer, bt_mesh_seg_ack_timeout(seg_n));
        }
    }

    if (rpl) {
        bt_mesh_update_rpl(rpl, net_rx);
    }

    /* Mark segment as received */
    rx->block |= BIT(seg_o);

    if (rx->block != BLOCK_COMPLETE(seg_n)) {
        *pdu_type = BLE_MESH_FRIEND_PDU_PARTIAL;
        return 0;
    }

    BT_DBG("Complete SDU");

    *pdu_type = BLE_MESH_FRIEND_PDU_COMPLETE;

    /* Stop SAR Discard timer when processing result is Last Segment */
    k_delayed_work_cancel(&rx->dis_timer);

    /* Stop SAR ACK timer when processing result is Last Segment */
    if (BLE_MESH_ADDR_IS_UNICAST(rx->dst)) {
        k_delayed_work_cancel(&rx->ack_timer);
    }

    send_ack(net_rx->sub, net_rx->ctx.recv_dst,
             net_rx->ctx.addr, net_rx->ctx.send_ttl,
             seq_auth, rx->block, rx->obo, rx);

    if (net_rx->ctl) {
        err = ctl_recv(net_rx, *hdr, &rx->buf, seq_auth);
    } else {
        err = sdu_recv(net_rx, (rx->seq_auth & 0xffffff), *hdr,
                       ASZMIC(hdr), &rx->buf);
    }

    seg_rx_reset(rx, false);

    return err;
}

int bt_mesh_trans_recv(struct net_buf_simple *buf, struct bt_mesh_net_rx *rx)
{
    uint64_t seq_auth = TRANS_SEQ_AUTH_NVAL;
    enum bt_mesh_friend_pdu_type pdu_type = BLE_MESH_FRIEND_PDU_SINGLE;
    struct net_buf_simple_state state = {0};
    uint8_t seg_count = 0U;
    int err = 0;

    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND)) {
        rx->friend_match = bt_mesh_friend_match(rx->sub->net_idx,
                                                rx->ctx.recv_dst);
    } else {
        rx->friend_match = false;
    }

    BT_DBG("src 0x%04x dst 0x%04x seq 0x%08x friend_match %u",
           rx->ctx.addr, rx->ctx.recv_dst, rx->seq, rx->friend_match);

    /* Remove network headers */
    net_buf_simple_pull(buf, BLE_MESH_NET_HDR_LEN);

    BT_DBG("Payload %s", bt_hex(buf->data, buf->len));

    /* If LPN mode is enabled messages are only accepted when we've
     * requested the Friend to send them. The messages must also
     * be encrypted using the Friend Credentials.
     */
    if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER) &&
        bt_mesh_lpn_established() && rx->net_if == BLE_MESH_NET_IF_ADV &&
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
            return 0;
        }

        err = trans_seg(buf, rx, &pdu_type, &seq_auth, &seg_count);
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
    for (size_t i = 0; i < ARRAY_SIZE(seg_rx); i++) {
        seg_rx_reset(&seg_rx[i], true);
    }
}

void bt_mesh_tx_reset(void)
{
    for (size_t i = 0; i < ARRAY_SIZE(seg_tx); i++) {
        seg_tx_reset(&seg_tx[i]);
    }
}

void bt_mesh_rx_reset_single(uint16_t src)
{
    if (!BLE_MESH_ADDR_IS_UNICAST(src)) {
        return;
    }

    for (size_t i = 0; i < ARRAY_SIZE(seg_rx); i++) {
        struct seg_rx *rx = &seg_rx[i];
        if (src == rx->src) {
            seg_rx_reset(rx, true);
        }
    }
}

void bt_mesh_tx_reset_single(uint16_t dst)
{
    if (!BLE_MESH_ADDR_IS_UNICAST(dst)) {
        return;
    }

    for (size_t i = 0; i < ARRAY_SIZE(seg_tx); i++) {
        struct seg_tx *tx = &seg_tx[i];
        if (dst == tx->dst) {
            seg_tx_reset(tx);
        }
    }
}

void bt_mesh_trans_init(void)
{
    bt_mesh_sar_init();

    for (size_t i = 0; i < ARRAY_SIZE(seg_rx); i++) {
        seg_rx[i].buf.__buf = (seg_rx_buf_data + (i * CONFIG_BLE_MESH_RX_SDU_MAX));
        seg_rx[i].buf.data = seg_rx[i].buf.__buf;
    }

    bt_mesh_r_mutex_create(&seg_tx_lock);
    bt_mesh_r_mutex_create(&seg_rx_lock);
}

#if CONFIG_BLE_MESH_DEINIT
void bt_mesh_trans_deinit(bool erase)
{
    bt_mesh_rx_reset();
    bt_mesh_tx_reset();
    bt_mesh_rpl_reset(erase);

    bt_mesh_r_mutex_free(&seg_tx_lock);
    bt_mesh_r_mutex_free(&seg_rx_lock);
}
#endif /* CONFIG_BLE_MESH_DEINIT */
