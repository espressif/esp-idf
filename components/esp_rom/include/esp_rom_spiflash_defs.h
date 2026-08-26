/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#pragma once

/**********************************************************
 * Public definations for ROM
 *********************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#define  ESP_ROM_SPIFLASH_BUSY_FLAG           BIT0
#define  ESP_ROM_SPIFLASH_WRENABLE_FLAG       BIT1
#define  ESP_ROM_SPIFLASH_BP0                 BIT2
#define  ESP_ROM_SPIFLASH_BP1                 BIT3
#define  ESP_ROM_SPIFLASH_BP2                 BIT4
#define  ESP_ROM_SPIFLASH_WR_PROTECT          (ESP_ROM_SPIFLASH_BP0|ESP_ROM_SPIFLASH_BP1|ESP_ROM_SPIFLASH_BP2)
#define  ESP_ROM_SPIFLASH_QE                  BIT9
#define  ESP_ROM_SPIFLASH_BP_MASK_ISSI         (BIT7 | BIT5 | BIT4 | BIT3 | BIT2)

#define FLASH_ID_GD25LQ32C  0xC86016

#ifdef __cplusplus
}
#endif
