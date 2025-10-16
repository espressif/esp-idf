/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "sdkconfig.h"
#include "soc/soc.h"
#include "soc/ext_mem_defs.h"
#include "esp_cpu.h"
#include "esp_fault.h"
#include "esp32c5/rom/rom_layout.h"
#include "esp_tee.h"

void esp_tee_configure_region_protection(void)
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
    const unsigned NONE    = PMP_L;
    const unsigned R       = PMP_L | PMP_R;
    const unsigned RW      = PMP_L | PMP_R | PMP_W;
    const unsigned RX      = PMP_L | PMP_R | PMP_X;
    const unsigned RWX     = PMP_L | PMP_R | PMP_W | PMP_X;

    //
    // Configure all the valid address regions using PMP
    //

    // 1. CPU Subsystem region - contains interrupt config registers
    PMP_ENTRY_CFG_RESET(0);
    const uint32_t pmpaddr0 = PMPADDR_NAPOT(SOC_CPU_SUBSYSTEM_LOW, SOC_CPU_SUBSYSTEM_HIGH);
    PMP_ENTRY_SET(0, pmpaddr0, PMP_NAPOT | RWX);
    _Static_assert(SOC_CPU_SUBSYSTEM_LOW < SOC_CPU_SUBSYSTEM_HIGH, "Invalid CPU subsystem region");

    // 2. I/D-ROM
    PMP_ENTRY_CFG_RESET(1);
    PMP_ENTRY_CFG_RESET(2);
    PMP_ENTRY_CFG_RESET(3);
    const uint32_t drom_start = (uint32_t)(ets_rom_layout_p->drom_start);
    if ((drom_start & (SOC_CPU_PMP_REGION_GRANULARITY - 1)) == 0) {
        PMP_ENTRY_SET(1, SOC_IROM_MASK_LOW, NONE);
        PMP_ENTRY_SET(2, drom_start, PMP_TOR | RX);
        PMP_ENTRY_SET(3, SOC_DROM_MASK_HIGH, PMP_TOR | R);
    } else {
        PMP_ENTRY_SET(1, SOC_IROM_MASK_LOW, NONE);
        PMP_ENTRY_SET(2, SOC_IROM_MASK_HIGH, PMP_TOR | RX);
        _Static_assert(SOC_IROM_MASK_LOW < SOC_IROM_MASK_HIGH, "Invalid I/D-ROM region");
    }

    // 3. IRAM and DRAM
    PMP_ENTRY_CFG_RESET(4);
    PMP_ENTRY_CFG_RESET(5);
    PMP_ENTRY_CFG_RESET(6);
    if (esp_cpu_dbgr_is_attached()) {
        // Anti-FI check that cpu is really in ocd mode
        ESP_FAULT_ASSERT(esp_cpu_dbgr_is_attached());
        PMP_ENTRY_SET(4, SOC_IRAM_LOW, NONE);
        PMP_ENTRY_SET(5, SOC_IRAM_HIGH, PMP_TOR | RWX);
        _Static_assert(SOC_IRAM_LOW < SOC_IRAM_HIGH, "Invalid RAM region");
    } else {
        // TODO: [IDF-13827] TEE SRAM region to be partitioned into text and data sections using APM
        // REE SRAM (D/IRAM)
        PMP_ENTRY_SET(4, (int)SOC_NS_IRAM_START, NONE);
        PMP_ENTRY_SET(5, (int)esp_tee_app_config.ns_iram_end, PMP_TOR | RX);
        PMP_ENTRY_SET(6, SOC_DRAM_HIGH, PMP_TOR | RW);
    }

    const uint32_t s_irom_resv_end = SOC_IROM_LOW + CONFIG_SECURE_TEE_IROM_SIZE + CONFIG_SECURE_TEE_DROM_SIZE;
    const uint32_t ns_irom_resv_end = ALIGN_UP_TO_MMU_PAGE_SIZE((uint32_t)esp_tee_app_config.ns_irom_end);
    const uint32_t ns_drom_resv_end = ALIGN_UP_TO_MMU_PAGE_SIZE((uint32_t)esp_tee_app_config.ns_drom_end);
    const uint32_t ns_drom_mmap_end = (uint32_t)(SOC_S_MMU_MMAP_RESV_START_VADDR);

    // 4. I_Cache / D_Cache (flash) - REE
    PMP_ENTRY_CFG_RESET(7);
    PMP_ENTRY_CFG_RESET(8);
    PMP_ENTRY_CFG_RESET(9);
    PMP_ENTRY_CFG_RESET(10);
    PMP_ENTRY_SET(7, s_irom_resv_end, NONE);
    PMP_ENTRY_SET(8, ns_irom_resv_end, PMP_TOR | RX);
    PMP_ENTRY_SET(9, ns_drom_resv_end, PMP_TOR | R);
    PMP_ENTRY_SET(10, ns_drom_mmap_end, PMP_TOR | RX);

    // 5. LP memory
    // TODO: LP memory to be further partitioned into text and data regions using APM
    PMP_ENTRY_CFG_RESET(11);
    const uint32_t pmpaddr11 = PMPADDR_NAPOT(SOC_RTC_IRAM_LOW, SOC_RTC_IRAM_HIGH);
    PMP_ENTRY_SET(11, pmpaddr11, PMP_NAPOT | RWX);
    _Static_assert(SOC_RTC_IRAM_LOW < SOC_RTC_IRAM_HIGH, "Invalid RTC IRAM region");

    // 6. Peripheral addresses
    PMP_ENTRY_CFG_RESET(12);
    const uint32_t pmpaddr12 = PMPADDR_NAPOT(SOC_PERIPHERAL_LOW, SOC_PERIPHERAL_HIGH);
    PMP_ENTRY_SET(12, pmpaddr12, PMP_NAPOT | RW);
    _Static_assert(SOC_PERIPHERAL_LOW < SOC_PERIPHERAL_HIGH, "Invalid peripheral region");

    //TODO: Add protection for SPIRAM
}
