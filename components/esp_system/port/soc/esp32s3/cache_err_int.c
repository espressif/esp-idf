/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file cache_err_int.c
 * @brief The cache has an interrupt that can be raised as soon as an access to a cached
 *        region (Flash, PSRAM) is done without the cache being enabled.
 *        We use that here to panic the CPU, which from a debugging perspective,
 *        is better than grabbing bad data from the bus.
 */

#include <stdint.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "soc/soc.h"
#include "soc/extmem_reg.h"
#include "soc/periph_defs.h"
#include "hal/cpu_hal.h"
#include "esp32s3/dport_access.h"
#include "esp32s3/rom/ets_sys.h"

void esp_cache_err_int_init(void)
{
    uint32_t core_id = cpu_hal_get_core_id();
    ESP_INTR_DISABLE(ETS_CACHEERR_INUM);

    // We do not register a handler for the interrupt because it is interrupt
    // level 4 which is not serviceable from C. Instead, xtensa_vectors.S has
    // a call to the panic handler for this interrupt.
    intr_matrix_set(core_id, ETS_CACHE_IA_INTR_SOURCE, ETS_CACHEERR_INUM);

    // Enable invalid cache access interrupt when the cache is disabled.
    // When the interrupt happens, we can not determine the CPU where the
    // invalid cache access has occurred. We enable the interrupt to catch
    // invalid access on both CPUs, but the interrupt is connected to the
    // CPU which happens to call this function.
    // For this reason, panic handler backtrace will not be correct if the
    // interrupt is connected to PRO CPU and invalid access happens on the APP CPU.

    SET_PERI_REG_MASK(EXTMEM_CACHE_ILG_INT_CLR_REG,
                      EXTMEM_MMU_ENTRY_FAULT_INT_CLR |
                      EXTMEM_DCACHE_WRITE_FLASH_INT_CLR |
                      EXTMEM_DCACHE_PRELOAD_OP_FAULT_INT_CLR |
                      EXTMEM_DCACHE_SYNC_OP_FAULT_INT_CLR |
                      EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_CLR |
                      EXTMEM_ICACHE_SYNC_OP_FAULT_INT_CLR);
    SET_PERI_REG_MASK(EXTMEM_CACHE_ILG_INT_ENA_REG,
                      EXTMEM_MMU_ENTRY_FAULT_INT_ENA |
                      EXTMEM_DCACHE_WRITE_FLASH_INT_ENA |
                      EXTMEM_DCACHE_PRELOAD_OP_FAULT_INT_ENA |
                      EXTMEM_DCACHE_SYNC_OP_FAULT_INT_ENA |
                      EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_ENA |
                      EXTMEM_ICACHE_SYNC_OP_FAULT_INT_ENA);

    if (core_id == PRO_CPU_NUM) {
        intr_matrix_set(core_id, ETS_CACHE_CORE0_ACS_INTR_SOURCE, ETS_CACHEERR_INUM);
        /* On the hardware side, stat by clearing all the bits reponsible for
         * enabling cache access error interrupts.  */
        SET_PERI_REG_MASK(EXTMEM_CORE0_ACS_CACHE_INT_CLR_REG,
                EXTMEM_CORE0_DBUS_REJECT_INT_CLR |
                EXTMEM_CORE0_DBUS_ACS_MSK_DC_INT_CLR |
                EXTMEM_CORE0_IBUS_REJECT_INT_CLR |
                EXTMEM_CORE0_IBUS_WR_IC_INT_CLR |
                EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_CLR);

        /* Enable cache access error interrupts */
        SET_PERI_REG_MASK(EXTMEM_CORE0_ACS_CACHE_INT_ENA_REG,
                EXTMEM_CORE0_DBUS_REJECT_INT_ENA |
                EXTMEM_CORE0_DBUS_ACS_MSK_DC_INT_ENA |
                EXTMEM_CORE0_IBUS_REJECT_INT_ENA |
                EXTMEM_CORE0_IBUS_WR_IC_INT_ENA |
                EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_ENA);
    } else {
        intr_matrix_set(core_id, ETS_CACHE_CORE1_ACS_INTR_SOURCE, ETS_CACHEERR_INUM);

        /* On the hardware side, stat by clearing all the bits reponsible for
         * enabling cache access error interrupts.  */
        SET_PERI_REG_MASK(EXTMEM_CORE1_ACS_CACHE_INT_CLR_REG,
                EXTMEM_CORE1_DBUS_REJECT_INT_CLR |
                EXTMEM_CORE1_DBUS_ACS_MSK_DC_INT_CLR |
                EXTMEM_CORE1_IBUS_REJECT_INT_CLR |
                EXTMEM_CORE1_IBUS_WR_IC_INT_CLR |
                EXTMEM_CORE1_IBUS_ACS_MSK_IC_INT_CLR);

        /* Enable cache access error interrupts */
        SET_PERI_REG_MASK(EXTMEM_CORE1_ACS_CACHE_INT_ENA_REG,
                EXTMEM_CORE1_DBUS_REJECT_INT_ENA |
                EXTMEM_CORE1_DBUS_ACS_MSK_DC_INT_ENA |
                EXTMEM_CORE1_IBUS_REJECT_INT_ENA |
                EXTMEM_CORE1_IBUS_WR_IC_INT_ENA |
                EXTMEM_CORE1_IBUS_ACS_MSK_IC_INT_ENA);
    }

    ESP_INTR_ENABLE(ETS_CACHEERR_INUM);
}

int IRAM_ATTR esp_cache_err_get_cpuid(void)
{
    const uint32_t pro_mask = EXTMEM_CORE0_DBUS_REJECT_ST |
                EXTMEM_CORE0_DBUS_ACS_MSK_DCACHE_ST |
                EXTMEM_CORE0_IBUS_REJECT_ST |
                EXTMEM_CORE0_IBUS_WR_ICACHE_ST |
                EXTMEM_CORE0_IBUS_ACS_MSK_ICACHE_ST;

    if (GET_PERI_REG_MASK(EXTMEM_CORE0_ACS_CACHE_INT_ST_REG, pro_mask)) {
        return PRO_CPU_NUM;
    }

    const uint32_t app_mask = EXTMEM_CORE1_DBUS_REJECT_ST |
                EXTMEM_CORE1_DBUS_ACS_MSK_DCACHE_ST |
                EXTMEM_CORE1_IBUS_REJECT_ST |
                EXTMEM_CORE1_IBUS_WR_ICACHE_ST |
                EXTMEM_CORE1_IBUS_ACS_MSK_ICACHE_ST;

    if (GET_PERI_REG_MASK(EXTMEM_CORE1_ACS_CACHE_INT_ST_REG, app_mask)) {
        return APP_CPU_NUM;
    }

    return -1;
}
