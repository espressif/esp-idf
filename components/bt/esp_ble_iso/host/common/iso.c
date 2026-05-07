/*
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 * SPDX-FileCopyrightText: 2023 Nordic Semiconductor
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/hci_types.h>
#include <zephyr/bluetooth/common/bt_str.h>

#include <../host/conn_internal.h>
#include <../host/iso_internal.h>
#include <../host/hci_core.h>

#include "common/host.h"
#include "common/task.h"
#include "common/app/gap.h"

#ifdef CONFIG_BT_NIMBLE_ISO_STD_FLOW_CTRL
#define ISO_STD_FLOW_CTRL       true
#else /* CONFIG_BT_NIMBLE_ISO_STD_FLOW_CTRL */
#define ISO_STD_FLOW_CTRL       false
#endif /* CONFIG_BT_NIMBLE_ISO_STD_FLOW_CTRL */

#define ISO_PKT_FIRST_FRAG      (0b00)
#define ISO_PKT_CONT_FRAG       (0b01)
#define ISO_PKT_COMP_SDU        (0b10)
#define ISO_PKT_LAST_FRAG       (0b11)

#define BT_LE_FEAT_SET(feat, n)     (feat[(n) >> 3] |= BIT((n) & 7))
#define BT_LE_FEAT_UNSET(feat, n)   (feat[(n) >> 3] &= ~BIT((n) & 7))

static sys_slist_t iso_cbs = SYS_SLIST_STATIC_INIT(&iso_cbs);

#if CONFIG_BT_ISO_UNICAST
static void hci_le_cis_disconnected(struct net_buf *buf);
#endif /* CONFIG_BT_ISO_UNICAST */

#if CONFIG_BT_ISO_TX
static void iso_tx_sdu_clear(uint16_t conn_handle, bool is_big);
#endif /* CONFIG_BT_ISO_TX */

extern void hci_le_biginfo_adv_report(struct net_buf *buf);

static struct bt_le_iso_cb iso_cb = {
#if CONFIG_BT_ISO_UNICAST
    .cis_dis    = hci_le_cis_disconnected,
    .cis_est    = hci_le_cis_established,
#if CONFIG_BT_ISO_PERIPHERAL
    .cis_req    = hci_le_cis_req,
#endif /* CONFIG_BT_ISO_PERIPHERAL */
#endif /* CONFIG_BT_ISO_UNICAST */
#if CONFIG_BT_ISO_BROADCASTER
    .big_create = hci_le_big_complete,
    .big_term   = hci_le_big_terminate,
#endif /* CONFIG_BT_ISO_BROADCASTER */
#if CONFIG_BT_ISO_SYNC_RECEIVER
    .big_est    = hci_le_big_sync_established,
    .big_lost   = hci_le_big_sync_lost,
    .biginfo    = hci_le_biginfo_adv_report,
#endif /* CONFIG_BT_ISO_SYNC_RECEIVER */
};

bool iso_log_enable = true;
bool iso_hw_log_enable = true;
bool isoal_post_lost = true;

extern uint16_t r_ble_ll_iso_free_buf_num_get(uint16_t conn_handle);
extern int r_ble_hci_trans_hs_iso_tx(const uint8_t *data, uint16_t length, void *arg);
extern void r_ble_ll_isoal_tx_comp_cb_set(void *cb);

_IDF_ONLY
int bt_le_iso_cb_register_safe(struct bt_le_iso_cb *cb)
{
    int err = 0;

    LOG_DBG("IsoCbReg");

    if (cb == NULL) {
        LOG_ERR("IsoCbNull");
        return -EINVAL;
    }

    bt_le_host_lock();

    if (sys_slist_find(&iso_cbs, &cb->node, NULL)) {
        LOG_WRN("IsoCbExist");
        err = -EEXIST;
        goto end;
    }

    sys_slist_append(&iso_cbs, &cb->node);

end:
    bt_le_host_unlock();

    return err;
}

_IDF_ONLY
void bt_le_iso_cb_unregister_safe(struct bt_le_iso_cb *cb)
{
    LOG_DBG("IsoCbUnreg");

    if (cb == NULL) {
        LOG_ERR("IsoCbNull");
        return;
    }

    bt_le_host_lock();
    sys_slist_find_and_remove(&iso_cbs, &cb->node);
    bt_le_host_unlock();
}

#if CONFIG_BT_ISO_UNICAST
static void hci_le_cis_disconnected(struct net_buf *buf)
{
    struct bt_hci_evt_disconn_complete *evt = (void *)buf->data;
    struct bt_conn *iso;

    LOG_DBG("CisDisconnectedEvt[%u]", evt->handle);

    iso = bt_conn_lookup_handle(evt->handle, BT_CONN_TYPE_ISO);
    assert(iso);

    iso->err = evt->reason;

    bt_iso_disconnected(iso);
    bt_conn_unref(iso);
}

static int iso_cis_disconn_evt_listener_safe(uint8_t *data)
{
    struct bt_le_iso_cb *listener = NULL;
    struct net_buf buf = {0};

    LOG_DBG("CisDisconnEvtListener");

    bt_le_host_lock();

    SYS_SLIST_FOR_EACH_CONTAINER(&iso_cbs, listener, node) {
        if (listener->cis_dis) {
            buf.data = data;
            listener->cis_dis(&buf);
        }
    }

#if CONFIG_BT_ISO_TX
    iso_tx_sdu_clear(sys_get_le16(data + 1), false);
#endif /* CONFIG_BT_ISO_TX */

    bt_le_host_unlock();

    return 0;
}

static int iso_cis_est_evt_listener_safe(uint8_t *data)
{
    struct bt_le_iso_cb *listener = NULL;
    struct net_buf buf = {0};

    LOG_DBG("CisEstEvtListener");

    bt_le_host_lock();

    SYS_SLIST_FOR_EACH_CONTAINER(&iso_cbs, listener, node) {
        if (listener->cis_est) {
            buf.data = data + 1;    /* The first octet is subev_code */
            listener->cis_est(&buf);
        }
    }

    bt_le_host_unlock();

    return 0;
}

#if CONFIG_BT_ISO_PERIPHERAL
static int iso_cis_req_evt_listener_safe(uint8_t *data)
{
    struct bt_le_iso_cb *listener = NULL;
    struct net_buf buf = {0};

    LOG_DBG("CisReqEvtListener");

    bt_le_host_lock();

    SYS_SLIST_FOR_EACH_CONTAINER(&iso_cbs, listener, node) {
        if (listener->cis_req) {
            buf.data = data + 1;    /* The first octet is subev_code */
            listener->cis_req(&buf);
        }
    }

    bt_le_host_unlock();

    return 0;
}
#endif /* CONFIG_BT_ISO_PERIPHERAL */
#endif /* CONFIG_BT_ISO_UNICAST */

#if CONFIG_BT_ISO_BROADCASTER
static int iso_create_big_comp_evt_listener_safe(uint8_t *data)
{
    struct bt_le_iso_cb *listener = NULL;
    struct net_buf buf = {0};

    LOG_DBG("CreateBigCompEvtListener");

    bt_le_host_lock();

    SYS_SLIST_FOR_EACH_CONTAINER(&iso_cbs, listener, node) {
        if (listener->big_create) {
            buf.data = data + 1;    /* The first octet is subev_code */
            listener->big_create(&buf);
        }
    }

    bt_le_host_unlock();

    return 0;
}

static int iso_term_big_comp_evt_listener_safe(uint8_t *data)
{
    struct bt_le_iso_cb *listener = NULL;
    struct net_buf buf = {0};

    LOG_DBG("TermBigCompEvtListener");

    bt_le_host_lock();

    SYS_SLIST_FOR_EACH_CONTAINER(&iso_cbs, listener, node) {
        if (listener->big_term) {
            buf.data = data + 1;    /* The first octet is subev_code */
            listener->big_term(&buf);
        }
    }

    iso_tx_sdu_clear(data[1], true);

    bt_le_host_unlock();

    return 0;
}
#endif /* CONFIG_BT_ISO_BROADCASTER */

#if CONFIG_BT_ISO_SYNC_RECEIVER
static int iso_big_sync_est_evt_listener_safe(uint8_t *data)
{
    struct bt_le_iso_cb *listener = NULL;
    struct net_buf buf = {0};

    LOG_DBG("BigSyncEstEvtListener");

    bt_le_host_lock();

    SYS_SLIST_FOR_EACH_CONTAINER(&iso_cbs, listener, node) {
        if (listener->big_est) {
            buf.data = data + 1;    /* The first octet is subev_code */
            listener->big_est(&buf);
        }
    }

    bt_le_host_unlock();

    return 0;
}

static int iso_big_sync_lost_evt_listener_safe(uint8_t *data)
{
    struct bt_le_iso_cb *listener = NULL;
    struct net_buf buf = {0};

    LOG_DBG("BigSyncLostEvtListener");

    bt_le_host_lock();

    SYS_SLIST_FOR_EACH_CONTAINER(&iso_cbs, listener, node) {
        if (listener->big_lost) {
            buf.data = data + 1;    /* The first octet is subev_code */
            listener->big_lost(&buf);
        }
    }

    bt_le_host_unlock();

    return 0;
}

static int iso_biginfo_adv_report_listener_safe(uint8_t *data)
{
    struct bt_le_iso_cb *listener = NULL;
    struct net_buf buf = {0};

    /* LOG_DBG("BigInfoAdvReportEvtListener"); */

    bt_le_host_lock();

    SYS_SLIST_FOR_EACH_CONTAINER(&iso_cbs, listener, node) {
        if (listener->biginfo) {
            buf.data = data + 1;    /* The first octet is subev_code */
            listener->biginfo(&buf);
        }
    }

    bt_le_host_unlock();

    /* The first octet is subev_code */
    bt_le_gap_app_biginfo_event(data + 1);

    return 0;
}
#endif /* CONFIG_BT_ISO_SYNC_RECEIVER */

static void handle_iso_event(uint8_t *data, size_t data_len)
{
    uint8_t event;
    bool le_meta;

    /* Note:
     * The data_len is guaranteed by the Host that it is exactly
     * matching the expected length.
     */

    le_meta = data[0];
    event = data[1];

    if (event != BT_HCI_EVT_LE_BIGINFO_ADV_REPORT) {
        LOG_DBG("HandleIsoEvt[%u][%u]", le_meta, event);
    }

#if CONFIG_BT_ISO_UNICAST
    if (le_meta == false) {
        assert(event == BT_HCI_EVT_DISCONN_COMPLETE);
        iso_cis_disconn_evt_listener_safe(data + 2);
        return;
    }
#else /* CONFIG_BT_ISO_UNICAST */
    ARG_UNUSED(le_meta);
#endif /* CONFIG_BT_ISO_UNICAST */

    switch (event) {
#if CONFIG_BT_ISO_UNICAST
    case BT_HCI_EVT_LE_CIS_ESTABLISHED:
        iso_cis_est_evt_listener_safe(data + 2);
        break;
#if CONFIG_BT_ISO_PERIPHERAL
    case BT_HCI_EVT_LE_CIS_REQ:
        iso_cis_req_evt_listener_safe(data + 2);
        break;
#endif /* CONFIG_BT_ISO_PERIPHERAL */
#endif /* CONFIG_BT_ISO_UNICAST */
#if CONFIG_BT_ISO_BROADCASTER
    case BT_HCI_EVT_LE_BIG_COMPLETE:
        iso_create_big_comp_evt_listener_safe(data + 2);
        break;
    case BT_HCI_EVT_LE_BIG_TERMINATE:
        iso_term_big_comp_evt_listener_safe(data + 2);
        break;
#endif /* CONFIG_BT_ISO_BROADCASTER */
#if CONFIG_BT_ISO_SYNC_RECEIVER
    case BT_HCI_EVT_LE_BIG_SYNC_ESTABLISHED:
        iso_big_sync_est_evt_listener_safe(data + 2);
        break;
    case BT_HCI_EVT_LE_BIG_SYNC_LOST:
        iso_big_sync_lost_evt_listener_safe(data + 2);
        break;
    case BT_HCI_EVT_LE_BIGINFO_ADV_REPORT:
        iso_biginfo_adv_report_listener_safe(data + 2);
        break;
#endif /* CONFIG_BT_ISO_SYNC_RECEIVER */
    default:
        assert(0);
        break;
    }
}

void bt_le_iso_handle_hci_event(uint8_t *data, size_t data_len)
{
    assert(data && data_len);
    handle_iso_event(data, data_len);
    free(data);
}

#if CONFIG_BT_ISO_TX
struct iso_tx_sdu_node {
    struct bt_iso_chan *chan;

    const uint8_t *sdu;
    uint16_t sdu_len;

    sys_snode_t node;
};

static sys_slist_t iso_tx_sdu_list = SYS_SLIST_STATIC_INIT(&iso_tx_sdu_list);

static int iso_tx_sdu_insert(struct bt_iso_chan *chan, const uint8_t *sdu, uint16_t sdu_len)
{
    struct iso_tx_sdu_node *sdu_node;

    sdu_node = calloc(1, sizeof(*sdu_node));
    if (sdu_node == NULL) {
        LOG_ERR("IsoTxInsertNoMem[%u]", sizeof(*sdu_node));
        return -ENOMEM;
    }

    sdu_node->chan = chan;
    sdu_node->sdu = sdu;
    sdu_node->sdu_len = sdu_len;

    sys_slist_append(&iso_tx_sdu_list, &sdu_node->node);

    return 0;
}

static void iso_tx_sdu_clear(uint16_t handle, bool is_big)
{
    struct iso_tx_sdu_node *sdu_node;
    struct iso_tx_sdu_node *sdu_tmp;
    struct bt_conn *iso;
    sys_snode_t *prev;

    LOG_DBG("IsoTxSduClear[%u][%u]", handle, is_big);

    prev = NULL;

    SYS_SLIST_FOR_EACH_CONTAINER_SAFE(&iso_tx_sdu_list, sdu_node, sdu_tmp, node) {
        iso = sdu_node->chan->iso;

        if ((is_big == false && iso->handle == handle) ||
                (is_big && iso->iso.info.type == BT_ISO_CHAN_TYPE_BROADCASTER &&
                 iso->iso.info.broadcaster.big_handle == handle)) {
            LOG_DBG("IsoSduRem[%u]", sdu_node->sdu_len);

            sys_slist_remove(&iso_tx_sdu_list, prev, &sdu_node->node);

            free((void *)sdu_node->sdu);
            free(sdu_node);

            continue;
        }

        prev = &sdu_node->node;
    }
}

static int validate_iso_send(struct bt_iso_chan *chan, struct net_buf *buf)
{
    if (chan == NULL || buf == NULL ||
            (buf->data == NULL && buf->len != 0)) {
        LOG_ERR("InvSendParam[%p][%p]", chan, buf);
        return -EINVAL;
    }

    if (chan->state != BT_ISO_STATE_CONNECTED) {
        LOG_ERR("ChanNotConn[%u]", chan->iso->handle);
        return -ENOTCONN;
    }

    if (chan->iso->iso.info.can_send == false) {
        LOG_ERR("ChanNotSend[%u]", chan->iso->handle);
        return -EINVAL;
    }

    if (buf->len > chan->qos->tx->sdu) {
        LOG_ERR("ChanCannotSend[%u][%u][%u]",
                chan->iso->handle, buf->len, chan->qos->tx->sdu);
        return -EMSGSIZE;
    }

    return 0;
}

static int iso_tx_now(struct bt_iso_chan *chan, const uint8_t *sdu,
                      uint16_t sdu_len, uint16_t seq_num,
                      uint32_t time_stamp, bool ts_valid)
{
    uint16_t data_total_len;
    uint8_t pb_flag;
    uint8_t *pkt;
    int err;

    pb_flag = ISO_PKT_COMP_SDU;
    data_total_len = (ts_valid ? 4 : 0) + 4 + sdu_len;

    /* From Spec:
     * In the Host to Controller direction, Data_Total_Length shall
     * be less than or equal to the size of buffer supported by the
     * Controller (which is returned using the ISO_Data_Packet_Length
     * return parameter of the LE Read Buffer Size command).
     *
     * TODO:
     * Check if ISO SDU needs to be fragmented here.
     */

    pkt = malloc(4 + data_total_len);
    if (pkt == NULL) {
        LOG_ERR("IsoTxNoMem[%u]", 4 + data_total_len);
        return -ENOMEM;
    }

    sys_put_le16((ts_valid << 14) | (pb_flag << 12) | chan->iso->handle, pkt);
    sys_put_le16(data_total_len, pkt + 2);
    if (ts_valid) {
        sys_put_le32(time_stamp, pkt + 4);
    }
    sys_put_le16(seq_num, pkt + (ts_valid ? 8 : 4));
    sys_put_le16(sdu_len, pkt + (ts_valid ? 10 : 6));

    memcpy(pkt + (ts_valid ? 12 : 8), sdu, sdu_len);

#if ISO_STD_FLOW_CTRL
    /* TODO:
     * - Use variables to record the ISO buffer size & count;
     * - Check if the available ISO buffers are enough to hold the SDU;
     */
#else /* ISO_STD_FLOW_CTRL */
    /* If the list is not empty, which means some ISO SDUs ahead of this
     * one are still waiting to be transmitted. In this case, this SDU
     * must be appended in the list.
     *
     * If the list is empty, but no free buffer exists in the controller,
     * then ISO flow control is triggered.
     */
    if (sys_slist_is_empty(&iso_tx_sdu_list) == false ||
            r_ble_ll_iso_free_buf_num_get(chan->iso->handle) == 0) {
        /* LOG_INF("IsoFlowCtrl!"); */
        err = iso_tx_sdu_insert(chan, pkt, 4 + data_total_len);
        if (err) {
            free(pkt);
        }
        return err;
    }
#endif /* ISO_STD_FLOW_CTRL */

    err = r_ble_hci_trans_hs_iso_tx(pkt, 4 + data_total_len, NULL);
    if (err) {
        /* err = -1: No BIS or CIS found;
         * err = -2: Bad ISO packet;
         * err = -3: Not able to send (e.g., CIS tx bn is 0, etc);
         * err = -4: Not ready to send (e.g. BIS or CIS not established, DP not setup, etc.);
         * err = -5: Last SDU fragment (generated over HCI) is failed to send;
         *
         * Note:
         * The allocated pkt buffer will be freed in our controller.
         *
         * TODO:
         * If using controller from other vendors, the pkt buffer may
         * needs to be freed here.
         */
        LOG_WRN("IsoTxFail[%d]", err);
        return err;
    }

#if ISO_STD_FLOW_CTRL
    /* TODO:
     * - Decrease the available ISO buffer count;
     * - Increase the available ISO buffer count once NoCP is received;
     */
#endif /* ISO_STD_FLOW_CTRL */

    return 0;
}

int bt_le_iso_tx(struct bt_iso_chan *chan,
                 struct net_buf *buf,
                 uint16_t seq_num,
                 uint32_t time_stamp,
                 bool ts_valid)
{
    int err;

    err = validate_iso_send(chan, buf);
    if (err) {
        return err;
    }

    return iso_tx_now(chan, buf->data, buf->len, seq_num, time_stamp, ts_valid);
}

struct iso_tx_comp_event {
    uint16_t conn_handle;
    struct bt_iso_tx_cb_info info;
};

void bt_le_iso_handle_tx_comp(uint8_t *data, size_t data_len)
{
    struct iso_tx_comp_event *evt = (struct iso_tx_comp_event *)data;
    struct iso_tx_sdu_node *sdu_node;
    struct bt_iso_chan *chan;
    struct bt_conn *iso;
    bt_conn_tx_cb_t cb;
    sys_snode_t *node;
    void *ud;
    int err;

    assert(data && data_len == sizeof(*evt));

    bt_le_host_lock();

    while (1) {
        /* If no free buffer exists, no need to check if any SDU is pending */
        if (r_ble_ll_iso_free_buf_num_get(evt->conn_handle) == 0) {
            break;
        }

        /* Fetch and remove the first ISO SDU from list if list is not empty */
        node = sys_slist_get(&iso_tx_sdu_list);
        if (node == NULL) {
            break;
        }

        sdu_node = CONTAINER_OF(node, struct iso_tx_sdu_node, node);

        err = r_ble_hci_trans_hs_iso_tx(sdu_node->sdu, sdu_node->sdu_len, NULL);
        if (err) {
            /* err = -1: No BIS or CIS found;
             * err = -2: Bad ISO packet;
             * err = -3: Not able to send (e.g., CIS tx bn is 0, etc);
             * err = -4: Not ready to send (e.g. BIS or CIS not established, DP not setup, etc.);
             * err = -5: Last SDU fragment (generated over HCI) is failed to send;
             *
             * Note:
             * The allocated pkt buffer will be freed in our controller.
             *
             * TODO:
             * If using controller from other vendors, the pkt buffer may
             * needs to be freed here.
             */
             LOG_WRN("IsoTxFail[%d]", err);
        }

        free(sdu_node);
    }

    iso = bt_conn_lookup_handle(evt->conn_handle, BT_CONN_TYPE_ISO);
    if (iso == NULL) {
        goto end;
    }

    chan = iso->iso.chan;
    assert(chan);

    cb = NULL;
    ud = NULL;
    chan->iso->get_and_clear_cb(NULL, NULL, &cb, &ud);

    if (cb) {
        cb(chan->iso, &evt->info, 0);
    }

end:
    bt_le_host_unlock();
    free(data);
}

static void iso_tx_comp_cb(uint16_t conn_handle, void *info, size_t size)
{
    struct iso_tx_comp_event *evt;
    int err;

    /* Note:
     * This callback is invoked in the BLE Controller task, the operation in
     * this function shall be as simple as possible and avoid log output.
     * Avoid acquiring host_lock here to prevent deadlock when the ISO task
     * holds host_lock while waiting for a Controller response. Instead, post
     * the event to the ISO task for processing.
     */

    assert(size == sizeof(struct bt_iso_tx_cb_info));

    evt = calloc(1, sizeof(*evt));
    if (evt == NULL) {
        LOG_ERR("IsoTxCompNoMem[%u]", sizeof(*evt));
        return;
    }

    evt->conn_handle = conn_handle;
    memcpy(&evt->info, info, sizeof(evt->info));

    err = bt_le_iso_task_post(ISO_QUEUE_ITEM_TYPE_ISO_TX_COMP, evt, sizeof(*evt));
    if (err) {
        LOG_ERR("IsoTxCompPostFail[%d]", err);
        free(evt);
    }
}
#else /* CONFIG_BT_ISO_TX */
void bt_le_iso_handle_tx_comp(uint8_t *data, size_t data_len)
{
    ARG_UNUSED(data);
    ARG_UNUSED(data_len);
}
#endif /* CONFIG_BT_ISO_TX */

#if CONFIG_BT_ISO_RX
void bt_le_iso_handle_rx_data(uint8_t *data, size_t data_len)
{
    struct net_buf buf = {0};

    assert(data && data_len);

    bt_le_host_lock();
    net_buf_simple_init_with_data(&buf.b, (void *)data, data_len);
    hci_iso(&buf);
    bt_le_host_unlock();

    free(data);
}

int bt_le_iso_rx(const uint8_t *data, uint16_t len, void *arg)
{
    uint8_t *rx_data;
    int err;

    /* Note:
     * This callback is invoked in the BLE Controller task. The data directly
     * points to the start of an ISO SDU, i.e. the conn_handle, etc. No extra
     * information for now.
     * Avoid acquiring host_lock here to prevent deadlock when the ISO task
     * holds host_lock while waiting for a Controller response. Instead, copy
     * the data and post the event to the ISO task for further processing.
     */

    ARG_UNUSED(arg);

    rx_data = calloc(1, len);
    if (rx_data == NULL) {
        LOG_ERR("IsoRxNoMem[%u]", len);
        return -ENOMEM;
    }

    memcpy(rx_data, data, len);

    err = bt_le_iso_task_post(ISO_QUEUE_ITEM_TYPE_ISO_RX_DATA, rx_data, len);
    if (err) {
        LOG_ERR("IsoRxPostFail[%d]", err);
        free(rx_data);
        return -EIO;
    }

    return 0;
}
#else /* CONFIG_BT_ISO_RX */
void bt_le_iso_handle_rx_data(uint8_t *data, size_t data_len)
{
    ARG_UNUSED(data);
    ARG_UNUSED(data_len);
}
#endif /* CONFIG_BT_ISO_RX */

int bt_le_iso_disconnect(uint16_t conn_handle, uint8_t reason)
{
    return bt_le_nimble_iso_disconnect(conn_handle, reason);
}

static void iso_features_set(void)
{
#if CONFIG_BT_ISO_CENTRAL
    BT_LE_FEAT_SET(bt_dev.le.features, BT_LE_FEAT_BIT_CIS_CENTRAL);
#endif /* CONFIG_BT_ISO_CENTRAL */

#if CONFIG_BT_ISO_PERIPHERAL
    BT_LE_FEAT_SET(bt_dev.le.features, BT_LE_FEAT_BIT_CIS_PERIPHERAL);
#endif /* CONFIG_BT_ISO_PERIPHERAL */

#if CONFIG_BT_ISO_BROADCASTER
    BT_LE_FEAT_SET(bt_dev.le.features, BT_LE_FEAT_BIT_ISO_BROADCASTER);
#endif /* CONFIG_BT_ISO_BROADCASTER */

#if CONFIG_BT_ISO_SYNC_RECEIVER
    BT_LE_FEAT_SET(bt_dev.le.features, BT_LE_FEAT_BIT_SYNC_RECEIVER);
#endif /* CONFIG_BT_ISO_SYNC_RECEIVER */

    LOG_DBG("IsoFeatSet[%s]", bt_hex(bt_dev.le.features, sizeof(bt_dev.le.features)));
}

static void iso_features_unset(void)
{
#if CONFIG_BT_ISO_CENTRAL
    BT_LE_FEAT_UNSET(bt_dev.le.features, BT_LE_FEAT_BIT_CIS_CENTRAL);
#endif /* CONFIG_BT_ISO_CENTRAL */

#if CONFIG_BT_ISO_PERIPHERAL
    BT_LE_FEAT_UNSET(bt_dev.le.features, BT_LE_FEAT_BIT_CIS_PERIPHERAL);
#endif /* CONFIG_BT_ISO_PERIPHERAL */

#if CONFIG_BT_ISO_BROADCASTER
    BT_LE_FEAT_UNSET(bt_dev.le.features, BT_LE_FEAT_BIT_ISO_BROADCASTER);
#endif /* CONFIG_BT_ISO_BROADCASTER */

#if CONFIG_BT_ISO_SYNC_RECEIVER
    BT_LE_FEAT_UNSET(bt_dev.le.features, BT_LE_FEAT_BIT_SYNC_RECEIVER);
#endif /* CONFIG_BT_ISO_SYNC_RECEIVER */

    LOG_DBG("IsoFeatUnset[%s]", bt_hex(bt_dev.le.features, sizeof(bt_dev.le.features)));
}

int bt_le_iso_init(void)
{
    int err;

    LOG_DBG("IsoInit");

    iso_features_set();

    err = bt_le_iso_cb_register_safe(&iso_cb);
    if (err) {
        return err;
    }

#if CONFIG_BT_ISO_TX
    r_ble_ll_isoal_tx_comp_cb_set(iso_tx_comp_cb);
#endif /* CONFIG_BT_ISO_TX */

    err = bt_le_nimble_iso_init();
    if (err) {
        return err;
    }

    return 0;
}

void bt_le_iso_deinit(void)
{
    LOG_DBG("IsoDeinit");

    iso_features_unset();

    bt_le_iso_cb_unregister_safe(&iso_cb);

#if CONFIG_BT_ISO_TX
    r_ble_ll_isoal_tx_comp_cb_set(NULL);
#endif /* CONFIG_BT_ISO_TX */

    bt_le_nimble_iso_deinit();
}
