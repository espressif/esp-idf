/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "sdkconfig.h"
#include "soc/soc.h"
#include "soc/ext_mem_defs.h"
#include "soc/lp_analog_peri_reg.h"
#include "soc/lp_wdt_reg.h"
#include "soc/plic_reg.h"
#include "soc/clint_reg.h"
#include "riscv/csr.h"
#include "esp_cpu.h"
#include "esp_fault.h"
#include "esp_tee.h"

#define CONDITIONAL_NONE        0x0
#define CONDITIONAL_R           PMP_R
#define CONDITIONAL_RX          PMP_R | PMP_X
#define CONDITIONAL_RW          PMP_R | PMP_W
#define CONDITIONAL_RWX         PMP_R | PMP_W | PMP_X

#define IS_PMA_ENTRY_UNLOCKED(ENTRY) \
    ((RV_READ_CSR((CSR_PMACFG0) + (ENTRY)) & PMA_L) == 0)

#define SWD_PROT_REG_START      (LP_WDT_SWD_CONFIG_REG)
#define SWD_PROT_REG_END        (LP_WDT_INT_CLR_REG)
#define BOD_PROT_REG_START      (DR_REG_LP_ANALOG_PERI_BASE)
#define BOD_PROT_REG_END        (DR_REG_LP_ANALOG_PERI_BASE + 0x40)

static void esp_tee_configure_invalid_regions(void)
{
    const unsigned PMA_NONE                            = PMA_L | PMA_EN;
    __attribute__((unused)) const unsigned PMA_RW      = PMA_L | PMA_EN | PMA_R | PMA_W;
    __attribute__((unused)) const unsigned PMA_RX      = PMA_L | PMA_EN | PMA_R | PMA_X;
    __attribute__((unused)) const unsigned PMA_RWX     = PMA_L | PMA_EN | PMA_R | PMA_W | PMA_X;

    // 1. Gap at bottom of address space
    PMA_ENTRY_SET_TOR(0, SOC_CPU_SUBSYSTEM_LOW, PMA_TOR | PMA_NONE);

    // 2. Gap between CPU subsystem region & IROM
    PMA_ENTRY_SET_TOR(1, SOC_CPU_SUBSYSTEM_HIGH, PMA_NONE);
    PMA_ENTRY_SET_TOR(2, SOC_IROM_MASK_LOW, PMA_TOR | PMA_NONE);

    // 3. Gap between ROM & RAM
    PMA_ENTRY_SET_TOR(3, SOC_DROM_MASK_HIGH, PMA_NONE);
    PMA_ENTRY_SET_TOR(4, SOC_IRAM_LOW, PMA_TOR | PMA_NONE);

    // 4. Gap between DRAM and I_Cache
    PMA_ENTRY_SET_TOR(5, SOC_IRAM_HIGH, PMA_NONE);
    PMA_ENTRY_SET_TOR(6, SOC_IROM_LOW, PMA_TOR | PMA_NONE);

    // 5. Gap between D_Cache & LP_RAM
    PMA_ENTRY_SET_TOR(7, SOC_DROM_HIGH, PMA_NONE);
    PMA_ENTRY_SET_TOR(8, SOC_RTC_IRAM_LOW, PMA_TOR | PMA_NONE);

    // 6. Gap between LP memory & peripheral addresses
    PMA_ENTRY_SET_TOR(9, SOC_RTC_IRAM_HIGH, PMA_NONE);
    PMA_ENTRY_SET_TOR(10, SOC_PERIPHERAL_LOW, PMA_TOR | PMA_NONE);

    // 7. End of address space
    PMA_ENTRY_SET_TOR(11, SOC_PERIPHERAL_HIGH, PMA_NONE);
    PMA_ENTRY_SET_TOR(12, UINT32_MAX, PMA_TOR | PMA_NONE);

    /* 8. Using PMA to configure the TEE text and data section access attribute. */
    assert(IS_PMA_ENTRY_UNLOCKED(13));
    assert(IS_PMA_ENTRY_UNLOCKED(14));
    assert(IS_PMA_ENTRY_UNLOCKED(15));
    PMA_RESET_AND_ENTRY_SET_TOR(13, SOC_S_IRAM_START, PMA_NONE);
    PMA_RESET_AND_ENTRY_SET_TOR(14, SOC_S_IRAM_END, PMA_TOR | PMA_RX);
    PMA_RESET_AND_ENTRY_SET_TOR(15, SOC_S_DRAM_END, PMA_TOR | PMA_RW);
}

void esp_tee_configure_region_protection(void)
{
    /* Notes on implementation:
     *
     * 1) Note: ESP32-C6 CPU doesn't support overlapping PMP regions
     *
     * 2) ESP32-C6 supports 16 PMA regions so we use this feature to block all the invalid address ranges
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

    // 1.1 I/D-ROM
    const uint32_t pmpaddr0 = PMPADDR_NAPOT(SOC_IROM_MASK_LOW, SOC_IROM_MASK_HIGH);
    PMP_ENTRY_SET(0, pmpaddr0, PMP_NAPOT | RX);
    _Static_assert(SOC_IROM_MASK_LOW < SOC_IROM_MASK_HIGH, "Invalid I/D-ROM region");

    /* TODO: Check whether changes are required here */
    if (esp_cpu_dbgr_is_attached()) {
        // Anti-FI check that cpu is really in ocd mode
        ESP_FAULT_ASSERT(esp_cpu_dbgr_is_attached());

        // 2. IRAM and DRAM
        PMP_ENTRY_SET(1, SOC_IRAM_LOW, NONE);
        PMP_ENTRY_SET(2, SOC_IRAM_HIGH, PMP_TOR | RWX);
        _Static_assert(SOC_IRAM_LOW < SOC_IRAM_HIGH, "Invalid RAM region");
    } else {
        // 2. IRAM and DRAM
        // Splitting the REE SRAM region into IRAM and DRAM
        PMP_ENTRY_SET(1, (int)SOC_NS_IRAM_START, NONE);
        PMP_ENTRY_SET(2, (int)esp_tee_app_config.ns_iram_end, PMP_TOR | RX);
        PMP_ENTRY_SET(3, SOC_DRAM_HIGH, PMP_TOR | RW);
    }

    const uint32_t s_irom_resv_end = SOC_IROM_LOW + CONFIG_SECURE_TEE_IROM_SIZE + CONFIG_SECURE_TEE_DROM_SIZE;
    const uint32_t ns_irom_resv_end = ALIGN_UP_TO_MMU_PAGE_SIZE((uint32_t)esp_tee_app_config.ns_irom_end);
    const uint32_t ns_drom_resv_end = ALIGN_UP_TO_MMU_PAGE_SIZE((uint32_t)esp_tee_app_config.ns_drom_end);
    const uint32_t ns_drom_mmap_end = (uint32_t)(SOC_S_MMU_MMAP_RESV_START_VADDR);

    // 4. I_Cache / D_Cache (flash) - REE
    PMP_ENTRY_CFG_RESET(5);
    PMP_ENTRY_CFG_RESET(6);
    PMP_ENTRY_CFG_RESET(7);
    PMP_ENTRY_SET(4, s_irom_resv_end, NONE);
    PMP_ENTRY_SET(5, ns_irom_resv_end, PMP_TOR | RX);
    PMP_ENTRY_SET(6, ns_drom_resv_end, PMP_TOR | R);
    PMP_ENTRY_SET(7, ns_drom_mmap_end, PMP_TOR | RX);

    // 5. LP memory
    /* Reset the corresponding PMP config because PMP_ENTRY_SET only sets the given bits
     * Bootloader might have given extra permissions and those won't be cleared
     */
    const uint32_t pmpaddr8 = PMPADDR_NAPOT(SOC_RTC_IRAM_LOW, SOC_RTC_IRAM_HIGH);
    PMP_ENTRY_SET(8, pmpaddr8, PMP_NAPOT | RWX);
    _Static_assert(SOC_RTC_IRAM_LOW < SOC_RTC_IRAM_HIGH, "Invalid RTC IRAM region");

    // 6. Super WDT and Brownout detector
    PMP_ENTRY_SET(9, SWD_PROT_REG_START, CONDITIONAL_NONE);
    PMP_ENTRY_SET(10, SWD_PROT_REG_END, PMP_TOR | CONDITIONAL_NONE);
    _Static_assert(SWD_PROT_REG_START < SWD_PROT_REG_END, "Invalid peripheral region");

    /* NOTE: Due to the limited number of PMP entries, NAPOT address matching had to be
     * utilized here. To meet the requirements of NAPOT, the adjacent 20 bytes have also
     * been protected along with the intended region.
     */
    const uint32_t pmpaddr11 = PMPADDR_NAPOT(BOD_PROT_REG_START, BOD_PROT_REG_END);
    PMP_ENTRY_SET(11, pmpaddr11, PMP_NAPOT | CONDITIONAL_NONE);
    _Static_assert(BOD_PROT_REG_START < BOD_PROT_REG_END, "Invalid peripheral region");

    // 7. Peripheral addresses
    const uint32_t pmpaddr12 = PMPADDR_NAPOT(SOC_PERIPHERAL_LOW, SOC_PERIPHERAL_HIGH);
    PMP_ENTRY_SET(12, pmpaddr12, PMP_NAPOT | RW);
    _Static_assert(SOC_PERIPHERAL_LOW < SOC_PERIPHERAL_HIGH, "Invalid peripheral region");

    // 8. User-mode interrupt controller registers
    const uint32_t pmpaddr13 = PMPADDR_NAPOT(DR_REG_PLIC_UX_BASE, DR_REG_PLIC_UX_END);
    PMP_ENTRY_SET(13, pmpaddr13, PMP_NAPOT | RW);
    _Static_assert(DR_REG_PLIC_UX_BASE < DR_REG_PLIC_UX_END, "Invalid User mode PLIC region");

    const uint32_t pmpaddr14 = PMPADDR_NAPOT(DR_REG_CLINT_U_BASE, DR_REG_CLINT_U_END);
    PMP_ENTRY_SET(14, pmpaddr14, PMP_NAPOT | RW);
    _Static_assert(DR_REG_CLINT_U_BASE < DR_REG_CLINT_U_END, "Invalid User mode CLINT region");
}
