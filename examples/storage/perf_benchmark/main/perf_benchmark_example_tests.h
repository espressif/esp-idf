/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include "driver/sdmmc_types.h"
#include "wear_levelling.h"

#include <stddef.h>

#define FLASH_BASE_PATH "/spiflash"
#define SD_BASE_PATH "/sdcard"
extern wl_handle_t s_wl_handle;

void spiflash_speed_test_raw_run(size_t rounds);
void spiflash_speed_test_spiffs_run(size_t rounds);
void spiflash_speed_test_fatfs_run(size_t rounds);
void spiflash_speed_test_littlefs_run(size_t rounds);
void sdcard_speed_test_raw_run(sdmmc_card_t *card, size_t rounds);
void sdcard_speed_test_fatfs_run(size_t rounds);
void sdcard_speed_test_littlefs_run(size_t rounds);
