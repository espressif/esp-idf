/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _Flash_Access_H_
#define _Flash_Access_H_
#include "esp_err.h"

/**
* @brief Universal flash access interface class
*
*/
class Flash_Access
{
public:
    virtual size_t get_flash_size() = 0;

    virtual esp_err_t erase_sector(size_t sector) = 0;
    virtual esp_err_t erase_range(size_t start_address, size_t size) = 0;

    virtual esp_err_t write(size_t dest_addr, const void *src, size_t size) = 0;
    virtual esp_err_t read(size_t src_addr, void *dest, size_t size) = 0;

    virtual size_t get_sector_size() = 0;

    virtual esp_err_t flush()
    {
        return ESP_OK;
    };

    virtual ~Flash_Access() {};
};

#endif // _Flash_Access_H_
