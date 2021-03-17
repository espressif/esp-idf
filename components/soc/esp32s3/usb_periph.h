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

#include <stdint.h>
#include <stdbool.h>
#include "soc/usb_pins.h"
#include "soc/gpio_sig_map.h"
#include "soc/usb_reg.h"
#include "soc/usb_types.h"
#include "soc/usb_struct.h"
#include "soc/usb_wrap_reg.h"
#include "soc/usb_wrap_struct.h"

/**
 * @brief A pin descriptor for init
 */
typedef struct {
    const int pin;
    const int func;
    const bool is_output;
    const int ext_phy_only;
} usb_iopin_dsc_t;

extern const usb_iopin_dsc_t usb_periph_iopins[];
