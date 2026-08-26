/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include "esp_attr.h"
#include "sdkconfig.h"
#include "esp_rom_spiflash.h"

#if CONFIG_SPI_FLASH_BROWNOUT_RESET

static bool flash_brownout_needs_reset = false;
static bool flash_erasing = false;

// This function could be called in startup
void spi_flash_needs_reset_check(void)
{
    // Currently only XMC is suggested to reset when brownout
#if CONFIG_SPI_FLASH_BROWNOUT_RESET_XMC
    if ((g_rom_flashchip.device_id >> 16) == 0x20) {
        flash_brownout_needs_reset = true;
    }
#endif
}

void spi_flash_set_erasing_flag(bool status)
{
    flash_erasing = status;
}

bool spi_flash_brownout_need_reset(void)
{
    return (flash_brownout_needs_reset && flash_erasing);
}

#endif //CONFIG_SPI_FLASH_BROWNOUT_RESET
