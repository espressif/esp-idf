/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _Partition_H_
#define _Partition_H_

#include "esp_err.h"

#include "Flash_Access.h"
#include "esp_partition.h"
#include "spi_flash_mmap.h"  // for SPI_FLASH_SEC_SIZE

/**
* @brief This class is used to access partition. Class implements Flash_Access interface
*
*/
class Partition : public Flash_Access
{

public:
    Partition(const esp_partition_t *partition);

    virtual size_t get_flash_size();

    virtual esp_err_t erase_sector(size_t sector);
    virtual esp_err_t erase_range(size_t start_address, size_t size);

    virtual esp_err_t write(size_t dest_addr, const void *src, size_t size);
    virtual esp_err_t read(size_t src_addr, void *dest, size_t size);

    virtual size_t get_sector_size();
    virtual bool is_readonly();

    virtual ~Partition();
protected:
    const esp_partition_t *partition;

};

#endif // _Partition_H_
