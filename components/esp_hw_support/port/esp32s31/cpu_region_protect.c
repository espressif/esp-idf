/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "sdkconfig.h"
#include "soc/soc.h"
#include "esp_cpu.h"
#include "esp_fault.h"
#include "esp32s31/rom/rom_layout.h"
#include "pmp_layout.h"

#define ALIGN_UP_TO_MMU_PAGE_SIZE(addr)   (((addr) + (SOC_MMU_PAGE_SIZE) - 1) & ~((SOC_MMU_PAGE_SIZE) - 1))

static void esp_cpu_configure_invalid_regions(void)
{
    const unsigned PMA_NONE                            = PMA_L | PMA_EN;
    __attribute__((unused)) const unsigned PMA_RW      = PMA_L | PMA_EN | PMA_R | PMA_W;
    __attribute__((unused)) const unsigned PMA_RX      = PMA_L | PMA_EN | PMA_R | PMA_X;
    __attribute__((unused)) const unsigned PMA_RWX     = PMA_L | PMA_EN | PMA_R | PMA_W | PMA_X;

    // ROM uses some PMA entries, so we need to clear them before using them in ESP-IDF.
    // The reset-and-set PMA macros are safe to use even though ESP32-S31 does not tolerate
    // overlapping *PMP* regions: PMA configures memory attributes (not locked protection), so
    // the transient pmpaddr=0 inside the macro does not raise a fault here (same as ESP32-H4).

    // 0. Gap at bottom of address space
    PMA_RESET_AND_ENTRY_SET_NAPOT(0, 0, SOC_CPU_SUBSYSTEM_LOW, PMA_NAPOT | PMA_NONE);

    // [SOC_CPU_SUBSYSTEM_LOW .. SOC_RTC_IRAM_HIGH) is all valid (CPU subsystem, on-chip
    //  peripherals and the LP TCM) and is configured using PMP below, so no PMA entry is needed.

    // 1. Gap between LP TCM (RTC SRAM) and HP TCM (internal SRAM)
    PMA_RESET_AND_ENTRY_SET_TOR(1, SOC_RTC_IRAM_HIGH, PMA_NONE);
    PMA_RESET_AND_ENTRY_SET_TOR(2, SOC_IRAM_LOW, PMA_TOR | PMA_NONE);

    // 2. Gap between HP TCM (internal SRAM) and ROM-Cache
    PMA_RESET_AND_ENTRY_SET_TOR(3, SOC_IRAM_HIGH, PMA_NONE);
    PMA_RESET_AND_ENTRY_SET_TOR(4, SOC_IROM_MASK_LOW, PMA_TOR | PMA_NONE);

    // 3. ROM has configured the ROM region to be cacheable, lock the configuration as RX.
    //    Configuring it here (as a valid RX region) lets the I/D-ROM PMP entries below use the
    //    DROM-split optimization and save a PMP entry.
    PMA_RESET_AND_ENTRY_SET_TOR(5, SOC_IROM_MASK_LOW, PMA_NONE);
    PMA_RESET_AND_ENTRY_SET_TOR(6, SOC_DROM_MASK_HIGH, PMA_TOR | PMA_RX);

    // 4. Gap between ROM-Cache and External flash (I/D-Cache)
    PMA_RESET_AND_ENTRY_SET_TOR(7, SOC_DROM_MASK_HIGH, PMA_NONE);
    PMA_RESET_AND_ENTRY_SET_TOR(8, SOC_IROM_LOW, PMA_TOR | PMA_NONE);

    // 5. ROM has configured the external flash MSPI region with RX permission; lock it as RX and
    //    make it cacheable. This is a valid region but is configured using PMA (instead of only
    //    PMP) because the cacheable attribute can only be set through PMA.
    PMA_RESET_AND_ENTRY_SET_NAPOT(9, SOC_IROM_LOW, (SOC_IROM_HIGH - SOC_IROM_LOW), PMA_NAPOT | PMA_RX);

    // 6. Gap between External flash (I/D-Cache) and External PSRAM
    PMA_RESET_AND_ENTRY_SET_TOR(10, SOC_IROM_HIGH, PMA_NONE);
    PMA_RESET_AND_ENTRY_SET_TOR(11, SOC_EXTRAM_LOW, PMA_TOR | PMA_NONE);

    // 7. ROM has configured the external PSRAM MSPI region with RX permission; add the W attribute
    //    and lock it, and make it cacheable. As above, this valid region is configured using PMA
    //    so that it can be marked cacheable (RWX so that PSRAM data and XIP-from-PSRAM both work).
    PMA_RESET_AND_ENTRY_SET_NAPOT(12, SOC_EXTRAM_LOW, (SOC_EXTRAM_HIGH - SOC_EXTRAM_LOW), PMA_NAPOT | PMA_RWX);

    // 8. Non-cacheable (cache-bypass) alias of External PSRAM at +SOC_NON_CACHEABLE_OFFSET_PSRAM,
    //    used by GDMA to reach PSRAM-resident descriptors/buffers coherently. Marked RW at a higher
    //    priority (lower index) than the catch-all below so it stays a valid region.
    PMA_RESET_AND_ENTRY_SET_NAPOT(13, (uint32_t)SOC_EXTRAM_LOW + (uint32_t)SOC_NON_CACHEABLE_OFFSET_PSRAM, (SOC_EXTRAM_HIGH - SOC_EXTRAM_LOW), PMA_NAPOT | PMA_RW);

    // 9. End of address space (everything above External PSRAM except the non-cacheable alias above)
    PMA_RESET_AND_ENTRY_SET_TOR(14, SOC_EXTRAM_HIGH, PMA_NONE);
    PMA_RESET_AND_ENTRY_SET_TOR(15, UINT32_MAX, PMA_TOR | PMA_NONE);
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

    /* Reset every PMP entry before programming it.
     *
     * The bootloader no longer configures any PMP entry, so on entry to the application each PMP
     * register still holds whatever the ROM left in it. PMP_ENTRY_SET only ORs configuration bits,
     * so stale permission bits must be cleared first (e.g. an entry that must end up as NONE would
     * otherwise keep any R/W/X bit the ROM set).
     *
     * ESP32-S31 does not support overlapping PMP regions, so the per-entry PMP_RESET_AND_ENTRY_SET
     * macro cannot be used for TOR entries (the transient pmpaddr=0 it writes creates a fleeting
     * wrapping overlap that faults on this CPU). Instead every entry is reset to a disabled state
     * up front and the entries are then programmed strictly in ascending-address order, so each TOR
     * base is already in place before the entry that uses it is enabled, and no overlapping
     * intermediate configuration is ever visible.
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
    const uint32_t pmpaddr_cpu_subsystem = PMPADDR_NAPOT(SOC_CPU_SUBSYSTEM_LOW, SOC_CPU_SUBSYSTEM_HIGH);
    PMP_ENTRY_SET(PMP_ENTRY_CPU_SUBSYSTEM, pmpaddr_cpu_subsystem, PMP_NAPOT | RWX);
    _Static_assert(SOC_CPU_SUBSYSTEM_LOW < SOC_CPU_SUBSYSTEM_HIGH, "Invalid CPU subsystem region");

    // 2. I/D-ROM (ROM-Cache)
#if CONFIG_ESP_SYSTEM_MEMPROT && CONFIG_ESP_SYSTEM_MEMPROT_PMP
    const uint32_t drom_start = (uint32_t) (ets_rom_layout_p->drom_start);
    if ((drom_start & (SOC_CPU_PMP_REGION_GRANULARITY - 1)) == 0) {
        // We can skip configuring the PMP entry for the [SOC_IROM_MASK_LOW - drom_start]
        // region as RX, as we already have configured a PMA entry with RX permissions for the
        // [SOC_IROM_MASK_LOW - SOC_DROM_MASK_HIGH] region that also makes it cacheable. Thus, we
        // save on one PMP entry.
        PMP_ENTRY_SET(PMP_ENTRY_ROM_LOW, drom_start, NONE);
        PMP_ENTRY_SET(PMP_ENTRY_ROM_HIGH, SOC_DROM_MASK_HIGH, PMP_TOR | R);
    } else
#endif
    {
        PMP_ENTRY_SET(PMP_ENTRY_ROM_LOW, SOC_IROM_MASK_LOW, NONE);
        PMP_ENTRY_SET(PMP_ENTRY_ROM_HIGH, SOC_IROM_MASK_HIGH, PMP_TOR | RX);
        _Static_assert(SOC_IROM_MASK_LOW < SOC_IROM_MASK_HIGH, "Invalid I/D-ROM region");
    }

    // 3. IRAM and DRAM (HP TCM)
    if (esp_cpu_dbgr_is_attached()) {
        // Anti-FI check that cpu is really in ocd mode
        ESP_FAULT_ASSERT(esp_cpu_dbgr_is_attached());

        PMP_ENTRY_SET(3, SOC_IRAM_LOW, NONE);
        PMP_ENTRY_SET(4, SOC_IRAM_HIGH, PMP_TOR | RWX);
        _Static_assert(SOC_IRAM_LOW < SOC_IRAM_HIGH, "Invalid RAM region");
    } else {
#if CONFIG_ESP_SYSTEM_MEMPROT && CONFIG_ESP_SYSTEM_MEMPROT_PMP
        extern int _iram_text_end;
        PMP_ENTRY_SET(3, SOC_IRAM_LOW, NONE);
        PMP_ENTRY_SET(4, (int)&_iram_text_end, PMP_TOR | RX);
        PMP_ENTRY_SET(5, SOC_DRAM_HIGH, PMP_TOR | RW);
#else
        PMP_ENTRY_SET(3, SOC_IRAM_LOW, NONE);
        PMP_ENTRY_SET(4, SOC_IRAM_HIGH, PMP_TOR | RWX);
        _Static_assert(SOC_IRAM_LOW < SOC_IRAM_HIGH, "Invalid RAM region");
#endif
    }

    // 4. I_Cache / D_Cache (external flash)
    //    The marker-based split uses _instruction_reserved_end / _rodata_reserved_end, which lie in
    //    the flash aperture only when .text/.rodata are flash-resident. Under XIP-from-PSRAM they
    //    move into PSRAM, so this higher-priority flash entry would shadow the PSRAM RWX entry
    //    (section 5) and make the heap-reused page-alignment gaps read-only. So split only when
    //    flash-resident; otherwise map the flash aperture as a single RX window and let section 5
    //    govern the XIP code/rodata/gaps.
#if CONFIG_ESP_SYSTEM_MEMPROT && CONFIG_ESP_SYSTEM_MEMPROT_PMP && !CONFIG_SPIRAM_FETCH_INSTRUCTIONS && !CONFIG_SPIRAM_RODATA
    extern int _instruction_reserved_end;
    extern int _rodata_reserved_end;

    const uint32_t page_aligned_irom_resv_end = ALIGN_UP_TO_MMU_PAGE_SIZE((uint32_t)(&_instruction_reserved_end));
    const uint32_t page_aligned_drom_resv_end = ALIGN_UP_TO_MMU_PAGE_SIZE((uint32_t)(&_rodata_reserved_end));

    PMP_ENTRY_SET(6, SOC_IROM_LOW, NONE);
    PMP_ENTRY_SET(7, page_aligned_irom_resv_end, PMP_TOR | RX);
    PMP_ENTRY_SET(8, page_aligned_drom_resv_end, PMP_TOR | R);
#else
    const uint32_t pmpaddr_flash = PMPADDR_NAPOT(SOC_IROM_LOW, SOC_IROM_HIGH);
    PMP_ENTRY_SET(6, pmpaddr_flash, PMP_NAPOT | RX);
    _Static_assert(SOC_IROM_LOW < SOC_IROM_HIGH, "Invalid I/D_Cache region");
#endif

    // 5. I_EXTRAM / D_EXTRAM (external PSRAM). The PMA entry configured above already makes this
    //    region cacheable; the PSRAM cache-bypass alias is kept valid by PMA (an address unmatched
    //    by PMP is allowed by PMP in M-mode, so it needs no extra PMP entry).
#if CONFIG_SPIRAM
#if CONFIG_ESP_SYSTEM_MEMPROT && CONFIG_ESP_SYSTEM_MEMPROT_PMP && (CONFIG_SPIRAM_FETCH_INSTRUCTIONS || CONFIG_SPIRAM_RODATA)
    /* XIP-from-PSRAM: .text is executed directly out of PSRAM, so enforce W^X instead of a single
     * RWX window. Map [EXTRAM_LOW, _instruction_reserved_end) as RX (code) and the remainder
     * (rodata, the page-alignment gaps and the reclaimed heap) as RW - i.e. non-executable - so an
     * execute access into the rodata / alignment-gap region faults (Instruction access fault), as
     * the memory-protection tests expect. A full P4-style per-section split (separate R rodata and
     * heap-only entries) does not fit the 16-entry PMP budget once the LP split takes 4 entries, but
     * this two-region split already enforces W^X. It uses entries 7-8 (left free by the flash
     * aperture's single-entry mapping under XIP) plus entry 9, programmed in ascending order. */
    extern int _instruction_reserved_end;
    PMP_ENTRY_SET(7, SOC_EXTRAM_LOW, NONE);
    PMP_ENTRY_SET(8, (uint32_t)(&_instruction_reserved_end), PMP_TOR | RX);
    PMP_ENTRY_SET(9, SOC_EXTRAM_HIGH, PMP_TOR | RW);
    _Static_assert(SOC_EXTRAM_LOW < SOC_EXTRAM_HIGH, "Invalid I/D_EXTRAM region");
#else
    /* PSRAM used as data only (no XIP): a single RWX NAPOT window. */
    const uint32_t pmpaddr_extram = PMPADDR_NAPOT(SOC_EXTRAM_LOW, SOC_EXTRAM_HIGH);
    PMP_ENTRY_SET(9, pmpaddr_extram, PMP_NAPOT | RWX);
    _Static_assert(SOC_EXTRAM_LOW < SOC_EXTRAM_HIGH, "Invalid I/D_EXTRAM region");
#endif
#endif /* CONFIG_SPIRAM */

    // 6. LP memory (LP TCM / RTC SRAM)
#if CONFIG_ESP_SYSTEM_MEMPROT && CONFIG_ESP_SYSTEM_MEMPROT_PMP
    extern int _rtc_text_start;
    extern int _rtc_text_end;
    /* LP TCM split into three regions (mirroring ESP32-P4 / ESP32-C6):
     *   [LOW, _rtc_text_start)        RW  : RTC-reserved mem + ULP/LP-core image (the HP core writes
     *                                       the program here, so it must stay writable).
     *   [_rtc_text_start, _rtc_text_end) RX : HP core rtc.text (e.g. deep-sleep wake stub).
     *   [_rtc_text_end, HIGH)         RW  : RTC data.
     * Programmed in ascending-address order (S31 faults on overlapping PMP entries); every entry was
     * already reset to a disabled state at the top of this function. */
    PMP_ENTRY_SET(10, SOC_RTC_IRAM_LOW, NONE);
#if CONFIG_ESP_SYSTEM_MEMPROT_PMP_LP_CORE_RESERVE_MEM_EXEC
    PMP_ENTRY_SET(11, (int)&_rtc_text_start, PMP_TOR | RWX);
#else
    PMP_ENTRY_SET(11, (int)&_rtc_text_start, PMP_TOR | RW);
#endif
    PMP_ENTRY_SET(12, (int)&_rtc_text_end, PMP_TOR | RX);
    PMP_ENTRY_SET(13, SOC_RTC_IRAM_HIGH, PMP_TOR | RW);
#else
    const uint32_t pmpaddr_rtc = PMPADDR_NAPOT(SOC_RTC_IRAM_LOW, SOC_RTC_IRAM_HIGH);
    PMP_ENTRY_SET(10, pmpaddr_rtc, PMP_NAPOT | RWX);
    _Static_assert(SOC_RTC_IRAM_LOW < SOC_RTC_IRAM_HIGH, "Invalid RTC IRAM region");
#endif

    // 7. Peripheral addresses (on-chip peripherals, CPU peripheral, cache-data memory and the
    //    debug address space). The LP peripherals are contiguous with the rest and are covered by
    //    this single window as well. Peripherals use the last PMP entries (14-15) so that the entry
    //    count used by the regions above (which varies with the build configuration) never collides.
    //    Both indices are frozen by pmp_layout.h.
    PMP_ENTRY_SET(PMP_ENTRY_PERIPHERAL_LOW, SOC_PERIPHERAL_LOW, NONE);
    PMP_ENTRY_SET(PMP_ENTRY_PERIPHERAL_HIGH, SOC_PERIPHERAL_HIGH, PMP_TOR | RW);
    _Static_assert(SOC_PERIPHERAL_LOW < SOC_PERIPHERAL_HIGH, "Invalid peripheral region");
}
#endif // BOOTLOADER_BUILD

void esp_cpu_configure_region_protection(void)
{
    /* Notes on implementation:
     *
     * 1) Note: ESP32-S31 CPU doesn't support overlapping PMP regions. Hence, like ESP32-H4, we
     *    must avoid the reset-and-set PMP macros for TOR entries (the transient pmpaddr=0 inside
     *    them creates a fleeting wrapping overlap that faults on this CPU). We use the non-reset
     *    PMP_ENTRY_SET and reset the relevant entries up front using PMP_ENTRY_CFG_RESET.
     *
     * 2) ESP32-S31 supports 16 PMA regions so we use this feature to block all the invalid address
     *    ranges and to mark the external flash/PSRAM and ROM regions as cacheable.
     *
     * 3) ESP32-S31 supports 16 PMP regions (writing pmpaddr>=16 raises an illegal-instruction fault,
     *    confirmed on rev v0.0 silicon). We use a combination of NAPOT (Naturally Aligned Power Of Two)
     *    and TOR (top of range) entries to map all the valid address space, bottom to top. The whole
     *    region set must therefore fit within entries 0-15, with the peripherals using the last entry.
     *
     * 4) Unlike ESP32-P4, the internal memories on ESP32-S31 are not aliased into a cached/uncached
     *    address pair, so each region needs only a single PMP entry (no CACHED_AND_UNCACHED helper).
     */

    /* The invalid (PMA) regions are configured in both the bootloader and the application. The valid
     * (PMP) regions are configured and locked by the application only: an unlocked PMP entry does not
     * constrain machine mode anyway, and a bootloader-locked entry could not be reconfigured until the
     * next CPU reset - which would freeze the entry layout into a bootloader <-> application ABI. The
     * bootloader therefore programs no PMP entry and the application owns and freezes the whole PMP
     * layout (see pmp_layout.h). */
    esp_cpu_configure_invalid_regions();

#ifndef BOOTLOADER_BUILD
    esp_cpu_configure_valid_regions();
#endif
}
