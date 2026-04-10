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

#ifndef _HCI_H4_H_
#define _HCI_H4_H_

#include <stdint.h>
#include "esp_hci_internal.h"

#define HCI_H4_NONE      0x00
#define HCI_H4_CMD       0x01
#define HCI_H4_ACL       0x02
#define HCI_H4_SYNC      0x03
#define HCI_H4_EVT       0x04
#define HCI_H4_ISO       0x05

typedef hci_driver_packet_t *(hci_h4_alloc_cmd)(void);
typedef void *(hci_h4_alloc_evt)(int);
typedef struct ble_mbuf *(hci_h4_alloc_acl)(void);
typedef void *(hci_h4_alloc_iso)(uint32_t);
#if CONFIG_BTDM_CTRL_MODE_BR_EDR_ONLY || CONFIG_BTDM_CTRL_MODE_BTDM
typedef hci_driver_packet_t *(hci_h4_alloc_sync)(uint16_t);
typedef hci_driver_packet_t *(hci_h4_alloc_bredr_acl)(uint16_t);
#endif // CONFIG_BTDM_CTRL_MODE_BR_EDR_ONLY || CONFIG_BTDM_CTRL_MODE_BTDM
struct hci_h4_allocators {
    hci_h4_alloc_cmd *cmd;
    hci_h4_alloc_acl *acl;
    hci_h4_alloc_evt *evt;
    hci_h4_alloc_iso *iso;
#if CONFIG_BTDM_CTRL_MODE_BR_EDR_ONLY || CONFIG_BTDM_CTRL_MODE_BTDM
    hci_h4_alloc_sync *sync;
    hci_h4_alloc_bredr_acl *bredr_acl;
#endif // CONFIG_BTDM_CTRL_MODE_BR_EDR_ONLY || CONFIG_BTDM_CTRL_MODE_BTDM
};

extern const struct hci_h4_allocators hci_h4_allocs_from_ll;
extern const struct hci_h4_allocators hci_h4_allocs_from_hs;

typedef void (hci_h4_free_cmd)(void * ptr);
typedef void (hci_h4_free_evt)(void * ptr);
typedef int  (hci_h4_free_acl)(struct ble_mbuf *om);
typedef void  (hci_h4_free_iso)(void *ptr);
typedef void (hci_h4_free_le_evt)(uint8_t *buf);
#if CONFIG_BTDM_CTRL_MODE_BR_EDR_ONLY || CONFIG_BTDM_CTRL_MODE_BTDM
typedef void (hci_h4_free_sync)(void *ptr);
typedef void (hci_h4_free_bredr_acl)(void *ptr);
#endif // CONFIG_BTDM_CTRL_MODE_BR_EDR_ONLY || CONFIG_BTDM_CTRL_MODE_BTDM
struct hci_h4_frees {
    hci_h4_free_cmd *cmd;
    hci_h4_free_acl *acl;
    hci_h4_free_evt *evt;
    hci_h4_free_iso *iso;
    hci_h4_free_le_evt *le_evt;
#if CONFIG_BTDM_CTRL_MODE_BR_EDR_ONLY || CONFIG_BTDM_CTRL_MODE_BTDM
    hci_h4_free_sync *sync;
    hci_h4_free_bredr_acl *bredr_acl;
#endif // CONFIG_BTDM_CTRL_MODE_BR_EDR_ONLY || CONFIG_BTDM_CTRL_MODE_BTDM
};

typedef int (hci_h4_frame_cb)(uint8_t pkt_type, void *data, int len, uint8_t data_source);

struct hci_h4_sm {
    uint8_t state;
    uint8_t pkt_type;
    uint8_t min_len;
    uint16_t len;
    uint16_t exp_len;
    uint8_t hdr[4];
    union {
        uint8_t *buf;
        struct ble_mbuf *om;
        hci_driver_packet_t *pkt;
    };

    const struct hci_h4_allocators *allocs;
    const struct hci_h4_frees *frees;
    hci_h4_frame_cb *frame_cb;
};

void hci_h4_sm_init(struct hci_h4_sm *h4sm,
                    const struct hci_h4_allocators *allocs,
                    const struct hci_h4_frees *frees,
                    hci_h4_frame_cb *frame_cb);

int hci_h4_sm_rx(struct hci_h4_sm *h4sm, const uint8_t *buf, uint16_t len);

#endif /* _HCI_H4_H_ */
