/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "ulp_lp_core_memory_shared.h"

#include "sdkconfig.h"
#include "soc/soc.h"
#include "esp_assert.h"

/* The last CONFIG_ULP_SHARED_MEM bytes of the reserved memory are reserved for a shared cfg struct
   The main cpu app and the ulp binary can share variables automatically through the linkerscript generated from
   esp32ulp_mapgen.py, but this is not available when compiling the ULP library.

   For those special cases, e.g. config settings. We can use this shared area.
    */
#define LP_CORE_SHARED_MEM_ADDR (SOC_RTC_DRAM_LOW + CONFIG_ULP_COPROC_RESERVE_MEM  - CONFIG_ULP_SHARED_MEM)

static ulp_lp_core_memory_shared_cfg_t *const s_shared_mem = (ulp_lp_core_memory_shared_cfg_t *)LP_CORE_SHARED_MEM_ADDR;

ESP_STATIC_ASSERT(CONFIG_ULP_SHARED_MEM == sizeof(ulp_lp_core_memory_shared_cfg_t));

ulp_lp_core_memory_shared_cfg_t* ulp_lp_core_memory_shared_cfg_get(void)
{
    return s_shared_mem;
}
