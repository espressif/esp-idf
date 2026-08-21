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
#include "esp32h4/rom/rom_layout.h"
#include "esp_macros.h"
#include "esp_attr.h"
#if !BOOTLOADER_BUILD && CONFIG_SPIRAM
#include "esp_private/esp_psram_extram.h"
#endif /* !BOOTLOADER_BUILD && CONFIG_SPIRAM */

#define ALIGN_UP_TO_MMU_PAGE_SIZE(addr)    ESP_ALIGN_UP(addr, SOC_MMU_PAGE_SIZE)

/* In single core mode, the ICache1 SRAM block is reclaimed as heap RAM (contiguous with D/IRAM,
 * see components/heap/port/esp32h4/memory_layout.c), so the valid RAM window must extend over it.
 * In multi-core mode it is used as actual I-Cache and must stay outside the window.
 */
#if CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
#define SOC_HP_RAM_HIGH   SOC_RAM_ICACHE1_HIGH
_Static_assert(SOC_IRAM_HIGH == SOC_RAM_ICACHE1_LOW, "ICache1 RAM must be contiguous with D/IRAM");
#else
#define SOC_HP_RAM_HIGH   SOC_IRAM_HIGH
#endif

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

    // 1. Gap between CPU subsystem region (valid, configured using PMP below) and ROM
    PMA_ENTRY_SET_TOR(1, SOC_CPU_SUBSYSTEM_HIGH, PMA_NONE);
    PMA_ENTRY_SET_TOR(2, SOC_IROM_MASK_LOW, PMA_TOR | PMA_NONE);

    // 2. ROM region, kept valid as RX so the deny chain does not swallow it; also the only no-W
    //    enforcement on the ROM in the bootloader stage, where no PMP entry exists.
    PMA_ENTRY_SET_TOR(3, SOC_DROM_MASK_HIGH, PMA_TOR | PMA_RX);

    // 3. Gap between ROM and the D/IRAM region, including the SRAM0 block (DCache data memory),
    //    which ESP-IDF never assigns to the CPU.
    PMA_ENTRY_SET_TOR(4, SOC_IRAM_LOW, PMA_TOR | PMA_NONE);

    // 4. Gap between the valid RAM window (configured using PMP below) and the external memory
    //    window. In multi-core mode this gap starts at the ICache1 block.
    PMA_ENTRY_SET_TOR(5, SOC_HP_RAM_HIGH, PMA_NONE);
    PMA_ENTRY_SET_TOR(6, SOC_IROM_LOW, PMA_TOR | PMA_NONE);

    // 5. External memory window (unified for flash and PSRAM): the cacheable attribute can only
    //    be set through PMA. W is only needed when PSRAM is in use, but the bootloader cannot see
    //    CONFIG_SPIRAM and the application's early load-to-PSRAM copy runs under this (still
    //    bootloader-programmed) entry - so the bootloader always grants W, the application narrows.
#if defined(BOOTLOADER_BUILD) || CONFIG_SPIRAM
    PMA_ENTRY_SET_TOR(7, SOC_IROM_HIGH, PMA_TOR | PMA_RWX);
#else
    PMA_ENTRY_SET_TOR(7, SOC_IROM_HIGH, PMA_TOR | PMA_RX);
#endif

    // 6. Gap between the external memory window and the peripherals (no LP/RTC RAM on this target)
    PMA_ENTRY_SET_TOR(8, SOC_PERIPHERAL_LOW, PMA_TOR | PMA_NONE);

    // 7. End of address space; the peripherals below are valid, configured using PMP
    PMA_ENTRY_SET_TOR(9, SOC_PERIPHERAL_HIGH, PMA_NONE);
    PMA_ENTRY_SET_TOR(10, UINT32_MAX, PMA_TOR | PMA_NONE);
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
     * ESP32-H4 is not known to support overlapping PMP regions, so the reset-and-set macros
     * cannot be used for TOR entries (their transient pmpaddr=0 creates a wrapping overlap);
     * instead all entries are disabled up front and programmed in ascending index order, so no
     * overlapping configuration - transient or final - is ever visible.
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
#if CONFIG_ESP_SYSTEM_MEMPROT && CONFIG_ESP_SYSTEM_MEMPROT_PMP
    const uint32_t drom_start = (uint32_t) (ets_rom_layout_p->drom_start);
    if (((drom_start & (SOC_CPU_PMP_REGION_GRANULARITY - 1)) == 0)
            && (drom_start > SOC_IROM_MASK_LOW) && (drom_start < SOC_DROM_MASK_HIGH)) {
        // Split the ROM into text (RX) and data (R), fully PMP-matched (this target has spare
        // entries, so the ROM text is not left PMP-unmatched as on ESP32-C5/S31).
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

    // 3. IRAM and DRAM
    if (esp_cpu_dbgr_is_attached()) {
        // Anti-FI check that cpu is really in ocd mode
        ESP_FAULT_ASSERT(esp_cpu_dbgr_is_attached());

        PMP_ENTRY_SET(4, SOC_IRAM_LOW, NONE);
        PMP_ENTRY_SET(5, SOC_HP_RAM_HIGH, PMP_TOR | RWX);
        _Static_assert(SOC_IRAM_LOW < SOC_IRAM_HIGH, "Invalid RAM region");
    } else {
#if CONFIG_ESP_SYSTEM_MEMPROT && CONFIG_ESP_SYSTEM_MEMPROT_PMP
        extern int _iram_text_end;
        PMP_ENTRY_SET(4, SOC_IRAM_LOW, NONE);
        PMP_ENTRY_SET(5, (int)&_iram_text_end, PMP_TOR | RX);
        PMP_ENTRY_SET(6, SOC_HP_RAM_HIGH, PMP_TOR | RW);
#else
        PMP_ENTRY_SET(4, SOC_IRAM_LOW, NONE);
        PMP_ENTRY_SET(5, SOC_HP_RAM_HIGH, PMP_TOR | RWX);
        _Static_assert(SOC_IRAM_LOW < SOC_IRAM_HIGH, "Invalid RAM region");
#endif
    }

    // 4. External memory window (unified I/D-Cache aperture for flash and PSRAM)
#if CONFIG_ESP_SYSTEM_MEMPROT && CONFIG_ESP_SYSTEM_MEMPROT_PMP
    extern int _instruction_reserved_end;
    extern int _rodata_reserved_end;

    const uint32_t page_aligned_irom_resv_end = ALIGN_UP_TO_MMU_PAGE_SIZE((uint32_t)(&_instruction_reserved_end));
    __attribute__((unused)) const uint32_t page_aligned_drom_resv_end = ALIGN_UP_TO_MMU_PAGE_SIZE((uint32_t)(&_rodata_reserved_end));

    PMP_ENTRY_SET(7, SOC_IROM_LOW, NONE);

/**
    Virtual space layout (single window shared by flash and PSRAM mappings):

    _________   <- SOC_IROM_LOW
    |       |
    |_______|   <- _instruction_reserved_end
    |_______|   <- page_aligned_irom_resv_end
    |       |
    |_______|   <- _rodata_reserved_end
    |_______|   <- page_aligned_drom_resv_end
    |       |
    |       |
    |       |
    |_______|   <- page_aligned_drom_resv_end + available_psram_heap
    |       |
    |       |
    |       |
    |       |
    |_______|   <- SOC_DROM_HIGH

    if CONFIG_SPIRAM_FETCH_INSTRUCTIONS: [_instruction_reserved_end, page_aligned_irom_resv_end) in heap (RW)
    if CONFIG_SPIRAM_RODATA: [_rodata_reserved_end, page_aligned_drom_resv_end) in heap (RW)
    if CONFIG_SPIRAM: [_rodata_reserved_end, page_aligned_drom_resv_end + available_psram_heap] in heap / reserved for mapping (RW)
*/

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS && CONFIG_SPIRAM_PRE_CONFIGURE_MEMORY_PROTECTION
    PMP_ENTRY_SET(8, (uint32_t)(&_instruction_reserved_end), PMP_TOR | RX);
    PMP_ENTRY_SET(9, page_aligned_irom_resv_end, PMP_TOR | RW);
#else
    PMP_ENTRY_SET(8, page_aligned_irom_resv_end, PMP_TOR | RX);
    PMP_ENTRY_SET(9, page_aligned_irom_resv_end, NONE);
#endif /* CONFIG_SPIRAM_FETCH_INSTRUCTIONS && CONFIG_SPIRAM_PRE_CONFIGURE_MEMORY_PROTECTION */

#if CONFIG_SPIRAM_RODATA && CONFIG_SPIRAM_PRE_CONFIGURE_MEMORY_PROTECTION
    PMP_ENTRY_SET(10, (uint32_t)(&_rodata_reserved_end), PMP_TOR | R);
#else
    PMP_ENTRY_SET(10, page_aligned_drom_resv_end, PMP_TOR | R);
#endif /* CONFIG_SPIRAM_RODATA && CONFIG_SPIRAM_PRE_CONFIGURE_MEMORY_PROTECTION */

#if CONFIG_SPIRAM_PRE_CONFIGURE_MEMORY_PROTECTION
    const size_t available_psram_heap = esp_psram_get_heap_size_to_protect();
    PMP_ENTRY_SET(11, ESP_ALIGN_UP(page_aligned_drom_resv_end + available_psram_heap, SOC_CPU_PMP_REGION_GRANULARITY), PMP_TOR | RW);
#endif /* CONFIG_SPIRAM_PRE_CONFIGURE_MEMORY_PROTECTION */
#else
    const uint32_t pmpaddr7 = PMPADDR_NAPOT(SOC_IROM_LOW, SOC_IROM_HIGH);
#if CONFIG_SPIRAM
    // The W permission is needed since PSRAM data is mapped into this window as well
    PMP_ENTRY_SET(7, pmpaddr7, PMP_NAPOT | RWX);
#else
    PMP_ENTRY_SET(7, pmpaddr7, PMP_NAPOT | RX);
#endif
    _Static_assert(SOC_IROM_LOW < SOC_IROM_HIGH, "Invalid I/D_Cache region");
#endif

    // 5. Peripheral addresses
    const uint32_t pmpaddr15 = PMPADDR_NAPOT(SOC_PERIPHERAL_LOW, SOC_PERIPHERAL_HIGH);
    PMP_ENTRY_SET(15, pmpaddr15, PMP_NAPOT | RW);
    _Static_assert(SOC_PERIPHERAL_LOW < SOC_PERIPHERAL_HIGH, "Invalid peripheral region");
}
#endif // BOOTLOADER_BUILD

void esp_cpu_configure_region_protection(void)
{
    /* Notes on implementation:
     *
     * 1) ESP32-H4 supports 16 PMA and 16 PMP regions. PMA blocks the invalid address ranges and
     *    marks the external memory window as cacheable; PMP maps the valid address space bottom
     *    to top with NAPOT and TOR entries, the peripherals using the last entry.
     *
     * 2) Unlike ESP32-S31/P4, external flash and PSRAM share a single 32 MB window
     *    ([SOC_IROM_LOW, SOC_IROM_HIGH)), so the PSRAM sections are carved out of that window.
     *    There is no LP/RTC RAM on this target, so no LP memory entries are needed.
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
