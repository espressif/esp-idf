// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _SOC_SDMMC_PERIPH_H_
#define _SOC_SDMMC_PERIPH_H_

#include <stdint.h>
//include soc related (generated) definitions
#include "soc/sdmmc_pins.h"
#include "soc/sdmmc_reg.h"
#include "soc/sdmmc_struct.h"
#include "soc/gpio_sig_map.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t clk_gpio;
    uint8_t cmd_gpio;
    uint8_t d0_gpio;
    uint8_t d1_gpio;
    uint8_t d2_gpio;
    uint8_t d3_gpio;
    uint8_t d4_gpio;
    uint8_t d5_gpio;
    uint8_t d6_gpio;
    uint8_t d7_gpio;
    uint8_t card_detect;
    uint8_t write_protect;
    uint8_t card_int;
    uint8_t width;
} sdmmc_slot_info_t;

/** pin and signal information of each slot */
extern const sdmmc_slot_info_t sdmmc_slot_info[];

#ifdef __cplusplus
}
#endif

#endif /* _SOC_SDMMC_PERIPH_H_ */