// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _DISKIO_SPIFLASH_DEFINED
#define _DISKIO_SPIFLASH_DEFINED

#ifdef __cplusplus
extern "C" {
#endif

#include "integer.h"
#include "wear_levelling.h"


/**
 * Register spi flash partition 
 *
 * @param pdrv  drive number
 * @param flash_handle  handle of the wear levelling partition.
 */
esp_err_t ff_diskio_register_wl_partition(BYTE pdrv, wl_handle_t flash_handle);
BYTE ff_diskio_get_pdrv_wl(wl_handle_t flash_handle);

#ifdef __cplusplus
}
#endif

#endif // _DISKIO_SPIFLASH_DEFINED
