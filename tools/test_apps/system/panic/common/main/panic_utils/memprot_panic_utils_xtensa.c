/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_private/panic_internal.h"

#include "esp_rom_sys.h"
#include "soc/sensitive_reg.h"
#include "soc/periph_defs.h"
#include "hal/memprot_ll.h"

extern void esp_panic_handler(panic_info_t *info);

void __real_esp_panic_handler(panic_info_t *info);

static void dump_memprot_status_register(void)
{
    esp_rom_printf("violation intr status ->\n");
#if CONFIG_IDF_TARGET_ESP32S2
    esp_rom_printf("  core 0 dram0:    %x\n", memprot_ll_dram0_get_intr_on_bit());
    esp_rom_printf("  core 0 iram0:    %x\n", memprot_ll_iram0_get_intr_on_bit());
    esp_rom_printf("  core 0 peribus1: %x\n", REG_GET_FIELD(DPORT_PMS_PRO_DPORT_6_REG, DPORT_PMS_PRO_DPORT_ILG_INTR));
    esp_rom_printf("  core 0 peribus2: %x\n", REG_GET_FIELD(DPORT_PMS_PRO_AHB_3_REG,   DPORT_PMS_PRO_AHB_ILG_INTR));
#elif CONFIG_IDF_TARGET_ESP32S3
    uint32_t reg_val0 = 0, reg_val1 = 0;

    memprot_ll_dram0_get_monitor_status_intr(PRO_CPU_NUM, &reg_val0);
    memprot_ll_dram0_get_monitor_status_intr(APP_CPU_NUM, &reg_val1);
    esp_rom_printf("  core 0 dram0:    %x\n", reg_val0);
    esp_rom_printf("  core 1 dram0:    %x\n", reg_val1);

    memprot_ll_iram0_get_monitor_status_intr(PRO_CPU_NUM, &reg_val0);
    memprot_ll_iram0_get_monitor_status_intr(APP_CPU_NUM, &reg_val1);
    esp_rom_printf("  core 0 iram0:    %x\n", reg_val0);
    esp_rom_printf("  core 1 iram0:    %x\n", reg_val1);

    memprot_ll_rtcfast_get_monitor_status_intr(PRO_CPU_NUM, &reg_val0);
    memprot_ll_rtcfast_get_monitor_status_intr(APP_CPU_NUM, &reg_val1);
    esp_rom_printf("  core 0 peribus:  %x\n", reg_val0);
    esp_rom_printf("  core 1 peribus:  %x\n", reg_val1);
#endif
}

void __wrap_esp_panic_handler(panic_info_t *info)
{
    dump_memprot_status_register();

    __real_esp_panic_handler(info);
}
