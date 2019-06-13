// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

#include "soc/rtc_periph.h"

/* Reg,Mux,Fun,IE,Up,Down,Rtc_number */
rtc_gpio_info_t* rtc_gpio_reg[RTC_GPIO_NUMBER] = {
    (rtc_gpio_info_t*)&RTCIO.touch_pad[0].val,
    (rtc_gpio_info_t*)&RTCIO.touch_pad[1].val,
    (rtc_gpio_info_t*)&RTCIO.touch_pad[2].val,
    (rtc_gpio_info_t*)&RTCIO.touch_pad[3].val,
    (rtc_gpio_info_t*)&RTCIO.touch_pad[4].val,
    (rtc_gpio_info_t*)&RTCIO.touch_pad[5].val,
    (rtc_gpio_info_t*)&RTCIO.touch_pad[6].val,
    (rtc_gpio_info_t*)&RTCIO.touch_pad[7].val,
    (rtc_gpio_info_t*)&RTCIO.touch_pad[8].val,
    (rtc_gpio_info_t*)&RTCIO.touch_pad[9].val,
    (rtc_gpio_info_t*)&RTCIO.touch_pad[10].val,
    (rtc_gpio_info_t*)&RTCIO.touch_pad[11].val,
    (rtc_gpio_info_t*)&RTCIO.touch_pad[12].val,
    (rtc_gpio_info_t*)&RTCIO.touch_pad[13].val,
    (rtc_gpio_info_t*)&RTCIO.touch_pad[14].val,
    (rtc_gpio_info_t*)&RTCIO.xtal_32p_pad.val,
    (rtc_gpio_info_t*)&RTCIO.xtal_32n_pad.val,
    (rtc_gpio_info_t*)&RTCIO.pad_dac[0].val,
    (rtc_gpio_info_t*)&RTCIO.pad_dac[1].val,
    (rtc_gpio_info_t*)&RTCIO.rtc_pad19.val,
    (rtc_gpio_info_t*)&RTCIO.rtc_pad20.val,
    (rtc_gpio_info_t*)&RTCIO.rtc_pad21.val
};