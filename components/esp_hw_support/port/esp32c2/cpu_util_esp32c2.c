/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <assert.h>
#include "esp_cpu.h"

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
     */
    const unsigned NONE = PMP_L ;
    const unsigned R       = PMP_L | PMP_R;
    const unsigned X       = PMP_L | PMP_X;
    const unsigned RW      = PMP_L | PMP_R | PMP_W;
    const unsigned RX      = PMP_L | PMP_R | PMP_X;
    const unsigned RWX     = PMP_L | PMP_R | PMP_W | PMP_X;

    // 1. IRAM
    PMP_ENTRY_SET(0,SOC_DIRAM_IRAM_LOW, NONE);
    PMP_ENTRY_SET(1,SOC_DIRAM_IRAM_HIGH, PMP_TOR|RWX); //TODO IRAM/DRAM spilt address

    // 2. DRAM
    PMP_ENTRY_SET(2,SOC_DIRAM_DRAM_LOW, NONE); //TODO IRAM/DRAM spilt address
    PMP_ENTRY_CFG_SET(3,PMP_TOR|RW);

    // 3. Debug region
    PMP_ENTRY_CFG_SET(4,PMP_NAPOT|RWX);

    // 4. DROM (flash dcache)
    PMP_ENTRY_CFG_SET(5,PMP_NAPOT|R);

    // 5. DROM_MASK
    PMP_ENTRY_CFG_SET(6,NONE);
    PMP_ENTRY_CFG_SET(7,PMP_TOR|R);

    // 6. IROM_MASK
    PMP_ENTRY_CFG_SET(8,NONE);
    PMP_ENTRY_CFG_SET(9,PMP_TOR|RX);

    // 7. IROM (flash icache)
    PMP_ENTRY_CFG_SET(10,PMP_NAPOT|RX);

    // 8. Peripheral addresses
    PMP_ENTRY_CFG_SET(11,PMP_NAPOT|RW);

    // 9. SRAM (used as ICache)
    PMP_ENTRY_CFG_SET(12,PMP_NAPOT|X);

    // 10. no access to any address below(0x0-0xFFFF_FFFF)
    PMP_ENTRY_CFG_SET(13,PMP_NA4|NONE);// last 4 bytes(0xFFFFFFFC)
    PMP_ENTRY_CFG_SET(14,NONE);
    PMP_ENTRY_CFG_SET(15,PMP_TOR|NONE);
}
