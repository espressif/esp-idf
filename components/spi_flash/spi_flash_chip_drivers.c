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

#include <stdlib.h>
#include "spi_flash_chip_driver.h"
#include "spi_flash_chip_generic.h"
#include "spi_flash_chip_issi.h"
#include "spi_flash_chip_gd.h"
#include "sdkconfig.h"

/*
 * Default registered chip drivers. Note these are tested in order and first
 * match is taken, so generic/catchall entries should go last. Note that the
 * esp_flash_registered_flash_ops pointer can be changed to point to a different
 * array of registered ops, if desired.
 *
 * It can be configured to support only available chips in the sdkconfig, to
 * avoid possible issues, and speed up the auto-detecting.
 */
static const spi_flash_chip_t *default_registered_chips[] = {
#ifdef CONFIG_SPI_FLASH_SUPPORT_ISSI_CHIP
    &esp_flash_chip_issi,
#endif
#ifdef CONFIG_SPI_FLASH_SUPPORT_GD_CHIP
    &esp_flash_chip_gd,
#endif
    &esp_flash_chip_generic,
    NULL,
};

const spi_flash_chip_t **esp_flash_registered_chips = default_registered_chips;
