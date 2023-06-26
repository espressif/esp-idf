/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "hal/lp_aon_ll.h"

#define rtc_hal_ext1_get_wakeup_status()                    lp_aon_hal_ext1_get_wakeup_status()
#define rtc_hal_ext1_clear_wakeup_status()                  lp_aon_hal_ext1_clear_wakeup_status()
#define rtc_hal_ext1_set_wakeup_pins(mask, mode)            lp_aon_hal_ext1_set_wakeup_pins(mask, mode)
#define rtc_hal_ext1_clear_wakeup_pins()                    lp_aon_hal_ext1_clear_wakeup_pins()
#define rtc_hal_ext1_get_wakeup_pins()                      lp_aon_hal_ext1_get_wakeup_pins()


#define lp_aon_hal_ext1_get_wakeup_status()                 lp_aon_ll_ext1_get_wakeup_status()
#define lp_aon_hal_ext1_clear_wakeup_status()               lp_aon_ll_ext1_clear_wakeup_status()
#define lp_aon_hal_ext1_set_wakeup_pins(mask, mode)         lp_aon_ll_ext1_set_wakeup_pins(mask, mode)
#define lp_aon_hal_ext1_clear_wakeup_pins()                 lp_aon_ll_ext1_clear_wakeup_pins()
#define lp_aon_hal_ext1_get_wakeup_pins()                   lp_aon_ll_ext1_get_wakeup_pins()

#define lp_aon_hal_inform_wakeup_type(dslp)                 lp_aon_ll_inform_wakeup_type(dslp)
