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

#ifdef __cplusplus
extern "C" {
#endif

#define SOC_TOUCH_SENSOR_NUM            (10)
#define SOC_TOUCH_SENSOR_BIT_MASK_MAX   (0x3ff)

#define SOC_TOUCH_PAD_MEASURE_WAIT      (0xFF)  /*!<The timer frequency is 8Mhz, the max value is 0xff */
#define SOC_TOUCH_PAD_THRESHOLD_MAX     (0)     /*!<If set touch threshold max value, The touch sensor can't be in touched status */

#ifdef __cplusplus
}
#endif
