/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <string.h>
#include <inttypes.h>

#include "esp_log.h"
#include "unity.h"
#include "esp_flash_chips/spi_flash_chip_driver.h"
#include "test_flash_utils.h"

TEST_CASE("spi_flash BDL test", "[esp_flash]")
{
    const esp_partition_t* part = get_test_flash_partition();
    esp_flash_t* chip = part->flash_chip;
    uint32_t offs = part->address;

    /* chip should be initialised */
    TEST_ASSERT(esp_flash_default_chip != NULL
                && esp_flash_chip_driver_initialized(esp_flash_default_chip));

    if (chip == NULL) {
        chip = esp_flash_default_chip; //use the default chip
    }

    esp_blockdev_handle_t esp_flash_blockdev = NULL;
    TEST_ESP_OK(esp_flash_get_blockdev(chip, &esp_flash_blockdev));
    TEST_ASSERT_NOT_NULL(esp_flash_blockdev);

    uint32_t sector_size = chip->chip_drv->sector_size;
    TEST_ASSERT(sector_size % 4096 == 0);

    const uint32_t target_addr = offs;
    const size_t data_size = sector_size; // a multiple of 4096 (sector size)

    //needs erase before write
    TEST_ESP_OK(esp_flash_blockdev->ops->erase(esp_flash_blockdev, target_addr, data_size));

    //write to the blockdev
    uint8_t* test_data = malloc(data_size);
    memset((void*)test_data, 'A', data_size);
    TEST_ESP_OK(esp_flash_blockdev->ops->write(esp_flash_blockdev, test_data, target_addr, data_size));

    //read from the blockdev the data written before
    uint8_t* data_buffer = malloc(data_size);
    memset((void*)data_buffer, 'X', data_size);
    TEST_ESP_OK(esp_flash_blockdev->ops->read(esp_flash_blockdev, data_buffer, data_size, target_addr, data_size));
    TEST_ASSERT_EQUAL(0, memcmp(test_data, data_buffer, data_size));

    //erase the data from the blockdev and check it's really wiped
    TEST_ESP_OK(esp_flash_blockdev->ops->erase(esp_flash_blockdev, target_addr, data_size));
    TEST_ESP_OK(esp_flash_blockdev->ops->read(esp_flash_blockdev, data_buffer, data_size, target_addr, data_size));
    {
        uint8_t ten_erased[10] = {[0 ... 9] = 0xFF};
        TEST_ASSERT_EQUAL(0, memcmp(ten_erased, data_buffer, 10));
    }
    free(data_buffer);
    free(test_data);

    TEST_ESP_OK(esp_flash_blockdev->ops->release(esp_flash_blockdev));
}
