// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "bootloader_common.h"
#include "sdkconfig.h"
#include "soc/efuse_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/io_mux_reg.h"
#include "esp32s2beta/rom/efuse.h"
#include "esp32s2beta/rom/gpio.h"
#include "esp32s2beta/rom/spi_flash.h"

#define FLASH_CLK_IO SPI_CLK_GPIO_NUM
#define FLASH_CS_IO SPI_CS0_GPIO_NUM
#define FLASH_SPIQ_IO SPI_Q_GPIO_NUM
#define FLASH_SPID_IO SPI_D_GPIO_NUM
#define FLASH_SPIWP_IO SPI_WP_GPIO_NUM
#define FLASH_SPIHD_IO SPI_HD_GPIO_NUM

void bootloader_configure_spi_pins(int drv)
{
    const uint32_t spiconfig = ets_efuse_get_spiconfig();
    if (spiconfig == EFUSE_SPICONFIG_SPI_DEFAULTS) {
        gpio_matrix_out(FLASH_CS_IO, SPICS0_OUT_IDX, 0, 0);
        gpio_matrix_out(FLASH_SPIQ_IO, SPIQ_OUT_IDX, 0, 0);
        gpio_matrix_in(FLASH_SPIQ_IO, SPIQ_IN_IDX, 0);
        gpio_matrix_out(FLASH_SPID_IO, SPID_OUT_IDX, 0, 0);
        gpio_matrix_in(FLASH_SPID_IO, SPID_IN_IDX, 0);
        gpio_matrix_out(FLASH_SPIWP_IO, SPIWP_OUT_IDX, 0, 0);
        gpio_matrix_in(FLASH_SPIWP_IO, SPIWP_IN_IDX, 0);
        gpio_matrix_out(FLASH_SPIHD_IO, SPIHD_OUT_IDX, 0, 0);
        gpio_matrix_in(FLASH_SPIHD_IO, SPIHD_IN_IDX, 0);
        //select pin function gpio
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_SPIHD_U, PIN_FUNC_GPIO);
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_SPIWP_U, PIN_FUNC_GPIO);
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_SPICS0_U, PIN_FUNC_GPIO);
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_SPIQ_U, PIN_FUNC_GPIO);
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_SPID_U, PIN_FUNC_GPIO);
        // flash clock signal should come from IO MUX.
        // set drive ability for clock
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_SPICLK_U, FUNC_SPICLK_SPICLK);
        SET_PERI_REG_BITS(PERIPHS_IO_MUX_SPICLK_U, FUN_DRV, drv, FUN_DRV_S);

#if CONFIG_SPIRAM_TYPE_ESPPSRAM32 || CONFIG_SPIRAM_TYPE_ESPPSRAM64
        uint32_t flash_id = g_rom_flashchip.device_id;
        if (flash_id == FLASH_ID_GD25LQ32C) {
            // Set drive ability for 1.8v flash in 80Mhz.
            SET_PERI_REG_BITS(PERIPHS_IO_MUX_SPIHD_U, FUN_DRV, 3, FUN_DRV_S);
            SET_PERI_REG_BITS(PERIPHS_IO_MUX_SPIWP_U, FUN_DRV, 3, FUN_DRV_S);
            SET_PERI_REG_BITS(PERIPHS_IO_MUX_SPICS0_U, FUN_DRV, 3, FUN_DRV_S);
            SET_PERI_REG_BITS(PERIPHS_IO_MUX_SPICLK_U, FUN_DRV, 3, FUN_DRV_S);
            SET_PERI_REG_BITS(PERIPHS_IO_MUX_SPIQ_U, FUN_DRV, 3, FUN_DRV_S);
            SET_PERI_REG_BITS(PERIPHS_IO_MUX_SPID_U, FUN_DRV, 3, FUN_DRV_S);
        }
#endif
    }
}
