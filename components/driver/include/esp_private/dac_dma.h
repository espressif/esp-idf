/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_intr_alloc.h"


#ifdef __cplusplus
extern "C" {
#endif

esp_err_t dac_dma_periph_init(int chan_num, uint32_t freq_hz, bool is_alternate);

esp_err_t dac_dma_periph_deinit(void);

esp_err_t dac_dma_periph_register_intr(intr_handler_t intr_handler_func, void *user_ctx);

esp_err_t dac_dma_periph_deregister_intr(void);

void dac_dma_periph_enable(void);

void dac_dma_periph_disable(void);

bool dac_dma_periph_intr_is_triggered(void);

uint32_t dac_dma_periph_intr_get_eof_desc(void);

void dac_dma_periph_dma_trans_start(uint32_t desc_addr);

#ifdef __cplusplus
}
#endif
