/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Structure and functions for returning ROM global layout
 *
 * This is for address symbols defined in the linker script, which may change during ECOs.
 */
typedef struct {
    void *dram0_stack_shared_mem_start;
    void *dram0_rtos_reserved_start;
    void *stack_sentry;
    void *stack;

    /* BTDM data */
    void *data_start_btdm;
    void *data_end_btdm;
    void *bss_start_btdm;
    void *bss_end_btdm;
    void *data_start_btdm_rom;
    void *data_end_btdm_rom;
    void *data_start_interface_btdm;
    void *data_end_interface_btdm;
    void *bss_start_interface_btdm;
    void *bss_end_interface_btdm;

    void *dram_start_phyrom;
    void *dram_end_phyrom;

    void *dram_start_usbdev_rom;
    void *dram_end_usbdev_rom;
    void *dram_start_uart_rom;
    void *dram_end_uart_rom;

} ets_rom_layout_t;

extern const ets_rom_layout_t * const ets_rom_layout_p;

#ifdef __cplusplus
}
#endif
