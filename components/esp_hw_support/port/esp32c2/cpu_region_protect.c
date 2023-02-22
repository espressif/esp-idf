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

#if CONFIG_ESP_SYSTEM_PMP_IDRAM_SPLIT && !BOOTLOADER_BUILD
extern int _iram_end;
extern int _data_start;
#define IRAM_END        (int)&_iram_end
#define DRAM_START      (int)&_data_start
#else
#define IRAM_END        SOC_DIRAM_IRAM_HIGH
#define DRAM_START      SOC_DIRAM_DRAM_LOW
#endif

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

void esp_cpu_configure_region_protection(void)
{
    /* Notes on implementation:
     *
     * 1) ESP32-C2 CPU support overlapping PMP regions, configuration is based on static priority
     * feature(lowest numbered entry has highest priority).
     *
     * 2) Therefore, we use TOR (top of range) and NAOPT entries to map the effective area.
     * Finally, define any address without access permission.
     *
     * 3) 3-15 PMPADDR entries be hardcoded to fixed value, 0-2 PMPADDR be programmed to split ID SRAM
     * as IRAM/DRAM. All PMPCFG entryies be available.
     *
     * 4) Ideally, PMPADDR 0-2 entries should be configured twice, once during bootloader startup and another during app startup.
     *    However, the CPU currently always executes in machine mode and to enforce these permissions in machine mode, we need
     *    to set the Lock (L) bit but if set once, it cannot be reconfigured. So, we only configure 0-2 PMPADDR during app startup.
     */
    const unsigned NONE    = PMP_L ;
    const unsigned R       = PMP_L | PMP_R;
    const unsigned X       = PMP_L | PMP_X;
    const unsigned RW      = PMP_L | PMP_R | PMP_W;
    const unsigned RX      = PMP_L | PMP_R | PMP_X;
    const unsigned RWX     = PMP_L | PMP_R | PMP_W | PMP_X;

    /* There are 4 configuration scenarios for PMPADDR 0-2
     *
     * 1. Bootloader build:
     *    - We cannot set the lock bit as we need to reconfigure it again for the application.
     *      We configure PMPADDR 0-1 to cover entire valid IRAM range and PMPADDR 2-3 to cover entire valid DRAM range.
     *
     * 2. Application build with CONFIG_ESP_SYSTEM_PMP_IDRAM_SPLIT enabled
     *    - We split the SRAM into IRAM and DRAM such that IRAM region cannot be accessed via DBUS
     *      and DRAM region cannot be accessed via IBUS. We use _iram_end and _data_start markers to set the boundaries.
     *      We also lock these entries so the R/W/X permissions are enforced even for machine mode
     *
     * 3. Application build with CONFIG_ESP_SYSTEM_PMP_IDRAM_SPLIT disabled
     *    - The IRAM-DRAM split is not enabled so we just need to ensure that access to only valid address ranges are successful
     *      so for that we set PMPADDR 0-1 to cover entire valid IRAM range and PMPADDR 2-3 to cover entire DRAM region.
     *      We also lock these entries so the R/W/X permissions are enforced even for machine mode
     *
     * 4. CPU is in OCD debug mode
     *    - The IRAM-DRAM split is not enabled so that OpenOCD can write and execute from IRAM.
     *      We set PMPADDR 0-1 to cover entire valid IRAM range and PMPADDR 2-3 to cover entire DRAM region.
     *      We also lock these entries so the R/W/X permissions are enforced even for machine mode
     *
     *  PMPADDR 3-15 are hard-coded and are appicable to both, bootloader and application. So we configure and lock
     *  these during BOOTLOADER build itself. During application build, reconfiguration of these PMPADDR entries
     *  are silently ignored by the CPU
     */

    if (esp_cpu_dbgr_is_attached()) {
        // Anti-FI check that cpu is really in ocd mode
        ESP_FAULT_ASSERT(esp_cpu_dbgr_is_attached());

        // 1. IRAM
        PMP_ENTRY_SET(0, SOC_DIRAM_IRAM_LOW, NONE);
        PMP_ENTRY_SET(1, SOC_DIRAM_IRAM_HIGH, PMP_TOR | RWX);

        // 2. DRAM
        PMP_ENTRY_SET(2, SOC_DIRAM_DRAM_LOW, NONE);
        PMP_ENTRY_CFG_SET(3, PMP_TOR | RW);
    } else {
        // 1. IRAM
        PMP_ENTRY_SET(0, SOC_DIRAM_IRAM_LOW, CONDITIONAL_NONE);

#if CONFIG_ESP_SYSTEM_PMP_IDRAM_SPLIT
        PMP_ENTRY_SET(1, IRAM_END, PMP_TOR | CONDITIONAL_RX);
#else
        PMP_ENTRY_SET(1, IRAM_END, PMP_TOR | CONDITIONAL_RWX);
#endif

        // 2. DRAM
        PMP_ENTRY_SET(2, DRAM_START, CONDITIONAL_NONE);
        PMP_ENTRY_CFG_SET(3, PMP_TOR | CONDITIONAL_RW);
    }

    // 3. Debug region
    PMP_ENTRY_CFG_SET(4, PMP_NAPOT | RWX);

    // 4. DROM (flash dcache)
    PMP_ENTRY_CFG_SET(5, PMP_NAPOT | R);

    // 5. DROM_MASK
    PMP_ENTRY_CFG_SET(6, NONE);
    PMP_ENTRY_CFG_SET(7, PMP_TOR | R);

    // 6. IROM_MASK
    PMP_ENTRY_CFG_SET(8, NONE);
    PMP_ENTRY_CFG_SET(9, PMP_TOR | RX);

    // 7. IROM (flash icache)
    PMP_ENTRY_CFG_SET(10, PMP_NAPOT | RX);

    // 8. Peripheral addresses
    PMP_ENTRY_CFG_SET(11, PMP_NAPOT | RW);

    // 9. SRAM (used as ICache)
    PMP_ENTRY_CFG_SET(12, PMP_NAPOT | X);

    // 10. no access to any address below(0x0-0xFFFF_FFFF)
    PMP_ENTRY_CFG_SET(13, PMP_NA4 | NONE);// last 4 bytes(0xFFFFFFFC)
    PMP_ENTRY_CFG_SET(14, NONE);
    PMP_ENTRY_CFG_SET(15, PMP_TOR | NONE);
}
