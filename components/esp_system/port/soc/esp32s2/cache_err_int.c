// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
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
 region (flash, psram) is done without the cache being enabled. We use that here
 to panic the CPU, which from a debugging perspective is better than grabbing bad
 data from the bus.
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "esp_err.h"
#include "esp_attr.h"

#include "esp_intr_alloc.h"

#include "soc/extmem_reg.h"
#include "soc/dport_reg.h"
#include "soc/periph_defs.h"
#include "hal/cpu_hal.h"

#include "esp32s2/dport_access.h"
#include "esp32s2/rom/ets_sys.h"  // for intr_matrix_set

#include "sdkconfig.h"

void esp_cache_err_int_init(void)
{
    uint32_t core_id = cpu_hal_get_core_id();
    ESP_INTR_DISABLE(ETS_MEMACCESS_ERR_INUM);

    // We do not register a handler for the interrupt because it is interrupt
    // level 4 which is not serviceable from C. Instead, xtensa_vectors.S has
    // a call to the panic handler for
    // this interrupt.
    intr_matrix_set(core_id, ETS_CACHE_IA_INTR_SOURCE, ETS_MEMACCESS_ERR_INUM);

    // Enable invalid cache access interrupt when the cache is disabled.
    // The status bits are cleared first, in case we are restarting after
    // a cache error has triggered.
    DPORT_SET_PERI_REG_MASK(EXTMEM_CACHE_DBG_INT_CLR_REG,
                            EXTMEM_MMU_ENTRY_FAULT_INT_CLR |
                            EXTMEM_DCACHE_REJECT_INT_CLR |
                            EXTMEM_DCACHE_WRITE_FLASH_INT_CLR |
                            EXTMEM_DC_PRELOAD_SIZE_FAULT_INT_CLR |
                            EXTMEM_DC_SYNC_SIZE_FAULT_INT_CLR |
                            EXTMEM_ICACHE_REJECT_INT_CLR |
                            EXTMEM_IC_PRELOAD_SIZE_FAULT_INT_CLR |
                            EXTMEM_IC_SYNC_SIZE_FAULT_INT_CLR);
    DPORT_SET_PERI_REG_MASK(EXTMEM_CACHE_DBG_INT_ENA_REG,
                            EXTMEM_MMU_ENTRY_FAULT_INT_ENA |
                            EXTMEM_DCACHE_REJECT_INT_ENA |
                            EXTMEM_DCACHE_WRITE_FLASH_INT_ENA |
                            EXTMEM_DC_PRELOAD_SIZE_FAULT_INT_ENA |
                            EXTMEM_DC_SYNC_SIZE_FAULT_INT_ENA |
                            EXTMEM_ICACHE_REJECT_INT_ENA |
                            EXTMEM_IC_PRELOAD_SIZE_FAULT_INT_ENA |
                            EXTMEM_IC_SYNC_SIZE_FAULT_INT_ENA |
                            EXTMEM_CACHE_DBG_EN);

    ESP_INTR_ENABLE(ETS_MEMACCESS_ERR_INUM);
}

int IRAM_ATTR esp_cache_err_get_cpuid(void)
{
    if (REG_READ(EXTMEM_CACHE_DBG_STATUS0_REG) != 0 ||
        REG_READ(EXTMEM_CACHE_DBG_STATUS1_REG) != 0) {
            return PRO_CPU_NUM;
    }
    return -1;
}
