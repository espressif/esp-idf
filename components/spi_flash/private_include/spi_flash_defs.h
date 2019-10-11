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

/* SPI commands (actual on-wire commands not SPI controller bitmasks)
   Suitable for use with spi_flash_hal_common_command static function.
*/
#define CMD_RDID       0x9F
#define CMD_WRSR       0x01
#define  SR_WIP        (1<<0) /* Status register write-in-progress bit */
#define  SR_WREN       (1<<1) /* Status register write enable bit */
#define CMD_WRSR2      0x31 /* Not all SPI flash uses this command */
#define CMD_WREN       0x06
#define CMD_WRDI       0x04
#define CMD_RDSR       0x05
#define CMD_RDSR2      0x35 /* Not all SPI flash uses this command */

#define CMD_FASTRD_QIO  0xEB
#define CMD_FASTRD_QUAD 0x6B
#define CMD_FASTRD_DIO  0xBB
#define CMD_FASTRD_DUAL 0x3B
#define CMD_FASTRD      0x0B
#define CMD_READ        0x03 /* Speed limited */

#define CMD_CHIP_ERASE 0xC7
#define CMD_SECTOR_ERASE 0x20
#define CMD_LARGE_BLOCK_ERASE 0xD8 /* 64KB block erase command */

#define CMD_RST_EN      0x66
#define CMD_RST_DEV     0x99


