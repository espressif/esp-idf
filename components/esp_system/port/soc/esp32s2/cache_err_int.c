/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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
#include "esp_cpu.h"

#include "esp_intr_alloc.h"

#include "soc/extmem_reg.h"
#include "soc/dport_reg.h"
#include "soc/periph_defs.h"
#include "hal/cache_ll.h"

#include "esp_rom_sys.h"

#include "sdkconfig.h"
#include "esp_private/cache_err_int.h"

#define DIM(array) (sizeof(array)/sizeof(*array))

/**
 * Structure used to define a flag/bit to test in case of cache error.
 * The message describes the cause of the error when the bit is set in
 * a given status register.
 */
typedef struct {
    const uint32_t bit;
    const char *msg;
    const uint32_t fault_addr_reg;
    const uint32_t fault_size_reg;
} register_bit_t;

/* Define the array that contains the status (bits) to test on the register
 * EXTMEM_CACHE_DBG_STATUS0_REG. each bit is accompanied by a small
 * message.
 * The messages have been pulled from the header file where the status bit
 * are defined.
*/
const register_bit_t dbg_status0_bits[] = {
    {
        .bit = EXTMEM_IC_SYNC_SIZE_FAULT_ST,
        .msg = "Icache sync parameter configuration error",
        .fault_addr_reg = EXTMEM_PRO_ICACHE_MEM_SYNC0_REG,
        .fault_size_reg = EXTMEM_PRO_ICACHE_MEM_SYNC1_REG,
    },
    {
        .bit = EXTMEM_IC_PRELOAD_SIZE_FAULT_ST,
        .msg = "Icache preload parameter configuration error",
        .fault_addr_reg = EXTMEM_PRO_ICACHE_PRELOAD_ADDR_REG,
        .fault_size_reg = EXTMEM_PRO_ICACHE_PRELOAD_SIZE_REG,
    },
    {
        .bit = EXTMEM_ICACHE_REJECT_ST,
        .msg = "Icache reject error",
        .fault_addr_reg = EXTMEM_PRO_ICACHE_REJECT_VADDR_REG,
        .fault_size_reg = 0,
    },
};

/* Define the array that contains the status (bits) to test on the register
    * EXTMEM_CACHE_DBG_STATUS1_REG. each bit is accompanied by a small
    * message.
    * The messages have been pulled from the header file where the status bit
    * are defined. */
const register_bit_t dbg_status1_bits[] = {
    {
        .bit = EXTMEM_DC_SYNC_SIZE_FAULT_ST,
        .msg = "Dcache sync parameter configuration error",
        .fault_addr_reg = EXTMEM_PRO_DCACHE_MEM_SYNC0_REG,
        .fault_size_reg = EXTMEM_PRO_DCACHE_MEM_SYNC1_REG,
    },
    {
        .bit = EXTMEM_DC_PRELOAD_SIZE_FAULT_ST,
        .msg = "Dcache preload parameter configuration error",
        .fault_addr_reg = EXTMEM_PRO_DCACHE_PRELOAD_ADDR_REG,
        .fault_size_reg = EXTMEM_PRO_DCACHE_PRELOAD_SIZE_REG,
    },
    {
        .bit = EXTMEM_DCACHE_REJECT_ST,
        .msg = "Dcache reject error",
        .fault_addr_reg = EXTMEM_PRO_DCACHE_REJECT_VADDR_REG,
        .fault_size_reg = 0,
    },
    {
        .bit = EXTMEM_DCACHE_WRITE_FLASH_ST,
        .msg = "Write back error occurred while dcache tries to write back to flash",
        .fault_addr_reg = 0,
        .fault_size_reg = 0,
    },
    {
        .bit = EXTMEM_MMU_ENTRY_FAULT_ST,
        .msg = "MMU entry fault error",
        .fault_addr_reg = 0,
        .fault_size_reg = 0,
    },
};
/**
 * Function to check each bits defined in the array reg_bits in the given
 * status register. The first bit from the array to be set in the status
 * register will have its associated message printed. This function returns
 * true. If not bit was set in the register, it returns false.
 * The order of the bits in the array is important as only the first bit to
 * be set in the register will have its associated message printed.
 */
static inline void get_cache_error(const uint32_t status, const register_bit_t *reg_bits,
                                   const uint32_t size, esp_cache_err_info_t *err_info)
{
    /* Browse the flag/bit array and test each one with the given status
     * register. */
    for (int i = 0; i < size; i++) {
        const uint32_t bit = reg_bits[i].bit;
        if ((status & bit) == bit) {
            /* Reason of the panic found, print the reason. */
            err_info->err_str = reg_bits[i].msg;
            err_info->vaddr = reg_bits[i].fault_addr_reg ? REG_READ(reg_bits[i].fault_addr_reg) : 0;
            err_info->size = reg_bits[i].fault_size_reg ? REG_READ(reg_bits[i].fault_size_reg) : 0;

            return;
        }
    }
}

void esp_cache_err_get_panic_info(esp_cache_err_info_t *err_info)
{
    if (err_info == NULL) {
        return;
    }
    /* Read the status register EXTMEM_CACHE_ILG_INT_ST_REG. This status
     * register is not equal to 0 when a cache access error occurred. */
    const uint32_t dbg_status0 = cache_ll_get_dbg_status0();
    const uint32_t dbg_status1 = cache_ll_get_dbg_status1();

    get_cache_error(dbg_status0, dbg_status0_bits, DIM(dbg_status0_bits), err_info);

    if (err_info->err_str == NULL) {
        get_cache_error(dbg_status1, dbg_status1_bits, DIM(dbg_status1_bits), err_info);
    }
}

void esp_cache_err_int_init(void)
{
    uint32_t core_id = esp_cpu_get_core_id();
    ESP_INTR_DISABLE(ETS_MEMACCESS_ERR_INUM);

    // We do not register a handler for the interrupt because it is interrupt
    // level 4 which is not serviceable from C. Instead, xtensa_vectors.S has
    // a call to the panic handler for
    // this interrupt.
    esp_rom_route_intr_matrix(core_id, ETS_CACHE_IA_INTR_SOURCE, ETS_MEMACCESS_ERR_INUM);

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

int esp_cache_err_get_cpuid(void)
{
    if (REG_READ(EXTMEM_CACHE_DBG_STATUS0_REG) != 0 ||
            REG_READ(EXTMEM_CACHE_DBG_STATUS1_REG) != 0) {
        return PRO_CPU_NUM;
    }
    return -1;
}
