/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "mesh/kernel.h"
#include "mesh.h"
#include "mesh/hci.h"
#include "mesh/common.h"
#include "mesh/ffs.h"
#include "ext_adv.h"
#include "beacon.h"
#include "prov_common.h"
#include "foundation.h"
#include "proxy_server.h"
#include "proxy_client.h"
#include "prov_pvnr.h"
#include "mesh/adapter.h"

#include "adv_common.h"
#include "ble_adv.h"

static struct bt_mesh_adv_queue *adv_queue;

static struct bt_mesh_adv_inst *adv_insts;

static inline void adv_send_start(uint16_t duration, int err,
                                  const struct bt_mesh_send_cb *cb,
                                  void *cb_data)
{
    if (cb && cb->start) {
        cb->start(duration, err, cb_data);
    }
}

static inline void adv_send_end(int err, const struct bt_mesh_send_cb *cb,
                                void *cb_data)
{
    if (cb && cb->end) {
        cb->end(err, cb_data);
    }
}

static inline int adv_send(struct bt_mesh_adv_inst *inst, uint16_t *adv_duration)
{
    struct net_buf *buf = inst->sending_buf;
    const struct bt_mesh_send_cb *cb = BLE_MESH_ADV(buf)->cb;
    void *cb_data = BLE_MESH_ADV(buf)->cb_data;
    struct bt_mesh_adv_param param = {0};
    uint16_t duration = 0U, adv_int = 0U;
    struct bt_mesh_adv_data ad = {0};
    int err = 0;

    BT_DBG("type %u len %u: %s", BLE_MESH_ADV(buf)->type,
        buf->len, bt_hex(buf->data, buf->len));

    switch (BLE_MESH_ADV(buf)->type) {
    case BLE_MESH_ADV_PROV:
    case BLE_MESH_ADV_DATA:
#if CONFIG_BLE_MESH_FRIEND
    case BLE_MESH_ADV_FRIEND:
#endif
#if CONFIG_BLE_MESH_RELAY_ADV_BUF
    case BLE_MESH_ADV_RELAY_DATA:
#endif
#if CONFIG_BLE_MESH_PROXY_SOLIC_PDU_TX
    case BLE_MESH_ADV_PROXY_SOLIC:
#endif
    case BLE_MESH_ADV_BEACON:
    case BLE_MESH_ADV_URI: {
        adv_int = MAX(ADV_ITVL_MIN,
                      BLE_MESH_TRANSMIT_INT(BLE_MESH_ADV(buf)->xmit));
        duration = (BLE_MESH_TRANSMIT_COUNT(BLE_MESH_ADV(buf)->xmit) + 1) *
                   (adv_int + 10);

        BT_DBG("count %u interval %ums duration %ums",
               BLE_MESH_TRANSMIT_COUNT(BLE_MESH_ADV(buf)->xmit) + 1, adv_int,
               duration);

        ad.type = adv_type[BLE_MESH_ADV(buf)->type];
        ad.data_len = buf->len;
        ad.data = buf->data;

        param.options = 0U;
        param.interval_min = ADV_SCAN_UNIT(adv_int);
        param.interval_max = param.interval_min;

        param.adv_duration = duration;
        param.adv_count = BLE_MESH_TRANSMIT_COUNT(BLE_MESH_ADV(buf)->xmit) + 1;

        param.primary_phy = BLE_MESH_ADV_PHY_1M;
        param.secondary_phy = BLE_MESH_ADV_PHY_1M;

#if CONFIG_BLE_MESH_PROXY_SOLIC_PDU_TX
        if (BLE_MESH_ADV(buf)->type == BLE_MESH_ADV_PROXY_SOLIC) {
            bt_mesh_adv_buf_ref_debug(__func__, buf, 3U, BLE_MESH_BUF_REF_SMALL);
            struct bt_mesh_adv_data solic_ad[2] = {
                BLE_MESH_ADV_DATA_BYTES(BLE_MESH_DATA_UUID16_ALL, 0x59, 0x18),
                BLE_MESH_ADV_DATA(BLE_MESH_DATA_SVC_DATA16, buf->data, buf->len),
            };
            err = bt_le_ext_adv_start(CONFIG_BLE_MESH_ADV_INST_ID, &param, solic_ad, ARRAY_SIZE(solic_ad), NULL, 0);
        } else
#endif
        {
            bt_mesh_adv_buf_ref_debug(__func__, buf, 4U, BLE_MESH_BUF_REF_SMALL);
            err = bt_le_ext_adv_start(inst->id, &param, &ad, 1, NULL, 0);
        }
        }
        break;
#if CONFIG_BLE_MESH_SUPPORT_BLE_ADV
    case BLE_MESH_ADV_BLE:
        struct bt_mesh_ble_adv_data data = {0};
        struct bt_mesh_ble_adv_tx *tx = cb_data;

        if (tx == NULL) {
            BT_ERR("Invalid adv user data");
            net_buf_unref(buf);
            return -EINVAL;
        }

        BT_DBG("interval %dms, duration %dms, period %dms, count %d",
            ADV_SCAN_INT(tx->param.interval), tx->param.duration,
            tx->param.period, tx->param.count);

        data.adv_data_len = tx->buf->data[0];
        if (data.adv_data_len) {
            memcpy(data.adv_data, tx->buf->data + 1, data.adv_data_len);
        }
        data.scan_rsp_data_len = tx->buf->data[data.adv_data_len + 1];
        if (data.scan_rsp_data_len) {
            memcpy(data.scan_rsp_data, tx->buf->data + data.adv_data_len + 2, data.scan_rsp_data_len);
        }
        duration = tx->param.duration;

        bt_mesh_adv_buf_ref_debug(__func__, buf, 3U, BLE_MESH_BUF_REF_SMALL);

        err = bt_mesh_ble_ext_adv_start(inst->id, &tx->param, &data);
        break;
#endif
    default:
        BT_ERR("Error Type");
        break;
    }

    adv_send_start(duration, err, cb, cb_data);
    if (err) {
        BT_ERR("Start advertising failed: err %d", err);
        return err;
    }

    *adv_duration = duration;
    BT_DBG("Advertising started. %u ms", duration);
    return 0;
}

static inline int find_valid_msg_from_queue(bt_mesh_queue_t *msg_queue, bt_mesh_msg_t *msg)
{
    while(uxQueueMessagesWaiting(msg_queue->handle)) {
        xQueueReceive(msg_queue->handle, msg, K_WAIT(K_FOREVER));

        /* In the previous adv task design, only
         * the *buf of messages pushed to the queue
         * by adv_update would be empty, but in the
         * new design, there is a new processing method
         * for adv_update's messages,
         * so *buf here cannot be empty. */
        assert(msg->arg);

        /* If the message is canceled for advertising,
         * then continue to retrieve the next message
         * from that queue. */
        if (!bt_mesh_atomic_cas(&BLE_MESH_ADV_BUSY(BLE_MESH_MSG_NET_BUF(msg)), 1, 0)) {
            bt_mesh_adv_buf_ref_debug(__func__, BLE_MESH_MSG_NET_BUF(msg), 1U, BLE_MESH_BUF_REF_EQUAL);
            /* Cancel the adv task's reference to this data packet.
             * tips: The reference of buffer by adv_task occurs
             * when the buffer is pushed into the queue.
             */
            net_buf_unref(BLE_MESH_MSG_NET_BUF(msg));
            /* Avoid reading the last message in the queue, which could lead
             * to pointing to an invalid buffer due to the absence of other
             * messages in the queue. */
            msg->arg = NULL;
            continue;
        }

#if CONFIG_BLE_MESH_RELAY_ADV_BUF
        /* If the relay message should be ignored,
         * then continue to retrieve the next message
         * from that queue. */
        if (msg->relay && bt_mesh_ignore_relay_packet(msg->timestamp)) {
            /* If the interval between "current time - msg.timestamp" is bigger than
             * BLE_MESH_RELAY_TIME_INTERVAL, this relay packet will not be sent.
             */
            BT_DBG("Ignore relay packet");
            net_buf_unref(BLE_MESH_MSG_NET_BUF(msg));
            msg->arg = NULL;
            continue;
        }
#endif
        break;
    }

    if (msg->arg == NULL) {
        return -EINVAL;
    }

    return 0;
}

static inline int active_idle_adv_instance(uint32_t *update_evts, uint16_t *min_duration)
{
    uint32_t evts = 0;
    uint16_t duration = K_FOREVER;
    uint16_t cur_min_duration = K_FOREVER;
    enum bt_mesh_adv_type adv_type = 0;
    struct bt_mesh_adv_inst *instance = NULL;
    bt_mesh_queue_t *msg_queue = NULL;
    bt_mesh_msg_t msg = {0};
    uint32_t spt_mask = 0;

#if (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_SERVER
    if (!adv_insts[BLE_MESH_ADV_PROXY_INS].busy) {
        BT_DBG("Mesh Proxy Advertising start");
        duration = bt_mesh_proxy_server_adv_start();
        if (duration < cur_min_duration) {
            cur_min_duration = duration;
        }
        adv_insts[BLE_MESH_ADV_PROXY_INS].busy = true;
        evts |= ADV_TASK_ADV_INST_EVT(adv_insts[BLE_MESH_ADV_PROXY_INS].id);
    }
#endif

    for (int i = BLE_MESH_ADV_INS; i < BLE_MESH_ADV_INS_TYPES_NUM; i++) {
        instance = &adv_insts[i];
        if (instance->busy
#if (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || \
    CONFIG_BLE_MESH_GATT_PROXY_SERVER
        || unlikely(instance->id == CONFIG_BLE_MESH_PROXY_ADV_INST_ID)
#endif
        ) {
            continue;
        }

        spt_mask = instance->spt_mask;
        adv_type = 0;

        while(spt_mask) {
            adv_type = find_lsb_set(spt_mask) - 1;
            spt_mask &= ~BIT(adv_type);
            msg_queue = &(bt_mesh_adv_types_mgnt_get(adv_type)->adv_q->q);

            /* When there is no new message in the queue, *buf (aka: msg.arg)
             * will be empty. */
            if (find_valid_msg_from_queue(msg_queue, &msg)) {
                BT_DBG("no valid message for instance %d", instance->id);
                continue;
            }

            instance->sending_buf = (struct net_buf *)msg.arg;
            if (adv_send(instance, &duration)) {
                BT_ERR("adv start failed");
                net_buf_unref(instance->sending_buf);
                instance->sending_buf = NULL;
                /* When this adv instance fails to broadcast, it could be
                 * due to some persistent issues, such as incorrect adv
                 * parameter settings, or it could be due to some temporary
                 * issues, such as memory allocation failure. Therefore, it
                 * is advisable to skip subsequent queue reads for this instance
                 * and attempt to broadcast subsequent data again next time,
                 * rather than disabling the adv instance. */
                break;
            }

            if (duration < cur_min_duration) {
                cur_min_duration = duration;
            }

            instance->busy = true;
            evts |= ADV_TASK_ADV_INST_EVT(adv_insts[i].id);

            /* Must be nullified to avoid affecting the next adv
             * instance's judgment on whether the message queue
             * is empty. */
            msg.arg = NULL;
            break;
        }
    }

    *min_duration = cur_min_duration;
    *update_evts |= evts;

    return 0;
}

static uint32_t received_adv_evts_handle(uint32_t recv_evts)
{
    uint32_t evt = 0;

    if (!recv_evts) {
        return 0;
    }

    for (int i = 0; recv_evts && i < BLE_MESH_ADV_INS_TYPES_NUM; i++) {
        evt = ADV_TASK_ADV_INST_EVT(adv_insts[i].id);
        if (recv_evts & evt) {
            recv_evts &= ~evt;
#if (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || \
CONFIG_BLE_MESH_GATT_PROXY_SERVER
            if (unlikely(i == BLE_MESH_ADV_PROXY_INS)) {
                BT_DBG("Mesh Proxy Advertising auto stop");
                bt_mesh_proxy_server_adv_flag_set(false);
            } else
#endif
            {
                /* adv_send_end maybe*/
                adv_send_end(0, BLE_MESH_ADV(adv_insts[i].sending_buf)->cb, BLE_MESH_ADV(adv_insts[i].sending_buf)->cb_data);
                bt_mesh_adv_buf_ref_debug(__func__, adv_insts[i].sending_buf, 4U, BLE_MESH_BUF_REF_SMALL);
                net_buf_unref(adv_insts[i].sending_buf);
                adv_insts[i].sending_buf = NULL;
            }
            adv_insts[i].busy = false;
        }
    }

    return recv_evts;
}

static void adv_thread(void *p)
{
    uint16_t adv_duration = K_FOREVER;
    uint32_t recv_evts = 0;
    uint32_t wait_evts = 0;

    BT_DBG("%s, starts", __func__);

    while (1) {
        adv_duration = K_FOREVER;
        wait_evts |= ADV_TASK_PKT_SEND_EVT;

        active_idle_adv_instance(&wait_evts, &adv_duration);

        ble_mesh_adv_task_wait(wait_evts, adv_duration, &recv_evts);

        wait_evts &= ~recv_evts;

#if (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_SERVER
        if (recv_evts & ADV_TASK_PROXY_ADV_UPD_EVT) {
            adv_insts[BLE_MESH_ADV_PROXY_INS].busy = false;
            recv_evts &= ~ADV_TASK_PROXY_ADV_UPD_EVT;
        }
#endif

        /**
         * `recv_evts == ADV_TASK_PKT_SEND_EVT` indicates that new packets
         * have been placed into the queue, and the advertising instances started
         * previous have not yet stopped.
         */
        if (recv_evts == ADV_TASK_PKT_SEND_EVT) {
            continue;
        }

        recv_evts = received_adv_evts_handle(recv_evts);

        if (recv_evts) {
            BT_ERR("Remain evts %08x to handle", recv_evts);
        }
    }
}

void bt_mesh_adv_update(void)
{
#if (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_SERVER
    BT_DBG("Mesh Proxy Advertising stopped manually");
    bt_mesh_proxy_server_adv_stop();
    if (adv_insts[BLE_MESH_ADV_PROXY_INS].busy) {
        ble_mesh_adv_task_wakeup(ADV_TASK_PROXY_ADV_UPD_EVT);
    }
#endif
}

void bt_mesh_adv_init(void)
{
    bt_mesh_adv_common_init();

    adv_insts = bt_mesh_get_adv_insts_set();
    adv_queue = bt_mesh_adv_queue_get();

#if CONFIG_BLE_MESH_RELAY_ADV_BUF
    bt_mesh_relay_adv_init();
#endif

#if CONFIG_BLE_MESH_SUPPORT_BLE_ADV
    bt_mesh_ble_adv_init();
#endif

    bt_mesh_adv_task_init(adv_thread);
}

#if CONFIG_BLE_MESH_DEINIT
void bt_mesh_adv_deinit(void)
{
    bt_mesh_adv_task_deinit();

    bt_mesh_adv_common_deinit();

    adv_insts = NULL;

#if CONFIG_BLE_MESH_RELAY_ADV_BUF
    bt_mesh_relay_adv_deinit();
#endif

#if CONFIG_BLE_MESH_SUPPORT_BLE_ADV
    bt_mesh_ble_adv_deinit();
#endif
}
#endif /* CONFIG_BLE_MESH_DEINIT */
