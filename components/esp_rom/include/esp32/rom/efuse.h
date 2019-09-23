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

#ifndef _ROM_EFUSE_H_
#define _ROM_EFUSE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup efuse_APIs efuse APIs
  * @brief     ESP32 efuse read/write APIs
  * @attention
  *
  */

/** @addtogroup efuse_APIs
  * @{
  */

/**
  * @brief  Do a efuse read operation, to update the efuse value to efuse read registers.
  *
  * @param  null
  *
  * @return null
  */
void ets_efuse_read_op(void);

/**
  * @brief  Do a efuse write operation, to update efuse write registers to efuse, then you need call ets_efuse_read_op again.
  *
  * @param  null
  *
  * @return null
  */
void ets_efuse_program_op(void);

/**
  * @brief  Read 8M Analog Clock value(8 bit) in efuse, the analog clock will not change with temperature.
  *         It can be used to test the external xtal frequency, do not touch this efuse field.
  *
  * @param  null
  *
  * @return u32: 1 for 100KHZ, range is 0 to 255.
  */
uint32_t ets_efuse_get_8M_clock(void);

/**
  * @brief  Read spi flash pin configuration from Efuse
  *
  * @return
  * - 0 for default SPI pins.
  * - 1 for default HSPI pins.
  * - Other values define a custom pin configuration mask. Pins are encoded as per the EFUSE_SPICONFIG_RET_SPICLK,
  *   EFUSE_SPICONFIG_RET_SPIQ, EFUSE_SPICONFIG_RET_SPID, EFUSE_SPICONFIG_RET_SPICS0, EFUSE_SPICONFIG_RET_SPIHD macros.
  *   WP pin (for quad I/O modes) is not saved in efuse and not returned by this function.
  */
uint32_t ets_efuse_get_spiconfig(void);

#define EFUSE_SPICONFIG_SPI_DEFAULTS 0
#define EFUSE_SPICONFIG_HSPI_DEFAULTS 1

#define EFUSE_SPICONFIG_RET_SPICLK_MASK         0x3f
#define EFUSE_SPICONFIG_RET_SPICLK_SHIFT        0
#define EFUSE_SPICONFIG_RET_SPICLK(ret)         (((ret) >> EFUSE_SPICONFIG_RET_SPICLK_SHIFT) & EFUSE_SPICONFIG_RET_SPICLK_MASK)

#define EFUSE_SPICONFIG_RET_SPIQ_MASK           0x3f
#define EFUSE_SPICONFIG_RET_SPIQ_SHIFT          6
#define EFUSE_SPICONFIG_RET_SPIQ(ret)           (((ret) >> EFUSE_SPICONFIG_RET_SPIQ_SHIFT) & EFUSE_SPICONFIG_RET_SPIQ_MASK)

#define EFUSE_SPICONFIG_RET_SPID_MASK           0x3f
#define EFUSE_SPICONFIG_RET_SPID_SHIFT          12
#define EFUSE_SPICONFIG_RET_SPID(ret)           (((ret) >> EFUSE_SPICONFIG_RET_SPID_SHIFT) & EFUSE_SPICONFIG_RET_SPID_MASK)

#define EFUSE_SPICONFIG_RET_SPICS0_MASK         0x3f
#define EFUSE_SPICONFIG_RET_SPICS0_SHIFT        18
#define EFUSE_SPICONFIG_RET_SPICS0(ret)         (((ret) >> EFUSE_SPICONFIG_RET_SPICS0_SHIFT) & EFUSE_SPICONFIG_RET_SPICS0_MASK)


#define EFUSE_SPICONFIG_RET_SPIHD_MASK          0x3f
#define EFUSE_SPICONFIG_RET_SPIHD_SHIFT         24
#define EFUSE_SPICONFIG_RET_SPIHD(ret)          (((ret) >> EFUSE_SPICONFIG_RET_SPIHD_SHIFT) & EFUSE_SPICONFIG_RET_SPIHD_MASK)

/**
  * @brief  A crc8 algorithm used in efuse check.
  *
  * @param  unsigned char const *p : Pointer to original data.
  *
  * @param  unsigned int len : Data length in byte.
  *
  * @return unsigned char: Crc value.
  */
unsigned char esp_crc8(unsigned char const *p, unsigned int len);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* _ROM_EFUSE_H_ */
