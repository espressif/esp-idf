// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _SOC_GPIO_PERIPH_H
#define _SOC_GPIO_PERIPH_H
#include "stdint.h"
#include "soc/gpio_pins.h"
#include "soc/io_mux_reg.h"
#ifdef __cplusplus
extern "C"
{
#endif

extern const uint32_t GPIO_PIN_MUX_REG[GPIO_PIN_COUNT];

#ifdef __cplusplus
}
#endif

#endif // _SOC_GPIO_PERIPH_H
