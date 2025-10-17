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

#define IS_PMA_ENTRY_UNLOCKED(ENTRY) \
    ((RV_READ_CSR((CSR_PMACFG0) + (ENTRY)) & PMA_L) == 0)

static void esp_tee_configure_invalid_regions(void)
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

    // 7. Using PMA to configure the TEE text and data section access attribute. */
    PMA_ENTRY_CFG_RESET(12);
    assert(IS_PMA_ENTRY_UNLOCKED(13));
    assert(IS_PMA_ENTRY_UNLOCKED(14));
    assert(IS_PMA_ENTRY_UNLOCKED(15));

    extern int _tee_iram_end;
    PMA_RESET_AND_ENTRY_SET_TOR(13, SOC_S_IRAM_START, PMA_NONE);
    PMA_RESET_AND_ENTRY_SET_TOR(14, (int)&_tee_iram_end, PMA_TOR | PMA_RX);
    PMA_RESET_AND_ENTRY_SET_TOR(15, SOC_S_DRAM_END, PMA_TOR | PMA_RW);
}

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
    // Configure all the invalid address regions using PMA
    //
    // We lock the PMA entries since they mark the invalid regions and is applicable to both the privilege modes
    //
    esp_tee_configure_invalid_regions();

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
