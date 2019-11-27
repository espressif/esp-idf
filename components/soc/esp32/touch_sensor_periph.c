// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "soc/touch_sensor_periph.h"

/* Store IO number corresponding to the Touch Sensor channel number. */
const int touch_sensor_channel_io_map[SOC_TOUCH_SENSOR_NUM] = {
    TOUCH_PAD_NUM0_GPIO_NUM,
    TOUCH_PAD_NUM1_GPIO_NUM,
    TOUCH_PAD_NUM2_GPIO_NUM,
    TOUCH_PAD_NUM3_GPIO_NUM,
    TOUCH_PAD_NUM4_GPIO_NUM,
    TOUCH_PAD_NUM5_GPIO_NUM,
    TOUCH_PAD_NUM6_GPIO_NUM,
    TOUCH_PAD_NUM7_GPIO_NUM,
    TOUCH_PAD_NUM8_GPIO_NUM,
    TOUCH_PAD_NUM9_GPIO_NUM
};