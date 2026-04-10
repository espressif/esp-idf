/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include "esp_hci_internal.h"
#include "common/hci_driver_mem.h"
#include "common/hci_driver_h4.h"

hci_driver_packet_t *
hci_driver_mem_cmd_alloc(void)
{
    return btdm_hci_trans_buf_alloc(HCI_H4_CMD, 0);
}

void *
hci_driver_mem_evt_alloc(int discardable)
{
    /* The controller shouldn't invoke this. */
    assert(0);
    return NULL;
}

struct ble_mbuf *
hci_driver_mem_acl_alloc(void)
{
    return ble_msys_get_pkthdr(0, ESP_HCI_INTERNAL_ACL_MBUF_LEADINGSPCAE);
}

struct ble_mbuf *
hci_driver_mem_acl_len_alloc(uint32_t len)
{
    return ble_msys_get_pkthdr(len, ESP_HCI_INTERNAL_ACL_MBUF_LEADINGSPCAE);
}

void *
hci_driver_mem_iso_alloc(uint32_t len)
{
    return malloc(len);
}

void
hci_driver_mem_iso_free(void *ptr)
{
    free(ptr);
}

struct ble_mbuf *
hci_driver_mem_iso_len_alloc(uint32_t len)
{
     return ble_msys_get_pkthdr(len, ESP_HCI_INTERNAL_ACL_MBUF_LEADINGSPCAE);
}

void
hci_driver_mem_cmd_free(void * ptr)
{
    hci_driver_packet_t *pkt = (hci_driver_packet_t *) ptr;
    btdm_hci_trans_buf_free(pkt);
}

void
hci_driver_mem_evt_free(void *ptr)
{
    hci_driver_packet_t *pkt = (hci_driver_packet_t *) ptr;
    btdm_hci_trans_buf_free(pkt);
}

#if UC_BTDM_CTRL_BR_EDR_IS_ENABLE
hci_driver_packet_t *
hci_driver_mem_bredr_acl_alloc(uint16_t handle)
{
    return btdm_hci_trans_buf_alloc(HCI_H4_ACL, handle);
}

void
hci_driver_mem_bredr_acl_free(void *ptr)
{
    hci_driver_packet_t *pkt = (hci_driver_packet_t *) ptr;
    bredr_hci_trans_acl_tx_done(pkt);
    bredr_hci_trans_acl_free(pkt);
}

hci_driver_packet_t *
hci_driver_mem_sync_alloc(uint16_t handle)
{
    return btdm_hci_trans_buf_alloc(HCI_H4_SYNC, handle);
}

void
hci_driver_mem_sync_free(void *ptr)
{
    hci_driver_packet_t *pkt = (hci_driver_packet_t *) ptr;
    bredr_hci_trans_sync_tx_done(pkt);
    bredr_hci_trans_sync_free(pkt);
}
#endif // UC_BTDM_CTRL_BR_EDR_IS_ENABLE

const struct hci_h4_allocators s_hci_driver_mem_alloc = {
    .cmd = hci_driver_mem_cmd_alloc,
    .evt = hci_driver_mem_evt_alloc,
    .acl = hci_driver_mem_acl_alloc,
    .iso = hci_driver_mem_iso_alloc,
#if UC_BTDM_CTRL_BR_EDR_IS_ENABLE
    .sync = hci_driver_mem_sync_alloc,
    .bredr_acl = hci_driver_mem_bredr_acl_alloc,
#endif // UC_BTDM_CTRL_BR_EDR_IS_ENABLE
};

const struct hci_h4_frees s_hci_driver_mem_free = {
    .cmd = hci_driver_mem_cmd_free,
    .evt = hci_driver_mem_evt_free,
    .acl = ble_mbuf_free_chain,
    .iso = hci_driver_mem_iso_free,
    .le_evt = r_ble_hci_trans_buf_free,
#if UC_BTDM_CTRL_BR_EDR_IS_ENABLE
    .sync = hci_driver_mem_sync_free,
    .bredr_acl = hci_driver_mem_bredr_acl_free,
#endif // UC_BTDM_CTRL_BR_EDR_IS_ENABLE
};
