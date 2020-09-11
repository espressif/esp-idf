// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

/*******************************************************************************
 * NOTICE
 * The ll is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The Lowlevel layer for SPI Flash

#pragma once

#include "gpspi_flash_ll.h"
#include "spimem_flash_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

// For esp32s2, spimem is equivalent to traditional spi peripherals found
// in esp32. Let the spi flash clock reg definitions reflect this.
#define SPI_FLASH_LL_CLKREG_VAL_5MHZ   {.spimem=SPIMEM_FLASH_LL_CLKREG_VAL_5MHZ}
#define SPI_FLASH_LL_CLKREG_VAL_10MHZ  {.spimem=SPIMEM_FLASH_LL_CLKREG_VAL_10MHZ}
#define SPI_FLASH_LL_CLKREG_VAL_20MHZ  {.spimem=SPIMEM_FLASH_LL_CLKREG_VAL_20MHZ}
#define SPI_FLASH_LL_CLKREG_VAL_26MHZ  {.spimem=SPIMEM_FLASH_LL_CLKREG_VAL_26MHZ}
#define SPI_FLASH_LL_CLKREG_VAL_40MHZ  {.spimem=SPIMEM_FLASH_LL_CLKREG_VAL_40MHZ}
#define SPI_FLASH_LL_CLKREG_VAL_80MHZ  {.spimem=SPIMEM_FLASH_LL_CLKREG_VAL_80MHZ}

#define spi_flash_ll_get_hw(host_id)  (((host_id)<=SPI1_HOST ? (spi_dev_t*) spimem_flash_ll_get_hw(host_id) \
                                      : gpspi_flash_ll_get_hw(host_id)))

#define spi_flash_ll_hw_get_id(dev) ({int dev_id = spimem_flash_ll_hw_get_id(dev); \
                                     if (dev_id < 0) {\
                                        dev_id = gpspi_flash_ll_hw_get_id(dev);\
                                     }\
                                     dev_id; \
                                    })

typedef union  {
    gpspi_flash_ll_clock_reg_t gpspi;
    spimem_flash_ll_clock_reg_t spimem;
} spi_flash_ll_clock_reg_t;

#ifdef GPSPI_BUILD
#define spi_flash_ll_reset(dev)                              gpspi_flash_ll_reset((spi_dev_t*)dev)
#define spi_flash_ll_cmd_is_done(dev)                        gpspi_flash_ll_cmd_is_done((spi_dev_t*)dev)
#define spi_flash_ll_get_buffer_data(dev, buffer, read_len)  gpspi_flash_ll_get_buffer_data((spi_dev_t*)dev, buffer, read_len)
#define spi_flash_ll_set_buffer_data(dev, buffer, len)       gpspi_flash_ll_set_buffer_data((spi_dev_t*)dev, buffer, len)
#define spi_flash_ll_user_start(dev)                         gpspi_flash_ll_user_start((spi_dev_t*)dev)
#define spi_flash_ll_host_idle(dev)                          gpspi_flash_ll_host_idle((spi_dev_t*)dev)
#define spi_flash_ll_read_phase(dev)                         gpspi_flash_ll_read_phase((spi_dev_t*)dev)
#define spi_flash_ll_set_cs_pin(dev, pin)                    gpspi_flash_ll_set_cs_pin((spi_dev_t*)dev, pin)
#define spi_flash_ll_set_read_mode(dev, read_mode)           gpspi_flash_ll_set_read_mode((spi_dev_t*)dev, read_mode)
#define spi_flash_ll_set_clock(dev, clk)                     gpspi_flash_ll_set_clock((spi_dev_t*)dev, (gpspi_flash_ll_clock_reg_t*)clk)
#define spi_flash_ll_set_miso_bitlen(dev, bitlen)            gpspi_flash_ll_set_miso_bitlen((spi_dev_t*)dev, bitlen)
#define spi_flash_ll_set_mosi_bitlen(dev, bitlen)            gpspi_flash_ll_set_mosi_bitlen((spi_dev_t*)dev, bitlen)
#define spi_flash_ll_set_command8(dev, cmd)                  gpspi_flash_ll_set_command8((spi_dev_t*)dev, cmd)
#define spi_flash_ll_set_addr_bitlen(dev, bitlen)            gpspi_flash_ll_set_addr_bitlen((spi_dev_t*)dev, bitlen)
#define spi_flash_ll_get_addr_bitlen(dev)                    gpspi_flash_ll_get_addr_bitlen((spi_dev_t*)dev)
#define spi_flash_ll_set_address(dev, addr)                  gpspi_flash_ll_set_address((spi_dev_t*)dev, addr)
#define spi_flash_ll_set_usr_address(dev, addr, bitlen)      gpspi_flash_ll_set_usr_address((spi_dev_t*)dev, addr, bitlen)
#define spi_flash_ll_set_dummy(dev, dummy)                   gpspi_flash_ll_set_dummy((spi_dev_t*)dev, dummy)
#define spi_flash_ll_set_dummy_out(dev, en, lev)             gpspi_flash_ll_set_dummy_out((spi_dev_t*)dev, en, lev)
#else
#define spi_flash_ll_reset(dev)                              spimem_flash_ll_reset((spi_mem_dev_t*)dev)
#define spi_flash_ll_cmd_is_done(dev)                        spimem_flash_ll_cmd_is_done((spi_mem_dev_t*)dev)
#define spi_flash_ll_erase_chip(dev)                         spimem_flash_ll_erase_chip((spi_mem_dev_t*)dev)
#define spi_flash_ll_erase_sector(dev)                       spimem_flash_ll_erase_sector((spi_mem_dev_t*)dev)
#define spi_flash_ll_erase_block(dev)                        spimem_flash_ll_erase_block((spi_mem_dev_t*)dev)
#define spi_flash_ll_set_write_protect(dev, wp)              spimem_flash_ll_set_write_protect((spi_mem_dev_t*)dev, wp)
#define spi_flash_ll_get_buffer_data(dev, buffer, read_len)  spimem_flash_ll_get_buffer_data((spi_mem_dev_t*)dev, buffer, read_len)
#define spi_flash_ll_set_buffer_data(dev, buffer, len)       spimem_flash_ll_set_buffer_data((spi_mem_dev_t*)dev, buffer, len)
#define spi_flash_ll_program_page(dev, buffer, len)          spimem_flash_ll_program_page((spi_mem_dev_t*)dev, buffer, len)
#define spi_flash_ll_user_start(dev)                         spimem_flash_ll_user_start((spi_mem_dev_t*)dev)
#define spi_flash_ll_host_idle(dev)                          spimem_flash_ll_host_idle((spi_mem_dev_t*)dev)
#define spi_flash_ll_read_phase(dev)                         spimem_flash_ll_read_phase((spi_mem_dev_t*)dev)
#define spi_flash_ll_set_cs_pin(dev, pin)                    spimem_flash_ll_set_cs_pin((spi_mem_dev_t*)dev, pin)
#define spi_flash_ll_set_read_mode(dev, read_mode)           spimem_flash_ll_set_read_mode((spi_mem_dev_t*)dev, read_mode)
#define spi_flash_ll_set_clock(dev, clk)                     spimem_flash_ll_set_clock((spi_mem_dev_t*)dev, (spimem_flash_ll_clock_reg_t*)clk)
#define spi_flash_ll_set_miso_bitlen(dev, bitlen)            spimem_flash_ll_set_miso_bitlen((spi_mem_dev_t*)dev, bitlen)
#define spi_flash_ll_set_mosi_bitlen(dev, bitlen)            spimem_flash_ll_set_mosi_bitlen((spi_mem_dev_t*)dev, bitlen)
#define spi_flash_ll_set_command8(dev, cmd)                  spimem_flash_ll_set_command8((spi_mem_dev_t*)dev, cmd)
#define spi_flash_ll_set_addr_bitlen(dev, bitlen)            spimem_flash_ll_set_addr_bitlen((spi_mem_dev_t*)dev, bitlen)
#define spi_flash_ll_get_addr_bitlen(dev)                    spimem_flash_ll_get_addr_bitlen((spi_mem_dev_t*) dev)
#define spi_flash_ll_set_address(dev, addr)                  spimem_flash_ll_set_address((spi_mem_dev_t*)dev, addr)
#define spi_flash_ll_set_usr_address(dev, addr, bitlen)      spimem_flash_ll_set_address((spi_mem_dev_t*)dev, addr)
#define spi_flash_ll_set_dummy(dev, dummy)                   spimem_flash_ll_set_dummy((spi_mem_dev_t*)dev, dummy)
#define spi_flash_ll_set_dummy_out(dev, en, lev)             spimem_flash_ll_set_dummy_out((spi_mem_dev_t*)dev, en, lev)
#endif

#ifdef __cplusplus
}
#endif