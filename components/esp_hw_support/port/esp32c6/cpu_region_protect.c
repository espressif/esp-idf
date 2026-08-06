/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "sdkconfig.h"
#include "soc/soc.h"
#include "esp_cpu.h"
#include "esp_fault.h"
#include "esp_macros.h"
#include "pmp_layout.h"

#define ALIGN_UP_TO_MMU_PAGE_SIZE(addr)    ESP_ALIGN_UP(addr, SOC_MMU_PAGE_SIZE)
#define ALIGN_DOWN_TO_MMU_PAGE_SIZE(addr)  ESP_ALIGN_DOWN(addr, SOC_MMU_PAGE_SIZE)

static void esp_cpu_configure_invalid_regions(void)
{
    const unsigned PMA_NONE                            = PMA_L | PMA_EN;
    __attribute__((unused)) const unsigned PMA_RW      = PMA_L | PMA_EN | PMA_R | PMA_W;
    __attribute__((unused)) const unsigned PMA_RX      = PMA_L | PMA_EN | PMA_R | PMA_X;
    __attribute__((unused)) const unsigned PMA_RWX     = PMA_L | PMA_EN | PMA_R | PMA_W | PMA_X;

    // 1. Gap at bottom of address space
    PMA_ENTRY_SET_TOR(0, SOC_CPU_SUBSYSTEM_LOW, PMA_TOR | PMA_NONE);

    // 2. Gap between CPU subsystem region & IROM
    PMA_ENTRY_SET_TOR(1, SOC_CPU_SUBSYSTEM_HIGH, PMA_NONE);
    PMA_ENTRY_SET_TOR(2, SOC_IROM_MASK_LOW, PMA_TOR | PMA_NONE);

    // 3. Gap between ROM & RAM
    PMA_ENTRY_SET_TOR(3, SOC_DROM_MASK_HIGH, PMA_NONE);
    PMA_ENTRY_SET_TOR(4, SOC_IRAM_LOW, PMA_TOR | PMA_NONE);

    // 4. Gap between DRAM and I_Cache
    PMA_ENTRY_SET_TOR(5, SOC_IRAM_HIGH, PMA_NONE);
    PMA_ENTRY_SET_TOR(6, SOC_IROM_LOW, PMA_TOR | PMA_NONE);

    // 5. Gap between D_Cache & LP_RAM
    PMA_ENTRY_SET_TOR(7, SOC_DROM_HIGH, PMA_NONE);
    PMA_ENTRY_SET_TOR(8, SOC_RTC_IRAM_LOW, PMA_TOR | PMA_NONE);

    // 6. Gap between LP memory & peripheral addresses
    PMA_ENTRY_SET_TOR(9, SOC_RTC_IRAM_HIGH, PMA_NONE);
    PMA_ENTRY_SET_TOR(10, SOC_PERIPHERAL_LOW, PMA_TOR | PMA_NONE);

    // 7. End of address space
    PMA_ENTRY_SET_TOR(11, SOC_PERIPHERAL_HIGH, PMA_NONE);
    PMA_ENTRY_SET_TOR(12, UINT32_MAX, PMA_TOR | PMA_NONE);

    PMA_ENTRY_CFG_RESET(13);
    PMA_ENTRY_CFG_RESET(14);
    PMA_ENTRY_CFG_RESET(15);
}

#ifndef BOOTLOADER_BUILD
static void esp_cpu_configure_valid_regions(void)
{
    /* There are 3 configuration scenarios for SRAM in the application
     *
     * 1. Application build with CONFIG_ESP_SYSTEM_MEMPROT enabled
     *    - We split the SRAM into IRAM and DRAM such that IRAM region cannot be written to
     *      and DRAM region cannot be executed. We use _iram_text_end and _data_start markers to set the boundaries.
     *      We also lock these entries so the R/W/X permissions are enforced even for machine mode
     *
     * 2. Application build with CONFIG_ESP_SYSTEM_MEMPROT disabled
     *    - The IRAM-DRAM split is not enabled so we just need to ensure that access to only valid address ranges are successful
     *      so for that we set PMP to cover entire valid IRAM and DRAM region.
     *      We also lock these entries so the R/W/X permissions are enforced even for machine mode
     *
     * 3. CPU is in OCD debug mode
     *    - The IRAM-DRAM split is not enabled so that OpenOCD can write and execute from IRAM.
     *      We set PMP to cover entire valid IRAM and DRAM region.
     *      We also lock these entries so the R/W/X permissions are enforced even for machine mode
     */
    const unsigned NONE    = PMP_L;
    __attribute__((unused)) const unsigned R       = PMP_L | PMP_R;
    const unsigned RW      = PMP_L | PMP_R | PMP_W;
    const unsigned RX      = PMP_L | PMP_R | PMP_X;
    const unsigned RWX     = PMP_L | PMP_R | PMP_W | PMP_X;

    // 1. CPU Subsystem region - contains debug mode code and interrupt config registers
    const uint32_t pmpaddr0 = PMPADDR_NAPOT(SOC_CPU_SUBSYSTEM_LOW, SOC_CPU_SUBSYSTEM_HIGH);
    PMP_RESET_AND_ENTRY_SET(PMP_ENTRY_CPU_SUBSYSTEM, pmpaddr0, PMP_NAPOT | RWX);
    _Static_assert(SOC_CPU_SUBSYSTEM_LOW < SOC_CPU_SUBSYSTEM_HIGH, "Invalid CPU subsystem region");

    // 2.1 I-ROM
    PMP_RESET_AND_ENTRY_SET(PMP_ENTRY_IROM_LOW, SOC_IROM_MASK_LOW, NONE);
    PMP_RESET_AND_ENTRY_SET(PMP_ENTRY_IROM_HIGH, SOC_IROM_MASK_HIGH, PMP_TOR | RX);
    _Static_assert(SOC_IROM_MASK_LOW < SOC_IROM_MASK_HIGH, "Invalid I-ROM region");

    /* 2.2 D-ROM - redundant with the I-ROM entry above (same range, already
     * locked RX); kept as a separate pair only to pin the app's IRAM/DRAM split
     * to entries 5-7.
     *
     * C6's ROM mask is not a power of two, so ROM needs a TOR pair on entries
     * 1-2. Commit d4167fea60c (v6.0) dropped this D-ROM pair and moved the split
     * down onto entries 3-4 - which bootloaders up to v5.2.1 lock as D-ROM. A
     * locked PMP entry can't be reconfigured until CPU reset, so such an app
     * can't gain IRAM execute permission and resets before app_main().
     * v5.2.2 freed 3-4, so only pre-v5.2.2 C6 bootloaders break.
     */
    PMP_RESET_AND_ENTRY_SET(PMP_ENTRY_DROM_LOW, SOC_DROM_MASK_LOW, NONE);
    PMP_RESET_AND_ENTRY_SET(PMP_ENTRY_DROM_HIGH, SOC_DROM_MASK_HIGH, PMP_TOR | R);
    _Static_assert(SOC_DROM_MASK_LOW < SOC_DROM_MASK_HIGH, "Invalid D-ROM region");

    if (esp_cpu_dbgr_is_attached()) {
        // Anti-FI check that cpu is really in ocd mode
        ESP_FAULT_ASSERT(esp_cpu_dbgr_is_attached());

        // 3. IRAM and DRAM
        const uint32_t pmpaddr5 = PMPADDR_NAPOT(SOC_IRAM_LOW, SOC_IRAM_HIGH);
        PMP_RESET_AND_ENTRY_SET(5, pmpaddr5, PMP_NAPOT | RWX);
        _Static_assert(SOC_IRAM_LOW < SOC_IRAM_HIGH, "Invalid RAM region");
    } else {
#if CONFIG_ESP_SYSTEM_MEMPROT && CONFIG_ESP_SYSTEM_MEMPROT_PMP
        extern int _iram_text_end;
        // 3. IRAM and DRAM
        PMP_RESET_AND_ENTRY_SET(5, SOC_IRAM_LOW, NONE);
        PMP_RESET_AND_ENTRY_SET(6, (int)&_iram_text_end, PMP_TOR | RX);
        PMP_RESET_AND_ENTRY_SET(7, SOC_DRAM_HIGH, PMP_TOR | RW);
#else
        // 3. IRAM and DRAM
        const uint32_t pmpaddr5 = PMPADDR_NAPOT(SOC_IRAM_LOW, SOC_IRAM_HIGH);
        PMP_RESET_AND_ENTRY_SET(5, pmpaddr5, PMP_NAPOT | RWX);
        _Static_assert(SOC_IRAM_LOW < SOC_IRAM_HIGH, "Invalid RAM region");
#endif
    }

#if CONFIG_ESP_SYSTEM_MEMPROT && CONFIG_ESP_SYSTEM_MEMPROT_PMP
    extern int _instruction_reserved_end;
    extern int _rodata_reserved_end;

    const uint32_t irom_resv_end = ALIGN_UP_TO_MMU_PAGE_SIZE((uint32_t)(&_instruction_reserved_end));
    const uint32_t drom_resv_end = ALIGN_UP_TO_MMU_PAGE_SIZE((uint32_t)(&_rodata_reserved_end));

    // 4. I_Cache / D_Cache (flash)
    PMP_RESET_AND_ENTRY_SET(8, SOC_IROM_LOW, NONE);
    PMP_RESET_AND_ENTRY_SET(9, irom_resv_end, PMP_TOR | RX);
    PMP_RESET_AND_ENTRY_SET(10, drom_resv_end, PMP_TOR | R);
#else
    // 4. I_Cache / D_Cache (flash)
    const uint32_t pmpaddr8 = PMPADDR_NAPOT(SOC_IROM_LOW, SOC_IROM_HIGH);
    PMP_RESET_AND_ENTRY_SET(8, pmpaddr8, PMP_NAPOT | RX);
    _Static_assert(SOC_IROM_LOW < SOC_IROM_HIGH, "Invalid I/D_Cache region");
#endif

    // 5. LP memory
#if CONFIG_ESP_SYSTEM_MEMPROT && CONFIG_ESP_SYSTEM_MEMPROT_PMP
    extern int _rtc_text_start;
    extern int _rtc_text_end;
    PMP_RESET_AND_ENTRY_SET(11, SOC_RTC_IRAM_LOW, NONE);

    // First part of LP mem is reserved for ULP coprocessor
#if CONFIG_ESP_SYSTEM_MEMPROT_PMP_LP_CORE_RESERVE_MEM_EXEC
    PMP_RESET_AND_ENTRY_SET(12, (int)&_rtc_text_start, PMP_TOR | RWX);
#else
    PMP_RESET_AND_ENTRY_SET(12, (int)&_rtc_text_start, PMP_TOR | RW);
#endif
    PMP_RESET_AND_ENTRY_SET(13, (int)&_rtc_text_end, PMP_TOR | RX);
    PMP_RESET_AND_ENTRY_SET(14, SOC_RTC_IRAM_HIGH, PMP_TOR | RW);
#else
    const uint32_t pmpaddr11 = PMPADDR_NAPOT(SOC_RTC_IRAM_LOW, SOC_RTC_IRAM_HIGH);
    PMP_RESET_AND_ENTRY_SET(11, pmpaddr11, PMP_NAPOT | RWX);
    _Static_assert(SOC_RTC_IRAM_LOW < SOC_RTC_IRAM_HIGH, "Invalid RTC IRAM region");
#endif

    // 6. Peripheral addresses
    const uint32_t pmpaddr15 = PMPADDR_NAPOT(SOC_PERIPHERAL_LOW, SOC_PERIPHERAL_HIGH);
    PMP_RESET_AND_ENTRY_SET(PMP_ENTRY_PERIPHERAL, pmpaddr15, PMP_NAPOT | RW);
    _Static_assert(SOC_PERIPHERAL_LOW < SOC_PERIPHERAL_HIGH, "Invalid peripheral region");
}
#endif // BOOTLOADER_BUILD

void esp_cpu_configure_region_protection(void)
{
    /* Notes on implementation:
     *
     * 1) Note: ESP32-C6 CPU doesn't support overlapping PMP regions
     *
     * 2) ESP32-C6 supports 16 PMA regions so we use this feature to block all the invalid address ranges
     *
     * 3) We use combination of NAPOT (Naturally Aligned Power Of Two) and TOR (top of range)
     * entries to map all the valid address space, bottom to top. This leaves us with some extra PMP entries
     * which can be used to provide more granular access
     *
     * 4) Entries are grouped in order with some static asserts to try and verify everything is
     * correct.
     */

    /* The invalid (PMA) regions are configured in both the bootloader and the
     * application; the valid (PMP) regions are configured and locked by the
     * application only (an unlocked PMP entry does not apply to M-mode, and a
     * locked one would survive into the application with no way to reconfigure
     * it until the next CPU reset). */
    esp_cpu_configure_invalid_regions();

#ifndef BOOTLOADER_BUILD
    esp_cpu_configure_valid_regions();
#endif
}
