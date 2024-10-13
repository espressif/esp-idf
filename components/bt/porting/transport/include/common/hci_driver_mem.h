/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _H_HCI_DRIVER_MEM_
#define _H_HCI_DRIVER_MEM_
#include <stdint.h>
#include "os/os_mbuf.h"

void *hci_driver_mem_cmd_alloc(void);

void *hci_driver_mem_evt_alloc(int discardable);

struct os_mbuf *hci_driver_mem_acl_alloc(void);

struct os_mbuf *hci_driver_mem_acl_len_alloc(uint32_t len);

struct os_mbuf *hci_driver_mem_iso_alloc(void);

struct os_mbuf *hci_driver_mem_iso_len_alloc(uint32_t len);

extern const struct hci_h4_allocators s_hci_driver_mem_alloc;

#endif // _H_HCI_DRIVER_MEM_
