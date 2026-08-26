/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include "common/hci_driver_mem.h"
#include "common/hci_driver_h4.h"
#include "esp_hci_internal.h"

void *
hci_driver_mem_cmd_alloc(void)
{
    return r_ble_hci_trans_buf_alloc(ESP_HCI_INTERNAL_BUF_CMD);
}

void *
hci_driver_mem_evt_alloc(int discardable)
{
    /* The controller shouldn't invoke this. */
    assert(0);
    return NULL;
}

struct os_mbuf *
hci_driver_mem_acl_alloc(void)
{
    return os_msys_get_pkthdr(0, ESP_HCI_INTERNAL_ACL_MBUF_LEADINGSPCAE);
}

struct os_mbuf *
hci_driver_mem_acl_len_alloc(uint32_t len)
{
    return os_msys_get_pkthdr(len, ESP_HCI_INTERNAL_ACL_MBUF_LEADINGSPCAE);
}

struct os_mbuf *
hci_driver_mem_iso_alloc(void)
{
     return os_msys_get_pkthdr(0, ESP_HCI_INTERNAL_ACL_MBUF_LEADINGSPCAE);
}

struct os_mbuf *
hci_driver_mem_iso_len_alloc(uint32_t len)
{
     return os_msys_get_pkthdr(len, ESP_HCI_INTERNAL_ACL_MBUF_LEADINGSPCAE);
}

const struct hci_h4_allocators s_hci_driver_mem_alloc = {
    .cmd = hci_driver_mem_cmd_alloc,
    .evt = hci_driver_mem_evt_alloc,
    .acl = hci_driver_mem_acl_alloc,
    .iso = hci_driver_mem_iso_alloc,
};

const struct hci_h4_frees s_hci_driver_mem_free = {
    .cmd = r_ble_hci_trans_buf_free,
    .evt = r_ble_hci_trans_buf_free,
    .acl = os_mbuf_free_chain,
    .iso = os_mbuf_free_chain,
};
