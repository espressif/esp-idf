// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdint.h>
#include <assert.h>
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"

typedef enum {
    PM_LIGHT_SLEEP = BIT(2),        /*!< WiFi PD, memory in light sleep */
} pm_sleep_mode_t;

typedef enum{
    PM_SW_NOREJECT = 0,
    PM_SW_REJECT   = 1
} pm_sw_reject_t;


/* These MAC-related functions are defined in the closed source part of
 * RTC library
 */
extern void pm_mac_init();
extern int pm_check_mac_idle();
extern void pm_mac_deinit();

/* This sleep-related function is called from the closed source part of RTC
 * library.
 */
pm_sw_reject_t pm_set_sleep_mode(pm_sleep_mode_t sleep_mode, void(*pmac_save_params)())
{
    (void) pmac_save_params; /* unused */

    pm_mac_deinit();
    if (pm_check_mac_idle()) {
        pm_mac_init();
        return PM_SW_REJECT;
    }

    rtc_sleep_config_t cfg = { 0 };
    cfg.soc_clk_sel = RTC_CNTL_SOC_CLK_SEL_XTL;

    switch (sleep_mode) {
        case PM_LIGHT_SLEEP:
            cfg.wifi_pd_en = 1;
            cfg.dig_dbias_wak = 4;
            cfg.dig_dbias_slp = 0;
            cfg.rtc_dbias_wak = 0;
            cfg.rtc_dbias_slp = 0;
            cfg.lslp_meminf_pd = 1;
            rtc_sleep_init(cfg);
            break;

        default:
            assert(0 && "unsupported sleep mode");
    }
    return PM_SW_NOREJECT;
}
