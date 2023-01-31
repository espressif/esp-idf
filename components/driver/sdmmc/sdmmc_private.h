/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "soc/sdmmc_periph.h"

typedef struct {
    uint32_t sdmmc_status;      ///< masked SDMMC interrupt status
    uint32_t dma_status;        ///< masked DMA interrupt status
} sdmmc_event_t;

void sdmmc_host_reset(void);

esp_err_t sdmmc_host_start_command(int slot, sdmmc_hw_cmd_t cmd, uint32_t arg);

esp_err_t sdmmc_host_wait_for_event(int tick_count, sdmmc_event_t* out_event);

void sdmmc_host_dma_prepare(sdmmc_desc_t* desc, size_t block_size, size_t data_size);

void sdmmc_host_dma_stop(void);

void sdmmc_host_dma_resume(void);

bool sdmmc_host_card_busy(void);

esp_err_t sdmmc_host_transaction_handler_init(void);

void sdmmc_host_transaction_handler_deinit(void);
