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

#ifdef BOOTLOADER_BUILD
// Without L bit set
#define CONDITIONAL_NONE        0x0
#define CONDITIONAL_RX          PMP_R | PMP_X
#define CONDITIONAL_RW          PMP_R | PMP_W
#define CONDITIONAL_RWX         PMP_R | PMP_W | PMP_X
#else
// With L bit set
#define CONDITIONAL_NONE        NONE
#define CONDITIONAL_RX          RX
#define CONDITIONAL_RW          RW
#define CONDITIONAL_RWX         RWX
#endif

//TODO: [ESP32H4] IDF-12588

static void esp_cpu_configure_invalid_regions(void)
{
    abort();
}

void esp_cpu_configure_region_protection(void)
{
    // ROM has configured the MSPI region with RX permission, we should add W attribute for psram
    PMA_ENTRY_SET_NAPOT(0, SOC_IROM_LOW, (SOC_IROM_HIGH - SOC_IROM_LOW), PMA_NAPOT | PMA_L | PMA_EN | PMA_R | PMA_W | PMA_X);
    return;
    /* Notes on implementation:
     *
     * 1) Note: ESP32-H4 CPU doesn't support overlapping PMP regions
     *
     * 2) ESP32-H4 supports 16 PMA regions so we use this feature to block all the invalid address ranges
     *
     * 3) We use combination of NAPOT (Naturally Aligned Power Of Two) and TOR (top of range)
     * entries to map all the valid address space, bottom to top. This leaves us with some extra PMP entries
     * which can be used to provide more granular access
     *
     * 4) Entries are grouped in order with some static asserts to try and verify everything is
     * correct.
     */

    /* There are 4 configuration scenarios for SRAM
     *
     * 1. Bootloader build:
     *    - We cannot set the lock bit as we need to reconfigure it again for the application.
     *      We configure PMP to cover entire valid IRAM and DRAM range.
     *
     * 2. Application build with CONFIG_ESP_SYSTEM_MEMPROT enabled
     *    - We split the SRAM into IRAM and DRAM such that IRAM region cannot be written to
     *      and DRAM region cannot be executed. We use _iram_end and _data_start markers to set the boundaries.
     *      We also lock these entries so the R/W/X permissions are enforced even for machine mode
     *
     * 3. Application build with CONFIG_ESP_SYSTEM_MEMPROT disabled
     *    - The IRAM-DRAM split is not enabled so we just need to ensure that access to only valid address ranges are successful
     *      so for that we set PMP to cover entire valid IRAM and DRAM region.
     *      We also lock these entries so the R/W/X permissions are enforced even for machine mode
     *
     * 4. CPU is in OCD debug mode
     *    - The IRAM-DRAM split is not enabled so that OpenOCD can write and execute from IRAM.
     *      We set PMP to cover entire valid IRAM and DRAM region.
     *      We also lock these entries so the R/W/X permissions are enforced even for machine mode
     */
    const unsigned NONE    = PMP_L;
    const unsigned R       = PMP_L | PMP_R;
    const unsigned RW      = PMP_L | PMP_R | PMP_W;
    const unsigned RX      = PMP_L | PMP_R | PMP_X;
    const unsigned RWX     = PMP_L | PMP_R | PMP_W | PMP_X;

    //
    // Configure all the invalid address regions using PMA
    //
    esp_cpu_configure_invalid_regions();

    //
    // Configure all the valid address regions using PMP
    //

    // 1. Debug region
    const uint32_t pmpaddr0 = PMPADDR_NAPOT(SOC_DEBUG_LOW, SOC_DEBUG_HIGH);
    PMP_ENTRY_SET(0, pmpaddr0, PMP_NAPOT | RWX);
    _Static_assert(SOC_DEBUG_LOW < SOC_DEBUG_HIGH, "Invalid CPU debug region");

    // 2.1 I-ROM
    PMP_ENTRY_SET(1, SOC_IROM_MASK_LOW, NONE);
    PMP_ENTRY_SET(2, SOC_IROM_MASK_HIGH, PMP_TOR | RX);
    _Static_assert(SOC_IROM_MASK_LOW < SOC_IROM_MASK_HIGH, "Invalid I-ROM region");

    // 2.2 D-ROM
    PMP_ENTRY_SET(3, SOC_DROM_MASK_LOW, NONE);
    PMP_ENTRY_SET(4, SOC_DROM_MASK_HIGH, PMP_TOR | R);
    _Static_assert(SOC_DROM_MASK_LOW < SOC_DROM_MASK_HIGH, "Invalid D-ROM region");

    if (esp_cpu_dbgr_is_attached()) {
        // Anti-FI check that cpu is really in ocd mode
        ESP_FAULT_ASSERT(esp_cpu_dbgr_is_attached());

        // 5. IRAM and DRAM
        // const uint32_t pmpaddr5 = PMPADDR_NAPOT(SOC_IRAM_LOW, SOC_IRAM_HIGH);
        // PMP_ENTRY_SET(5, pmpaddr5, PMP_NAPOT | RWX);
        // _Static_assert(SOC_IRAM_LOW < SOC_IRAM_HIGH, "Invalid RAM region");
        PMP_ENTRY_SET(5, SOC_IRAM_LOW, NONE);
        PMP_ENTRY_SET(6, SOC_IRAM_HIGH, PMP_TOR | RWX);
        _Static_assert(SOC_IRAM_LOW < SOC_IRAM_HIGH, "Invalid RAM region");
    } else {
#if CONFIG_ESP_SYSTEM_MEMPROT && CONFIG_ESP_SYSTEM_MEMPROT_PMP && !BOOTLOADER_BUILD
        extern int _iram_end;
        // 5. IRAM and DRAM
        /* Reset the corresponding PMP config because PMP_ENTRY_SET only sets the given bits
         * Bootloader might have given extra permissions and those won't be cleared
         */
        PMP_ENTRY_CFG_RESET(5);
        PMP_ENTRY_CFG_RESET(6);
        PMP_ENTRY_CFG_RESET(7);
        PMP_ENTRY_SET(5, SOC_IRAM_LOW, NONE);
        PMP_ENTRY_SET(6, (int)&_iram_end, PMP_TOR | RX);
        PMP_ENTRY_SET(7, SOC_DRAM_HIGH, PMP_TOR | RW);
#else
        // 5. IRAM and DRAM
        // const uint32_t pmpaddr5 = PMPADDR_NAPOT(SOC_IRAM_LOW, SOC_IRAM_HIGH);
        // PMP_ENTRY_SET(5, pmpaddr5, PMP_NAPOT | CONDITIONAL_RWX);
        // _Static_assert(SOC_IRAM_LOW < SOC_IRAM_HIGH, "Invalid RAM region");
        PMP_ENTRY_SET(5, SOC_IRAM_LOW, NONE);
        PMP_ENTRY_SET(6, SOC_IRAM_HIGH, PMP_TOR | RWX);
        _Static_assert(SOC_IRAM_LOW < SOC_IRAM_HIGH, "Invalid RAM region");
#endif
    }

    // 4. I_Cache (flash)
    const uint32_t pmpaddr8 = PMPADDR_NAPOT(SOC_IROM_LOW, SOC_IROM_HIGH);
    PMP_ENTRY_SET(8, pmpaddr8, PMP_NAPOT | RX);
    _Static_assert(SOC_IROM_LOW < SOC_IROM_HIGH, "Invalid I_Cache region");

    // 5. D_Cache (flash)
    const uint32_t pmpaddr9 = PMPADDR_NAPOT(SOC_DROM_LOW, SOC_DROM_HIGH);
    PMP_ENTRY_SET(9, pmpaddr9, PMP_NAPOT | R);
    _Static_assert(SOC_DROM_LOW < SOC_DROM_HIGH, "Invalid D_Cache region");

    // 6. LP memory
#if CONFIG_ESP_SYSTEM_MEMPROT && CONFIG_ESP_SYSTEM_MEMPROT_PMP && !BOOTLOADER_BUILD
    extern int _rtc_text_end;
    /* Reset the corresponding PMP config because PMP_ENTRY_SET only sets the given bits
     * Bootloader might have given extra permissions and those won't be cleared
     */
    PMP_ENTRY_CFG_RESET(10);
    PMP_ENTRY_CFG_RESET(11);
    PMP_ENTRY_CFG_RESET(12);
    PMP_ENTRY_CFG_RESET(13);
    PMP_ENTRY_SET(10, SOC_RTC_IRAM_LOW, NONE);

    // Repeat same previous entry, to ensure next entry has correct base address (TOR)
    PMP_ENTRY_SET(11, SOC_RTC_IRAM_LOW, NONE);
    PMP_ENTRY_SET(12, (int)&_rtc_text_end, PMP_TOR | RX);
    PMP_ENTRY_SET(13, SOC_RTC_IRAM_HIGH, PMP_TOR | RW);
#else
    const uint32_t pmpaddr10 = PMPADDR_NAPOT(SOC_RTC_IRAM_LOW, SOC_RTC_IRAM_HIGH);
    PMP_ENTRY_SET(10, pmpaddr10, PMP_NAPOT | CONDITIONAL_RWX);
    _Static_assert(SOC_RTC_IRAM_LOW < SOC_RTC_IRAM_HIGH, "Invalid RTC IRAM region");
#endif


    // 7. Peripheral addresses
    const uint32_t pmpaddr14 = PMPADDR_NAPOT(SOC_PERIPHERAL_LOW, SOC_PERIPHERAL_HIGH);
    PMP_ENTRY_SET(14, pmpaddr14, PMP_NAPOT | RW);
    _Static_assert(SOC_PERIPHERAL_LOW < SOC_PERIPHERAL_HIGH, "Invalid peripheral region");
}
