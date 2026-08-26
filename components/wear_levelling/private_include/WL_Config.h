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

typedef struct ALIGNED_(16) WL_Config_s {/*!< Size of wl_config_t structure should be divided by 16 for encryption*/
    size_t   wl_partition_start_addr;    /*!< Partition start address in the flash*/
    uint32_t wl_partition_size;          /*!< Size of partition used to store data in bytes*/
    uint32_t wl_page_size;               /*!< One page size in bytes. Page size can be more than memory block. Here, wl_page_size >= N*flash_sector_size, where N > 0.*/
    uint32_t flash_sector_size;          /*!< Size of flash memory sector that will be erased and stored at once (erase)*/
    uint32_t wl_update_rate;             /*!< Memory erase count after which physical sector and dummy sector swaps its address.*/
    uint32_t wl_pos_update_record_size;  /*!< Number of bytes for storing pos update record appended on the state sector data after every wl_update_rate*/
    uint32_t version;                    /*!< A version of current implementation. To erase and reallocate complete memory this ID must be different from id before.*/
    size_t   wl_temp_buff_size;          /*!< Size of temporary allocated buffer to copy from one flash area to another. The best way, if this value will be equal to sector size.*/
    uint32_t crc32;                      /*!< CRC for this config*/
} wl_config_t;

#ifndef _MSC_VER // MSVS has different format for this define
static_assert(sizeof(wl_config_t) % 16 == 0, "Size of wl_config_t structure should be compatible with flash encryption");
#endif // _MSC_VER

#endif // _WL_Config_H_
