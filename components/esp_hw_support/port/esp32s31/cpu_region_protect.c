/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "sdkconfig.h"
#include "soc/soc.h"
#include "esp_cpu.h"
#include "esp_fault.h"
#include "hal/cache_ll.h"
#include "riscv/csr.h"
#if CONFIG_SPIRAM
#include "esp_private/esp_psram_extram.h"
#endif /* CONFIG_SPIRAM */

/* TODO: [ESP32S31] IDF-15238 */

#ifdef BOOTLOADER_BUILD
// Without L bit set
#define CONDITIONAL_NONE        0x0
#define CONDITIONAL_R           PMP_R
#define CONDITIONAL_RX          PMP_R | PMP_X
#define CONDITIONAL_RW          PMP_R | PMP_W
#define CONDITIONAL_RWX         PMP_R | PMP_W | PMP_X
#else
// With L bit set
#define CONDITIONAL_NONE        NONE
#define CONDITIONAL_R           R
#define CONDITIONAL_RX          RX
#define CONDITIONAL_RW          RW
#define CONDITIONAL_RWX         RWX
#endif

#define ALIGN_UP_TO_MMU_PAGE_SIZE(addr) (((addr) + (SOC_MMU_PAGE_SIZE) - 1) & ~((SOC_MMU_PAGE_SIZE) - 1))
#define ALIGN_DOWN_TO_MMU_PAGE_SIZE(addr)  ((addr) & ~((SOC_MMU_PAGE_SIZE) - 1))
#define ALIGN_UP(addr, align)  ((addr) & ~((align) - 1))

void esp_cpu_configure_region_protection(void)
{
    /* TODO: [ESP32S31] IDF-15238 */
}
