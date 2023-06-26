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

void esp_cpu_configure_region_protection(void)
{
    /* Notes on implementation:
     *
     * 1) Note: ESP32-C3 CPU doesn't support overlapping PMP regions
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
    const unsigned R       = PMP_L | PMP_TOR | PMP_R;
    const unsigned RW      = PMP_L | PMP_TOR | PMP_R | PMP_W;
    const unsigned RX      = PMP_L | PMP_TOR | PMP_R | PMP_X;
    const unsigned RWX     = PMP_L | PMP_TOR | PMP_R | PMP_W | PMP_X;

    // 1. Gap at bottom of address space
    PMP_ENTRY_SET(0, SOC_DEBUG_LOW, NONE);

    // 2. Debug region
    PMP_ENTRY_SET(1, SOC_DEBUG_HIGH, RWX);
    _Static_assert(SOC_DEBUG_LOW < SOC_DEBUG_HIGH, "Invalid CPU debug region");

    // 3. Gap between debug region & DROM (flash cache)
    PMP_ENTRY_SET(2, SOC_DROM_LOW, NONE);
    _Static_assert(SOC_DEBUG_HIGH < SOC_DROM_LOW, "Invalid PMP entry order");

    // 4. DROM (flash cache)
    // 5. Gap between DROM & DRAM
    // (Note: To save PMP entries these two are merged into one read-only region)
    PMP_ENTRY_SET(3, SOC_DRAM_LOW, R);
    _Static_assert(SOC_DROM_LOW < SOC_DROM_HIGH, "Invalid DROM region");
    _Static_assert(SOC_DROM_HIGH < SOC_DRAM_LOW, "Invalid PMP entry order");

    // 6. DRAM
    PMP_ENTRY_SET(4, SOC_DRAM_HIGH, RW);
    _Static_assert(SOC_DRAM_LOW < SOC_DRAM_HIGH, "Invalid DRAM region");

    // 7. Gap between DRAM and Mask DROM
    // 8. Mask DROM
    // (Note: to save PMP entries these two are merged into one read-only region)
    PMP_ENTRY_SET(5, SOC_DROM_MASK_HIGH, R);
    _Static_assert(SOC_DRAM_HIGH < SOC_DROM_MASK_LOW, "Invalid PMP entry order");
    _Static_assert(SOC_DROM_MASK_LOW < SOC_DROM_MASK_HIGH, "Invalid mask DROM region");

    // 9. Gap between mask DROM and mask IROM
    // 10. Mask IROM
    // (Note: to save PMP entries these two are merged into one RX region)
    PMP_ENTRY_SET(6, SOC_IROM_MASK_HIGH, RX);
    _Static_assert(SOC_DROM_MASK_HIGH < SOC_IROM_MASK_LOW, "Invalid PMP entry order");
    _Static_assert(SOC_IROM_MASK_LOW < SOC_IROM_MASK_HIGH, "Invalid mask IROM region");

    // 11. Gap between mask IROM & IRAM
    PMP_ENTRY_SET(7, SOC_IRAM_LOW, NONE);
    _Static_assert(SOC_IROM_MASK_HIGH < SOC_IRAM_LOW, "Invalid PMP entry order");

    // 12. IRAM
    PMP_ENTRY_SET(8, SOC_IRAM_HIGH, RWX);
    _Static_assert(SOC_IRAM_LOW < SOC_IRAM_HIGH, "Invalid IRAM region");

    // 13. Gap between IRAM and IROM
    // 14. IROM (flash cache)
    // (Note: to save PMP entries these two are merged into one RX region)
    PMP_ENTRY_SET(9, SOC_IROM_HIGH, RX);
    _Static_assert(SOC_IRAM_HIGH < SOC_IROM_LOW, "Invalid PMP entry order");
    _Static_assert(SOC_IROM_LOW < SOC_IROM_HIGH, "Invalid IROM region");

    // 15. Gap between IROM & RTC slow memory
    PMP_ENTRY_SET(10, SOC_RTC_IRAM_LOW, NONE);
    _Static_assert(SOC_IROM_HIGH < SOC_RTC_IRAM_LOW, "Invalid PMP entry order");

    // 16. RTC fast memory
    PMP_ENTRY_SET(11, SOC_RTC_IRAM_HIGH, RWX);
    _Static_assert(SOC_RTC_IRAM_LOW < SOC_RTC_IRAM_HIGH, "Invalid RTC IRAM region");

    // 17. Gap between RTC fast memory & peripheral addresses
    PMP_ENTRY_SET(12, SOC_PERIPHERAL_LOW, NONE);
    _Static_assert(SOC_RTC_IRAM_HIGH < SOC_PERIPHERAL_LOW, "Invalid PMP entry order");

    // 18. Peripheral addresses
    PMP_ENTRY_SET(13, SOC_PERIPHERAL_HIGH, RW);
    _Static_assert(SOC_PERIPHERAL_LOW < SOC_PERIPHERAL_HIGH, "Invalid peripheral region");

    // 19. End of address space
    PMP_ENTRY_SET(14, UINT32_MAX, NONE); // all but last 4 bytes
    PMP_ENTRY_SET(15, UINT32_MAX, PMP_L | PMP_NA4);  // last 4 bytes
}
