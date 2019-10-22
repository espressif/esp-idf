// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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
#include "esp_flash.h"
#include "spi_flash_chip_driver.h"


/**
 * GD (GigaDevice) SPI flash chip_drv, uses all the above functions for its operations. In
 * default autodetection, this is used as a catchall if a more specific chip_drv
 * is not found.
 *
 * Note that this is for GD chips with product ID 40H (GD25Q) and 60H (GD25LQ). The chip diver uses
 * different commands to write the SR2 register according to the chip ID. For GD25Q40 - GD25Q16
 * chips, and GD25LQ chips, WRSR (01H) command is used; while WRSR2 (31H) is used for GD25Q32 -
 * GD25Q127 chips.
 */
extern const spi_flash_chip_t esp_flash_chip_gd;
