/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "hal/sdmmc_periph.h"
#include "esp_private/sd_host_private.h"

esp_err_t sdmmc_host_reset(void);

esp_err_t sdmmc_host_start_command(int slot, sdmmc_hw_cmd_t cmd, uint32_t arg);

esp_err_t sdmmc_host_wait_for_event(int tick_count, sd_host_sdmmc_event_t* out_event);

void sdmmc_host_dma_stop(void);

void sdmmc_host_dma_resume(void);

bool sdmmc_host_card_busy(void);

void sdmmc_host_enable_clk_cmd11(int slot, bool enable);

esp_err_t sdmmc_host_transaction_handler_init(void);

void sdmmc_host_transaction_handler_deinit(void);

//get slot handle, for legacy driver compatibility
sd_host_slot_handle_t sdmmc_get_slot_handle(int slot_id);
