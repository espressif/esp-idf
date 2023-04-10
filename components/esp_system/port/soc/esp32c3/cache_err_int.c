/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
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
    esprv_intc_int_set_type(ETS_CACHEERR_INUM, INTR_TYPE_LEVEL);
    esprv_intc_int_set_priority(ETS_CACHEERR_INUM, SOC_INTERRUPT_LEVEL_MEDIUM);

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
