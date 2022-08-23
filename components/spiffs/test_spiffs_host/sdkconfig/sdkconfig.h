/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#define CONFIG_IDF_TARGET_ESP32 1
#define CONFIG_SPIFFS_USE_MAGIC_LENGTH 1
#define CONFIG_SPIFFS_MAX_PARTITIONS 3
#define CONFIG_SPIFFS_OBJ_NAME_LEN 32
#define CONFIG_SPIFFS_PAGE_SIZE 256
#define CONFIG_SPIFFS_GC_MAX_RUNS 10
#define CONFIG_SPIFFS_CACHE_WR 1
#define CONFIG_SPIFFS_CACHE 1
#define CONFIG_SPIFFS_META_LENGTH 4
#define CONFIG_SPIFFS_USE_MAGIC 1
#define CONFIG_SPIFFS_PAGE_CHECK 1
#define CONFIG_SPIFFS_USE_MTIME 1

#define CONFIG_WL_SECTOR_SIZE 4096

// for log component with linux target
#define CONFIG_LOG_DEFAULT_LEVEL 3
#define CONFIG_LOG_MAXIMUM_LEVEL 3
#define CONFIG_LOG_TIMESTAMP_SOURCE_RTOS 1

#define CONFIG_PARTITION_TABLE_OFFSET 0x8000

#define CONFIG_ESPTOOLPY_FLASHSIZE "8MB"
//currently use the legacy implementation, since the stubs for new HAL are not done yet
#define CONFIG_SPI_FLASH_USE_LEGACY_IMPL 1
#define CONFIG_MMU_PAGE_SIZE 0X10000 // 64KB

#undef _Static_assert
#define _Static_assert(cond, message)
