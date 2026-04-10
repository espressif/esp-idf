/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "esp_hci_internal.h"
#include "common/hci_driver_h4.h"
#include "common/hci_driver_util.h"

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

#define HCI_H4_SM_W4_PKT_TYPE   0
#define HCI_H4_SM_W4_HEADER     1
#define HCI_H4_SM_W4_PAYLOAD    2
#define HCI_H4_SM_COMPLETED     3

struct hci_h4_input_buffer {
    const uint8_t *buf;
    uint16_t len;
};

static int
hci_h4_frame_start(struct hci_h4_sm *rxs, uint8_t pkt_type)
{
    rxs->pkt_type = pkt_type;
    rxs->len = 0;
    rxs->exp_len = 0;

    switch (rxs->pkt_type) {
    case HCI_H4_CMD:
        rxs->min_len = 3;
        break;
    case HCI_H4_ACL:
    case HCI_H4_ISO:
        rxs->min_len = 4;
        break;
#if (!CONFIG_BT_CONTROLLER_ENABLED)
    case HCI_H4_EVT:
        rxs->min_len = 2;
        break;
#endif // (!CONFIG_BT_CONTROLLER_ENABLED)
    default:
        /* !TODO: Sync loss. Need to wait for reset. */
        return -1;
    }

    return 0;
}

static int
hci_h4_ib_consume(struct hci_h4_input_buffer *ib, uint16_t len)
{
    HCI_TRANS_ASSERT((ib->len >= len), ib->len, len);

    ib->buf += len;
    ib->len -= len;

    return len;
}

static int
hci_h4_ib_pull_min_len(struct hci_h4_sm *rxs,
                       struct hci_h4_input_buffer *ib)
{
    uint16_t len;

    len = min(ib->len, rxs->min_len - rxs->len);
    memcpy(&rxs->hdr[rxs->len], ib->buf, len);

    rxs->len += len;
    hci_h4_ib_consume(ib, len);


    return rxs->len != rxs->min_len;
}

static int
hci_h4_sm_w4_header(struct hci_h4_sm *h4sm, struct hci_h4_input_buffer *ib)
{
    int rc;
    uint16_t conn_handle = 0;

    rc = hci_h4_ib_pull_min_len(h4sm, ib);
    if (rc) {
        /* need more data */
        return 1;
    }

    switch (h4sm->pkt_type) {
    case HCI_H4_CMD:
        HCI_TRANS_ASSERT(h4sm->allocs && h4sm->allocs->cmd, 0, 0);
        h4sm->pkt = h4sm->allocs->cmd();
        if (!h4sm->pkt) {
            return -1;
        }

        memcpy(h4sm->pkt->data, h4sm->hdr, h4sm->len);
        h4sm->exp_len = h4sm->hdr[2] + 3;
        break;
    case HCI_H4_ACL:
        conn_handle = btdm_get_le16(&h4sm->hdr[0]) & HCI_INTERNAL_CONN_MASK;
#if UC_BT_CTRL_BLE_IS_ENABLE
        if (HCI_INTERNAL_CONN_IS_BLE(conn_handle)) {
            h4sm->om = h4sm->allocs->acl();
            if (!h4sm->om) {
                return -1;
            }

            if (ble_mbuf_append(h4sm->om, h4sm->hdr, h4sm->len)) {
                return -1;
            }
            h4sm->exp_len = btdm_get_le16(&h4sm->hdr[2]) + 4;
            break;
        }
#endif // UC_BT_CTRL_BLE_IS_ENABLE
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
        if (HCI_INTERNAL_CONN_IS_BREDR(conn_handle)) {
            h4sm->exp_len = btdm_get_le16(&h4sm->hdr[2]) + 4;
            h4sm->pkt = h4sm->allocs->bredr_acl(conn_handle);
            if (!h4sm->pkt) {
                return -1;
            }
            memcpy(h4sm->pkt->data, h4sm->hdr, h4sm->len);
            break;
        }
        return -1;
    case HCI_H4_SYNC:
        conn_handle = btdm_get_le16(&h4sm->hdr[0]) & HCI_INTERNAL_CONN_MASK;
        h4sm->exp_len = h4sm->hdr[2] + 3;
        h4sm->pkt = h4sm->allocs->sync(conn_handle);
        if (!h4sm->pkt) {
            return -1;
        }
        memcpy(h4sm->pkt->data, h4sm->hdr, h4sm->len);
        break;
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE
#if !CONFIG_BT_CONTROLLER_ENABLED
    case HCI_H4_EVT:
        if (h4sm->hdr[0] == BLE_HCI_EVCODE_LE_META) {
            /* For LE Meta event we need 3 bytes to parse header */
            h4sm->min_len = 3;
            rc = hci_h4_ib_pull_min_len(h4sm, ib);
            if (rc) {
                /* need more data */
                return 1;
            }
        }

        HCI_TRANS_ASSERT(h4sm->allocs && h4sm->allocs->evt, 0, 0);

        /* We can drop legacy advertising events if there's no free buffer in
         * discardable pool.
         */
        if (h4sm->hdr[2] == BLE_HCI_LE_SUBEV_ADV_RPT) {
            h4sm->buf = h4sm->allocs->evt(1);
        } else {
            h4sm->buf = h4sm->allocs->evt(0);
            if (!h4sm->buf) {
                return -1;
            }
        }

        if (h4sm->buf) {
            memcpy(h4sm->buf, h4sm->hdr, h4sm->len);
        }

        h4sm->exp_len = h4sm->hdr[1] + 2;
        break;
#endif // !CONFIG_BT_CONTROLLER_ENABLED
#if CONFIG_BT_LE_ISO_SUPPORT
    case HCI_H4_ISO:
        h4sm->exp_len = (btdm_get_le16(&h4sm->hdr[2]) & 0x7fff) + 4;
        h4sm->buf = h4sm->allocs->iso(h4sm->exp_len);
        if (!h4sm->buf) {
            return -1;
        }

        memcpy(h4sm->buf, h4sm->hdr, h4sm->len);
        break;
#endif // CONFIG_BT_LE_ISO_SUPPORT
    default:
        return -2;
    }

    return 0;
}

static int
hci_h4_sm_w4_payload(struct hci_h4_sm *h4sm,
                     struct hci_h4_input_buffer *ib)
{
    uint16_t len;

    len = min(ib->len, h4sm->exp_len - h4sm->len);
    switch (h4sm->pkt_type) {
    case HCI_H4_CMD:
        if (h4sm->pkt) {
            memcpy(&h4sm->pkt->data[h4sm->len], ib->buf, len);
        }
        break;
#if !CONFIG_BT_CONTROLLER_ENABLED
        case HCI_H4_EVT:
#endif // !CONFIG_BT_CONTROLLER_ENABLED
    case HCI_H4_ISO:
        if (h4sm->buf) {
            memcpy(&h4sm->buf[h4sm->len], ib->buf, len);
        }
        break;
    case HCI_H4_ACL:
        uint16_t conn_handle = btdm_get_le16(&h4sm->hdr[0]) & HCI_INTERNAL_CONN_MASK;
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
        if (HCI_INTERNAL_CONN_IS_BREDR(conn_handle)) {
            memcpy(&h4sm->pkt->data[h4sm->len], ib->buf, len);
            break;
        }
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE
#if UC_BT_CTRL_BLE_IS_ENABLE
        if (HCI_INTERNAL_CONN_IS_BLE(conn_handle)) {
            uint16_t mbuf_len = BLE_MBUF_PKTLEN(h4sm->om);
            int rc = ble_mbuf_append(h4sm->om, ib->buf, len);
            if (rc) {
                /* Some data may already be appended so need to adjust h4sm only by
                 * the size of appended data.
                 */
                len = BLE_MBUF_PKTLEN(h4sm->om) - mbuf_len;
                h4sm->len += len;
                hci_h4_ib_consume(ib, len);
                return -1;
            }
        }
        break;
#endif // UC_BT_CTRL_BLE_IS_ENABLE
    default:
        return -2;
    }

    h4sm->len += len;
    hci_h4_ib_consume(ib, len);

    /* return 1 if need more data */
    return h4sm->len != h4sm->exp_len;
}

static void
hci_h4_sm_completed(struct hci_h4_sm *h4sm)
{
    int rc;
    uint8_t data_source = 0xFF;

    switch (h4sm->pkt_type) {
#if CONFIG_BT_CONTROLLER_ENABLED
    case HCI_H4_ACL:
        uint16_t conn_handle = btdm_get_le16(&h4sm->hdr[0]) & HCI_INTERNAL_CONN_MASK;
#if UC_BT_CTRL_BLE_IS_ENABLE
        if (HCI_INTERNAL_CONN_IS_BLE(conn_handle)) {
            if (h4sm->om) {
                rc = h4sm->frame_cb(h4sm->pkt_type, h4sm->om, h4sm->len, HCI_DRIVER_LE_ACL);
                HCI_TRANS_ASSERT(rc == 0, rc, 0);
                h4sm->om = NULL;
            }
        }
#endif // UC_BT_CTRL_BLE_IS_ENABLE
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
        if (HCI_INTERNAL_CONN_IS_BREDR(conn_handle)) {
            if (h4sm->buf) {
                rc = h4sm->frame_cb(h4sm->pkt_type, (void *)h4sm->buf, h4sm->len, HCI_DRIVER_BREDR_ACL);
                HCI_TRANS_ASSERT(rc == 0, rc, 0);
                h4sm->buf = NULL;
            }
        }
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE
        break;
    case HCI_H4_CMD:
    case HCI_H4_ISO:
        if (h4sm->buf) {
            switch(h4sm->pkt_type) {
                case HCI_H4_CMD:
                    data_source = HCI_DRIVER_CMD;
                    break;
                case HCI_H4_ISO:
                    data_source = HCI_DRIVER_LE_ISO;
                    break;
            }

            rc = h4sm->frame_cb(h4sm->pkt_type, (void *)h4sm->buf, h4sm->len, data_source);
            h4sm->buf = NULL;
        }
        break;
#else
    case HCI_H4_CMD:
    case HCI_H4_EVT:
        if (h4sm->buf) {
            rc = h4sm->frame_cb(h4sm->pkt_type, h4sm->buf);
            if (rc != 0) {
                ble_transport_free(h4sm->buf);
            }
            h4sm->buf = NULL;
        }
        break;
    case HCI_H4_ACL:
    case HCI_H4_ISO:
        if (h4sm->om) {
            rc = h4sm->frame_cb(h4sm->pkt_type, h4sm->om);
            if (rc != 0) {
                os_mbuf_free_chain(h4sm->om);
            }
            h4sm->om = NULL;
        }
        break;
#endif // CONFIG_BT_CONTROLLER_ENABLED
    default:
        assert(0);
        break;
    }
}

static int
hci_h4_sm_free_buf(struct hci_h4_sm *h4sm)
{
    switch (h4sm->pkt_type) {
        case HCI_H4_CMD:
            if (h4sm->buf) {
                h4sm->frees->cmd(h4sm->buf);
                h4sm->buf = NULL;
            }
            break;
#if (!CONFIG_BT_CONTROLLER_ENABLED)
        case HCI_H4_EVT:
            if (h4sm->buf) {
                h4sm->frees->evt(h4sm->buf);
                h4sm->buf = NULL;
            }
            break;
#endif  // (!CONFIG_BT_CONTROLLER_ENABLED)
        case HCI_H4_ACL:
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
            uint16_t conn_handle = btdm_get_le16(&h4sm->hdr[0]) & HCI_INTERNAL_CONN_MASK;
            if (HCI_INTERNAL_CONN_IS_BLE(conn_handle)) {
                if (h4sm->om) {
                    h4sm->frees->acl(h4sm->om);
                    h4sm->om = NULL;
                }
            } else {
                if (h4sm->pkt) {
                    h4sm->frees->bredr_acl(h4sm->pkt);
                    h4sm->pkt = NULL;
                }
            }
#else
            if (h4sm->om) {
                h4sm->frees->acl(h4sm->om);
                h4sm->om = NULL;
            }
#endif
            break;
        case HCI_H4_ISO:
            if (h4sm->buf) {
                h4sm->frees->iso(h4sm->buf);
                h4sm->buf = NULL;
            }
            break;
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
        case HCI_H4_SYNC:
            if (h4sm->pkt) {
                h4sm->frees->sync(h4sm->pkt);
                h4sm->pkt = NULL;
            }
            break;
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE
        default:
            break;
    }

    return 0;
}

int
hci_h4_sm_rx(struct hci_h4_sm *h4sm, const uint8_t *buf, uint16_t len)
{
    struct hci_h4_input_buffer ib = {
        .buf = buf,
        .len = len,
    };

    int rc = 0;
    while (ib.len && (rc >= 0)) {
        rc = 0;
        switch (h4sm->state) {
        case HCI_H4_SM_W4_PKT_TYPE:
            if (hci_h4_frame_start(h4sm, ib.buf[0]) < 0) {
                return -1;
            }

            hci_h4_ib_consume(&ib, 1);
            h4sm->state = HCI_H4_SM_W4_HEADER;
        /* no break */
        case HCI_H4_SM_W4_HEADER:
            rc = hci_h4_sm_w4_header(h4sm, &ib);
            if (rc) {
                break;
            }

            h4sm->state = HCI_H4_SM_W4_PAYLOAD;
        /* no break */
        case HCI_H4_SM_W4_PAYLOAD:
            rc = hci_h4_sm_w4_payload(h4sm, &ib);
            if (rc) {
                break;
            }

            h4sm->state = HCI_H4_SM_COMPLETED;
        /* no break */
        case HCI_H4_SM_COMPLETED:
            hci_h4_sm_completed(h4sm);
            h4sm->state = HCI_H4_SM_W4_PKT_TYPE;
            break;
        default:
            return -1;
        }
    }

    if (rc < 0) {
        hci_h4_sm_free_buf(h4sm);
        h4sm->state = HCI_H4_SM_W4_PKT_TYPE;
        return -1;
    }
    /* Calculate consumed bytes
     *
     * Note: we should always consume some bytes unless there is an oom error.
     * It's also possible that we have an oom error but already consumed some
     * data, in such case just return success and error will be returned on next
     * pass.
     */
    len = len - ib.len;
    if (len == 0) {
        HCI_TRANS_ASSERT((rc < 0), rc, ib.len);
        return -1;
    }

    return len;
}

void
hci_h4_sm_init(struct hci_h4_sm *h4sm, const struct hci_h4_allocators *allocs, const struct hci_h4_frees *frees,
               hci_h4_frame_cb *frame_cb)
{
    memset(h4sm, 0, sizeof(*h4sm));
    h4sm->allocs = allocs;
    h4sm->frees = frees;
    h4sm->frame_cb = frame_cb;
}
