/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SPI_Flash_H_
#define _SPI_Flash_H_

#include "esp_err.h"
#include "Flash_Access.h"

/**
* @brief This class is used to access SPI flash devices. Class implements Flash_Access interface
*
*/
class SPI_Flash : public Flash_Access
{

public:
    SPI_Flash();

    size_t get_flash_size() override;
    esp_err_t erase_sector(size_t sector) override;
    esp_err_t erase_range(size_t start_address, size_t size) override;
    esp_err_t write(size_t dest_addr, const void *src, size_t size) override;
    esp_err_t read(size_t src_addr, void *dest, size_t size) override;
    size_t get_sector_size() override;
    ~SPI_Flash() override;
};

#endif // _SPI_Flash_H_
