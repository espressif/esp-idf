/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "nvs_bootloader.h"

// logs the result of the call to the nvs_bootloader_read function
void log_nvs_bootloader_read_list(const esp_err_t return_code, const nvs_bootloader_read_list_t read_list[], const size_t read_list_count);

void log_nvs_bootloader_read_list_OK(const nvs_bootloader_read_list_t read_list[], const size_t read_list_count);
void log_nvs_bootloader_read_list_INVALID_ARG(const nvs_bootloader_read_list_t read_list[], const size_t read_list_count);
void log_nvs_bootloader_read_list_NOT_FINISHED(const nvs_bootloader_read_list_t read_list[], const size_t read_list_count);
