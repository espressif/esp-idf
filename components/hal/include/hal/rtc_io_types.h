// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

/** RTCIO output/input mode type. */
typedef enum {
    RTC_GPIO_MODE_INPUT_ONLY ,  /*!< Pad input */
    RTC_GPIO_MODE_OUTPUT_ONLY,  /*!< Pad output */
    RTC_GPIO_MODE_INPUT_OUTPUT, /*!< Pad input + output */
    RTC_GPIO_MODE_DISABLED,     /*!< Pad (output + input) disable */
    RTC_GPIO_MODE_OUTPUT_OD,    /*!< Pad open-drain output */
    RTC_GPIO_MODE_INPUT_OUTPUT_OD, /*!< Pad input + open-drain output */
} rtc_gpio_mode_t;
