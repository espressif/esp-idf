// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "rom/spi_flash.h"
#include "soc/spi_reg.h"

static const uint32_t STATUS_QIE_BIT = (1 << 9); /* Quad Enable */

#define SPI_IDX 1
#define OTH_IDX 0

#ifndef BOOTLOADER_BUILD
#define ATTR IRAM_ATTR
#else
#define ATTR
#endif // BOOTLOADER_BUILD

extern SpiFlashChip SPI_flashchip_data;

static void ATTR Wait_SPI_Idle(void)
{
  /* Wait for SPI state machine to be idle */
  while((REG_READ(SPI_EXT2_REG(SPI_IDX)) & SPI_ST)) {
  }
  while(REG_READ(SPI_EXT2_REG(OTH_IDX)) & SPI_ST) {
  }
}

/* Modified version of SPIUnlock() that replaces version in ROM.

   This works around a bug where SPIUnlock sometimes reads the wrong
   high status byte (RDSR2 result) and then copies it back to the
   flash status, which can cause the CMP bit or Status Register
   Protect bit to become set.

   Like other ROM SPI functions, this function is not designed to be
   called directly from an RTOS environment without taking precautions
   about interrupts, CPU coordination, flash mapping. However some of
   the functions in esp_spi_flash.c call it.
 */
SpiFlashOpResult ATTR SPIUnlock(void)
{
  uint32_t status;

  Wait_SPI_Idle();

  if (SPI_read_status_high(&status) != SPI_FLASH_RESULT_OK) {
    return SPI_FLASH_RESULT_ERR;
  }

  /* Clear all bits except QIE, if it is set.
     (This is different from ROM SPIUnlock, which keeps all bits as-is.)
   */
  status &= STATUS_QIE_BIT;

  Wait_SPI_Idle();
  REG_WRITE(SPI_CMD_REG(SPI_IDX), SPI_FLASH_WREN);
  while(REG_READ(SPI_CMD_REG(SPI_IDX)) != 0) {
  }
  Wait_SPI_Idle();

  SET_PERI_REG_MASK(SPI_CTRL_REG(SPI_IDX), SPI_WRSR_2B);
  if (SPI_write_status(&SPI_flashchip_data, status) != SPI_FLASH_RESULT_OK) {
    return SPI_FLASH_RESULT_ERR;
  }

  return SPI_FLASH_RESULT_OK;
}
