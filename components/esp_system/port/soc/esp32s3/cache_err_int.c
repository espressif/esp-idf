/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
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
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_cpu.h"
#include "esp_intr_alloc.h"
#include "soc/soc.h"
#include "soc/periph_defs.h"
#include "esp_rom_sys.h"
#include "hal/cache_ll.h"
#include "soc/syscon_struct.h"
#include "esp_private/cache_err_int.h"

static const char *TAG = "CACHE_ERR";

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
 * EXTMEM_CACHE_ILG_INT_ST_REG. each bit is accompanied by a small
 * message.
 * The messages have been pulled from the header file where the status bit
 * are defined.
*/
const register_bit_t ilg_int_st_bits[] = {
    {
        .bit = EXTMEM_ICACHE_SYNC_OP_FAULT_ST,
        .msg = "Icache sync parameter configuration error",
        .fault_addr_reg = 0,
        .fault_size_reg = 0,
    },
    {
        .bit = EXTMEM_ICACHE_PRELOAD_OP_FAULT_ST,
        .msg = "Icache preload parameter configuration error",
        .fault_addr_reg = 0,
        .fault_size_reg = 0,
    },
    {
        .bit = EXTMEM_DCACHE_SYNC_OP_FAULT_ST,
        .msg = "Dcache sync parameter configuration error",
        .fault_addr_reg = 0,
        .fault_size_reg = 0,
    },
    {
        .bit = EXTMEM_DCACHE_PRELOAD_OP_FAULT_ST,
        .msg = "Dcache preload parameter configuration error",
        .fault_addr_reg = 0,
        .fault_size_reg = 0,
    },
    {
        .bit = EXTMEM_DCACHE_WRITE_FLASH_ST,
        .msg = "Write back error occurred while dcache tries to write back to flash\r\nThe following backtrace may not indicate the code that caused Cache invalid access",
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
    const uint32_t illegal_intr_status = cache_ll_l1_get_illegal_error_intr_status(0, CACHE_LL_L1_ILG_EVENT_MASK);

    get_cache_error(illegal_intr_status, ilg_int_st_bits, DIM(ilg_int_st_bits), err_info);

}

void esp_cache_err_int_init(void)
{
    uint32_t core_id = esp_cpu_get_core_id();
    ESP_INTR_DISABLE(ETS_CACHEERR_INUM);

    // We do not register a handler for the interrupt because it is interrupt
    // level 4 which is not serviceable from C. Instead, xtensa_vectors.S has
    // a call to the panic handler for this interrupt.
    esp_rom_route_intr_matrix(core_id, ETS_CACHE_IA_INTR_SOURCE, ETS_CACHEERR_INUM);

    // Enable invalid cache access interrupt when the cache is disabled.
    // When the interrupt happens, we can not determine the CPU where the
    // invalid cache access has occurred. We enable the interrupt to catch
    // invalid access on both CPUs, but the interrupt is connected to the
    // CPU which happens to call this function.
    // For this reason, panic handler backtrace will not be correct if the
    // interrupt is connected to PRO CPU and invalid access happens on the APP CPU.

    ESP_DRAM_LOGV(TAG, "illegal error intr clr & ena mask is: 0x%x", CACHE_LL_L1_ILG_EVENT_MASK);
    //illegal error intr doesn't depend on cache_id
    cache_ll_l1_clear_illegal_error_intr(0, CACHE_LL_L1_ILG_EVENT_MASK);
    cache_ll_l1_enable_illegal_error_intr(0, CACHE_LL_L1_ILG_EVENT_MASK);

    if (core_id == PRO_CPU_NUM) {
        esp_rom_route_intr_matrix(core_id, ETS_CACHE_CORE0_ACS_INTR_SOURCE, ETS_CACHEERR_INUM);

        /* On the hardware side, stat by clearing all the bits responsible for
         * enabling cache access error interrupts.  */
        ESP_DRAM_LOGV(TAG, "core 0 access error intr clr & ena mask is: 0x%x", CACHE_LL_L1_ACCESS_EVENT_MASK);
        cache_ll_l1_clear_access_error_intr(0, CACHE_LL_L1_ACCESS_EVENT_MASK);
        cache_ll_l1_enable_access_error_intr(0, CACHE_LL_L1_ACCESS_EVENT_MASK);
    } else {
        esp_rom_route_intr_matrix(core_id, ETS_CACHE_CORE1_ACS_INTR_SOURCE, ETS_CACHEERR_INUM);

        /* On the hardware side, stat by clearing all the bits responsible for
         * enabling cache access error interrupts.  */
        ESP_DRAM_LOGV(TAG, "core 1 access error intr clr & ena mask is: 0x%x", CACHE_LL_L1_ACCESS_EVENT_MASK);
        cache_ll_l1_clear_access_error_intr(1, CACHE_LL_L1_ACCESS_EVENT_MASK);
        cache_ll_l1_enable_access_error_intr(1, CACHE_LL_L1_ACCESS_EVENT_MASK);
    }

    ESP_INTR_ENABLE(ETS_CACHEERR_INUM);
}

int esp_cache_err_get_cpuid(void)
{
    if (cache_ll_l1_get_access_error_intr_status(0, CACHE_LL_L1_ACCESS_EVENT_MASK)) {
        return PRO_CPU_NUM;
    }

    if (cache_ll_l1_get_access_error_intr_status(1, CACHE_LL_L1_ACCESS_EVENT_MASK)) {
        return APP_CPU_NUM;
    }

    return -1;
}
