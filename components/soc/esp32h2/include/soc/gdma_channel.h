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

// The following macros have a format SOC_[periph][instance_id] to make it work with `GDMA_MAKE_TRIGGER`
#define SOC_GDMA_TRIG_PERIPH_M2M0    (-1)
#define SOC_GDMA_TRIG_PERIPH_SPI2    (0)
#define SOC_GDMA_TRIG_PERIPH_UART0   (2)
#define SOC_GDMA_TRIG_PERIPH_I2S0    (3)
#define SOC_GDMA_TRIG_PERIPH_AES0    (6)
#define SOC_GDMA_TRIG_PERIPH_SHA0    (7)
#define SOC_GDMA_TRIG_PERIPH_ADC0    (8)
