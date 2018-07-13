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

#ifndef _SOC_SDIO_SLAVE_PERIPH_H_
#define _SOC_SDIO_SLAVE_PERIPH_H_

#include <stdint.h>
//include soc related (generated) definitions
#include "soc/sdio_slave_pins.h"
#include "soc/slc_reg.h"
#include "soc/slc_struct.h"
#include "soc/host_reg.h"
#include "soc/host_struct.h"
#include "soc/hinf_reg.h"
#include "soc/hinf_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/** pin and signal information of each slot */
typedef struct {
    uint32_t clk_gpio;
    uint32_t cmd_gpio;
    uint32_t d0_gpio;
    uint32_t d1_gpio;
    uint32_t d2_gpio;
    uint32_t d3_gpio;
    int      func;
} sdio_slave_slot_info_t;

extern const sdio_slave_slot_info_t sdio_slave_slot_info[];

#ifdef __cplusplus
}
#endif

#endif /* _SOC_SDIO_SLAVE_PERIPH_H_ */