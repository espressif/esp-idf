/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
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
#include <os/os.h>
#include <os/os_mbuf.h>
#include "esp_hci_driver.h"
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

    rc = hci_h4_ib_pull_min_len(h4sm, ib);
    if (rc) {
        /* need more data */
        return 1;
    }

    switch (h4sm->pkt_type) {
    case HCI_H4_CMD:
        HCI_TRANS_ASSERT(h4sm->allocs && h4sm->allocs->cmd, 0, 0);
        h4sm->buf = h4sm->allocs->cmd();
        if (!h4sm->buf) {
            return -1;
        }

        memcpy(h4sm->buf, h4sm->hdr, h4sm->len);
        h4sm->exp_len = h4sm->hdr[2] + 3;

        break;
    case HCI_H4_ACL:
        HCI_TRANS_ASSERT(h4sm->allocs && h4sm->allocs->acl, 0, 0);
        h4sm->om = h4sm->allocs->acl();
        if (!h4sm->om) {
            return -1;
        }

        os_mbuf_append(h4sm->om, h4sm->hdr, h4sm->len);
        h4sm->exp_len = get_le16(&h4sm->hdr[2]) + 4;
        break;
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
    case HCI_H4_ISO:
        HCI_TRANS_ASSERT(h4sm->allocs && h4sm->allocs->iso, 0, 0);
        h4sm->om = h4sm->allocs->iso();
        if (!h4sm->om) {
            return -1;
        }

        os_mbuf_append(h4sm->om, h4sm->hdr, h4sm->len);
        h4sm->exp_len = (get_le16(&h4sm->hdr[2]) & 0x7fff) + 4;
        break;
    default:
        return -2;
    }

    return 0;
}

static int
hci_h4_sm_w4_payload(struct hci_h4_sm *h4sm,
                     struct hci_h4_input_buffer *ib)
{
    uint16_t mbuf_len;
    uint16_t len;
    int rc;

    len = min(ib->len, h4sm->exp_len - h4sm->len);
    switch (h4sm->pkt_type) {
    case HCI_H4_CMD:
#if (!CONFIG_BT_CONTROLLER_ENABLED)
    case HCI_H4_EVT:
#endif // (!CONFIG_BT_CONTROLLER_ENABLED)
        if (h4sm->buf) {
            memcpy(&h4sm->buf[h4sm->len], ib->buf, len);
        }
        break;
    case HCI_H4_ACL:
    case HCI_H4_ISO:
        HCI_TRANS_ASSERT(h4sm->om, h4sm->pkt_type, len);
        mbuf_len = OS_MBUF_PKTLEN(h4sm->om);
        rc = os_mbuf_append(h4sm->om, ib->buf, len);
        if (rc) {
            /* Some data may already be appended so need to adjust h4sm only by
             * the size of appended data.
             */
            len = OS_MBUF_PKTLEN(h4sm->om) - mbuf_len;
            h4sm->len += len;
            hci_h4_ib_consume(ib, len);
            return -1;
        }
        break;
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

    switch (h4sm->pkt_type) {
#if CONFIG_BT_CONTROLLER_ENABLED
    case HCI_H4_CMD:
        if (h4sm->buf) {
            HCI_TRANS_ASSERT(h4sm->frame_cb, 0, 0);
            rc = h4sm->frame_cb(h4sm->pkt_type, h4sm->buf);
            HCI_TRANS_ASSERT(rc == 0, rc, 0);
            h4sm->buf = NULL;
        }
        break;
    case HCI_H4_ACL:
    case HCI_H4_ISO:
        if (h4sm->om) {
            HCI_TRANS_ASSERT(h4sm->frame_cb, 0, 0);
            rc = h4sm->frame_cb(h4sm->pkt_type, h4sm->om);
            HCI_TRANS_ASSERT(rc == 0, rc, 0);
            h4sm->om = NULL;
        }
        break;
#else
    case HCI_H4_CMD:
        if (h4sm->buf) {
            HCI_TRANS_ASSERT(h4sm->frame_cb, 0, 0);
            rc = h4sm->frame_cb(h4sm->pkt_type, h4sm->buf);
            if (rc != 0) {
                HCI_TRANS_ASSERT(h4sm->frees && h4sm->frees->cmd, rc, (uint32_t)h4sm->frees);
                h4sm->frees->cmd(h4sm->buf);
            }
            h4sm->buf = NULL;
        }
        break;
    case HCI_H4_EVT:
        if (h4sm->buf) {
            HCI_TRANS_ASSERT(h4sm->frame_cb, 0, 0);
            rc = h4sm->frame_cb(h4sm->pkt_type, h4sm->buf);
            if (rc != 0) {
                HCI_TRANS_ASSERT(h4sm->frees && h4sm->frees->evt, rc, (uint32_t)h4sm->frees);
                h4sm->frees->evt(h4sm->buf);
            }
            h4sm->buf = NULL;
        }
        break;
    case HCI_H4_ACL:
        if (h4sm->om) {
            HCI_TRANS_ASSERT(h4sm->frame_cb, 0, 0);
            rc = h4sm->frame_cb(h4sm->pkt_type, h4sm->om);
            if (rc != 0) {
                HCI_TRANS_ASSERT(h4sm->frees && h4sm->frees->acl, rc, (uint32_t)h4sm->frees);
                h4sm->frees->acl(h4sm->om);
            }
            h4sm->om = NULL;
        }
        break;
    case HCI_H4_ISO:
        if (h4sm->om) {
            HCI_TRANS_ASSERT(h4sm->frame_cb, 0, 0);
            rc = h4sm->frame_cb(h4sm->pkt_type, h4sm->om);
            if (rc != 0) {
                HCI_TRANS_ASSERT(h4sm->frees && h4sm->frees->iso, rc, (uint32_t)h4sm->frees);
                h4sm->frees->iso(h4sm->om);
            }
            h4sm->om = NULL;
        }
        break;
#endif // CONFIG_BT_CONTROLLER_ENABLED
    default:
        HCI_TRANS_ASSERT(0, h4sm->pkt_type, 0);
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
            if (h4sm->om) {
                h4sm->frees->acl(h4sm->om);
                h4sm->om = NULL;
            }
            break;
        case HCI_H4_ISO:
            if (h4sm->om) {
                h4sm->frees->iso(h4sm->om);
                h4sm->om = NULL;
            }
            break;
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
