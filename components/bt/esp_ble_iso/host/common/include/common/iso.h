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

#include "nimble/iso.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bt_le_iso_cb {
    void (*cis_dis)(struct net_buf *buf);
    void (*cis_est)(struct net_buf *buf);
    void (*cis_req)(struct net_buf *buf);
    void (*big_create)(struct net_buf *buf);
    void (*big_term)(struct net_buf *buf);
    void (*big_est)(struct net_buf *buf);
    void (*big_lost)(struct net_buf *buf);
    void (*biginfo)(struct net_buf *buf);

    sys_snode_t node;
};

struct bt_iso_tx_cb_info {
    uint16_t completed_pkts;
    uint16_t not_txd_pkts;

    uint8_t pkt_cnt;
    struct {
        uint16_t seq_num;
        uint32_t tx_time_stamp;
        uint32_t time_offset : 24;
        uint32_t drift : 1;
        uint32_t sdu_ref_time;
        uint32_t sdu_hdl_time;
        uint32_t sdu_rec_time;
    } pkt[2];
};

int bt_le_iso_cb_register_safe(struct bt_le_iso_cb *cb);

void bt_le_iso_cb_unregister_safe(struct bt_le_iso_cb *cb);

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

int bt_le_iso_init(void);

void bt_le_iso_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* HOST_COMMON_ISO_H_ */
