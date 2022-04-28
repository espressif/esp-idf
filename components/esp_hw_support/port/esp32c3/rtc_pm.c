/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <assert.h>
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/syscon_reg.h"

typedef enum {
    PM_LIGHT_SLEEP = BIT(2),        /*!< WiFi PD, memory in light sleep */
} pm_sleep_mode_t;

typedef enum {
    PM_SW_NOREJECT = 0,
    PM_SW_REJECT   = 1
} pm_sw_reject_t;


/* These MAC-related functions are defined in the closed source part of
 * RTC library
 */
extern void pm_mac_init(void);
extern int pm_check_mac_idle(void);
extern void pm_mac_deinit(void);

/* This sleep-related function is called from the closed source part of RTC
 * library.
 */
pm_sw_reject_t pm_set_sleep_mode(pm_sleep_mode_t sleep_mode, void(*pmac_save_params)(void))
{
    (void) pmac_save_params; /* unused */

    pm_mac_deinit();
    if (pm_check_mac_idle()) {
        pm_mac_init();
        return PM_SW_REJECT;
    }

    rtc_sleep_config_t cfg = { 0 };

    switch (sleep_mode) {
    case PM_LIGHT_SLEEP:
        cfg.wifi_pd_en = 1;
        cfg.dig_dbias_wak = 4;
        cfg.dig_dbias_slp = 0;
        cfg.rtc_dbias_wak = 0;
        cfg.rtc_dbias_slp = 0;
        rtc_sleep_init(cfg);
        break;

    default:
        assert(0 && "unsupported sleep mode");
    }
    return PM_SW_NOREJECT;
}
