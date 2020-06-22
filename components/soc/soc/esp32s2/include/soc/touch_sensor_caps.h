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

#define SOC_TOUCH_SENSOR_NUM                (15)    /*! 15 Touch channels */
#define SOC_TOUCH_SENSOR_BIT_MASK_MAX       (0x7fff)/*! 15 Touch channels */

#define SOC_TOUCH_PAD_MEASURE_WAIT          (0xFF)  /*!<The timer frequency is 8Mhz, the max value is 0xff */
#define SOC_TOUCH_PAD_THRESHOLD_MAX         (0x1FFFFF)  /*!<If set touch threshold max value, The touch sensor can't be in touched status */

#define SOC_TOUCH_SHIELD_CHANNEL            (14) /*!< The waterproof function includes a shielded channel (TOUCH_PAD_NUM14) */
#define SOC_TOUCH_DENOISE_CHANNEL           (0)  /*!< T0 is an internal channel that does not have a corresponding external GPIO. 
                                                    T0 will work simultaneously with the measured channel Tn. Finally, the actual 
                                                    measured value of Tn is the value after subtracting lower bits of T0. */
#define SOC_TOUCH_PROXIMITY_CHANNEL_NUM     (3)  /* Sopport touch proximity channel number. */

#ifdef __cplusplus
}
#endif
