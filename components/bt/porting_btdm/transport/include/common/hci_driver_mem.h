/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _H_HCI_DRIVER_MEM_
#define _H_HCI_DRIVER_MEM_
#include <stdint.h>
#include "esp_hci_internal.h"

hci_driver_packet_t *hci_driver_mem_cmd_alloc(void);

void *hci_driver_mem_evt_alloc(int discardable);

struct ble_mbuf *hci_driver_mem_acl_alloc(void);

struct ble_mbuf *hci_driver_mem_acl_len_alloc(uint32_t len);

void *hci_driver_mem_iso_alloc(uint32_t len);

struct ble_mbuf *hci_driver_mem_iso_len_alloc(uint32_t len);

extern const struct hci_h4_allocators s_hci_driver_mem_alloc;
extern const struct hci_h4_frees s_hci_driver_mem_free;
#if CONFIG_BTDM_CTRL_MODE_BR_EDR_ONLY || CONFIG_BTDM_CTRL_MODE_BTDM
hci_driver_packet_t *hci_driver_mem_bredr_acl_alloc(uint16_t len);

hci_driver_packet_t *hci_driver_mem_sync_alloc(uint16_t len);
#endif // CONFIG_BTDM_CTRL_MODE_BR_EDR_ONLY || CONFIG_BTDM_CTRL_MODE_BTDM

#endif // _H_HCI_DRIVER_MEM_
