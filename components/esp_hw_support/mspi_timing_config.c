/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
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
#include "mspi_timing_config.h"
#if SOC_MEMSPI_TIMING_TUNING_BY_MSPI_DELAY
#include "mspi_timing_tuning_configs.h"
#include "hal/mspi_timing_tuning_ll.h"
#endif


#if SOC_MEMSPI_CORE_CLK_SHARED_WITH_PSRAM
#define FLASH_LOW_SPEED_CORE_CLOCK_MHZ     MSPI_TIMING_LL_CORE_CLOCK_MHZ_DEFAULT
#define FLASH_HIGH_SPEED_CORE_CLOCK_MHZ    MSPI_TIMING_CORE_CLOCK_MHZ
#define PSRAM_LOW_SPEED_CORE_CLOCK_MHZ     MSPI_TIMING_LL_CORE_CLOCK_MHZ_DEFAULT
#define PSRAM_HIGH_SPEED_CORE_CLOCK_MHZ    MSPI_TIMING_CORE_CLOCK_MHZ
#endif  //SOC_MEMSPI_CORE_CLK_SHARED_WITH_PSRAM


#if SOC_SPI_MEM_SUPPORT_TIMING_TUNING
/**
 * Currently we only need these on chips with timing tuning
 */
//-------------------------------------MSPI Clock Setting-------------------------------------//
static void s_mspi_flash_set_core_clock(uint8_t spi_num, uint32_t core_clock_mhz)
{
    mspi_timing_ll_set_core_clock(spi_num, core_clock_mhz);
}

static void s_mspi_psram_set_core_clock(uint8_t spi_num, uint32_t core_clock_mhz)
{
    mspi_timing_ll_set_core_clock(spi_num, core_clock_mhz);
}

void mspi_timing_config_set_flash_clock(uint32_t flash_freq_mhz, mspi_timing_speed_mode_t speed_mode, bool control_both_mspi)
{
    uint32_t core_clock_mhz = 0;
    if (speed_mode == MSPI_TIMING_SPEED_MODE_LOW_PERF) {
        core_clock_mhz = FLASH_LOW_SPEED_CORE_CLOCK_MHZ;
    } else {
        core_clock_mhz = FLASH_HIGH_SPEED_CORE_CLOCK_MHZ;
    }
    //SPI0 and SPI1 share the register for core clock. So we only set SPI0 here.
    s_mspi_flash_set_core_clock(0, core_clock_mhz);

    uint32_t freqdiv = core_clock_mhz / flash_freq_mhz;
    assert(freqdiv > 0);
    mspi_timing_ll_set_flash_clock(0, freqdiv);
    if (control_both_mspi) {
        mspi_timing_ll_set_flash_clock(1, freqdiv);
    }
}

void mspi_timing_config_set_psram_clock(uint32_t psram_freq_mhz, mspi_timing_speed_mode_t speed_mode, bool control_both_mspi)
{
    (void)control_both_mspi;  // for compatibility
    uint32_t core_clock_mhz = 0;
    if (speed_mode == MSPI_TIMING_SPEED_MODE_LOW_PERF) {
        core_clock_mhz = PSRAM_LOW_SPEED_CORE_CLOCK_MHZ;
    } else {
        core_clock_mhz = PSRAM_HIGH_SPEED_CORE_CLOCK_MHZ;
    }
    //SPI0 and SPI1 share the register for core clock. So we only set SPI0 here.
    s_mspi_psram_set_core_clock(0, core_clock_mhz);

    uint32_t freqdiv = core_clock_mhz / psram_freq_mhz;
    assert(freqdiv > 0);
    mspi_timing_ll_set_psram_clock(0, freqdiv);
}
#endif  //#if SOC_SPI_MEM_SUPPORT_TIMING_TUNING
