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

#ifndef H_BLE_HCI_ISO_
#define H_BLE_HCI_ISO_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ble_hci_iso_pkt_hdr {
    uint16_t conn_handle : 12,
             pb_flag : 2,
             ts_flag : 1;
    uint16_t data_load_len;
} __attribute__((packed));

struct ble_hci_iso_pkt {
    /* ISO header */
    uint16_t conn_handle;           /* CIS or BIS handle. */
    uint8_t  pb_flag : 2,           /* Packet boundary flag. */
             ts_flag : 1;           /* Timestamp flag. */
    uint16_t iso_data_load_len;     /* ISO Data Load length. */

    /* ISO Data Load */
    uint32_t time_stamp;            /* Timestamp. */
    uint16_t packet_seq;            /* Packet sequence number. */
    uint16_t iso_sdu_len;           /* SDU length. */
    uint8_t  ps_flag;               /* Packet status. */
    const uint8_t *sdu;             /* First byte of ISO SDU. */
};

struct ble_hci_iso_tx {
    struct ble_hci_iso_pkt pkt;
};

struct ble_hci_iso_rx {
    struct ble_hci_iso_pkt pkt;
};

struct ble_hci_iso_conn {
    uint16_t conn_handle;
    uint8_t  enabled : 1,
             pb_flag : 2,
             ts_flag : 1;
    uint32_t time_stamp;
    uint16_t pkt_seq_num;
    uint16_t sdu_len;
};

int ble_hci_set_iso_buf_sz(uint16_t pktlen, uint8_t max_pkts);

void ble_hci_get_iso_buf_size(uint16_t *pktlen, uint8_t *max_pkts);

void ble_hci_add_iso_avail_pkts(uint16_t delta);

struct ble_hci_iso_conn *ble_hci_iso_conn_find(uint16_t conn_handle);

int esp_ble_hci_iso_tx(uint16_t conn_handle, const uint8_t *sdu, uint16_t sdu_len,
                      bool ts_flag, uint32_t time_stamp, uint16_t pkt_seq_num);

#ifdef __cplusplus
}
#endif

#endif /* H_BLE_HCI_ISO_ */
