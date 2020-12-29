// Copyright 2019-2020 Espressif Systems (Shanghai) PTE LTD
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

#define SOC_TOUCH_SENSOR_NUM                (15)

#define SOC_TOUCH_PAD_MEASURE_WAIT_MAX          (0xFF)
#define SOC_TOUCH_PAD_THRESHOLD_MAX         (0x1FFFFF)

#define SOC_TOUCH_SHIELD_CHANNEL            (14)
#define SOC_TOUCH_DENOISE_CHANNEL           (0)
#define SOC_TOUCH_PROXIMITY_CHANNEL_NUM     (3)
#define SOC_TOUCH_PAD_WAKE_SUPPORTED        (1)     /*!<Supports waking up from touch pad trigger */

#ifdef __cplusplus
}
#endif
