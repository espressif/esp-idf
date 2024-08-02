/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "ulp_lp_core_memory_shared.h"

#include "sdkconfig.h"
#include "soc/soc.h"
#include "esp_rom_caps.h"
#include "esp_assert.h"

#define ALIGN_DOWN(SIZE, AL)   (SIZE & ~(AL - 1))

/* The last CONFIG_ULP_SHARED_MEM bytes of the reserved memory are reserved for a shared cfg struct
   The main cpu app and the ulp binary can share variables automatically through the linkerscript generated from
   esp32ulp_mapgen.py, but this is not available when compiling the ULP library.

   For those special cases, e.g. config settings. We can use this shared area.
    */

#if IS_ULP_COCPU
static ulp_lp_core_memory_shared_cfg_t __attribute__((section(".shared_mem"))) s_shared_mem = {};
ESP_STATIC_ASSERT(CONFIG_ULP_SHARED_MEM == sizeof(ulp_lp_core_memory_shared_cfg_t));
#endif

ulp_lp_core_memory_shared_cfg_t* ulp_lp_core_memory_shared_cfg_get(void)
{
#if IS_ULP_COCPU
    return &s_shared_mem;
#else
#if ESP_ROM_HAS_LP_ROM
    extern uint32_t _rtc_ulp_memory_start;
    uint32_t ulp_base_addr = (uint32_t)&_rtc_ulp_memory_start;
#else
    uint32_t ulp_base_addr = SOC_RTC_DRAM_LOW;
#endif
    /* Ensure the end where the shared memory starts is aligned to 8 bytes
    if updating this also update the same in ulp_lp_core_riscv.ld
    */
    return (ulp_lp_core_memory_shared_cfg_t *)(ulp_base_addr + ALIGN_DOWN(CONFIG_ULP_COPROC_RESERVE_MEM, 0x8)  - CONFIG_ULP_SHARED_MEM);
#endif
}
