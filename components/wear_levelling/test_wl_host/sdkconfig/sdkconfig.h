/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#define CONFIG_IDF_TARGET_ESP32 1
#define CONFIG_WL_SECTOR_SIZE 4096
#define CONFIG_LOG_DEFAULT_LEVEL 3
#define CONFIG_PARTITION_TABLE_OFFSET 0x8000
#define CONFIG_ESPTOOLPY_FLASHSIZE "8MB"
//currently use the legacy implementation, since the stubs for new HAL are not done yet
#define CONFIG_SPI_FLASH_USE_LEGACY_IMPL 1
#define CONFIG_MMU_PAGE_SIZE 0X10000 // 64KB
