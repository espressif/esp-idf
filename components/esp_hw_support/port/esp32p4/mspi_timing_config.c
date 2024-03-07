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
#include "esp_private/periph_ctrl.h"
#include "esp_private/mspi_timing_config.h"
#include "mspi_timing_tuning_configs.h"
#include "hal/psram_ctrlr_ll.h"

const static char *TAG = "MSPI Timing";

//-------------------------------------MSPI Clock Setting-------------------------------------//
void mspi_timing_config_set_psram_clock(uint32_t psram_freq_mhz, mspi_timing_speed_mode_t speed_mode, bool control_both_mspi)
{
    uint32_t freqdiv = MSPI_TIMING_MPLL_FREQ_MHZ / MSPI_TIMING_CORE_CLOCK_DIV / psram_freq_mhz;
    assert(freqdiv > 0);
    ESP_EARLY_LOGD(TAG, "psram_freq_mhz: %" PRIu32 " mhz, bus clock div: %" PRIu32, psram_freq_mhz, freqdiv);
    PERIPH_RCC_ATOMIC() {
    	//MSPI2 and MSPI3 share the register for core clock. So we only set MSPI2 here.
        psram_ctrlr_ll_set_core_clock(PSRAM_CTRLR_LL_MSPI_ID_2, MSPI_TIMING_CORE_CLOCK_DIV);
        psram_ctrlr_ll_set_bus_clock(PSRAM_CTRLR_LL_MSPI_ID_3, freqdiv);
        psram_ctrlr_ll_set_bus_clock(PSRAM_CTRLR_LL_MSPI_ID_2, freqdiv);
    }
}

void mspi_timing_config_set_flash_clock(uint32_t flash_freq_mhz, mspi_timing_speed_mode_t speed_mode, bool control_both_mspi)
{
    //For compatibility
}
