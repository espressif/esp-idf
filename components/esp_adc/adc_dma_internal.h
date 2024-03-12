/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_intr_alloc.h"
#include "hal/dma_types.h"
#include "hal/adc_hal.h"
#include "adc_continuous_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ADC DMA context
 */
typedef struct {
#if SOC_GDMA_SUPPORTED
    gdma_channel_handle_t   gdma_chan;
#elif CONFIG_IDF_TARGET_ESP32S2
//On ESP32S2, there is no gdma, so use SPI DMA to transmit data
    spi_dma_ctx_t           *spi_dma_ctx;
    spi_dev_t               *adc_spi_dev;
#elif CONFIG_IDF_TARGET_ESP32
//On ESP32, there is no gdma, so use I2S DMA to transmit data
    i2s_dev_t               *adc_i2s_dev;
#endif
    intr_handle_t           dma_intr_hdl;
} adc_dma_t;

#ifdef __cplusplus
}
#endif
