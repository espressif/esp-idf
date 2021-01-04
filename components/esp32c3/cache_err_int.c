// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*
 The cache has an interrupt that can be raised as soon as an access to a cached
 region (flash) is done without the cache being enabled. We use that here
 to panic the CPU, which from a debugging perspective is better than grabbing bad
 data from the bus.
*/
#include "esp32c3/rom/ets_sys.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "soc/extmem_reg.h"
#include "soc/periph_defs.h"
#include "riscv/interrupt.h"

void esp_cache_err_int_init(void)
{
    const uint32_t core_id = 0;

    /* Disable cache interrupts if enabled. */
    ESP_INTR_DISABLE(ETS_CACHEERR_INUM);

    /**
     *  Bind all cache errors to ETS_CACHEERR_INUM interrupt. we will deal with
     * them in handler by different types
     * I) Cache access error
     *     1. dbus trying to write to icache
     *     2. dbus authentication fail
     *     3. cpu access icache while dbus is disabled [1]
     *     4. ibus authentication fail
     *     5. ibus trying to write icache
     *     6. cpu access icache while ibus is disabled
     * II) Cache illegal error
     *     1. dbus counter overflow
     *     2. ibus counter overflow
     *     3. mmu entry fault
     *     4. icache preload configurations fault
     *     5. icache sync configuration fault
     *
     * [1]: On ESP32C3 boards, the caches are shared but buses are still
     * distinct. So, we have an ibus and a dbus sharing the same cache.
     * This error can occur if the dbus performs a request but the icache
     * (or simply cache) is disabled.
     */
    intr_matrix_set(core_id, ETS_CACHE_IA_INTR_SOURCE, ETS_CACHEERR_INUM);
    intr_matrix_set(core_id, ETS_CACHE_CORE0_ACS_INTR_SOURCE, ETS_CACHEERR_INUM);

    /* Set the type and priority to cache error interrupts. */
    esprv_intc_int_set_type(BIT(ETS_CACHEERR_INUM), INTR_TYPE_LEVEL);
    esprv_intc_int_set_priority(ETS_CACHEERR_INUM, SOC_INTERRUPT_LEVEL_MEDIUM);

    /* On the hardware side, stat by clearing all the bits reponsible for
    * enabling cache access error interrupts.  */
    SET_PERI_REG_MASK(EXTMEM_CORE0_ACS_CACHE_INT_CLR_REG,
                      EXTMEM_CORE0_DBUS_WR_IC_INT_CLR |
                      EXTMEM_CORE0_DBUS_REJECT_INT_CLR |
                      EXTMEM_CORE0_DBUS_ACS_MSK_IC_INT_CLR |
                      EXTMEM_CORE0_IBUS_REJECT_INT_CLR |
                      EXTMEM_CORE0_IBUS_WR_IC_INT_CLR |
                      EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_CLR);

    /* Enable these interrupts. */
    SET_PERI_REG_MASK(EXTMEM_CORE0_ACS_CACHE_INT_ENA_REG,
                      EXTMEM_CORE0_DBUS_WR_IC_INT_ENA |
                      EXTMEM_CORE0_DBUS_REJECT_INT_ENA |
                      EXTMEM_CORE0_DBUS_ACS_MSK_IC_INT_ENA |
                      EXTMEM_CORE0_IBUS_REJECT_INT_ENA |
                      EXTMEM_CORE0_IBUS_WR_IC_INT_ENA |
                      EXTMEM_CORE0_IBUS_ACS_MSK_IC_INT_ENA);

    /* Same goes for cache illegal error: start by clearing the bits and then
    * set them back. */
    SET_PERI_REG_MASK(EXTMEM_CACHE_ILG_INT_CLR_REG,
                      EXTMEM_MMU_ENTRY_FAULT_INT_CLR |
                      EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_CLR |
                      EXTMEM_ICACHE_SYNC_OP_FAULT_INT_CLR);

    SET_PERI_REG_MASK(EXTMEM_CACHE_ILG_INT_ENA_REG,
                      EXTMEM_MMU_ENTRY_FAULT_INT_ENA |
                      EXTMEM_ICACHE_PRELOAD_OP_FAULT_INT_ENA |
                      EXTMEM_ICACHE_SYNC_OP_FAULT_INT_ENA);

    /* Enable the interrupts for cache error. */
    ESP_INTR_ENABLE(ETS_CACHEERR_INUM);
}

int IRAM_ATTR esp_cache_err_get_cpuid(void)
{
    return 0;
}
