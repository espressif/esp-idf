/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BDL_Access_H_
#define _BDL_Access_H_

#include "esp_err.h"
#include "Flash_Access.h"
#include "esp_blockdev.h"

/**
 * @brief Flash_Access adapter that delegates all I/O to an esp_blockdev_handle_t.
 *
 * Used exclusively by the BDL wear-levelling path so that the core WL_Flash
 * engine can operate on any block device without knowing about esp_partition.
 */
class BDL_Access : public Flash_Access
{
public:
    explicit BDL_Access(esp_blockdev_handle_t bdl_device);

    size_t get_flash_size() override;

    esp_err_t erase_sector(size_t sector) override;
    esp_err_t erase_range(size_t start_address, size_t size) override;

    esp_err_t write(size_t dest_addr, const void *src, size_t size) override;
    esp_err_t read(size_t src_addr, void *dest, size_t size) override;

    size_t get_sector_size() override;
    bool is_readonly() override;

    ~BDL_Access() override;

private:
    esp_blockdev_handle_t bdl_device;
};

#endif // _BDL_Access_H_
