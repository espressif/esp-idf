/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "sdkconfig.h"
#include "soc/soc.h"
#include "esp_cpu.h"
#include "esp_fault.h"

// ESP32H2-TODO: IDF-6452
void esp_cpu_configure_region_protection(void)
{
    /* Notes on implementation:
     *
     * 1) Note: ESP32-H2 CPU doesn't support overlapping PMP regions
     *
     * 2) Therefore, we use TOR (top of range) entries to map the whole address
     * space, bottom to top.
     *
     * 3) There are not enough entries to describe all the memory regions 100% accurately.
     *
     * 4) This means some gaps (invalid memory) are accessible. Priority for extending regions
     * to cover gaps is to extend read-only or read-execute regions or read-only regions only
     * (executing unmapped addresses should always fault with invalid instruction, read-only means
     * stores will correctly fault even if reads may return some invalid value.)
     *
     * 5) Entries are grouped in order with some static asserts to try and verify everything is
     * correct.
     */
    const unsigned NONE = PMP_L | PMP_TOR;
    const unsigned RW      = PMP_L | PMP_TOR | PMP_R | PMP_W;
    const unsigned RX      = PMP_L | PMP_TOR | PMP_R | PMP_X;
    const unsigned RWX     = PMP_L | PMP_TOR | PMP_R | PMP_W | PMP_X;

    // 1. Gap at bottom of address space
    PMP_ENTRY_SET(0, SOC_DEBUG_LOW, NONE);

    // 2. Debug region
    PMP_ENTRY_SET(1, SOC_DEBUG_HIGH, RWX);
    _Static_assert(SOC_DEBUG_LOW < SOC_DEBUG_HIGH, "Invalid CPU debug region");

    // 3. Gap between debug region & IROM
    PMP_ENTRY_SET(2, SOC_IROM_MASK_LOW, NONE);
    _Static_assert(SOC_DEBUG_HIGH < SOC_IROM_MASK_LOW, "Invalid PMP entry order");

    // 4. ROM
    PMP_ENTRY_SET(3, SOC_DROM_MASK_HIGH, RX);
    _Static_assert(SOC_IROM_MASK_LOW < SOC_DROM_MASK_HIGH, "Invalid ROM region");

    // 5. Gap between ROM & RAM
    PMP_ENTRY_SET(4, SOC_IRAM_LOW, NONE);
    _Static_assert(SOC_DROM_MASK_HIGH < SOC_IRAM_LOW, "Invalid PMP entry order");

    // 6. RAM
    PMP_ENTRY_SET(5, SOC_IRAM_HIGH, RWX);
    _Static_assert(SOC_IRAM_LOW < SOC_IRAM_HIGH, "Invalid RAM region");

    // 7. Gap between DRAM and I_Cache
    PMP_ENTRY_SET(6, SOC_IROM_LOW, NONE);
    _Static_assert(SOC_IRAM_HIGH < SOC_IROM_LOW, "Invalid PMP entry order");

    // 8. I_Cache (flash)
    PMP_ENTRY_SET(7, SOC_IROM_HIGH, RWX);
    _Static_assert(SOC_IROM_LOW < SOC_IROM_HIGH, "Invalid I_Cache region");

    // 9. D_Cache (flash)
    PMP_ENTRY_SET(8, SOC_DROM_HIGH, RW);
    _Static_assert(SOC_DROM_LOW < SOC_DROM_HIGH, "Invalid D_Cache region");

    // 10. Gap between D_Cache & LP_RAM
    PMP_ENTRY_SET(9, SOC_RTC_IRAM_LOW, NONE);
    _Static_assert(SOC_DROM_HIGH < SOC_RTC_IRAM_LOW, "Invalid PMP entry order");

    // 16. LP memory
    PMP_ENTRY_SET(10, SOC_RTC_IRAM_HIGH, RWX);
    _Static_assert(SOC_RTC_IRAM_LOW < SOC_RTC_IRAM_HIGH, "Invalid RTC IRAM region");

    // 17. Gap between LP memory & peripheral addresses
    PMP_ENTRY_SET(11, SOC_PERIPHERAL_LOW, NONE);
    _Static_assert(SOC_RTC_IRAM_HIGH < SOC_PERIPHERAL_LOW, "Invalid PMP entry order");

    // 18. Peripheral addresses
    PMP_ENTRY_SET(12, SOC_PERIPHERAL_HIGH, RW);
    _Static_assert(SOC_PERIPHERAL_LOW < SOC_PERIPHERAL_HIGH, "Invalid peripheral region");

    // 19. End of address space
    PMP_ENTRY_SET(13, UINT32_MAX, NONE); // all but last 4 bytes
    PMP_ENTRY_SET(14, UINT32_MAX, PMP_L | PMP_NA4);  // last 4 bytes
}
