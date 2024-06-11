/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SUPPORT_BTDM            0
#define SUPPORT_BTBB            0
#define SUPPORT_WIFI            1
#define SUPPORT_USB_DWCOTG      0
#define SUPPORT_COEXIST         1
#define SUPPORT_MBEDTLS         0

/* Structure and functions for returning ROM global layout
 *
 * This is for address symbols defined in the linker script, which may change during ECOs.
 */
typedef struct {
    void *dram0_stack_shared_mem_start;
    void *dram0_rtos_reserved_start;
    void *stack_sentry;
    void *stack;

#if SUPPORT_BTDM
    void *data_start_btdm;
    void *data_end_btdm;
    void *bss_start_btdm;
    void *bss_end_btdm;
    void *data_start_btdm_rom;
    void *data_start_interface_btdm;
    void *data_end_interface_btdm;
    void *bss_start_interface_btdm;
    void *bss_end_interface_btdm;
#endif

#if SUPPORT_BTBB
    void *dram_start_btbbrom;
    void *dram_end_btbbrom;
#endif

#if SUPPORT_BTDM || SUPPORT_WIFI
    void *dram_start_phyrom;
    void *dram_end_phyrom;
#endif

#if SUPPORT_WIFI
    void *dram_start_net80211;
    void *dram_end_net80211;
    void *data_start_interface_net80211;
    void *data_end_interface_net80211;
    void *bss_start_interface_net80211;
    void *bss_end_interface_net80211;
    void *dram_start_pp;
    void *dram_end_pp;
    void *data_start_interface_pp;
    void *data_end_interface_pp;
    void *bss_start_interface_pp;
    void *bss_end_interface_pp;
#endif

#if SUPPORT_COEXIST
    void *dram_start_coexist;
    void *dram_end_coexist;
    void *data_start_interface_coexist;
    void *data_end_interface_coexist;
    void *bss_start_interface_coexist;
    void *bss_end_interface_coexist;
#endif

#if SUPPORT_MBEDTLS
    void *dram_start_mbedtls_rom;
    void *dram_end_mbedtls_rom;
#endif

#if SUPPORT_USB_DWCOTG
    void *dram_start_usb_dwcotg_rom;
    void *dram_end_usb_dwcotg_rom;
#else
    //Two reserved members are defined here, so the structure will not be broken,
    //please keep in mind that there is no memory can be released between
    //dram_start_usb_reserved_rom ~ dram_end_usb_reserved_rom.
    void *dram_start_usb_reserved_rom;
    void *dram_end_usb_reserved_rom;
#endif

    void *dram_start_uart_rom;
    void *dram_end_uart_rom;
} ets_rom_layout_t;

extern const ets_rom_layout_t *const ets_rom_layout_p;

#ifdef __cplusplus
}
#endif
