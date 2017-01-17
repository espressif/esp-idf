// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#pragma once

#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "soc/sdmmc_struct.h"

typedef struct {
    uint32_t sdmmc_status;      ///< masked SDMMC interrupt status
    uint32_t dma_status;        ///< masked DMA interrupt status
} sdmmc_event_t;

void sdmmc_host_reset();

esp_err_t sdmmc_host_start_command(int slot, sdmmc_hw_cmd_t cmd, uint32_t arg);

esp_err_t sdmmc_host_wait_for_event(int tick_count, sdmmc_event_t* out_event);

void sdmmc_host_dma_prepare(sdmmc_desc_t* desc, size_t block_size, size_t data_size);

void sdmmc_host_dma_stop();

void sdmmc_host_dma_resume();

esp_err_t sdmmc_host_transaction_handler_init();

void sdmmc_host_transaction_handler_deinit();

