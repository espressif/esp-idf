/*
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 * SPDX-FileCopyrightText: 2023 Nordic Semiconductor
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_COMMON_ISO_H_
#define HOST_COMMON_ISO_H_

#include <stdint.h>

#include "sdkconfig.h"

#if CONFIG_BT_BLUEDROID_ENABLED
#include "bluedroid/iso.h"
#else
#include "nimble/iso.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct net_buf;

struct bt_le_iso_cb {
    void (*cis_dis)(struct net_buf *buf);
    void (*cis_est)(struct net_buf *buf);
    void (*cis_est_v2)(struct net_buf *buf);
    void (*cis_req)(struct net_buf *buf);
    void (*big_create)(struct net_buf *buf);
    void (*big_term)(struct net_buf *buf);
    void (*big_est)(struct net_buf *buf);
    void (*big_lost)(struct net_buf *buf);
    void (*biginfo)(struct net_buf *buf);

    sys_snode_t node;
};

/* Passed to the sent callback; times are in us on the LC clock.
 *
 * Each entry of sdu[] is one SDU with the information of its first Data PDU,
 * never of a later fragment or a dropped packet. sdu_ref_time is the reference
 * time of the SDU ([Vol 6] Part G, Section 3): tx_time_stamp minus time_offset
 * for framed PDUs, tx_time_stamp plus j SDU_Intervals for the j-th unframed SDU
 * of the event (from 0), which HCI_LE_Read_ISO_TX_Sync cannot tell.
 *
 * Unframed: an entry is reported when the last Data PDU of the SDU is
 * recycled. If its Data PDUs are recycled over several times, e.g. a CIS with
 * FT > 1, it is also reported early at the end of the first one with more set,
 * and the final report counts the whole SDU. Framed: one
 * entry per Data PDU holding the start of an SDU; of several SDUs in one PDU
 * only the last.
 *
 * The layout only grows: new fields go at the end and version is incremented.
 */
#define BT_ISO_TX_CB_INFO_VERSION   1
#define BT_ISO_TX_CB_SDU_MAX        4

struct bt_iso_tx_cb_info {
    uint8_t version;                /* BT_ISO_TX_CB_INFO_VERSION */
    uint8_t framing;                /* 0x00 unframed: sdu[].unf valid;
                                     * 0x01 framed segmentable: sdu[].fra valid;
                                     * 0x02 framed unsegmented: sdu[].fra valid
                                     */
    uint8_t sdu_cnt;                /* Valid entries in sdu[] */
    uint8_t sdu_dropped;            /* SDUs not reported because sdu[] was full */

    struct bt_iso_tx_cb_sdu {
        uint16_t seq_num;           /* Packet_Sequence_Number of the SDU */
        uint32_t tx_time_stamp;     /* TX_Time_Stamp: CIG reference point or BIG anchor point */
        uint32_t sdu_ref_time;      /* Reference time of the SDU, see above */
        uint32_t sdu_hdl_time;      /* LC time when ISOAL handled the first Data PDU of the SDU */
        uint32_t sdu_rec_time;      /* LC time of the recycling that reported the entry */
        union {
            struct {
                uint8_t pdu_cnt;        /* Data PDUs (fragments) of the SDU recycled so far */
                uint8_t txd_cnt;        /* Those of them sent at least once */
                uint8_t late : 1;       /* A fragment was handled at or after tx_time_stamp */
                uint8_t more : 1;       /* Early report: more Data PDUs to come */
                uint32_t sdu_end_time;  /* LC time when ISOAL handled the last part of the SDU */
            } unf;                      /* Unframed PDUs */
            struct {
                uint32_t time_offset : 24;  /* Time_Offset, tx_time_stamp - sdu_ref_time */
            } fra;                      /* Framed PDUs */
        };
    } sdu[BT_ISO_TX_CB_SDU_MAX];
};

int bt_le_iso_cb_register(struct bt_le_iso_cb *cb);

void bt_le_iso_cb_unregister(struct bt_le_iso_cb *cb);

void bt_le_iso_handle_hci_event(uint8_t *data, size_t data_len);

void bt_le_iso_handle_tx_comp(uint8_t *data, size_t data_len);

void bt_le_iso_handle_rx_data(uint8_t *data, size_t data_len);

int bt_le_iso_tx(struct bt_iso_chan *chan,
                 struct net_buf *buf,
                 uint16_t seq_num,
                 uint32_t time_stamp,
                 bool ts_valid);

int bt_le_iso_rx(const uint8_t *data, uint16_t len, void *arg);

int bt_le_iso_disconnect(uint16_t conn_handle, uint8_t reason);

void bt_le_iso_state_reset(void);

size_t bt_le_iso_report_busy(void);

int bt_le_iso_init(void);

void bt_le_iso_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* HOST_COMMON_ISO_H_ */
