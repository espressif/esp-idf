/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32P4 && (CONFIG_ESP_REV_MIN_FULL == 300)
#include "soc/hp_system_reg.h"
#include "hal/mmu_ll.h"
#include "hal/mspi_ll.h"

void sleep_flash_p4_rev3_workaround(void)
{
    REG_CLR_BIT(SPI_MEM_C_CACHE_FCTRL_REG, SPI_MEM_C_CLOSE_AXI_INF_EN);
    REG_SET_BIT(SPI_MEM_C_CACHE_FCTRL_REG, SPI_MEM_C_AXI_REQ_EN);
    REG_SET_FIELD(HP_SYSTEM_CORE_ERR_RESP_DIS_REG, HP_SYSTEM_CORE_ERR_RESP_DIS, 0x7);
    REG_WRITE(SPI_MEM_C_MMU_ITEM_INDEX_REG, 0);
    uint32_t mmu_backup = mmu_ll_read_entry(MMU_LL_FLASH_MMU_ID, 0);
    mmu_ll_write_entry(MMU_LL_FLASH_MMU_ID, 0, 0, MMU_TARGET_FLASH0);
    __attribute__((unused)) volatile uint32_t val = 0;
    val = REG_READ(0x80000000);
    val = REG_READ(0x80000080);
    mmu_ll_write_entry(MMU_LL_FLASH_MMU_ID, 0, mmu_backup, MMU_TARGET_FLASH0);
    _mspi_timing_ll_reset_mspi();
    _mspi_timing_ll_reset_mspi_apb();
    REG_SET_FIELD(HP_SYSTEM_CORE_ERR_RESP_DIS_REG, HP_SYSTEM_CORE_ERR_RESP_DIS, 0);
}
#endif
