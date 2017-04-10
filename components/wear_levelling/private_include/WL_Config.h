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

#ifndef _WL_Config_H_
#define _WL_Config_H_

#include "Flash_Access.h"

/**
* @brief This class is used as a structure to configure wear levelling module
*
*/
typedef struct WL_Config_s {
    size_t start_addr;      /*!< start address in the flash*/
    uint32_t full_mem_size; /*!< Amount of memory used to store data in bytes*/
    uint32_t page_size;     /*!< One page size in bytes. Page could be more then memory block. This parameter must be page_size >= N*block_size.*/
    uint32_t sector_size;   /*!< size of flash memory sector that will be erased and stored at once (erase)*/
    uint32_t updaterate;    /*!< Amount of accesses before block will be moved*/
    uint32_t wr_size;       /*!< Minimum amount of bytes per one block at write operation: 1...*/
    uint32_t version;       /*!< A version of current implementatioon. To erase and reallocate complete memory this ID must be different from id before.*/
    size_t   temp_buff_size;  /*!< Size of temporary allocated buffer to copy from one flash area to another. The best way, if this value will be equal to sector size.*/
    uint32_t crc;           /*!< CRC for this config*/

} wl_config_t;

#endif // _WL_Config_H_