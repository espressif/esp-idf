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
#include "esp_macros.h"
#include "esp_attr.h"
#if !BOOTLOADER_BUILD && CONFIG_SPIRAM
#include "esp_private/esp_psram_extram.h"
#endif /* !BOOTLOADER_BUILD && CONFIG_SPIRAM */

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

    // 0. Gap at bottom of address space
    PMA_ENTRY_SET_NAPOT(0, 0, SOC_CPU_SUBSYSTEM_LOW, PMA_NAPOT | PMA_NONE);

    // [SOC_CPU_SUBSYSTEM_LOW .. SOC_RTC_IRAM_HIGH) is all valid (CPU subsystem, on-chip
    // peripherals and the LP TCM) and is configured using PMP below, so no PMA entry is needed.
    // Due to less PMA entries we cannot configure all the invalid region gaps in this region.

    // 1. Gap between LP TCM (RTC SRAM) and HP TCM (internal SRAM)
    PMA_ENTRY_SET_TOR(1, SOC_RTC_IRAM_HIGH, PMA_NONE);
    PMA_ENTRY_SET_TOR(2, SOC_IRAM_LOW, PMA_TOR | PMA_NONE);

    // 2. Gap between HP TCM (internal SRAM) and ROM-Cache
    PMA_ENTRY_SET_TOR(3, SOC_IRAM_HIGH, PMA_NONE);
    PMA_ENTRY_SET_TOR(4, SOC_IROM_MASK_LOW, PMA_TOR | PMA_NONE);

    // 3. ROM has configured the ROM region to be cacheable, keep the configuration as RX.
    //    Configuring it here (as a valid RX region) lets the I/D-ROM PMP entries below use the
    //    DROM-split optimization and save a PMP entry. This region's base is entry 4's address.
    PMA_ENTRY_SET_TOR(5, SOC_DROM_MASK_HIGH, PMA_TOR | PMA_RX);

    // 4. Gap between ROM-Cache and External flash (I/D-Cache)
    PMA_ENTRY_SET_TOR(6, SOC_IROM_LOW, PMA_TOR | PMA_NONE);

    // 5. External flash (I/D-Cache): kept RX and made cacheable. The cacheable attribute can only
    //    be set through PMA, which is why this valid region is configured here and not only by PMP.
    PMA_ENTRY_SET_TOR(7, SOC_IROM_HIGH, PMA_TOR | PMA_RX);

    // 6. Gap between External flash and External PSRAM. This range is decoded as flash but lies
    //    beyond the MMU-mappable window, so it would alias onto the mapped flash: deny it.
    PMA_ENTRY_SET_TOR(8, SOC_EXTRAM_LOW, PMA_TOR | PMA_NONE);

    // 7. External PSRAM: cacheable and RWX so that PSRAM data and XIP-from-PSRAM both work.
    PMA_ENTRY_SET_TOR(9, SOC_EXTRAM_HIGH, PMA_TOR | PMA_RWX);

    // 8. Gap between External PSRAM and the cache-bypass aliases
    PMA_ENTRY_SET_TOR(10, (uint32_t)SOC_IROM_LOW + (uint32_t)SOC_NON_CACHEABLE_OFFSET_FLASH, PMA_TOR | PMA_NONE);

    // 9. Non-cacheable (cache-bypass) alias of External flash
    PMA_ENTRY_SET_TOR(11, (uint32_t)SOC_IROM_HIGH + (uint32_t)SOC_NON_CACHEABLE_OFFSET_FLASH, PMA_TOR | PMA_RX);

    // 10. Gap between the flash and PSRAM cache-bypass aliases
    PMA_ENTRY_SET_TOR(12, (uint32_t)SOC_EXTRAM_LOW + (uint32_t)SOC_NON_CACHEABLE_OFFSET_PSRAM, PMA_TOR | PMA_NONE);

    // 11. Non-cacheable (cache-bypass) alias of External PSRAM, used by GDMA to reach
    //     PSRAM-resident descriptors/buffers coherently
    PMA_ENTRY_SET_TOR(13, (uint32_t)SOC_EXTRAM_HIGH + (uint32_t)SOC_NON_CACHEABLE_OFFSET_PSRAM, PMA_TOR | PMA_RW);

    // 12. End of address space
    PMA_ENTRY_SET_TOR(14, UINT32_MAX, PMA_TOR | PMA_NONE);
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
     * up front (so no region is active while programming) and the entries are then programmed in
     * ascending index order, so each TOR base address is already in place before the entry that
     * uses it is enabled. The final regions themselves are disjoint, so no overlapping
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

    // 1. CPU Subsystem region - contains debug mode code and interrupt config registers - and the
    //    peripherals (on-chip peripherals, CPU peripheral, cache-data memory, the debug address
    //    space and the LP peripherals, which are all contiguous with each other).
    //
    //    These two are programmed as a TOR chain rather than as separate NAPOT/TOR groups so that
    //    the peripheral window costs a single entry: entry 0's region is [0, SOC_CPU_SUBSYSTEM_HIGH)
    //    (its TOR base is implicitly 0) and entry 1 then chains off entry 0's address. That frees
    //    the entry the PSRAM section split below needs. The [0, SOC_CPU_SUBSYSTEM_LOW) part that
    //    entry 0 over-covers is denied by PMA, so it stays inaccessible.
    //
    //    The peripheral window must not be widened to a NAPOT: rounding its top up
    //    would cover ROM, HP TCM and LP TCM, and in particular the ROM text that the DROM-split
    //    optimisation below deliberately leaves PMP-unmatched, which would then lose execute
    //    permission and lock the CPU up on the first ROM fetch.
    PMP_ENTRY_SET(0, SOC_CPU_SUBSYSTEM_HIGH, PMP_TOR | RWX);
    PMP_ENTRY_SET(1, SOC_PERIPHERAL_HIGH, PMP_TOR | RW);
    _Static_assert(SOC_CPU_SUBSYSTEM_LOW < SOC_CPU_SUBSYSTEM_HIGH, "Invalid CPU subsystem region");
    _Static_assert(SOC_PERIPHERAL_LOW < SOC_PERIPHERAL_HIGH, "Invalid peripheral region");

    // 2. I/D-ROM (ROM-Cache)
#if CONFIG_ESP_SYSTEM_MEMPROT && CONFIG_ESP_SYSTEM_MEMPROT_PMP
    const uint32_t drom_start = (uint32_t) (ets_rom_layout_p->drom_start);
    if ((drom_start & (SOC_CPU_PMP_REGION_GRANULARITY - 1)) == 0) {
        // We can skip configuring the PMP entry for the [SOC_IROM_MASK_LOW - drom_start]
        // region as RX, as we already have configured a PMA entry with RX permissions for the
        // [SOC_IROM_MASK_LOW - SOC_DROM_MASK_HIGH] region that also makes it cacheable. Thus, we
        // save on one PMP entry. Note that this leaves the ROM text PMP-unmatched on purpose (an
        // address unmatched by PMP is allowed in M-mode), so no later entry may cover it.
        PMP_ENTRY_SET(2, drom_start, NONE);
        PMP_ENTRY_SET(3, SOC_DROM_MASK_HIGH, PMP_TOR | R);
    } else
#endif
    {
        PMP_ENTRY_SET(2, SOC_IROM_MASK_LOW, NONE);
        PMP_ENTRY_SET(3, SOC_IROM_MASK_HIGH, PMP_TOR | RX);
        _Static_assert(SOC_IROM_MASK_LOW < SOC_IROM_MASK_HIGH, "Invalid I/D-ROM region");
    }

    // 3. IRAM and DRAM (HP TCM)
    if (esp_cpu_dbgr_is_attached()) {
        // Anti-FI check that cpu is really in ocd mode
        ESP_FAULT_ASSERT(esp_cpu_dbgr_is_attached());

        const uint32_t pmpaddr_sram = PMPADDR_NAPOT(SOC_IRAM_LOW, SOC_IRAM_HIGH);
        PMP_ENTRY_SET(4, pmpaddr_sram, PMP_NAPOT | RWX);
        _Static_assert(SOC_IRAM_LOW < SOC_IRAM_HIGH, "Invalid RAM region");
    } else {
#if CONFIG_ESP_SYSTEM_MEMPROT && CONFIG_ESP_SYSTEM_MEMPROT_PMP
        extern int _iram_text_end;
        PMP_ENTRY_SET(4, SOC_IRAM_LOW, NONE);
        PMP_ENTRY_SET(5, (int)&_iram_text_end, PMP_TOR | RX);
        PMP_ENTRY_SET(6, SOC_DRAM_HIGH, PMP_TOR | RW);
#else
        const uint32_t pmpaddr_sram = PMPADDR_NAPOT(SOC_IRAM_LOW, SOC_IRAM_HIGH);
        PMP_ENTRY_SET(4, pmpaddr_sram, PMP_NAPOT | RWX);
        _Static_assert(SOC_IRAM_LOW < SOC_IRAM_HIGH, "Invalid RAM region");
#endif
    }

    // 4. External flash (I/D-Cache) and External PSRAM (I/D-EXTRAM)
#if CONFIG_SPIRAM && (CONFIG_SPIRAM_FETCH_INSTRUCTIONS || CONFIG_SPIRAM_RODATA)
#if CONFIG_ESP_SYSTEM_MEMPROT && CONFIG_ESP_SYSTEM_MEMPROT_PMP && CONFIG_SPIRAM_PRE_CONFIGURE_MEMORY_PROTECTION
    /* XIP-from-PSRAM: split PSRAM by section, mirroring ESP32-P4. Flash needs no PMP entry here,
     * the PMA entry above already grants it RX. Each combination of the two section markers is
     * handled separately, since which markers exist decides which boundaries are valid.
     *
     * The reclaimed heap starts at the MMU page boundary after the last XIP section, because
     * esp_mmu_map_reserve_block_with_caps() hands out MMU-page-aligned blocks from the free head.
     * The heap top is therefore page_aligned_<section>_resv_end + heap size, not the unaligned
     * section end. This split describes the layout that esp_psram_init() produces, so it is only
     * programmed when CONFIG_SPIRAM_PRE_CONFIGURE_MEMORY_PROTECTION says that layout applies;
     * otherwise PSRAM is left as one permissive window below, as on ESP32-P4. */
    const size_t available_psram_heap = esp_psram_get_heap_size_to_protect();

    PMP_ENTRY_SET(7, SOC_EXTRAM_LOW, NONE);

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS && CONFIG_SPIRAM_RODATA
    extern int _instruction_reserved_end;
    extern int _rodata_reserved_end;
    const uint32_t page_aligned_irom_resv_end = ALIGN_UP_TO_MMU_PAGE_SIZE((uint32_t)(&_instruction_reserved_end));

    PMP_ENTRY_SET(8, (uint32_t)(&_instruction_reserved_end), PMP_TOR | RX);
    PMP_ENTRY_SET(9, page_aligned_irom_resv_end, PMP_TOR | RW);
    PMP_ENTRY_SET(10, (uint32_t)(&_rodata_reserved_end), PMP_TOR | R);
    const uint32_t page_aligned_drom_resv_end = ALIGN_UP_TO_MMU_PAGE_SIZE((uint32_t)(&_rodata_reserved_end));
    PMP_ENTRY_SET(11, ESP_ALIGN_UP(page_aligned_drom_resv_end + available_psram_heap, SOC_CPU_PMP_REGION_GRANULARITY), PMP_TOR | RW);

#elif CONFIG_SPIRAM_FETCH_INSTRUCTIONS
    /* Only .text moved to PSRAM; .rodata stays flash-resident. */
    extern int _instruction_reserved_end;
    const uint32_t page_aligned_irom_resv_end = ALIGN_UP_TO_MMU_PAGE_SIZE((uint32_t)(&_instruction_reserved_end));

    PMP_ENTRY_SET(8, (uint32_t)(&_instruction_reserved_end), PMP_TOR | RX);
    PMP_ENTRY_SET(9, page_aligned_irom_resv_end, PMP_TOR | RW);
    PMP_ENTRY_SET(10, ESP_ALIGN_UP(page_aligned_irom_resv_end + available_psram_heap, SOC_CPU_PMP_REGION_GRANULARITY), PMP_TOR | RW);

#else /* CONFIG_SPIRAM_RODATA */
    /* Only .rodata moved to PSRAM; nothing in PSRAM is executable. */
    extern int _rodata_reserved_end;

    PMP_ENTRY_SET(8, (uint32_t)(&_rodata_reserved_end), PMP_TOR | R);
    const uint32_t page_aligned_drom_resv_end = ALIGN_UP_TO_MMU_PAGE_SIZE((uint32_t)(&_rodata_reserved_end));
    PMP_ENTRY_SET(9, ESP_ALIGN_UP(page_aligned_drom_resv_end + available_psram_heap, SOC_CPU_PMP_REGION_GRANULARITY), PMP_TOR | RW);
    _Static_assert(SOC_EXTRAM_LOW < SOC_EXTRAM_HIGH, "Invalid I/D_EXTRAM region");
#endif
#else
    /* Memory protection disabled, or the pre-configured PSRAM layout does not apply: a single
     * permissive NAPOT window. It must carry X because .text is executed directly out of PSRAM
     * in this configuration. */
    const uint32_t pmpaddr_extram = PMPADDR_NAPOT(SOC_EXTRAM_LOW, SOC_EXTRAM_HIGH);
    PMP_ENTRY_SET(7, pmpaddr_extram, PMP_NAPOT | RWX);
    _Static_assert(SOC_EXTRAM_LOW < SOC_EXTRAM_HIGH, "Invalid I/D_EXTRAM region");
#endif
#else
#if CONFIG_ESP_SYSTEM_MEMPROT && CONFIG_ESP_SYSTEM_MEMPROT_PMP
    extern int _instruction_reserved_end;
    const uint32_t page_aligned_irom_resv_end = ALIGN_UP_TO_MMU_PAGE_SIZE((uint32_t)(&_instruction_reserved_end));

    PMP_ENTRY_SET(7, SOC_IROM_LOW, NONE);
    PMP_ENTRY_SET(8, page_aligned_irom_resv_end, PMP_TOR | RX);
    PMP_ENTRY_SET(9, SOC_DROM_HIGH, PMP_TOR | R);
    _Static_assert(SOC_IROM_LOW < SOC_IROM_HIGH, "Invalid I/D_Cache region");
#else
    const uint32_t pmpaddr_flash = PMPADDR_NAPOT(SOC_IROM_LOW, SOC_IROM_HIGH);
    PMP_ENTRY_SET(7, pmpaddr_flash, PMP_NAPOT | RX);
    _Static_assert(SOC_IROM_LOW < SOC_IROM_HIGH, "Invalid I/D_Cache region");
#endif

#if CONFIG_SPIRAM
    /* PSRAM used as data only (no XIP): a single NAPOT window, RW so that nothing in external RAM
     * is executable. The PMA entry configured above already makes this region cacheable; the PSRAM
     * cache-bypass alias is kept valid by PMA (an address unmatched by PMP is allowed by PMP in
     * M-mode, so it needs no extra PMP entry).
     *
     * The entry is locked, so it is only narrowed to RW when the pre-configured PSRAM layout
     * applies. Otherwise the application owns the region - it may map and execute its own code
     * there - and the window stays RWX, as on ESP32-P4. */
    const uint32_t pmpaddr_extram = PMPADDR_NAPOT(SOC_EXTRAM_LOW, SOC_EXTRAM_HIGH);
#if CONFIG_ESP_SYSTEM_MEMPROT && CONFIG_ESP_SYSTEM_MEMPROT_PMP && CONFIG_SPIRAM_PRE_CONFIGURE_MEMORY_PROTECTION
    PMP_ENTRY_SET(10, pmpaddr_extram, PMP_NAPOT | RW);
#else
    PMP_ENTRY_SET(10, pmpaddr_extram, PMP_NAPOT | RWX);
#endif
    _Static_assert(SOC_EXTRAM_LOW < SOC_EXTRAM_HIGH, "Invalid EXTRAM region");
#endif /* CONFIG_SPIRAM */

#endif /* CONFIG_SPIRAM && (CONFIG_SPIRAM_FETCH_INSTRUCTIONS || CONFIG_SPIRAM_RODATA) */

    // 5. LP memory (LP TCM / RTC SRAM)
#if CONFIG_ESP_SYSTEM_MEMPROT && CONFIG_ESP_SYSTEM_MEMPROT_PMP
    extern int _rtc_text_start;
    extern int _rtc_text_end;
    PMP_ENTRY_SET(12, SOC_RTC_IRAM_LOW, NONE);
#if CONFIG_ESP_SYSTEM_MEMPROT_PMP_LP_CORE_RESERVE_MEM_EXEC
    PMP_ENTRY_SET(13, (int)&_rtc_text_start, PMP_TOR | RWX);
#else
    PMP_ENTRY_SET(13, (int)&_rtc_text_start, PMP_TOR | RW);
#endif
    PMP_ENTRY_SET(14, (int)&_rtc_text_end, PMP_TOR | RX);
    PMP_ENTRY_SET(15, SOC_RTC_IRAM_HIGH, PMP_TOR | RW);
#else
    const uint32_t pmpaddr_rtc = PMPADDR_NAPOT(SOC_RTC_IRAM_LOW, SOC_RTC_IRAM_HIGH);
    PMP_ENTRY_SET(12, pmpaddr_rtc, PMP_NAPOT | RWX);
    _Static_assert(SOC_RTC_IRAM_LOW < SOC_RTC_IRAM_HIGH, "Invalid RTC IRAM region");
#endif
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

    /* The invalid (PMA) regions are configured by both the bootloader and the application; the valid
     * (PMP) regions by the application only, since an unlocked PMP entry does not constrain M-mode.
     * Only the application locks either, so the bootloader freezes nothing and the whole PMA/PMP
     * layout stays application-owned - an implementation detail, not an ABI. */
    esp_cpu_configure_invalid_regions();

#ifndef BOOTLOADER_BUILD
    esp_cpu_configure_valid_regions();
#endif
}
