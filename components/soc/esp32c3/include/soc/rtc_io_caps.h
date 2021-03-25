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

/* No dedicated RTCIO subsystem on ESP32-C3. RTC functions are still supported
 * for hold, wake & 32kHz crystal functions - via rtc_cntl_reg */
#define SOC_RTCIO_PIN_COUNT    0

#define RTCIO_LL_PIN_FUNC     0
