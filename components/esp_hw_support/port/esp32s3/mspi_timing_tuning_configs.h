/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

//FLASH: core clock 160M, module clock 40M, DTR mode
#define MSPI_TIMING_FLASH_CONFIG_TABLE_CORE_CLK_160M_MODULE_CLK_40M_DTR_MODE         {{1, 0, 0}, {0, 0, 0}, {2, 1, 1}, {2, 0, 1}, {2, 2, 2}, {2, 1, 2}, {1, 0, 1}, {0, 0, 1}}
#define MSPI_TIMING_FLASH_CONFIG_NUM_CORE_CLK_160M_MODULE_CLK_40M_DTR_MODE           8
#define MSPI_TIMING_FLASH_DEFAULT_CONFIG_ID_CORE_CLK_160M_MODULE_CLK_40M_DTR_MODE    2

//FLASH: core clock 160M, module clock 80M, DTR mode
#define MSPI_TIMING_FLASH_CONFIG_TABLE_CORE_CLK_160M_MODULE_CLK_80M_DTR_MODE         {{0, 0, 0}, {4, 2, 2}, {2, 1, 2}, {4, 1, 2}, {1, 0, 1}, {4, 0, 2}, {0, 0, 1}, {4, 2, 3}, {2, 1, 3}, {4, 1, 3}, {1, 0, 2}, {4, 0, 3}, {0, 0, 2}, {4, 2, 4}}
#define MSPI_TIMING_FLASH_CONFIG_NUM_CORE_CLK_160M_MODULE_CLK_80M_DTR_MODE           14
#define MSPI_TIMING_FLASH_DEFAULT_CONFIG_ID_CORE_CLK_160M_MODULE_CLK_80M_DTR_MODE    1

//FLASH: core clock 240M, module clock 120M, DTR mode
#define MSPI_TIMING_FLASH_CONFIG_TABLE_CORE_CLK_240M_MODULE_CLK_120M_DTR_MODE        {{0, 0, 0}, {4, 1, 2}, {1, 0, 1}, {4, 0, 2}, {0, 0, 1}, {4, 1, 3}, {1, 0, 2}, {4, 0, 3}, {0, 0, 2}, {4, 1, 4}, {1, 0, 3}, {4, 0, 4}, {0, 0, 3}, {4, 1, 5}}
#define MSPI_TIMING_FLASH_CONFIG_NUM_CORE_CLK_240M_MODULE_CLK_120M_DTR_MODE          14
#define MSPI_TIMING_FLASH_DEFAULT_CONFIG_ID_CORE_CLK_240M_MODULE_CLK_120M_DTR_MODE   1

//FLASH: core clock 160M, module clock 80M, STR mode
#define MSPI_TIMING_FLASH_CONFIG_TABLE_CORE_CLK_160M_MODULE_CLK_80M_STR_MODE         {{1, 0, 0}, {0, 0, 0}, {2, 1, 1}, {2, 0, 1}, {2, 2, 2}, {2, 1, 2}, {1, 0, 1}, {0, 0, 1}}
#define MSPI_TIMING_FLASH_CONFIG_NUM_CORE_CLK_160M_MODULE_CLK_80M_STR_MODE           8
#define MSPI_TIMING_FLASH_DEFAULT_CONFIG_ID_CORE_CLK_160M_MODULE_CLK_80M_STR_MODE    2

//FLASH: core clock 120M, module clock 120M, STR mode
#define MSPI_TIMING_FLASH_CONFIG_TABLE_CORE_CLK_120M_MODULE_CLK_120M_STR_MODE        {{2, 0, 1}, {0, 0, 0}, {2, 2, 2}, {1, 0, 1}, {2, 0, 2}, {0, 0, 1}, {2, 2, 3}, {1, 0, 2}, {2, 0, 3}, {0, 0, 2}, {2, 2, 4}, {1, 0, 3}}
#define MSPI_TIMING_FLASH_CONFIG_NUM_CORE_CLK_120M_MODULE_CLK_120M_STR_MODE          12
#define MSPI_TIMING_FLASH_DEFAULT_CONFIG_ID_CORE_CLK_120M_MODULE_CLK_120M_STR_MODE   2

//FLASH: core clock 240M, module clock 120M, STR mode
#define MSPI_TIMING_FLASH_CONFIG_TABLE_CORE_CLK_240M_MODULE_CLK_120M_STR_MODE        {{1, 0, 0}, {0, 0, 0}, {1, 1, 1}, {2, 3, 2}, {1, 0, 1}, {0, 0, 1}, {1, 1, 2}, {2, 3, 3}, {1, 0, 2}, {0, 0, 2}, {1, 1, 3}, {2, 3, 4}}
#define MSPI_TIMING_FLASH_CONFIG_NUM_CORE_CLK_240M_MODULE_CLK_120M_STR_MODE          12
#define MSPI_TIMING_FLASH_DEFAULT_CONFIG_ID_CORE_CLK_240M_MODULE_CLK_120M_STR_MODE   2

//PSRAM: core clock 80M, module clock 40M, DTR mode
#define MSPI_TIMING_PSRAM_CONFIG_TABLE_CORE_CLK_80M_MODULE_CLK_40M_DTR_MODE          {{1, 0, 0}, {2, 1, 1}, {2, 0, 1}, {0, 0, 0}, {3, 1, 1}, {3, 0, 1}, {1, 0, 1}, {2, 1, 2}, {2, 0, 2}, {0, 0, 1}, {3, 1, 2}, {3, 0, 2}}
#define MSPI_TIMING_PSRAM_CONFIG_NUM_CORE_CLK_80M_MODULE_CLK_40M_DTR_MODE            12
#define MSPI_TIMING_PSRAM_DEFAULT_CONFIG_ID_CORE_CLK_80M_MODULE_CLK_40M_DTR_MODE     4

//PSRAM: core clock 160M, module clock 80M, DTR mode
#define MSPI_TIMING_PSRAM_CONFIG_TABLE_CORE_CLK_160M_MODULE_CLK_80M_DTR_MODE         {{0, 0, 0}, {4, 2, 2}, {2, 1, 2}, {4, 1, 2}, {1, 0, 1}, {4, 0, 2}, {0, 0, 1}, {4, 2, 3}, {2, 1, 3}, {4, 1, 3}, {1, 0, 2}, {4, 0, 3}, {0, 0, 2}, {4, 2, 4}}
#define MSPI_TIMING_PSRAM_CONFIG_NUM_CORE_CLK_160M_MODULE_CLK_80M_DTR_MODE           14
#define MSPI_TIMING_PSRAM_DEFAULT_CONFIG_ID_CORE_CLK_160M_MODULE_CLK_80M_DTR_MODE    1

//PSRAM: core clock 240M, module clock 120M, STR mode
#define MSPI_TIMING_PSRAM_CONFIG_TABLE_CORE_CLK_240M_MODULE_CLK_120M_STR_MODE        {{1, 0, 0}, {0, 0, 0}, {1, 1, 1}, {2, 3, 2}, {1, 0, 1}, {0, 0, 1}, {1, 1, 2}, {2, 3, 3}, {1, 0, 2}, {0, 0, 2}, {1, 1, 3}, {2, 3, 4}}
#define MSPI_TIMING_PSRAM_CONFIG_NUM_CORE_CLK_240M_MODULE_CLK_120M_STR_MODE          12
#define MSPI_TIMING_PSRAM_DEFAULT_CONFIG_ID_CORE_CLK_240M_MODULE_CLK_120M_STR_MODE   2

//PSRAM: core clock 120M, module clock 120M, STR mode
#define MSPI_TIMING_PSRAM_CONFIG_TABLE_CORE_CLK_120M_MODULE_CLK_120M_STR_MODE        {{2, 0, 1}, {0, 0, 0}, {2, 2, 2}, {1, 0, 1}, {2, 0, 2}, {0, 0, 1}, {2, 2, 3}, {1, 0, 2}, {2, 0, 3}, {0, 0, 2}, {2, 2, 4}, {1, 0, 3}}
#define MSPI_TIMING_PSRAM_CONFIG_NUM_CORE_CLK_120M_MODULE_CLK_120M_STR_MODE          12
#define MSPI_TIMING_PSRAM_DEFAULT_CONFIG_ID_CORE_CLK_120M_MODULE_CLK_120M_STR_MODE   2
