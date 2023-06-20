/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
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

#define FLASH_BASE_PATH "/spiflash"
#define SD_BASE_PATH "/sdcard"
extern wl_handle_t s_wl_handle;

void spiflash_speed_test_raw_run(void);
void spiflash_speed_test_fs_run(void);
void sdcard_speed_test_raw_run(sdmmc_card_t *card);
void sdcard_speed_test_fatfs_run(void);
