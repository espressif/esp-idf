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

#include <string.h>
#include <assert.h>
#include "hci/ble_hci_iso.h"
#include "common/bt_target.h"
#include "osi/mutex.h"

#if (BLE_FEAT_ISO_EN == TRUE)

#define BLE_HCI_ISO_PB_FIRST_FRAG           0
#define BLE_HCI_ISO_PB_CONT_FRAG            1
#define BLE_HCI_ISO_PB_COMP_SDU             2
#define BLE_HCI_ISO_PB_LAST_FRAG            3

#define BLE_HCI_ISO_DATA_HDR_SZ             4
#define BLE_HCI_ISO_DATA_LOAD_TS_SZ         4
#define BLE_HCI_ISO_DATA_LOAD_HDR_SZ        4

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

static uint16_t ble_hs_iso_buf_sz;
static uint8_t ble_hs_iso_max_pkts;

#if (BLE_ISO_STD_FLOW_CTRL == TRUE)
/* Number of available ISO transmit buffers on the controller.
 * It must only be accessed while the host mutex is locked.
 */
static uint16_t ble_hs_iso_avail_pkts;
#endif // #if (BLE_ISO_STD_FLOW_CTRL == TRUE)

static struct ble_hci_iso_conn g_iso_conn[BLE_ISO_BIS_MAX_COUNT + BLE_ISO_CIS_MAX_COUNT];

int
ble_hci_set_iso_buf_sz(uint16_t pktlen, uint8_t max_pkts)
{
    if (pktlen == 0 || max_pkts == 0) {
        return 3;
    }

    ble_hs_iso_buf_sz = pktlen;
    ble_hs_iso_max_pkts = max_pkts;

#if (BLE_ISO_STD_FLOW_CTRL == TRUE)
    ble_hs_iso_avail_pkts = max_pkts;
#endif /* (BLE_ISO_STD_FLOW_CTRL) */

    HCI_TRACE_WARNING("ISO Flow Control:\n");
    HCI_TRACE_WARNING("          Length: %u\n", pktlen);
    HCI_TRACE_WARNING("           Count: %u\n", max_pkts);
    HCI_TRACE_WARNING("          Status: ");
#if (BLE_ISO_STD_FLOW_CTRL == TRUE)
    HCI_TRACE_WARNING("%s\n", "Standard");
#elif (BLE_ISO_NON_STD_FLOW_CTRL == TRUE)
    HCI_TRACE_WARNING("%s\n", "Non-standard");
#else
    HCI_TRACE_WARNING("%s\n", "Not support");
#endif

    return 0;
}

void
ble_hci_get_iso_buf_size(uint16_t *pktlen, uint8_t *max_pkts)
{
    assert(pktlen && max_pkts);

    *pktlen = ble_hs_iso_buf_sz;
    *max_pkts = ble_hs_iso_max_pkts;
}

#if (BLE_ISO_STD_FLOW_CTRL == TRUE)
void
ble_hci_add_iso_avail_pkts(uint16_t delta)
{
    osi_mutex_global_lock();

    if (ble_hs_iso_avail_pkts + delta > ble_hs_iso_max_pkts) {
        HCI_TRACE_ERROR("ISO_HS_RESET %u %u %u\n", ble_hs_iso_avail_pkts, delta, ble_hs_iso_max_pkts);
        // ble_hs_sched_reset(BLE_HS_ECONTROLLER);
        assert(0);
    } else {
        ble_hs_iso_avail_pkts += delta;
    }

    osi_mutex_global_unlock();
}
#endif /* (BLE_ISO_STD_FLOW_CTRL) */

#define BLE_ARRAY_SIZE(x)   (sizeof(x)/sizeof((x)[0]))

struct ble_hci_iso_conn *
ble_hci_iso_conn_find(uint16_t conn_handle)
{
    for (size_t i = 0; i < BLE_ARRAY_SIZE(g_iso_conn); i++) {
        struct ble_hci_iso_conn *conn = &g_iso_conn[i];

        if (conn->enabled &&
            conn->conn_handle == conn_handle) {
            return conn;
        }
    }

    return NULL;
}

int ble_hci_iso_conn_free(uint16_t conn_handle)
{
    for (size_t i = 0; i < BLE_ARRAY_SIZE(g_iso_conn); i++) {
        struct ble_hci_iso_conn *conn = &g_iso_conn[i];

        if (conn->enabled &&
            conn->conn_handle == conn_handle) {
            conn->enabled = 0;
            conn->conn_handle = 0;
            return 0;
        }
    }

    return 1;
}

static struct ble_hci_iso_conn *
ble_hci_iso_alloc_conn(uint16_t conn_handle)
{
    for (size_t i = 0; i < BLE_ARRAY_SIZE(g_iso_conn); i++) {
        struct ble_hci_iso_conn *conn = &g_iso_conn[i];

        if (!conn->enabled) {
            conn->enabled = 1;
            conn->conn_handle = conn_handle;
            conn->pkt_seq_num = 0;  /* Initialize to 0 */
            return conn;
        }
    }

    return NULL;
}

#if (BLE_ISO_STD_FLOW_CTRL == TRUE)
static uint8_t
ble_hci_iso_buf_needed(struct ble_hs_iso_conn *conn)
{
    uint16_t sdu_offset;
    uint16_t dl_len;
    uint8_t dlh_len;
    uint8_t count;

    dlh_len = (conn->ts_flag ? BLE_HCI_ISO_DATA_LOAD_TS_SZ : 0) + BLE_HCI_ISO_DATA_LOAD_HDR_SZ;
    sdu_offset = 0;
    count = 1;  /* 1 extra since framed pdu may be used */

    while (1) {
        dl_len = min(dlh_len + conn->sdu_len - sdu_offset, ble_hs_iso_buf_sz);

        count += 1;

        sdu_offset += dl_len - dlh_len;
        assert(sdu_offset <= conn->sdu_len);

        if (sdu_offset == conn->sdu_len) {
            break;
        }

        /* No data load header for continuation/last segment */
        dlh_len = 0;
    }

    return count;
}
#endif /* (BLE_ISO_STD_FLOW_CTRL) */

static void
ble_hci_iso_hdr_append(struct ble_hci_iso_conn *conn,
                          uint8_t *frag,
                          uint8_t pb_flag, uint16_t dl_len)
{
    uint32_t pkt_hdr;
    uint32_t dl_hdr;

    pkt_hdr = conn->conn_handle;
    pkt_hdr |= (pb_flag << 12);
    if (pb_flag == BLE_HCI_ISO_PB_FIRST_FRAG ||
        pb_flag == BLE_HCI_ISO_PB_COMP_SDU) {
        pkt_hdr |= (conn->ts_flag << 14);
    }
    pkt_hdr |= (dl_len << 16);

    memcpy(frag, &pkt_hdr, BLE_HCI_ISO_DATA_HDR_SZ);

    /* No data load header for continuation/last segment */
    if (pb_flag == BLE_HCI_ISO_PB_CONT_FRAG ||
        pb_flag == BLE_HCI_ISO_PB_LAST_FRAG) {
        return;
    }

    if (conn->ts_flag) {
        memcpy(frag + BLE_HCI_ISO_DATA_HDR_SZ, &conn->time_stamp, BLE_HCI_ISO_DATA_LOAD_TS_SZ);
    }

    dl_hdr = (conn->sdu_len << 16) | conn->pkt_seq_num;

    memcpy(frag + BLE_HCI_ISO_DATA_HDR_SZ + (conn->ts_flag ? BLE_HCI_ISO_DATA_LOAD_TS_SZ : 0),
           &dl_hdr, BLE_HCI_ISO_DATA_LOAD_HDR_SZ);
}

int
ble_hci_tx_iso_data(const uint8_t *data, uint16_t len, void *arg)
{
    int rc = 1;
#if CONFIG_BT_LE_ISO_SUPPORT
    // return ble_transport_to_ll_iso(data, len, arg);
    extern int ble_hci_trans_hs_iso_tx(const uint8_t *data, uint16_t length, void *arg);
    rc = ble_hci_trans_hs_iso_tx(data, len, arg);
#endif // #if CONFIG_BT_LE_ISO_SUPPORT
    return rc;
}

static int
ble_hci_iso_tx_now(struct ble_hci_iso_conn *conn, const uint8_t *sdu,
                      uint32_t time_stamp, uint16_t pkt_seq_num)
{
    uint8_t dlh_len;
    uint8_t *frag;
    int rc;

#if (BLE_ISO_STD_FLOW_CTRL == TRUE)
    /* Get the Controller ISO buffer needed for the SDU */
    uint8_t count = ble_hs_hci_iso_buf_needed(conn);

    /* Make sure the Controller ISO buffer can accommodate the SDU completely */
    if (count > ble_hs_iso_avail_pkts) {
        HCI_TRACE_ERROR("ISO flow control(%u/%u)!\n", count, ble_hs_iso_avail_pkts);
        return 1;
    }
#elif (BLE_ISO_NON_STD_FLOW_CTRL == TRUE)
    extern uint16_t ble_ll_iso_free_buf_num_get(uint16_t conn_handle);
    if (ble_ll_iso_free_buf_num_get(conn->conn_handle) == 0) {
        HCI_TRACE_ERROR("ISO flow control!\n");
        return 1;
    }
#endif

    dlh_len = (conn->ts_flag ? BLE_HCI_ISO_DATA_LOAD_TS_SZ : 0) + BLE_HCI_ISO_DATA_LOAD_HDR_SZ;

    frag = malloc(BLE_HCI_ISO_DATA_HDR_SZ + dlh_len + conn->sdu_len);
    if (frag == NULL) {
        HCI_TRACE_ERROR("frag is null\n");
        return 6;
    }

    ble_hci_iso_hdr_append(conn,
                              frag,
                              BLE_HCI_ISO_PB_COMP_SDU,
                              dlh_len + conn->sdu_len);

    memcpy(frag + BLE_HCI_ISO_DATA_HDR_SZ + dlh_len,
           sdu,
           conn->sdu_len);

    rc = ble_hci_tx_iso_data(frag, BLE_HCI_ISO_DATA_HDR_SZ + dlh_len + conn->sdu_len, NULL);
    if (rc) {
        HCI_TRACE_ERROR("iso tx failed\n");
        return 14;
    }

#if (BLE_ISO_STD_FLOW_CTRL == TRUE)
    /* If an ISO SDU is fragmented into fragments, flow control is not supported.
     *
     * Currently even if an SDU is larger than the ISO buffer size, fragmentation
     * will not happen here, the SDU will be posted to Controller completely.
     */
    ble_hs_iso_avail_pkts -= 1;
#endif /* (BLE_ISO_STD_FLOW_CTRL) */

    return 0;
}

int
esp_ble_hci_iso_tx(uint16_t conn_handle, const uint8_t *sdu, uint16_t sdu_len,
                  bool ts_flag, uint32_t time_stamp, uint16_t pkt_seq_num)
{
    struct ble_hci_iso_conn *conn;
    uint16_t max_sdu;
    int rc;

    osi_mutex_global_lock();

    /* For sending empty SDU, we could use an SDU with zero length. */
    if (sdu == NULL && sdu_len != 0) {
        HCI_TRACE_ERROR("Invalid iso sdu\n");
        rc = 3;
        goto end;
    }

    /* TODO:
     * Use Host functions to replace the above LL implementation.
     */
    max_sdu = 0x0FFF;

    if (sdu_len > max_sdu) {
        HCI_TRACE_ERROR("Too large iso sdu %d, max_sdu %d\n", sdu_len, max_sdu);
        rc = 8;
        goto end;
    }

    conn = ble_hci_iso_conn_find(conn_handle);
    if (!conn) {
        conn = ble_hci_iso_alloc_conn(conn_handle);
    }
    if (!conn) {
        rc = 7;
        goto end;
    }

    conn->ts_flag = ts_flag;
    conn->time_stamp = time_stamp;
    conn->pkt_seq_num = pkt_seq_num;
    conn->sdu_len = sdu_len;

    rc = ble_hci_iso_tx_now(conn, sdu, time_stamp, pkt_seq_num);
end:
    osi_mutex_global_unlock();
    return rc;
}

#endif // #if (BLE_FEAT_ISO_EN == TRUE)
