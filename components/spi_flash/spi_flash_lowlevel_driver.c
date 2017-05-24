// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
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
#include <stdlib.h>
#include "spi_flash_lowlevel_driver.h"

extern const esp_flash_driver_t esp_flash_generic_chip_driver;
extern const esp_flash_driver_t esp_flash_issi_chip_driver;

/* Default registered chip drivers.

   Note these are tested in order and first match is taken, so generic/catchall entries
   should go last.

   Note that the esp_flash_registered_flash_ops pointer can be
   changed to point to a different array of registered ops, if desired.
*/
static const esp_flash_driver_t *default_registered_flash_drivers[] = {
    &esp_flash_issi_chip_driver,
    &esp_flash_generic_chip_driver,
    NULL,
};

const esp_flash_driver_t **esp_flash_registered_flash_drivers = default_registered_flash_drivers;
