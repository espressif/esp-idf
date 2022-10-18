// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Enable Quad I/O mode in bootloader (if configured)
 *
 * Queries attached SPI flash ID and sends correct SPI flash
 * commands to enable QIO or QOUT mode, then enables this mode.
 */
void bootloader_enable_qio_mode(void);

/**
 * @brief Read flash ID by sending 0x9F command
 * @return flash raw ID
 *     mfg_id = (ID >> 16) & 0xFF;
       flash_id = ID & 0xffff;
 */
uint32_t bootloader_read_flash_id(void);

#ifdef __cplusplus
}
#endif
