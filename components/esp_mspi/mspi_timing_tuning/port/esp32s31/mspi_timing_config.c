/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_types.h"
#include "esp_log.h"
#include "soc/soc_caps.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/mspi_timing_config.h"
#include "mspi_timing_tuning_configs.h"
#include "hal/psram_ctrlr_ll.h"
#include "hal/mspi_ll.h"
#include "hal/clk_tree_ll.h"

ESP_LOG_ATTR_TAG(TAG, "MSPI Timing");

//-------------------------------------MSPI Clock Setting-------------------------------------//
void mspi_timing_config_set_psram_clock(uint32_t psram_freq_mhz, mspi_timing_speed_mode_t speed_mode, bool control_both_mspi)
{
    uint32_t freqdiv = MSPI_TIMING_MPLL_FREQ_MHZ / MSPI_TIMING_CORE_CLOCK_DIV / psram_freq_mhz;
    assert(freqdiv > 0);
    ESP_DRAM_LOGD(TAG, "psram_freq_mhz: %" PRIu32 " mhz, bus clock div: %" PRIu32, psram_freq_mhz, freqdiv);
    PERIPH_RCC_ATOMIC() {
        //MSPI2 and MSPI3 share the register for core clock. So we only set MSPI2 here.
        psram_ctrlr_ll_enable_core_clock(PSRAM_CTRLR_LL_MSPI_ID_2, true);
        psram_ctrlr_ll_set_core_clock_div(PSRAM_CTRLR_LL_MSPI_ID_2, MSPI_TIMING_CORE_CLOCK_DIV);
        psram_ctrlr_ll_set_bus_clock(PSRAM_CTRLR_LL_MSPI_ID_3, freqdiv);
        psram_ctrlr_ll_set_bus_clock(PSRAM_CTRLR_LL_MSPI_ID_2, freqdiv);
    }
}

void mspi_timing_config_set_flash_clock(uint32_t flash_freq_mhz, mspi_timing_speed_mode_t speed_mode, bool control_both_mspi)
{
#if MSPI_TIMING_FLASH_NEEDS_TUNING
    // In ESP32S31 bootloader, the flash clock source is set to CPLL by default for a 80MHz flash frequency.
    // When timing tuning is needed, it means the flash frequency needs to be raised to 120MHz, so we need to set the flash clock source to BBPLL.
    // esp_clk_tree_initialize() is called later, so we cannot call esp_clk_tree_enable_src() to enable BBPLL here.
    // Instead, we will enable BBPLL by LL directly. ref count will still be incremented by esp_clk_tree_initialize() later.
    if (mspi_timing_ll_get_flash_clk_src(MSPI_TIMING_LL_MSPI_ID_0) != FLASH_CLK_SRC_BBPLL) {
        clk_ll_bbpll_enable();
        _mspi_timing_ll_set_flash_clk_src(MSPI_TIMING_LL_MSPI_ID_0, FLASH_CLK_SRC_BBPLL);
    }

    uint32_t core_clock_mhz = MSPI_TIMING_BBPLL_FREQ_MHZ / MSPI_TIMING_LL_HP_FLASH_CORE_CLK_DIV;
    assert(core_clock_mhz == 120);
    uint32_t freqdiv = core_clock_mhz / flash_freq_mhz;

    PERIPH_RCC_ATOMIC() {
        //core clock shared among SPI0 / SPI1
        mspi_timing_ll_set_flash_core_clock(MSPI_TIMING_LL_MSPI_ID_0, core_clock_mhz);
    }

    mspi_timing_ll_set_flash_clock(MSPI_TIMING_LL_MSPI_ID_0, freqdiv);
    if (control_both_mspi) {
        mspi_timing_ll_set_flash_clock(MSPI_TIMING_LL_MSPI_ID_1, freqdiv);
    }

    mspi_timing_ll_mask_invalid_dqs(MSPI_TIMING_LL_MSPI_ID_0, true);
    mspi_timing_ll_mask_invalid_dqs(MSPI_TIMING_LL_MSPI_ID_1, true);
#endif
}
