/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
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

static const char *TAG = "CACHE_ERR";

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

        /* On the hardware side, stat by clearing all the bits reponsible for
         * enabling cache access error interrupts.  */
        ESP_DRAM_LOGV(TAG, "core 0 access error intr clr & ena mask is: 0x%x", CACHE_LL_L1_ACCESS_EVENT_MASK);
        cache_ll_l1_clear_access_error_intr(0, CACHE_LL_L1_ACCESS_EVENT_MASK);
        cache_ll_l1_enable_access_error_intr(0, CACHE_LL_L1_ACCESS_EVENT_MASK);
    } else {
        esp_rom_route_intr_matrix(core_id, ETS_CACHE_CORE1_ACS_INTR_SOURCE, ETS_CACHEERR_INUM);

        /* On the hardware side, stat by clearing all the bits reponsible for
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
