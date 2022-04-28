/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _WL_Config_H_
#define _WL_Config_H_

#include "Flash_Access.h"

/**
* @brief This class is used as a structure to configure wear levelling module
*
*/

#if defined(_MSC_VER)
#define ALIGNED_(x) __declspec(align(x))
#else
#if defined(__GNUC__)
#define ALIGNED_(x) __attribute__ ((aligned(x)))
#endif
#endif

typedef struct ALIGNED_(16) WL_Config_s { /*!< Size of wl_config_t structure should be divided by 16 for encryption*/
    size_t   start_addr;    /*!< start address in the flash*/
    uint32_t full_mem_size; /*!< Amount of memory used to store data in bytes*/
    uint32_t page_size;     /*!< One page size in bytes. Page could be more then memory block. This parameter must be page_size >= N*block_size.*/
    uint32_t sector_size;   /*!< size of flash memory sector that will be erased and stored at once (erase)*/
    uint32_t updaterate;    /*!< Amount of accesses before block will be moved*/
    uint32_t wr_size;       /*!< Minimum amount of bytes per one block at write operation: 1...*/
    uint32_t version;       /*!< A version of current implementation. To erase and reallocate complete memory this ID must be different from id before.*/
    size_t   temp_buff_size;  /*!< Size of temporary allocated buffer to copy from one flash area to another. The best way, if this value will be equal to sector size.*/
    uint32_t crc;           /*!< CRC for this config*/
} wl_config_t;

#ifndef _MSC_VER // MSVS has different format for this define
static_assert(sizeof(wl_config_t) % 16 == 0, "Size of wl_config_t structure should be compatible with flash encryption");
#endif // _MSC_VER

#endif // _WL_Config_H_
