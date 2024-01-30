/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 The cache has an interrupt that can be raised as soon as an access to a cached
 region (flash) is done without the cache being enabled. We use that here
 to panic the CPU, which from a debugging perspective is better than grabbing bad
 data from the bus.
*/
#include "esp_rom_sys.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include "soc/periph_defs.h"
#include "riscv/interrupt.h"
#include "hal/cache_ll.h"

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
} register_bit_t;

/* Define the array that contains the status (bits) to test on the register
    * EXTMEM_CORE0_ACS_CACHE_INT_ST_REG. each bit is accompanied by a small
    * message.
    * The messages have been pulled from the header file where the status bit
    * are defined. */
const register_bit_t core0_acs_bits[] = {
    {
        .bit = EXTMEM_CORE0_DBUS_WR_ICACHE_ST,
        .msg = "Dbus tried to write cache"
    },
    {
        .bit = EXTMEM_CORE0_DBUS_REJECT_ST,
        .msg = "Dbus authentication failed"
    },
    {
        .bit = EXTMEM_CORE0_DBUS_ACS_MSK_ICACHE_ST,
        .msg = "Cached memory region accessed while dbus or cache is disabled"
    },
    {
        .bit = EXTMEM_CORE0_IBUS_REJECT_ST,
        .msg = "Ibus authentication failed"
    },
    {
        .bit = EXTMEM_CORE0_IBUS_WR_ICACHE_ST,
        .msg = "Ibus tried to write cache"
    },
    {
        .bit = EXTMEM_CORE0_IBUS_ACS_MSK_ICACHE_ST,
        .msg = "Cached memory region accessed while ibus or cache is disabled"
    },
};

/* Same goes for the register EXTMEM_CACHE_ILG_INT_ST_REG and its bits. */
const register_bit_t cache_ilg_bits[] = {
    {
        .bit = EXTMEM_MMU_ENTRY_FAULT_ST,
        .msg = "MMU entry fault"
    },
    {
        .bit = EXTMEM_ICACHE_PRELOAD_OP_FAULT_ST,
        .msg = "Preload configurations fault"
    },
    {
        .bit = EXTMEM_ICACHE_SYNC_OP_FAULT_ST,
        .msg = "Sync configurations fault"
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
static inline const char* test_and_print_register_bits(const uint32_t status,
                                                       const register_bit_t *reg_bits,
                                                       const uint32_t size)
{
    /* Browse the flag/bit array and test each one with the given status
     * register. */
    for (int i = 0; i < size; i++) {
        const uint32_t bit = reg_bits[i].bit;
        if ((status & bit) == bit) {
            /* Reason of the panic found, print the reason. */
            return reg_bits[i].msg;
        }
    }

    /* Panic cause not found, no message was printed. */
    return NULL;
}

const char *esp_cache_err_panic_string(void)
{
    /* Read the status register EXTMEM_CORE0_ACS_CACHE_INT_ST_REG. This status
     * register is not equal to 0 when a cache access error occured. */
    const uint32_t access_err_status = cache_ll_l1_get_access_error_intr_status(0, CACHE_LL_L1_ACCESS_EVENT_MASK);

    /* If the panic is due to a cache access error, one of the bit of the
     * register is set. Thus, this function will return an error string. */
    const char* err_str = test_and_print_register_bits(access_err_status, core0_acs_bits, DIM(core0_acs_bits));

    /* If the panic was due to a cache illegal error, the previous call returned NULL and this
     * EXTMEM_CACHE_ILG_INT_ST_REG register should not be equal to 0.
     * Check each bit of it and return the message associated if found. */
    if (err_str == NULL) {
        const uint32_t cache_ilg_status = cache_ll_l1_get_illegal_error_intr_status(0, CACHE_LL_L1_ILG_EVENT_MASK);
        err_str = test_and_print_register_bits(cache_ilg_status, cache_ilg_bits, DIM(cache_ilg_bits));
    }

    return err_str;
}

bool esp_cache_err_has_active_err(void)
{
    return cache_ll_l1_get_access_error_intr_status(0, CACHE_LL_L1_ACCESS_EVENT_MASK) || cache_ll_l1_get_illegal_error_intr_status(0, CACHE_LL_L1_ILG_EVENT_MASK);
}

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
    esp_rom_route_intr_matrix(core_id, ETS_CACHE_IA_INTR_SOURCE, ETS_CACHEERR_INUM);
    esp_rom_route_intr_matrix(core_id, ETS_CACHE_CORE0_ACS_INTR_SOURCE, ETS_CACHEERR_INUM);

    /* Set the type and priority to cache error interrupts. */
    esprv_int_set_type(ETS_CACHEERR_INUM, INTR_TYPE_LEVEL);
    esprv_int_set_priority(ETS_CACHEERR_INUM, SOC_INTERRUPT_LEVEL_MEDIUM);

    ESP_DRAM_LOGV(TAG, "access error intr clr & ena mask is: 0x%x", CACHE_LL_L1_ACCESS_EVENT_MASK);
    /* On the hardware side, start by clearing all the bits reponsible for cache access error */
    cache_ll_l1_clear_access_error_intr(0, CACHE_LL_L1_ACCESS_EVENT_MASK);
    /* Then enable cache access error interrupts. */
    cache_ll_l1_enable_access_error_intr(0, CACHE_LL_L1_ACCESS_EVENT_MASK);

    /* Same goes for cache illegal error: start by clearing the bits and then
    * set them back. */
    ESP_DRAM_LOGV(TAG, "illegal error intr clr & ena mask is: 0x%x", CACHE_LL_L1_ILG_EVENT_MASK);
    cache_ll_l1_clear_illegal_error_intr(0, CACHE_LL_L1_ILG_EVENT_MASK);
    cache_ll_l1_enable_illegal_error_intr(0, CACHE_LL_L1_ILG_EVENT_MASK);

    /* Enable the interrupts for cache error. */
    ESP_INTR_ENABLE(ETS_CACHEERR_INUM);
}

int esp_cache_err_get_cpuid(void)
{
    return 0;
}
