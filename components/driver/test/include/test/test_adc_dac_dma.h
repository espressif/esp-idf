/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/* Common header for ESP32-S2 ADC/DAC tests */

/**
 * SPI DMA type.
 */
typedef enum {
    DMA_ONLY_ADC_INLINK = BIT(1),   /*!<Select ADC-DMA config. */
    DMA_ONLY_DAC_OUTLINK = BIT(2),  /*!<Select DAC-DMA config. */
    DMA_BOTH_ADC_DAC,               /*!<Select DAC-DMA and ADC-DMA config. */
#define DMA_BOTH_ADC_DAC (DMA_ONLY_ADC_INLINK | DMA_ONLY_DAC_OUTLINK)
} spi_dma_link_type_t;

/**
 * Register SPI-DMA interrupt handler.
 *
 * @param handler       Handler.
 * @param handler_arg   Handler parameter.
 * @param intr_mask     DMA interrupt type mask.
 */
esp_err_t adc_dac_dma_isr_register(intr_handler_t handler, void* handler_arg, uint32_t intr_mask);

/**
 * Deregister SPI-DMA interrupt handler.
 *
 * @param handler       Handler.
 * @param handler_arg   Handler parameter.
 */
esp_err_t adc_dac_dma_isr_deregister(intr_handler_t handler, void* handler_arg);

/**
 * Reset DMA linker pointer and start DMA.
 *
 * @param type     DMA linker type. See ``spi_dma_link_type_t``.
 * @param dma_addr DMA linker addr.
 * @param int_msk  DMA interrupt type mask.
 */
void adc_dac_dma_linker_start(spi_dma_link_type_t type, void *dma_addr, uint32_t int_msk);

/**
 * Stop DMA.
 *
 * @param type DMA linker type. See ``spi_dma_link_type_t``.
 */
void adc_dac_dma_linker_stop(spi_dma_link_type_t type);

/**
 * Deinit SPI3 DMA. Disable interrupt, stop DMA trans.
 */
void adc_dac_dma_linker_deinit(void);
