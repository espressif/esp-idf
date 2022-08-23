/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "spi_flash_mmap.h"

int spi_flash_get_chip_size(void)
{
    return (1024 * 1024 * 1024);
}
