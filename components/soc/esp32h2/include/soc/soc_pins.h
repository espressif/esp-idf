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

/*
 * Pin definition header file. The long term plan is to have a single soc_pins.h for all
 * peripherals. Now we temporarily separate these information into periph_pins/channels.h for each
 * peripheral and include them here to avoid developing conflicts in those header files.
 */

#pragma once

#include "soc/gpio_pins.h"
#include "soc/spi_pins.h"
