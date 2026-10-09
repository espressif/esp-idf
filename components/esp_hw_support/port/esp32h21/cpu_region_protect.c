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
#include "esp32h21/rom/rom_layout.h"
#include "esp_macros.h"
#include "esp_attr.h"

#define ALIGN_UP_TO_MMU_PAGE_SIZE(addr)    ESP_ALIGN_UP(addr, SOC_MMU_PAGE_SIZE)

NOINLINE_ATTR IRAM_ATTR static void esp_cpu_configure_invalid_regions(void)
{
#ifdef BOOTLOADER_BUILD
    /* Don't lock PMA entries in the bootloader: an enabled entry constrains M-mode even without
     * the lock bit (unlike PMP), and a locked entry would be frozen until CPU reset, turning the
     * PMA layout into a bootloader<->application ABI. Only the application locks. */
    const unsigned PMA_LOCK = 0;
#else
    const unsigned PMA_LOCK = PMA_L;
#endif
    const unsigned PMA_NONE                            = PMA_LOCK | PMA_EN;
    __attribute__((unused)) const unsigned PMA_RW      = PMA_LOCK | PMA_EN | PMA_R | PMA_W;
    __attribute__((unused)) const unsigned PMA_RX      = PMA_LOCK | PMA_EN | PMA_R | PMA_X;
    __attribute__((unused)) const unsigned PMA_RWX     = PMA_LOCK | PMA_EN | PMA_R | PMA_W | PMA_X;

    /* Disable every PMA entry before programming.
     * Descending order helps to correctly reset the TOR settings (if any).
     * This also clears the entries the ROM programmed (LP RAM, RAM and the MSPI window at
     * entries 13-15); the chain below re-grants the MSPI window explicitly.
     */
    PMA_ENTRY_CFG_RESET(15);
    PMA_ENTRY_CFG_RESET(14);
    PMA_ENTRY_CFG_RESET(13);
    PMA_ENTRY_CFG_RESET(12);
    PMA_ENTRY_CFG_RESET(11);
    PMA_ENTRY_CFG_RESET(10);
    PMA_ENTRY_CFG_RESET(9);
    PMA_ENTRY_CFG_RESET(8);
    PMA_ENTRY_CFG_RESET(7);
    PMA_ENTRY_CFG_RESET(6);
    PMA_ENTRY_CFG_RESET(5);
    PMA_ENTRY_CFG_RESET(4);
    PMA_ENTRY_CFG_RESET(3);
    PMA_ENTRY_CFG_RESET(2);
    PMA_ENTRY_CFG_RESET(1);
    PMA_ENTRY_CFG_RESET(0);

    // 1. Gap at bottom of address space
    PMA_ENTRY_SET_NAPOT(0, 0, SOC_CPU_SUBSYSTEM_LOW, PMA_NAPOT | PMA_NONE);

    // 2. Gap between CPU subsystem region & IROM
    PMA_ENTRY_SET_TOR(1, SOC_CPU_SUBSYSTEM_HIGH, PMA_NONE);
    PMA_ENTRY_SET_TOR(2, SOC_IROM_MASK_LOW, PMA_TOR | PMA_NONE);

    // 3. Gap between ROM & RAM
    PMA_ENTRY_SET_TOR(3, SOC_DROM_MASK_HIGH, PMA_NONE);
    PMA_ENTRY_SET_TOR(4, SOC_IRAM_LOW, PMA_TOR | PMA_NONE);

    // 4. Gap between DRAM and I_Cache
    PMA_ENTRY_SET_TOR(5, SOC_IRAM_HIGH, PMA_NONE);
    PMA_ENTRY_SET_TOR(6, SOC_IROM_LOW, PMA_TOR | PMA_NONE);

    // 5. I_Cache / D_Cache (flash)
    PMA_ENTRY_SET_TOR(7, SOC_DROM_HIGH, PMA_TOR | PMA_RX);

    // 6. Gap between D_Cache & LP_RAM
    PMA_ENTRY_SET_TOR(8, SOC_RTC_IRAM_LOW, PMA_TOR | PMA_NONE);

    // 7. Gap between LP memory & peripheral addresses
    PMA_ENTRY_SET_TOR(9, SOC_RTC_IRAM_HIGH, PMA_NONE);
    PMA_ENTRY_SET_TOR(10, SOC_PERIPHERAL_LOW, PMA_TOR | PMA_NONE);

    // 8. End of address space
    PMA_ENTRY_SET_TOR(11, SOC_PERIPHERAL_HIGH, PMA_NONE);
    PMA_ENTRY_SET_TOR(12, UINT32_MAX, PMA_TOR | PMA_NONE);
}

#ifndef BOOTLOADER_BUILD
static void esp_cpu_configure_valid_regions(void)
{
    /* There are 3 configuration scenarios for SRAM
     *
     * 1. Application build with CONFIG_ESP_SYSTEM_MEMPROT enabled
     *    - We split the SRAM into IRAM and DRAM such that IRAM region cannot be written to
     *      and DRAM region cannot be executed. We use the _iram_text_end marker to set the boundary.
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
    __attribute__((unused)) const unsigned RW      = PMP_L | PMP_R | PMP_W;
    __attribute__((unused)) const unsigned RX      = PMP_L | PMP_R | PMP_X;
    __attribute__((unused)) const unsigned RWX     = PMP_L | PMP_R | PMP_W | PMP_X;

    /* Reset every PMP entry before programming: the bootloader configures no PMP, so the
     * registers hold whatever the ROM left, and PMP_ENTRY_SET only ORs configuration bits.
     * ESP32-H21 CPU doesn't support overlapping PMP regions, so the reset-and-set macros cannot
     * be used for TOR entries (their transient pmpaddr=0 creates a wrapping overlap); instead all
     * entries are disabled up front and programmed in ascending index order, so no overlapping
     * configuration - transient or final - is ever visible.
     */
    PMP_ENTRY_CFG_RESET(0);
    PMP_ENTRY_CFG_RESET(1);
    PMP_ENTRY_CFG_RESET(2);
    PMP_ENTRY_CFG_RESET(3);
    PMP_ENTRY_CFG_RESET(4);
    PMP_ENTRY_CFG_RESET(5);
    PMP_ENTRY_CFG_RESET(6);
    PMP_ENTRY_CFG_RESET(7);
    PMP_ENTRY_CFG_RESET(8);
    PMP_ENTRY_CFG_RESET(9);
    PMP_ENTRY_CFG_RESET(10);
    PMP_ENTRY_CFG_RESET(11);
    PMP_ENTRY_CFG_RESET(12);
    PMP_ENTRY_CFG_RESET(13);
    PMP_ENTRY_CFG_RESET(14);
    PMP_ENTRY_CFG_RESET(15);

    // 1. CPU Subsystem region - contains debug mode code and interrupt config registers
    const uint32_t pmpaddr0 = PMPADDR_NAPOT(SOC_CPU_SUBSYSTEM_LOW, SOC_CPU_SUBSYSTEM_HIGH);
    PMP_ENTRY_SET(0, pmpaddr0, PMP_NAPOT | RWX);
    _Static_assert(SOC_CPU_SUBSYSTEM_LOW < SOC_CPU_SUBSYSTEM_HIGH, "Invalid CPU subsystem region");

    // 2. I/D-ROM
    /* The ROM layout of non-MP (Mass Production) revisions has no drom_start field, so the
     * ROM is mapped as a single RX region there.
     */
#if CONFIG_ESP32H21_SELECTS_REV_MP && CONFIG_ESP_SYSTEM_MEMPROT && CONFIG_ESP_SYSTEM_MEMPROT_PMP
    const uint32_t drom_start = (uint32_t) (ets_rom_layout_p->drom_start);
    if (((drom_start & (SOC_CPU_PMP_REGION_GRANULARITY - 1)) == 0)
            && (drom_start > SOC_IROM_MASK_LOW) && (drom_start < SOC_DROM_MASK_HIGH)) {
        // Split the ROM into text (RX) and data (R), fully PMP-matched.
        PMP_ENTRY_SET(1, SOC_IROM_MASK_LOW, NONE);
        PMP_ENTRY_SET(2, drom_start, PMP_TOR | RX);
        PMP_ENTRY_SET(3, SOC_DROM_MASK_HIGH, PMP_TOR | R);
    } else
#endif
    {
        PMP_ENTRY_SET(1, SOC_IROM_MASK_LOW, NONE);
        PMP_ENTRY_SET(2, SOC_IROM_MASK_HIGH, PMP_TOR | RX);
        _Static_assert(SOC_IROM_MASK_LOW < SOC_IROM_MASK_HIGH, "Invalid I/D-ROM region");
    }

    // 3. IRAM and DRAM (HP SRAM is not a power of two in size, so TOR is used throughout)
    if (esp_cpu_dbgr_is_attached()) {
        // Anti-FI check that cpu is really in ocd mode
        ESP_FAULT_ASSERT(esp_cpu_dbgr_is_attached());

        PMP_ENTRY_SET(4, SOC_IRAM_LOW, NONE);
        PMP_ENTRY_SET(5, SOC_IRAM_HIGH, PMP_TOR | RWX);
        _Static_assert(SOC_IRAM_LOW < SOC_IRAM_HIGH, "Invalid RAM region");
    } else {
#if CONFIG_ESP_SYSTEM_MEMPROT && CONFIG_ESP_SYSTEM_MEMPROT_PMP
        extern int _iram_text_end;
        PMP_ENTRY_SET(4, SOC_IRAM_LOW, NONE);
        PMP_ENTRY_SET(5, (int)&_iram_text_end, PMP_TOR | RX);
        PMP_ENTRY_SET(6, SOC_DRAM_HIGH, PMP_TOR | RW);
#else
        PMP_ENTRY_SET(4, SOC_IRAM_LOW, NONE);
        PMP_ENTRY_SET(5, SOC_IRAM_HIGH, PMP_TOR | RWX);
        _Static_assert(SOC_IRAM_LOW < SOC_IRAM_HIGH, "Invalid RAM region");
#endif
    }

    // 4. I_Cache / D_Cache (flash)
#if CONFIG_ESP_SYSTEM_MEMPROT && CONFIG_ESP_SYSTEM_MEMPROT_PMP
    extern int _instruction_reserved_end;
    extern int _rodata_reserved_end;

    const uint32_t page_aligned_irom_resv_end = ALIGN_UP_TO_MMU_PAGE_SIZE((uint32_t)(&_instruction_reserved_end));
    const uint32_t page_aligned_drom_resv_end = ALIGN_UP_TO_MMU_PAGE_SIZE((uint32_t)(&_rodata_reserved_end));

    PMP_ENTRY_SET(7, SOC_IROM_LOW, NONE);
    PMP_ENTRY_SET(8, page_aligned_irom_resv_end, PMP_TOR | RX);
    PMP_ENTRY_SET(9, page_aligned_drom_resv_end, PMP_TOR | R);
#else
    const uint32_t pmpaddr7 = PMPADDR_NAPOT(SOC_IROM_LOW, SOC_IROM_HIGH);
    PMP_ENTRY_SET(7, pmpaddr7, PMP_NAPOT | RX);
    _Static_assert(SOC_IROM_LOW < SOC_IROM_HIGH, "Invalid I/D_Cache region");
#endif

    // 5. LP memory
#if CONFIG_ESP_SYSTEM_MEMPROT && CONFIG_ESP_SYSTEM_MEMPROT_PMP
    extern int _rtc_text_start;
    extern int _rtc_text_end;
    PMP_ENTRY_SET(10, SOC_RTC_IRAM_LOW, NONE);
    PMP_ENTRY_SET(11, (int)&_rtc_text_start, PMP_TOR | RW);
    PMP_ENTRY_SET(12, (int)&_rtc_text_end, PMP_TOR | RX);
    PMP_ENTRY_SET(13, SOC_RTC_IRAM_HIGH, PMP_TOR | RW);
#else
    const uint32_t pmpaddr10 = PMPADDR_NAPOT(SOC_RTC_IRAM_LOW, SOC_RTC_IRAM_HIGH);
    PMP_ENTRY_SET(10, pmpaddr10, PMP_NAPOT | RWX);
    _Static_assert(SOC_RTC_IRAM_LOW < SOC_RTC_IRAM_HIGH, "Invalid RTC IRAM region");
#endif

    // 6. Peripheral addresses
    const uint32_t pmpaddr15 = PMPADDR_NAPOT(SOC_PERIPHERAL_LOW, SOC_PERIPHERAL_HIGH);
    PMP_ENTRY_SET(15, pmpaddr15, PMP_NAPOT | RW);
    _Static_assert(SOC_PERIPHERAL_LOW < SOC_PERIPHERAL_HIGH, "Invalid peripheral region");
}
#endif // BOOTLOADER_BUILD

void esp_cpu_configure_region_protection(void)
{
    /* Notes on implementation:
     *
     * 1) ESP32-H21 supports 16 PMA and 16 PMP regions. PMA blocks the invalid address ranges and
     *    marks the external flash window as cacheable; PMP maps the valid address space bottom
     *    to top with NAPOT and TOR entries, the peripherals using the last entry.
     *
     * 2) ESP32-H21 CPU doesn't support overlapping PMP regions, so all entries are reset up front
     *    and programmed in ascending order with the plain SET macros.
     *
     * 3) There is no PSRAM on this target, and the LP memory has no LP core reserved area, so the
     *    LP RTC text starts at the base of the LP memory.
     */

    /* The invalid (PMA) regions are configured by both the bootloader and the application; the valid
     * (PMP) regions by the application only, since an unlocked PMP entry does not constrain M-mode.
     * Only the application locks either, so the bootloader freezes nothing and the whole PMA/PMP
     * layout stays application-owned - an implementation detail, not an ABI. */
    esp_cpu_configure_invalid_regions();

#ifndef BOOTLOADER_BUILD
    esp_cpu_configure_valid_regions();
#endif
}
