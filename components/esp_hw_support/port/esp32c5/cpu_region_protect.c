/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "soc/soc.h"
#include "esp_cpu.h"
#include "esp_fault.h"
#include "esp_macros.h"
#include "esp32c5/rom/rom_layout.h"
#if !BOOTLOADER_BUILD && CONFIG_SPIRAM
#include "esp_private/esp_psram_extram.h"
#endif /* !BOOTLOADER_BUILD && CONFIG_SPIRAM */

#include "pmp_layout.h"

#define ALIGN_UP_TO_MMU_PAGE_SIZE(addr)    ESP_ALIGN_UP(addr, SOC_MMU_PAGE_SIZE)
#define ALIGN_DOWN_TO_MMU_PAGE_SIZE(addr)  ESP_ALIGN_DOWN(addr, SOC_MMU_PAGE_SIZE)

static void esp_cpu_configure_invalid_regions(void)
{
    const unsigned PMA_NONE                            = PMA_L | PMA_EN;
    __attribute__((unused)) const unsigned PMA_RW      = PMA_L | PMA_EN | PMA_R | PMA_W;
    __attribute__((unused)) const unsigned PMA_RX      = PMA_L | PMA_EN | PMA_R | PMA_X;
    __attribute__((unused)) const unsigned PMA_RWX     = PMA_L | PMA_EN | PMA_R | PMA_W | PMA_X;

    // ROM uses some PMA entries, so we need to clear them before using them in ESP-IDF

    // 0. Gap at bottom of address space
    PMA_RESET_AND_ENTRY_SET_NAPOT(0, 0, SOC_CPU_SUBSYSTEM_LOW, PMA_NAPOT | PMA_NONE);

    // 1. Gap between debug region & IROM
    PMA_RESET_AND_ENTRY_SET_TOR(1, SOC_CPU_SUBSYSTEM_HIGH, PMA_NONE);
    PMA_RESET_AND_ENTRY_SET_TOR(2, SOC_IROM_MASK_LOW, PMA_TOR | PMA_NONE);

    // 2. ROM has configured the ROM region to be cacheable, so we just need to lock the configuration
    PMA_RESET_AND_ENTRY_SET_TOR(3, SOC_IROM_MASK_LOW, PMA_NONE);
    PMA_RESET_AND_ENTRY_SET_TOR(4, SOC_DROM_MASK_HIGH, PMA_TOR | PMA_RX);

    // 3. Gap between DRAM and I_Cache
    PMA_RESET_AND_ENTRY_SET_TOR(5, SOC_IRAM_HIGH, PMA_NONE);
    PMA_RESET_AND_ENTRY_SET_TOR(6, SOC_IROM_LOW, PMA_TOR | PMA_NONE);

    // 4. ROM has configured the MSPI region with RX permission, we should add W attribute for psram and lock the configuration
    // This function sets invalid regions but this is a valid memory region configuration that could have
    // been configured using PMP as well, but due to insufficient PMP entries we are configuring this using PMA.
    // This entry is also required to be set using PMA because the region needs to be configured as cacheable.
    PMA_RESET_AND_ENTRY_SET_NAPOT(7, SOC_IROM_LOW, (SOC_IROM_HIGH - SOC_IROM_LOW), PMA_NAPOT | PMA_RWX);

    // 5. Gap between D_Cache & LP_RAM
    PMA_RESET_AND_ENTRY_SET_TOR(8, SOC_DROM_HIGH, PMA_NONE);
    PMA_RESET_AND_ENTRY_SET_TOR(9, SOC_RTC_IRAM_LOW, PMA_TOR | PMA_NONE);

    // 6. End of address space
    PMA_RESET_AND_ENTRY_SET_TOR(10, SOC_PERIPHERAL_HIGH, PMA_NONE);
    PMA_RESET_AND_ENTRY_SET_TOR(11, UINT32_MAX, PMA_TOR | PMA_NONE);

    /* NOTE: ESP-TEE [IDF-13827]
     *
     * Reserving some PMA entries to repurpose them for partitioning the
     * TEE SRAM as IRAM (RX) and DRAM (RW). Thus, with ESP-TEE enabled,
     * invalid region accesses to the LP memory → peripherals and
     * ROM → RAM regions will not raise exceptions. (treated as no-ops)
     */
#if !CONFIG_SECURE_ENABLE_TEE
    // 9. Gap between ROM & RAM
    PMA_RESET_AND_ENTRY_SET_TOR(12, SOC_DROM_MASK_HIGH, PMA_NONE);
    PMA_RESET_AND_ENTRY_SET_TOR(13, SOC_IRAM_LOW, PMA_TOR | PMA_NONE);

    // 10. Gap between LP memory & peripheral addresses
    PMA_RESET_AND_ENTRY_SET_TOR(14, SOC_RTC_IRAM_HIGH, PMA_NONE);
    PMA_RESET_AND_ENTRY_SET_TOR(15, SOC_PERIPHERAL_LOW, PMA_TOR | PMA_NONE);
#else
    PMA_ENTRY_CFG_RESET(12);
    PMA_ENTRY_CFG_RESET(13);
    PMA_ENTRY_CFG_RESET(14);
    PMA_ENTRY_CFG_RESET(15);
#endif
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
    __attribute__((unused)) const unsigned RW      = PMP_L | PMP_R | PMP_W;
    __attribute__((unused)) const unsigned RX      = PMP_L | PMP_R | PMP_X;
    __attribute__((unused)) const unsigned RWX     = PMP_L | PMP_R | PMP_W | PMP_X;

    // 1. CPU Subsystem region - contains interrupt config registers
    const uint32_t pmpaddr0 = PMPADDR_NAPOT(SOC_CPU_SUBSYSTEM_LOW, SOC_CPU_SUBSYSTEM_HIGH);
    PMP_RESET_AND_ENTRY_SET(PMP_ENTRY_CPU_SUBSYSTEM, pmpaddr0, PMP_NAPOT | RWX);
    _Static_assert(SOC_CPU_SUBSYSTEM_LOW < SOC_CPU_SUBSYSTEM_HIGH, "Invalid CPU subsystem region");

    // 2. I/D-ROM
#if CONFIG_ESP_SYSTEM_MEMPROT && CONFIG_ESP_SYSTEM_MEMPROT_PMP
    const uint32_t drom_start = (uint32_t) (ets_rom_layout_p->drom_start);
    if ((drom_start & (SOC_CPU_PMP_REGION_GRANULARITY - 1)) == 0) {
        // We can skip configuring the PMP entry for the [SOC_IROM_MASK_LOW - drom_start]
        // region as RX, as we already have configured a PMA entry with RX permissions for the
        // [SOC_IROM_MASK_LOW - SOC_DROM_MASK_HIGH] region that helps us to also configure
        // the region as cacheable. Thus, we save on one PMP entry.
        /* No cfg reset on the two ROM entries, deliberately: v6.0/v6.1
         * bootloaders lock PMP_ENTRY_ROM_LOW at SOC_IROM_MASK_LOW, so on such
         * devices the TOR region below spans the ROM text and must keep the X
         * bit those bootloaders left in PMP_ENTRY_ROM_HIGH. PMP_ENTRY_SET only
         * ORs cfg bits and cannot clear it; on newer bootloaders (no PMP
         * writes) the entries are clean and ROM data gets the intended R.
         */
        PMP_ENTRY_SET(PMP_ENTRY_ROM_LOW, drom_start, NONE);
        PMP_ENTRY_SET(PMP_ENTRY_ROM_HIGH, SOC_DROM_MASK_HIGH, PMP_TOR | R);
    } else
#endif
    {
        PMP_RESET_AND_ENTRY_SET(PMP_ENTRY_ROM_LOW, SOC_IROM_MASK_LOW, NONE);
        PMP_RESET_AND_ENTRY_SET(PMP_ENTRY_ROM_HIGH, SOC_IROM_MASK_HIGH, PMP_TOR | RX);
        _Static_assert(SOC_IROM_MASK_LOW < SOC_IROM_MASK_HIGH, "Invalid I/D-ROM region");
    }

    // 3. IRAM and DRAM
    if (esp_cpu_dbgr_is_attached()) {
        // Anti-FI check that cpu is really in ocd mode
        ESP_FAULT_ASSERT(esp_cpu_dbgr_is_attached());

        PMP_RESET_AND_ENTRY_SET(3, SOC_IRAM_LOW, NONE);
        PMP_RESET_AND_ENTRY_SET(4, SOC_IRAM_HIGH, PMP_TOR | RWX);
        _Static_assert(SOC_IRAM_LOW < SOC_IRAM_HIGH, "Invalid RAM region");
    } else {
#if CONFIG_ESP_SYSTEM_MEMPROT && CONFIG_ESP_SYSTEM_MEMPROT_PMP
        extern int _iram_text_end;
        PMP_RESET_AND_ENTRY_SET(3, SOC_IRAM_LOW, NONE);
        PMP_RESET_AND_ENTRY_SET(4, (int)&_iram_text_end, PMP_TOR | RX);
        PMP_RESET_AND_ENTRY_SET(5, SOC_DRAM_HIGH, PMP_TOR | RW);
#else
        PMP_RESET_AND_ENTRY_SET(3, SOC_IRAM_LOW, NONE);
        PMP_RESET_AND_ENTRY_SET(4, SOC_IRAM_HIGH, PMP_TOR | RWX);
        _Static_assert(SOC_IRAM_LOW < SOC_IRAM_HIGH, "Invalid RAM region");
#endif
    }

    // 4. I_Cache / D_Cache (flash)
#if CONFIG_ESP_SYSTEM_MEMPROT && CONFIG_ESP_SYSTEM_MEMPROT_PMP
    extern int _instruction_reserved_end;
    extern int _rodata_reserved_end;

    const uint32_t page_aligned_irom_resv_end = ALIGN_UP_TO_MMU_PAGE_SIZE((uint32_t)(&_instruction_reserved_end));
    __attribute__((unused)) const uint32_t page_aligned_drom_resv_end = ALIGN_UP_TO_MMU_PAGE_SIZE((uint32_t)(&_rodata_reserved_end));

    PMP_RESET_AND_ENTRY_SET(6, SOC_IROM_LOW, NONE);

/**
    Virtual space layout:

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
    PMP_RESET_AND_ENTRY_SET(7, (uint32_t)(&_instruction_reserved_end), PMP_TOR | RX);
    PMP_RESET_AND_ENTRY_SET(8, page_aligned_irom_resv_end, PMP_TOR | RW);
#else
    PMP_RESET_AND_ENTRY_SET(7, page_aligned_irom_resv_end, PMP_TOR | RX);
    PMP_RESET_AND_ENTRY_SET(8, page_aligned_irom_resv_end, NONE);
#endif /* CONFIG_SPIRAM_FETCH_INSTRUCTIONS && CONFIG_SPIRAM_PRE_CONFIGURE_MEMORY_PROTECTION */

#if CONFIG_SPIRAM_RODATA && CONFIG_SPIRAM_PRE_CONFIGURE_MEMORY_PROTECTION
    PMP_RESET_AND_ENTRY_SET(9, (uint32_t)(&_rodata_reserved_end), PMP_TOR | R);
#else
    PMP_RESET_AND_ENTRY_SET(9, page_aligned_drom_resv_end, PMP_TOR | R);
#endif /* CONFIG_SPIRAM_RODATA && CONFIG_SPIRAM_PRE_CONFIGURE_MEMORY_PROTECTION */

#if CONFIG_SPIRAM_PRE_CONFIGURE_MEMORY_PROTECTION
    size_t available_psram_heap = esp_psram_get_heap_size_to_protect();
    PMP_RESET_AND_ENTRY_SET(10, ESP_ALIGN_UP(page_aligned_drom_resv_end + available_psram_heap, SOC_CPU_PMP_REGION_GRANULARITY), PMP_TOR | RW);
#endif /* CONFIG_SPIRAM_PRE_CONFIGURE_MEMORY_PROTECTION */
#else
    const uint32_t pmpaddr6 = PMPADDR_NAPOT(SOC_IROM_LOW, SOC_IROM_HIGH);
    // Add the W attribute in the case of PSRAM
    PMP_RESET_AND_ENTRY_SET(6, pmpaddr6, PMP_NAPOT | RWX);
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
     * 1) Note: ESP32-C5 CPU support overlapping PMP regions
     *
     * 2) ESP32-C5 supports 16 PMA regions so we use this feature to block all the invalid address ranges
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
