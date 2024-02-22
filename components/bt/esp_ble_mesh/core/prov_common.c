/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>

#include "adv.h"
#include "crypto.h"
#include "mesh/mutex.h"
#include "mesh/common.h"
#include "mesh/access.h"
#include "prov_common.h"

static const struct bt_mesh_prov *prov;

const struct bt_mesh_prov *bt_mesh_prov_get(void)
{
    return prov;
}

int bt_mesh_prov_set(const struct bt_mesh_prov *val)
{
    prov = val;
    return 0;
}

void bt_mesh_prov_buf_init(struct net_buf_simple *buf, uint8_t type)
{
    net_buf_simple_reserve(buf, PROV_BUF_HEADROOM);
    net_buf_simple_add_u8(buf, type);
}

#define OUTPUT_OOB_BLINK    0x00
#define OUTPUT_OOB_BEEP     0x01
#define OUTPUT_OOB_VIBRATE  0x02
#define OUTPUT_OOB_NUMBER   0x03
#define OUTPUT_OOB_STRING   0x04

#define INPUT_OOB_PUSH      0x00
#define INPUT_OOB_TWIST     0x01
#define INPUT_OOB_NUMBER    0x02
#define INPUT_OOB_STRING    0x03

bt_mesh_output_action_t bt_mesh_prov_output_action(uint8_t action)
{
    switch (action) {
    case OUTPUT_OOB_BLINK:
        return BLE_MESH_BLINK;
    case OUTPUT_OOB_BEEP:
        return BLE_MESH_BEEP;
    case OUTPUT_OOB_VIBRATE:
        return BLE_MESH_VIBRATE;
    case OUTPUT_OOB_NUMBER:
        return BLE_MESH_DISPLAY_NUMBER;
    case OUTPUT_OOB_STRING:
        return BLE_MESH_DISPLAY_STRING;
    default:
        return BLE_MESH_NO_OUTPUT;
    }
}

bt_mesh_input_action_t bt_mesh_prov_input_action(uint8_t action)
{
    switch (action) {
    case INPUT_OOB_PUSH:
        return BLE_MESH_PUSH;
    case INPUT_OOB_TWIST:
        return BLE_MESH_TWIST;
    case INPUT_OOB_NUMBER:
        return BLE_MESH_ENTER_NUMBER;
    case INPUT_OOB_STRING:
        return BLE_MESH_ENTER_STRING;
    default:
        return BLE_MESH_NO_INPUT;
    }
}

static const struct {
    uint16_t length;
} prov_pdu[] = {
    { 1  }, /* Provisioning Invite */
    { 11 }, /* Provisioning Capabilities */
    { 5  }, /* Provisioning Start */
    { 64 }, /* Provisioning Public Key */
    { 0  }, /* Provisioning Input Complete */
    { 16 }, /* Provisioning Confirmation */
    { 16 }, /* Provisioning Random */
    { 33 }, /* Provisioning Data */
    { 0  }, /* Provisioning Complete */
    { 1  }, /* Provisioning Failed */
    { 6  }, /* Provisioning Record Request */
    { 7  }, /* Provisioning Record Response */
    { 0  }, /* Provisioning Records Get */
    { 2  }, /* Provisioning Records List */
};

bool bt_mesh_prov_pdu_check(uint8_t type, uint16_t length, uint8_t *reason)
{
    if (prov_pdu[type].length != length) {
#if CONFIG_BLE_MESH_CERT_BASED_PROV
        if ((type == PROV_REC_LIST || type == PROV_REC_RSP) &&
            length >= prov_pdu[type].length) {
            return true;
        }
#endif

#if CONFIG_BLE_MESH_PROV_EPA
        /* NOTE: PROV_CONFIRM and PROV_RANDOM PDU may have length 16 or 32 */
        if ((type == PROV_CONFIRM || type == PROV_RANDOM) && length == 32) {
            return true;
        }
#endif

        BT_ERR("Invalid length %u for type 0x%02x", length, type);
        if (reason) {
            *reason = PROV_ERR_NVAL_FMT;
        }
        return false;
    }

    return true;
}

#if CONFIG_BLE_MESH_PB_ADV

/* 3 transmissions, 20ms interval */
#define PROV_XMIT       BLE_MESH_TRANSMIT(2, 20)

/* 3 transmissions, 20ms interval */
#define CLOSE_XMIT      BLE_MESH_TRANSMIT(2, 20)

#define CLOSE_TIMEOUT   K_MSEC(100)

#define BUF_TIMEOUT     K_MSEC(400)

#define XACT_SEG_DATA(link, _seg)   (&link->rx.buf->data[20 + (((_seg) - 1) * 23)])
#define XACT_SEG_RECV(link, _seg)   (link->rx.seg &= ~(1 << (_seg)))

static uint8_t bt_mesh_prov_buf_type_get(struct net_buf_simple *buf)
{
    return buf->data[PROV_BUF_HEADROOM];
}

uint8_t node_next_xact_id(struct bt_mesh_prov_link *link)
{
    if (link->tx.id != 0 && link->tx.id != 0xFF) {
        return ++link->tx.id;
    }

    link->tx.id = 0x80;
    return link->tx.id;
}

uint8_t pvnr_next_xact_id(struct bt_mesh_prov_link *link)
{
    if (link->tx.id > 0x7F) {
        link->tx.id = 0;
    }
    return link->tx.id++;
}

bool bt_mesh_gen_prov_start(struct bt_mesh_prov_link *link,
                            struct net_buf_simple *buf,
                            struct prov_rx *rx, bool *close)
{
    if (link->rx.seg) {
        BT_INFO("Get Start while there are unreceived segments");
        return false;
    }

    if (link->rx.prev_id == rx->xact_id) {
        BT_INFO("Resending ack");
        bt_mesh_gen_prov_ack_send(link, rx->xact_id);
        return false;
    }

    link->rx.buf->len = net_buf_simple_pull_be16(buf);
    link->rx.id = rx->xact_id;
    link->rx.fcs = net_buf_simple_pull_u8(buf);

    BT_DBG("len %u last_seg %u total_len %u fcs 0x%02x", buf->len,
            START_LAST_SEG(rx->gpc), link->rx.buf->len, link->rx.fcs);

    /* At least one-octet pdu type is needed */
    if (link->rx.buf->len < 1) {
        BT_ERR("Ignoring zero-length provisioning PDU");
        if (close) {
            *close = true;
        }
        return false;
    }

    if (START_LAST_SEG(rx->gpc) > START_LAST_SEG_MAX) {
        BT_ERR("Invalid SegN 0x%02x", START_LAST_SEG(rx->gpc));
        if (close) {
            *close = true;
        }
        return false;
    }

    if (link->rx.buf->len > link->rx.buf->size) {
        BT_ERR("Too large provisioning PDU (%u bytes)",
                link->rx.buf->len);
        if (close) {
            *close = true;
        }
        return false;
    }

    if (START_LAST_SEG(rx->gpc) > 0 && link->rx.buf->len <= 20) {
        BT_ERR("Too small total length for multi-segment PDU");
        if (close) {
            *close = true;
        }
        return false;
    }

    link->rx.seg = (1 << (START_LAST_SEG(rx->gpc) + 1)) - 1;
    link->rx.last_seg = START_LAST_SEG(rx->gpc);
    memcpy(link->rx.buf->data, buf->data, buf->len);
    XACT_SEG_RECV(link, 0);

    /* Still have some segments to receive */
    if (link->rx.seg) {
        return false;
    }

    return true;
}

bool bt_mesh_gen_prov_cont(struct bt_mesh_prov_link *link,
                           struct net_buf_simple *buf,
                           struct prov_rx *rx, bool *close)
{
    uint8_t seg = CONT_SEG_INDEX(rx->gpc);

    BT_DBG("len %u, seg_index %u", buf->len, seg);

    if (link->rx.seg == 0 && link->rx.prev_id == rx->xact_id) {
        BT_INFO("Resending ack");
        bt_mesh_gen_prov_ack_send(link, rx->xact_id);
        return false;
    }

    if (rx->xact_id != link->rx.id) {
        BT_WARN("Data for unknown transaction (%u != %u)",
                rx->xact_id, link->rx.id);
        return false;
    }

    if (seg > link->rx.last_seg) {
        BT_ERR("Invalid segment index %u", seg);
        if (close) {
            *close = true;
        }
        return false;
    }

    if (seg == link->rx.last_seg) {
        uint8_t expect_len = (link->rx.buf->len - 20 -
                              (23 * (link->rx.last_seg - 1)));
        if (expect_len != buf->len) {
            BT_ERR("Incorrect last seg len: %u != %u",
                    expect_len, buf->len);
            if (close) {
                *close = true;
            }
            return false;
        }
    }

    if ((link->rx.seg & BIT(seg)) == 0) {
        BT_INFO("Ignore already received segment");
        return false;
    }

    memcpy(XACT_SEG_DATA(link, seg), buf->data, buf->len);
    XACT_SEG_RECV(link, seg);

    /* Still have some segments to receive */
    if (link->rx.seg) {
        return false;
    }

    return true;
}

static struct net_buf *adv_buf_create(void)
{
    struct net_buf *buf = NULL;

    buf = bt_mesh_adv_create(BLE_MESH_ADV_PROV, BUF_TIMEOUT);
    if (!buf) {
        BT_ERR("Out of provisioning buffers");
        return NULL;
    }

    return buf;
}

static void ack_complete(uint16_t duration, int err, void *user_data)
{
    struct bt_mesh_prov_link *link = user_data;

    BT_DBG("xact %u complete", link->pending_ack);

    link->pending_ack = PROV_XACT_NVAL;
}

void bt_mesh_gen_prov_ack_send(struct bt_mesh_prov_link *link, uint8_t xact_id)
{
    static const struct bt_mesh_send_cb cb = {
        .start = ack_complete,
    };
    const struct bt_mesh_send_cb *complete = NULL;
    struct net_buf *buf = NULL;

    BT_DBG("xact_id %u", xact_id);

    if (link->pending_ack == xact_id) {
        BT_DBG("Not sending duplicate ack");
        return;
    }

    buf = adv_buf_create();
    if (!buf) {
        return;
    }

    if (link->pending_ack == PROV_XACT_NVAL) {
        link->pending_ack = xact_id;
        complete = &cb;
    } else {
        complete = NULL;
    }

    net_buf_add_be32(buf, link->link_id);
    net_buf_add_u8(buf, xact_id);
    net_buf_add_u8(buf, GPC_ACK);

    bt_mesh_adv_send(buf, PROV_XMIT, complete, link);
    net_buf_unref(buf);
}

static void free_segments(struct bt_mesh_prov_link *link)
{
    for (size_t i = 0; i < ARRAY_SIZE(link->tx.buf); i++) {
        struct net_buf *buf = link->tx.buf[i];

        if (!buf) {
            break;
        }

        link->tx.buf[i] = NULL;
        bt_mesh_adv_buf_ref_debug(__func__, buf, 3U, BLE_MESH_BUF_REF_SMALL);
        /* Mark as canceled */
        BLE_MESH_ADV(buf)->busy = 0U;
        net_buf_unref(buf);
    }
}

void bt_mesh_prov_clear_tx(struct bt_mesh_prov_link *link, bool cancel)
{
    bt_mesh_mutex_lock(&link->buf_lock);

    if (cancel) {
        k_delayed_work_cancel(&link->tx.retransmit);
    }

    free_segments(link);

    bt_mesh_mutex_unlock(&link->buf_lock);
}

static void buf_sent(int err, void *user_data)
{
    struct bt_mesh_prov_link *link = user_data;
    int32_t timeout = RETRANSMIT_TIMEOUT;

    if (!link->tx.buf[0]) {
        return;
    }

    /* The following may happen in TWO situations:
     * 1. LINK_CLOSING flag is set, and Link Close is sent;
     * 2. LINK_CLOSING flag is set, and any other provisioning
     *    PDU within the adv queue is sent.
     * Regarding the second situation, since LINK_CLOSING flag
     * is set, so once a pdu is sent, the link could be closed.
     */
    if (bt_mesh_atomic_test_bit(link->flags, LINK_CLOSING)) {
        timeout = CLOSE_TIMEOUT;
    }

    k_delayed_work_submit(&link->tx.retransmit, timeout);
}

static struct bt_mesh_send_cb buf_sent_cb = {
    .end = buf_sent,
};

static void prov_retransmit(struct k_work *work)
{
    struct bt_mesh_prov_link *link = work->user_data;
    int64_t timeout = TRANSACTION_TIMEOUT;

    if (!bt_mesh_atomic_test_bit(link->flags, LINK_ACTIVE) &&
        !bt_mesh_atomic_test_bit(link->flags, LINK_CLOSING)) {
        BT_WARN("Link not active");
        return;
    }

#if CONFIG_BLE_MESH_FAST_PROV
    if (link->tx_pdu_type >= link->last_tx_pdu) {
        timeout = K_SECONDS(30);
    }
#endif /* CONFIG_BLE_MESH_FAST_PROV */

    /* Use a timeout of 0 ~ 60s for PB-Remote Link Open Procedure. */
    if (bt_mesh_atomic_test_bit(link->flags, PB_REMOTE) &&
        bt_mesh_atomic_test_bit(link->flags, PBR_OPENING)) {
        timeout = K_SECONDS(link->pb_remote_timeout);
    }

    if (k_uptime_get() - link->tx.start > timeout) {
        BT_WARN("Timeout, giving up transaction");
        if (bt_mesh_atomic_test_bit(link->flags, PB_REMOTE)) {
            bt_mesh_prov_clear_tx(link, true);
            link->pb_remote_cbd = false;
            link->pb_remote_reset = false;
            link->pb_remote_csp = true;
            if (link->pb_remote_close) {
                link->pb_remote_close(link, CLOSE_REASON_TIMEOUT);
            }
        } else {
            /* Send Link Close if the device is Provisioner, or
             * directly reset adv link if the device is Node.
             */
            if (link->retrans_timeout) {
                link->retrans_timeout(link, CLOSE_REASON_TIMEOUT);
            }
        }
        return;
    }

    if (bt_mesh_atomic_test_bit(link->flags, LINK_CLOSING)) {
        if (bt_mesh_atomic_test_bit(link->flags, PB_REMOTE)) {
            bt_mesh_prov_clear_tx(link, true);
            link->pb_remote_cbd = false;
            /* In this case, no need to send Link Close */
            link->pb_remote_reset = true;
            if (link->pb_remote_close) {
                link->pb_remote_close(link, link->reason);
            }
        } else {
            if (link->reset_adv_link) {
                link->reset_adv_link(link, link->reason);
            }
        }
        return;
    }

    bt_mesh_mutex_lock(&link->buf_lock);

    for (size_t i = 0; i < ARRAY_SIZE(link->tx.buf); i++) {
        struct net_buf *buf = link->tx.buf[i];

        if (!buf) {
            break;
        }

        if (BLE_MESH_ADV(buf)->busy) {
            continue;
        }

        BT_DBG("%u bytes: %s", buf->len, bt_hex(buf->data, buf->len));

        if (i + 1 < ARRAY_SIZE(link->tx.buf) && link->tx.buf[i + 1]) {
            bt_mesh_adv_send(buf, PROV_XMIT, NULL, NULL);
        } else {
            bt_mesh_adv_send(buf, PROV_XMIT, &buf_sent_cb, link);
        }
    }

    bt_mesh_mutex_unlock(&link->buf_lock);
}

int bt_mesh_prov_retransmit_init(struct bt_mesh_prov_link *link)
{
    link->tx.retransmit.work.user_data = link;

    return k_delayed_work_init(&link->tx.retransmit, prov_retransmit);
}

static void send_reliable(struct bt_mesh_prov_link *link, uint8_t xmit)
{
    link->tx.start = k_uptime_get();

    for (size_t i = 0; i < ARRAY_SIZE(link->tx.buf); i++) {
        struct net_buf *buf = link->tx.buf[i];

        if (!buf) {
            break;
        }

        if (i + 1 < ARRAY_SIZE(link->tx.buf) && link->tx.buf[i + 1]) {
            bt_mesh_adv_send(buf, xmit, NULL, NULL);
        } else {
            bt_mesh_adv_send(buf, xmit, &buf_sent_cb, link);
        }
    }
}

int bt_mesh_prov_bearer_ctl_send(struct bt_mesh_prov_link *link, uint8_t op,
                                 void *data, uint8_t data_len)
{
    struct net_buf *buf = NULL;
    uint8_t xmit = 0;

    BT_DBG("op 0x%02x data_len %u", op, data_len);

    bt_mesh_prov_clear_tx(link, true);

    buf = adv_buf_create();
    if (!buf) {
        return -ENOBUFS;
    }

    net_buf_add_be32(buf, link->link_id);
    /* Transaction ID, always 0 for Bearer messages */
    net_buf_add_u8(buf, 0x00);
    net_buf_add_u8(buf, GPC_CTL(op));
    net_buf_add_mem(buf, data, data_len);

    link->tx.buf[0] = buf;
    link->tx.id = 0;   /* Set Transaction ID to 0 */

    xmit = (op == LINK_CLOSE) ? CLOSE_XMIT : PROV_XMIT;

    send_reliable(link, xmit);

    if (op == LINK_CLOSE) {
        bt_mesh_atomic_clear_bit(link->flags, LINK_ACTIVE);
        bt_mesh_atomic_set_bit(link->flags, LINK_CLOSING);
        link->reason = *((uint8_t *)data);
    }

    return 0;
}

static uint8_t last_seg(uint8_t len)
{
    if (len <= START_PAYLOAD_MAX) {
        return 0;
    }

    len -= START_PAYLOAD_MAX;

    return 1 + (len / CONT_PAYLOAD_MAX);
}

int bt_mesh_prov_send_adv(struct bt_mesh_prov_link *link, struct net_buf_simple *msg)
{
    struct net_buf *start = NULL, *buf = NULL;
    int32_t timeout = PROTOCOL_TIMEOUT;
    uint8_t seg_len = 0U, seg_id = 0U;
    uint8_t xact_id = 0U;

    BT_DBG("len %u: %s", msg->len, bt_hex(msg->data, msg->len));

    if (link->next_xact_id == NULL) {
        BT_ERR("Empty transaction id cb");
        return -EIO;
    }

    bt_mesh_prov_clear_tx(link, true);

    start = adv_buf_create();
    if (!start) {
        return -ENOBUFS;
    }

    xact_id = link->next_xact_id(link);
    net_buf_add_be32(start, link->link_id);
    net_buf_add_u8(start, xact_id);

    net_buf_add_u8(start, GPC_START(last_seg(msg->len)));
    net_buf_add_be16(start, msg->len);
    net_buf_add_u8(start, bt_mesh_fcs_calc(msg->data, msg->len));

    link->tx.buf[0] = start;

#if CONFIG_BLE_MESH_FAST_PROV
    link->tx_pdu_type = msg->data[0];
#endif /* CONFIG_BLE_MESH_FAST_PROV */

    seg_len = MIN(msg->len, START_PAYLOAD_MAX);
    BT_DBG("seg 0 len %u: %s", seg_len, bt_hex(msg->data, seg_len));
    net_buf_add_mem(start, msg->data, seg_len);
    net_buf_simple_pull(msg, seg_len);

    for (seg_id = 1; msg->len > 0; seg_id++) {
        if (seg_id >= ARRAY_SIZE(link->tx.buf)) {
            BT_ERR("Too big message (seg_id %d)", seg_id);
            bt_mesh_prov_clear_tx(link, false);
            return -E2BIG;
        }

        buf = adv_buf_create();
        if (!buf) {
            bt_mesh_prov_clear_tx(link, false);
            return -ENOBUFS;
        }

        link->tx.buf[seg_id] = buf;

        seg_len = MIN(msg->len, CONT_PAYLOAD_MAX);

        BT_DBG("seg_id %u len %u: %s", seg_id, seg_len,
               bt_hex(msg->data, seg_len));

        net_buf_add_be32(buf, link->link_id);
        net_buf_add_u8(buf, xact_id);
        net_buf_add_u8(buf, GPC_CONT(seg_id));
        net_buf_add_mem(buf, msg->data, seg_len);
        net_buf_simple_pull(msg, seg_len);
    }

    send_reliable(link, PROV_XMIT);

#if CONFIG_BLE_MESH_FAST_PROV
    if (link->tx_pdu_type >= link->last_tx_pdu) {
        timeout = K_SECONDS(60);
    }
#endif /* CONFIG_BLE_MESH_FAST_PROV */

    k_delayed_work_submit(&link->prot_timer, timeout);

    return 0;
}
#endif /* CONFIG_BLE_MESH_PB_ADV */

int bt_mesh_prov_send(struct bt_mesh_prov_link *link, struct net_buf_simple *buf)
{
#if CONFIG_BLE_MESH_RPR_SRV
    if (bt_mesh_atomic_test_bit(link->flags, PB_NPPI)) {
        if (link->pb_remote_send) {
            BT_INFO("NPPI, send prov pdu 0x%02x", buf->data[0]);
            return link->pb_remote_send(link, buf);
        }

        BT_ERR("No NPPI send callback provided");
        return -EIO;
    }
#endif /* CONFIG_BLE_MESH_RPR_SRV */

#if CONFIG_BLE_MESH_RPR_CLI || CONFIG_BLE_MESH_RPR_SRV
    if (bt_mesh_atomic_test_bit(link->flags, PB_REMOTE)) {
        if (link->pb_remote_send) {
            BT_INFO("PB-Remote, send prov pdu 0x%02x", buf->data[0]);
            return link->pb_remote_send(link, buf);
        }
        BT_ERR("No PB-Remote send callback provided");
        return -EIO;
    }
#endif /* CONFIG_BLE_MESH_RPR_CLI || CONFIG_BLE_MESH_RPR_SRV */

#if CONFIG_BLE_MESH_PB_GATT
    if (link->conn) {
        if (link->pb_gatt_send) {
            return link->pb_gatt_send(link, buf);
        }

        BT_ERR("No PB-GATT send callback provided");
        return -EIO;
    }
#endif /* CONFIG_BLE_MESH_PB_GATT */

#if CONFIG_BLE_MESH_PB_ADV
    if (bt_mesh_prov_buf_type_get(buf) == PROV_FAILED) {
        /* For case MESH/NODE/PROV/BV-10-C, Node must send Transaction
         * ACK before Provisioning Failed message is transmitted.
         */
        bt_mesh_gen_prov_ack_send(link, link->rx.id);
    }

    return bt_mesh_prov_send_adv(link, buf);
#endif /* CONFIG_BLE_MESH_PB_ADV */

    /* Shall not reach here. */
    return 0;
}
