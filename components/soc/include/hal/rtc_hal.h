// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#pragma once

#include "hal/gpio_types.h"
#include "hal/rtc_io_ll.h"
#include "hal/rtc_cntl_ll.h"

#define rtc_hal_ext1_get_wakeup_pins()                    rtc_cntl_ll_ext1_get_wakeup_pins()

#define rtc_hal_ext1_set_wakeup_pins(mask, mode)          rtc_cntl_ll_ext1_set_wakeup_pins(mask, mode)

#define rtc_hal_ext1_clear_wakeup_pins()                  rtc_cntl_ll_ext1_clear_wakeup_pins()

#define rtc_hal_set_wakeup_timer(ticks)                   rtc_cntl_ll_set_wakeup_timer(ticks)

/*
 * Enable wakeup from ULP coprocessor.
 */
#define rtc_hal_ulp_wakeup_enable()                       rtc_cntl_ll_ulp_wakeup_enable()

