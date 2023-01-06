/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "riscv/rvruntime-frames.h"
#include "esp_private/panic_internal.h"

#include "esp_rom_sys.h"
#include "soc/periph_defs.h"
#include "hal/memprot_ll.h"

#define MCAUSE_ILLEGAL_INSTRUCTION  (2)

extern void panic_arch_fill_info(void *frame, panic_info_t *info);

extern void panic_soc_fill_info(void *frame, panic_info_t *info);

extern void esp_panic_handler(panic_info_t *info);

void __real_esp_panic_handler(panic_info_t *info);

void __real_panic_arch_fill_info(void *frame, panic_info_t *info);

static void dump_memprot_status_register(void)
{
    esp_rom_printf("violation intr status ->\n");
    esp_rom_printf("  core 0 dram0:    %x\n", memprot_ll_dram0_get_monitor_status_intr());
    esp_rom_printf("  core 0 iram0:    %x\n", memprot_ll_iram0_get_monitor_status_intr());
    esp_rom_printf("  core 0 peribus:  %x\n", memprot_ll_rtcfast_get_monitor_status_intr());
}

void __wrap_panic_arch_fill_info(void *frame, panic_info_t *info)
{
    RvExcFrame *regs = (RvExcFrame *) frame;

    if (regs->mcause == MCAUSE_ILLEGAL_INSTRUCTION) {

        uint32_t pms_violate_intr = memprot_ll_dram0_get_monitor_status_intr()  |
                                    memprot_ll_iram0_get_monitor_status_intr()  |
                                    memprot_ll_rtcfast_get_monitor_status_intr();

        if (pms_violate_intr) {
            regs->mcause = ETS_MEMPROT_ERR_INUM;
            panic_soc_fill_info(frame, info);
            return;
        }
    }

    __real_panic_arch_fill_info(frame, info);
}

void __wrap_esp_panic_handler(panic_info_t *info)
{
    dump_memprot_status_register();

    __real_esp_panic_handler(info);
}
